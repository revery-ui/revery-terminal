open ReveryTerminal;

// This type [t] is the heart of our app - the entire state
// of our terminal is contained here!
type t = {
  screen: Screen.t,
  cursor: Cursor.t,
  font: option(Font.t),
  pixelWidth: int,
  pixelHeight: int,
};

let initial: t = {
  screen: Screen.initial,
  cursor: Cursor.{visible: false, row: 0, column: 0},
  font: None,
  pixelWidth: 1,
  pixelHeight: 1,
};

type size = {
  rows: int,
  columns: int,
};

type windowSize = {
  pixelWidth: int,
  pixelHeight: int,
};

type msg =
  | InputKey(Vterm.key, Vterm.modifier)
  | Terminal(Terminal.msg)
  | FontLoaded(Font.t)
  | WindowSizeChanged(windowSize);

let calculateRowsAndColumns = ({pixelWidth, pixelHeight, font, _}) => {
  switch (font) {
  | None => (0, 0)
  | Some(loadedFont) =>
    let {characterWidth, lineHeight, _}: Font.t = loadedFont;

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
    effect |> Isolinear.Effect.map(msg => Terminal(msg));
  } else {
    // All good, we're at the right size - nothing to do.
    Isolinear.Effect.none;
  };
};

let termPropEffect = _prop => {
  Isolinear.Effect.none;
};

let mapTerminalEffect = terminalEffect =>
  terminalEffect |> Isolinear.Effect.map(msg => Terminal(msg));

let updater = (model, msg) => {
  let noop = (model, Isolinear.Effect.none);

  switch (msg) {
  | InputKey(key, modifier) => (
      model,
      Terminal.Effects.input(~id=1, ~key, ~modifier) |> mapTerminalEffect,
    )
  | Terminal(ReveryTerminal.ScreenResized(screen)) => (
      {...model, screen},
      Isolinear.Effect.none,
    )
  | Terminal(ReveryTerminal.ScreenUpdated(screen)) => (
      {...model, screen},
      Isolinear.Effect.none,
    )
  | Terminal(ReveryTerminal.CursorMoved(cursor)) => (
      {...model, cursor},
      Isolinear.Effect.none,
    )
  | Terminal(ReveryTerminal.TermPropChanged(prop)) => 
   (
      model,
      termPropEffect(prop),
    )
  | FontLoaded(font) =>
    let newModel = {...model, font: Some(font)};
    (newModel, checkResizeTerminalEffect(newModel));
  | WindowSizeChanged({pixelWidth, pixelHeight}) =>
    let newModel = {...model, pixelWidth, pixelHeight};
    (newModel, checkResizeTerminalEffect(newModel));
  // TODO:
  | Terminal(_) => noop
  };
};

let subscriptions = _model => {
  Terminal.Sub.terminal(~id=1, ~cmd="/bin/bash", ~rows=40, ~columns=80)
  |> Isolinear.Sub.map(msg => Terminal(msg));
};
