import math

n = int(input())
a = []
b = []
score = []

for i in range(n):
    ta, tb = map(int, input().split())
    a.append(ta)
#    b.append(tb)
    score.append(tb + 2*ta)

sum_a = sum(a)
#sum_b = sum(b)

#half = math.ceil((sum_a + sum_b) / 2)
score.sort(reverse=True)

now = 0

for i in range(n+1):
    if now > sum_a:
        print(i)
        break

    now += score[i]