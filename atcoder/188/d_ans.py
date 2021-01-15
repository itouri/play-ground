N, C = list(map(int, input().split()))
events = []

for i in range(N):
    a, b, c = (list(map(int, input().split())))
    a -= 1
    events.append((a, c))
    events.append((b, -c))

events.sort()

t = 0
fee = 0
ans = 0

for x, y in events:
    if x != t:
        ans += min(C, fee) * (x - t)
        t = x
    fee += y

print(ans)
