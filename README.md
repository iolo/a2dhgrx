# dhgrx

Apple //e Double Hi-Res Graphics Extension

## Features

- 140x192 logical pixels 16 colors (4bits per pixel = 2 bits in AUX + 2 bits in MAIN)
- x=0..139 -> 0..559 physical pixels
- avoid ambiguous color artifacts by using 2 bits per pixel in AUX and 2 bits per pixel in MAIN at the same time

## APIs

- init
- exit
- cls
  - color
- plot
  - x1, y1, color
- hline
  - x1, x2, y1, color
- vline
  - y1, y2, x2, color
- rect
  - x1, y1, x2, y2, color
- fillrect
  - x1, y1, x2, y2, color
- pixmap
  - x1, y1, x2, y2, src
- bitmap
  - x1, y1, x2, y2, src, color
- putc
  - x1, y1, color, ch
- puts
  - x1, y1, color, src

### Pixmap(color sprite)

- 1 byte per pixel
  - lo-nibble is color
  - hi-nibble is mask(0: transparent, others: opaque)

### Bitmap(monochrome sprite)

- 1 bit per pixel
  - 0: transparent
  - 1: opaque

### Fonts

- 7x8 bitmap(byte-aligned; MSB=0)
- 5x7 bounding box
- 8 bytes per glyph
- $20..$7F ASCII character glpyhs

## AppleSoft Basic Extension with `&` Prefix

- &GR - init
- &TEXT - exit
- &HOME - cls
- &PLOT x1,y1 - plot
- &HLIN x1,x2 AT y1 - hline
- &VLIN y1,y2 AT x1 - vline
- &R x1,y1,x2,y1 - rect
- &F x1,y1,x2,y2 - fillrect
- &XDRAW src TO x1,y1,x2,y2 - pixmap
- &DRAW src TO x1,y1,x2,y2 - bixmap
- &PRINT number - putc
- &PRINT string - puts
  - NOTE: no `;` and `,` support. use `+` for concatenation.
- &COLOR=color
- &HTAB x1
- &VTAB y1

## Demo

- [Download demo.po](demo.po) and run on your Apple //e(or later) or Emulator
- [Run on Browser with Apple2ts Emulator](https://apple2ts.com/#https://github.com/iolo/a2dhgrx/raw/refs/heads/main/demo.po)

## See Also

- [Official Technical Notes for Apple IIe Double Hi-Resolution Graphics](dhgrtechnote.txt)
- https://en.wikipedia.org/wiki/Apple_II_graphics
- https://www.appleoldies.ca/azgraphics33/index.htm#doublehi
- https://github.com/iolo/apple2-image-editor/blob/main/dhgr.mjs
- [a2dgrx](https://github.com/iolo/a2dgrx) - Apple //e Double Lo-Res Graphics Extension

---
May the **SOURCE** be with you!
