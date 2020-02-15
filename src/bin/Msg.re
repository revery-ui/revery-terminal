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
  | TerminalResized(Screen.t)
  | TerminalCursorMoved(cursorPosition)
  | TerminalScreenUpdated(Screen.t)
  | TerminalPropSet(Vterm.TermProp.t);
