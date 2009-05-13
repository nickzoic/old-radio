#!/usr/bin/perl -w
# $Id: identify.pl,v 1.4 2009-05-13 08:19:39 nick Exp $

# Uses the output of dmesg and lsusb to identify which ttyUSB* is which device.

use strict;

# Map of serial numbers to the numbers I've written on the boards in texta.

my %serials = (
    # Revision2 boards (green)
    A7RUH59A => 4,			# S
    A7RUH71A => 5,
    A7RUH73F => 6,			# L
    A7RUH72S => 8,			# S
   
    # Revision3 boards (red) 
    A8007xwJ => 10,			# L
    A8007xwM => 11,			# L
    A8007xwK => 12,			# S
    A8007xwg => 13,
    A8007xwG => 14,
    A8007xwp => 15,
    A8007xwB => 16,
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

foreach my $dev (sort { $device{$a} cmp $device{$b} } keys %device) {
    my ($manu, $prod, $serial) = ('','','');
    foreach (`lsusb -v -s $dev`) {
        if (/iManufacturer\s+\d+\s+(\w+)/) { $manu = $1; }
        if (/iProduct\s+\d+\s+(\w+)/) { $prod = $1; } 
        if (/iSerial\s+\d+\s+(\w+)/) { $serial = $1; }
    }
    my $num = $serials{$serial} || "?";
    print "/dev/$device{$dev} $num $serial $manu $prod $dev\n";
}
