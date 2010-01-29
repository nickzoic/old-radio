#!/usr/bin/env python

from sys import argv

size = int(argv[1]) if len(argv)>1 else 10

for y in range(0,size):
    for x in range(0,size):
        n = y * size + x
        if y: print "%d %d" % (n, n-size)
        if x: print "%d %d" % (n, n-1)

