.macro TXY
	phx
	ply
.endmacro

.macro TYX
	phy
	plx
.endmacro

; modified: A
.macro  PUSH r1, r2, r3, r4, r5, r6, r7
  .ifblank	r1
    .exitmacro
  .else
    lda	r1
    pha
  .endif
	PUSH r2, r3, r4, r5, r6, r7
.endmacro


; modified: A
.macro  PULL r1, r2, r3, r4, r5, r6, r7
  .ifblank	r1
    .exitmacro
  .else
    pla
    sta	r1
  .endif
	PULL r2, r3, r4, r5, r6, r7
.endmacro


.macro AUX_BANK
	sta $c055
.endmacro


.macro MAIN_BANK
	sta $c054
.endmacro


; modified: A
.macro	MAIN2AUX start, end, dest
	lda	#<start
	sta	$3c
	lda	#>start
	sta	$3d	; src start
	lda	#<end
	sta	$3e
	lda	#>end
	sta	$3f	; src end
	lda	#<dest
	sta	$42
	lda	#>dest
	sta	$43	; dest addr
	sec		; main to aux
	jsr	$c311	; auxmove
.endmacro


; modified: A
.macro	AUX2MAIN start, end, dest
	lda	#<start
	sta	$3c
	lda	#>start
	sta	$3d	; src start
	lda	#<end
	sta	$3e
	lda	#>end
	sta	$3f	; src end
	lda	#<dest
	sta	$42
	lda	#>dest
	sta	$43	; dest addr
	clc		; aux to main
	jsr	$c311	; auxmove
.endmacro

