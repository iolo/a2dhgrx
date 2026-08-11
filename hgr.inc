HGR_PAGE1_BASE = $2000
HGR_PAGE2_BASE = $4000
HGR_BASE = HGR_PAGE1_BASE

HGR_WIDTH				= 140		; 140 color-pixels, 7 color-pixels in 2 bytes = 20 groups of 2 bytes = 40 bytes per row
HGR_HEIGHT			= 192
HGR_WIDTH_BYTES	= 40

; pre-compute the hgr address for each y value(y=0..191)
; hgr_offset[y] = (y/64)*0x28 + (y%8)*0x400 + ((y/8)&7)*0x80;

.align $100
HGR_OFFSET_HI:
	; section 0
	.byte $0, $4, $8, $c, $10, $14, $18, $1c ; block 0
	.byte $0, $4, $8, $c, $10, $14, $18, $1c ; block 1
	.byte $1, $5, $9, $d, $11, $15, $19, $1d ; block 2
	.byte $1, $5, $9, $d, $11, $15, $19, $1d ; block 3
	.byte $2, $6, $a, $e, $12, $16, $1a, $1e ; block 4
	.byte $2, $6, $a, $e, $12, $16, $1a, $1e ; block 5
	.byte $3, $7, $b, $f, $13, $17, $1b, $1f ; block 6
	.byte $3, $7, $b, $f, $13, $17, $1b, $1f ; block 7
	; section 1
	.byte $0, $4, $8, $c, $10, $14, $18, $1c ; block 0
	.byte $0, $4, $8, $c, $10, $14, $18, $1c ; block 1
	.byte $1, $5, $9, $d, $11, $15, $19, $1d ; block 2
	.byte $1, $5, $9, $d, $11, $15, $19, $1d ; block 3
	.byte $2, $6, $a, $e, $12, $16, $1a, $1e ; block 4
	.byte $2, $6, $a, $e, $12, $16, $1a, $1e ; block 5
	.byte $3, $7, $b, $f, $13, $17, $1b, $1f ; block 6
	.byte $3, $7, $b, $f, $13, $17, $1b, $1f ; block 7
	; section 2
	.byte $0, $4, $8, $c, $10, $14, $18, $1c ; block 0
	.byte $0, $4, $8, $c, $10, $14, $18, $1c ; block 1
	.byte $1, $5, $9, $d, $11, $15, $19, $1d ; block 2
	.byte $1, $5, $9, $d, $11, $15, $19, $1d ; block 3
	.byte $2, $6, $a, $e, $12, $16, $1a, $1e ; block 4
	.byte $2, $6, $a, $e, $12, $16, $1a, $1e ; block 5
	.byte $3, $7, $b, $f, $13, $17, $1b, $1f ; block 6
	.byte $3, $7, $b, $f, $13, $17, $1b, $1f ; block 7
	; 192 bytes
	;.res 256-192 ; padding for page($ff bytes) aligned
	; free 64 bytes!

.align $100
HGR_OFFSET_LO:
	; section 0
	.byte $00, $00, $00, $00, $00, $00, $00, $00 ; block 0
	.byte $80, $80, $80, $80, $80, $80, $80, $80 ; block 1
	.byte $00, $00, $00, $00, $00, $00, $00, $00 ; block 2
	.byte $80, $80, $80, $80, $80, $80, $80, $80 ; block 3
	.byte $00, $00, $00, $00, $00, $00, $00, $00 ; block 4
	.byte $80, $80, $80, $80, $80, $80, $80, $80 ; block 5
	.byte $00, $00, $00, $00, $00, $00, $00, $00 ; block 6
	.byte $80, $80, $80, $80, $80, $80, $80, $80 ; block 7
	; section 1
	.byte $28, $28, $28, $28, $28, $28, $28, $28 ; block 0
	.byte $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8 ; block 1
	.byte $28, $28, $28, $28, $28, $28, $28, $28 ; block 2
	.byte $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8 ; block 3
	.byte $28, $28, $28, $28, $28, $28, $28, $28 ; block 4
	.byte $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8 ; block 5
	.byte $28, $28, $28, $28, $28, $28, $28, $28 ; block 6
	.byte $a8, $a8, $a8, $a8, $a8, $a8, $a8, $a8 ; block 7
	; section 2
	.byte $50, $50, $50, $50, $50, $50, $50, $50 ; block 0
	.byte $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0 ; block 1
	.byte $50, $50, $50, $50, $50, $50, $50, $50 ; block 2
	.byte $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0 ; block 3
	.byte $50, $50, $50, $50, $50, $50, $50, $50 ; block 4
	.byte $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0 ; block 5
	.byte $50, $50, $50, $50, $50, $50, $50, $50 ; block 6
	.byte $d0, $d0, $d0, $d0, $d0, $d0, $d0, $d0 ; block 7
	; 192 bytes
	;.res 256-192 ; padding for page($ff bytes) aligned
	; free 64 bytes!


; pixel x(0..139) -> byte offset x(0..39) in row
; pixel 0..6 -> byte 0..1, pixel 17..13 -> byte 2..3, ... pixel 138..139 -> byte 38..39
; HGR_OFFSET_X[x] = (x / 7) * 2
;
; ldx x1
; lda HGR_OFFSET_X,x1

.align $100
HGR_OFFSET_X:
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


; pixel x(0..139) -> pixel group(0..6; A..G)
; HGR_PIXEL_GROUP[x] = x % 7 = 0..6 -> pixel group 0..6(A..G)
;
; ldx x1
; lda HGR_PIXEL_GROUP,x1

.align $100
HGR_PIXEL_GROUP:
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

