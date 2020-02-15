type t = {
  screen: Screen.t,
  cursor: Msg.cursorPosition,
};

let initial: t = {
  screen: Screen.initial,
  cursor: Msg.{visible: false, row: 0, column: 0},
};

let updater = (model, msg) => {
  switch (msg) {
  | Msg.InputKey(key) => (model, Terminal.Effects.input(~id=1, ~key))
  | Msg.TerminalResized(screen) => (
      {...model, screen},
      Isolinear.Effect.none,
    )
  | Msg.TerminalScreenUpdated(screen) => (
      {...model, screen},
      Isolinear.Effect.none,
    )
  | Msg.TerminalCursorMoved(cursor) => (
      {...model, cursor},
      Isolinear.Effect.none,
    )
  | Init
  | Msg.TerminalPropSet(_) => (model, Isolinear.Effect.none)
  };
};
