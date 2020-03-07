module DamageInfo = {
  type t = {
    row: int,
    col: int,
  };
};

type t = {
  damageCounter: int,
  rows: int,
  columns: int,
  dirtyCells: array(bool),
  cells: array(Vterm.ScreenCell.t),
  scrollBack: RingBuffer.t(array(Vterm.ScreenCell.t)),
  vterm: option(Vterm.t),
};

let getVisibleCell = (~row, ~column, screen) => {
  switch (screen.vterm) {
  | None => Vterm.ScreenCell.empty
  | Some(vterm) =>
    let idx = row * screen.columns + column;
    if (idx >= Array.length(screen.cells)) {
      Vterm.ScreenCell.empty;
    } else {
      if (screen.dirtyCells[idx]) {
        screen.cells[idx] = Vterm.Screen.getCell(~row, ~col=column, vterm);
        screen.dirtyCells[idx] = false;
      };

      screen.cells[idx];
    };
  };
};

let updateCell = ({columns, cells, dirtyCells, _}, damage: DamageInfo.t) => {
  let idx = damage.row * columns + damage.col;
  dirtyCells[idx] = true;
};

let updateDirtyCells = (model, damages) => {
  List.iter(updateCell(model), damages);
};

let damaged = (model, damages: list(DamageInfo.t)) => {
  // UGLY MUTATION
  updateDirtyCells(model, damages);
  {...model, damageCounter: model.damageCounter + 1};
};

let getVisibleRows = model => model.rows;
let getTotalRows = model => model.rows + RingBuffer.size(model.scrollBack);

let pushScrollback = (~cells, screen) => {
  RingBuffer.push(cells, screen.scrollBack);

  {...screen, damageCounter: screen.damageCounter + 1};
};

let popScrollback = (~cells as _, screen) => {
  {
    // TODO
    ...screen,
    damageCounter: screen.damageCounter + 1,
  };
};

let getCell = (~row, ~column, screen) => {
  let scrollbackRows = RingBuffer.size(screen.scrollBack);

  if (row >= scrollbackRows) {
    getVisibleCell(~row=row - scrollbackRows, ~column, screen);
  } else {
    let scrollbackRow = RingBuffer.getAt(row, screen.scrollBack);
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
    dirtyCells: Array.make(rows * columns, true),
    cells: Array.make(rows * columns, Vterm.ScreenCell.empty),
  };
};

let make = (~vterm: Vterm.t, ~scrollBackSize, ~rows, ~columns) => {
  damageCounter: 0,
  rows: 0,
  columns: 0,
  dirtyCells: Array.make(rows * columns, true),
  cells: Array.make(rows * columns, Vterm.ScreenCell.empty),
  scrollBack: RingBuffer.make(~capacity=scrollBackSize, [||]),
  vterm: Some(vterm),
};

let initial = {
  damageCounter: 0,
  rows: 0,
  columns: 0,
  dirtyCells: Array.make(0, true),
  cells: Array.make(0, Vterm.ScreenCell.empty),
  scrollBack: RingBuffer.make(~capacity=0, [||]),
  vterm: None,
};
