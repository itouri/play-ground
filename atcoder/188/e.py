N, M = list(map(int, input().split()))
a_s = list(map(int, input().split()))
e_s = []

ans = -99999999999999999999

def dfs(node, a_min, a_max):
    global ans
    for e in [e for e in e_s if e[0] == node]:
        a_min = min(a_s[node], a_min)
        a_max = max(a_s[node], a_max)
        #print(node,"->",e[1], a_min, a_max)
        dfs(e[1], a_min, a_max)

    a_min = min(a_s[node], a_min)
    a_max = max(a_s[node], a_max)
    ans = max(ans, a_max - a_min)

for i in range(M):
    x, y = list(map(int, input().split()))
    e_s.append([x-1, y-1])

dfs(0, a_s[0], a_s[0])
print(ans)