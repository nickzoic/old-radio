#!/usr/bin/perl -w
use strict;

print STDERR "searching ...";

foreach my $line (<>) {
	my ($host, $ip) = split /,/, $line;
	$host =~ s/\.eng$/.eng.monash.edu.au/;
	next unless $host;
	print STDERR "\n$host\t";

	my $ping = `ssh grimaud ping -w 1 -c 1 -q $host`;
	next unless $ping =~ /1 received/;
	print STDERR "PING\t";

	my $result = `./distkeys.expect $host`;
	next unless $result =~ /authorized_keys/;

	print STDERR "KEYS";

	print "$host\n";
}

print STDERR "\n... done\n";
