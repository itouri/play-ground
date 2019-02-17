import os
if __name__ == "__main__":
    print(os.path.join("a/b/c", "d/e"))

    # # import が分かってないとわからん
    # import a
    # print(os.path.join("a/b/c", "d/e"))

    with mock.patch('os.path.join', return_value='dummy'):
        os.path.join('a/b/c', 'd/e')