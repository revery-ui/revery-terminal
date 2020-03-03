module Color = Revery.Color;
module Colors = Revery.Colors;
open Revery.Draw;
open Revery.UI;
open Revery.UI.Components;

module Styles = {
  let container = (bg) =>
    Style.[
      backgroundColor(bg),
      position(`Absolute),
      justifyContent(`Center),
      alignItems(`Center),
      bottom(0),
      top(0),
      left(0),
      right(0),
    ];

  let scrollBarWidth = 8;

  let scrollBar =
    Style.[
      position(`Absolute),
      justifyContent(`Center),
      alignItems(`Center),
      bottom(0),
      top(0),
      right(0),
      width(scrollBarWidth),
    ];
};

type terminalSize = {
  width: int,
  height: int,
};

let%component make =
    (
      ~defaultBackground=?,
      ~defaultForeground=?,
      ~theme=Theme.default,
      ~screen: Screen.t,
      ~cursor: Cursor.t,
      ~font: Font.t,
      (),
    ) => {

  let%hook () = Hooks.effect(Always, () => None);
  let%hook (size, setSize) = Hooks.state({width: 0, height: 0});

  let bg =
    switch (defaultBackground) {
    | Some(v) => v
    | None => theme(0)
    };

  let fg =
    switch (defaultForeground) {
    | Some(v) => v
    | None => theme(15)
    };


  let getColor = (color: Vterm.Color.t) => {
    let outColor =
      switch (color) {
      | DefaultBackground => bg
      | DefaultForeground => fg
      | Rgb(r, g, b) =>
        if (r == 0 && g == 0 && b == 0) {
          bg;
        } else if (r == 240 && g == 240 && b == 240) {
          fg;
        } else {
          Revery.Color.rgb_int(r, g, b);
        }
      | Index(idx) => theme(idx)
      };

    Revery.Color.toSkia(outColor);
  };

  let getFgColor = (cell: Vterm.ScreenCell.t) => {
    cell.reverse == 0 ? getColor(cell.fg) : getColor(cell.bg);
  };

  let getBgColor = (cell: Vterm.ScreenCell.t) => {
    cell.reverse == 0 ? getColor(cell.bg) : getColor(cell.fg);
  };

  let element =
    <View
      style=Styles.container(bg)
      onDimensionsChanged={({width, height,_}) => {
       setSize((_) => {
        width,
        height,
       });
      }}
    >
    <Canvas
      style=Styles.container(bg)
      render={canvasContext => {
        let {
          font,
          lineHeight,
          characterWidth,
          characterHeight,
          fontSize,
          smoothing,
        }: Font.t = font;
        let defaultBackgroundColor = bg |> Color.toSkia;

        let backgroundPaint = Skia.Paint.make();
        Skia.Paint.setAntiAlias(backgroundPaint, false);

        let textPaint = Skia.Paint.make();
        let typeFace = Revery.Font.getSkiaTypeface(font);
        Skia.Paint.setTypeface(textPaint, typeFace);
        Skia.Paint.setTextSize(textPaint, fontSize);
        Revery.Font.Smoothing.setPaint(smoothing, textPaint);

        Skia.Paint.setLcdRenderText(textPaint, true);

        let columns = Screen.getColumns(screen);
        let rows = Screen.getTotalRows(screen);

        let renderBackground = (row, yOffset) => {
          for (column in 0 to columns - 1) {
              let cell = Screen.getCell(~row, ~column, screen);

              let bgColor = getBgColor(cell);
              if (bgColor != defaultBackgroundColor) {
                Skia.Paint.setColor(backgroundPaint, bgColor);
                CanvasContext.drawRectLtwh(
                  ~paint=backgroundPaint,
                  ~left=float(column) *. characterWidth,
                  ~top=yOffset,
                  ~height=lineHeight,
                  ~width=characterWidth,
                  canvasContext,
                );
              };
          };
        };

        let renderText = (row, yOffset) => {
          for (column in 0 to columns - 1) {
              let cell = Screen.getCell(~row, ~column, screen);

              let fgColor = getFgColor(cell);

              Skia.Paint.setColor(textPaint, fgColor);
              if (String.length(cell.chars) > 0) {
                let char = cell.chars.[0];
                let code = Char.code(char);
                if (code != 0) {
                  CanvasContext.drawText(
                    ~paint=textPaint,
                    ~x=float(column) *. characterWidth,
                    ~y=yOffset +. characterHeight,
                    ~text=String.make(1, cell.chars.[0]),
                    canvasContext,
                  );
                };
              };
          };
        };
        
        let perLineRenderer = ImmediateList.render(
          ~scrollY=0.,
          ~rowHeight=lineHeight,
          ~height=lineHeight *. float(rows),
          ~count=rows,
        );

        perLineRenderer(~render=renderBackground, ());
        perLineRenderer(~render=renderText, ());

        // If the cursor is visible, let's paint it now
        if (cursor.visible) {
          Skia.Paint.setColor(textPaint, getColor(DefaultForeground));
          CanvasContext.drawRectLtwh(
            ~paint=textPaint,
            ~left=float(cursor.column) *. characterWidth,
            ~top=float(cursor.row) *. lineHeight,
            ~width=characterWidth,
            ~height=lineHeight,
            canvasContext,
          );
        };
      }}
    />
    <View style=Styles.scrollBar>
    <Slider
      vertical=true
      sliderLength={size.height}
      thumbLength={100}
      trackThickness={Styles.scrollBarWidth}
      thumbThickness={Styles.scrollBarWidth}
      minimumValue=0.
      maximumValue=1.
      value=0.5
    />
    </View>
  </View>;


  element;
};
