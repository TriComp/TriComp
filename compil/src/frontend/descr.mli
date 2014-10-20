(* High-level specification of a knitted garment *)

type key = string

module StringMap : Map.S 
  with type key = key (* C'est super lisible ça ... *)

(* Atoms*)

type pattern (* Description of a minimal pattern, maybe we can start with Endroit | Envers |... (knit/purl stitches?) *)

type trapezoid = { height : int
                 ; shift : int
                 ; lower_width : int
                 ; upper_width : int
                 ; pattern : pattern
                 }

type join_slot = Left | Right

(* Elements *)

(* Note : how do we want to specify width/height? On each piece? On trapezoids? *)
type element = Bottom of key * element
             | Trapezoid of trapezoid * element
             | Split of element * int * element (* Left, gap width, right *)
             | Link of key * join_slot
             | Stop

(* Garment *)

type garment = { pieces : element StringMap.t
               ; stuff : unit (* Placeholder *)
               }
