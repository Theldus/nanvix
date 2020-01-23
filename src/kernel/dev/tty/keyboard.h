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

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

	/* ASCII characters. */
	#define ESC         27
	#define BACKSPACE  '\b'
	#define TAB        '\t'
	#define ENTER      '\n'
	#define RETURN     '\r'
	#define NEWLINE   ENTER

	/* Non-ASCII special scan-codes. */
	#define KESC          1
	#define KF1        0x80
	#define KF2   (KF1 + 1)
	#define KF3   (KF2 + 1)
	#define KF4   (KF3 + 1)
	#define KF5   (KF4 + 1)
	#define KF6   (KF5 + 1)
	#define KF7   (KF6 + 1)
	#define KF8   (KF7 + 1)
	#define KF9   (KF8 + 1)
	#define KF10  (KF9 + 1)
	#define KF11 (KF10 + 1)
	#define KF12 (KF11 + 1)

	/* Cursor keys. */
	#define KINS           0x90
	#define KDEL     (KINS + 1)
	#define KHOME    (KDEL + 1)
	#define KEND    (KHOME + 1)
	#define KPGUP    (KEND + 1)
	#define KPGDN   (KPGUP + 1)
	#define KLEFT   (KPGDN + 1)
	#define KUP     (KLEFT + 1)
	#define KDOWN     (KUP + 1)
	#define KRIGHT  (KDOWN + 1)
	#define KCTRL  (KRIGHT + 1)

	/* Meta keys. */
	#define KMETA_ALT   0x0200
	#define KMETA_CTRL  0x0400
	#define KMETA_SHIFT 0x0800
	#define KMETA_CAPS  0x1000
	#define KMETA_NUM   0x2000
	#define KMETA_SCRL  0x4000
	#define KMETA_ANY   (KMETA_ALT | KMETA_CTRL | KMETA_SHIFT)

	/* Other keys. */
	#define KRLEFT_CTRL   0x1D
	#define KRRIGHT_CTRL  0x1D
	#define KRLEFT_ALT    0x38
	#define KRRIGHT_ALT   0x38
	#define KRLEFT_SHIFT  0x2A
	#define KRRIGHT_SHIFT 0x36
	#define KRCAPS_LOCK   0x3A
	#define KRSCROLL_LOCK 0x46
	#define KRNUM_LOCK    0x45
	#define KRDEL         0x53

#endif /* _KEYBOARD_H_ */
