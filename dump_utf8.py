s = "Hello,한글! 가나다라"
print(s)
print('## ucs2');
b = s.encode('utf-16')
print('- len: ', len(b))
print('- hex: ' + ' '.join(f'{byte:02x}' for byte in b))
print('- dec: ' + ' '.join(f'{byte}' for byte in b))
print('- hex(word): ' + ' '.join(f'{ord(c):04x}' for c in s))
print('- dec(word): ' + ' '.join(f'{ord(c)}' for c in s))

print('## utf8');
b = s.encode('utf-8')
print('- len: ', len(b))
print('- hex: ' + ' '.join(f'{byte:02x}' for byte in b))
print('- dec: ' + ' '.join(f'{byte}' for byte in b))



