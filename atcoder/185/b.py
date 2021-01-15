N, M, T = list(map(int, input().split()))
a_s = []
b_s = []
now = 0

for i in range(M):
    a, b = list(map(int, input().split()))
    a_s.append(a)
    b_s.append(b)

for i in range(M):
    if i == 0:
        now = N - a_s[i]
    else:
        now = now - (a_s[i] - b_s[i-1])
        
    if now <= 0:
        print("No")
        exit()
    
    now += (b_s[i] - a_s[i])
    now = min(N, now)


if now - (T - b_s[-1]) <= 0:
    print("No")
    exit()

print("Yes")