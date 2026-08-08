; Hangul Input/Output Program for Apple //e Double Hi-Res Graphics
	.org	$4000

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
	beq	@break ; jmp always

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
	; fall through

@break:
	rts
.endproc


; draw char with 7x8 font
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


; draw text string with 7x8 font
; @param x1(modified)
; @param y1(modified)
; @param src - pointer to string ends with zero
.proc	dhgr_puts8
	ldy	#0
@loop:
	lda	(src),y
	beq	@break
	sta	ch
	phy
	jsr	dhgr_putc8
	ply
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

font7x8:
	.incbin	"font7x8.bin"
