open Terminal;

type t = {
  screen: Screen.t,
  cursor: Terminal.cursorPosition,
  font: option(Msg.fontInfo),
};

let initial: t = {
  screen: Screen.initial,
  cursor: Terminal.{visible: false, row: 0, column: 0},
  font: None,
};

let mapTerminalEffect = (terminalEffect) => 
  terminalEffect
  |> Isolinear.Effect.map(msg => Msg.Terminal(msg));

let updater = (model: t, msg: Msg.t) => {

  let noop = (model, Isolinear.Effect.none);

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
  // TODO:
  | Init
  | Msg.FontLoaded(_) => noop
  | Msg.Terminal(PropSet(_)) => noop
  | Msg.WindowSizeChanged(_) => noop
  };
};
