module Cursor: {
  type t = {
    row: int,
    column: int,
    visible: bool,
  };
};

module Font: {
  type t = {
    font: Revery.Font.t,
    fontSize: float,
    lineHeight: float,
    characterHeight: float,
    characterWidth: float,
  };

  let make: (~size: float, Revery.Font.t) => t;
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

let write: (~input: string, t) => int;
let input: (~key: int32, t) => unit;
let resize: (~rows: int, ~columns: int, t) => unit;

let render:
  (~font: Font.t, ~screen: Screen.t, ~cursor: Cursor.t) => Revery.UI.element;
