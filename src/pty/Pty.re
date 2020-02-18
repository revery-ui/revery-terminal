module Internal = {
  external openpty: unit => (Unix.file_descr, Unix.file_descr) =
    "revery_terminal_openpty";
  external ioctl_set_size: (Unix.file_descr, int, int) => int =
    "revery_terminal_ioctl_set_size";
};

type t = Unix.file_descr;

// This code was inspired from:
// https://www.uninformativ.de/blog/postings/2018-02-24/0/POSTING-en.html
let create = (~shellCmd) => {
  let (master, slave) = Internal.openpty();
  let pid = Unix.fork();

  // pid == 0 means the 'child' process - this is the one we'll
  // run the shell.
  if (pid == 0) {
    let _sid: int = Unix.setsid();
    Unix.close(master);
    Unix.dup2(slave, Unix.stdin);
    Unix.dup2(slave, Unix.stdout);
    Unix.dup2(slave, Unix.stderr);

    Unix.close(slave);
    // This doesn't ever return
    Unix.execv(shellCmd, [||]);
  } else {
    Unix.close(slave);
  };

  master;
};

let get_descr = v => v;

let resize = (~rows, ~columns, pty: t) => {
  let _: int = Internal.ioctl_set_size(pty, columns, rows);
  ();
};
