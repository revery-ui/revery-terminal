type size = {
  rows: int,
  columns: int,
};

type cursorPosition = {
  row: int,
  column: int,
  visible: bool,
};

type t =
  | Init
  | InputKey(int32)
  | Terminal(Terminal.msg)
