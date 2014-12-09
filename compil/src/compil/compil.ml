open Core.Std
open Descr

module Dep :
sig
  type t = int*int*string (* pos, width, name *)
  include Comparable.S with type t := t
end =
  struct
    module T =
      struct
	type t = int*int*string with sexp, compare
      end
    include T
    include Comparable.Make(T)
  end

module SSet = String.Set
module DSet = Dep.Set
module SMap = String.Map

type free = Piece.t SMap.t with sexp
type deps = DSet.t SMap.t with sexp

type dep_graph = (Piece.t SMap.t * DSet.t SMap.t)  with sexp(* free (with base width), deps *)

type compil_settings = { min_width : int
		       ; min_height : int (* May not be useful *)
		       }


let fail f = ksprintf (fun s -> Failure s |> raise) f (* Used like printf *)

(* Builds a graph on the set of elements with vertices between e1 and e2 if e1 depends on e2 *)
let make_dep_graph garment : dep_graph =
  let add_dep name pos w = function
    | None -> Some (DSet.singleton (pos, w, name))
    | Some deps -> Some (DSet.add deps (pos, w, name)) in
  let rec depends curr_name curr_width acc = function
    | Split l ->
       List.fold ~init:acc
		 ~f:(fun acc' (_, w, elt) -> depends curr_name w acc' elt) l
    | Trapezoid (t, e) -> depends curr_name t.upper_width acc e
    | Link (target_name, pos) ->
       let (free, deps) = acc in
       (SMap.remove free target_name,
	SMap.change deps target_name (add_dep curr_name pos curr_width))
  in
  let all = garment.elements in
  SMap.fold garment.elements
    ~init:(all, SMap.empty)
    ~f:(fun ~key:name ~data:(w,elt) acc -> depends name w acc elt)

let check_trapezoid trapezoid : unit = ()(* Echoue en utilisant fail *)

(* Checks for collisions in split/links + basic errors (negative lengths etc.) *)
let sanity_check settings garment (deps:deps) : unit =
  let rec aux curr_name curr_width = function
    | Split l ->
       let f intervals (pos, w, _) =
	 if w < settings.min_width then
	   fail "Narrow branch in split in piece \"%s\"." curr_name;
	 if pos + w < settings.min_width
	 || pos > curr_width - settings.min_width then
	   fail "Stray branch in split in piece \"%s\"." curr_name;
	 Interval.Int.create pos (pos + w) :: intervals
       in
       let intervals =
	 List.fold l ~init:[] ~f:f
       in
       if not (Interval.Int.are_disjoint intervals) then
	 fail "Branch collision in split in piece \"%s\"." curr_name;
       List.iter l ~f:(fun (_, w, e) -> aux curr_name w e)
    | Trapezoid (t, e) ->
       if t.upper_width < settings.min_width then
	 fail "Narrow trapezoid in piece \"%s\"." curr_name;
       if t.height < settings.min_height then
	 fail "Flat trapezoid in piece \"%s\"." curr_name;
       check_trapezoid t;
    (* <0 should be checked during parsing *)
    (* Pattern constraints go here *)
       aux curr_name t.upper_width e
    | Link (n, _) ->
      if not (SMap.mem garment.elements n) then
        fail "Undefined link : \"%s\" in piece \"%s\"." n curr_name
  in
  SMap.iter garment.elements
	    ~f:(fun ~key:name ~data:(w,elt) -> aux name w elt);
  (* We now check for collisions in links *)
  let dep_check ~key:curr_name ~data:(curr_width, curr_deps) =
    let f intervals (pos, w, source) =
      if w < settings.min_width then
        fail "Narrow join from piece \"%s\" to piece \"%s\"." source curr_name;
      if pos + w < settings.min_width
      || pos > curr_width - settings.min_width then
	fail "Stray branch in split in piece \"%s\"." curr_name;
      Interval.Int.create pos (pos + w) :: intervals
    in
    let intervals =
      DSet.fold curr_deps ~init:[] ~f:f
    in
    if not (Interval.Int.are_disjoint intervals) then
      fail "Branch collision in split in piece \"%s\"." curr_name
  in
  SMap.iter deps ~f:dep_check

let x = 2
