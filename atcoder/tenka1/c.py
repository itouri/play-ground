n = int(input())
aa = [int(input()) for i in range(0, n)]
aa.sort()
ana = [0] * n
ana[0] = aa[n//2-1]
ana[n-1] = aa[n//2]
for i in range(0, (n//2)-1):
    ana[2+2*i] = aa[i]
    ana[1+2*i] = aa[i + (n//2)+1]
if n % 2 != 0:
    ana[n-2] = aa[n-1]
if n == 3:
    ana[0] = aa[1]
    ana[1] = aa[0]
    ana[2] = aa[2]

a = 0
for i in range(1, len(ana)):
    a += abs(ana[i-1] - ana[i])

print(a)
#print(a, ana)

##import itertools

#seq = [3,1,4,1,5,9]
# seq = [1,2,3,4,5,6,7,8,9]
# lls = list(itertools.permutations(seq))
# mx = 0
# m = []
# for ll in lls:
#     a = 0
#     for i in range(1, len(ll)):
#         a += abs(ll[i-1] - ll[i]) 
#     if a > mx:
#         mx = max(a, mx)
#         m = ll
#print(mx, m)