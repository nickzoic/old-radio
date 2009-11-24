#!/usr/bin/perl -w
use strict;

foreach my $host (<>) {
	chomp $host;
	my @radios = `ssh $host ls /dev/radio\\* 2>/dev/null`;
	next unless @radios;

	`./findradios.expect $host`;
	foreach my $r (@radios) {
	    chomp($r);
	    print "$host $r\n";
	}
}
