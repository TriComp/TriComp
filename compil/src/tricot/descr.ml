open Core.Std

(* High-level specification of a knitted garment *)

type name = string with sexp, compare

(* Atoms*)

type pattern = string with sexp, compare(* Description of a minimal pattern, maybe we can start with Endroit | Envers |... (knit/purl stitches?) *)

type trapezoid = { height : int
                 ; shift : int
                 ; upper_width : int
                 ; pattern : pattern
                 } with sexp, compare

(* Elements *)

(* Note : how do we want to specify width/height? On each piece? On trapezoids? *)
type element = Trapezoid of trapezoid * element
             | Split of (int*int*element) list (* pos, width, element *)
             | Link of name * int with sexp, compare

(* Garment *)

type garment = { elements : (int*element) String.Map.t
               ; name : string
               ; descr : string
               }

(* Constructeurs *)

let print_trapezoid t =
  sprintf "trapezoid ( height : %d, shift : %d, upper_width : %d, pattern : %s )"
    t.height t.shift t.upper_width t.pattern

let tab s shift = (String.make shift ' ')^s

let rec print_block padding = function
    Split [] | Link _ as e -> sprintf "{ %s }" (print_element padding e)
  | e -> sprintf "{ %s\n%s}" (print_element padding e) padding

and print_element padding = function
    Split [] -> sprintf "stop"
  | Link (n, pos) -> sprintf "link %s %d" n pos
  | Trapezoid (t,e) ->
    sprintf "%s\n%s|| %s" (print_trapezoid t) padding (print_element padding e)
  | Split l ->
    let head = sprintf "split " in
    let new_padding = tab padding (String.length head) in
    List.fold l ~init:head
                ~f:(fun acc (pos,w,e) -> sprintf "%s %d %d %s" acc pos w (print_block new_padding e)) (***)

let print_elements ~key:name ~data:(w,element) s =
  let head = sprintf "piece %s " name in
  let padding = tab "" (String.length head) in
  sprintf "%s\n\n%s:= start %d\n%s|| %s" s head w padding (print_element padding element)

let print_garment g =
  sprintf "Name : %s\nDescription : \"%s\"%s\n%!" g.name g.descr (String.Map.fold g.elements ~init:"" ~f:print_elements)


module Element : sig
  type t = element
  include Comparable.S with type t := t
end = struct
  module T = struct
    type t = element with sexp, compare
  end
  include T
  include Comparable.Make(T)
end
