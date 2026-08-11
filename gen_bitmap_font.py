"""Convert the Bits'N'Picas PNG export to compact bitmap files."""

from pathlib import Path

from PIL import Image


INPUT_PATH = Path("5x12.png")
ENGLISH_OUTPUT_PATH = Path("eng5x12.bin")
KOREAN_OUTPUT_PATH = Path("kor10x12.bin")

EXPECTED_IMAGE_SIZE = (1157, 26)

ENGLISH_GLYPH_COUNT = 96  # 0x20 through 0x7f
ENGLISH_GLYPH_SIZE = (5, 12)
ENGLISH_ORIGIN = (2, 1)
ENGLISH_STRIDE = 6

# 19 initials * 3 forms + 21 medials + 27 finals
KOREAN_GLYPH_COUNT = 105
KOREAN_GLYPH_SIZE = (10, 12)
KOREAN_ORIGIN = (2, 14)
KOREAN_STRIDE = 11


def glyph_bits(
    image: Image.Image,
    glyph_index: int,
    origin: tuple[int, int],
    size: tuple[int, int],
    stride: int,
) -> list[int]:
    """Return one glyph's pixels in top-to-bottom, left-to-right order."""
    origin_x, origin_y = origin
    width, height = size
    glyph_x = origin_x + glyph_index * stride
    bits = []

    for y in range(origin_y, origin_y + height):
        for x in range(glyph_x, glyph_x + width):
            pixel = image.getpixel((x, y))
            if pixel not in ((0, 0, 0, 0), (0, 0, 0, 255)):
                raise ValueError(
                    f"unexpected pixel {pixel} in glyph {glyph_index} at ({x}, {y})"
                )
            bits.append(1 if pixel[3] else 0)

    return bits


def pack_bits(bits: list[int]) -> bytes:
    """Pack bits MSB first, padding the final byte with zero bits."""
    padding = (-len(bits)) % 8
    bits.extend([0] * padding)

    packed = bytearray()
    for offset in range(0, len(bits), 8):
        byte = 0
        for bit in bits[offset : offset + 8]:
            byte = (byte << 1) | bit
        packed.append(byte)
    return bytes(packed)


def convert_glyphs(
    image: Image.Image,
    glyph_count: int,
    origin: tuple[int, int],
    size: tuple[int, int],
    stride: int,
) -> bytes:
    """Extract and pack a horizontal run of glyphs."""
    output = bytearray()
    for glyph_index in range(glyph_count):
        bits = glyph_bits(image, glyph_index, origin, size, stride)
        output.extend(pack_bits(bits))
    return bytes(output)


def main() -> None:
    with Image.open(INPUT_PATH) as source:
        image = source.convert("RGBA")

    if image.size != EXPECTED_IMAGE_SIZE:
        raise ValueError(
            f"{INPUT_PATH} must be {EXPECTED_IMAGE_SIZE[0]}x"
            f"{EXPECTED_IMAGE_SIZE[1]} pixels, got {image.width}x{image.height}"
        )

    english = convert_glyphs(
        image,
        ENGLISH_GLYPH_COUNT,
        ENGLISH_ORIGIN,
        ENGLISH_GLYPH_SIZE,
        ENGLISH_STRIDE,
    )
    korean = convert_glyphs(
        image,
        KOREAN_GLYPH_COUNT,
        KOREAN_ORIGIN,
        KOREAN_GLYPH_SIZE,
        KOREAN_STRIDE,
    )

    expected_english_size = ENGLISH_GLYPH_COUNT * 8
    expected_korean_size = KOREAN_GLYPH_COUNT * 15
    if len(english) != expected_english_size:
        raise AssertionError(
            f"English output is {len(english)} bytes, expected {expected_english_size}"
        )
    if len(korean) != expected_korean_size:
        raise AssertionError(
            f"Korean output is {len(korean)} bytes, expected {expected_korean_size}"
        )

    ENGLISH_OUTPUT_PATH.write_bytes(english)
    KOREAN_OUTPUT_PATH.write_bytes(korean)

    print(f"Wrote {ENGLISH_OUTPUT_PATH} ({len(english)} bytes)")
    print(f"Wrote {KOREAN_OUTPUT_PATH} ({len(korean)} bytes)")


if __name__ == "__main__":
    main()
