open Descr

let uniq l =
  let rec aux acc = function
    | [] -> List.rev acc
    | x::y::zs when x=y -> aux acc (y::zs)
    | x::xs -> aux (x::acc) xs in
  aux [] (List.sort compare l)

let out_edges elt =
  let rec aux acc = function
  | Trapezoid (_, next) -> aux acc next
  | Split (left, _, right) ->
    let acc' = aux acc left in
    aux acc' right
  | Link (name, _) -> name::acc
  | Stop -> acc in
  uniq (aux [] elt)

let topo_sort elts = ()
