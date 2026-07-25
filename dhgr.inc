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

.ifndef DIV7_TBL

; pixel x(0..139) -> byte offset x(0..39) in row
; pixel 0..6 -> byte 0..1, pixel 17..13 -> byte 2..3, ... pixel 138..139 -> byte 38..39
; DHGR_OFFSET_X[x] = (x / 7) * 2
;
; see also HGR_OFFSET_LO/HGR_OFFSET_HI

.align $100
DHGR_OFFSET_X:
	.byte 0, 0, 0, 0, 0, 0, 0 ; pixels 0..6
	.byte 2, 2, 2, 2, 2, 2, 2 ; pixels 7..13
	.byte 4, 4, 4, 4, 4, 4, 4 ; pixels 14..20
	.byte 6, 6, 6, 6, 6, 6, 6 ; pixels 21..27
	.byte 8, 8, 8, 8, 8, 8, 8 ; pixels 28..34
	.byte 10, 10, 10, 10, 10, 10, 10 ; pixels 35..41
	.byte 12, 12, 12, 12, 12, 12, 12 ; pixels 42..48
	.byte 14, 14, 14, 14, 14, 14, 14 ; pixels 49..55
	.byte 16, 16, 16, 16, 16, 16, 16 ; pixels 56..62
	.byte 18, 18, 18, 18, 18, 18, 18 ; pixels 63..69
	.byte 20, 20, 20, 20, 20, 20, 20 ; pixels 70..76
	.byte 22, 22, 22, 22, 22, 22, 22 ; pixels 77..83
	.byte 24, 24, 24, 24, 24, 24, 24 ; pixels 84..90
	.byte 26, 26, 26, 26, 26, 26, 26 ; pixels 91..97
	.byte 28, 28, 28, 28, 28, 28, 28 ; pixels 98..104
	.byte 30, 30, 30, 30, 30, 30, 30 ; pixels 105..111
	.byte 32, 32, 32, 32, 32, 32, 32 ; pixels 112..118
	.byte 34, 34, 34, 34, 34, 34, 34 ; pixels 119..125
	.byte 36, 36, 36, 36, 36, 36, 36 ; pixels 126..132
	.byte 38, 38, 38, 38, 38, 38, 38 ; pixels 133..139
	;.res 256-140
.endif

.ifndef MOD7_TBL

; pixel x(0..139) -> pixel group(0..6; A..G in DHGR pixel layout)
; x % 7 = 0..6 -> pixel group 0..6(A..G)
;
; pixel group will determine which 4 bits of the 4 bytes will be used to store the color-pixel.

.align $100
DHGR_PIXEL_GROUP:
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	.byte 0, 1, 2, 3, 4, 5, 6
	;.res 256-140
.endif

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

