/* PDCurses */

#include "pdcnanvix.h"
#include <ctype.h>
#include <termios.h>

#define ESC '\x1b'

static unsigned char last_key[8];
static int rawmode     = 0;
static int cbreak_mode = 0;

/* enable raw mode, whether true raw or cbreak. */
void _enable_raw(int c_lflag)
{
    struct termios raw;

    raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);

    /*
     * Local modes:
     * echoing off, canonical off, no extended functions,
     * and (optionally) no signal chars (^Z,^C).
     *
     * VMIN and VTIME should be 0, so we can check (and get) the
     * key available whenever a successful can to check_key is
     * made.
     */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | c_lflag);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    /* put terminal in raw mode after flushing */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) < 0)
        return;
}

/* set keyboard binary wrapper. */
void PDC_set_keyboard_binary(bool on)
{
    PDC_LOG(("PDC_set_keyboard_binary() - called\n"));

    /* If disable. */
    if (!on)
    {
        if (rawmode || cbreak_mode)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
            rawmode = 0;
            cbreak_mode = 0;
        }
        return;
    }

    else if (on && rawmode)
        return;

    /* Enable raw mode, ignoring signals. */
    _enable_raw(ISIG);
    rawmode = 1;
    cbreak_mode = 0;
}

/* our extra set cbreak wrapper. */
void PDC_set_cbreak(bool on)
{
    PDC_LOG(("PDC_set_cbreak() - called\n"));

    /* If disable. */
    if (!on)
    {
        if (cbreak_mode || rawmode)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
            cbreak_mode = 0;
            rawmode = 0;
        }
        return;
    }

    else if (on && cbreak_mode)
        return;

    /* Enable raw mode, not-ignoring signals. */
    _enable_raw(0);
    cbreak_mode = 1;
    rawmode = 0;
}

/* check if a key or mouse event is waiting */

bool PDC_check_key(void)
{
    int ret;

    memset(last_key, 0, sizeof(last_key));
    ret = read(STDIN_FILENO, last_key, sizeof(last_key));
    if (!ret)
        return FALSE;

    /*
     * Thankfully, our non-blocking read also _saves_ the
     * last key press, so we can reuse-it inside PDC_get_key().
     *
     * A small note:
     * I purposely save only the last read to a call to read, rather
     * than keeping a queue or something like that. Although it may
     * seem absurd, the PDcurses port to sdl1/ does something similar,
     * keeping only the last event obtained via SDL_PollEvent. A user
     * typing _too_ quickly (something above 150 WPM) might miss a
     * few keys, but I don't expect 'unusual' usage like that.
     *
     * It is also worth noting that if there is a read, I am
     * guaranteed that (in Nanvix) I will always have the complete
     * keystroke characters, as they are inserted (completely) into
     * the TTY buffer even before waking up the process.
     *
     * It is up to PDC_get_key (and possibly auxiliary functions) to
     * read the buffer and interpret which key was pressed.
     */
    return TRUE;
}

/* return the next available key or mouse event */

int PDC_get_key(void)
{
    /* We do not use this. */
    SP->key_modifiers = 0;

    /* Interpret byte sequence. */
    if (last_key[0] != ESC)
    {
        /*
         * Nanvix always returns '\n', regardless in rawmode or
         * not, so we need to convert \n to carriage return.
         */
        if (last_key[0] == '\n')
            return '\r';

        /*
         * Nanvix already returns the 'cooked' character (unless
         * in escape sequence), so we don't need an intermediate
         * table to convert anything.
         */
        return last_key[0];
    }

    /* ESC sequences. */
    if (last_key[1] == 0) return ESC;

    /* ESC [ sequences. */
    if (last_key[1] == '[')
    {
        if (last_key[2] >= '0' && last_key[2] <= '9')
        {
            /* Extended escape. */
            if (last_key[3] == 0) return ESC;

            if (last_key[3] == '~')
            {
                switch (last_key[2])
                {
                    case '2': return KEY_IC;
                    case '3': return KEY_DC;
                    case '5': return KEY_PPAGE;
                    case '6': return KEY_NPAGE;
                }
            }
        }

        /* Arrows, HOME and END keys. */
        else
        {
            switch (last_key[2])
            {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
    }

    /* Maybe ESC 0 sequences. */
    else if (last_key[1] == 'O')
    {
        switch (last_key[2])
        {
            case 'H': return KEY_HOME;
            case 'F': return KEY_END;
        }
    }

    /* Maybe ALT?. */
    else if (isalnum(last_key[1]))
    {
        if (isdigit(last_key[1]))
            return last_key[1] + (ALT_0 - '0');

        else
        {
            int ch = tolower(last_key[1]);
            return ch + (ALT_A - 'a');
        }
    }

    return (-1);
}

/* discard any pending keyboard or mouse input -- this is the core
   routine for flushinp() */

void PDC_flushinp(void)
{
    PDC_LOG(("PDC_flushinp() - called\n"));

    /* Same trick as SDL1 port. */
    while (PDC_check_key());
}

bool PDC_has_mouse(void)
{
    return FALSE;
}

int PDC_mouse_set(void)
{
   return ERR;
}

int PDC_modifiers_set(void)
{
    return OK;
}
