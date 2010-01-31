#!/usr/bin/perl -w
use strict;

foreach my $host (<>) {
	chomp $host;
	my @radios = `ssh $host ls /dev/radio\\* 2>/dev/null`;
	if (!@radios) {
		print STDERR "$host\n";
		next;
	}

	`./findradios.expect $host`;

	`scp -rp distkeys_files/bin $host:`;

	foreach my $r (@radios) {
	    chomp($r);
	    print "$host $r\n";
	}
}
