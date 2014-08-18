#!/usr/bin/env python2.7

import sys
import re
from matplotlib import pyplot


def main():
    while sys.stdin.readline().strip() != "":
        pass

    numbers = sys.stdin.read()
    numbers = re.split(r"[ \n]", numbers)

    numbers = [float(n) for n in numbers if n.strip() != ""]

    x = []
    y = []
    for i, n in enumerate(numbers):
        if n == 0:
            continue
        x.append(i / 100.)
        y.append(n)

    pyplot.plot(x, y, '.')
    pyplot.show()


if __name__ == "__main__":
    main()
