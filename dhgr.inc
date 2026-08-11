.include "hgr.inc"

; DHGR mode pixel layout
;
; 80 bytes per row(40 in AUX, 40 in MAIN), 7 color-pixels per 4 bytes, total 140 color-pixels per row.
;
; |bank      | AUX        |MAIN         | AUX         | MAIN       |   |
; |----------|------------|-------------+-------------|------------|---|
; |address   | $2000      |$2000        | $2001       | $2001      |...|
; |----------|------------|-------------+-------------|------------|---|
; |bit offset| 7 654 3210 | 7 65 4321 0 | 7 6 5432 10 | 7 6543 210 |...|
; |pixel     | - BBB AAAA | - DD CCCC B | - F EEEE DD | - GGGG FFF |...|
; |pixel bit | - 123 0123 | - 23 0123 0 | - 3 0123 01 | - 0123 012 |...|
; |x         |   1   0    |   3  2    1 |   5 4    3  |   6    5   |...|
;

DHGR_PAGE1_BASE	= HGR_PAGE1_BASE
DHGR_PAGE2_BASE	= HGR_PAGE2_BASE
DHGR_BASE				= DHGR_PAGE1_BASE

DHGR_WIDTH			= HGR_WIDTH							; 140 color-pixels, 7 color-pixels in 4 bytes = 20 groups of 4 bytes = 80 bytes per row
DHGR_HEIGHT			= HGR_HEIGHT
DHGR_WIDTH_BYTES= HGR_WIDTH_BYTES * 2		; 40 bytes in AUX + 40 bytes in MAIN

DHGR_OFFSET_X		= HGR_OFFSET_X
DHGR_PIXEL_GROUP= HGR_PIXEL_GROUP

; color 0..15 -> fill bytes 0..3
;
; to fill 4 bytes(7 consequent color-pixels) with a same color:
;
; lda color
; and #$f
; tax
;
; lda DHGR_FILL_BYTE_0,x
; AUX_BANK
; sta addr
; MAIN_BANK
; lda DHGR_FILL_BYTE_1,x
; sta addr
;
; inc addr
;
; lda DHGR_FILL_BYTE_2,x
; AUX_BANK
; sta addr
; MAIN_BANK
; lda DHGR_FILL_BYTE_3,x
; sta addr

DHGR_FILL_BYTE_0:
	.byte %00000000, %00010001, %00100010, %00110011, %01000100, %01010101, %01100110, %01110111
  .byte %00001000, %00011001, %00101010, %00111011, %01001100, %01011101, %01101110, %01111111

DHGR_FILL_BYTE_1:
	.byte %00000000, %00100010, %01000100, %01100110, %00001000, %00101010, %01001100, %01101110
  .byte %00010001, %00110011, %01010101, %01110111, %00011001, %00111011, %01011101, %01111111

DHGR_FILL_BYTE_2:
	.byte %00000000, %01000100, %00001000, %01001100, %00010001, %01010101, %00011001, %01011101
  .byte %00100010, %01100110, %00101010, %01101110, %00110011, %01110111, %00111011, %01111111

DHGR_FILL_BYTE_3:
	.byte %00000000, %00001000, %00010001, %00011001, %00100010, %00101010, %00110011, %00111011
  .byte %01000100, %01001100, %01010101, %01011101, %01100110, %01101110, %01110111, %01111111

.macro DHGR_FILL_4BYTES color, addr
  lda color
  and #$f
  tax

  lda DHGR_FILL_BYTE_0,x
  AUX_BANK
  sta addr
  MAIN_BANK
  lda DHGR_FILL_BYTE_1,x
  sta addr

  inc addr

  lda DHGR_FILL_BYTE_2,x
  AUX_BANK
  sta addr
  MAIN_BANK
  lda DHGR_FILL_BYTE_3,x
  sta addr
.endmacro
