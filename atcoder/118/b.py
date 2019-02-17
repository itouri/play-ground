n, m = list(map(int, input().split()))
a = list(map(int, input().split()))
a = a[1:]
for i in range(n-1):
    t = list(map(int, input().split()))
    a = set(a) & set(t[1:])
print(len(a))