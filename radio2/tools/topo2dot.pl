#!/usr/bin/perl

print "graph foo {\n";

while (<>) {
	next unless /^(\d+)\s+(\d+)/;
	next unless $2 > $1;
	print "\t$1 -- $2;\n";
}

print "}\n";
