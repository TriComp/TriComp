open Core.Std
open Descr

type basic_block_graph =
  { preds : (Interval.Int.t * string) list
  ; base_width : int
  ; body : trapezoid list
  ; top_width : int
  ; successors : (Interval.Int.t * basic_block_graph) list
  }

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

module DSet = Dep.Set
module SMap = String.Map

type free = Piece.t SMap.t with sexp
type deps = DSet.t SMap.t with sexp

type dep_graph = (Piece.t SMap.t * DSet.t SMap.t)  with sexp (* free (with base width), deps *)

type compil_settings = { min_width : int
		       ; min_height : int (* May not be useful *)
		       }


let fail f = ksprintf (fun s -> Failure s |> raise) f (* Used like printf *)

(* Builds a graph on the set of elements with vertices between e1 and e2 if e1 depends on e2 *)
let make_dep_graph garment : dep_graph =
  let add_dep name pos w = function
    | None -> Some (DSet.singleton (pos, w, name))
    | Some deps -> Some (DSet.add deps (pos, w, name)) in
  let do_if_unseen name f (seen, acc) e =
    match SMap.find seen name with
    | None -> f (SMap.add ~key:name ~data:false seen, acc) e
    | Some false -> fail "Cycle involving piece \"%s\"." name
    | Some true -> (seen, acc) in
  let rec depends curr_name first curr_width (seen, acc) = function
    | Split l ->
      let (seen_res, acc_res) = List.fold ~init:(seen, acc)
	  ~f:(fun acc' (_, w, elt) -> depends curr_name false w acc' elt) l in
      if first then
        let seen' = SMap.add ~key:curr_name ~data:true (SMap.remove seen curr_name) in
        (seen', acc_res)
      else
        (seen_res, acc_res)
    | Trapezoid (t, e) -> depends curr_name first t.upper_width (seen, acc) e
    | Link (target_name, pos) ->
      let (free, deps) = acc in
      match SMap.find garment.elements target_name with
      | None -> fail "Undefined link : \"%s\" in piece \"%s\"." target_name curr_name
      | Some (w, e) -> do_if_unseen target_name (depends target_name true w) (seen,(SMap.remove free target_name,
	SMap.change deps target_name (add_dep curr_name pos curr_width))) e
  in
  let all = garment.elements in
  let (_, res) =
    SMap.fold garment.elements
      ~init:(SMap.empty,(all, SMap.empty))
      ~f:(fun ~key:name ~data:(w,elt) acc -> do_if_unseen name (depends name true w) acc elt) in
  res

let check_trapezoid curr_width (t :trapezoid) : unit =
  if t.shift <> 0 then
    fail "Only rectangles are supported at the moment. (shift %d is non-null)"
      t.shift;
  if t.upper_width <> curr_width then
    fail "Only rectangles are supported at the moment. (upper_width %d is different from base width %d)"
      t.upper_width curr_width;
  let (name, pattern) = t.pattern in
  if t.height mod (Array.length pattern) <> 0 then
    fail "Trapezoid height %d is not a multiple of pattern %s height (%d)."
      t.height name (Array.length pattern);
  if t.upper_width mod (Array.length pattern.(0)) <> 0 then
    fail "Trapezoid width %d is not a multiple of pattern %s width (%d)."
      t.upper_width name (Array.length pattern.(0))

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
       check_trapezoid curr_width t;
    (* <0 should be checked during parsing *)
       aux curr_name t.upper_width e
    | Link (n, _) ->
      if not (SMap.mem garment.elements n) then
        fail "Undefined link : \"%s\" in piece \"%s\"." n curr_name
  in
  SMap.iter garment.elements
	    ~f:(fun ~key:name ~data:(w,elt) -> aux name w elt);
  (* We now check for collisions in links *)
  let dep_check ~key:curr_name ~data:curr_deps =
    let curr_width = match SMap.find garment.elements curr_name with
      | Some (w, _) -> w
      | None -> assert false in (* Undefined link, should have been detected earlier *)
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


let compile garment (free, deps) : string =
  let remove_dep target_name key unsat_deps =
    SMap.change unsat_deps target_name
      (function None -> assert false
              | Some deps ->
                assert (DSet.mem deps key);
                Some (DSet.remove deps key)) in
  let rec explore curr_name curr_width (ws, unsat_deps, s) = function
    | Split l ->
      List.fold l
        ~init:(ws, unsat_deps, s)
        ~f:(fun (ws, unsat_deps, s) (pos, w, next) ->
            (*Knit that shit up*)
            explore curr_name w (ws, unsat_deps, s) next)
    | Trapezoid (t, next) ->
      (*Knit it*)
      explore curr_name t.upper_width (ws, unsat_deps, s) next
    | Link (target_name, pos) ->
      let unsat_deps' = remove_dep target_name (pos, curr_width, curr_name) unsat_deps in
      match SMap.find unsat_deps' target_name with
      | None -> assert false
      | Some deps ->
        if DSet.is_empty deps then
          (target_name::ws, unsat_deps', s)
        else
          (ws, unsat_deps', s)
  in
  let rec consume_ws (ws, unsat_deps, s) : string =
    match ws with
    | [] -> s
    | name::rest ->
      match SMap.find garment.elements name with
      | None -> assert false
      | Some (w, e) -> explore name w (rest, unsat_deps, s) e |> consume_ws
  in
  let header = sprintf "Instructions for \"%s\":\n" garment.name in
  consume_ws (SMap.keys free, deps, header)
