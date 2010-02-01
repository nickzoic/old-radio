#!/usr/bin/env python
# Topology generator by Nick Moore <nick@zoic.org>

import heapq
import random
from math import sqrt, pi, sin, cos, acos
from sys import argv, exit, stderr

# parameters

if len(argv) < 2:
    stderr.write("Usage: %s <nnodes> <minrange> <maxrange> <seed>\n" % argv[0]);
    exit(1)

nnodes = len(argv)>1 and int(argv[1]) or 400
minrange = len(argv)>2 and float(argv[2]) or 0
maxrange = len(argv)>3 and float(argv[3]) or 10
seed = len(argv)>4 and argv[4] or None

random.seed(seed)

minrange2 = minrange * minrange
maxrange2 = maxrange * maxrange

# nodes is a list of tuples (x,y)
nodes = [(1000,1000)]
links = []

minx = maxx = nodes[0][0]
miny = maxy = nodes[0][1]

print "# Generated by %s" % argv[0]
print "# Version $Id: spread_topo.py,v 1.3 2010-02-01 05:05:31 nick Exp $"
print "# NNODES %d" % nnodes
print "# MINRANGE %f" % minrange
print "# MAXRANGE %f" % maxrange
print "# seed %s" % seed

while len(nodes) < nnodes:
    x = random.random() * (2 * maxrange + maxx-minx) + minx - maxrange
    y = random.random() * (2 * maxrange + maxy-miny) + miny - maxrange
    node1 = (x, y)
    nlinks = []

    tooclose = False
    for m, node2 in enumerate(nodes):
        dist2 = (node1[0]-node2[0])**2 + (node1[1]-node2[1])**2
        if dist2 < minrange2:
            tooclose=True
            break
        if dist2 < maxrange2:
            nlinks.append((len(nodes), m, sqrt(dist2)))
    if not tooclose and nlinks:
        print "# %d	= %f, %f" % (len(nodes), x, y)        
        nodes.append((x,y))
        links += nlinks
        minx = min(x,minx)
        miny = min(y,miny)
        maxx = max(x,maxx)
        maxy = max(y,maxy)
    
print "# NLINKS %d" % len(links)
print "# mean_degree %f" % (2.0 * len(links) / len(nodes))

for n, m, d in links:
    print "%d\t%d # %f" % (n,m,d) 
