HGR_PAGE1_BASE = $2000
HGR_PAGE2_BASE = $4000
HGR_BASE = HGR_PAGE1_BASE

; pre-compute the hgr address for each y value(y=0..191)
; hgr_offset[y] = (y/64)*0x28 + (y%8)*0x400 + ((y/8)&7)*0x80;
;
; see also DHGR_OFFSET_X(or DIV7_TBL)

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
