from copy import deepcopy

_player_num = 0
_player_name = []
_player_vlaue = []
_longest_name_len = 0

_undo_player_vlaue = []

def undo():
    global _player_vlaue
    global _undo_player_vlaue
    if (len(_undo_player_vlaue) == 0):
        print("undo only once")
        return
    _player_vlaue = deepcopy(_undo_player_vlaue)
    _undo_player_vlaue = []
    
def push_pv():
    global _player_vlaue
    global _undo_player_vlaue
    _undo_player_vlaue = deepcopy(_player_vlaue)

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
    global _player_vlaue
    while True:
        a = []
        try:
            a = list(map(int, input().split()))
        except ValueError:
            print("invalid input")
            continue
        
        if len(a) == 1 and a[0] == 0:
            undo()
        elif len(a) == 2:
            push_pv()
            _player_vlaue[a[0]-1] += a[1]
        elif len(a) == 3:
            push_pv()
            _player_vlaue[a[0]-1] -= a[2]
            _player_vlaue[a[1]-1] += a[2]
        else:
            print("invalid format")
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
