open Core.Std
open Descr

type dep_graph = Element.Set.t * Element.Set.t String.Map.t

(* Builds a graph on the set of elements with vertices between e1 and e2 if e1 depends on e2 *)
(*let make_dep_graph garment =
  let children acc = function
      Split l -> List.fold ~init:acc ~f:(fun
  let f ~key:name ~data:(_,element) s
*)
let check garment : [>`Ok of dep_graph | `Error] = `Error
