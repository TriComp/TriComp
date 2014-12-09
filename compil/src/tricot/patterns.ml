open Core.Std
open Descr

let pat1 =
  let res = [|[|true; false|];
	      [|true; false|]|]
  in
  ("cotes_1x1", res)

let pat2 =
  let res = [|[|true; false|];
	      [|false; true|]|]
    in
  ("point_de_riz", res)

let pat3 =
  let res = [|[|true; true; false; false|];
	      [|true; true; false; false|]|]
  in
  ("cotes_2x2", res)

let pat4 =
  let res = [|[|false; true; true|];
	      [|false; true; true|]|]
  in
  ("fausses_cotes_anglaises", res)

let pat5 =
  let res = [|[|true|];
	      [|true|];
	      [|false|];
	      [|true|]|]
  in
  ("jersey_raye", res)

let pat6 =
  let res = [|[|false; false; false; false; false; true;  true;  true |];
	      [|true;  true;  true;  true;  true; false; false; false|]|]
  in
  ("cotes_plates", res)

let pat7 =
  let res = [|[|true; true; true; false; true; true; true; false; false; false|];
	      [|true; true; true; false; true; true; true; false; false; false|];
	      [|true; true; true; false; true; true; true; false; false; false|];
	      [|true; true; true;  true; true; true; true;  true;  true; true|]|]
  in
  ("Cotes_piquees", res)

let pat8 =
  let res = [|[|false; false; false; true; false; false; false|];
	      [|false; false;  true; true;  true; false; false|];
	      [|false;  true;  true; true;  true;  true; false|];
	      [| true;  true;  true; true;  true;  true; true|];
	      [|false;  true;  true; true;  true;  true; false|];
	      [|false; false;  true; true;  true; false; false|];
	      [|false; false; false; true; false; false; false|]|]
  in
  ("losanges", res)

let pat9 =
  let res = [|[|false; true; false; true; false; true; false; false; false; false; false|];
	      [| true; false; true; false; true; false; true;  true;  true;  true; true|]|]
  in
  ("cotes_point_de_riz", res)

let pat10 =
  let res = [|[|false; false; false; true; true; true|];
	      [|false; false; false; true; true; true|];
	      [|false; false; false; true; true; true|];
	      [|true; true; true; false; false; false|];
	      [|true; true; true; false; false; false|];
	      [|true; true; true; false; false; false|]|]
  in
  ("damier_3x3", res)

let check_pattern ((_, pattern):pattern) =
  let open Array in
  assert (length pattern > 0);
  let width = length (pattern.(0)) in
  assert (width > 0);
  assert (for_all pattern ~f:(fun line -> length line = width))


let default_patterns =
  let pattern_list = [pat1; pat2; pat3; pat4; pat5; pat6; pat7; pat8; pat9; pat10] in
  List.iter pattern_list ~f:check_pattern;
  match String.Map.of_alist pattern_list with
  | `Duplicate_key s ->
    eprintf "Fatal error : Duplicate name : %s\n%!" s;
    assert false
  | `Ok map -> map
