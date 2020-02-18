open Terminal;

// This type [t] is the heart of our app - the entire state
// of our terminal is contained here!
type t = {
  screen: Screen.t,
  cursor: Terminal.cursorPosition,
  font: option(Msg.fontInfo),
  pixelWidth: int,
  pixelHeight: int,
};

let initial: t = {
  screen: Screen.initial,
  cursor: Terminal.{visible: false, row: 0, column: 0},
  font: None,
  pixelWidth: 1,
  pixelHeight: 1,
};

let calculateRowsAndColumns = ({pixelWidth, pixelHeight, font}) => {
  switch (font) {
  | None => (0, 0)
  | Some(loadedFont) =>
    let {characterWidth, lineHeight, _}: Msg.fontInfo = loadedFont;

    let columns = int_of_float(float(pixelWidth) /. characterWidth);
    let rows = int_of_float(float(pixelHeight) /. lineHeight);
    (rows, columns);
  };
};

let checkResizeTerminalEffect = model => {
  // Get current rows / columns
  let columns = Screen.getColumns(model.screen);
  let rows = Screen.getColumns(model.screen);

  // Check what our rows / columns should be
  let (newRows, newColumns) = calculateRowsAndColumns(model);

  if (newRows != rows || newColumns != columns) {
    // We're out of sync - ask the terminal to resize to our new size
    let effect =
      Terminal.Effects.resize(~id=1, ~rows=newRows, ~columns=newColumns);

    // Effects can dispatch... so we need to map from the terminal messages
    // to our messages
    effect |> Isolinear.Effect.map(msg => Msg.Terminal(msg));
  } else {
    // All good, we're at the right size - nothing to do.
    Isolinear.Effect.none;
  };
};

let mapTerminalEffect = terminalEffect =>
  terminalEffect |> Isolinear.Effect.map(msg => Msg.Terminal(msg));

let updater = (model: t, msg: Msg.t) => {
  let noop = (model, Isolinear.Effect.none);

  switch (msg) {
  | Msg.InputKey(key) => (
      model,
      Terminal.Effects.input(~id=1, ~key) |> mapTerminalEffect,
    )
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
  | Msg.FontLoaded(font) =>
    let newModel = {...model, font: Some(font)};
    (newModel, checkResizeTerminalEffect(newModel));
  | Msg.WindowSizeChanged({pixelWidth, pixelHeight}) =>
    let newModel = {...model, pixelWidth, pixelHeight};
    (newModel, checkResizeTerminalEffect(newModel));
  // TODO:
  | Msg.Terminal(PropSet(_)) => noop
  };
};
