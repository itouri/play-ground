_player_num = 0
_player_name = []
_player_vlaue = []
_longest_name_len = 0

def display_status():
    print()
    for i in range(_player_num):
        print(i+1, _player_name[i].ljust(_longest_name_len), _player_vlaue[i])
    print()

def input_player_name():
    global _player_vlaue
    global _player_name
    print("please input player names")
    _player_vlaue = [15000 for i in range(_player_num)]
    _player_name = ["" for i in range(_player_num)]
    for i in range(_player_num):
        print("Player"+str(i+1)+": ")
        _player_name[i] = input()

def game():
    while True:
        a = []
        try:
            a = list(map(int, input().split()))
        except ValueError:
            print("not invalid input")
            continue
        
        if len(a) == 2:
            _player_vlaue[a[0]-1] += a[1]
        elif len(a) == 3:
            _player_vlaue[a[0]-1] -= a[2]
            _player_vlaue[a[1]-1] += a[2]
        else:
            print("format is not valid")
            continue
        
        display_status()

def init():
    print("please input player number")
    global _player_num
    _player_num = int(input())

    c = "y"
    while c == "y":
        input_player_name()
        print("need retry? y/n")
        c = input()

    global _longest_name_len
    _longest_name_len = max(map(len, _player_name))

if __name__ == '__main__':
    init()
    display_status()
    game()
