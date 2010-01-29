#!/usr/bin/perl
use strict;
use List::Util qw(min max);

my %edges = ();

while (<>) {
	next if /^#/;
	my ($n1, $n2, $p12, $p21) = split;
	$p12 ||= 1.0;
	$p21 ||= 1.0;
	$edges{$n1}{$n2} = $p12 + $p21;
}


print "graph foo {\n";
foreach my $n1 (keys %edges) {
    foreach my $n2 (keys %{$edges{$n1}}) {
	my $w = $edges{$n1}{$n2};
	my $l = 2.0 / $w;
        print "\t$n1 -- $n2 [ len=$l ];\n";
    }
}
print "};\n";
