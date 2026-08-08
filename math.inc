.align $100
; faaast n / 7 for HGR/DHGR modes
;
; ldx n
; lda DIV7_TBL,x
.align $100
DIV7_TBL:
    .BYTE 0,0,0,0,0,0,0
    .BYTE 1,1,1,1,1,1,1
    .BYTE 2,2,2,2,2,2,2
    .BYTE 3,3,3,3,3,3,3
    .BYTE 4,4,4,4,4,4,4
    .BYTE 5,5,5,5,5,5,5
    .BYTE 6,6,6,6,6,6,6
    .BYTE 7,7,7,7,7,7,7
    .BYTE 8,8,8,8,8,8,8
    .BYTE 9,9,9,9,9,9,9
    .BYTE 10,10,10,10,10,10,10
    .BYTE 11,11,11,11,11,11,11
    .BYTE 12,12,12,12,12,12,12
    .BYTE 13,13,13,13,13,13,13
    .BYTE 14,14,14,14,14,14,14
    .BYTE 15,15,15,15,15,15,15
    .BYTE 16,16,16,16,16,16,16
    .BYTE 17,17,17,17,17,17,17
    .BYTE 18,18,18,18,18,18,18
    .BYTE 19,19,19,19,19,19,19

    .BYTE 20,20,20,20,20,20,20
    .BYTE 21,21,21,21,21,21,21
    .BYTE 22,22,22,22,22,22,22
    .BYTE 23,23,23,23,23,23,23
    .BYTE 24,24,24,24,24,24,24
    .BYTE 25,25,25,25,25,25,25
    .BYTE 26,26,26,26,26,26,26
    .BYTE 27,27,27,27,27,27,27
    .BYTE 28,28,28,28,28,28,28
    .BYTE 29,29,29,29,29,29,29

    .BYTE 30,30,30,30,30,30,30
    .BYTE 31,31,31,31,31,31,31
    .BYTE 32,32,32,32,32,32,32
    .BYTE 33,33,33,33,33,33,33
    .BYTE 34,34,34,34,34,34,34
    .BYTE 35,35,35,35,35,35,35
    .BYTE 36,36,36,36	; 252-255

; faaast n % 7 for HGR/DHGR modes
;
; ldx n
; lda MOD7_TBL,x
.align $100
MOD7_TBL:
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6

    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6

    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3,4,5,6
    .BYTE 0,1,2,3 ; 252-255

; n1 = n1 + n2
.macro ADD16 n1, n2
  clc
  lda n1
  adc n2
  sta n1
  lda n1+1
  adc n2+1
  sta n1+1
.endmacro

; n1 = n1 - n2
.macro SUB16 n1, n2
  sec
  lda n1
  sbc n2
  sta n1
  lda n1+1
.endmacro

; 8bit registers
MATH_AL = $50
MATH_AH = $51
MATH_BL = $54
MATH_BH = $55
MATH_CL = $52
MATH_CH = $53
MATH_DL = $56
MATH_DH = $57
; 16bit registers
MATH_AX = MATH_AL
MATH_BX = MATH_BL
MATH_CX = MATH_CL
MATH_DX = MATH_DL
; 32bit registers
MATH_EAX = MATH_AX
MATH_EBX = MATH_BX

; push 8bit registers to stack
; modified: A
.macro MATH_PUSH8 r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
  lda	r1
	pha
.endif
	MATH_PUSH8 r2, r3, r4, r5, r6, r7
.endmacro

; push 16bit registers to stack
; modified: A
.macro MATH_PUSH16 r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
  lda	r1
	pha
  lda	r1+1
  pha
.endif
	MATH_PUSH16 r2, r3, r4, r5, r6, r7
.endmacro

; pull(pop) 8bit registers to stack
; modified: A
.macro  MATH_PULL8 r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
  pla
  sta	r1
.endif
	MATH_PULL8 r2, r3, r4, r5, r6, r7
.endmacro

; pull(pop) 16bit registers to stack
; modified: A
.macro  MATH_PULL16 r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
	pla
  sta	r1+1
  pla
  sta	r1
.endif
	MATH_PULL16 r2, r3, r4, r5, r6, r7
.endmacro

; load 8bit register from immediate or absolute
.macro MATH_LOAD8 reg,arg
  .if (.match(.left (1, {arg}), #))
    ; immediate
    lda #<(.right (.tcount ({arg})-1, {arg})
    sta reg
  .else
    ; absolute
    lda arg
    sta reg
  .endif
.endmacro

; load 16bit register from immediate or absolute
.macro MATH_LOAD16 reg,arg
  .if (.match(.left (1, {arg}), #))
    ; immediate
    lda #<(.right (.tcount ({arg})-1, {arg}))
    sta reg
    lda #>(.right (.tcount ({arg})-1, {arg}))
    sta reg+1
  .else
    ; absolute
    lda arg
    sta reg
    lda arg+1
    sta reg+1
  .endif
.endmacro

; store 8bit register to absolute
.macro MATH_STORE8 reg,arg
  lda reg
  sta arg
.endmacro

; store 16bit register to absolute
.macro MATH_STORE16 reg,arg
  lda reg
  sta arg
  lda reg+1
  sta arg+1
.endmacro

; AX = AX + BX
.proc MATH_ADD16
  clc
  lda MATH_AL
  adc MATH_BL
  sta MATH_AL
  lda MATH_AH
  adc MATH_BH
  sta MATH_AH
  rts
.endproc

; https://codebase64.net/doku.php?id=base:16bit_division_16-bit_result
; AX = AX / BX, CX = AX % BX
.proc MATH_DIV16
dividend = MATH_AX
divisor = MATH_BX
remainder = MATH_CX
result = dividend ;save memory by reusing divident to store the result

@divide:
  lda #0	        ;preset remainder to 0
	sta remainder
	sta remainder+1
	ldx #16	        ;repeat for each bit: ...

@divloop:
  asl dividend	;dividend lb & hb*2, msb -> Carry
	rol dividend+1
	rol remainder	;remainder lb & hb * 2 + msb from carry
	rol remainder+1
	lda remainder
	sec
	sbc divisor	;substract divisor to see if it fits in
	tay	        ;lb result -> Y, for we may need it later
	lda remainder+1
	sbc divisor+1
	bcc @skip	;if carry=0 then divisor didn't fit in yet

	sta remainder+1	;else save substraction result as new remainder,
	sty remainder
	inc result	;and INCrement result cause divisor fit in 1 times

@skip:
  dex
	bne @divloop
	rts
.endproc
