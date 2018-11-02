n, aa = input(), list(map(int, input().split()))
s = sum(aa)
m = s / 2
i = 0
min = float("inf")
ans = 0
aa.pop()
for a in aa:
    s -= a
    i += a  
    dif = abs(s - m)
    if dif < min:
        min = dif
        ans = abs(s-i)
print(ans)
