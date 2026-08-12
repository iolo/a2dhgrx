; convert utf8 string to ucs2 string
;
; @param src utf8 string pointer. null terminated. max length = 255 bytes
; @param dst ucs2 string pointer. null terminated. max length = 255 bytes(127 wchars)
.proc utf8_to_ucs2
byte1 = x1
byte2 = y1
byte3 = x2
byte4 = y2
code = color

	ldy #0	; src index
	ldx #0	; dst index

@loop:
	lda	(src),y		; A = *src
	beq	@break

	cmp	#%11000000
	bcc	@utf8_1byte	; A < 1100000

	cmp	#%11100000
	bcc	@utf8_2byte	; A < 11100000

	cmp	#%11110000
	bcc	@utf8_3byte	; A < 11110000

	;;cmp #%11111000
	;;bcc @utf8_4byte	; A < 11111000

@unsupported:
	iny
	lda	#$3f	; '?'
	sta	code
	lda	#0
	sta	code+1

@continue:
	phy		; save y(src index)
	TXY		; x(dst index) -> y
	lda	code
	sta	(dst),y	; *dst++ = code
	iny
	lda	code+1
	sta	(dst),y	; *dst++ = code+1
	iny
	TYX		; y -> x(dst index)
	ply		; restore y(src index)
	jmp	@loop

@break:
	TXY		; x(dst index) -> y
	sta	(dst),y	; *dst++ = 0
	iny
	sta	(dst),y	; *dst = 0
	rts

@utf8_1byte:
; 0xxxxxxx -> 0x00..0x7f
	;;lda	(src),y
	;;and	#%01111111
	sta	code
	iny

	lda	#0
	sta	code+1

	jmp	@continue

@utf8_2byte:
; 110HHHLL 10LLLLLL -> 0x80..0x07ff (11bits)
	;;lda	(src),y
	and	#%00011111
	sta	byte1	; byte1 = *src++ & 000HHHLL
	iny

	lda	(src),y
	and	#%00111111
	sta	byte2	; byte2 = *src++ & 00LLLLLL
	iny

	lda	byte1
	asl
	asl
	asl
	asl
	asl
	asl
	ora	byte2
	sta	code

	lda	byte1
	lsr
	lsr
	lsr
	sta	code+1

	jmp	@continue

@utf8_3byte:
; 1110xxxx 10xxxxxx 10xxxxxx -> 0x800..0xffff (16bits)
	;;lda	(src),y
	and	#%00001111
	sta	byte1	; byte1 = *src++ & 0000HHHH
	iny

	lda	(src),y
	and	#%00111111
	sta	byte2	; byte2 = *src++ & 00HHHHLL
	iny

	lda	(src),y
	and	#%00111111
	sta	byte3	; byte3 = *src++ & 00LLLLLL
	iny

	lda	byte2
	asl
	asl
	asl
	asl
	asl
	asl
	ora	byte3
	sta	code

	lda	byte1
	asl
	asl
	asl
	asl
	sta	byte1
	lda	byte2
	lsr
	lsr
	ora	byte1
	sta	code+1

	jmp	@continue

@utf8_4byte:
; 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx -> 0x10000..0x10ffff (21bits)
	iny
	iny
	iny
	jmp	@unsupported
.endproc

; decompose ucs2 into cho, jung, jong
; @param code(modified)
; @return cho, jung, jong
.proc decompose_ucs2
;;	; ensure (syllable < 0xAC00 || syllable > 0xD7A3)
;;	lda	code+1
;;	cmp	#>KOR_CODE_BEGIN
;;	bcc	@not_hangul	; error if hi(code) < $AC
;;	cmp	#>KOR_CODE_END
;;	beq	@check_lo
;;	bcs	@not_hangul	; error if hi(code) >= $D7
;;@check_lo:
;;	lda	code
;;	cmp	#<(KOR_CODE_END+1)
;;	bcs	@not_hangul	; error if hi(code) == $D7 and lo(code) >= $A4

	lda	code
	sec
	sbc	#<KOR_CODE_BEGIN
	sta	code
	lda	code+1
	sbc	#>KOR_CODE_BEGIN
	sta	code+1		;  code -= 0xAC00;

	MATH_PUSH16 MATH_AX,MATH_BX,MATH_CX

	; jong = code % JONG_COUNTT
	; jung = (code / JONG_COUNT) % JUNG_COUNT
	; cho = (code / JONG_COUNT) / JUNG_COUNT

	; MATH_AX = code / 28, MATH_CX = code % 28
	MATH_LOAD16 MATH_AX,code
	MATH_LOAD16 MATH_BX,#JONG_COUNT
	jsr	MATH_DIV16
	MATH_STORE8 MATH_CL,jong

	; MATH_AX = (code / 28) / 21, MATH_CX = (code / 28) % 21
	MATH_LOAD16 MATH_BX,#JUNG_COUNT
	jsr	MATH_DIV16
	MATH_STORE8 MATH_CL,jung
	MATH_STORE8 MATH_AL,cho

	MATH_PULL16 MATH_CX,MATH_BX,MATH_AX
	rts

@not_hangul:
	lda #0
	sta cho
	sta jung
	sta jong
	rts
.endproc

; compose cho, jung, jong into ucs2
; @param cho
; @param jung
; @param jong
; @returns src
.proc compose_ucs2
	lda #(CHO_COUNT-1)
	cmp cho
	bcc @error	; 18 < cho
	lda #(JUNG_COUNT-1)
	cmp jung
	bcc @error	; 20 < jung
	lda #(JONG_COUNT-1)
	cmp jong
	bcc @error	; 27 < jong
@error:
	lda #$7f
	sta code
	lda #0
	sta code+1
	rts

	; code = cho * 588 + jung * 28 + jong

	; 588 = 512 + 64 + 8 + 4
	; code += cho << 2; // cho * 4
	lda cho
	asl
	asl
	clc
	tax
	adc code
	lda #0
	adc code+1
	; code += cho << 3; // cho * 8
	txa
	asl
	clc
	tax
	adc code
	lda #0
	adc code+1
	; code += cho << 6; // cho * 64
	txa
	asl
	asl
	asl
	tax
	clc
	adc code
	lda #0
	adc code+1
	; code += cho << 9; // cho * 512
	txa
	asl
	asl
	asl
	clc
	adc code
	lda #0
	adc code+1

	; 28 = 16 + 8 + 4
	; code += jung << 2; // jung * 4
	lda jung
	asl
	asl
	clc
	tax
	adc code
	lda #0
	adc code+1
	; code += jung << 3; // jung * 8
	txa
	asl
	tax
	clc
	adc code
	lda #0
	adc code+1
	; code += jung << 4; // jung * 16
	txa
	asl
	tax
	clc
	adc code
	lda #0
	adc code+1

	; code += jong;
	lda jong
	clc
	adc code
	lda #0
	adc code+1

	; code += 0xAC00
	;lda code
	;clc
	;adc #$00
	lda code+1
	adc #$AC
	sta code+1

	rts
.endproc

