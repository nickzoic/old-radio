#!/usr/bin/perl -w
# $Id: runmany.pl,v 1.2 2009-11-24 04:05:39 nick Exp $

use strict;
use File::Temp qw'tempfile tempdir';
use List::Util qw'min';

my $program = shift;
my $args = join ' ', @ARGV;

my @radios = map { [ /(\w+)\s+(/dev/radio(\d+))/ ] } <>;
my $min_id = min( map { $_->[2] } @radios );

printf STDERR "RUNNING %d RADIOS!\n", scalar @radios;

my $tempdir = tempdir( CLEANUP => 1 );

my %waitpid = ();
for my $radio (@radios) {
    if (my $pid = fork()) {
	$waitpid{$pid} = 1;
    } else {
	my ($host, $device, $id) = @$radio;
	$id = 0 if $id == $min_id;
        print STDERR "ssh $host $program $device $id $args > $tempdir/$$.log\n";
        exec("ssh $host $program $device $id $args > $tempdir/$$.log");
    }
}

do {
	print STDERR "Waiting ...\n";
} while (wait > 0);

print STDERR "Sorting ...\n";

system("cat $tempdir/*.log | sort -n");

print STDERR "Done.\n"; 
