sx, sy, gx, gy = list(map(int, input().split()))
gy = -gy
d = (gy - sy) / (gx - sx)
distance = sy / -d
print(sx+distance)