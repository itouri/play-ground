s = input()
print(len(s) - abs(s.count('0') - s.count('1')))