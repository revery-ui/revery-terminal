type t = {
  size: int,
  rows: array(array(Vterm.ScreenCell.t)),
  // The current place to insert
  nextInsertPosition: int,
  // 'Start' position
  startPosition: int,
};

let make = (~size) => {
  size, 
  rows: Array.make(size, [||]),
  nextInsertPosition: 0,
  startPosition: 0,
};

let push = (~cells, scrollBack) => {
  let { size, nextInsertPosition, _ } = scrollBack;
  let idx = nextInsertPosition mod size;

  let startPosition = nextInsertPosition >= size ? nextInsertPosition - 1 : 
  scrollBack.startPosition;

  let rows = Array.copy(scrollBack.rows);

  rows[idx] = Array.copy(cells);

  {
    ...scrollBack,
    rows,
    nextInsertPosition: nextInsertPosition + 1,
    startPosition,
  };
}
let pop = (~cells as _, scrollBack) => scrollBack;

let getAt = (~index, scrollBack) => {
  let idx = (index - scrollBack.startPosition) mod scrollBack.size;
  scrollBack.rows[idx]
};

let getAvailableRows = (scrollBack) => {
  scrollBack.nextInsertPosition > scrollBack.size ?
  scrollBack.size : scrollBack.nextInsertPosition;
}
