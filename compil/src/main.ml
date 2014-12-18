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

let sanity_check garment =
  let (free, deps) = Compil.make_dep_graph garment in
  try
    Compil.sanity_check Compil.({ min_width = 0; min_height = 0}) garment deps
  with
    Failure s -> eprintf "Error : %s\n%!" s;
    exit 1

let compile garment : string =
  let (free, deps) = Compil.make_dep_graph garment in
  (try
    Compil.sanity_check Compil.({ min_width = 0; min_height = 0}) garment deps
   with
     Failure s -> eprintf "Error : %s\n%!" s;
     exit 1);
  Compil.compile garment (free, deps)

let parse action input =
  try ( match input with
        `Stdin -> stdin
      | `File f ->
        try In_channel.create f with
        | Sys_error s ->
          eprintf "Invalid source : %s\n%!" s;
          exit 1 )
      |> Lexing.from_channel
      |> Descr_parser.main Descr_lexer.token
      |> action
  with
  | Failure s -> eprintf "%s\n%!" s; exit 1
(*  | _ -> eprintf "Something went terribly wrong, oops\n%!"; exit 42 *)

let write output s : unit =
  let chan =
    match output with
    | None -> stdout
    | Some file ->
      try Out_channel.create file with
      | _ -> eprintf "Error while opening file %s\n%!" file; exit 6
  in
  fprintf chan "%s%!" s

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
    ( fun file_in file_out inplace () ->
       let file_out' =
         if inplace then
           match file_in with
           | `File f -> Some f
           | `Stdin -> None
         else
           file_out
       in
       parse Descr.print_garment file_in |> write file_out'
    )

let check =
  Command.basic ~summary:"Check .tricot file"
    Command.Spec.(
      empty
      +> anon (maybe_with_default `Stdin ("filename" %: input))
    )
    (fun file_in () ->
       parse sanity_check file_in
    )

let compil =
  Command.basic ~summary:"Compil .tricot file"
    Command.Spec.(
      empty
      +> anon (maybe_with_default `Stdin ("filename" %: input))
      +> flag "-o" (optional string) ~doc:"output Write result to 'output'"
    )
    (fun file_in file_out () ->
       parse compile file_in |> write file_out
    )

let command =
  Command.group ~summary:"Compil and manipulate .tricot files"
    [ "format", format; "check", check; "compil", compil ]

let () = Command.run command
