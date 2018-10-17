n, aa = int(input()), list(map(int, input().split()))
l = r = max(aa)
for a in aa:
    if a == l:
        continue
    if abs(a - l/2) < abs(r - l/2):
        r = a
print(l, r)
