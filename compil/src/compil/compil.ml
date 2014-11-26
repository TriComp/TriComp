open Core.Std
open Descr

module SSet = String.Set
module SMap = String.Map

type dep_graph = element SMap.t * SSet.t SMap.t (* free, deps *)

(* Builds a graph on the set of elements with vertices between e1 and e2 if e1 depends on e2 *)
let make_dep_graph garment : dep_graph =
  let add_dep name = function
    | None -> Some (SSet.singleton name)
    | Some deps -> Some (SSet.add deps name) in
  let rec depends curr_name acc = function
    | Split l -> List.fold ~init:acc ~f:(fun acc' (_, _, elt) -> depends curr_name acc' elt) l
    | Trapezoid (_, e) -> depends curr_name acc e
    | Link (target_name, _) ->
      let (free, deps) = acc in
      (SMap.remove free target_name, SMap.change deps target_name (add_dep curr_name))
  in
  let free = SMap.map garment.elements ~f:(fun (_,e) -> e) in
  SMap.fold garment.elements
    ~init:(free, SMap.empty)
    ~f:(fun ~key:name ~data:(_,elt) acc -> depends name acc elt)

let check garment : [>`Ok of dep_graph | `Error] = `Error
