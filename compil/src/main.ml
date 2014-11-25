open Core.Std

let parse input () =
    ( match input with
        `Stdin -> stdin
      | `File f ->
        try In_channel.create f with
        | Sys_error s ->
          eprintf "Invalid source : %s\n%!" s;
          exit 1 )
    |> Lexing.from_channel
    |> Descr_parser.main Descr_lexer.token
    |> Descr.print_garment
    |> printf "%s%!"



let input =
  Command.Spec.Arg_type.create
    (function
        "-" -> `Stdin
      | filename ->
        match Sys.is_file filename with
          `Yes -> `File filename
        | _ ->
          eprintf "'%s' is not a regular file.\n%!" filename;
          exit 1
    )

let command = Command.basic
    ~summary:"Generates a sequence of instructions from a .tricot file"
    ~readme:(const "")
    Command.Spec.(empty +> anon ("input" %: input))
    parse

let _ =
  Command.run command
