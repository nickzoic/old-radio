#!/usr/bin/perl -w
# $Id: runmany.pl,v 1.1 2009-11-24 03:20:33 nick Exp $

use strict;
use File::Temp qw'tempfile tempdir';

my $program = shift;
my $args = join ' ', @ARGV;

my @remotes=qw'cthulhu 10.107.2.104';

my @radios = split "\n", `ls -1 /dev/radio*`;

for my $remote (@remotes) {
    push @radios, map {
	"$remote:$_"
    } split "\n", `ssh $remote 'ls -1 /dev/radio*'`;
}

printf STDERR "FOUND %d RADIOS!\n", scalar @radios;

my $tempdir = tempdir( CLEANUP => 1 );

my %waitpid = ();
for my $radio (@radios) {
    if (my $pid = fork()) {
	$waitpid{$pid} = 1;
    } else {
        if ($radio =~ m{([^:]+):(/dev/radio(\d+))}) {
           print STDERR "ssh $1 $program $2 $args $3 > $tempdir/$$.log\n";
           exec("ssh $1 $program $2 $args $3 > $tempdir/$$.log");
        } elsif ($radio =~ m{/dev/radio(\d+)} ) {
	   print STDERR "$program $radio $args $1 > $tempdir/$$.log\n";
	   exec("$program $radio $args $1 > $tempdir/$$.log");
        }
    }
}

do {
	print STDERR "Waiting ...\n";
} while (wait > 0);

print STDERR "Sorting ...\n";

system("cat $tempdir/*.log | sort -n");

print STDERR "Done.\n"; 
