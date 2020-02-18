type cursorPosition = {
  row: int,
  column: int,
  visible: bool,
};

type msg =
  | Resized(Screen.t)
  | ScreenUpdated(Screen.t)
  | PropSet(Vterm.TermProp.t)
  | CursorMoved(cursorPosition);

module Sub: {
  let terminal:
    (~id: int, ~cmd: string, ~rows: int, ~columns: int) =>
    Isolinear.Sub.t(msg);
};

module Effects: {
  let input: (~id: int, ~key: int32) => Isolinear.Effect.t(msg);
  let resize:
    (~id: int, ~rows: int, ~columns: int) => Isolinear.Effect.t(msg);
};
