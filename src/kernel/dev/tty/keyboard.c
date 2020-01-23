/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nanvix/const.h>
#include <nanvix/dev.h>
#include <nanvix/hal.h>
#include <nanvix/klib.h>
#include <signal.h>
#include <stdint.h>
#include <termios.h>
#include "keyboard.h"
#include "tty.h"

/**
 * @brief US International non-shifted key map.
 */
PRIVATE uint8_t ascii_non_shift[] = {
	'\0', ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '-', '=', BACKSPACE, TAB, 'q', 'w', 'e', 'r', 't',
	'y', 'u', 'i', 'o', 'p',  '[', ']', ENTER, 0, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0,
	' ', 0, KF1, KF2, KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10,
	0, 0, KHOME, KUP, KPGUP,'-', KLEFT, '5', KRIGHT, '+', KEND,
	KDOWN, KPGDN, KINS, KDEL, 0, 0, 0, KF11, KF12
};

/* Keymap: US International  shifted scan codes to ASCII. */
PRIVATE uint8_t ascii_shift[] = {
	'\0', ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
	'_', '+', BACKSPACE, TAB, 'Q', 'W',   'E', 'R', 'T', 'Y', 'U',
	'I', 'O', 'P',   '{', '}', ENTER, KCTRL, 'A', 'S', 'D', 'F', 'G',
	'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|','Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0, KF1,   KF2,
	KF3, KF4, KF5, KF6, KF7, KF8, KF9, KF10, 0, 0, KHOME, KUP,
	KPGUP, '-', KLEFT, '5',   KRIGHT, '+', KEND, KDOWN, KPGDN,
	KINS, KDEL, 0, 0, 0, KF11, KF12
};


/**
 * @brief Keyboard flags.
 */
enum flags
{
	ANY   = (1 << 0), /**< Any key pressed.   */
	SHIFT = (1 << 1), /**< Shift key pressed. */
	CTRL  = (1 << 2)  /**< CTRL key pressed.  */
};

/**
 * @brief Keyboard flags.
 */
PRIVATE enum flags mode = 0;

/*
 * Parses key hit and decodes it to a scan code.
 */
PRIVATE uint8_t parse_key_hit(void)
{
	uint8_t scancode;
	uint8_t port_value;

    scancode = inputb(0x60);

    port_value = inputb(0x61);
    outputb(0x61, port_value | 0x80);
    outputb(0x61, port_value & ~0x80);

	/* A key was released. */
    if(scancode & 0x80)
    {
        scancode &= 0x7F;

        /* Parse scan code. */
        switch (scancode)
        {
			/* Shift. */
			case KRLEFT_SHIFT:
			case KRRIGHT_SHIFT:
				mode &= ~SHIFT;
				break;

			/* CTRL. */
			case KRLEFT_CTRL:
				mode &= ~CTRL;
				break;

			/* Any other. */
			default:
				mode &= ~ANY;
				break;
		}
    }

   	/* A key was pressed. */
    else
    {
        /* Parse scan code. */
        switch (scancode)
        {
			/* Shift. */
			case KRLEFT_SHIFT:
			case KRRIGHT_SHIFT:
				mode |= SHIFT;
				break;

			/* CTRL. */
			case KRLEFT_CTRL:
				mode |= CTRL;
				break;

			/* Any other. */
			default:
				mode |= ANY;
				break;
		}
    }

    return (scancode);
}

/*
 * Gets the ASCII code for the pressed key.
 */
PRIVATE uint8_t get_ascii(void)
{
    uint8_t scancode;
    uint8_t code;

    scancode = parse_key_hit();

	/* Printable character. */
    if (mode & ANY)
    {
		code = (mode & SHIFT) ? ascii_shift[scancode]:ascii_non_shift[scancode];

		/* CTRL pressed. */
		if (mode & CTRL)
			return ((code < 96) ? code - 64 : code - 96);

		return (code);
	}

    return (0);
}

/**
 * @brief Handles a keyboard interrupt.
 *
 * @details Handles a keyboard interrupt, parsing scan codes received from the
 *          keyboard controller to ascii codes.
 */
PUBLIC void do_keyboard_hit(void)
{
	uint8_t ascii_code;

	ascii_code = get_ascii();

	/* Ignore. */
	if (ascii_code == 0)
		return;

	/* Parse ASCII code. */
	switch(ascii_code)
	{
		/* Fall through. */
		case KINS:
        case KDEL:
        case KHOME:
        case KEND:
        	/*  TODO: implement. */
            break;

		/* Fall through. */
		case KUP:
		case KDOWN:
		case KRIGHT:
		case KLEFT:
		case KPGUP:
        case KPGDN:
			tty_int(ascii_code);
        	break;

		default:
		    tty_int(ascii_code);
        	break;
	}
}


/*
 * Initializes the keyboard driver.
 */
PUBLIC void keyboard_init(void)
{
	set_hwint(INT_KEYBOARD, &do_keyboard_hit);

    while (inputb(0x64) & 1)
		inputb(0x60);
}
