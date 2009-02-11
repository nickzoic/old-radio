#!/usr/bin/perl -w
# $Id: identify.pl,v 1.2 2009-02-11 06:36:38 nick Exp $

# Uses the output of dmesg and lsusb to identify which ttyUSB* is which device.

use strict;

# Map of serial numbers to the numbers I've written on the boards in texta.

my %serials = (
    A7RUH59A => 4,
    A7RUH71A => 5,
    A7RUH73F => 6,
    A7RUH72S => 8,
);

my %address = ();
my %device = ();

# Run through dmesg to determine current state of devices and construct
# a map of bus:devnum to device name

foreach (`dmesg`) {
    if (/^\[[\d. ]+\] usb (\d+)-([^:]+):.*address (\d+)/) {
        $address{"$1-$2"} = $3;
    }
    if (/^\[[\d. ]+\] usb (\d+)-([^:]+):.*(ttyUSB\d+)/) {
        my $address = $address{"$1-$2"};
        $device{"$1:$address"} = $3;
    }
    if (/^\[[\d. ]+\] usb (\d+)-([^:]+):.*disconnect/) {
        my $address = $address{"$1-$2"};
        delete $device{"$1:$address"};
        delete $address{"$1-$2"};
    }
}

# Run through the map of bus:devnum to device name, use lsusb to extract
# serial number information for each device and print it out.

foreach my $dev (keys %device) {
    my ($manu, $prod, $serial) = ('','','');
    foreach (`lsusb -v -s $dev`) {
        if (/iManufacturer\s+\d+\s+(\w+)/) { $manu = $1; }
        if (/iProduct\s+\d+\s+(\w+)/) { $prod = $1; } 
        if (/iSerial\s+\d+\s+(\w+)/) { $serial = $1; }
    }
    my $num = $serials{$serial} || "?";
    print "/dev/$device{$dev} $num $serial $manu $prod $dev\n";
}
