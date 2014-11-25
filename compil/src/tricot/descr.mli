open Core.Std

(* High-level specification of a knitted garment *)

type name = string

(* Atoms*)

type pattern = string(* Description of a minimal pattern, maybe we can start with Endroit | Envers |... (knit/purl stitches?) *)

type trapezoid = { height : int
                 ; shift : int
                 ; upper_width : int
                 ; pattern : pattern
                 }

(* Elements *)

(* Note : how do we want to specify width/height? On each piece? On trapezoids? *)

type element = Trapezoid of trapezoid * element
             | Split of (int*int*element) list (* pos, width, element *)
             | Link of name * int

module Element : sig
  type t = element
  include Comparable.S with type t := t
end

(* Garment *)

type garment = { elements : (int*element) String.Map.t
               ; name : string
               ; descr : string
               }

val print_garment : garment -> string
