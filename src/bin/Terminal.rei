type msg = ReveryTerminal.effect;

module Sub: {
  let terminal:
    (~id: int, ~cmd: string, ~rows: int, ~columns: int) =>
    Isolinear.Sub.t(msg);
};

module Effects: {
  let input: (~id: int, ~key: int32) => Isolinear.Effect.t(msg);
  let resize:
    (~id: int, ~rows: int, ~columns: int) => Isolinear.Effect.t(msg);
};
