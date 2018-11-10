n, m = list(map(int, input().split()))
tmps = [[] for _ in range(n)]
pyxs = []

for i in range(m):
    p, y = list(map(int, input().split()))
    pyxs.append([p, y, i])

pyxs.sort(key = lambda x: x[1])

print(pyxs)

num = [0] * (n+1)
ans = [None]*m

for p, y, x in pyxs:
    ans[x] = '{:06}'.format(p) + '{:06}'.format(num[p]+1)
    num[p] += 1

for s in ans:
    print(s)