import copy
N = int(input())
a_s = list(map(int, input().split()))
o_a_s = copy.copy(a_s)

while True:
    for i in range(len(a_s)//2):
        if len(a_s) == 2:
            print(o_a_s.index(min(a_s[i], a_s[i+1]))+1)
            exit()

        if a_s[i] > a_s[i+1]:
            a_s.pop(i+1)
        else:
            a_s.pop(i)
        