module Color = Revery.Color;
module Colors = Revery.Colors;
open Revery.Draw;
open Revery.UI;
open Revery.UI.Components;

module Styles = {
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

let make =
    (~onScroll, ~height, ~scrollY, ~screen: Screen.t, ~font: Font.t, ()) => {
  let totalRows = Screen.getTotalRows(screen);
  let screenRows = Screen.getScreenRows(screen);

  let totalSize = float(totalRows) *. font.lineHeight;
  let screenSize = float(screenRows) *. font.lineHeight;

  let pixelHeight = float_of_int(height);

  let thumbLength = int_of_float(screenSize /. totalSize *. pixelHeight);

  let maximumValue = totalSize -. screenSize;

  <Slider
    onValueChanged={v => onScroll(v)}
    vertical=true
    sliderLength=height
    thumbLength
    trackThickness=Styles.scrollBarWidth
    thumbThickness=Styles.scrollBarWidth
    minimumValue=0.
    maximumValue
    value=scrollY
  />;
};
