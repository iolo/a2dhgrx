#include <apple2.h>
#include <conio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <em.h>

#define HGR_PAGE1_BASE 0x2000
#define HGR_PAGE2_BASE 0x4000
#define HGR_BASE HGR_PAGE1_BASE
#define HGR_WIDTH 140 // 280 physical pixel -> 140 logical pixels
#define HGR_HEIGHT 192

// Apple II hi-res base offset of each line(0..191)
// Apple II HGR interleaves 8-line blocks across 3 sections.
// ((y % 8) * 1024) % 8192 + ((y / 8) % 2) * 128
// prettier-ignore
const uint16_t HGR_OFFSET[HGR_HEIGHT] = {
    // section 0
    0x000,
    0x400,
    0x800,
    0xc00,
    0x1000,
    0x1400,
    0x1800,
    0x1c00, // block 0
    0x080,
    0x480,
    0x880,
    0xc80,
    0x1080,
    0x1480,
    0x1880,
    0x1c80, // block 1
    0x100,
    0x500,
    0x900,
    0xd00,
    0x1100,
    0x1500,
    0x1900,
    0x1d00, // block 2
    0x180,
    0x580,
    0x980,
    0xd80,
    0x1180,
    0x1580,
    0x1980,
    0x1d80, // block 3
    0x200,
    0x600,
    0xa00,
    0xe00,
    0x1200,
    0x1600,
    0x1a00,
    0x1e00, // block 4
    0x280,
    0x680,
    0xa80,
    0xe80,
    0x1280,
    0x1680,
    0x1a80,
    0x1e80, // block 5
    0x300,
    0x700,
    0xb00,
    0xf00,
    0x1300,
    0x1700,
    0x1b00,
    0x1f00, // block 6
    0x380,
    0x780,
    0xb80,
    0xf80,
    0x1380,
    0x1780,
    0x1b80,
    0x1f80, // block 7
    // section 1
    0x028,
    0x428,
    0x828,
    0xc28,
    0x1028,
    0x1428,
    0x1828,
    0x1c28, // block 0
    0x0a8,
    0x4a8,
    0x8a8,
    0xca8,
    0x10a8,
    0x14a8,
    0x18a8,
    0x1ca8, // block 1
    0x128,
    0x528,
    0x928,
    0xd28,
    0x1128,
    0x1528,
    0x1928,
    0x1d28, // block 2
    0x1a8,
    0x5a8,
    0x9a8,
    0xda8,
    0x11a8,
    0x15a8,
    0x19a8,
    0x1da8, // block 3
    0x228,
    0x628,
    0xa28,
    0xe28,
    0x1228,
    0x1628,
    0x1a28,
    0x1e28, // block 4
    0x2a8,
    0x6a8,
    0xaa8,
    0xea8,
    0x12a8,
    0x16a8,
    0x1aa8,
    0x1ea8, // block 5
    0x328,
    0x728,
    0xb28,
    0xf28,
    0x1328,
    0x1728,
    0x1b28,
    0x1f28, // block 6
    0x3a8,
    0x7a8,
    0xba8,
    0xfa8,
    0x13a8,
    0x17a8,
    0x1ba8,
    0x1fa8, // block 7
    // section 2
    0x050,
    0x450,
    0x850,
    0xc50,
    0x1050,
    0x1450,
    0x1850,
    0x1c50, // block 0
    0x0d0,
    0x4d0,
    0x8d0,
    0xcd0,
    0x10d0,
    0x14d0,
    0x18d0,
    0x1cd0, // block 1
    0x150,
    0x550,
    0x950,
    0xd50,
    0x1150,
    0x1550,
    0x1950,
    0x1d50, // block 2
    0x1d0,
    0x5d0,
    0x9d0,
    0xdd0,
    0x11d0,
    0x15d0,
    0x19d0,
    0x1dd0, // block 3
    0x250,
    0x650,
    0xa50,
    0xe50,
    0x1250,
    0x1650,
    0x1a50,
    0x1e50, // block 4
    0x2d0,
    0x6d0,
    0xad0,
    0xed0,
    0x12d0,
    0x16d0,
    0x1ad0,
    0x1ed0, // block 5
    0x350,
    0x750,
    0xb50,
    0xf50,
    0x1350,
    0x1750,
    0x1b50,
    0x1f50, // block 6
    0x3d0,
    0x7d0,
    0xbd0,
    0xfd0,
    0x13d0,
    0x17d0,
    0x1bd0,
    0x1fd0, // block 7
};

#define DHGR_WIDTH 140 // 560 physical pixels -> 140 logical pixels
#define DHGR_HEIGHT HGR_HEIGHT
#define DHGR_COLORS 16

// framebuffer
uint8_t *fb = (uint8_t *)HGR_PAGE1_BASE;

// DHGR memory layout:
//
// 80 bytes per row(40 in AUX, 40 in MAIN), 7 color-pixels per 4 bytes, total
// 140 color-pixels per row.
//
// |bank      | AUX        |MAIN         | AUX         | MAIN       |   |
// |----------|------------|-------------+-------------|------------|---|
// |address   | $2000      |$2000        | $2001       | $2001      |...|
// |----------|------------|-------------+-------------|------------|---|
// |bit offset| 7 654 3210 | 7 65 4321 0 | 7 6 5432 10 | 7 6543 210 |...|
// |pixel     | - BBB AAAA | - DD CCCC B | - F EEEE DD | - GGGG FFF |...|
// |pixel bit | - 123 0123 | - 23 0123 0 | - 3 0123 01 | - 0123 012 |...|
// |x         |   1   0    |   3  2    1 |   5 4    3  |   6    5   |...|

// pre-calculated 4-bytes for 7 color-pixels with the same color
//
// byte0 = ((color & 0b0111) << 4) | color;
// byte1 = ((color & 0b0011) << 5) | (color << 1) | ((color & 0b1000) >> 3);
// byte2 = ((color & 0b0001) << 6) | (color << 2) | ((color & 0b1100) >> 2);
// byte3 = (color << 3) | ((color & 0b1110) >> 1);
//
// color=0000 | 0 000 0000 | 0 00 0000 0 | 0 0 0000 00 | 0 0000 000 |
// color=0001 | 0 001 0001 | 0 01 0001 0 | 0 1 0001 00 | 0 0001 000 |
// color=0010 | 0 010 0010 | 0 10 0010 0 | 0 0 0010 00 | 0 0010 001 |
// color=0011 | 0 011 0011 | 0 11 0011 0 | 0 1 0011 00 | 0 0011 001 |
// color=0100 | 0 100 0100 | 0 00 0100 0 | 0 0 0100 01 | 0 0100 010 |
// color=0101 | 0 101 0101 | 0 01 0101 0 | 0 1 0101 01 | 0 0101 010 |
// color=0110 | 0 110 0110 | 0 10 0110 0 | 0 0 0110 01 | 0 0110 011 |
// color=0111 | 0 111 0111 | 0 11 0111 0 | 0 1 0111 01 | 0 0111 011 |
// color=1000 | 0 000 1000 | 0 00 1000 1 | 0 0 1000 10 | 0 1000 100 |
// color=1001 | 0 001 1001 | 0 01 1001 1 | 0 1 1001 10 | 0 1001 100 |
// color=1010 | 0 010 1010 | 0 10 1010 1 | 0 0 1010 10 | 0 1010 101 |
// color=1011 | 0 011 1011 | 0 11 1011 1 | 0 1 1011 10 | 0 1011 101 |
// color=1100 | 0 100 1100 | 0 00 1100 1 | 0 0 1100 11 | 0 1100 110 |
// color=1101 | 0 101 1101 | 0 01 1101 1 | 0 1 1101 11 | 0 1101 110 |
// color=1110 | 0 110 1110 | 0 10 1110 1 | 0 0 1110 11 | 0 1110 111 |
// color=1111 | 0 111 1111 | 0 11 1111 1 | 0 1 1111 11 | 0 1111 111 |
const uint8_t DHGR_FILL_BYTES[DHGR_COLORS][4] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000},
    {0b00010001, 0b00100010, 0b01000100, 0b00001000},
    {0b00100010, 0b01000100, 0b00001000, 0b00010001},
    {0b00110011, 0b01100110, 0b01001100, 0b00011001},
    {0b01000100, 0b00001000, 0b00010001, 0b00100010},
    {0b01010101, 0b00101010, 0b01010101, 0b00101010},
    {0b01100110, 0b01001100, 0b00011001, 0b00110011},
    {0b01110111, 0b01101110, 0b01011101, 0b00111011},
    {0b00001000, 0b00010001, 0b00100010, 0b01000100},
    {0b00011001, 0b00110011, 0b01100110, 0b01001100},
    {0b00101010, 0b01010101, 0b00101010, 0b01010101},
    {0b00111011, 0b01110111, 0b01101110, 0b01011101},
    {0b01001100, 0b00011001, 0b00110011, 0b01100110},
    {0b01011101, 0b00111011, 0b01110111, 0b01101110},
    {0b01101110, 0b01011101, 0b00111011, 0b01110111},
    {0b01111111, 0b01111111, 0b01111111, 0b01111111},
};

// pixel x -> byte x
// pixel 0..6 -> byte 0..1, pixel 17..13 -> byte 2..3, ... pixel 138..139 ->
// byte 38..39 x / 2 / 7 prettier-ignore
const uint8_t DHGR_OFFSET_X[DHGR_WIDTH] = {
    0,  0,  0,  0,  0,  0,  0,  // pixels 0..6
    2,  2,  2,  2,  2,  2,  2,  // pixels 7..13
    4,  4,  4,  4,  4,  4,  4,  // pixels 14..20
    6,  6,  6,  6,  6,  6,  6,  // pixels 21..27
    8,  8,  8,  8,  8,  8,  8,  // pixels 28..34
    10, 10, 10, 10, 10, 10, 10, // pixels 35..41
    12, 12, 12, 12, 12, 12, 12, // pixels 42..48
    14, 14, 14, 14, 14, 14, 14, // pixels 49..55
    16, 16, 16, 16, 16, 16, 16, // pixels 56..62
    18, 18, 18, 18, 18, 18, 18, // pixels 63..69
    20, 20, 20, 20, 20, 20, 20, // pixels 70..76
    22, 22, 22, 22, 22, 22, 22, // pixels 77..83
    24, 24, 24, 24, 24, 24, 24, // pixels 84..90
    26, 26, 26, 26, 26, 26, 26, // pixels 91..97
    28, 28, 28, 28, 28, 28, 28, // pixels 98..104
    30, 30, 30, 30, 30, 30, 30, // pixels 105..111
    32, 32, 32, 32, 32, 32, 32, // pixels 112..118
    34, 34, 34, 34, 34, 34, 34, // pixels 119..125
    36, 36, 36, 36, 36, 36, 36, // pixels 126..132
    38, 38, 38, 38, 38, 38, 38, // pixels 133..139
};

// color
// 0..139 -> 0..39
#define DHGR_PIXEL_OFFSET(x, y) (HGR_OFFSET[y] + DHGR_OFFSET_X[x])

enum DhgrColor {
  BLACK   = 0, // 0000
  DKBLUE  = 1, // 0001
  DKGREEN = 2, // 0010
  BLUE    = 3, // 0011
  BROWN   = 4, // 0100
  LTGRAY  = 5, // 0101
  GREEN   = 6, // 0110
  AQUA    = 7, // 0111
  RED     = 8, // 1000
  PURPLE  = 9, // 1001
  DKGRAY  = 10,// 1010
  LTBLUE  = 11,// 1011
  ORANGE  = 12,// 1100
  PINK    = 13,// 1101
  YELLOW  = 14,// 1110
  WHITE   = 15 // 1111
};

const char *DHGR_COLOR_NAMES[DHGR_COLORS] = {
    "BLACK",  "DKBLUE", "DKGREEN", "BLUE",   "BROWN",  "LTGRAY",
    "GREEN",  "AQUA",   "RED",     "PURPLE", "DKGRAY", "LTBLUE",
    "ORANGE", "PINK",   "YELLOW",  "WHITE",
};

void dhgr_init() {
  __asm__("sta $c050"); // graphics
  __asm__("sta $c053"); // mixed
  //__asm__("sta $c052"); // full
  __asm__("sta $c054"); // page 1
  //__asm__("sta $c055"); // page 2
  __asm__("sta $c057"); // hi-res
  __asm__("sta $c05e"); // an3 on
}

void dhgr_exit() {
  __asm__("sta $c051"); // text
  __asm__("sta $c05f"); // an3 off
  __asm__("sta $c054"); // page 1
}

#define AUX_BANK() __asm__("sta $c055")
#define MAIN_BANK() __asm__("sta $c054")

void __fastcall__ main2aux(uint16_t src0, uint16_t src1, uint16_t dst0) {
  uint16_t *src = (uint16_t *)0x3c;
  uint16_t *dst = (uint16_t *)0x42;
  src[0] = src0;
  src[1] = src1;
  dst[0] = dst0;
  __asm__("sec");
  __asm__("jsr $c311");
}

void __fastcall__ aux2main(uint16_t src0, uint16_t src1, uint16_t dst0) {
  uint16_t *src = (uint16_t *)0x3c;
  uint16_t *dst = (uint16_t *)0x42;
  src[0] = src0;
  src[1] = src1;
  dst[0] = dst0;
  __asm__("clc");
  __asm__("jsr $c311");
}

void __fastcall__ memset2(uint8_t *dst, uint8_t hi, uint8_t lo,
                          uint16_t count) {
  while (--count) {
    *dst++ = hi;
    *dst++ = lo;
  }
}

void cls(uint8_t color) {
  register uint8_t *ptr;
  register uint8_t x, y;
  register uint8_t byte0, byte1, byte2, byte3;

  byte0 = DHGR_FILL_BYTES[color][0];
  byte1 = DHGR_FILL_BYTES[color][1];
  byte2 = DHGR_FILL_BYTES[color][2];
  byte3 = DHGR_FILL_BYTES[color][3];

  for (y = 0; y < DHGR_HEIGHT; y++) {
    ptr = (uint8_t *)(HGR_BASE + HGR_OFFSET[y]);
    for (x = 0; x < 40; x += 2) {
      AUX_BANK();
      *(ptr + x) = byte0;
      *(ptr + x + 1) = byte2;
      MAIN_BANK();
      *(ptr + x) = byte1;
      *(ptr + x + 1) = byte3;
    }
  }
}

// put 7 color-pixels in a 4-byte group, starting at (x, y), x must be multiple
// of 7
void __fastcall__ plot7(uint8_t x, uint8_t y, uint8_t color) {
  register uint8_t *ptr =
      (uint8_t *)(HGR_BASE + HGR_OFFSET[y] + DHGR_OFFSET_X[x]);

  AUX_BANK();
  *ptr = DHGR_FILL_BYTES[color][0];
  *(ptr + 1) = DHGR_FILL_BYTES[color][2];
  MAIN_BANK();
  *ptr = DHGR_FILL_BYTES[color][1];
  *(ptr + 1) = DHGR_FILL_BYTES[color][3];
}

// TODO: optimize
void __fastcall__ plot(uint8_t x, uint8_t y, uint8_t color) {
  register uint8_t *ptr =
      (uint8_t *)(HGR_BASE + HGR_OFFSET[y] + DHGR_OFFSET_X[x]);

  switch (x % 7) {
  case 0: // A
    AUX_BANK();
    *ptr = (*ptr & 0b11110000) | (color & 0b1111);
    MAIN_BANK();
    break;
  case 1: // B
    AUX_BANK();
    *ptr = (*ptr & 0b10001111) | ((color << 4) & 0b01110000);
    MAIN_BANK();
    *ptr = (*ptr & 0b11111110) | ((color >> 3) & 0b00000001);
    break;
  case 2: // C
    *ptr = (*ptr & 0b11100001) | ((color << 1) & 0b00011110);
    break;
  case 3: // D
    *ptr = (*ptr & ~0b01100000) | ((color & 0b0011) << 5);
    AUX_BANK();
    *(ptr + 1) = (*(ptr + 1) & 0b11111100) | ((color >> 2) & 0b00000011);
    MAIN_BANK();
    break;
  case 4: // E
    AUX_BANK();
    *(ptr + 1) = (*(ptr + 1) & 0b11000011) | ((color << 2) & 0b00111100);
    MAIN_BANK();
    break;
  case 5: // F
    AUX_BANK();
    *(ptr + 1) = (*(ptr + 1) & 0b10111111) | ((color << 6) & 0b01000000);
    MAIN_BANK();
    *(ptr + 1) = (*(ptr + 1) & 0b11111000) | ((color >> 1) & 0b00000111);
    break;
  case 6: // G
    *(ptr + 1) = (*(ptr + 1) & 0b10000111) | ((color << 3) & 0b01111000);
    break;
  }
}

uint8_t __fastcall__ scrn(uint8_t x, uint8_t y) {
  register uint8_t *ptr =
      (uint8_t *)(HGR_BASE + HGR_OFFSET[y] + DHGR_OFFSET_X[x]);
  register uint8_t byte0, byte1, byte2, byte3;

  switch (x % 7) {
  case 0: // A
    AUX_BANK();
    byte0 = *ptr;
    MAIN_BANK();
    return byte0 & 0b00001111;
  case 1: // B
    AUX_BANK();
    byte0 = *ptr;
    MAIN_BANK();
    byte1 = *ptr;
    return ((byte0 & 0b01110000) >> 4) | ((byte1 & 0b00000001) << 3);
  case 2: // C
    byte1 = *ptr;
    return (byte1 & 0b00011110) >> 1;
  case 3: // D
    byte1 = *ptr;
    AUX_BANK();
    byte2 = *(ptr + 1);
    MAIN_BANK();
    return ((byte1 & 0b01100000) >> 5) | ((byte2 & 0b00000011) << 2);
  case 4: // E
    AUX_BANK();
    byte2 = *(ptr + 1);
    MAIN_BANK();
    return (byte2 & 0b00111100) >> 2;
  case 5: // F
    AUX_BANK();
    byte2 = *(ptr + 1);
    MAIN_BANK();
    byte3 = *(ptr + 1);
    return ((byte2 & 0b01000000) >> 6) | ((byte3 & 0b00000111) << 1);
  case 6: // G
    byte3 = *(ptr + 1);
    return (byte3 & 0b01111000) >> 3;
  }
}

void __fastcall__ hline(uint8_t x0, uint8_t x1, uint8_t y, uint8_t color) {
  if (x0 > x1) {
    uint8_t tmp = x0;
    x1 = x0;
    x0 = tmp;
  }

  // shorter than 7 pixels, use dhgr_plot to draw each pixel(slow)
  // ex. hline 1..6
  if (x1 - x0 < 7) {
    while (x0 <= x1) {
      plot(x0++, y, color);
    }
    return;
  }

  // longer than 7 pixels, split into 3 parts: left(slow), middle(fast),
  // right(slow) ex. 5..22 -> 5..6, 7..20, 21..22

  // left
  while (x0 % 7) {
    plot(x0++, y, color);
  }

  // middle
  while (x0 + 7 <= x1) {
    plot7(x0, y, color);
    x0 += 7;
  }

  // right
  while (x0 <= x1) {
    plot(x0++, y, color);
  }
}

void __fastcall__ vline(uint8_t x, uint8_t y0, uint8_t y1, uint8_t color) {
  uint8_t y;
  if (y0 > y1) {
    uint8_t tmp = y0;
    y1 = y0;
    y0 = tmp;
  }
  for (y = y0; y <= y1; y += 1) {
    plot(x, y, color);
  }
}

void rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
  hline(x0, x1, y0, color);
  hline(x0, x1, y1, color);
  vline(x0, y0 + 1, y1 - 1, color);
  vline(x1, y0 + 1, y1 - 1, color);
}

void fillrect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
  uint8_t y;
  for (y = y0; y <= y1; y += 1) {
    hline(x0, x1, y, color);
  }
}

void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
  int dx, dy, sx, sy, err, e2;
  if (x0 == x1) {
    vline(x0, y0, y1, color);
    return;
  }
  if (y0 == y1) {
    hline(x0, x1, y0, color);
    return;
  }
  dx = abs(x1 - x0);
  sx = x0 < x1 ? 1 : -1;
  dy = -abs(y1 - y0);
  sy = y0 < y1 ? 1 : -1;
  err = dx + dy;
  while (1) {
    plot(x0, y0, color);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void ellipse(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
  int a, b, xc, yc, dx, dy, a2, b2, err;
  if (x0 == x1) {
    vline(x0, y0, y1, color);
    return;
  }
  if (y0 == y1) {
    hline(x0, x1, y0, color);
    return;
  }
  if (x0 > x1) {
    uint8_t tmp = x0;
    x1 = x0;
    x0 = tmp;
  }
  if (y0 > y1) {
    uint8_t tmp = y0;
    y1 = y0;
    y0 = tmp;
  }
  a = (x1 - x0) / 2;
  b = (y1 - y0) / 2;
  xc = x0 + a;
  yc = y0 + b;
  dx = 0;
  dy = b;
  a2 = a * a;
  b2 = b * b;
  err = b2 - (2 * b - 1) * a2;
  do {
    plot(xc + dx, yc + dy, color);
    plot(xc - dx, yc + dy, color);
    plot(xc - dx, yc - dy, color);
    plot(xc + dx, yc - dy, color);
    if (err < 0) {
      err += b2 * (2 * dx + 3);
    } else if (err > 0) {
      err += a2 * (-2 * dy + 3);
      dy--;
    } else {
      err += b2 * (2 * dx + 3) + a2 * (-2 * dy + 3);
      dy--;
    }
    dx++;
  } while (dy >= 0);
}

// TODO: optimize
void floodfill(uint8_t x, uint8_t y, uint8_t color) {
  uint8_t target_color;

  target_color = scrn(x, y);
  if (target_color == color)
    return;

  plot(x, y, color);

  if (x > 0 && scrn(x - 1, y) == target_color)
    floodfill(x - 1, y, color);
  if (x < DHGR_WIDTH - 1 && scrn(x + 1, y) == target_color)
    floodfill(x + 1, y, color);
  if (y > 0 && scrn(x, y - 1) == target_color)
    floodfill(x, y - 1, color);
  if (y < DHGR_HEIGHT - 1 && scrn(x, y + 1) == target_color)
    floodfill(x, y + 1, color);
}

void cls_test() {
  uint8_t c;
  for (c = 0; c < DHGR_COLORS; c++) {
    cputsxy(0, 21, DHGR_COLOR_NAMES[c]);
    cls(c);
  }
}

void plot_test() {
  uint16_t i, x, y, c;
  cls(BLACK);
  for (i = 0; i < 140; i += 1) {
    x = i; // rand() % DHGR_WIDTH;
    y = i; // rand() % DHGR_HEIGHT;
    c = rand() % DHGR_COLORS;
    plot(x, y, c);
  }
}

void hline_test() {
  uint16_t y;
  cls(BLACK);
  for (y = 0; y < DHGR_HEIGHT; y += 1) {
    hline(3, y % DHGR_WIDTH, y, y % DHGR_COLORS);
  }
}

void vline_test() {
  uint16_t x;
  cls(BLACK);
  for (x = 0; x < DHGR_WIDTH; x += 1) {
    vline(x, 0, x % DHGR_HEIGHT, x % DHGR_COLORS);
  }
}

void rect_test() {
  uint16_t i, x0, y0, x1, y1;
  cls(BLACK);
  for (i = 0; i < 50; i += 1) {
    x0 = rand() % (DHGR_WIDTH - 50);
    y0 = rand() % (DHGR_HEIGHT - 50);
    x1 = x0 + rand() % 50;
    y1 = y0 + rand() % 50;
    rect(x0, y0, x1, y1, i % DHGR_COLORS);
  }
}

void fillrect_test() {
  uint16_t i, x0, y0, x1, y1;
  cls(BLACK);
  for (i = 0; i < 50; i += 1) {
    x0 = rand() % (DHGR_WIDTH - 50);
    y0 = rand() % (DHGR_HEIGHT - 50);
    x1 = x0 + rand() % 50;
    y1 = y0 + rand() % 50;
    fillrect(x0, y0, x1, y1, i % DHGR_COLORS);
  }
}

void load_image_file(char *filename) {
  int fd;
  fd = open(filename, O_RDONLY);
  AUX_BANK();
  read(fd, (void *)0x2000, 0x2000);
  MAIN_BANK();
  read(fd, (void *)0x2000, 0x2000);
  close(fd);
}

void image_test() {
  load_image_file("image1.dhgr");
}

#define ASC_BEGIN 0x20
#define KOR_BEGIN 0xAC00
#define NUM_CHO 19
#define NUM_JUNG 21
#define NUM_JONG 28 // with filler
#define CHO_KIND 3
#define JUNG_KIND 1
#define JONG_KIND 1
#define FONT_ASC_WIDTH 7
#define FONT_ASC_WIDTH_BYTE 1
#define FONT_KOR_WIDTH 14
#define FONT_KOR_WIDTH_BYTE 2
#define FONT_HEIGHT 12

#define TEXT_LINE_HEIGHT FONT_HEIGHT
#define TEXT_COLS (560 / FONT_ASC_WIDTH)
#define TEXT_ROWS (192 / TEXT_LINE_HEIGHT)

#define NUM_ASC_FONT (0x7f - 0x20)
#define NUM_HAN_FONT                                                           \
  (NUM_CHO * CHO_KIND + NUM_JUNG * JUNG_KIND + NUM_JONG * JONG_KIND)

#include "7x12.c"

#include "ko.c"

const uint8_t cho_kind_by_jung[NUM_JONG] = {
    // ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ
    0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 1, 2, 0,
};

void __fastcall__ putbyte(uint8_t x, uint8_t y, uint8_t c) {
  register uint8_t *ptr = (uint8_t *)(HGR_BASE + HGR_OFFSET[y] + (x >> 1));
  if ((x & 1) == 0)
    AUX_BANK();
  *ptr = c;
  MAIN_BANK();
}

void __fastcall__ putbyte_xor(uint8_t x, uint8_t y, uint8_t c) {
  register uint8_t *ptr = (uint8_t *)(HGR_BASE + HGR_OFFSET[y] + (x >> 1));
  if ((x & 1) == 0)
    AUX_BANK();
  *ptr ^= c;
  MAIN_BANK();
}

void putasc(uint8_t x, uint8_t y, uint8_t *font) {
  register uint8_t i;
  for (i = 0; i < FONT_HEIGHT; i++) {
    putbyte_xor(x, y + i, font[i]);
  }
}

void puthan(uint8_t x, uint8_t y, uint8_t *cho, uint8_t *jung, uint8_t *jong) {
  register uint8_t i, p, byte1, byte2;
  for (i = 0, p = 0; i < FONT_HEIGHT; i++, p += 2) {
    if (cho) {
      byte1 = cho[p];
      byte2 = cho[p + 1];
    }
    if (jung) {
      byte1 |= jung[p];
      byte2 |= jung[p + 1];
    }
    if (jong) {
      byte1 |= jong[p];
      byte2 |= jong[p + 1];
    }
    putbyte_xor(x, y + i, byte1);
    putbyte_xor(x + 1, y + i, byte2);
  }
}

int8_t putucs2(uint8_t x, uint8_t y, uint16_t code) {
  uint8_t cho, jung, jong, cho_index, jung_index, jong_index;
  if (code < 0x20) {
    return 0;
  }
  if (code < 0x80) {
    putasc(x, y, asc[code - ASC_BEGIN]);
    return 1;
  }
  decomposeHangul(code, &cho, &jung, &jong);
  cho_index = NUM_CHO * cho_kind_by_jung[jung] + cho;
  jung_index = CHO_KIND * NUM_CHO + jung;
  jong_index = CHO_KIND * NUM_CHO + JUNG_KIND * NUM_JUNG + jong;
  puthan(x, y, han[cho_index], han[jung_index], han[jong_index]);
  return 2;
}

void putucs2str(uint8_t x, uint8_t y, uint16_t *str) {
  uint16_t *p = str;
  while (*p != 0) {
    x += putucs2(x, y, *p++);
    if (x > TEXT_COLS) {
      x = 0;
      y += TEXT_LINE_HEIGHT;
      if (y > DHGR_HEIGHT) {
        // TODO: scroll? wrap? or stop?
        break;
      }
    }
  }
}

void pututf8str(uint8_t x, uint8_t y, uint8_t *str) {
  uint8_t *p = str;
  uint8_t byte1, byte2, byte3, byte4;
  uint16_t code;
  while (*p != 0) {
    byte1 = *p++;
    if (byte1 < 0b10000000) {
      // 0xxxxxxx -> 0x00..0x7f
      code = byte1;
    } else if (byte1 < 0b11000000) {
      byte2 = *p++;
      // 110xxxxx 10xxxxxx -> 0x80..0x07ff (11bits)
      code = ((byte1 & 0b00011111) << 6) | (byte2 & 0b00111111);
    } else if (byte1 < 0b11110000) {
      byte2 = *p++;
      byte3 = *p++;
      // 1110xxxx 10xxxxxx 10xxxxxx -> 0x800..0xffff (16bits)
      code = ((byte1 & 0b00001111) << 12) | ((byte2 & 0b00111111) << 6) |
             (byte3 & 0b00111111);
    } else if (byte1 < 0b11111000) {
      byte2 = *p++;
      byte3 = *p++;
      byte4 = *p++;
      // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx -> 0x10000..0x10ffff (21bits)
      // code = ((byte1 & 0b00000111) << 18) | ((byte2 & 0b00111111) << 12) |
      // ((byte3 & 0b00111111) << 6) | (byte4 & 0b00111111);
      // unsupported!
      code = 0x7f;
    } else {
      // 10xxxxxx -> 0x80..0xbf???
      // unsupported!
      code = 0x7f;
    }
    // utf8 -> ucs2 -> split cho+jung+jong -> put
    x += putucs2(x, y, code);
    if (x > TEXT_COLS) {
      x = 0;
      y += TEXT_LINE_HEIGHT;
      if (y > DHGR_HEIGHT) {
        // TODO: scroll? wrap? or stop?
        break;
      }
    }
  }
}

uint16_t ucs2_sample[] = {
    'H',    'e',    'l',    'l',    'o',    ',',    'W',    'o',  'r',
    'l',    'd',    '!',    '1',    '2',    '3',    '4',    '5',  '6',
    '7',    '8',    '9',    '0',    ' ',    'A',    'B',    'C',  'D',
    'E',    'F',    'G',    'H',    'I',    'J',    'K',    'L',  'M',
    'N',    'O',    'P',    'Q',    'R',    'S',    'T',    'U',  'V',
    'E',    'X',    'Y',    'Z',    ' ',    'a',    'b',    'c',  'd',
    'e',    'f',    'g',    'h',    'i',    'j',    'k',    'l',  'm',
    'n',    'o',    'p',    'q',    'r',    's',    't',    'u',  'v',
    'w',    'x',    'y',    'z',    ' ',    '!',    '@',    '#',  '$',
    '%',    '^',    '&',    '*',    '(',    ')',    '-',    '=',  '+',
    '[',    ']',    '{',    '}',    ';',    ':',    '\'',   '"',  ',',
    '.',    '/',    '?',    ' ',    0xd55c, 0xae00, 0xc740, 0x20, 0xc544,
    0xb984, 0xb2f5, 0xb2e4, 0x20,   0xadf8, 0xb7ec, 0xb098, 0x20, 0xace8,
    0xce58, 0xc544, 0xd504, 0xb2e4, 0x2e,   0,
};

uint8_t *utf8_sample =
    "Hello,World! ABCDEFGHIJKLMNOPQRSTUVEXYZ abcdefghijklmnopqrstuvwxyz "
    "1234567890 !@#$%^&*()-=_+[]{};:<>,./? 한글은 아름답다 그러나 골치아프다!";
void text_test() {
  cls(BLACK);
  putucs2str(0, 0, (uint16_t *)ucs2_sample);
  putucs2str(0, 30, (uint16_t *)ucs2_sample);
  pututf8str(0, 60, utf8_sample);
  pututf8str(0, 90, utf8_sample);
  cgetc();
  cls(WHITE);
  putucs2str(0, 0, (uint16_t *)ucs2_sample);
  putucs2str(0, 30, (uint16_t *)ucs2_sample);
  pututf8str(0, 60, utf8_sample);
  pututf8str(0, 90, utf8_sample);
}

void main() {
  /*
  char c;

  while (1) {
    clrscr();
    cputsxy(0, 0, "DHGR TEST");
    cputsxy(0, 1, "1. CLS");
    cputsxy(0, 2, "2. PLOT");
    cputsxy(0, 3, "3. HLINE");
    cputsxy(0, 4, "4. VLINE");
    cputsxy(0, 5, "5. RECT");
    cputsxy(0, 6, "6. FILLRECT");
    cputsxy(0, 7, "7. IMAGE");
    cputsxy(0, 8, "8. TEXT");
    cputsxy(0, 9, "Q. QUIT");

    c = cgetc();
    if (c == 'q' || c == 'Q') {
      dhgr_exit();
      return;
    }

    dhgr_init();
    switch (c) {
    case '1':
      cputsxy(0, 20, "CLS TEST");
      cls_test();
      break;
    case '2':
      cputsxy(0, 20, "PLOT TEST");
      plot_test();
      break;
    case '3':
      cputsxy(0, 20, "HLINE TEST");
      hline_test();
      break;
    case '4':
      cputsxy(0, 20, "VLINE TEST");
      vline_test();
      break;
    case '5':
      cputsxy(0, 20, "RECT TEST");
      rect_test();
      break;
    case '6':
      cputsxy(0, 20, "FILLRECT TEST");
      fillrect_test();
      break;
    case '7':
      cputsxy(0, 20, "IMAGE TEST");
      image_test();
      break;
    case '8':
      cputsxy(0, 20, "TEXT TEST");
      text_test();
      break;
    }

    cputsxy(0, 20, "PRESS ANY KEY TO CONTINUE...");
    cgetc();
    dhgr_exit();
  }
  */
  register uint8_t y;
  register uint8_t* addr;

// |bank      | AUX        |MAIN         | AUX         | MAIN       |   |
// |----------|------------|-------------+-------------|------------|---|
// |address   | $2000      |$2000        | $2001       | $2001      |...|
// |----------|------------|-------------+-------------|------------|---|
// |bit offset| 7 654 3210 | 7 65 4321 0 | 7 6 5432 10 | 7 6543 210 |...|
// |pixel     | - BBB AAAA | - DD CCCC B | - F EEEE DD | - GGGG FFF |...|
// |pixel bit | - 123 0123 | - 23 0123 0 | - 3 0123 01 | - 0123 012 |...|
// |x         |   1   0    |   3  2    1 |   5 4    3  |   6    5   |...|
  dhgr_init();
  cls(BLACK);
  for (y = 0; y < 192; y++) {
    addr = HGR_BASE + HGR_OFFSET[y];
    AUX_BANK();
    *addr = y & 0b0001111;
    MAIN_BANK();
  }
  cgetc();
  cls(WHITE);
  for (y = 0; y < 192; y++) {
    addr = HGR_BASE + HGR_OFFSET[y];
    AUX_BANK();
    *addr = y & 0b0001111;
    MAIN_BANK();
  }
  cgetc();
  dhgr_exit();
}
