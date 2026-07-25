	.org	$4000
	.pc02	; use 64c02 instructions

; zero-page variables
x1	=	$fa
y1	=	$fb
x2	=	$fc
y2	=	$fd
color	=	$fe
ch	=	$ff
src	=	$ce 	; pointer to source address (2 bytes)
dst	=	$eb 	; pointer to destination address (2 bytes)

; dhgr routines
dhgr_init	=	$6000
dhgr_exit	=	$6003
dhgr_cls	=	$6006
dhgr_plot	=	$6009
dhgr_hline	=	$600c
dhgr_vline	=	$600f
dhgr_rect	=	$6012
dhgr_fillrect	=	$6015
dhgr_pixmap	=	$6018
dhgr_bitmap	=	$601b
dhgr_putc	=	$601e
dhgr_puts	=	$6021

; applesoft basic tokens
bas_gr		=	$88
bas_text	=	$89
bas_plot	=	$8d
bas_hlin	=	$8e
bas_vlin	=	$8f
bas_draw	=	$94
bas_xdraw	=	$95
bas_htab	=	$96
bas_home	=	$97
bas_color	=	$a0
bas_vtab	=	$a2
bas_print	=	$ba
bas_to		=	$c1
bas_at		=	$c5

bas_synerr	=	$dec9

; applesoft basic routines and variables
bas_valtyp	=	$11	; flags for last FAC operation($00=num, $ff=str) (2b)
bas_linnum	=	$50	; line number (2b)
bas_varpnt	=	$83	; pointer to last-used variable's value (2b)
bas_fac		=	$9d	; floating point accumulator (6b)
bas_chrget	=	$b1
bas_chrgot	=	$b7
bas_ampvec	=	$03f5
bas_chkcom	=	$debe
bas_frmnum	=	$dd67	; Evaluate expression, make sure it is numeric
bas_frmevl	=	$dd7b	; Evaluate the expression at TXTPTR, leaving the result in FAC.
				; Works for both string and numeric expressions.
bas_mkint	=	$e108	; Convert FAC to integer. Must be positive and less than 32768.
bas_getbyt	=	$e6f8	; Evaluate expression at TXTPTR, and convert it to single byte in X-reg.
bas_getadr	=	$e752	; Convert FAC to a 16-bit value in LINNUM.

; entry point
	lda	#$4c		; jmp opcode
	sta	bas_ampvec
	lda	#<dhgr_ampvec
	sta	bas_ampvec+1
	lda	#>dhgr_ampvec
	sta	bas_ampvec+2
	rts


.proc	dhgr_ampvec
	ldy	#0
@loop:
	cmp	tokens,y
	beq	@match
	iny
	cpy	#num_tokens
	bne	@loop
	jmp	bas_synerr

@match:
	jsr	bas_chrget	; consume token
	lda	amp_lo,y
	sta	@amp_addr
	lda	amp_hi,y
	sta	@amp_addr+1
	jmp	(@amp_addr)
	rts

@amp_addr:
	.word	0
.endproc


; &GR - dhgr_init
.proc	amp_gr
	jsr	dhgr_init
	rts
.endproc


; &TEXT - dhgr_exit
.proc	amp_text
	jsr	dhgr_exit
	rts
.endproc


; &COLOR=color
.proc	amp_color
	jsr	bas_getbyt
	stx	color
	rts
.endproc


; &HTAB x1
.proc   amp_htab
	jsr	bas_getbyt
	stx	x1
	rts
.endproc


; &HTAB y1
.proc   amp_vtab
	jsr	bas_getbyt
	stx	y1
	rts
.endproc


; &HOME - dhgr_cls
.proc	amp_home
	jsr	dhgr_cls
	lda	#0
	sta	x1
	sta	y1
	sta	x2
	sta	y2
	lda	color
	lda	ch
	rts
.endproc


; &PLOT x,y - dhgr_plot
.proc	amp_plot
	jsr	_get_x1y1
	jsr	dhgr_plot
	rts
.endproc


; &HLIN x1,x2 AT y1 - dhgr_hline
.proc	amp_hlin
	jsr	bas_getbyt	; get x1
	stx	x1
	jsr	bas_chkcom
	jsr	bas_getbyt	; get x2
	stx	x2
	jsr	_check_bas_at
	jsr	bas_getbyt	; get y1
	stx	y1
	jsr	dhgr_hline
	rts
@err:
	jmp	bas_synerr
.endproc


; &VLIN y1,y2 AT x1 - dhgr_vline
.proc	amp_vlin
	jsr	bas_getbyt	; get y1
	stx	y1
	jsr	bas_chkcom
	jsr	bas_getbyt	; get y2
	stx	y2
	jsr	_check_bas_at
	jsr	bas_getbyt	; get x1
	stx	x1
	jsr	dhgr_vline
	rts
.endproc


; &R x1,y1,x2,y2 - dhgr_rect
.proc	amp_r
	jsr	_get_x1y1x2y2
	jsr	dhgr_rect
	rts
.endproc


; &F x1,y1,x2,y2 - dhgr_fillrect
.proc	amp_f
	jsr	_get_x1y1x2y2
	jsr	dhgr_fillrect
	rts
.endproc


; &XDRAW src AT x1,y1,x2,y2 - dhgr_pixmap
.proc	amp_xdraw
	jsr	_get_src
	jsr	_check_bas_at
	jsr	_get_x1y1x2y2
	jsr	dhgr_pixmap
	rts
.endproc


; &DRAW src AT x1,y1,x2,y2 - dhgr_bitmap
.proc	amp_draw
	jsr	_get_src
	jsr	_check_bas_at
	jsr	_get_x1y1x2y2
	jsr	dhgr_bitmap
	rts
.endproc


; &PRINT expr - dhgr_putc for number expr., dhgr_puts for string expr.
.proc	amp_print
	jsr	bas_frmevl
	bit	bas_valtyp
	bmi	@is_str
	; number
	jsr	bas_mkint
	ldx	bas_fac+4
	stx	ch
	jsr	dhgr_putc
	rts

@is_str:
	; applesoft basic string(length addrlo addrhi) to c string(null terminated)
	lda	bas_fac+1
	sta	src
	lda	bas_fac+2
	sta	src+1
	ldy	bas_fac		; length of string
	lda	(src),y		; save original char
	pha
	phy
	lda	#0
	sta	(src),y		; temp. set 0 to end of string
	jsr	dhgr_puts
	ply
	pla
	sta	(src),y		; restore original char
	rts
.endproc


.proc	_get_src
	jsr	bas_frmnum
	jsr	bas_getadr
	lda	bas_linnum
	sta	src
	lda	bas_linnum+1
	sta	src+1
	rts
.endproc


.proc	_get_x1y1
	jsr	bas_getbyt	; get x1
	stx	x1
	jsr	bas_chkcom
	jsr	bas_getbyt	; get y1
	stx	y1
	rts
.endproc


.proc	_get_x1y1x2y2
	jsr	bas_getbyt	; get x1
	stx	x1
	jsr	bas_chkcom
	jsr	bas_getbyt	; get y1
	stx	y1
	jsr	bas_chkcom
	jsr	bas_getbyt	; get x2
	stx	x2
	jsr	bas_chkcom
	jsr	bas_getbyt	; get y2
	stx	y2
	rts
.endproc


.proc	_check_bas_at
	jsr	bas_chrgot	; expect 'AT'
	cmp	#bas_at
	bne	@err
	jsr	bas_chrget	; consume 'AT'
	rts
@err:
	jmp	bas_synerr
.endproc


tokens:
	.byte bas_color, bas_htab, bas_vtab
	.byte bas_xdraw, bas_draw
	.byte 'F', 'R'
	.byte bas_vlin, bas_hlin, bas_plot
	.byte bas_print, bas_home, bas_text, bas_gr
tokens_end:

num_tokens	=	tokens_end - tokens

amp_lo:
	.byte <amp_color, <amp_htab, <amp_vtab
	.byte <amp_xdraw, <amp_draw
	.byte <amp_f, <amp_r
	.byte <amp_vlin, <amp_hlin, <amp_plot
	.byte <amp_print, <amp_home, <amp_text, <amp_gr

amp_hi:
	.byte >amp_color, >amp_htab, >amp_vtab
	.byte >amp_xdraw, >amp_draw
	.byte >amp_f, >amp_r
	.byte >amp_vlin, >amp_hlin, >amp_plot
	.byte >amp_print, >amp_home, >amp_text, >amp_gr
