import functools, fractions
n, a = input(), list(map(int, input().split()))
print(functools.reduce(fractions.gcd, a))