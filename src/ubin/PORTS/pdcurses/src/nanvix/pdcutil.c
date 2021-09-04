/* PDCurses */

#include "pdcnanvix.h"

void PDC_beep(void)
{
    PDC_LOG(("PDC_beep() - called\n"));
}

void PDC_napms(int ms)
{
    PDC_LOG(("PDC_napms() - called: ms=%d\n", ms));
    
    usleep(1000 * ms);
}

const char *PDC_sysname(void)
{
    return "NANVIX";
}
