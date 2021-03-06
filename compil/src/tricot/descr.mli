open Core.Std

(* High-level specification of a knitted garment *)

type name = string

(* Atoms*)

type pattern = string * bool Array.t Array.t with sexp, compare(* Description of a minimal pattern *)

val get_dims : pattern -> int * int

type trapezoid = { height : int
                 ; shift : int
                 ; upper_width : int
                 ; pattern : pattern
                 }

val trad_trapezoid : width:int -> trapezoid -> string

(* Elements *)

(* Note : how do we want to specify width/height? On each piece? On trapezoids? *)

type element = Trapezoid of trapezoid * element
             | Split of (int*int*element) list (* pos, width, element *)
             | Link of name * int

module Piece : sig
  type t = int*element with sexp
  include Comparable.S with type t := t
end

(* Garment *)

type garment = { elements : Piece.t String.Map.t
               ; name : string
               ; descr : string
               }

val print_garment : garment -> string
