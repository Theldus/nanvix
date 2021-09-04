/* PDCurses */

#ifndef PDCNANVIX_H
#define PDCNANVIX_H

#include <curspriv.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern short pdc_curstoreal[16];
extern short pdc_curstoansi[16];
extern short pdc_oldf;
extern short pdc_oldb;
extern struct termios orig_termios;

#endif
