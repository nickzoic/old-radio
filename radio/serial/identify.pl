#!/usr/bin/perl -w
# $Id: identify.pl,v 1.1 2009-02-11 04:12:01 nick Exp $

# Uses the output of dmesg and lsusb to identify which ttyUSB* is which device.

use strict;
use Data::Dumper;

my %lookup = (
	A7RUH59A => 4,
	A7RUH71A => 5,
	A7RUH73F => 6,
	A7RUH72S => 8,
);

my %address = ();
my %device = ();

foreach (`dmesg`) {
    if (/^\[[\d. ]+\] usb (\d+)-([^:]+):.*address (\d+)/) {
        $address{"$1-$2"} = $3;
    }
    if (/^\[[\d. ]+\] usb (\d+)-([^:]+):.*ttyUSB(\d+)/) {
        my $address = $address{"$1-$2"};
        $device{"$1:$address"} = $3;
    }
    if (/^\[[\d. ]+\] usb (\d+)-([^:]+):.*disconnect/) {
        my $address = $address{"$1-$2"};
        delete $device{"$1:$address"};
        delete $address{"$1-$2"};
    }
}

foreach my $dev (keys %device) {
    my ($manu, $prod, $serial) = ('','','');
    foreach (`lsusb -v -s $dev`) {
        if (/iManufacturer\s+\d+\s+(\w+)/) { $manu = $1; }
        if (/iProduct\s+\d+\s+(\w+)/) { $prod = $1; } 
        if (/iSerial\s+\d+\s+(\w+)/) { $serial = $1; }
    }
    my $num = $lookup{$serial} || "?";
    print "/dev/ttyUSB$device{$dev} $num $serial $manu $prod\n";
}
