module Screen = Screen;

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

type t = {
	screen: ref(Screen.t),
	vterm: Vterm.t,
};

type unsubscribe = unit => unit;

let make = (~rows: int, ~columns: int, ~onEffect as dispatch) => {
      let vterm = Vterm.make(~rows, ~cols=columns);
      let screen =
        ref(
          Screen.initial |> Screen.resize(~rows, ~columns),
        );
      Vterm.setUtf8(~utf8=true, vterm);
      Vterm.Screen.setAltScreen(~enabled=true, vterm);

      Vterm.setOutputCallback(
        ~onOutput=str => dispatch(Output(str)),
        vterm,
      );

      Vterm.Screen.setMoveCursorCallback(
        ~onMoveCursor=
          (newPos, _oldPos, _visible) => 
            dispatch(CursorMoved{
              row: newPos.row,
              column: newPos.col,
              visible: true,
            }),
        vterm,
      );

      Vterm.Screen.setResizeCallback(
        ~onResize=
          ({rows, cols}) => {
            screen := Screen.resize(~rows, ~columns=cols, screen^);
            dispatch(ScreenResized(screen^));
          },
        vterm,
      );

      Vterm.Screen.setDamageCallback(
        ~onDamage=
          ({startRow, startCol, endRow, endCol}: Vterm.Rect.t) => {
            let damages = ref([]);
            for (x in startCol to endCol - 1) {
              for (y in startRow to endRow - 1) {
                let cell = Vterm.Screen.getCell(~row=y, ~col=x, vterm);
                damages :=
                  [
                    Screen.DamageInfo.{row: y, col: x, cell},
                    ...damages^,
                  ];
              };
            };
            screen := Screen.damaged(screen^, damages^);
			dispatch(ScreenUpdated(screen^));
          },
        vterm,
      );

	{screen, vterm};
}

let write = (~input: string, ({vterm, _})) => {
	Vterm.write(~input, vterm);
};

let input = (~key: int32, ({vterm, _})) => {
	Vterm.Keyboard.unichar(vterm, key, Vterm.None);
};
