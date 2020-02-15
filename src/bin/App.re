open Revery;
open Revery.UI;

module Store =
  Isolinear.Store.Make({
    type msg = Msg.t;
    type model = Model.t;

    let initial = Model.initial;

    let updater = Model.updater;
    let subscriptions = model => {
      Terminal.Sub.terminal(~id=1, ~cmd="/bin/bash", ~rows=40, ~columns=80);
    };
  });

let start = window => {
  // Set up some nice colorized logging
  Timber.App.enable();
  Timber.App.setLevel(Timber.Level.trace);

  // And enable record backtraces, to check for exceptions
  Printexc.record_backtrace(true);
  Printexc.set_uncaught_exception_handler((exn, backtrace) => {
    prerr_endline("Exception: " ++ Printexc.to_string(exn))
  });

  let _: unit => unit =
    Revery.Window.onTextInputCommit(
      window, ({text}: Revery.Events.textInputEvent) => {
      String.iter(
        c => {
          let charCode = Char.code(c) |> Int32.of_int;
          Store.dispatch(Msg.InputKey(charCode));
        },
        text,
      )
    });

  let _: unit => unit =
    Revery.Window.onKeyDown(
      window,
      (keyEvent: Key.KeyEvent.t) => {
        let {keycode, _}: Key.KeyEvent.t = keyEvent;

        if (keycode == 13 || keycode == 27) {
          Store.dispatch(Msg.InputKey(keycode |> Int32.of_int));
        };
      },
    );

  Revery.Window.startTextInput(window);

  Store.dispatch(Msg.Init);
  //Store.dispatch(Msg.Resized(40, 80));
  /*Vterm.setOutputCallback(~onOutput=(str) => {
      output_string(output, str);
      flush(output);
    }, vterm);

    Vterm.Screen.setResizeCallback(~onResize=({rows, cols}) => {
        Store.dispatch(Model.Msg.Resized(rows, cols));
    }, vterm);

    Vterm.Screen.setTermPropCallback(~onSetTermProp=(termProp) => {
      print_endline(Vterm.TermProp.toString(termProp));
    }, vterm)

    Vterm.Screen.setMoveCursorCallback(~onMoveCursor=(newPos, oldPos, visible) => {
       Store.dispatch(Model.Msg.CursorMoved(Model.Cursor.{
        row: newPos.row,
        column: newPos.col,
        visible,
       }));
    }, vterm);*/
  /*Vterm.Screen.setMoveRectCallback(~onMoveRect=(dest, src) => {
      print_endline ("Move rect - dest: " ++ Vterm.Rect.toString(dest));
      print_endline ("Move rect - src: " ++ Vterm.Rect.toString(src));
    }, vterm);*/
  /* Vterm.Screen.setDamageCallback(~onDamage=({startRow, startCol, endRow, endCol}: Vterm.Rect.t) => {

       let damages = ref([]);
       for (x in startCol to endCol - 1) {
         for (y in startRow to endRow - 1) {
           let cell = Vterm.Screen.getCell(~row=y, ~col=x, vterm);
           damages := [Model.DamageInfo.{row: y, col: x, cell}, ...damages^];
           ();
         }
       }
       Store.dispatch(Model.Msg.Damaged(damages^));

       //Gc.full_major();

       }, vterm);

     let inputThread: Thread.t = Thread.create(() => {
        while (true) {
           let c = input_char(input);
           let str = String.make(1, c);
           Revery.App.runOnMainThread(() => {
             let _ = Vterm.write(~input=str, vterm);
           });
        }
     }, ());
     */
};

let init = app => {
  let win = App.createWindow(app, "Revery Terminal");

  let redraw = UI.start(win, <View />);

  let onModelChanged = newModel => {
    let () = redraw(Renderer.render(newModel));
    ();
  };

  let _unsubscribe: unit => unit = Store.onModelChanged(onModelChanged);
  let _unsubscribe: unit => unit =
    Store.onPendingEffect(() => {Store.runPendingEffects()});
  start(win);
};

App.start(init);
