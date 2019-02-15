def zero(s):
    if s[0] == "0":
        return s[1:]

def one(s):
    if s[0] == "1":
        return s[1:]

def rule_seq(s, rules):
    if s == None or not rules:
        return s
    else:
        return rule_seq(rules[0](s), rules[1:])

if __name__ == '__main__':
    print(rule_seq('0101',[zero,one,zero]))
