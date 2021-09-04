/* PDCurses */

#include "pdcnanvix.h"

#include <stdlib.h>
#include <termios.h>

short pdc_oldf; /* Old foreground.  */
short pdc_oldb; /* Old boackground. */
struct termios orig_termios;

static short realtocurs[16] =
{
    COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_CYAN, COLOR_RED,
    COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE, COLOR_BLACK + 8,
    COLOR_BLUE + 8, COLOR_GREEN + 8, COLOR_CYAN + 8, COLOR_RED + 8,
    COLOR_MAGENTA + 8, COLOR_YELLOW + 8, COLOR_WHITE + 8
};

static short ansitocurs[16] =
{
    COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE,
    COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE, COLOR_BLACK + 8,
    COLOR_RED + 8, COLOR_GREEN + 8, COLOR_YELLOW + 8, COLOR_BLUE + 8,
    COLOR_MAGENTA + 8, COLOR_CYAN + 8, COLOR_WHITE + 8
};

short pdc_curstoreal[16];
short pdc_curstoansi[16];

/* close the physical screen -- may restore the screen to its state
   before PDC_scr_open(); miscellaneous cleanup */

void PDC_scr_close(void)
{
    /*
     * Get into a 'sane' state:
     * - Restore TTY attributes.
     * - Set colors to default
     * - Clear screen and jump cursor to the top.
     */
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    write(STDOUT_FILENO, "\x1b[0m", 4);
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void PDC_scr_free(void)
{
}

/* open the physical screen -- miscellaneous initialization, may save
   the existing screen for later restoration */

int PDC_scr_open(void)
{
    int i;

    PDC_LOG(("PDC_scr_open() - called\n"));

    for (i = 0; i < 16; i++)
    {
        pdc_curstoreal[realtocurs[i]] = i;
        pdc_curstoansi[ansitocurs[i]] = i;
    }

    SP->orig_attr = FALSE;
    SP->audible = FALSE;
    SP->mono = FALSE;
    SP->termattrs = A_COLOR | A_REVERSE | A_BOLD;
    SP->_preserve = FALSE;

    /* Save our original termios at the very beginning. */
    tcgetattr(STDIN_FILENO, &orig_termios);

    /*
     * Set a know mode by default
     *
     * Some PDCurses demos do not set an initial mode, which can lead
     * to unexpected behavior if the default (cooked) mode is not
     * expected.
     *
     * Therefore, set the initial mode to cbreak() as it is usually
     * the most used.
     */
    PDC_set_cbreak(TRUE);

    return OK;
}

/* the core of resize_term() */

int PDC_resize_screen(int nlines, int ncols)
{
    PDC_LOG(("PDC_resize_screen() - called. Lines: %d Cols: %d\n",
        nlines, ncols));

    return OK;
}

void PDC_reset_prog_mode(void)
{
    PDC_LOG(("PDC_reset_prog_mode() - called.\n"));
}

void PDC_reset_shell_mode(void)
{
    PDC_LOG(("PDC_reset_shell_mode() - called.\n"));
}

void PDC_restore_screen_mode(int i)
{
    ((void)i);
}

void PDC_save_screen_mode(int i)
{
   ((void)i);
}

bool PDC_can_change_color(void)
{
    return FALSE;
}

int PDC_color_content(short color, short *red, short *green, short *blue)
{
    *red = 0;
    *green = 0;
    *blue = 0;

    return OK;
}

int PDC_init_color(short color, short red, short green, short blue)
{
    return OK;
}
