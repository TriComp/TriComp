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

let compile_trapezoid width trapezoid : string =
  let rev array = array |> Array.to_list |> List.rev |> Array.of_list in
  let stitch flip value = if flip = value then "endroit" else "envers" in
  (*let duplicate array n =
    let l = (Array.length array) in
    Array.create (l*n) array.(0) |> Array.mapi ~f:(fun i _ -> array.(i mod l)) in *)
  let rle flip line mult array =
    let f (s, prec) value =
      match prec with
      | None -> (s, Some (value, 1))
      | Some (value', n) ->
        if value = value' then
          (s, Some (value', n + 1))
        else
          let plur = if n > 1 then "s" else "" in
          let s' = sprintf "%s %d maille%s %s," s n plur (stitch flip value) in
          (s', Some (value, 1))
    in
    match Array.fold array ~init:(sprintf "\tLigne %d : *" line, None) ~f:f with
    | (_, None) -> assert false
    | (s, Some (value, n)) ->
      let plur = if n > 1 then "s" else "" in
      sprintf "%s %d maille%s %s * %d fois\n" s n plur (stitch (not flip) value) mult
  in
  let (pat_w, pat_h) = get_dims trapezoid.pattern in
  let x_count = width / pat_w in
  let y_count = trapezoid.height / pat_h in
  let (_, pattern) = trapezoid.pattern in
  let (s_res, _) =
    Array.foldi pattern
      ~init:(sprintf "Répetez %d fois le motif suivant :\n" y_count, false)
      ~f:(fun line (s, flip) row ->
          let directed_row = if flip then rev row else row in
          (*let full_row = duplicate directed_row x_count in*)
          (sprintf "%s%s" s (rle flip (line+1) x_count directed_row), not flip)
      )
  in
  s_res

let compile garment (free, deps) : string =
  let remove_dep target_name key unsat_deps =
    SMap.change unsat_deps target_name
      (function None -> assert false
              | Some deps ->
                assert (DSet.mem deps key);
                Some (DSet.remove deps key)) in
  let rec explore curr_name curr_width (ws, unsat_deps, s) = function
    | Split l ->
      let (ws_res, unsat_deps_res, s_res, _) =
        List.fold l
          ~init:(ws, unsat_deps, s, None)
          ~f:(fun (ws, unsat_deps, s, last) (pos, w, next) ->
              let s_pref =
                if pos < 0 then
                  sprintf "Montez %d mailles à droite\n" (-pos)
                else
                  "" in
              let s_suff =
                if pos + w > curr_width then
                  sprintf "Montez %d mailles à gauche\n" (pos + w - curr_width)
                else
                  "" in
              let skip = match last with
                | None -> ""
                | Some pos' ->
                  if pos = pos'
                  then ""
                  else sprintf "Fermez %d mailles\n" (pos - pos')
              in
              let s' = sprintf "%s%s%s%s" s s_pref skip s_suff in
              let (ws_res, unsat_deps_res, s_res) =
                explore curr_name w (ws, unsat_deps, s') next in
              (ws_res, unsat_deps_res, s_res, Some (pos + w))
            )
      in (ws_res, unsat_deps_res, s_res)
    | Trapezoid (t, next) ->
      let s' = sprintf "%s\n%s\n" s (compile_trapezoid curr_width t) in
      explore curr_name t.upper_width (ws, unsat_deps, s') next
    | Link (target_name, pos) ->
      let unsat_deps' = remove_dep target_name (pos, curr_width, curr_name) unsat_deps in
      match SMap.find unsat_deps' target_name with
      | None -> assert false
      | Some deps ->
        if DSet.is_empty deps then
          (target_name::ws, unsat_deps', s)
        else
          let s' = sprintf "%sLaissez l'aiguille de côté pour le moment.\n" s in
          (ws, unsat_deps', s')
  in
  let rec consume_ws (ws, unsat_deps, s) : string =
    match ws with
    | [] -> sprintf "%s\nFélicitations, vous avez fini!\n" s
    | name::rest ->
      match SMap.find garment.elements name with
      | None -> assert false
      | Some (w, e) ->
        let preds = match SMap.find deps name with
          | None -> DSet.empty
          | Some preds -> preds in
        let s' =
          if DSet.is_empty preds then
            sprintf "%s\nDébut de la pièce \"%s\":\nMontez %d mailles.\n" s name w
          else
            sprintf "%s\nDébut de la pièce \"%s\":\nAssemblez les dépendances.\n" s name
        in
        explore name w (rest, unsat_deps, s') e |> consume_ws
  in
  let header = sprintf "Instructions pour \"%s\":\n" garment.name in
  consume_ws (SMap.keys free, deps, header)
