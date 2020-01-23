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
#include <stdlib.h>
#include <errno.h>
#include "local.h"

#define MIN_LINE_SIZE 4
#define DEFAULT_LINE_SIZE 128

/**
 * @brief Behaves like getline(), except that a line delimiter other
 * than newline can be specified as the delimiter argument.
 *
 * @param bufptr Buffer pointer.
 * @param n Line size.
 * @param delim Delimiter.
 * @param fp Stream.
 *
 * @return On success, returns the number of characters read, including
 * the delimiter character, but not including the terminating null byte
 * ('\0'). This value can be used to handle embedded null bytes in the
 * line read. Otherwise, returns -1 on failure (including end-of-
 * file condition). In the event of an error, errno is set to indicate
 * the cause.
 */
ssize_t getdelim (char **bufptr, size_t *n, int delim, FILE *fp)
{
  char *buf;
  char *ptr;
  size_t newsize, numbytes;
  int pos;
  int ch;
  int cont;

  if (fp == NULL || bufptr == NULL || n == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  buf = *bufptr;
  if (buf == NULL || *n < MIN_LINE_SIZE) 
    {
      buf = (char *)realloc (*bufptr, DEFAULT_LINE_SIZE);
      if (buf == NULL)
        {
	  return -1;
        }
      *bufptr = buf;
      *n = DEFAULT_LINE_SIZE;
    }

  CHECK_INIT (_REENT, fp);

  _newlib_flockfile_start (fp);

  numbytes = *n;
  ptr = buf;

  cont = 1;

  while (cont)
    {
      /* fill buffer - leaving room for nul-terminator */
      while (--numbytes > 0)
        {
          if ((ch = getc_unlocked (fp)) == EOF)
            {
	      cont = 0;
              break;
            }
	  else 
            {
              *ptr++ = ch;
              if (ch == delim)
                {
                  cont = 0;
                  break;
                }
            }
        }

      if (cont)
        {
          /* Buffer is too small so reallocate a larger buffer.  */
          pos = ptr - buf;
          newsize = (*n << 1);
          buf = realloc (buf, newsize);
          if (buf == NULL)
            {
              cont = 0;
              break;
            }

          /* After reallocating, continue in new buffer */          
          *bufptr = buf;
          *n = newsize;
          ptr = buf + pos;
          numbytes = newsize - pos;
        }
    }

  _newlib_flockfile_end (fp);

  /* if no input data, return failure */
  if (ptr == buf)
    return -1;

  /* otherwise, nul-terminate and return number of bytes read */
  *ptr = '\0';
  return (ssize_t)(ptr - buf);
}
