#!/usr/bin/env python
# Topology generator by Nick Moore <nick@zoic.org>

import heapq
import random
from math import sqrt, pi, sin, cos, acos
from sys import argv, exit, stderr

# parameters

if len(argv) < 2:
    stderr.write("Usage: %s <nnodes> <nlinks> <rrange> <flags> <seed>\n" % argv[0]);
    exit(1)
    
nnodes = len(argv)>1 and int(argv[1]) or 400
nlinks = len(argv)>2 and int(argv[2]) or nnodes * 5
rrange = len(argv)>3 and float(argv[3]) or None
flags = len(argv)>4 and argv[4] or ""
seed = len(argv)>5 and argv[5] or None

random.seed(seed)

spherical = "S" in flags

# links is a heapq of tuples (-distance^2, node1, node2)
# -distance^2 because that way the longest item is on top, and
# we don't really need to bother sqrting things just to sort them.
# Note that node1 > node2 always ... each link appears only once,
# not twice, in this list.
links = []

# nodes is a list of tuples (x,y)
nodes = []

# number of links in the heapqueue: len(links) is quite slow.
qlinks = 0

for n in range(0, nnodes):
    x = random.random()
    y = random.random()
    if spherical:
	x = x * 2 * pi
        y = acos( 2 * y - 1 )
    node1 = ( x, y)
    for m, node2 in enumerate(nodes):
	if spherical:
	    x1 = cos(node1[0])*sin(node1[1])
	    y1 = sin(node1[0])*sin(node1[1])
	    z1 = cos(node1[1])
	    x2 = cos(node2[0])*sin(node2[1])
	    y2 = sin(node2[0])*sin(node2[1])
	    z2 = cos(node2[1])
            dist2 = (x1-x2)**2 + (y1-y2)**2 + (z1-z2)**2
	else:
            dist2 = (node1[0]-node2[0])**2 + (node1[1]-node2[1])**2
        
        if qlinks < nlinks:
            # We collect the first nlinks links into the heapqueue
            link = (-dist2, n, m)
            heapq.heappush(links, link)
            qlinks += 1
        elif dist2 < -links[0][0]:
            # Then once the heapqueue is full, we only collect links if they're
            # shorter than the longer link on the heapqueue already.
            link = (-dist2, n, m)
            heapq.heapreplace(links, link)
            
    nodes.append(node1)

# work out radio range from length of longest link.    
nrange = sqrt(-links[0][0])
if rrange and not spherical:
    scale = rrange / nrange
else:
    scale = 1.0

# make list of neighbours to each node.
neigh = [ [] for n in nodes ]
for link in links:
    neigh[link[1]].append(link[2])
    neigh[link[2]].append(link[1])

print "# NNODES %d" % nnodes
print "# NLINKS %d" % nlinks
print "# mean_degree %f" % (nlinks * 2.0 / nnodes)
print "# RRANGE %f" % (rrange or nrange) 
print "# field_size %f" % scale
print "# flags %s" % flags
print "# seed %s" % seed

#for n in range(0, nnodes):
#    print "%d %f %f %s" % (
#        n,
#        nodes[n][0]*scale,
#        nodes[n][1]*scale,
#        ' '.join([str(x) for x in neigh[n]]),
#    )

for n1 in range(0, nnodes):
    print "# %d\t\t\t\t%f\t%f" % (n1, nodes[n1][0] * scale, nodes[n1][1] * scale);
    for n2 in neigh[n1]:
        dist = sqrt((nodes[n1][0]-nodes[n2][0])**2 + (nodes[n1][1]-nodes[n2][1])**2)
        print "%d\t%d\t# %f" % (n1, n2, dist * scale)
 
