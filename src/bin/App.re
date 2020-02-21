open Revery;
open Revery.UI;

open ReveryTerminal;

module Store =
  Isolinear.Store.Make({
    type msg = Model.msg;
    type model = Model.t;

    let initial = Model.initial;

    let updater = Model.updater;
    let subscriptions = Model.subscriptions;
  });

let init = app => {
  // Create a primary window for our app
  let window = App.createWindow(app, "Revery Terminal");

  let emptyElement = <View />;

  // Connect with the isolinear store -
  let redraw = UI.start(window, emptyElement);

  let render = ({screen, cursor, font, _}: Model.t) =>
    font
    |> Option.map(font => ReveryTerminal.render(~font, ~screen, ~cursor))
    |> Option.value(~default=emptyElement);

  // ...wire up any state changes to trigger a redraw of the UI
  let _unsubscribe: unit => unit =
    Store.onModelChanged(newModel => newModel |> render |> redraw);

  // ...and whenever there are side-effects pending, run those immediately.
  let _unsubscribe: unit => unit =
    Store.onPendingEffect(() => {Store.runPendingEffects()});

  // Set up some nice colorized logging
  Timber.App.enable();
  Timber.App.setLevel(Timber.Level.trace);

  // And enable record backtraces, to check for exceptions
  Printexc.record_backtrace(true);
  Printexc.set_uncaught_exception_handler((exn, _backtrace) => {
    prerr_endline("Exception: " ++ Printexc.to_string(exn))
  });

  // Set up Revery window listeners
  // This would be even nicer if we had a 'ReveryIso' (revery + isolinear)
  // project that provided subscriptions and effects for working with Revery.
  let _: unit => unit =
    Revery.Window.onTextInputCommit(
      window, ({text}: Revery.Events.textInputEvent) => {
      String.iter(
        c => {
          let charCode = Char.code(c) |> Int32.of_int;
          Store.dispatch(InputKey(charCode));
        },
        text,
      )
    });

  let _: unit => unit =
    Revery.Window.onKeyDown(
      window,
      (keyEvent: Key.KeyEvent.t) => {
        let {keycode, _}: Key.KeyEvent.t = keyEvent;

        if (keycode == 8  /* backspace */
            || keycode == 9  /* tab */
            || keycode == 13  /* return */
            || keycode == 27) {
          /* escape */ Store.dispatch(InputKey(keycode |> Int32.of_int));
        };
      },
    );

  let _: unit => unit =
    Revery.Window.onSizeChanged(window, ({width, height}) => {
      Store.dispatch(
        WindowSizeChanged({pixelWidth: width, pixelHeight: height}),
      )
    });

  Revery.Window.startTextInput(window);

  // Load font, and dispatch on success
  let fontPath =
    Revery.Environment.executingDirectory ++ "JetBrainsMono-Medium.ttf";

  switch (Revery.Font.load(fontPath)) {
  | Ok(font) =>
    Store.dispatch(Model.FontLoaded(Font.make(~size=12.0, font)))
  | Error(msg) => failwith(msg)
  };
};

App.start(init);
