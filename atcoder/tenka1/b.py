a, b, k = list(map(int, input().split()))
i = 0
while(True):
    if i == k:
        break
    i += 1
    if a % 2 != 0:
        a -= 1
    b +=  a / 2
    a //= 2

    if i == k:
        break
    i += 1
    if b % 2 != 0:
        b -= 1
    a += b / 2
    b //= 2
print(int(a), int(b))