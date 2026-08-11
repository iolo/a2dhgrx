#include <stdint.h>
#include <peekpoke.h>
#include "dhgrxlib.h"

// zero-page variables used by the library
#define X1 0xfa
#define Y1 0xfb
#define X2 0xfc
#define Y2 0xfd
#define COLOR 0xfe
#define CH 0xff
#define SRC 0xce
#define DST 0xeb

//#define POKE(addr, val) *((uint8_t*)addr) = val
//#define PEEK(addr) *((uint8_t*)addr)
#define CALL(addr) __asm__("jsr %w", addr)

void __fastcall__ dhgr_init() {
  CALL(DHGRX_ADDR);
}

void __fastcall__ dhgr_exit() {
  CALL(DHGRX_ADDR+3);
}

void __fastcall__ dhgr_cls(uint8_t color) {
  POKE(COLOR, color);
  CALL(DHGRX_ADDR+6);
}

void __fastcall__ dhgr_plot(uint8_t x, uint8_t y, uint8_t c) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, c);
  CALL(DHGRX_ADDR+9);
}

void __fastcall__ dhgr_hline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(COLOR, c);
  CALL(DHGRX_ADDR+12);
}

void __fastcall__ dhgr_vline(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(Y2, y2);
  POKE(COLOR, c);
  CALL(DHGRX_ADDR+15);
}

void __fastcall__ dhgr_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(COLOR, c);
  CALL(DHGRX_ADDR+18);
}

void __fastcall__ dhgr_fillrect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t c) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(COLOR, c);
  CALL(DHGRX_ADDR+21);
}

void __fastcall__ dhgr_pixmap(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKEW(SRC, (unsigned)data);
  CALL(DHGRX_ADDR+24);
}

void __fastcall__ dhgr_bitmap(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, const uint8_t* data) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKE(COLOR, color);
  POKEW(SRC, (unsigned)data);
  CALL(DHGRX_ADDR+27);
}

void __fastcall__ dhgr_puts(uint8_t x, uint8_t y, uint8_t color, const char* data) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, color);
  POKEW(SRC, (unsigned)data);
  CALL(DHGRX_ADDR+30);
}

void __fastcall__ dhgr_puts2(uint8_t x, uint8_t y, uint8_t color, const uint16_t* data) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, color);
  POKEW(SRC, (unsigned)data);
  CALL(DHGRX_ADDR+33);
}

void __fastcall__ dhgr_puts_utf8(uint8_t x, uint8_t y, uint8_t color, const char* data) {
  POKE(X1, x);
  POKE(Y1, y);
  POKE(COLOR, color);
  POKEW(SRC, (unsigned)data);
  CALL(DHGRX_ADDR+36);
}

void __fastcall__ dhgr_load(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKEW(SRC, (unsigned)data);
  CALL(DHGRX_ADDR+39);
}

void __fastcall__ dhgr_save(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data) {
  POKE(X1, x1);
  POKE(Y1, y1);
  POKE(X2, x2);
  POKE(Y2, y2);
  POKEW(DST, (unsigned)data);
  CALL(DHGRX_ADDR+42);
}

void __fastcall__ dhgr_mixed() {
  POKE(0xC053, 0);
}

void __fastcall__ dhgr_fullscreen() {
  POKE(0xC052, 0);
}

void __fastcall__ MAIN2AUX(uint16_t src0, uint16_t src1, uint16_t dst0) {
  POKEW(0x3c, (unsigned)src0);
  POKEW(0x3e, (unsigned)src1);
  POKEW(0x42, (unsigned)dst0);
  __asm__("sec");
  __asm__("jsr $c311");
}

void __fastcall__ AUX2MAIN(uint16_t src0, uint16_t src1, uint16_t dst0) {
  POKEW(0x3c, (unsigned)src0);
  POKEW(0x3e, (unsigned)src1);
  POKEW(0x42, (unsigned)dst0);
  __asm__("clc");
  __asm__("jsr $c311");
}
