let _ =
  if Array.length Sys.argv != 2 then
    print_string "Usage: ./main.native filename.tricot\n"
  else
    let input = open_in Sys.argv.(1) in
    let buf = Lexing.from_channel input in
    let s = Descr.print_garment (Descr_parser.main Descr_lexer.token buf) in
    print_string s
