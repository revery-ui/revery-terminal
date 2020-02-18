type size = {
  rows: int,
  columns: int,
};

type cursorPosition = {
  row: int,
  column: int,
  visible: bool,
};

type windowSize = {
  pixelWidth: int,
  pixelHeight: int,
};

type fontInfo = {
  font: Revery.Font.t,
  lineHeight: float,
  characterHeight: float,
  characterWidth: float,
};

type t =
  | Init
  | InputKey(int32)
  | Terminal(Terminal.msg)
  | FontLoaded(fontInfo)
  | WindowSizeChanged(windowSize);
