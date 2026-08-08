text = "가나다힣"

print(f"{'글자':<3} | {'UTF-8 (10진수)':<15} | {'UCS-2 상위':<8} | {'UCS-2 하위':<8}")
print("-" * 55)

for char in text:
    # UTF-8 10진수 리스트
    utf8_dec = list(char.encode('utf-8'))

    # UCS-2 코드 포인트 추출
    code_point = ord(char)

    # 비트 연산으로 상위/하위 8비트 분리
    high_byte = code_point >> 8       # 상위 8비트 추출
    low_byte = code_point & 0xFF      # 하위 8비트 추출

    print(f"{char:<4} | {str(utf8_dec):<15} | {high_byte:<10} | {low_byte:<10}")

