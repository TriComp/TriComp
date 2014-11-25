{
  open Descr_parser
}

rule token = parse
  | [' ' '\t' '\n' '\r'] 			{ token lexbuf }

  | "//"                                        { line_comment lexbuf }
  | "/*"                                        { comment lexbuf }

  | '"'                                         { descr lexbuf }

  | '('						{ LPAREN }
  | ')'						{ RPAREN }
  | '{'                                         { LBRACE }
  | '}'                                         { RBRACE }

  | "piece"                                     { PIECE }
  | ":="                                        { DEF }
  | "||"                                        { SEQ }
  | ','                                         { SEP }
  | ':'                                         { COL }

  | "Name :"                                    { H_NAME }
  | "Description :"                             { H_DESCR }

  | "start"                                     { START }
  | "stop"                                      { STOP }
  | "trapezoid"                                 { TRAPEZOID }
  | "link"                                      { LINK }
  | "split"                                     { SPLIT }

  | '-'?['0'-'9']+ as n                         { INT (int_of_string n) }
  | ['a'-'z']['a'-'z' '0'-'9' '_']* as s        { NAME s }

  | eof                                         { EOF }

and line_comment = parse
  | '\n'                                        { token lexbuf }
  | eof                                         { EOF }
  | _                                           { line_comment lexbuf }

and comment = parse
  | "*/"                                        { token lexbuf }
  | eof                                         { EOF }
  | _                                           { comment lexbuf }

and descr = parse
  | (_* as d)'"'                                { DESCR d }
