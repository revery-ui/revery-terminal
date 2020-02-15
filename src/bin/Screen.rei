type t = Screen_Internal.t;

let getCell: (~row: int, ~column: int, t) => Vterm.ScreenCell.t;

let getRows: t => int;
let getColumns: t => int;

let initial: t;
