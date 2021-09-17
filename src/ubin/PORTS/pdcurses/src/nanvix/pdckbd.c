/* PDCurses */

#include "pdcnanvix.h"
#include <ctype.h>
#include <termios.h>

#define ESC '\x1b'

static unsigned char last_keystroke[8]; 
static int keys_amnt   = 0;
static int key_pos     = 0;
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
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
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
    /* If there are pending keys to be read. */
    if (key_pos < keys_amnt)
        return TRUE;

    memset(last_keystroke, 0, sizeof(last_keystroke));
    keys_amnt = read(STDIN_FILENO, last_keystroke, sizeof(last_keystroke));
    key_pos   = 0;

    if (keys_amnt <= 0)
    {
        keys_amnt = 0;
        return FALSE;
    }

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

int PDC_get_key(WINDOW *win)
{
    int ret_key;

    if (key_pos >= keys_amnt)
        return (-1);

    /* We do not use this. */
    SP->key_modifiers = 0;

    /*
     * Check if we have to return the raw read key, or the cooked.
     *    _use_keypad == true, cooked
     *    _use_keypad == false, escape sequence, 1 char per time
     *
     * _use_keypad is set by invoking keypad(win, true)
     *
     * By default _use_keypad is false!.
     */
    if (!win->_use_keypad)
    {
        /*
         * Convert to \n or \r if needed.
         */
        if (last_keystroke[key_pos] == '\r' || last_keystroke[key_pos] == '\n')
        {
            if (SP->autocr)
                ret_key = '\n';
            else
                ret_key = '\r';
        }
        else
            ret_key = last_keystroke[key_pos];

        key_pos++;
        return (ret_key);
    }

    /*
     * If (keypad == true) mode but ALT key, return like cooked.
     *
     * Some observations need to be done here:
     * Although PDCurses provides keys for ALT+key, this is a
     * unique feature of PDCurses, and therefore we should not
     * use it unless explicitly required (via
     * -DUSE_PDCURSES_SPECIFIC_KEYS).
     *
     * Therefore, when receiving an ALT+key, we will emit an
     * escape sequence regardless of which mode we are in.
     */
    if (keys_amnt == 2 && last_keystroke[0] == ESC &&
        isgraph(last_keystroke[1]))
    {
        ret_key = last_keystroke[key_pos];
        key_pos++;
        return (ret_key);
    }

    /* === Anything else === */
    key_pos = keys_amnt;

    /* Interpret byte sequence. */
    if (last_keystroke[0] != ESC)
    {
        /*
         * Convert to \n or \r if needed.
         */
        if (last_keystroke[0] == '\r' || last_keystroke[0] == '\n')
        {
            if (SP->autocr)
                ret_key = '\n';
            else
                ret_key = '\r';
        }
        else
            ret_key = last_keystroke[0];

        /*
         * Nanvix already returns the 'cooked' character (unless
         * in escape sequence), so we don't need an intermediate
         * table to convert anything.
         */
        return ret_key;
    }

    /* ESC sequences. */
    if (last_keystroke[1] == 0) return ESC;

    /* ESC [ sequences. */
    if (last_keystroke[1] == '[')
    {
        if (last_keystroke[2] >= '0' && last_keystroke[2] <= '9')
        {
            /* Extended escape. */
            if (last_keystroke[3] == 0) return ESC;

            if (last_keystroke[3] == '~')
            {
                switch (last_keystroke[2])
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
            switch (last_keystroke[2])
            {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
            }
        }
    }

    /* Maybe ESC 0 sequences. */
    else if (last_keystroke[1] == 'O')
    {
        switch (last_keystroke[2])
        {
            case 'H': return KEY_HOME;
            case 'F': return KEY_END;
        }
    }

    /* Only PDCurses have dedicated keys for ALT =/. */
#ifdef USE_PDCURSES_SPECIFIC_KEYS
    /* Maybe ALT?. */
    else if (isalnum(last_keystroke[1]))
    {
        if (isdigit(last_keystroke[1]))
            return last_keystroke[1] + (ALT_0 - '0');

        else
        {
            int ch = tolower(last_keystroke[1]);
            return ch + (ALT_A - 'a');
        }
    }
#endif

    return (-1);
}

/* discard any pending keyboard or mouse input -- this is the core
   routine for flushinp() */

void PDC_flushinp(void)
{
    PDC_LOG(("PDC_flushinp() - called\n"));

    /* Same trick as SDL1 port. */
    do
    {
        keys_amnt = 0;
        key_pos   = 0;
    } while (PDC_check_key());
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
