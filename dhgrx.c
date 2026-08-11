#define DHGR_WIDTH 140
#define DHGR_HEIGHT 192

typedef enum {
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
} dhgr_color_t;


#define DG 0x6000
#define X1 0xfa
#define Y1 0xfb
#define X2 0xfc
#define Y2 0xfd
#define COLOR 0xfe
#define CH 0xff
#define SRC 0xce
#define DST 0xeb

#define POKE(addr, val) *((uint8_t*)addr) = val
#define PEEK(addr) *((uint8_t*)addr)
#define CALL(addr) __asm__("jsr %w", addr)

void __fastcall__ dhgr_init() {
  CALL(DG);
}

void __fastcall__ dhgr_exit() {
  CALL(DG+3);
}

void __fastcall__ dhgr_cls(uint8_t color) {
  POKE(COLOR, color);
  CALL(DG+6);
}

void __fastcall__ dhgr_plot(uint8_t x, uint8_t y, uint8_t c) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, c);
  CALL(DG+9);
}

void __fastcall__ dhgr_hline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(COLOR, c);
  CALL(DG+12);
}

void __fastcall__ dhgr_vline(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(Y2, y2);
  POKE(COLOR, c);
  CALL(DG+15);
}

void __fastcall__ dhgr_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(COLOR, c);
  CALL(DG+18);
}

void __fastcall__ dhgr_fillrect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(COLOR, c);
  CALL(DG+21);
}

void __fastcall__ dhgr_pixmap(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(SRC, (uint16_t)data & 0xff);
  POKE(SRC+1, ((uint16_t)data >> 8) & 0xff);
  CALL(DG+24);
}

void __fastcall__ dhgr_bitmap(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, const uint8_t* data) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(COLOR, color);
  POKE(SRC, (uint16_t)data & 0xff);
  POKE(SRC+1, ((uint16_t)data >> 8) & 0xff);
  CALL(DG+27);
}

void __fastcall__ dhgr_puts(uint8_t x, uint8_t y, uint8_t color, const char* data) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, color);
  POKE(SRC, (uint16_t)data & 0xff);
  POKE(SRC+1, ((uint16_t)data >> 8) & 0xff);
  CALL(DG+33);
}

void __fastcall__ dhgr_puts2(uint8_t x, uint8_t y, uint8_t color, const uint16_t* data) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, color);
  POKE(SRC, (uint16_t)data & 0xff);
  POKE(SRC+1, ((uint16_t)data >> 8) & 0xff);
  CALL(DG+39);
}

void __fastcall__ dhgr_puts_utf8(uint8_t x, uint8_t y, uint8_t color, const char* data) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, color);
  POKE(SRC, (uint16_t)data & 0xff);
  POKE(SRC+1, ((uint16_t)data >> 8) & 0xff);
  CALL(DG+42);
}

char dhgr_getc() {
  // TODO: blink cursor
  return cgetc();
}

void __fastcall__ dhgr_mixed() {
  POKE(0xC053, 0);
}

void __fastcall__ dhgr_fullscreen() {
  POKE(0xC052, 0);
}

