type t;

let push: (~cells: array(Vterm.ScreenCell.t), t) => t;

let pop: (~cells: array(Vterm.ScreenCell.t), t) => t;

let make: (~size: int) => t;

let getAt: (~index: int, t) => array(Vterm.ScreenCell.t);
