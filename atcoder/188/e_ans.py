N, M = list(map(int, input().split()))
a_s = list(map(int, input().split()))
e_s = [[] for i in range(N)]

dp = [10**10] * N

ans = -10**10

for i in range(M):
    x, y = list(map(int, input().split()))
    x -= 1
    y -= 1
    e_s[x].append(y)

for i in range(N):
    ans = max(ans, a_s[i]-dp[i])
    for e in e_s[i]:
        dp[e] = min(dp[e], dp[i], a_s[i])
        print(dp)

print(ans)