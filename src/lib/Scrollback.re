type t = {
  size: int,
  // The 'scrollback' is represented as a circular array,
  // where 'startPosition' is the position of index 0,
  // and 'nextInsertPosition' is the location of the next insert.
  rows: array(array(Vterm.ScreenCell.t)),
  // The current place to insert
  mutable nextInsertPosition: int,
  // 'Start' position
  mutable startPosition: int,
};

let make = (~size) => {
  size,
  rows: Array.make(size, [||]),
  nextInsertPosition: 0,
  startPosition: 0,
};

let push = (~cells, scrollBack) => {
  let {size, nextInsertPosition, _} = scrollBack;
  let idx = nextInsertPosition mod size;

  let startPosition =
    nextInsertPosition >= size
      ? nextInsertPosition - 1 : scrollBack.startPosition;

  // Slower, immutable version:
  //let rows = Array.copy(scrollBack.rows);

  // Faster, unsafe version:
  let rows = scrollBack.rows;

  rows[idx] = cells;

  scrollback.nextInsertPosition = nextInsertPosition + 1;
  scrollback.startPosition = startPosition;
};

let pop = (~cells as _, scrollBack) => ();

let getAt = (~index, scrollBack) => {
  let idx = (index - scrollBack.startPosition) mod scrollBack.size;
  scrollBack.rows[idx];
};

let getAvailableRows = scrollBack => {
  scrollBack.nextInsertPosition > scrollBack.size
    ? scrollBack.size : scrollBack.nextInsertPosition;
};
