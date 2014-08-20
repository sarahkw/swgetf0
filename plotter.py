#!/usr/bin/env python2.7

import sys
import ctypes
from matplotlib import pyplot


def read_floats():
    while True:
        ret = sys.stdin.read(4)
        if ret == "":
            return
        yield ctypes.c_float.from_buffer_copy(ret).value

def main():

    x = []
    y = []
    for i, n in enumerate(read_floats()):
        if n == 0:
            continue
        x.append(i / 100.)
        y.append(n)

    pyplot.plot(x, y, '.')
    pyplot.show()


if __name__ == "__main__":
    main()
