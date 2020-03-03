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
  scrollBack: Scrollback.t,
};

let getScreenCell = (~row, ~column, screen) => {
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

let getScreenRows = model => model.rows;
let getTotalRows = model =>
  model.rows + Scrollback.getAvailableRows(model.scrollBack);

let pushScrollback = (~cells, screen) => {
  {
    ...screen,
    damageCounter: screen.damageCounter + 1,
    scrollBack: Scrollback.push(~cells, screen.scrollBack),
  };
};

let popScrollback = (~cells, screen) => {
  {
    ...screen,
    damageCounter: screen.damageCounter + 1,
    scrollBack: Scrollback.pop(~cells, screen.scrollBack),
  };
};

let getCell = (~row, ~column, screen) => {
  let scrollbackRows = Scrollback.getAvailableRows(screen.scrollBack);

  if (row >= scrollbackRows) {
    getScreenCell(~row=row - scrollbackRows, ~column, screen);
  } else {
    let scrollbackRow = Scrollback.getAt(~index=row, screen.scrollBack);
    if (column >= Array.length(scrollbackRow)) {
      Vterm.ScreenCell.empty;
    } else {
      scrollbackRow[column];
    };
  };
};
let getColumns = model => model.columns;

let resize = (~rows, ~columns, model) => {
  {
    ...model,
    damageCounter: model.damageCounter + 1,
    rows,
    columns,
    cells: Array.make(rows * columns, Vterm.ScreenCell.empty),
  };
};

let make = (~scrollBackSize, ~rows, ~columns) => {
  damageCounter: 0,
  rows: 0,
  columns: 0,
  cells: Array.make(rows * columns, Vterm.ScreenCell.empty),
  scrollBack: Scrollback.make(~size=scrollBackSize),
};

let initial = {
  damageCounter: 0,
  rows: 0,
  columns: 0,
  cells: Array.make(0, Vterm.ScreenCell.empty),
  scrollBack: Scrollback.make(~size=0),
};
