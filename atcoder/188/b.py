N = int(input())
a_s = list(map(int, input().split()))
b_s = list(map(int, input().split()))

ans = 0
for i in range(N):
    ans += a_s[i] * b_s[i]

print("Yes" if ans == 0 else "No")