#!/usr/bin/env python

from sys import argv

size = int(argv[1]) if len(argv)>1 else 10

for y in range(0,size):
    for x in range(0,size):
        n = y * size + x
	if y != 0: print "%d %d" % (n, n-size)
	if x != 0: print "%d %d" % (n, n-1)
	if x != size-1: print "%d %d" % (n, n+1)
	if y != size-1: print "%d %d" % (n, n+size)

