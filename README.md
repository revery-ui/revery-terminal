# revery-terminal

---

A terminal emulator built with Revery.

This is a barebones terminal emulator factored out from the work to include a terminal emulator in [Onivim 2](https://github.com/onivim/oni2).

It's also a fun exploration in leveraging an Elm-style architecture in a natively compiled application.

![image](https://user-images.githubusercontent.com/13532591/74748975-3fbbf300-521e-11ea-8583-9ae1d4a18c35.png)

## Building

- `npm install -g esy@0.6.2`
- `esy install`
- `esy build`

## Running

- `esy run`

## Limitations

This is just meant as an exploration, and isn't production-ready. In particular, there are some pieces missing:

- No Unicode support at the moment - assumes ASCII rendering
- No ligatures or text shaping
- No Windows support at the moment - we use the `openpty` API which is not available on Windows
- Minimal keys are handled

These would all be great contribution opportunities, though!

## Source Overview

### Main modules

- [`src/bin/App.re`](src/bin/App.re) - this is the entry point for the application (where we start the window, wire up our store).
- [`src/bin/Renderer.re`](src/bin/Renderer.re) - the implementation for the renderer - drawing the terminal to the window. A pure function of `Model.t => Revery.element`.
- [`src/bin/Model.re`](src/bin/Model.re) - this is the heart of our application - that provides the definition of `Model.t` as well as how it changes over time (by providing an `updater` function).
- [`src/bin/Msg.re`](src/bin/Msg.re) - defines `Msg.t` - the ways our application's model can change.

### Supporting modules

- [`src/bin/Screen.rei`](src/bin/Screen.rei) - helper to manage the state of the screen
- [`src/pty`](src/pty) - small OCaml binding to the `openpty` system call, to create a pseudoterminal.

## Technologies Used

- [Revery](https://github.com/revery-ui/revery) to provide the window management and drawing functionality.
- [reason-libvterm](https://github.com/revery-ui/reason-libvterm) bindings for [libvterm](http://www.leonerd.org.uk/code/libvterm)
- [isolinear](https://github.com/bryphe/isolinear) as an Elm-like applicatiion management framework.

We also use the [JetBrains Mono](https://www.jetbrains.com/lp/mono/) font.

## License

MIT

