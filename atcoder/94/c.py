n, xx = int(input()), list(map(int, input().split()))
xs = sorted(xx)
[print( xs[n//2] if x <= xs[n//2-1] else xs[n//2-1] ) for x in xx ]
