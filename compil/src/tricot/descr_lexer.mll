{
  open Descr_parser
}

rule token = parse
  | '\n'                                        { Lexing.new_line lexbuf; token lexbuf }
  | [' ' '\t' '\r'] 			        { token lexbuf }

  | "//"                                        { line_comment lexbuf }
  | "/*"                                        { comment lexbuf }

  | '"'                                         { descr "" lexbuf }

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
  | '\n'                                        { Lexing.new_line lexbuf; token lexbuf }
  | eof                                         { EOF }
  | _                                           { line_comment lexbuf }

and comment = parse
  | '\n'                                        { Lexing.new_line lexbuf; comment lexbuf }
  | "*/"                                        { token lexbuf }
  | eof                                         { EOF }
  | _                                           { comment lexbuf }

and descr s = parse
  | '\n'                                        { Lexing.new_line lexbuf; descr (s^"\n") lexbuf }
  | '\r'                                        { descr s lexbuf }
  | '"'                                         { DESCR s }
  | [^'\n' '\r' '"']* as s'                     { descr (s^s') lexbuf }
