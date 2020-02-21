open Revery;
open Revery.UI;

module Store =
  Isolinear.Store.Make({
    type msg = Msg.t;
    type model = Model.t;

    let initial = Model.initial;

    let updater = Model.updater;
    let subscriptions = _model => {
      Terminal.Sub.terminal(~id=1, ~cmd="/bin/bash", ~rows=40, ~columns=80)
      |> Isolinear.Sub.map(msg => Msg.Terminal(msg));
    };
  });

let loadFont = () => {
  // Load font
  let fontPath =
    Revery.Environment.executingDirectory ++ "JetBrainsMono-Medium.ttf";
  let fontLoadResult = Revery.Font.load(fontPath);
  let fontSize = 12.0;

  switch (fontLoadResult) {
  | Ok(font) =>
    let {height, lineHeight, _}: Revery.Font.FontMetrics.t =
      Revery.Font.getMetrics(font, fontSize);
    let {width, _}: Revery.Font.measureResult =
      Revery.Font.measure(font, fontSize, "M");
    Store.dispatch(
      Msg.FontLoaded({
        font,
        fontSize,
        lineHeight,
        characterHeight: height,
        characterWidth: width,
      }),
    );
  | Error(msg) => failwith(msg)
  };
};

let init = app => {
  ReveryTerminal.hello();

  // Create a primary window for our app
  let window = App.createWindow(app, "Revery Terminal");

  // Connect with the isolinear store -
  let redraw = UI.start(window, <View />);

  // ...wire up any state changes to trigger a redraw of the UI
  let _unsubscribe: unit => unit =
    Store.onModelChanged(newModel => newModel |> Renderer.render |> redraw);

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

        if (keycode == 8  /* backspace */
            || keycode == 9  /* tab */
            || keycode == 13  /* return */
            || keycode == 27) {
          /* escape */ Store.dispatch(Msg.InputKey(keycode |> Int32.of_int));
        };
      },
    );

  let _: unit => unit =
    Revery.Window.onSizeChanged(window, ({width, height}) => {
      Store.dispatch(
        Msg.WindowSizeChanged({pixelWidth: width, pixelHeight: height}),
      )
    });

  Revery.Window.startTextInput(window);

  loadFont();
};

App.start(init);
