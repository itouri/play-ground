import math
n = int(input()) - 1
a_s = list(map(int, input().split()))
ans = 0
weight = 1
a_s.sort()
print(a_s)
for i in range(0, math.ceil(n/2)):
    #print(i)
    ans += (2 + i) * n * (a_s[i+1] - a_s[i])
    print(ans)

for i in range(math.ceil(n/2), n)[::-1]:
    #print(i)
    ans += (2 + (n-1-i)) * n * (a_s[i+1] - a_s[i])
    print(ans)

print(ans)