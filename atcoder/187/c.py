n = int(input())
s = []
s_exp = []

for i in range(n):
    tmp_s = input()
    s.append(tmp_s) if tmp_s[0] != "!" else s_exp.append(tmp_s[1:])

satis = set(s) & set(s_exp)
print(satis.pop() if len(satis) != 0 else "satisfiable")