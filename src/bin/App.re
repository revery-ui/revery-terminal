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
          let charCode = Char.code(c) |> Uchar.of_int;
          Store.dispatch(InputKey(Unicode(charCode), None));
        },
        text,
      )
    });

  let _: unit => unit =
    Revery.Window.onKeyDown(
      window,
      (keyEvent: Key.KeyEvent.t) => {
        open Vterm;
        let {keycode, keymod, _}: Key.KeyEvent.t = keyEvent;

        if (Key.Keymod.isControlDown(keymod)) {
          let keyName =
            Key.Keycode.getName(keycode) |> String.lowercase_ascii;
          // Only handle simple ascii case for now
          if (String.length(keyName) == 1) {
            let uchar = keyName.[0] |> Char.code |> Uchar.of_int;
            Store.dispatch(InputKey(Unicode(uchar), Control));
          };
        } else {
          let key =
            switch (keycode) {
            // From: https://wiki.libsdl.org/SDLKeycodeLookup
            | 8 => Some(Backspace)
            | 9 => Some(Tab)
            | 13 => Some(Enter)
            | 27 => Some(Escape)
            | 127 => Some(Delete)
            | 1073741898 => Some(Home)
            | 1073741899 => Some(PageUp)
            | 1073741901 => Some(End)
            | 1073741902 => Some(PageDown)
            | 1073741903 => Some(Right)
            | 1073741904 => Some(Left)
            | 1073741905 => Some(Down)
            | 1073741906 => Some(Up)
            | _ => None
            };

          key |> Option.iter(k => Store.dispatch(InputKey(k, None)));
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
