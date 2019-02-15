n = int(input())
aa = [int(input()) for i in range(0, n)]
aa.sort()
up, down = 0,0 #up-> \/\/ down->/\/\
up += aa[n//2+1]
down += -1*up
for i in range(0, n//2):
    up += 
    down += 

print(max(up, down))
