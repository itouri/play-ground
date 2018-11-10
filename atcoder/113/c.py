n, m = list(map(int, input().split()))
<<<<<<< HEAD
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
=======
tmp = [[0 for i in range(1)] for j in range(m)]
pys = []
for i in range(0, m):
    p, y = list(map(int, input().split()))
    tmp[p].append(y)
    pys.append([p, y])

ttmp = [[0 for i in range(1)] for j in range(m)]

for i in range(m):
    ttmp[i] = sorted(tmp[i])

for py in pys:
    p, y = py
    yindex = ttmp[p].index(y)
    print('{:06}'.format(p) + '{:06}'.format(yindex))
>>>>>>> 726c3bff0bf977c8b396da028e8ce3f28358b3fb
