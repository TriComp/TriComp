open Core.Std

(* High-level specification of a knitted garment *)

type name = string

(* Atoms*)

type pattern = string(* Description of a minimal pattern, maybe we can start with Endroit | Envers |... (knit/purl stitches?) *)

type trapezoid = { height : int
                 ; shift : int
                 ; lower_width : int
                 ; upper_width : int
                 ; pattern : pattern
                 }

type join_slot = Left | Right

(* Elements *)

(* Note : how do we want to specify width/height? On each piece? On trapezoids? *)
type element = (* Bottom of element *)
             | Trapezoid of trapezoid * element
             | Split of element * int * element (* Left, gap width, right *)
             | Link of name * join_slot
             | Stop

(* Garment *)

type garment = { elements : element String.Map.t
               ; name : string
               ; descr : string
               }

(* Constructeurs *)

let print_trapezoid t =
  sprintf "trapezoid ( height : %d, shift : %d, upper_width : %d, lower_width : %d, pattern : %s )"
    t.height t.shift t.upper_width t.lower_width t.pattern

let tab s shift = (String.make shift ' ')^s

let rec print_block padding = function
  | Stop | Link _ as e -> sprintf "{ %s }" (print_element padding e)
  | e -> sprintf "{ %s\n%s}" (print_element padding e) padding

and print_element padding = function
    Stop -> sprintf "stop"
  | Link (s,Left) -> sprintf "link left %s" s
  | Link (s,Right) -> sprintf "link right %s" s
  | Trapezoid (t,e) ->
    sprintf "%s\n%s|| %s" (print_trapezoid t) padding (print_element padding e)
  | Split (e1,x,e2) ->
    let head = sprintf "split %d" x in
    let new_padding = tab padding (String.length head) in
    sprintf "%s %s%s" head (print_block new_padding e1) (print_block new_padding e2)

let print_elements ~key:name ~data:element s =
  let head = sprintf "piece %s " name in
  let padding = tab "" (String.length head) in
  sprintf "%s\n\n%s:= start\n%s|| %s" s head padding (print_element padding element)

let print_garment g =
  sprintf "Name : %s\nDescription : \"%s\"%s\n%!" g.name g.descr (String.Map.fold g.elements ~init:"" ~f:print_elements)
