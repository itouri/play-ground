a, b = input().split()
sa = sb = 0
for i in range(3):
    sa += int(a[i])
    sb += int(b[i])
print(sa if sa > sb else sb)