module DamageInfo = {
  type t = {
    row: int,
    col: int,
    cell: Vterm.ScreenCell.t,
  };
};

type t = {
  damageCounter: int,
  rows: int,
  columns: int,
  cells: array(Vterm.ScreenCell.t),
};

let getCell = (~row, ~column, screen) => {
  let idx = row * screen.columns + column;
  if (idx >= Array.length(screen.cells)) {
    Vterm.ScreenCell.empty;
  } else {
    screen.cells[idx];
  };
};

let updateCell = ({columns, cells, _}, damage: DamageInfo.t) => {
  let idx = damage.row * columns + damage.col;
  cells[idx] = damage.cell;
};

let updateCells = (model, damages) => {
  List.iter(updateCell(model), damages);
};

let damaged = (model, damages: list(DamageInfo.t)) => {
  // UGLY MUTATION
  updateCells(model, damages);
  {...model, damageCounter: model.damageCounter + 1};
};

let getRows = model => model.rows;
let getColumns = model => model.columns;

let resize = (~rows, ~columns, model) => {
  {
    damageCounter: model.damageCounter + 1,
    rows,
    columns,
    cells: Array.make(rows * columns, Vterm.ScreenCell.empty),
  };
};

let initial = {
  damageCounter: 0,
  rows: 0,
  columns: 0,
  cells: Array.make(0, Vterm.ScreenCell.empty),
};
