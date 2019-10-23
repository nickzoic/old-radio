# WHAT IS THIS?

This is the remnants of a project I worked on between 2008-2010 
which was going to form the backbone of my never-completed M.Eng.Sc thesis.

I had access to some very cheap ASK radio modules which were able to
transmit serial data, and also some microcontrollers with onboard
UARTs.  The idea was to make a large number of mesh nodes to
investigate the behaviour of [Virtual Localization](https://nick.zoic.org/art/virtual-localization/)
in real life rather than just simulation.

The UARTs on these devices always use a single start and single stop bit,
and radio modules used had very limited performance especially on long
runs of '0' or '1' bits.  
To work around this limitation this code uses a symbol table to translate
6 data bits into 1 symbol, each symbol being 10 bits long including start 
and stop bits.  Each symbol has a limited number of sequential '0's or '1's.
This provides good DC balance to the receiver, and also provides error 
detection as bit errors generally lead to unbalanced symbols.
The exact parameters of which symbols to use are obviously open to experiment.

Unforunately work and a young family ended up taking priority so
this project got shelved and the degree abandoned.
I recently found this code on an old CD-ROM in a dusty file box,
and thought I might as well throw it up here.

## First Version

* radio/serial/symbols.py : takes a bunch of parameters describing the limits 
  of symbol patterns and generates a bidirectional symbol table as a fragment of C.

* radio/serial/symbols.c : includes the output of the above, provides 
  functions `bytes_to_symbols` and `symbols_to_bytes` to convert.
 
* radio/serial/sendrecv.c : handles preamble and so on

## Second Version

This was an attempt to rewrite the above in a way that would run the same
code either on real hardware or in simulation.  

* radio2/src/symbols.i : generated symbol table 

# Future Work

This work has been superceded by the advent of tiny WiFi-capable Microcontrollers
such as the [Espressif ESP32](https://nick.zoic.org/tag/esp32/) and so is unlikely
to have much relevance any more.

If you've read this far, you might also find [L2IoT : IoT without IP](https://nick.zoic.org/art/l2iot-iot-without-ip/) interesting.
