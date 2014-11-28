%{
open Descr

let eprintf = Core.Std.eprintf (* It had to be done *)
let sprintf = Core.Std.sprintf

module Map = Core.Std.String.Map

type param = Int of int | Name of string

exception Parse_error of string

let print_pos (pos : Lexing.position) = Lexing.(sprintf "Line %d, Symbol %d" pos.pos_lnum (pos.pos_cnum - pos.pos_bol))

let sort_split = Core.Std.List.sort ~cmp:compare

let get_arg arg map =
  match Map.find map arg with
    Some arg -> arg
  | None ->
    eprintf "Arg missing : %s\n%!" arg;
    exit 1

let get_arg_int arg map =
  match get_arg arg map with
    Int i -> i
  | _ ->
    eprintf "Int missing : %s\n%!" arg;
    exit 2

let get_arg_name arg map =
  match get_arg arg map with
    Name n -> n
  | _ ->
    eprintf "Name missing : %s\n%!" arg;
    exit 3

let make_trapezoid args =
  { height = get_arg_int "height" args
  ; shift = get_arg_int "shift" args
  ; upper_width = get_arg_int "upper_width" args
  ; pattern = get_arg_name "pattern" args
  }

%}

%token <string> NAME DESCR
%token <int> INT
%token H_NAME H_DESCR
%token LPAREN RPAREN LBRACE RBRACE
%token PIECE DEF SEQ SEP COL
%token START STOP TRAPEZOID LINK SPLIT
%token EOF

%start <Descr.garment> main

%%


main:
  H_NAME; n = NAME; H_DESCR; d = DESCR; e = body; EOF
                                                { { elements = e
                                                  ; name = n
                                                  ; descr = d }
                                                }
| error                                         { raise (Parse_error "Invalid header") }
;

body:
  (* Empty *)                                   { Map.empty }
| b = body; PIECE; n = NAME; DEF; START; w = INT; SEQ; e = element
                                                { Map.add b n (w,e) }
| body; PIECE; n = NAME; error                  { raise (Parse_error (sprintf "Error while parsing piece %s" n)) }
| body; error                                   { raise (Parse_error (sprintf "Syntax error at %s" (print_pos $startpos($1)))) }
;

element:
  STOP                                          { Split [] }
| LINK; n = NAME; pos = INT                     { Link (n, pos) }
| SPLIT; l = split_body                         { Split (sort_split l) }
| TRAPEZOID; p = trapezoid_params; SEQ; e = element
                                                { Trapezoid (p,e) }
;

split_body:
                                                { [] }
| b = split_body; pos = INT; w = INT; LBRACE; e = element; RBRACE
                                                { (pos, w, e)::b }

trapezoid_params:
  LPAREN; ps = params; RPAREN                   { (make_trapezoid ps) }

params:
  (* Empty *)                                   { Map.empty }
| p = param                                     { let (k,v) = p in
                                                  Map.singleton k v }
| ps = params; SEP; p = param                   { let (k,v) = p in
                                                  Map.add ps k v }
;

param:
  k = NAME; COL; v = value                      { (k, v) }
;

value:
  i = INT                                       { Int i }
| n = NAME                                      { Name n }
;
