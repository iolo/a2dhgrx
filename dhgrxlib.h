#ifndef __DHGRXLIB_H__
#define __DHGRXLIB_H__

/*
 * Double Hi-Res Graphics Library for Apple II
 */

#ifndef DHGRX_ADDR
#define DHGRX_ADDR 0x6000
#endif

#define DHGR_WIDTH 140
#define DHGR_HEIGHT 192
#define DHGR_NUM_COLORS 16

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


void __fastcall__ dhgr_init();
void __fastcall__ dhgr_exit();
void __fastcall__ dhgr_cls(uint8_t color);
void __fastcall__ dhgr_plot(uint8_t x, uint8_t y, uint8_t c);
void __fastcall__ dhgr_hline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t c);
void __fastcall__ dhgr_vline(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t c);
void __fastcall__ dhgr_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t c);
void __fastcall__ dhgr_fillrect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t c);
void __fastcall__ dhgr_pixmap(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data);
void __fastcall__ dhgr_bitmap(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color, const uint8_t* data);
void __fastcall__ dhgr_puts(uint8_t x, uint8_t y, uint8_t color, const char* data);
void __fastcall__ dhgr_puts2(uint8_t x, uint8_t y, uint8_t color, const uint16_t* data);
void __fastcall__ dhgr_puts_utf8(uint8_t x, uint8_t y, uint8_t color, const char* data);
void __fastcall__ dhgr_load(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data);
void __fastcall__ dhgr_save(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, const uint8_t* data);
void __fastcall__ dhgr_mixed();
void __fastcall__ dhgr_fullscreen();

#endif // __DHGRXLIB_H__
