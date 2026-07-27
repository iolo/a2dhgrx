.macro  PUSH_VARS r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
	lda	r1
	pha
.endif
	PUSH_VARS r2, r3, r4, r5, r6, r7
.endmacro


.macro  PULL_VARS r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
	pla
	sta	r1
.endif
	PULL_VARS	r2, r3, r4, r5, r6, r7
.endmacro

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

