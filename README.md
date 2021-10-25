# Conway's Game of Life

This runs Conway's Game of Life.
See https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

GTK 3 is used for creating the window.
(Uses gtkmm for C++ bindings)

Uses Cairo to draw.

## Building

This uses the meson build system.

```
meson setup build
cd build
meson compile
```

This should complain appropriately if you don't have the gtkmm dev libraries
installed.
