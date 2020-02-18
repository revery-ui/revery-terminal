open Revery;
open Revery.UI;

module Store =
  Isolinear.Store.Make({
    type msg = Msg.t;
    type model = Model.t;

    let initial = Model.initial;

    let updater = Model.updater;
    let subscriptions = model => {
      Terminal.Sub.terminal(~id=1, ~cmd="/bin/bash", ~rows=40, ~columns=80)
      |> Isolinear.Sub.map(msg => Msg.Terminal(msg));
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

  let _: unit => unit =
    Revery.Window.onSizeChanged(
      window,
      ({width, height}) => {
         Store.dispatch(Msg.WindowSizeChanged({
          pixelWidth: width,
          pixelHeight: height,
         }))
      });

  Revery.Window.startTextInput(window);

  Store.dispatch(Msg.Init);
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
