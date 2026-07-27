FONT_WIDTH	= 4
FONT_HEIGHT	= 6
FONT_CODE_BEGIN = $20
FONT_CODE_END = $7F
FONT_GLYPH_BYTES = 3
FONT_GLYPH_COUNT= 96

; 4x6 font
; (bounding box: 3x5)
; 6 rows in 3 bytes; 2 rows in a byte; 4 pixels in a row
font4x6:
	.byte	$00,$00,$00
	.byte	$44,$40,$40
	.byte	$AA,$00,$00
	.byte	$AE,$AE,$A0
	.byte	$6C,$E6,$C0
	.byte	$A2,$48,$A0
	.byte	$48,$4A,$60
	.byte	$44,$00,$00
	.byte	$48,$88,$40
	.byte	$42,$22,$40
	.byte	$0A,$4A,$00
	.byte	$04,$E4,$00
	.byte	$00,$04,$80
	.byte	$00,$E0,$00
	.byte	$00,$04,$40
	.byte	$22,$48,$80
	.byte	$6A,$AA,$C0
	.byte	$4C,$44,$E0
	.byte	$C2,$48,$E0
	.byte	$C2,$42,$C0
	.byte	$AA,$E2,$20
	.byte	$E8,$C2,$C0
	.byte	$68,$EA,$C0
	.byte	$E2,$44,$40
	.byte	$6A,$EA,$C0
	.byte	$6A,$E2,$C0
	.byte	$04,$04,$00
	.byte	$04,$04,$80
	.byte	$24,$84,$20
	.byte	$0E,$0E,$00
	.byte	$84,$24,$80
	.byte	$C2,$40,$40
	.byte	$4E,$E8,$60
	.byte	$4A,$EA,$A0
	.byte	$CA,$CA,$C0
	.byte	$68,$88,$60
	.byte	$CA,$AA,$C0
	.byte	$E8,$C8,$E0
	.byte	$E8,$C8,$80
	.byte	$68,$AA,$60
	.byte	$AA,$EA,$A0
	.byte	$E4,$44,$E0
	.byte	$22,$2A,$40
	.byte	$AA,$CA,$A0
	.byte	$88,$88,$E0
	.byte	$AE,$EA,$A0
	.byte	$CA,$AA,$A0
	.byte	$4A,$AA,$40
	.byte	$CA,$C8,$80
	.byte	$4A,$AC,$60
	.byte	$CA,$CA,$A0
	.byte	$68,$42,$C0
	.byte	$E4,$44,$40
	.byte	$AA,$AA,$E0
	.byte	$AA,$AA,$40
	.byte	$AA,$EE,$A0
	.byte	$AA,$4A,$A0
	.byte	$AA,$44,$40
	.byte	$E2,$48,$E0
	.byte	$C8,$88,$C0
	.byte	$88,$42,$20
	.byte	$62,$22,$60
	.byte	$4A,$00,$00
	.byte	$00,$00,$E0
	.byte	$42,$00,$00
	.byte	$06,$AA,$60
	.byte	$8C,$AA,$C0
	.byte	$06,$88,$60
	.byte	$26,$AA,$60
	.byte	$04,$AC,$60
	.byte	$24,$E4,$40
	.byte	$06,$A6,$2C
	.byte	$8C,$AA,$A0
	.byte	$40,$44,$40
	.byte	$40,$44,$48
	.byte	$8A,$CC,$A0
	.byte	$44,$44,$60
	.byte	$0A,$EA,$A0
	.byte	$0C,$AA,$A0
	.byte	$04,$AA,$40
	.byte	$0C,$AA,$C8
	.byte	$06,$AA,$62
	.byte	$0C,$A8,$80
	.byte	$06,$82,$C0
	.byte	$4E,$44,$20
	.byte	$0A,$AA,$60
	.byte	$0A,$AA,$40
	.byte	$0A,$AE,$A0
	.byte	$0A,$44,$A0
	.byte	$0A,$A6,$2C
	.byte	$0E,$28,$E0
	.byte	$64,$84,$60
	.byte	$44,$44,$40
	.byte	$C4,$24,$C0
	.byte	$6C,$00,$00
	.byte	$A4,$A4,$A0
