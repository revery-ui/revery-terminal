type t;

let push: (~cells: array(Vterm.ScreenCell.t), t) => unit;

let pop: (~cells: array(Vterm.ScreenCell.t), t) => unit;

let make: (~size: int) => t;

let getAt: (~index: int, t) => array(Vterm.ScreenCell.t);

let getAvailableRows: t => int;
