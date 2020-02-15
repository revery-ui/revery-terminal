module Sub: {
  let terminal:
    (~id: int, ~cmd: string, ~rows: int, ~columns: int) =>
    Isolinear.Sub.t(Msg.t);
};

module Effects: {
  let input: (~id: int, ~key: int32) => Isolinear.Effect.t(Msg.t);
};
