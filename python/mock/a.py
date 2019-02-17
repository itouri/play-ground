import os
def dummy(*args, **kwargs):
    return "dummy"

os.path.join = dummy
