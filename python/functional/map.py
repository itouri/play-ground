import random

name_length = map(len, ["abc","bdc","cdf"])
print(name_length)

squares = map(lambda x: x * x,[0,1,2,3,4])
print(squares)

names = ["aaa","bbb","ccc"]
# new_names = map(lambda x: random.choice(["AAA","BBB","CCC"]),names)
new_names = map(hash, names)
print(new_names)

sum = reduce(lambda a, x: a + x, [0,1,2,3,4])
print(sum)

people = [{'name': 'Mary', 'height': 160},
        {'name': 'Isla', 'height': 80},
        {'name': 'Sam'}]
people = filter(lambda x: 'height' in x, people)
heights = map(lambda x: x['height'], people)
if len(heights) > 0:
    ave = reduce(lambda a, x: a + x, heights) / len(heights)
print(ave)