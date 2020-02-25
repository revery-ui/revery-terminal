type msg = ReveryTerminal.effect;

module Internal = {
  type t = {
    pty: Pty.t,
    fd: Unix.file_descr,
    input: in_channel,
    output: out_channel,
    inputThread: Thread.t,
    terminal: ReveryTerminal.t,
    effectStream: Isolinear.Stream.t(ReveryTerminal.effect),
    refCount: ref(int),
  };

  let idToTerminal: Hashtbl.t(int, t) = Hashtbl.create(1);

  let create = (~id, ~rows, ~columns, ~cmd) => {
    switch (Hashtbl.find_opt(idToTerminal, id)) {
    | Some(term) =>
      incr(term.refCount);
      term;
    | None =>
      let (effectStream, effectDispatch) = Isolinear.Stream.create();

      // Create pseudo-terminal pty
      let pty = Pty.create(~shellCmd=cmd);
      let fd = Pty.get_descr(pty);
      let input = Unix.in_channel_of_descr(fd);
      let output = Unix.out_channel_of_descr(fd);
      let () = Pty.resize(~rows, ~columns, pty);

      let onEffect = (eff: ReveryTerminal.effect) => {
        switch (eff) {
        | Output(str) =>
          // Send 'output' to terminal
            output_string(output, str);
            flush(output);
        | eff => effectDispatch(eff)
        };
      };

      let terminal = ReveryTerminal.make(~rows, ~columns, ~onEffect);
      let inputThread: Thread.t =
        Thread.create(
          () => {
            while (true) {
              let c = input_char(input);
              let str = String.make(1, c);
              Revery.App.runOnMainThread(() => {
                let _ = ReveryTerminal.write(~input=str, terminal);
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
        terminal,
        inputThread,
        refCount,
        effectStream,
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

        let unsubscribe =
          Isolinear.Stream.subscribe(info.effectStream, effect => {
            dispatch(effect)
          });

        {unsubscribe, id: params.id};
      };

      let update = (~params as _, ~state, ~dispatch as _) => {
        state;
      };

      let dispose = (~params as _, ~state) => {
        state.unsubscribe();
        Internal.dispose(~id=state.id);
      };
    });

  let terminal = (~id, ~cmd, ~rows, ~columns) => {
    TerminalSub.create({id, cmd, rows, columns});
  };
};

module Effects = {
  let input = (~id: int, ~key, ~modifier) =>
    Isolinear.Effect.create(~name="terminal.input", () => {
      switch (Hashtbl.find_opt(Internal.idToTerminal, id)) {
      | None => ()
      | Some({terminal, _}) =>
        ReveryTerminal.input(~modifier, ~key, terminal)
      }
    });
  let resize = (~id: int, ~rows, ~columns) =>
    Isolinear.Effect.create(~name="terminal.input", () => {
      switch (Hashtbl.find_opt(Internal.idToTerminal, id)) {
      | None => ()
      | Some({pty, terminal, _}) =>
        Pty.resize(~rows, ~columns, pty);
        ReveryTerminal.resize(~rows, ~columns, terminal);
      }
    });
};
