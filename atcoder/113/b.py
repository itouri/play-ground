n = int(input())
t, a = list(map(int, input().split()))
hs = list(map(int, input().split()))

min_diff = 9999999999999
ans = -1

for i in range(0, n):
    tmp = abs(a - (t - hs[i] * 0.006))
    if tmp < min_diff:
        min_diff = tmp
        ans = i + 1
print(ans)