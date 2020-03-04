module RingBuffer = RingBuffer;

module Cursor: {
  type t = {
    row: int,
    column: int,
    visible: bool,
  };
};

module Theme: {
  type t = int => Revery.Color.t;

  let default: t;
};

module Font: {
  type t = {
    font: Revery.Font.t,
    fontSize: float,
    lineHeight: float,
    characterHeight: float,
    characterWidth: float,
    smoothing: Revery.Font.Smoothing.t,
  };

  let make:
    (~smoothing: Revery.Font.Smoothing.t=?, ~size: float, Revery.Font.t) => t;
};

module Screen: {
  type t;

  let initial: t;

  // Get a cell, where [row] is 0-indexed, starting at the earliest
  // row in the scrollback buffer.
  let getCell: (~row: int, ~column: int, t) => Vterm.ScreenCell.t;

  // [getTotalRows(screen)] returns the count of total rows, including scrollback.
  // For example, if the screen shows 40 rows, and there are 100 lines
  // in the scrollback buffer, this would report 140.
  let getTotalRows: t => int;

  // [getVisibleRows(screen)] returns the number of visible rows, not including scrollback.
  let getVisibleRows: t => int;

  let getColumns: t => int;
};

type effect =
  | ScreenResized(Screen.t)
  | ScreenUpdated(Screen.t)
  | TermPropChanged(Vterm.TermProp.t)
  | CursorMoved(Cursor.t)
  | Output(string);

type t;

type unsubscribe = unit => unit;

let make:
  (
    ~scrollBackSize: int=?,
    ~rows: int,
    ~columns: int,
    ~onEffect: effect => unit,
    unit
  ) =>
  t;

let write: (~input: string, t) => unit;
let input: (~modifier: Vterm.modifier=?, ~key: Vterm.key, t) => unit;
let resize: (~rows: int, ~columns: int, t) => unit;

let render:
  (
    ~defaultForeground: Revery.Color.t=?,
    ~defaultBackground: Revery.Color.t=?,
    ~scrollBarBackground: Revery.Color.t=?,
    ~scrollBarThumb: Revery.Color.t=?,
    ~theme: Theme.t=?,
    ~font: Font.t,
    ~cursor: Cursor.t,
    Screen.t
  ) =>
  Revery.UI.element;
