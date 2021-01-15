n = int(input())
sum_a = 0
score = []

for i in range(n):
    ta, tb = map(int, input().split())
    sum_a += ta
    score.append(tb + 2*ta)

score.sort(reverse=True)
now = 0

for i in range(n+1):
    if now > sum_a:
        print(i)
        break
    now += score[i]