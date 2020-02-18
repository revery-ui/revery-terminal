open Terminal;

type t = {
  screen: Screen.t,
  cursor: Terminal.cursorPosition,
};

let initial: t = {
  screen: Screen.initial,
  cursor: Terminal.{visible: false, row: 0, column: 0},
};

let mapTerminalEffect = (terminalEffect) => 
  terminalEffect
  |> Isolinear.Effect.map(msg => Msg.Terminal(msg));

let updater = (model: t, msg: Msg.t) => {
  switch (msg) {
  | Msg.InputKey(key) => (model, 
    Terminal.Effects.input(~id=1, ~key) |> mapTerminalEffect)
  | Msg.Terminal(Resized(screen)) => (
      {...model, screen},
      Isolinear.Effect.none,
    )
  | Msg.Terminal(ScreenUpdated(screen)) => (
      {...model, screen},
      Isolinear.Effect.none,
    )
  | Msg.Terminal(CursorMoved(cursor)) => (
      {...model, cursor},
      Isolinear.Effect.none,
    )
  | Init
  | Msg.Terminal(PropSet(_)) => (model, Isolinear.Effect.none)
  };
};
