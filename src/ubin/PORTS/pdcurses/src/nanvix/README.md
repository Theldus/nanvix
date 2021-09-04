PDCurses for Nanvix
===================
This directory contains PDCurses source code files specific to Nanvix.


Building
--------
Just invoke `make`.

Demo files can be built by invoking `make demos`.

It's worth noting that this port also adds (unintentionally) PDcurses support to
the Linux terminal, as all control is done via ANSI Escape Sequences (keyboard
and screen). To experiment directly on a Linux terminal, set `CC` to your
local/host compiler rather than the Nanvix's compiler.


Distribution Status
-------------------
The files in this directory are released to the public domain.


Acknowledgements
----------------
All PDCurses developers and ports contributors as they served as the
basis/inspiration for the Nanvix port.
