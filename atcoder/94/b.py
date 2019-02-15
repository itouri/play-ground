n, m, x = list(map(int, input().split()))
aa = list(map(int, input().split()))

l = r = 0
for a in aa:
    if a < x:
        l = l + 1
    else:
        r = r + 1
print(min(l, r))