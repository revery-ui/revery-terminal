type cursorPosition = {
  row: int,
  column: int,
  visible: bool,
};

type msg =
  | Resized(Screen.t)
  | ScreenUpdated(Screen.t)
  | PropSet(Vterm.TermProp.t)
  | CursorMoved(cursorPosition);

module Internal = {
  type t = {
    pty: Pty.t,
    fd: Unix.file_descr,
    input: in_channel,
    output: out_channel,
    inputThread: Thread.t,
    vterm: Vterm.t,
    screen: ref(Screen_Internal.t),
    resizeStream: Isolinear.Stream.t(Screen.t),
    screenUpdateStream: Isolinear.Stream.t(Screen.t),
    cursorMoveStream: Isolinear.Stream.t(cursorPosition),
    //termPropsStream: Isolinear.Stream.t(Vterm.TermProp.t),
    refCount: ref(int),
  };

  let idToTerminal: Hashtbl.t(int, t) = Hashtbl.create(1);

  let create = (~id, ~rows, ~columns, ~cmd) => {
    switch (Hashtbl.find_opt(idToTerminal, id)) {
    | Some(term) =>
      incr(term.refCount);
      term;
    | None =>
      let (resizeStream, resizeDispatch) = Isolinear.Stream.create();
      let (screenUpdateStream, screenUpdateDispatch) =
        Isolinear.Stream.create();
      let (cursorMoveStream, cursorMoveDispatch) = Isolinear.Stream.create();

      let pty = Pty.create(~shellCmd="/bin/bash");
      let fd = Pty.get_descr(pty);
      let input = Unix.in_channel_of_descr(fd);
      let output = Unix.out_channel_of_descr(fd);

      let () = Pty.resize(~rows, ~columns, pty);

      let vterm = Vterm.make(~rows, ~cols=columns);
      let screen =
        ref(
          Screen_Internal.initial |> Screen_Internal.resize(~rows, ~columns),
        );
      Vterm.setUtf8(~utf8=true, vterm);
      Vterm.Screen.setAltScreen(~enabled=true, vterm);

      Vterm.setOutputCallback(
        ~onOutput=
          str => {
            output_string(output, str);
            flush(output);
          },
        vterm,
      );

      Vterm.Screen.setMoveCursorCallback(
        ~onMoveCursor=
          (newPos, oldPos, visible) => {
            cursorMoveDispatch({
              row: newPos.row,
              column: newPos.col,
              visible: true,
            })
          },
        vterm,
      );

      Vterm.Screen.setResizeCallback(
        ~onResize=
          ({rows, cols}) => {
            screen := Screen_Internal.resize(~rows, ~columns=cols, screen^);
            resizeDispatch(screen^);
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
                    Screen_Internal.DamageInfo.{row: y, col: x, cell},
                    ...damages^,
                  ];
              };
            };
            screen := Screen_Internal.damaged(screen^, damages^);
            let () = screenUpdateDispatch(screen^);
            ();
          },
        vterm,
      );

      let inputThread: Thread.t =
        Thread.create(
          () => {
            while (true) {
              let c = input_char(input);
              let str = String.make(1, c);
              Revery.App.runOnMainThread(() => {
                let _ = Vterm.write(~input=str, vterm);
                ();
              });
            }
          },
          (),
        );

      let refCount = ref(0);
      let info = {
        pty,
        fd,
        input,
        output,
        vterm,
        inputThread,
        refCount,
        screen,
        resizeStream,
        screenUpdateStream,
        cursorMoveStream,
      };
      Hashtbl.replace(idToTerminal, id, info);
      info;
    };
  };

  let dispose = (~id) => {
    switch (Hashtbl.find_opt(idToTerminal, id)) {
    | None => ()
    | Some(term) =>
      decr(term.refCount);
      if (term.refCount^ == 0) {
        Thread.kill(term.inputThread);
        // Close fd
        Hashtbl.remove(idToTerminal, id);
      };
    };
  };
};

module Sub = {
  type terminalSubParams = {
    id: int,
    cmd: string,
    rows: int,
    columns: int,
  };
  type outerMsg = msg;
  module TerminalSub =
    Isolinear.Sub.Make({
      type msg = outerMsg;
      type params = terminalSubParams;

      let subscriptionName = "Terminal";
      type state = {
        id: int,
        unsubscribe: unit => unit,
      };
      let getUniqueId = (params: params) => string_of_int(params.id);

      let init = (~params: params, ~dispatch) => {
        let info =
          Internal.create(
            ~id=params.id,
            ~cmd=params.cmd,
            ~rows=params.rows,
            ~columns=params.columns,
          );

        let sub1 =
          Isolinear.Stream.subscribe(info.resizeStream, screen => {
            dispatch(Resized(screen))
          });

        let sub2 =
          Isolinear.Stream.subscribe(info.screenUpdateStream, screen => {
            dispatch(ScreenUpdated(screen))
          });

        let sub3 =
          Isolinear.Stream.subscribe(info.cursorMoveStream, cursorPos => {
            dispatch(CursorMoved(cursorPos))
          });

        let unsubscribe = () => {
          sub1();
          sub2();
          sub3();
        };
        {unsubscribe, id: params.id};
      };

      let update = (~params, ~state, ~dispatch) => {
        state;
      };

      let dispose = (~params, ~state) => {
        state.unsubscribe();
        Internal.dispose(state.id);
      };
    });

  let terminal = (~id, ~cmd, ~rows, ~columns) => {
    TerminalSub.create({id, cmd, rows, columns});
  };
};

module Effects = {
  let input = (~id: int, ~key: int32) =>
    Isolinear.Effect.create(~name="terminal.input", () => {
      switch (Hashtbl.find_opt(Internal.idToTerminal, id)) {
      | None => ()
      | Some({vterm, _}) => Vterm.Keyboard.unichar(vterm, key, Vterm.None)
      }
    });
  let resize = (~id: int, ~rows, ~columns) =>
    Isolinear.Effect.create(~name="terminal.input", () => {
      switch (Hashtbl.find_opt(Internal.idToTerminal, id)) {
      | None => ()
      | Some({pty, vterm, screen, _}) =>
        Pty.resize(~rows, ~columns, pty);
        screen := Screen_Internal.resize(~rows, ~columns, screen^);
        Vterm.setSize(~size={rows, cols: columns}, vterm);

        // After the size changed - re-get all the cells
        let damages = ref([]);
        for (x in 0 to columns - 1) {
          for (y in 0 to rows - 1) {
            let cell = Vterm.Screen.getCell(~row=y, ~col=x, vterm);
            damages :=
              [
                Screen_Internal.DamageInfo.{row: y, col: x, cell},
                ...damages^,
              ];
          };
        };
        screen := Screen_Internal.damaged(screen^, damages^);
      }
    });
};
