type t;

/*
 * [create(~shellCmd="/bin/bash")] creates a new pseudo-terminal session,
 * forking the process, creating a PTY,
 * and running [shellCmd].
 */
let create: (~shellCmd: string) => t;

let resize: (~rows: int, ~columns: int, t) => unit;

let get_descr: t => Unix.file_descr;
