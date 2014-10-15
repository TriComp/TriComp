(*construction de la description de piece en language bas-niveau *)

type stitch = Endroit | Envers | Torse;; 

type row = stitch list * int;; 
(* description du motif et le nombre de fois qu'on le répète sur la ligne *)

type parallelogram = row list * int;;
(*suite des rangs à tricoter et le nombre de fois qu'on les répètes pour parvenir à tricoter*)

type piece = parallelogram list;;
(*en fait piece correspond à un arbre de prllgm, à continuer *)

(*construction du type d'un modèle de tricot*)

type title = string;;

type description = string;;

type instructions = piece;;
(* le type piece en langage bas-niveau donne les informations necessaires pour 
donner les instructions à l'utilisateur *)

type modele = title * description * instructions;;



(* fonction de traduction vers le texte final *)
let trad = function
	| Endroit -> "endroit"
	| Envers -> "envers"
	| Torse -> "torse" ;;

let trad : stitch list -> string = function
	(a :: l) -> "un point " ^ (trad a) ^ " puis " ^ trad l
	| a :: nil  -> "un point " ^ trad a 
	| nil -> "";;
