# revery-terminal

---

A terminal emulator built with Revery.

This is a barebones terminal emulator factored out from the work to include a terminal emulator in [Onivim 2](https://github.com/onivim/oni2).

It's also a fun exploration in leveraging an Elm-style architecture in a natively compiled application.

## Building

- `npm install -g esy@0.6.2`
- `esy install`
- `esy build`

## Running

- `esy run`

## Limitations

This is just meant as an exploration, and isn't production-ready. In particular, there are some pieces missing:

- No Unicode support at the moment - assumes ASCII rendering - TODO
- No ligatures or text shaping
- No Windows support at the moment - we use the `openpty` API which is not available on Windows - TODO
- Minimal keys are handled - TODO

These would all be great contribution opportunities, though!

## Source Overview

TODO

## Technologies Used

- [Revery](https://github.com/revery-ui/revery) to provide the window management and drawing functionality.
- [reason-libvterm](https://github.com/revery-ui/reason-libvterm) bindings for [libvterm](http://www.leonerd.org.uk/code/libvterm)
- [isolinear](https://github.com/bryphe/isolinear) as an Elm-like applicatiion management framework.

We also use the [JetBrains Mono](https://www.jetbrains.com/lp/mono/) font.

## License

MIT

