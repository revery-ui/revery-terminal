module Color = Revery.Color;
module Colors = Revery.Colors;
open Revery.Draw;
open Revery.UI;

module Styles = {
  let container = bg =>
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

  let scrollBarContainer =
    Style.[position(`Absolute), bottom(0), top(0), right(0)];
};

type terminalSize = {
  width: int,
  height: int,
};

let%component make =
              (
                ~defaultBackground=?,
                ~defaultForeground=?,
                ~scrollBarBackground=Colors.black,
                ~scrollBarThumb=Colors.darkGray,
                ~scrollBarThickness=8,
                ~theme=Theme.default,
                ~screen: Screen.t,
                ~cursor: Cursor.t,
                ~font: Font.t,
                (),
              ) => {
  let%hook (size, setSize) = Hooks.state({width: 0, height: 0});
  let%hook (userScrollY, setUserScrollY) = Hooks.state(None);

  let totalRows = Screen.getTotalRows(screen);
  let screenRows = Screen.getVisibleRows(screen);
  let scrollBackRows = totalRows - screenRows;

  let screenScrollY = float(scrollBackRows) *. font.lineHeight;

  let scrollY =
    switch (userScrollY) {
    | Some(v) => v
    | None => screenScrollY
    };

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

  let onScroll = y => {
    let y = y <= 0. ? 0. : y;
    let maxScroll = float(scrollBackRows) *. font.lineHeight;
    let y = y >= maxScroll ? maxScroll : y;

    if (Float.abs(y -. screenScrollY) <= 10.0) {
      setUserScrollY(_ => None);
    } else {
      setUserScrollY(_ => Some(y));
    };
  };

  let onWheel = ({deltaY, _}: NodeEvents.mouseWheelEventParams) => {
    let newScroll = scrollY -. deltaY *. 25.0;
    onScroll(newScroll);
  };

  let element =
    <View
      style={Styles.container(bg)}
      onMouseWheel=onWheel
      onDimensionsChanged={({width, height, _}) => {
        setSize(_ => {width, height})
      }}>
      <Canvas
        style={Styles.container(bg)}
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
          Revery.Font.Smoothing.setPaint(~smoothing, textPaint);

          Skia.Paint.setLcdRenderText(textPaint, true);

          let columns = Screen.getColumns(screen);
          let rows = Screen.getTotalRows(screen);

          let renderBackground = (row, yOffset) =>
            {for (column in 0 to columns - 1) {
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
             }};
               
          let buffer = Buffer.create(4);

          let renderText = (row, yOffset) =>
            {for (column in 0 to columns - 1) {
               let cell = Screen.getCell(~row, ~column, screen);

               let fgColor = getFgColor(cell);

               Skia.Paint.setColor(textPaint, fgColor);
               Skia.Paint.setTextEncoding(textPaint, Utf8);
               let codeInt = Uchar.to_int(cell.char);
               if (codeInt !== 0) {
                 Buffer.clear(buffer);
                 Buffer.add_utf_8_uchar(buffer, cell.char);
                 let str = Buffer.contents(buffer);
                 CanvasContext.drawText(
                   ~paint=textPaint,
                   ~x=float(column) *. characterWidth,
                   ~y=yOffset +. characterHeight,
                   ~text=str,
                   canvasContext,
                 );
               }
             }};

          let perLineRenderer =
            ImmediateList.render(
              ~scrollY,
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
              ~top=
                float(scrollBackRows + cursor.row) *. lineHeight -. scrollY,
              ~width=characterWidth,
              ~height=lineHeight,
              canvasContext,
            );
          };
        }}
      />
      <View style=Styles.scrollBarContainer>
        <TerminalScrollBarView
          onScroll
          height={size.height}
          width=scrollBarThickness
          trackColor=scrollBarBackground
          thumbColor=scrollBarThumb
          scrollY
          screen
          font
        />
      </View>
    </View>;

  element;
};
