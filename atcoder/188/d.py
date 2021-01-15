N, C = list(map(int, input().split()))
table = []
marged_table = []
ab_list = []

ans = 0

for i in range(N):
    a, b, c = (list(map(int, input().split())))
    table.append([a, c])
    table.append([b+1, -c])
    ab_list.append(a)
    ab_list.append(b+1)

ab_list = list(set(ab_list))
sorted(ab_list)

for ab in ab_list:
    s = sum(e[1] for e in table if e[0] == ab)
    marged_table.append([ab, s])

time = 0
value = 0
for t, v in marged_table:
    if time == 0:
        time = t
        value = v
        continue

    ans += (t - time) * min(value, C)
    value = v + value
    time = t

print(ans)
