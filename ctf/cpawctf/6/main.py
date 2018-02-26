f = open('text.txt')
e_txt = f.read()
f.close()

key = 3

txt = ""
for c in list(e_txt):
    if 'A' <= c <= 'Z':
        txt += chr((ord(c) - ord('A') - key) % 26 + ord('A'))
    elif 'a' <= c <= 'z':
        txt += chr((ord(c) - ord('a') - key) % 26 + ord('a'))
    else:
        txt += c
print(txt)