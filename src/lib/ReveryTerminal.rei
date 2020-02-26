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

  let getCell: (~row: int, ~column: int, t) => Vterm.ScreenCell.t;

  let getRows: t => int;
  let getColumns: t => int;

  let initial: t;
};

type effect =
  | ScreenResized(Screen.t)
  | ScreenUpdated(Screen.t)
  | TermPropChanged(Vterm.TermProp.t)
  | CursorMoved(Cursor.t)
  | Output(string);

type t;

type unsubscribe = unit => unit;

let make: (~rows: int, ~columns: int, ~onEffect: effect => unit) => t;

let write: (~input: string, t) => unit;
let input: (~modifier: Vterm.modifier=?, ~key: Vterm.key, t) => unit;
let resize: (~rows: int, ~columns: int, t) => unit;

let render:
  (
   ~defaultForeground: Revery.Color.t=?,
   ~defaultBackground: Revery.Color.t=?, ~theme: Theme.t=?, ~font: Font.t, ~cursor: Cursor.t, Screen.t) =>
  Revery.UI.element;
