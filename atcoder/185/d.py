import math
N, M = list(map(int, input().split()))
a_s = list(map(int, input().split()))
a_s.append(N+1)
a_s.sort()


if N == M:
    print(0)
    exit()
if M == 0:
    print(1)
    exit()

ds = []
prob_a = 0
for a in a_s:
    dist = a - prob_a - 1
    if dist != 0:
        ds.append(dist)
    prob_a = a

# gcd = ds[0]
# for d in ds:
#     gcd = math.gcd(gcd, d)

# ans = 0
# for d in ds:
#     ans += d / gcd

ans = 0
min_ds = min(ds)
for d in ds:
    ans += math.ceil(d / min_ds)

print(int(ans))
