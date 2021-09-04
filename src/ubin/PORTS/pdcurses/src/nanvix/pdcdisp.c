/* PDCurses */

#include "pdcnanvix.h"
#include "../common/acs437.h"

static bool in_bold = FALSE;

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

    /*
     * ANSI Coordinates start at (1,1) while curses
     * start at (0,0).
     */
    fprintf(stdout, "\x1b[%d;%dH", row + 1, col + 1);
    fflush(stdout);
}

void _set_ansi_color(short f, short b, attr_t attr)
{
    char esc[64], *p;
    short tmp;
    bool bold;

    if (f > 15 || b > 15)
        return;

    f = pdc_curstoansi[f];
    b = pdc_curstoansi[b];

    if (attr & A_REVERSE)
    {
        tmp = f;
        f = b;
        b = tmp;
    }

    bold = !!(attr & A_BOLD);

    /* Start escape sequence. */
    p = esc + sprintf(esc, "\x1b[");

    /* Set foreground. */
    if (f != pdc_oldf)
    {
        if (f < 8)
            p += sprintf(p, "%d", f + 30);
        /*
         * 0-  7:  standard colors (as in ESC [ 30–37 m)
         * 8- 15:  high intensity colors (as in ESC [ 90–97 m)
         */
        else if (f < 16)
            p += sprintf(p, "%d", f + 82);
        pdc_oldf = f;
    }

    /* Set background. */
    if (b != pdc_oldb)
    {
        if (strlen(esc) > 2)
            p += sprintf(p, ";");
        if (b < 8)
            p += sprintf(p, "%d", b + 40);
        else if (b < 16)
            p += sprintf(p, "%d", b + 92);
        pdc_oldb = b;
    }

    /* Enable bold/bright mode. */
    if (bold != in_bold)
    {
        if (strlen(esc) > 2)
            p += sprintf(p, ";");

        if (bold)
            p += sprintf(p, "1");
        else
            p += sprintf(p, "2");

        in_bold = bold;
    }

    /* Close our sequence and write to stdout. */
    if (strlen(esc) > 2)
    {
        sprintf(p, "m");
        write(STDOUT_FILENO, esc, strlen(esc));
    }
}

void _new_packet(attr_t attr, int lineno, int x, int len, const chtype *srcp)
{
    char buffer[256];
    short fore;
    short back;
    int j;

    pair_content(PAIR_NUMBER(attr), &fore, &back);

    for (j = 0; j < len; j++)
    {
        chtype ch = srcp[j];

        if (ch & A_ALTCHARSET && !(ch & 0xff80))
            ch = acs_map[ch & 0x7f];

        buffer[j] = ch & A_CHARTEXT;
    }

    PDC_gotoyx(lineno, x);
    _set_ansi_color(fore, back, attr);
    write(STDOUT_FILENO, buffer, len);
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    attr_t old_attr, attr;
    int i, j;

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    old_attr = *srcp & (A_ATTRIBUTES ^ A_ALTCHARSET);

    for (i = 1, j = 1; j < len; i++, j++)
    {
        attr = srcp[i] & (A_ATTRIBUTES ^ A_ALTCHARSET);

        if (attr != old_attr)
        {
            _new_packet(old_attr, lineno, x, i, srcp);
            old_attr = attr;
            srcp += i;
            x += i;
            i = 0;
        }
    }

    _new_packet(old_attr, lineno, x, i, srcp);
}

void PDC_doupdate(void)
{
}
