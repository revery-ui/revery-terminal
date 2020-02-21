module Screen: {
	type t;

	let getCell: (~row: int, ~column: int, t) => Vterm.ScreenCell.t;

	let getRows: t => int;
	let getColumns: t => int;

	let initial: t;
};

type cursorPosition = {
	row: int,
	column: int,
	visible: bool,
};

type effect =
| ScreenResized(Screen.t)
| ScreenUpdated(Screen.t)
| TermPropChanged(Vterm.TermProp.t)
| CursorMoved(cursorPosition)
| Output(string);

type t;

type unsubscribe = unit => unit;

let make: (~rows: int, ~columns: int, ~onEffect: effect => unit) => t;

let write: (~input: string, t) => int;

let input: (~key: int32, t) => unit;
