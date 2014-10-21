let _ =
  let input = open_in Sys.argv.(1) in
  let buf = Lexing.from_channel input in
  let s = Descr.print_garment (Descr_parser.main Descr_lexer.token buf) in
  print_string s
