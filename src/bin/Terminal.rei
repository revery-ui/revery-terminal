type msg = ReveryTerminal.effect;

module Sub: {
  let terminal:
    (~id: int, ~cmd: string, ~rows: int, ~columns: int) =>
    Isolinear.Sub.t(msg);
};

module Effects: {
  let input: (~id: int, ~key: Vterm.key) => Isolinear.Effect.t(msg);
  let resize:
    (~id: int, ~rows: int, ~columns: int) => Isolinear.Effect.t(msg);
};
