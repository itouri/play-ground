n, m = list(map(int, input().split()))
ary = list(map(int, input().split()))
c = [2, 5, 5, 4, 5, 6, 3, 7, 6]
aa = []
for a in ary:
    aa.append(c[a-1])
aa.sort()
print(aa)
i = 0


def tmp():
    i = 0
    while(True):
        ans = [0] * m
        aaa = n % aa[0] + (i * aa[0])
        ans[0] = int(n / aa[0]) - i
        for j in range(1, m-1):
            if aaa >= aa[j]:
                ans[j] = int(aaa / aa[j])
                aaa = aaa % aa[j]
                if aaa == 0:
                    return ans
        i += 1


ans = tmp()
print(ans)