%{
open Descr
open Printf
type param = Int of int | Name of string

exception Parse_error of string (* TODO : Add position in file *)


let get_arg arg map =
  try StringMap.find arg map
    with Not_found -> eprintf "Arg missing : %s\n%!" arg; exit 1

let get_arg_int arg map =
  match get_arg arg map with
    Int i -> i
  | _ -> eprintf "Int missing : %s\n%!" arg; exit 2


let get_arg_name arg map =
  match get_arg arg map with
    Name n -> n
  | _ -> eprintf "Name missing : %s\n%!" arg; exit 3


let make_trapezoid args =
  { height = get_arg_int "height" args
  ; shift = get_arg_int "shift" args
  ; lower_width = get_arg_int "lower_width" args
  ; upper_width = get_arg_int "upper_width" args
  ; pattern = get_arg_name "pattern" args
  }

%}

%token <string> NAME DESCR
%token <int> INT
%token H_NAME H_DESCR
%token LPAREN RPAREN LBRACE RBRACE
%token PIECE DEF SEQ SEP COL
%token START STOP TRAPEZOID LINK LEFT RIGHT SPLIT
%token EOF

%right SEQ

%start <Descr.garment> main

%%


main:
  H_NAME; n = NAME; H_DESCR; d = DESCR; e = body; EOF
                                                { { elements = e
                                                  ; name = n
                                                  ; descr = d }
                                                }
;

body:
  (* Empty *)                                   { StringMap.empty }
| PIECE; n = NAME; DEF; START; SEQ; e = element; b = body
                                                { StringMap.add n e b }
;

element:
  STOP                                          { Stop }
| LINK; s = slot; n = NAME                      { Link (n,s) }
| SPLIT; i = INT; LBRACE l = element; RBRACE; LBRACE; r = element; RBRACE
                                                { Split (l,i,r) }
| TRAPEZOID; p = trapezoid_params; SEQ; e = element
                                                { Trapezoid (p,e) }

;

slot:
  LEFT                                          { Left }
| RIGHT                                         { Right }
;

trapezoid_params:
  LPAREN; ps = params; RPAREN                    { (make_trapezoid ps) }

params:
  (* Empty *)                                   { StringMap.empty }
| p = param                                     { let (k,v) = p in
                                                  StringMap.add k v StringMap.empty }
| p = param; SEP; ps = params                   { let (k,v) = p in
                                                  StringMap.add k v ps }
;

param:
  k = NAME; COL; v = value                      { (k, v) }
;

value:
  i = INT                                       { Int i }
| n = NAME                                      { Name n }
;
