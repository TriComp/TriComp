let _ =
  let input = open_in Sys.argv.(1) in
  let buf = Lexing.from_channel input in
  Descr_parser.main Descr_lexer.token buf
