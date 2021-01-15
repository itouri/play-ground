N = int(input())

depth = [-1] * N

depth[0] = 0
dp = [0] * N

a = [0] * N
b = [0] * N

es = [list() for i in range(N)]

def depthDFS(a, d):
    depth[a] = d
    for next in es[a]:
        if depth[next] == -1:
            depthDFS(next, d+1)


def imosDFS(a, now):
    now += dp[a]
    dp[a] = now
    for next in es[a]:
        if depth[next] > depth[a]:
            imosDFS(next, now)


for i in range(N-1):
    ta, tb = map(int, input().split())
    a[i] = ta
    b[i] = tb
    a[i] -= 1
    b[i] -= 1
    es[a[i]].append(b[i])
    es[b[i]].append(a[i])

# print(es)

depthDFS(0, 0)
# print(depth)

Q = int(input())
for i in range(Q):
    t, e, x = map(int, input().split())
    e -= 1

    va = vb = 0

    if t == 1:
        va = a[e]
        vb = b[e]
    else:
        va = b[e]
        vb = a[e]

    if depth[va] < depth[vb]:
        dp[0] += x
        dp[vb] -= x
    else:
        dp[va] += x
    # print(dp)

imosDFS(0, 0)

# print("---------------------")
for dps in dp:
    print(dps)