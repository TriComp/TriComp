open Core.Std

(* Command line arguments:
   ./tricomp COMMAND [ARGS]
   COMMAND:
    * format [-i] [-o <output>] <filename>
        -i: in place
    * check <filename>
    * compil [-o <output>] <filename>
 *)

let print garment =
  printf "%s%!" (Descr.print_garment garment)

let compute_deps garment =
  let (free, deps) = Compil.make_dep_graph garment in
  (try
    Compil.sanity_check Compil.({ min_width = 0; min_height = 0}) garment deps
   with
     Failure s -> eprintf "Error : %s\n%!" s;
     exit 1);
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


let format =
  Command.basic ~summary:"Format .tricot file"
    Command.Spec.(
      empty
      +> anon (maybe_with_default `Stdin ("filename" %: input))
      +> flag "-o" (optional string) ~doc:"output Write result to 'output'"
      +> flag "-i" no_arg ~doc:" Modify input file in place"
    )
    (fun filename output inplace () ->
       (*TODO: format file or stdin and write output to output or to input file*)
       ()
    )

let check =
  Command.basic ~summary:"Check .tricot file"
    Command.Spec.(
      empty
      +> anon (maybe_with_default `Stdin ("filename" %: input))
    )
    (fun filename () ->
       (*TODO: check file or stdin*)
       ()
    )

let compil =
  Command.basic ~summary:"Compil .tricot file"
    Command.Spec.(
      empty
      +> anon (maybe_with_default `Stdin ("filename" %: input))
      +> flag "-o" (optional string) ~doc:"output Write result to 'output'"
    )
    (fun filename output ->
       (*TODO: compil file or stdin and write output to output*)
       parse compute_deps filename
    )

let command =
  Command.group ~summary:"Compil and manipulate .tricot files"
    [ "format", format; "check", check; "compil", compil ]

let () = Command.run command
