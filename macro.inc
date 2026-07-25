.macro  push	r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
	lda	r1
	pha
.endif
	push	r2, r3, r4, r5, r6, r7
.endmacro


.macro  pull	r1, r2, r3, r4, r5, r6, r7
.ifblank	r1
	.exitmacro
.else
	pla
	sta	r1
.endif
	pull	r2, r3, r4, r5, r6, r7
.endmacro

.macro	main2aux start, end, dest
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

.macro	aux2main start, end, dest
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

