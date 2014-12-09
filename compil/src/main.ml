open Core.Std

let print garment =
  printf "%s%!" (Descr.print_garment garment)

let compute_deps garment =
  let (free, deps) = Compil.make_dep_graph garment in
  Compil.sanity_check Compil.({ min_width = 0; min_height = 0}) garment deps;
  deps
  |> Compil.sexp_of_deps
  |> Sexp.to_string
  |> printf "deps:%s\n%!"


let parse action input () =
    ( match input with
        `Stdin -> stdin
      | `File f ->
        try In_channel.create f with
        | Sys_error s ->
          eprintf "Invalid source : %s\n%!" s;
          exit 1 )
    |> Lexing.from_channel
    |> Descr_parser.main Descr_lexer.token
    |> action

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

let command action = Command.basic
    ~summary:"Generates a sequence of instructions from a .tricot file"
    ~readme:(const "")
    Command.Spec.(empty +> anon ("input" %: input))
    (parse action)

let _ =
  Command.run (command compute_deps)
