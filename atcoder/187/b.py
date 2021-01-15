n = int(input())
x = []
y = []
for i in range(n):
    temp_x, temp_y = input().split()
    x.append(int(temp_x))
    y.append(int(temp_y))

ans = 0
for i in range(n):
    for j in range(i+1, n):
        d = (y[j] - y[i]) / (x[j] - x[i])
        if -1 <= d and d <= 1:
            ans += 1

print(ans)