; DHGRX - Apple //e Double Hi-Res Graphics Extension
	.org	$6000
	.pc02	; use 65c02 instructions
	.feature c_comments

.include	"macro.inc"

; shared variables in zero-page
; should be used with PUSH/PULL to avoid conflict with other routines
x1	=	$fa	; 250d
y1	=	$fb	; 251d
x2	=	$fc	; 252d
y2	=	$fd	; 253d
color	=	$fe	; 254d
ch	=	$ff	; 255d
src	=	$ce	; 206d; pointer to source address (2 bytes)
dst	=	$eb	; 235d; pointer to destination address (2 bytes)

; entry point == jump table
.proc	dhgrx
	jmp	dhgr_init	; $6000(24576)
	jmp	dhgr_exit	; +3
	jmp	dhgr_cls	; +6
	jmp	dhgr_plot	; +9
	jmp	dhgr_hline	; +12
	jmp	dhgr_vline	; +15
	jmp	dhgr_rect	; +18
	jmp	dhgr_fillrect	; +21
	jmp	dhgr_pixmap	; +24
	jmp	dhgr_bitmap	; +27
	jmp	dhgr_puts	; +30
	jmp	dhgr_puts_ucs2	; +33
	jmp	dhgr_puts_utf8	; +36
	jmp	dhgr_load	; +39
	jmp	dhgr_save	; +42
	.word	font4x6		; +45
	.word	font5x12	; +47
	.word	font10x12	; +49
.endproc

; switch to double hi-res mixed/page1
.proc	dhgr_init
	sta	$c050 ; graphics
	sta	$c053 ; mixed
	;sta	$c052 ; full
	sta	$c054 ; page1
	;sta	$c055 ; page2
	sta	$c057 ; hires
	sta	$c05e ; an3 on

	; copy this code(MAIN $6000..$7fff) to AUX to avoid crash while bank switching
	MAIN2AUX $6000, $7fff, $6000

	rts
.endproc

; switch to text page1
.proc	dhgr_exit
	sta	$c051 ; text
	sta	$c05f ; an3 off
	sta	$c054 ; page1
	rts
.endproc


; clear dobule hi-res page1
; @param color
.proc	dhgr_cls
	lda	color
	and	#%1111
	tax
	lda	DHGR_FILL_BYTE_0,x
	sta	x1
	lda	DHGR_FILL_BYTE_1,x
	sta	y1
	lda	DHGR_FILL_BYTE_2,x
	sta	x2
	lda	DHGR_FILL_BYTE_3,x
	sta	y2

	ldx	#0 ; y=0
@loop_y:
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y]

	ldy	#0 ; x=0
@loop_x:
; AUX dst[x] = byte0
	lda	x1
	AUX_BANK
	sta     (dst),y
	MAIN_BANK

; MAIN dst[x] = byte1
	lda	y1
	;MAIN_BANK
	sta     (dst),y

	iny	; x++

; AUX dst[x] = byte2
	lda	x2
	AUX_BANK
	sta     (dst),y
	MAIN_BANK

; MAIN dst[x] = byte3
	lda	y2
	;MAIN_BANK
	sta     (dst),y

	iny	; x++
	cpy	#(DHGR_WIDTH_BYTES/2)	; 40 in aux + 40 in main
	bne	@loop_x ; while(x!=40)

	inx	; y++
	cpx	#DHGR_HEIGHT
	bne	@loop_y ; while(y!=192)

	rts
.endproc

; draw a pixel
; @param x1
; @param y1
; @param color
.proc	dhgr_plot
	ldy	y1
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,y
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,y
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y1]

	ldx	x1
	lda     HGR_OFFSET_X,x
	tay	; Y = (x1 / 7) * 2 = byte offset of x1
	lda	HGR_PIXEL_GROUP,x	; A = x1 % 7 = pixel group
	bne	@not_group_a

@group_a:
	; byte0
	AUX_BANK
	lda	(dst),y
	MAIN_BANK
	and	#%11110000
	sta	ch

	lda	color
	and	#%00001111
	ora	ch

	AUX_BANK
	sta	(dst),y
	MAIN_BANK

	rts

@not_group_a:
	dec
	bne	@not_group_b

@group_b:
	; byte0
	AUX_BANK
	lda	(dst),y
	MAIN_BANK
	and	#%10001111
	sta	ch

	lda	color
	asl
	asl
	asl
	asl
	and	#%01110000
	ora	ch

	AUX_BANK
	sta	(dst),y
	MAIN_BANK

	; byte1
	;MAIN_BANK
	lda	(dst),y
	and	#%11111110
	sta	ch

	lda	color
	lsr
	lsr
	lsr
	and	#%00000001
	ora	ch	; ch = composed byte for framebuffer

	;MAIN_BANK
	sta	(dst),y

	rts

@not_group_b:
	dec
	bne	@not_group_c

@group_c:
	; byte1
	;MAIN_BANK
	lda	(dst),y
	and	#%11100001
	sta	ch

	lda	color
	asl
	and	#%00011110
	ora	ch	; ch = composed byte for framebuffer

	;MAIN_BANK
	sta	(dst),y

	rts

@not_group_c:
	dec
	bne	@not_group_d

@group_d:
	; byte1
	;MAIN_BANK
	lda	(dst),y
	and	#%10011111
	sta	ch

	lda	color
	asl
	asl
	asl
	asl
	asl
	and	#%01100000
	ora	ch

	;MAIN_BANK
	sta	(dst),y

	iny	; for byte2..3

	; byte2
	AUX_BANK
	lda	(dst),y
	MAIN_BANK
	and	#%11111100
	sta	ch

	lda	color
	lsr
	lsr
	and	#%00000011
	ora	ch

	AUX_BANK
	sta	(dst),y
	MAIN_BANK

	rts
@not_group_d:
	dec
	bne	@not_group_e

@group_e:
	iny	; for byte2..3

	; byte2
	AUX_BANK
	lda	(dst),y
	MAIN_BANK
	and	#%11000011
	sta	ch

	lda	color
	asl
	asl
	and	#%00111100
	ora	ch

	AUX_BANK
	sta	(dst),y
	MAIN_BANK

	rts

@not_group_e:
	dec
	bne	@not_group_f

@group_f:
	iny	; for byte2..3

	; byte2
	AUX_BANK
	lda	(dst),y
	MAIN_BANK
	and	#%10111111
	sta	ch

	lda	color
	asl
	asl
	asl
	asl
	asl
	asl
	and	#%01000000
	ora	ch

	AUX_BANK
	sta	(dst),y
	MAIN_BANK

	; byte3
	;MAIN_BANK
	lda	(dst),y
	and	#%11111000
	sta	ch

	lda	color
	lsr
	and	#%00000111
	ora	ch

	;MAIN_BANK
	sta	(dst),y

	rts
@not_group_f:

@group_g:
	iny	; for byte2..3

	;byte3
	;MAIN_BANK
	lda	(dst),y
	and	#%10000111
	sta	ch

	lda	color
	asl
	asl
	asl
	and	#%01111000
	ora	ch

	;MAIN_BANK
	sta	(dst),y

	rts
.endproc


; draw horizontal line
; @param x1(modified)
; @param x2
; @param y1
; @param color
.proc	dhgr_hline
	lda	x1
	cmp	x2
	bcc	@no_swap
	ldx	x2
	sta	x2
	stx	x1
@no_swap:

	lda	x2
	sec
	sbc	x1
	cmp	#6	; x2 - x1 < 7
	bcs	@loopx_left

; shorter than 7 pixels, use dhgr_plot to draw each pixel(slow)
; ex. hline 1..6
@loopx_short:
	lda	x2
	cmp	x1
	bcc	@end_loopx_short ; while (x1 <= x2)
	jsr 	dhgr_plot
	inc	x1
	bne 	@loopx_short ; endwhile
@end_loopx_short:
	rts

; longer than 7 pixels, split into 3 parts: left(slow), middle(fast), right(slow)
; ex. 5..22 -> left(slow):5..6, middle(fast):7..20, right(slow):21..22
;
@loopx_left:
	ldx	x1
	lda	HGR_PIXEL_GROUP,x
	beq	@end_loopx_left; while (x1 % 7)
	jsr 	dhgr_plot
	inc	x1
	bne	@loopx_left ; endwhile

@end_loopx_left:
	ldx	y1
	lda	HGR_OFFSET_LO,x
	clc
	adc	#<HGR_BASE
	sta	dst
	lda	HGR_OFFSET_HI,x
	adc	#>HGR_BASE
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y]

	ldx	x1
	lda	HGR_OFFSET_X,x
	tay	; Y = (x1 / 7) * 2 = byte offset of x1

	lda	color
	and	#%1111
	tax	; X <- color & 0x0f

@loopx_middle:
	lda	x1
	clc
	adc	#7
	cmp	x2
	bcs	@loopx_right ; while (x1 + 7 <= x2)
	sta	x1	; x1 += 7

	lda	DHGR_FILL_BYTE_0,x
	AUX_BANK
	sta	(dst),y
	MAIN_BANK
	lda	DHGR_FILL_BYTE_1,x
	sta	(dst),y

	iny

	lda	DHGR_FILL_BYTE_2,x
	AUX_BANK
	sta	(dst),y
	MAIN_BANK
	lda	DHGR_FILL_BYTE_3,x
	sta	(dst),y

	iny
	bne	@loopx_middle ; endwhile

@loopx_right:
	lda	x2
	cmp	x1
	bcc	@end_loopx_right ; while (x1 <= x2)
	jsr 	dhgr_plot
	inc	x1
	bne	@loopx_right ; endwhile

@end_loopx_right:
	rts
.endproc


; draw vertical line
; @param y1(modified)
; @param y2
; @param x1
; @param color
.proc	dhgr_vline
	lda	y1
	cmp	y2
	bcc	@no_swap
	ldx	y2
	sta	y2
	stx	y1
@no_swap:

@loopy:
	jsr 	dhgr_plot
	inc	y1
	lda	y2
	cmp	y1
	bcs	@loopy ; while (y1 <= y2)
	rts
.endproc


; draw rectangle
; @param x1
; @param x2
; @param y1
; @param y2
; @param color
.proc	dhgr_rect
	PUSH	x1
	jsr 	dhgr_hline	; hlin x1,x2,y1
	PULL	x1

	PUSH	x1, y1
	lda 	y2
	sta 	y1
	jsr 	dhgr_hline	; hlin x1,x2,y2
	PULL	y1, x1

	inc 	y1
	dec 	y2

	PUSH	y1
	jsr 	dhgr_vline	; vlin y1+1,y2-1,x1
	PULL	y1

	lda 	x2
	sta 	x1
	jsr 	dhgr_vline	; vlin y1+1,y2-1,x2

	rts
.endproc


; draw filled rectangle
; @param x1
; @param x2
; @param y1(modified)
; @param y2
; @param color
.proc	dhgr_fillrect
	lda	y1
	cmp	y2
	bcc	@no_swap
	ldx	y2
	sta	y2
	stx	y1
@no_swap:

@loopy:
	PUSH	x1
	jsr 	dhgr_hline
	PULL	x1
	inc	y1
	lda	y2
	cmp	y1
	bcs	@loopy ; while (y1 <= y2)
	rts
.endproc


; draw pixmap
;
; @param x1(modified)
; @param y1(modified)
; @param x2
; @param y2
; @param src - pointer to pixel data; 1 byte per pixel; (x2-x1+1)*(y2-y1+1) bytes
.proc	dhgr_pixmap
	lda	x1
	sta	@_x1

	ldy	#0
@loop:
	lda	(src),y
	sta	color
	and	#$f0
	bne	@skip	; skip transparent(when hi-nibble is not zero)
	phy
	jsr 	dhgr_plot
	ply
@skip:
	iny	; next pixel

	inc	x1
	ldx	x2
	cpx	x1
	bcs	@loop	; x2 >= ++x1

	ldx	@_x1
	stx	x1	; reset x1(next row, first column)
	inc	y1
	ldx	y2
	cpx	y1
	bcs	@loop	; y2 >= ++y1

	rts

@_x1:	.byte	0
.endproc


; draw bitmap
;
; @param x1(modified)
; @param y1(modified)
; @param x2
; @param y2
; @param color
; @param src - pointer to pixel data; 1 bit per pixel; (x2-x1+1)*(y2-y1+1) bytes
.proc	dhgr_bitmap
	lda	x1
	sta	@_x1
	;//lda	color
	;//sta	@_color

@loop:
	ldy	#0
	lda	(src),y
	ldx	#8	; 8 bits in a byte
@loop_bit:
	asl
	bcc	@skip	; skip skip transparent
	;//ldy	@_color
	;//sty	color
	pha
	phx
	jsr 	dhgr_plot
	plx
	pla
@skip:
	inc	x1	; next column
	ldy	x2
	cpy	x1
	bcs	@continue; while x1 <= x2

	ldy	@_x1
	sty	x1	; carriage return
	inc	y1	; line feed
	ldy	y2
	cpy	y1
	bcs	@continue; while ++y1 <= y2

	rts

@continue:
	dex		; next bit

	bne	@loop_bit
	inc	src	; next byte
	bne	@loop
	inc	src+1	; next page
	bne	@loop	; jmp always
	rts		; safe guard

@_x1:	.byte	0
;//@_color:.byte	0
.endproc

; load byte-aligned rectangular area from src to video memory
;
; @param x1 0..39 (40 bytes per row)
; @param y1 0..191
; @param x2 0..39
; @param y2 0..191
; @param src(modified)
.proc dhgr_load
	ldx	y1	; NOTE: reg. X for y
@loop_y:
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y]

	ldy	x1	; NOTE: reg. Y for x(for zeropage indirect addressing)
@loop_x:
	lda	(src)
	AUX_BANK
	sta	(dst),y	; aux bank = *src++
	MAIN_BANK
	INC16	src

	lda	(src)
	sta	(dst),y ; main bank = *src++
	INC16	src

	iny
	cpy	x2
	bcc	@loop_x ; while(x <= x2)
	beq	@loop_x

	inx
	cpx	y2
	bcc	@loop_y ; while(y <= y2)
	beq	@loop_y

	rts
.endproc

; save byte-aligned rectangular area from video memory to dst
;
; @param x1 0..39 (40 bytes per row)
; @param y1 0..191
; @param x2 0..39
; @param y2 0..191
; @param dst
.proc dhgr_save
	ldx	y1	; NOTE: reg. X for y
@loop_y:
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	src
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	src+1	; src = HGR_BASE + HGR_OFFSET[y]

	ldy	x1	; NOTE: reg. Y for x(for zeropage indirect addressing)
@loop_x:
	AUX_BANK
	lda	(src),y
	MAIN_BANK
	sta	(dst)	; *dst++ = aux bank
	INC16	dst

	lda	(src),y
	sta	(dst)	; *dst++ = main bank(same address)
	INC16	dst

	iny
	cpy	x2
	bcc	@loop_x ; while(x <= x2)
	beq	@loop_x

	inx
	cpx	y2
	bcc	@loop_y ; while(y <= y2)
	beq	@loop_y

	rts
.endproc

FONT_WIDTH	= 4
FONT_HEIGHT	= 6
FONT_CODE_BEGIN = $20
FONT_CODE_END	= $7F
FONT_GLYPH_BYTES= 3
FONT_GLYPH_COUNT= 96

; draw char with 4x6 font
;
; @param x1(modified)
; @param y1(modified)
; @param color
; @param ch
.proc	dhgr_putc
	lda	#<font4x6
	sta	src
	lda	#>font4x6
	sta	src+1

	lda	ch
	sec
	sbc	#$20	; A = ch - $20
	sta	ch

	lda	src
	clc
	adc	ch
	adc	ch
	adc	ch
	sta	src
	lda	src+1
	adc	#$00
	sta	src+1	; src = font4x6 + (ch - 0x20)*3

	lda	x1
	clc
	adc	#(FONT_WIDTH-1)
	sta	x2

	lda	y1
	clc
	adc	#(FONT_HEIGHT-1)
	sta	y2

	jsr	dhgr_bitmap
	rts
.endproc


; draw text string with 4x6 font
;
; @param x1(modified)
; @param y1(modified)
; @param color
; @param src - pointer to string ends with zero
.proc	dhgr_puts
	ldy	#0
@loop:
	lda	(src),y
	beq	@break
	sta	ch
	PUSH	x1, y1, color, src, src+1
	phy
	jsr	dhgr_putc
	ply
	PULL	src+1, src, color, y1, x1
	iny		; next char

	lda	x1
	clc
	adc	#FONT_WIDTH
	sta	x1
	lda	#(DHGR_WIDTH-1)
	cmp	x1
	bcs	@loop
	lda	#0
	sta	x1	; carrage return
	lda	y1
	clc
	adc	#FONT_HEIGHT
	sta	y1	; line feed
	lda	#(DHGR_HEIGHT-1)
	cmp	y1
	bcs	@loop
@break:
	rts
.endproc


ENG_FONT_ADDR = font5x12
ENG_FONT_WIDTH = 5
ENG_FONT_HEIGHT = 12
ENG_CODE_BEGIN = $20
ENG_CODE_END = $7f
ENG_GLYPH_COUNT = 96	; 0x20 ~ 0x7f
ENG_GLYPH_SIZE = 8	; 5x12 = 60bits -> 60/8 <= 8bytes

KOR_FONT_WIDTH = 10
KOR_FONT_HEIGHT = 12
KOR_CODE_BEGIN = $ac00	; 가
KOR_CODE_END = $d7a3	; 힣
NUM_CHO_FORM = 3	; 0 for vertical-shaped, 1 for horizontal-shaped, 2 for compound
NUM_JUNG_FORM = 1
NUM_JONG_FORM = 1
CHO_COUNT = 19		; ㄱ,ㄲ,ㄴ,ㄷ,ㄸ,ㄹ,ㅁ,ㅂ,ㅃ,ㅅ,ㅆ,ㅇ,ㅈ,ㅉ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ
JUNG_COUNT = 21		; ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ
JONG_COUNT = 28		; fill,ㄱ,ㄲ,ㄳ,ㄴ,ㄵ,ㄶ,ㄷ,ㄹ,ㄺ,ㄻ,ㄼ,ㄽ,ㄾ,ㄿ,ㅀ,ㅁ,ㅂ,ㅄ,ㅅ,ㅆ,ㅇ,ㅈ,ㅊ,ㅋ,ㅌ,ㅍ,ㅎ
KOR_GLYPH_SIZE = 15	; 10x12 = 120bits -> 120/8 = 15bytes
KOR_GLYPH_COUNT = NUM_CHO_FORM*CHO_COUNT + NUM_JUNG_FORM*JUNG_COUNT + NUM_JONG_FORM*(JONG_COUNT-1)

KOR_FONT_ADDR = font10x12
CHO_FONT_ADDR = KOR_FONT_ADDR
JUNG_FONT_ADDR = CHO_FONT_ADDR + NUM_CHO_FORM * CHO_COUNT * KOR_GLYPH_SIZE
JONG_FONT_ADDR = JUNG_FONT_ADDR + NUM_JUNG_FORM * JUNG_COUNT * KOR_GLYPH_SIZE


; variables for korean decomposition in zero-page
cho	=	x1	; 250d
jung	=	y1	; 251d
jong	=	x2	; 252d
code	=	x2	; 252d; unicode char code(2 bytes);

; draw ucs2 char with 5x12 font
; @param x1(modified)
; @param y1(modified)
; @param code(modified)
; @param color
; @returns carry=0 for english, set carry=1 for korean
.proc dhgr_putc_ucs2
	lda	code+1
	bne	@kor	; korean if hi byte is not zero

@eng:
	lda	code
	sec
	sbc	#ENG_CODE_BEGIN	; A = code - $20

	ldx	#0
	stx	ch	; ch=0; temporary use ch as hi byte
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch	; ch:A = A << 3 = (code - $20) * 8

	clc
	adc	#<ENG_FONT_ADDR
	sta	src
	lda	ch	; hi-byte
	adc	#>ENG_FONT_ADDR
	sta	src+1	; dst = eng_font_addr + (code - $20) * 8

	lda	x1
	adc	#(ENG_FONT_WIDTH-1)
	sta	x2
	lda	y1
	adc	#(ENG_FONT_HEIGHT-1)
	sta	y2
	PUSH x1,y1
	jsr	dhgr_bitmap	; draw eng font in src to framebuffer x1,y1,x1+5-1,y1+12-1
	PULL y1,x1

	clc	; carry=0 for english
	rts

@kor:
	PUSH	x1,y1

	lda	#<kor_buf
	sta	dst
	lda	#>kor_buf
	sta	dst+1	; dst = kor_buf

	jsr	decompose_ucs2	; code -> cho,jung,jong

; get cho font
	lda	cho
	; determine cho_form by jung and calculate cho font offset
	ldy	jung
	ldx	cho_form_by_jung,y	; cho_form=0,1,2
	clc
	adc	cho_form_offset,x	; cho_form_offset=cho_form*19=0,19,38
	sta	cho	; cho = cho_form * cho_count + cho

	ldx	#0
	stx	ch	; ch=0 for hi-byte
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch
	sec
	sbc	cho
	sta	cho
	lda	ch
	sbc	#0
	sta	ch	; ch:cho = (cho_form * cho_count + cho) * 15

	lda	cho
	clc
	adc	#<CHO_FONT_ADDR
	sta	src
	lda	ch	; hi-byte
	adc	#>CHO_FONT_ADDR
	sta	src+1	; src = cho_font_base + (cho_form*cho_count + cho)*15; cho_form=0,1,2

	ldy	#(KOR_GLYPH_SIZE-1)
@loop_cho_font:
	lda	(src),y
	sta	(dst),y
	dey
	bpl	@loop_cho_font	; copy cho font in src to kor_buf in dst

; get jung font
	lda	jung
	ldx	#0
	stx	ch	; ch=0 for hi-byte
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch
	sec
	sbc	jung
	sta	jung
	lda	ch
	sbc	#0
	sta	ch	; ch:jung = (jung << 4) - jung = jung * 15

	lda	jung
	clc
	adc	#<JUNG_FONT_ADDR
	sta	src
	lda	ch
	adc	#>JUNG_FONT_ADDR
	sta	src+1	; src = jung_font_base + (jung_form*jung_count + jung)*15; jung_form=0 always for now

	ldy	#(KOR_GLYPH_SIZE-1)
@loop_jung_font:
	lda	(dst),y
	ora	(src),y
	sta	(dst),y
	dey
	bpl	@loop_jung_font	; merge jung font in src into kor_buf in dst

; get jong font
	lda	jong
	beq	@no_jong	; if jong==0, skip jong font
	dec
	sta	jong	; jong -= 1; no glyph for fill

	ldx	#0
	stx	ch	; ch=0 for hi-byte
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch
	asl
	rol	ch
	sec
	sbc	jong
	sta	jong
	lda	ch
	sbc	#0
	sta	ch	; ch:jong = (jong << 4) - jong = jong * 15

	lda	jong
	clc
	adc	#<JONG_FONT_ADDR
	sta	src
	lda	ch	; hi-byte
	adc	#>JONG_FONT_ADDR
	sta	src+1	; src = jong_font_base + (jong_form*jong_count + jong)*15; jong_form=0 always for now

	ldy	#(KOR_GLYPH_SIZE-1)
@loop_jong_font:
	lda	(dst),y
	ora	(src),y
	sta	(dst),y
	dey
	bpl	@loop_jong_font	; merge jong font in src to kor_buf in dst

@no_jong:
	PULL	y1,x1

	lda	#<kor_buf
	sta	src
	lda	#>kor_buf
	sta	src+1	; src = kor_buf
	lda	x1
	clc
	adc	#(KOR_FONT_WIDTH-1)
	sta	x2
	lda	y1
	clc
	adc	#(KOR_FONT_HEIGHT-1)
	sta	y2
	PUSH x1,y1
	jsr	dhgr_bitmap	; draw kor font in src to framebuffer x1,y1,x1+10-1,y1+12-1
	PULL y1,x1

	sec	; carry=1 for korean
	rts
.endproc

; draw ucs2 text string
;
; @param x1(modified)
; @param y1(modified)
; @param src - pointer to string ends with zero
; @param color
.proc dhgr_puts_ucs2
	ldy	#0 ; src index
@loop:
	lda	(src),y ; lo
	sta	code
	iny		; next byte
	ora	(src),y
	beq	@break	; break if hi=0 and lo=0
	lda	(src),y ; hi
	sta	code+1
	phy
	PUSH src,src+1,x1,y1
	jsr	dhgr_putc_ucs2
	PULL y1,x1,src+1,src
	ply
	iny		; next ucs2 char

	bcs	@forward_kor
@forward_eng:
	lda	#ENG_FONT_WIDTH
	bne	@forward	; jmp always
@forward_kor:
	lda	#KOR_FONT_WIDTH
@forward:
	clc
	adc	x1	; x1 += 5 for english, x1 += 10 for korean
	sta	x1
	cmp	#(DHGR_WIDTH-5)
	bcc	@loop	; if x1 < 140-5

	lda	#0
	sta	x1	; x=0 carrage return

	lda	y1
	clc
	adc	#12	; y1+=12 line feed
	sta	y1
	cmp	#(DHGR_HEIGHT-12)
	bcc	@loop ; while y1 < 192-12
	; TODO: scroll up? or return to top? or stop?
@break:
	rts
.endproc


; draw utf8 text string
; @param x1(modified)
; @param y1(modified)
; @param color(modified)
; @param src - pointer to string ends with zero
.proc dhgr_puts_utf8
	PUSH	x1,y1,color

	lda	#<ucs2_buf
	sta	dst
	lda	#>ucs2_buf
	sta	dst+1	; dst = ucs2_buf
	jsr	utf8_to_ucs2	; convert utf8 in src to ucs2 in ucs2_buf

	PULL	color,y1,x1

	lda	#<ucs2_buf
	sta	src
	lda	#>ucs2_buf
	sta	src+1	; src = ucs2_buf
	jsr	dhgr_puts_ucs2 ; draw ucs2 string in src
	rts
.endproc

.include	"softswitch.inc"
.include	"math.inc"
.include	"dhgr.inc"
;.include	"pixmap.inc"
;.include	"bitmap.inc"
.include	"kor.s"

cho_form_by_jung:
	; cho=0..19, jung=0..20 -> cho_form=0,1,2
	;     ㅏ,ㅐ,ㅑ,ㅒ,ㅓ,ㅔ,ㅕ,ㅖ,ㅗ,ㅘ,ㅙ,ㅚ,ㅛ,ㅜ,ㅝ,ㅞ,ㅟ,ㅠ,ㅡ,ㅢ,ㅣ
	.byte 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 1, 2, 0

cho_form_offset:
	; cho_font_addr + cho_form * cho_count; cho_form=0,1,2; cho_count=19
	.byte	0, CHO_COUNT, CHO_COUNT*2

; kor font buffer
kor_buf:
	.res 15

.align $100
; utf8_to_ucs2 buffer
ucs2_buf:
	.res 256

.align $100
font4x6:
	.incbin "font4x6.bin"

;//.align $100
font5x12:
	.incbin "font5x12.bin"

;//.align $100
font10x12:
	.incbin "font10x12.bin"
