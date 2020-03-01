type t = {
  size: int,
  rows: array(array(Vterm.ScreenCell.t)),
};

let make = (~size) => {size, rows: Array.make(size, [||])};

let push = (~cells as _, scrollBack) => scrollBack;
let pop = (~cells as _, scrollBack) => scrollBack;

let getAt = (~index as _, scrollBack) => [||];
