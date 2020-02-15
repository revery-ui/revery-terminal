open Revery;
open Revery.Draw;
open Revery.UI;
open Revery.UI.Components;

let render = (model: Model.t) => {
  let containerStyle =
    Style.[
      backgroundColor(Colors.black),
      position(`Absolute),
      justifyContent(`Center),
      alignItems(`Center),
      bottom(0),
      top(0),
      left(0),
      right(0),
    ];

  let innerStyle = Style.[flexDirection(`Row), alignItems(`FlexEnd)];

  let fontPath =
    Revery.Environment.executingDirectory ++ "JetBrainsMono-Medium.ttf";
  let font = Revery.Font.load(fontPath);

  let getColor = (color: Vterm.Color.t) => {
    switch (color) {
    | DefaultBackground => Colors.black |> Color.toSkia
    | DefaultForeground => Colors.purple |> Color.toSkia
    | Rgb(r, g, b) =>
      Skia.Color.makeArgb(
        255l,
        r |> Int32.of_int,
        g |> Int32.of_int,
        b |> Int32.of_int,
      )
    | _ => Colors.green |> Color.toSkia
    };
  };

  let paint = Skia.Paint.make();

  let (width, height) =
    switch (font) {
    | Ok(typeface) =>
      let typeFace = Revery.Font.getSkiaTypeface(typeface);
      Skia.Paint.setTypeface(paint, typeFace);
      Skia.Paint.setTextSize(paint, 12.);
      Skia.Paint.setAntiAlias(paint, true);
      Skia.Paint.setSubpixelText(paint, true);
      Skia.Paint.setLcdRenderText(paint, true);
      let {height, _}: Revery.Font.FontMetrics.t =
        Revery.Font.getMetrics(typeface, 12.);
      let width = Skia.Paint.measureText(paint, "H", None);
      (width, height);
    | _ => (10., 10.)
    };

  let element =
    <Canvas
      style=containerStyle
      render={canvasContext => {
        open Model;
        let columns = Screen.getColumns(model.screen);
        let rows = Screen.getRows(model.screen);
        print_endline("ROWS: " ++ string_of_int(rows));
        for (column in 0 to columns - 1) {
          for (row in 0 to rows - 1) {
            let cell = Screen.getCell(~row, ~column, model.screen);
            let fgColor = getColor(cell.fg);
            Skia.Paint.setColor(paint, fgColor);
            if (String.length(cell.chars) > 0) {
              CanvasContext.drawText(
                ~paint,
                ~x=float(column) *. width,
                ~y=float(row) *. height +. height,
                ~text=String.make(1, cell.chars.[0]),
                canvasContext,
              );
            };
          };
        };

        // If the cursor is visible, let's paint it now
        if (model.cursor.visible) {
          Skia.Paint.setColor(paint, Colors.white |> Color.toSkia);
          CanvasContext.drawRectLtwh(
            ~paint,
            ~left=float(model.cursor.column) *. width,
            ~top=float(model.cursor.row) *. height,
            ~width,
            ~height,
            canvasContext,
          );
        };
      }}
    />;

  element;
};
