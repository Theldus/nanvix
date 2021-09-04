/* PDCurses */

#include "pdcnanvix.h"

int PDC_curs_set(int visibility)
{
    int ret_vis;

    PDC_LOG(("PDC_curs_set() - called: visibility=%d\n", visibility));

    ret_vis = SP->visibility;

    SP->visibility = visibility;

    /* Hide. */
    if (visibility == 0)
        write(STDOUT_FILENO, "\x1b[?25l", 6);

    /* Anything else, show. */
    else
        write(STDOUT_FILENO, "\x1b[?25h", 6);

    PDC_gotoyx(SP->cursrow, SP->curscol);

    return ret_vis;
}

void PDC_set_title(const char *title)
{
    PDC_LOG(("PDC_set_title() - called: <%s>\n", title));
}

int PDC_set_blink(bool blinkon)
{
    COLORS = 16;
    return OK;
}

int PDC_set_bold(bool boldon)
{
    return boldon ? ERR : OK;
}
