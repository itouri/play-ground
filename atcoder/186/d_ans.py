n = int(input())
a_s = list(map(int, input().split()))
a_s.sort()
ans = 0
sum_a = 0

for i in range(0, len(a_s)-1):
    sum_a = ((a_s[i+1] - a_s[i]) * (i+1)) + sum_a
    ans += sum_a

print(ans)