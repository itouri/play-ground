n, m = list(map(int, input().split()))
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
