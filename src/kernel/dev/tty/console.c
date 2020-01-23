/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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
#include <dev/8250.h>
#include <sys/types.h>
#include <stdint.h>
#include "tty.h"

/* Forward definitions. */
static void console_set_color(uint16_t color);
static void cursor_move(void);
static void cursor_disable(void);
static void cursor_enable(void);
static void ansi_reset_buffer(void);

/* Video specifications (Text mode). */
#define VIDEO_ADDR  0xb8000 /* Video memory address. */
#define VIDEO_WIDTH      80 /* Video width.          */
#define VIDEO_HIGH       25 /* Video high.           */

/* Video registers. */
#define VIDEO_CRTL_REG 0x3d4 /* Video control register. */
#define VIDEO_DATA_REG 0x3d5 /* Video data register.    */

/* Video control offset registers. */
#define VIDEO_HTOT 0x00 /* Horizontal total.               */
#define VIDEO_HDEE 0x01 /* Horizontal display enabled end. */
#define VIDEO_SHB  0x02 /* Start horizontal blanking.      */
#define VIDEO_EHB  0x03 /* End Horizontal blanking.        */
#define VIDEO_SHRP 0x04 /* Start horizontal retrace pulse. */
#define VIDEO_EHRP 0x05 /* End horizontal retrace pulse.   */
#define VIDEO_VTR  0x06 /* Vertical total.                 */
#define VIDEO_OVRF 0x07 /* Overflow.                       */
#define VIDEO_PRS  0x08 /* Preset row scan.                */
#define VIDEO_MSL  0x09 /* Maximum scan line.              */
#define VIDEO_CS   0x0a /* Cursor start.                   */
#define VIDEO_CE   0x0b /* Cursor end.                     */
#define VIDEO_SAH  0x0c /* Start address high.             */
#define VIDEO_SAL  0x0d /* Start address low.              */
#define VIDEO_CLH  0x0e /* Cursor location high.           */
#define VIDEO_CLL  0x0f /* Cursor location low.            */
#define VIDEO_RSR  0x10 /* Vertical retrace start.         */
#define VIDEO_RSE  0x11 /* Vertical retrace end.           */
#define VIDEO_VDEE 0x12 /* Vertical display-enable end.    */
#define VIDEO_OFF  0x13 /* Offset.                         */
#define VIDEO_ULOC 0x14 /* Underline location.             */
#define VIDEO_SVB  0x15 /* Start vertical blanking.        */
#define VIDEO_EVB  0x16 /* End vertical blanking.          */
#define VIDEO_CMC  0x17 /* CRT mode control.               */
#define VIDEO_LCMP 0x18 /* Line compare.                   */

/*
 * ANSI Escape Sequences, or better saying:
 * functions that use for Control Sequence Introducer.
 *
 * Note: This list is not exaustive and Nanvix do not supports all the
 * commands below.
 */
#define ANSI_ESCAPE  '\x1b' /* ANSI Escape.                                 */
#define CSI '['      /* Control Sequence Introducer.                        */
#define CSI_CUU 'A'  /* Cursor Up (default = 1) (CUU).                      */
#define CSI_CUD 'B'  /* Cursor Down (default = 1) (CUD).                    */
#define CSI_CUF 'C'  /* Cursor Forward (default = 1) (CUF).                 */
#define CSI_CUB 'D'  /* Cursor Backward (default = 1) (CUB).                */
#define CSI_CNL 'E'  /* Cursor Next Line (default = 1) (CNL).               */
#define CSI_CPL 'F'  /* Cursor Preceding Line (default = 1) (CPL).          */
#define CSI_CHA 'G'  /* Cursor Character Absolute (default = [r,1]) (CHA).  */
#define CSI_CUP 'H'  /* Cursor Position [r;c] (default = [1,1]) (CUP).      */
#define CSI_ED  'J'  /* Erase in Display (ED).
                        P s = 0 -> Erase Below (default)
                        P s = 1 -> Erase Above
                        P s = 2 -> Erase All
                        P s = 3 -> Erase Saved Lines (xterm).               */

#define CSI_EL  'K'  /* Erase in Line (EL).
                        P s = 0 -> Erase to Right (default)
                        P s = 1 -> Erase to Left
                        P s = 2 -> Erase All.                               */

#define CSI_SU  'S'  /* Scroll up P s lines (default = 1) (SU).             */
#define CSI_SD  'T'  /* Scroll down P s lines (default = 1) (SD).           */
#define CSI_SGR 'm'  /* Select Graphics Rendition                           */

#define CSI_DSR 'n'  /* Device Status Report (DSR)
                        P s = 5 -> Status Report CSI 0 n (‘‘OK’’)
                        P s = 6 -> Report Cursor Position (CPR) [row;column] as
                        CSI r ; c R                                         */
#define CSI_DECRST 'l' /* DECTCEM - DEC Private Mode Reset.                 */
#define CSI_DECSET 'h' /* DECTCEM - DEC Private Mode Set.                   */

/*
 * ANSI States.
 */
#define STATE_NONE     0 /* Default state, do not process escape sequences. */
#define STATE_ESCAPE   1 /* ANSI Escape found, next character must be '['.  */
#define STATE_CSI      2 /* Control Sequence Introducer Found.              */
#define STATE_CSI_DEC  4 /* CSI Found and DEC enabled too.                  */

/* Color item. */
struct color_item
{ uint16_t ansi_color; uint16_t vga_color; };

/*
 * Color table
 *
 * Table that maps an ANSI Color to 80x25 console mode color
 */
PRIVATE struct color_item ctable [] =
{
	/* Foreground colors. */
	{30, BLACK},
	{31, RED},
	{32, GREEN},
	{33, BROWN},
	{34, BLUE},
	{35, MAGENTA},
	{36, CYAN},
	{37, WHITE},

	/* Background colors. */
	{40, BLACK},
	{41, RED},
	{42, GREEN},
	{43, BROWN},
	{44, BLUE},
	{45, MAGENTA},
	{46, CYAN},
	{47, WHITE}
};

/* Color table indexes. */
#define FG_COLOR_START 0  /* Foreground color start, inclusive. */
#define FG_COLOR_END   8  /* Foreground color end, exclusive.   */
#define BG_COLOR_START 8  /* Background color start, inclusive. */
#define BG_COLOR_END   16 /* Background color start, inclusive. */

/*
 * Console State
 *
 * All the ANSI Escape Sequences will be dealt as a state machine,
 * so we have to keep the state somewhere.
 */
PRIVATE struct console_state
{
	uint8_t fg_color; /* Foreground color. */
	uint8_t bg_color; /* Background color. */
	uint16_t color;   /* Fore+Back color.  */
	uint8_t state;    /* Current state.    */
} cstate = {
	.fg_color = LIGHT_GREY,
	.bg_color = BLACK,
	.color = 0,
	.state = STATE_NONE
};

/* ANSI Buffer.
 *
 * Since each escape sequence parse is dealt char-by-char,
 * we have to keep a little auxiliar memory to hold infos
 * between calls.
 */

/* Max parameters. */
#define MAX_NPARAM 4

/* Buffer. */
PRIVATE struct ansi_buff
{
	uint16_t nparam[MAX_NPARAM]; /* Numerical parameters.         */
	uint16_t nparam_count;       /* Numerical parameters counter. */
} ansi_buff = {0};

/*
 * Console cursor.
 */
PRIVATE struct
{
	int x; /* Horizontal axis position. */
	int y; /* Vertical axis position.   */
} cursor;

/* Video memory.*/
PRIVATE uint16_t *video = (uint16_t*)VIDEO_ADDR;

/*============================================================================*
 *							   ANSI Family Functions                          *
 *============================================================================*/

/**
 * @brief Handles 'Cursor UP' Event.
 */
PRIVATE void ansi_cuu(void)
{
	int times; /* Numer of times to move. */

	/* Number of times. */
	if (ansi_buff.nparam[0] != 0)
		times = ansi_buff.nparam[0];
	else
		times = 1;

	/* Tries to move upward. */
	if (cursor.y - times >= 0)
		cursor.y -= times;
	else
		cursor.y = 0;

	cursor_move();
	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/**
 * @brief Handles 'Cursor DOWN' Event.
 */
PRIVATE void ansi_cud(void)
{
	int times; /* Numer of times to move. */

	/* Number of times. */
	if (ansi_buff.nparam[0] != 0)
		times = ansi_buff.nparam[0];
	else
		times = 1;

	/* Tries to move downward. */
	if (cursor.y + times >= VIDEO_HIGH-1)
		cursor.y += times;
	else
		cursor.y = VIDEO_HIGH-1;

	cursor_move();
	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/**
 * @brief Handles 'Cursor Forward' Event.
 */
PRIVATE void ansi_cuf(void)
{
	int times; /* Numer of times to move. */

	/* Number of times. */
	if (ansi_buff.nparam[0] != 0)
		times = ansi_buff.nparam[0];
	else
		times = 1;

	/* Tries to move forward. */
	if (cursor.x + times >= VIDEO_WIDTH-1)
		cursor.x += times;
	else
		cursor.x = VIDEO_WIDTH-1;

	cursor_move();
	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/**
 * @brief Handles 'Cursor Backward' Event.
 */
PRIVATE void ansi_cub(void)
{
	int times; /* Numer of times to move. */

	/* Number of times. */
	if (ansi_buff.nparam[0] != 0)
		times = ansi_buff.nparam[0];
	else
		times = 1;

	/* Tries to move forward. */
	if (cursor.x - times >= 0)
		cursor.x -= times;
	else
		cursor.x = 0;

	cursor_move();
	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/**
 * @brief Handles Cursor Position Event.
 */
PRIVATE void ansi_cup(void)
{
	if (ansi_buff.nparam_count > 0)
	{
		if (ansi_buff.nparam[0] <= 1)
			cursor.y = 0;
		else
			cursor.y = KMIN(ansi_buff.nparam[0] - 1, VIDEO_HIGH-1);

		if (ansi_buff.nparam[1] <= 1)
			cursor.x = 0;
		else
			cursor.x = KMIN(ansi_buff.nparam[1] - 1, VIDEO_WIDTH-1);
	}
	else
		cursor.x = cursor.y = 0;

	cstate.state = STATE_NONE;
	ansi_reset_buffer();
	cursor_move();
}

/**
 * @brief Handles 'Erase In-Display' Event.
 */
PRIVATE inline void ansi_ed(void)
{
	console_clear(ansi_buff.nparam[0]);
	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/**
 * @brief Handles 'Erase In-Line' Event.
 */
PRIVATE void ansi_el(void)
{
	uint16_t *pstart;  /* Start pointer.        */
	uint16_t *pend;    /* End pointer.          */
	int xstart, xend;  /* Start and end x-axis. */
	int op;            /* Operation.            */

	op = ansi_buff.nparam[0];

	/* To right. */
	if (op == 0)
	{
		xstart = cursor.x;
		xend = VIDEO_WIDTH;
	}

	/* To left. */
	else if (op == 1)
	{
		xstart = 0;
		xend = cursor.x;
	}

	/* All. */
	else
	{
		xstart = 0;
		xend = VIDEO_WIDTH;
	}

	pstart = &video[cursor.y*VIDEO_WIDTH+xstart];
	pend   = &video[cursor.y*VIDEO_WIDTH+xend];

	while (pstart < pend)
	{
		*pstart = (cstate.bg_color << 8) | (' ');
		pstart++;
	}

	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/**
 * @brief Handles 'Select Graphics Rendition' Event.
 */
PRIVATE void ansi_sgr(void)
{
	/* First parameter will always exist. */
	console_set_color(ansi_buff.nparam[0]);

	/* Check for 2nd and 3rd. */
	if (ansi_buff.nparam_count > 0)
		console_set_color(ansi_buff.nparam[1]);

	if (ansi_buff.nparam_count > 1)
		console_set_color(ansi_buff.nparam[2]);

	cstate.state = STATE_NONE;
	ansi_reset_buffer();
}

/*============================================================================*
 *							   ANSI Helpers Functions                         *
 *============================================================================*/

/*
 * @brief Reset the current mode and colors to default
 */
PRIVATE inline void ansi_reset_colors(void)
{
	cstate.fg_color = LIGHT_GREY;
	cstate.bg_color = BLACK;
	cstate.color = (cstate.bg_color << 12) | (cstate.fg_color << 8);
}

/*
 * @brief Reset the parameters buffer.
 */
PRIVATE inline void ansi_reset_buffer(void)
{
	ansi_buff.nparam_count = 0;
	ansi_buff.nparam[0] = 0;
	ansi_buff.nparam[1] = 0;
	ansi_buff.nparam[2] = 0;
	ansi_buff.nparam[3] = 0;
}

/*
 * @brief Reset all the state to the default.
 */
PRIVATE void ansi_reset_state(void)
{
	/* Reset state. */
	cstate.state = STATE_NONE;

	/* Colors. */
	ansi_reset_colors();

	/* Buffer. */
	ansi_reset_buffer();
}

/*
 * @brief Parse the current character and set the current
 * state machine accordingly. The main escape sequence
 * parser is here.
 *
 * @param ch Character to be analyzed.
 */
PRIVATE void ansi_parse(uint8_t ch)
{
	switch (cstate.state)
	{
		/* Our default case. */
		case STATE_NONE:
		{
			if (ch == ANSI_ESCAPE)
				cstate.state = STATE_ESCAPE;
			else
				console_put(ch, WHITE);

			break;
		}

		/* Escape sequence found. */
		case STATE_ESCAPE:
		{
			if (ch == CSI)
				cstate.state = STATE_CSI;

			/* Invalid state, lets reset everything. */
			else
			{
				ansi_reset_state();
				console_put(ANSI_ESCAPE, WHITE);
				console_put(ch, WHITE);
			}
			break;
		}

		/* Control Sequence Introducer found, our hard work is here. */
		case STATE_CSI:
		{
			/* If one of our supported states. */
			switch (ch)
			{
				/* Cursor UPward. */
				case CSI_CUU:
				{
					ansi_cuu();
					break;
				}

				/* Cursor DOWNward. */
				case CSI_CUD:
				{
					ansi_cud();
					break;
				}

				/* Cursor Forward. */
				case CSI_CUF:
				{
					ansi_cuf();
					break;
				}

				/* Cursor Backward. */
				case CSI_CUB:
				{
					ansi_cub();
					break;
				}

				/* Cursor Position. */
				case CSI_CUP:
				{
					ansi_cup();
					break;
				}

				/* Erase in Display. */
				case CSI_ED:
				{
					ansi_ed();
					break;
				}

				/* Erase in Line. */
				case CSI_EL:
				{
					ansi_el();
					break;
				}

				/* Select Graphics Rendition. */
				case CSI_SGR:
				{
					ansi_sgr();
					break;
				}

				/* Number separator. */
				case ';':
					ansi_buff.nparam_count++;
					break;

				/* '?' option, DEC specific commands. */
				case '?':
					cstate.state = STATE_CSI_DEC;
					break;

				default:
				{
					/* If not number, we do not know how to deal with, lets
					 * reset some things. */
					if (ch < '0' || ch > '9')
					{
						ansi_reset_state();
						return;
					}

					/* Lets save this number if there is space enough. */
					if (ansi_buff.nparam_count >= MAX_NPARAM)
					{
						ansi_reset_state();
						return;
					}

					ansi_buff.nparam[ansi_buff.nparam_count] =
						(ansi_buff.nparam[ansi_buff.nparam_count] * 10) +
						(ch - '0');

					break;
				}
			}

			break;
		}

		/*
		 * CSI Enabled _and_ DEC command.
		 *
		 * This is very redundant, but DEC commands screwed
		 * the logic here, so I have to repeat a couple of
		 * things.
		 */
		case STATE_CSI_DEC:
		{
			switch (ch)
			{
				/* DEC Private Mode Reset. */
				case CSI_DECRST:
				{
					/* Cursor hide. */
					if (ansi_buff.nparam[0] == 25)
						cursor_disable();

					cstate.state = STATE_NONE;
					ansi_reset_buffer();
					break;
				}

				/* DEC Private Mode Set. */
				case CSI_DECSET:
				{
					/* Cursor show. */
					if (ansi_buff.nparam[0] == 25)
						cursor_enable();

					cstate.state = STATE_NONE;
					ansi_reset_buffer();
					break;
				}

				/* Number separator. */
				case ';':
					ansi_buff.nparam_count++;
					break;

				default:
				{
					/* If not number, we do not know how to deal with, lets
					 * reset some things. */
					if (ch < '0' || ch > '9')
					{
						ansi_reset_state();
						return;
					}

					/* Lets save this number if there is space enough. */
					if (ansi_buff.nparam_count >= MAX_NPARAM)
					{
						ansi_reset_state();
						return;
					}

					ansi_buff.nparam[ansi_buff.nparam_count] =
						(ansi_buff.nparam[ansi_buff.nparam_count] * 10) +
						(ch - '0');

					break;
				}
			}
		}
	}
}

/*============================================================================*
 *							     Cursor Functions                             *
 *============================================================================*/

/**
 * Moves the hardware console cursor.
 */
PRIVATE void cursor_move(void)
{
	word_t cursor_location = cursor.y*VIDEO_WIDTH + cursor.x;

	outputb(VIDEO_CRTL_REG, VIDEO_CLH);
	outputb(VIDEO_DATA_REG, (byte_t) ((cursor_location >> 8) & 0xFF));
	outputb(VIDEO_CRTL_REG, VIDEO_CLL);
	outputb(VIDEO_DATA_REG, (byte_t) (cursor_location & 0xFF));
}

/**
 * @brief Disable cursor.
 */
PRIVATE void cursor_disable(void)
{
	outputb(VIDEO_CRTL_REG, VIDEO_CS);
	outputb(VIDEO_DATA_REG, 0x20);
}

/**
 * @brief Enable cursor.
 */
PRIVATE void cursor_enable()
{
	outputb(VIDEO_CRTL_REG, VIDEO_CS);
	outputb(VIDEO_DATA_REG, (inputb(VIDEO_DATA_REG) & 0xC0) | 0);
	outputb(VIDEO_CRTL_REG, VIDEO_CE);
	outputb(VIDEO_DATA_REG, (inputb(VIDEO_DATA_REG) & 0xE0) | 15);
}

/*============================================================================*
 *							     Console Functions                            *
 *============================================================================*/

/*
 * @brief Set background, foreground and mode for a specified
 * @p color.
 *
 * @param color Background, foregroudn ou mode to be set.
 */
PRIVATE void console_set_color(uint16_t color)
{
	int i;               /* Loop index. */
	uint16_t tmp_color;  /* Temp color. */

	/* If foreground. */
	if (color >= 30 && color <= 39)
	{
		/* Default foreground?. */
		if (color == 39)
			cstate.fg_color = LIGHT_GREY;
		else
		{
			/* Get console color equivalent. */
			for (i = FG_COLOR_START; i < FG_COLOR_END; i++)
				if (ctable[i].ansi_color == color)
					break;

			cstate.fg_color = ctable[i].vga_color;
		}

		cstate.color &= 0xF8FF;
		cstate.color |= (cstate.fg_color << 8);
	}

	/* If background. */
	else if (color >= 40 && color <= 47)
	{
		/* Get console color equivalent. */
		for (i = BG_COLOR_START; i < BG_COLOR_END; i++)
			if (ctable[i].ansi_color == color)
				break;

		cstate.bg_color = ctable[i].vga_color;
		cstate.color &= 0x8FFF;
		cstate.color |= (cstate.bg_color << 12);
	}

	/* If a supported mode. */
	else if (color == 0 || color == 1 || color == 7)
	{
		/* Reset everything. */
		if (color == 0)
			ansi_reset_colors();

		/* Bold/Bright */
		else if (color == 1)
			cstate.color |= COLOR_BOLD;

		/* Reversed. */
		else
		{
			tmp_color = cstate.bg_color;
			cstate.bg_color = cstate.fg_color;
			cstate.fg_color = tmp_color;
			cstate.color = (cstate.bg_color << 12) | (cstate.fg_color << 8);
		}
	}

	/* Anything else, do nothing. */
}

/*
 * Scrolls down the console by one row.
 */
PRIVATE void console_scrolldown(void)
{
	uint16_t *p;

	/* Pull lines up. */
	for (p = video; p < (video + (VIDEO_HIGH-1)*(VIDEO_WIDTH)); p++)
		*p = *(p + VIDEO_WIDTH);

	/* Blank last line. */
	for (; p < video + VIDEO_HIGH*VIDEO_WIDTH; p++)
		*p = (BLACK << 8) | (' ');

	/* Set cursor position. */
	cursor.x = 0; cursor.y = VIDEO_HIGH - 1;
}

/*
 * Outputs a colored ASCII character on the console device.
 */
PUBLIC void console_put(uint8_t ch, uint8_t color)
{
	((void)color);
	((void)console_scrolldown);

#ifdef VGA_ENABLE
	/* Parse character. */
	switch (ch)
	{
		/* New line. */
		case '\n':
			cursor.y++;
			cursor.x = 0;
			break;

		/* Carriage return. */
		case '\r':
			cursor.x = 0;
			break;

		/* Tabulation. */
		case '\t':
			/* FIXME. */
			cursor.x += 4 - (cursor.x & 3);
			break;

		/* Backspace. */
		case '\b':
			if (cursor.x > 0)
				cursor.x--;
			else if (cursor.y > 0)
			{
				cursor.x = VIDEO_WIDTH - 1;
				cursor.y--;
			}
			video[cursor.y*VIDEO_WIDTH +cursor.x] = cstate.color | (' ');
			break;

		/* Any other. */
		default:
			video[cursor.y*VIDEO_WIDTH +cursor.x] = cstate.color | (ch);
			cursor.x++;
			break;
	}

	/* Set cursor position. */
	if (cursor.x >= VIDEO_WIDTH)
	{
		cursor.x = 0;
		cursor.y++;
	}
	if (cursor.y >= VIDEO_HIGH)
		console_scrolldown();
	cursor_move();
#endif

#ifdef SERIAL_ENABLE
	/* Write into serial port. */
	uart8250_write(ch);
#endif
}

/*
 * Clears the console accordingly with the mode @p mode.
 *
 * @mode Clear mode: 0: above cursor, 1: below cursor, 2: all.
 */
PUBLIC void console_clear(int mode)
{
	uint16_t *pstart;
	uint16_t *pend;

	/* Lines below cursor. */
	if (mode == 0)
	{
		pstart = &video[(cursor.y+1)*VIDEO_WIDTH+0];
		pend   = &video[VIDEO_HIGH*VIDEO_WIDTH+VIDEO_WIDTH];
	}

	/* Lines above cursor. */
	else if (mode == 1)
	{
		pstart = &video[0];
		pend   = &video[(cursor.y-1)*VIDEO_WIDTH+VIDEO_WIDTH];
	}

	/* Blank all lines. */
	else if (mode == 2)
	{
		pstart = &video[0];
		pend   = &video[VIDEO_HIGH*VIDEO_WIDTH+VIDEO_WIDTH];
	}

	/*
	 * Especial mode: Blank all lines but also resetting the cursor.
	 * This is not ANSI and only used inside the Nanvix internals.
	 */
	else if (mode == 3)
	{
		pstart = &video[0];
		pend   = &video[VIDEO_HIGH*VIDEO_WIDTH+VIDEO_WIDTH];

		/* Set console cursor position. */
		cursor.x = cursor.y = 0;
		cursor_move();
	}

	while (pstart < pend)
	{
		*pstart = (cstate.bg_color << 8) | (' ');
		pstart++;
	}
}

/*
 * Flushes a buffer on the console device.
 */
PUBLIC void console_write(struct kbuffer *buffer)
{
	uint8_t ch;

	/* Outputs all characters. */
	while (!KBUFFER_EMPTY((*buffer)))
	{
		KBUFFER_GET((*buffer), ch);

		ansi_parse(ch);
	}
}

/*
 * Initializes the console driver.
 */
PUBLIC void console_init(void)
{
	/* Set cursor shape. */
	outputb(VIDEO_CRTL_REG, VIDEO_CS);
	outputb(VIDEO_DATA_REG, 0x00);
	outputb(VIDEO_CRTL_REG, VIDEO_CE);
	outputb(VIDEO_DATA_REG, 0x1f);

	/* Clear the console. */
	console_clear(3);

	/* Set console state. */
	ansi_reset_state();
}
