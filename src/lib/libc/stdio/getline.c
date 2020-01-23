/*
 * Copyright(C) 2020-2020 Davidson Francis <davidsondfgl@gmail.com>
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

/* Copyright 2002, Red Hat Inc. - all rights reserved */

#include <_ansi.h>
#include <stdio.h>

/**
 * @brief Read an entire line from stream, storing the address of the
 * buffer containing the text into *lineptr. The buffer is null-termi‐
 * nated and includes the newline character, if one was found.
 *
 * @param lptr Line pointer.
 * @param n Line size.
 * @param fp Stream.
 *
 * @return On success, returns the number of characters read, including
 * the delimiter character, but not including the terminating null byte
 * ('\0'). This value can be used to handle embedded null bytes in the
 * line read. Otherwise, returns -1 on failure (including end-of-
 * file condition). In the event of an error, errno is set to indicate
 * the cause.
 */
ssize_t getline (char **lptr, size_t *n, FILE *fp)
{
	return getdelim (lptr, n, '\n', fp);
}
