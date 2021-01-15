import math
def solve(n, s, k):
    g = math.gcd(n, k)
    if s % g != 0:
        return -1
    n //= g
    s = n - s // g
    k //= g

    ans = (s * pow(k, -1, n)) % n
    return ans


if __name__ == "__main__":    
    t = int(input())
    for i in range(t):
        n, s, k = list(map(int, input().split()))
        print(solve(n, s, k))
        