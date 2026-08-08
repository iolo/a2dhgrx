# decompose cho,jung,jong from hangul syllables
for code in range(0xAC00, 0xD7A4, 0x100):
    syllable = (code - 0xAC00)
    jong = syllable % 28
    jung = (syllable // 28) % 21
    cho = (syllable // 28) // 21
    cho_ch=chr(0x1100+cho)
    jung_ch=chr(0x1161+jung)
    jong_ch=chr(0x11A8+jong) if jong > 0 else ''
    print(f"Syllable: {code:x}({chr(code)}) {cho}({cho_ch}) {jung}({jung_ch}), {jong}({jong_ch})")
