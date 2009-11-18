#!/usr/bin/perl -w
use strict;

$ENV{LD_LIBRARY_PATH} = ".";
# Find all the hostnames in this student subnet

my @hosts = ();
foreach my $a ( 154 .. 155) {
    foreach my $b ( 1 .. 254 ) {
	print STDERR ".";
	my $host = `dig +short -x 118.138.$a.$b`;	
	chomp($host);
	next unless $host;

	my $ping = `ping -w 1 -c 1 -q $host`;
	next unless $ping =~ /1 received/;

	my $result = `./distkeys.expect $host`;
	next unless $result =~ /authorized_keys/;

	my @radios = `ssh ecsetemp\@$host ls /dev/radio* 2>/dev/null`;
	if (@radios) {
	    foreach my $r (@radios) {
	    	chomp($r);
	        print "$host $r\n";
	    }
	} else {
	    print "$host\n";
        }
    }
    print STDERR "\n";
}
