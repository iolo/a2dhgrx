; DHGRX - Apple //e Double Hi-Res Graphics Extension
	.org	$6000
	.pc02	; use 65c02 instructions
	.feature c_comments

.include	"macro.inc"

.macro AUX_BANK
	sta $c055
.endmacro

.macro MAIN_BANK
	sta $c054
.endmacro

; zero-page variables
x1	=	$fa
y1	=	$fb
x2	=	$fc
y2	=	$fd
color	=	$fe
ch	=	$ff
src	=	$ce 	; pointer to source address (2 bytes)
dst	=	$eb 	; pointer to destination address (2 bytes)

; entry point == jump table
.proc	dhgrx
	jmp	dhgr_init	; $6000(24576)
	jmp	dhgr_exit	; $6003
	jmp	dhgr_cls	; $6006
	jmp	dhgr_plot	; $6009
	jmp	dhgr_hline	; $600C
	jmp	dhgr_vline	; $600F
	jmp	dhgr_rect	; $6012
	jmp	dhgr_fillrect	; $6015
	jmp	dhgr_pixmap	; $6018
	jmp	dhgr_bitmap	; $601B
	jmp	dhgr_putc	; $601E
	jmp	dhgr_puts	; $6021
	jmp	dhgr_put8	; $6024
	jmp	dhgr_putc8	; $6027
	jmp	dhgr_puts8	; $602A
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
	cpy	#(DHGR_BYTES_PER_ROW/2)	; 40 in aux + 40 in main
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
	lda     DIV7_TBL,x
	asl
	tay	; Y = (x1 / 7) * 2 = byte offset of x1
	lda	MOD7_TBL,x	; A = x1 % 7 = color pixel group
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
	ora	ch

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
	ora	ch

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
	lda	MOD7_TBL,x
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
	lda	DIV7_TBL,x
	asl
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
	PUSH_VARS x1
	jsr 	dhgr_hline	; hlin x1,x2,y1
	PULL_VARS x1

	PUSH_VARS x1, y1
	lda 	y2
	sta 	y1
	jsr 	dhgr_hline	; hlin x1,x2,y2
	PULL_VARS y1, x1

	inc 	y1
	dec 	y2

	PUSH_VARS y1
	jsr 	dhgr_vline	; vlin y1+1,y2-1,x1
	PULL_VARS y1

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
	PUSH_VARS x1
	jsr 	dhgr_hline
	PULL_VARS x1
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
	lda	color
	sta	@_color

@loop:
	ldy	#0
	lda	(src),y
	ldx	#8	; 8 bits in a byte
@loop_bit:
	asl
	bcc	@skip	; skip skip transparent
	ldy	@_color
	sty	color
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
@_color:.byte	0
.endproc


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
	PUSH_VARS x1, y1, color, src, src+1
	phy
	jsr	dhgr_putc
	ply
	PULL_VARS src+1, src, color, y1, x1
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


; draw 7x8 bitmap; fast but byte-aligned
;
; @param x1(modified) 0..79
; @param y1(modified) 0..191
; @param src - pointer to pixel data; 1 bit per pixel; MSB ignored; 1*8=8bytes.
.proc dhgr_put8
	ldy	#0	; Y = src offset
	lda	x1
	lsr
	sta	x1	; x1 = x1 / 2 = dst offset
	bcc	@loopy_even

@loopy_odd:
	ldx	y1
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y1]

	lda	(src),y

	phy
	ldy	x1
	sta	(dst),y ; dst[x] = src[y]
	ply

	inc	y1
	iny
	cpy	#8 ; font height
	bne	@loopy_odd
	rts

@loopy_even:
	ldx	y1
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y1]

	lda	(src),y

	phy
	ldy	x1
	AUX_BANK
	sta	(dst),y	; dst[x] = src[y]
	MAIN_BANK
	ply

	inc	y1
	iny
	cpy	#8
	bne	@loopy_even
	rts
.endproc

; draw 14x16 bitmap; fast but byte-aligned
;
; @param x1(modified) 0..79
; @param y1(modified) 0..191
; @param src - pointer to pixel data; 1 bit per pixel; MSB ignored; 2*8=16bytes
.proc dhgr_put16
	ldy	#0	; Y = src offset
	lda	x1
	lsr		;
	sta	x1	; x1 = x1 / 2 = dst offset
	bcc	@loopy_even

@loopy_odd:
	ldx	y1
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y1]

	; first byte: main bank
	lda	(src),y

	phy
	ldy	x1
	sta	(dst),y	; dst[x] = src[y]
	ply

	; second byte: aux bank & next address
	iny
	lda	(src),y

	ldy	x1
	iny
	AUX_BANK
	sta	(dst),y	; dst[x+1] = src[++y]
	MAIN_BANK

	inc	y1
	iny
	cpy	#8 ; font height
	bne	@loopy_odd
	rts

@loopy_even:
	ldx	y1
	lda	#<HGR_BASE
	clc
	adc	HGR_OFFSET_LO,x
	sta	dst
	lda	#>HGR_BASE
	adc	HGR_OFFSET_HI,x
	sta	dst+1	; dst = HGR_BASE + HGR_OFFSET[y1]

	; first byte: aux bank
	lda	(src),y

	phy
	ldy	x1
	AUX_BANK
	sta	(dst),y	; dst[x] = src[y]
	MAIN_BANK
	ply

	; second byte: main bank & same address
	iny
	lda	(src),y

	ldy	x1
	sta	(dst),y	; dst[x] = src[++y]

	inc	y1
	iny
	cpy	#16
	bne	@loopy_even
	rts
.endproc

; draw char
; @param x1(modified)
; @param y1(modified)
; @param ch
.proc dhgr_putc8
	lda	#<font7x8
	sta	src
	lda	#>font7x8
	sta	src+1

	lda	ch
	sec
	sbc	#$20	; A = ch - $20
	sta	ch
	lsr
	lsr
	lsr
	lsr
	lsr
	pha	; hi((ch - $20) * 8)

	lda	ch
	asl
	asl
	asl	; lo((ch - $20) * 8)

	clc
	adc	src
	sta	src

	pla
	adc	src+1
	sta	src+1

	jsr	dhgr_put8
	;jsr	dhgr_put16
	rts
.endproc


; draw text string
; @param x1(modified)
; @param y1(modified)
; @param src - pointer to string ends with zero
.proc	dhgr_puts8
	ldy	#0
@loop:
	lda	(src),y
	beq	@break
	sta	ch
	PUSH_VARS x1, y1, src, src+1
	phy
	jsr	dhgr_putc8
	ply
	PULL_VARS src+1, src, y1, x1
	iny		; next char

	lda	x1
	clc
	adc	#1	; x1+=1 foward
	sta	x1
	cmp	#(DHGR_BYTES_PER_ROW)
	bcc	@loop	; if x1 < 80

	lda	#0
	sta	x1	; x=0 carrage return

	lda	y1
	clc
	adc	#8	; y1+=8 line feed
	sta	y1
	cmp	#(DHGR_HEIGHT-8)
	bcc	@loop ; while y1 < 192-8
@break:
	rts
.endproc


.include	"softswitch.inc"
.include	"math.inc"
.include	"hgr.inc"
.include	"dhgr.inc"
.include	"4x6.inc"
.include	"7x8.inc"
;.include	"pixmap.inc"
;.include	"bitmap.inc"
