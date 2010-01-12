#!/usr/bin/perl
use strict;
use List::Util qw(min max);


my %edges = ();

while (<>) {
	next if /^#/;
	my ($n1, $n2, $w) = split;
	($n1, $n2) = ($n2, $n1) if $n2 < $n1;
	$w ||= 1.0;
	$edges{$n1}{$n2} += $w;
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
