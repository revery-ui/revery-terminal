module RingBuffer = RingBuffer;
module Cursor = Cursor;
module Font = Font;
module Screen = Screen;
module Theme = Theme;

type effect =
  | ScreenResized(Screen.t)
  | ScreenUpdated(Screen.t)
  | TermPropChanged(Vterm.TermProp.t)
  | CursorMoved(Cursor.t)
  | Output(string);

type t = {
  screen: ref(Screen.t),
  vterm: Vterm.t,
  cursor: ref(Cursor.t),
};

type unsubscribe = unit => unit;

let make =
    (
      ~scrollBackSize=1000,
      ~rows: int,
      ~columns: int,
      ~onEffect as dispatch,
      (),
    ) => {
  let cursor = ref(Cursor.initial);
  let vterm = Vterm.make(~rows, ~cols=columns);
  let screen = ref(Screen.make(~scrollBackSize, ~rows, ~columns));
  Vterm.setUtf8(~utf8=true, vterm);
  Vterm.Screen.setAltScreen(~enabled=true, vterm);

  Vterm.setOutputCallback(~onOutput=str => {dispatch(Output(str))}, vterm);

  Vterm.Screen.setMoveCursorCallback(
    ~onMoveCursor=
      (newPos, _oldPos, _visible) => {
        let newCursor =
          Cursor.{row: newPos.row, column: newPos.col, visible: true};
        dispatch(CursorMoved(newCursor));
        cursor := newCursor;
      },
    vterm,
  );

  Vterm.Screen.setResizeCallback(
    ~onResize=
      ({rows, cols}) => {
        screen := Screen.resize(~rows, ~columns=cols, screen^);
        dispatch(ScreenResized(screen^));
      },
    vterm,
  );

  Vterm.Screen.setScrollbackPushCallback(
    ~onPushLine=
      cells => {
        screen := Screen.pushScrollback(~cells, screen^);
        dispatch(ScreenUpdated(screen^));
      },
    vterm,
  );
  Vterm.Screen.setScrollbackPopCallback(
    ~onPopLine=
      cells => {
        screen := Screen.pushScrollback(~cells, screen^);
        dispatch(ScreenUpdated(screen^));
      },
    vterm,
  );

  Vterm.Screen.setDamageCallback(
    ~onDamage=
      ({startRow, startCol, endRow, endCol}: Vterm.Rect.t) => {
        let damages = ref([]);
        for (x in startCol to endCol - 1) {
          for (y in startRow to endRow - 1) {
            let cell = Vterm.Screen.getCell(~row=y, ~col=x, vterm);
            damages :=
              [Screen.DamageInfo.{row: y, col: x, cell}, ...damages^];
          };
        };
        screen := Screen.damaged(screen^, damages^);
        dispatch(ScreenUpdated(screen^));
      },
    vterm,
  );

  Vterm.Screen.setTermPropCallback(
    ~onSetTermProp=prop => {dispatch(TermPropChanged(prop))},
    vterm,
  );

  {screen, vterm, cursor};
};

let resize = (~rows, ~columns, {vterm, screen, _}) => {
  screen := Screen.resize(~rows, ~columns, screen^);
  Vterm.setSize(~size={rows, cols: columns}, vterm);

  // After the size changed - re-get all the cells
  let damages = ref([]);
  for (x in 0 to columns - 1) {
    for (y in 0 to rows - 1) {
      let cell = Vterm.Screen.getCell(~row=y, ~col=x, vterm);
      damages := [Screen.DamageInfo.{row: y, col: x, cell}, ...damages^];
    };
  };
  screen := Screen.damaged(screen^, damages^);
};

let write = (~input: string, {vterm, _}) => {
  Vterm.write(~input, vterm) |> (ignore: int => unit);
};

let input = (~modifier=Vterm.None, ~key: Vterm.key, {vterm, _}) => {
  Vterm.Keyboard.input(vterm, key, modifier);
};

let render =
    (
      ~defaultForeground=?,
      ~defaultBackground=?,
      ~scrollBarBackground=?,
      ~scrollBarThumb=?,
      ~scrollBarThickness=?,
      ~theme=Theme.default,
      ~font,
      ~cursor,
      screen,
    ) =>
  <TerminalView
    ?defaultForeground
    ?defaultBackground
    ?scrollBarBackground
    ?scrollBarThumb
    ?scrollBarThickness
    theme
    screen
    cursor
    font
  />;
