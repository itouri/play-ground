a, b, k = list(map(int, input().split()))
kk = []
for i in range(1, 101):
    if a % i == 0 and b % i == 0:
        kk.append(i)
print(kk[-k])
