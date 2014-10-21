(* construction de la description de piece en language bas-niveau *)

type stitch = Endroit | Envers | Torse

type row = stitch list * int
(* description du motif et le nombre de fois qu'on le répète sur la ligne *)

type parallelogram = row list * int
(* suite des rangs à tricoter et le nombre de fois qu'on les repete pour parvenir à tricoter *)

type piece = parallelogram list
(* en fait piece correspond à un arbre de trapezes, a continuer *)

(* construction du type d'un modèle de tricot *)

type title = string

type description = string

type instructions = piece
(* le type piece en langage bas-niveau donne les informations necessaires pour 
donner les instructions à l'utilisateur *)

type modele = title * description * instructions;;


let string_of_stitch = function
	| Endroit -> "endroit"
	| Envers -> "envers"
	| Torse -> "torse"

let rec string_of_stitchlist = function
	| [] -> ""
	| [a]  -> "un point " ^ string_of_stitch a
	| a :: l -> "un point " ^ string_of_stitch a ^ " puis " ^ string_of_stitchlist l

let string_of_instructions r =
	let (l,i) = r in
	"Tricotez " ^ string_of_stitchlist l
	^ ", et répétez " ^ string_of_int i ^ " fois ce motif. \n"

let rec string_of_row = function
	| [] -> ""
	| [a] -> string_of_instructions a
	| a::l -> string_of_instructions a ^ "         " ^ string_of_row l

let string_of_rowlist l =
	let rec aux i = function 
		a::q ->  "Rang " ^ string_of_int i ^ " : " ^ string_of_row a ^ aux (i+1) q
		| [] -> "" in
	aux 1 l

let string_of_parallelogram p =
	let (rl, i) = p in
	string_of_rowlist rl ^ "Tricotez au total " ^ string_of_int i 
	^ " fois ces " ^ string_of_int (List.length rl) ^ " rangs.\n"

let rec string_of_piece = function
	a::l -> string_of_parallelogram a ^ "\n" ^ string_of_piece l
	| [] -> ""

let string_of_modele m = 
	let (title, description, piece) = m in
	title ^ "\n" ^ description ^ "\n\n"
	^ "Suivez les instructions suivantes pour obtenir votre tricot : \n\n"
	^ string_of_piece piece

(*exemple *)
let test () =
	let p = [ [([Endroit; Envers], 12)]; [([Envers; Endroit; Envers],8)]; [([Envers; Endroit],2);([Endroit; Envers],2)]], 10 in
	let m = "ECHARPE", "écharpe polaire de Blaire", [p] in
	let s = string_of_modele m in
		print_string s;;

if !Sys.interactive then () else test ();;
