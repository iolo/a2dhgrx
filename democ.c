#include <apple2.h>
#include <conio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void cls_demo() {
  uint8_t c;
  for (c = 0; c < 16; c++) {
    dhgr_cls(c);
  }
}

void plot_demo() {
  uint8_t i, x, y, c;
  for (i = 0; i < 100; i++) {
    x = rand() % 140;
    y = rand() % 192;
    c = rand() % 16;
    dhgr_plot(x, y, c);
  }
}

void hline_demo() {
  uint8_t x, y, c;
  x = 0;
  c = 1;
  for (y = 0; y < 192; y++) {
    dhgr_hline(x, y, 139-x, c);
    if (++c > 15) c = 0;
    if (++x > 60) x = 0;
  }
}

void vline_demo() {
  uint8_t x, y, c;
  y = 0;
  c = 1;
  for (x = 0; x < 140; x++) {
    dhgr_vline(x, y, 191-y, c);
    if (++c > 15) c = 0;
    if (++y > 60) y = 0;
  }
}

void rect_demo() {
  uint8_t i, c;
  c = 0;
  for (i = 0; i < 60; i++) {
    dhgr_rect(i, i, 139-i, 191-i, c);
    if (++c > 15) c = 0;
  }
}

void fillrect_demo() {
  uint8_t i, x, y, c;
  x = 0;
  y = 0;
  c = 0;
  for (i = 0; i < 60; i++) {
    dhgr_fillrect(x, y, x + 20-1-1, y+20-1-1, c);
    if (++c > 15) c = 0;
    x+=20;
    if (x > 139) { x = 0; y += 20; if  (y > 191) y = 0; }
  }
}

void pixmap_demo() {
  uint8_t i, x, y;
  uint8_t data[7*7] = {
0, 0, 15, 15, 15, 0, 0,
0, 15, 1, 1, 1, 15, 0,
15, 1, 0, 1, 0, 1, 15,
15, 1, 1, 1, 1, 1, 15,
15, 1, 0, 0, 0, 1, 15,
0, 15, 1, 1, 1, 15, 0,
0, 0, 15, 15, 15, 0, 0,
  };
  x = 0;
  y = 0;
  for (i = 0; i < 60; i++) {
    dhgr_pixmap(x, y, x+7-1, y+7-1, data);
    x += 10;
    if (x > 139) { x = 0; y += 10; if (y > 191) y = 0; }
  }
}

void bitmap_demo() {
  uint8_t i, x, y, c;
  uint8_t data[8] = {
0b00111000,
0b01000100,
0b10101010,
0b10000010,
0b10111010,
0b01000100,
0b00111000,
0b00000000,
  };
  x = 0;
  y = 0;
  c = 1;
  for (i = 0; i < 60; i++) {
    dhgr_bitmap(x, y, x+7-1, y+7-1, c, data);
    x += 10;
    if (x > 139) { x = 0; y += 10; if (y > 191) y = 0; }
    if (++c > 15) c = 0;
  }
}

char* ascii = "Hello,World! 1234567890";
uint16_t ucs2[] = {
  'H', 'e', 'l', 'l', 'o', ',', 'w', 'o', 'r', 'l', 'd', '!', ' ',
  0xd55c, 0xae00, ' ', 0xac00, 0xb098, 0xb2e4, 0xb77c, 0,
};
//char* utf8 = "Hello,World! 한글 가나다라";
char* utf8 = "Hi,한글";

void text_demo() {
  uint8_t i, c;
  c = 1;
  // ascii
  for (i = 0; i < 15; i++) {
    dhgr_puts(i, i*6, c, ascii);
    if (++c > 15) c = 0;
  }
  clrscr();
  cputsxy(0, 21, "Press any key...");
  cgetc();
  dhgr_cls(0);
  // ucs2 ascii+korean
  for (i = 0; i < 15; i++) {
    dhgr_puts2(i, i*12, c, ucs2);
    if (++c > 15) c = 0;
  }
  clrscr();
  cputsxy(0, 21, "Press any key...");
  cgetc();
  dhgr_cls(0);
  // utf8 ascii+korean
  for (i = 0; i < 15; i++) {
    dhgr_puts_utf8(i, i*12, c, utf8);
    if (++c > 15) c = 0;
  }
}

void main() {
  char c;
  while (1) {
    clrscr();
    cputsxy(0, 0, "DHGRX DEMO");
    cputsxy(0, 1, "1. CLS");
    cputsxy(0, 2, "2. PLOT");
    cputsxy(0, 3, "3. HLINE");
    cputsxy(0, 4, "4. VLINE");
    cputsxy(0, 5, "5. RECT");
    cputsxy(0, 6, "6. FILLRECT");
    cputsxy(0, 7, "7. PIXMAP");
    cputsxy(0, 8, "8. BITMAP");
    cputsxy(0, 9, "9. TEXT");
    cputsxy(0, 10, "Q. QUIT");
    cputsxy(0, 12, "SELECT..");
    c = cgetc();

    dhgr_init();
    dhgr_cls(0);

    if (c == 'q' || c == 'Q') {
      dhgr_exit();
      return;
    }

    switch(c) {
      case '1':
        cputsxy(0, 20, "CLS");
        cls_demo();
        break;
      case '2':
        cputsxy(0, 20, "PLOT");
        plot_demo();
        break;
      case '3':
        cputsxy(0, 20, "HLINE");
        hline_demo();
        break;
      case '4':
        cputsxy(0, 20, "VLINE");
        vline_demo();
        break;
      case '5':
        cputsxy(0, 20, "RECT");
        rect_demo();
        break;
      case '6':
        cputsxy(0, 20, "FILLRECT");
        fillrect_demo();
        break;
      case '7':
        cputsxy(0, 20, "PIXMAP");
        pixmap_demo();
        break;
      case '8':
        cputsxy(0, 20, "BITMAP");
        bitmap_demo();
        break;
      case '9':
        cputsxy(0, 20, "TEXT");
        text_demo();
        break;
    }

    cputsxy(0, 21, "Press any key to continue...");
    cgetc();

    dhgr_exit();
  }
}
