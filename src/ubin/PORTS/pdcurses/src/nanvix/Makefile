# Makefile for PDCurses for Nanvix

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/common/libobjs.mif

osdir = $(PDCURSES_SRCDIR)/nanvix

PDCURSES_NANVIX_H = $(osdir)/pdcnanvix.h

ifeq ($(DEBUG),Y)
	CFLAGS  = -g -Wall -DPDCDEBUG
else
	CFLAGS  = -O2 -Wall
endif

CC          = i386-elf-nanvix-gcc
CFLAGS     += -DNANVIX_PORT
BUILD		= $(CC) $(CFLAGS) -I$(PDCURSES_SRCDIR)
LINK		= $(CC)
LDFLAGS		= $(LIBCURSES) $(SLIBS)
RANLIB		= ranlib
LIBCURSES	= libpdcurses.a
NVX_DEMOS   = xmas_nvx

.PHONY: all libs clean demos

all:	libs

libs:	$(LIBCURSES)

clean:
	-rm -rf *.o trace $(LIBCURSES) $(DEMOS) $(NVX_DEMOS)

demos:	$(DEMOS) $(NVX_DEMOS)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	ar rv $@ $?
	-$(RANLIB) $@

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_NANVIX_H)
$(DEMOS) : $(PDCURSES_CURSES_H) $(LIBCURSES)
tui.o tuidemo.o : $(PDCURSES_CURSES_H)
panel.o ptest: $(PANEL_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(BUILD) -c $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(BUILD) $(SFLAGS) -c $<

firework: $(demodir)/firework.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

ozdemo: $(demodir)/ozdemo.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

ptest: $(demodir)/ptest.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

rain: $(demodir)/rain.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

testcurs: $(demodir)/testcurs.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

tuidemo: tuidemo.o tui.o
	$(LINK) tui.o tuidemo.o -o $@ $(LDFLAGS)

tui.o: $(demodir)/tui.c $(demodir)/tui.h
	$(BUILD) -c $(DEMOFLAGS) $(demodir)/tui.c

tuidemo.o: $(demodir)/tuidemo.c
	$(BUILD) -c $(DEMOFLAGS) $(demodir)/tuidemo.c

worm: $(demodir)/worm.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

xmas: $(demodir)/xmas.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)

xmas_nvx: $(osdir)/demos/xmas_nvx.c
	$(BUILD) $(DEMOFLAGS) -o $@ $< $(LDFLAGS)
