/*
 * Copyright(C) 2016-2017 Davidson Francis <davidsondfgl@gmail.com>
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

/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "%W% (Berkeley) %G%";
#endif /* LIBC_SCCS and not lint */

#include <_ansi.h>
#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "local.h"

/*
 * Expand the ungetc buffer `in place'.  That is, adjust fp->_p when
 * the buffer moves, so that it points the same distance from the end,
 * and move the bytes in the buffer around as necessary so that they
 * are all at the end (stack-style).
 */

/*static*/
int __submore(struct _reent *rptr, register FILE *fp)
{
  register int i;
  register unsigned char *p;

  if (fp->_ub._base == fp->_ubuf)
    {
      /*
       * Get a new buffer (rather than expanding the old one).
       */
      if ((p = (unsigned char *) _malloc_r (rptr, (size_t) BUFSIZ)) == NULL)
	return EOF;
      fp->_ub._base = p;
      fp->_ub._size = BUFSIZ;
      p += BUFSIZ - sizeof (fp->_ubuf);
      for (i = sizeof (fp->_ubuf); --i >= 0;)
	p[i] = fp->_ubuf[i];
      fp->_p = p;
      return 0;
    }
  i = fp->_ub._size;
  p = (unsigned char *) _realloc_r (rptr, (_PTR) (fp->_ub._base), i << 1);
  if (p == NULL)
    return EOF;
  _CAST_VOID memcpy ((_PTR) (p + i), (_PTR) p, (size_t) i);
  fp->_p = p + i;
  fp->_ub._base = p;
  fp->_ub._size = i << 1;
  return 0;
}

int _ungetc_r(struct _reent *rptr, int c, register FILE *fp)
{
  if (c == EOF)
    return (EOF);

  /* Ensure stdio has been initialized.
     ??? Might be able to remove this as some other stdio routine should
     have already been called to get the char we are un-getting.  */

  CHECK_INIT (rptr, fp);

  _newlib_flockfile_start (fp);

  ORIENT (fp, -1);

  /* After ungetc, we won't be at eof anymore */
  fp->_flags &= ~__SEOF;

  if ((fp->_flags & __SRD) == 0)
    {
      /*
       * Not already reading: no good unless reading-and-writing.
       * Otherwise, flush any current write stuff.
       */
      if ((fp->_flags & __SRW) == 0)
        {
          _newlib_flockfile_exit (fp);
          return EOF;
        }
      if (fp->_flags & __SWR)
	{
	  if (_fflush_r (rptr, fp))
            {
              _newlib_flockfile_exit (fp);
              return EOF;
            }
	  fp->_flags &= ~__SWR;
	  fp->_w = 0;
	  fp->_lbfsize = 0;
	}
      fp->_flags |= __SRD;
    }
  c = (unsigned char) c;

  /*
   * If we are in the middle of ungetc'ing, just continue.
   * This may require expanding the current ungetc buffer.
   */

  if (HASUB (fp))
    {
      if (fp->_r >= fp->_ub._size && __submore (rptr, fp))
        {
          _newlib_flockfile_exit (fp);
          return EOF;
        }
      *--fp->_p = c;
      fp->_r++;
      _newlib_flockfile_exit (fp);
      return c;
    }

  /*
   * If we can handle this by simply backing up, do so,
   * but never replace the original character.
   * (This makes sscanf() work when scanning `const' data.)
   */

  if (fp->_bf._base != NULL && fp->_p > fp->_bf._base && fp->_p[-1] == c)
    {
      fp->_p--;
      fp->_r++;
      _newlib_flockfile_exit (fp);
      return c;
    }

  /*
   * Create an ungetc buffer.
   * Initially, we will use the `reserve' buffer.
   */

  fp->_ur = fp->_r;
  fp->_up = fp->_p;
  fp->_ub._base = fp->_ubuf;
  fp->_ub._size = sizeof (fp->_ubuf);
  fp->_ubuf[sizeof (fp->_ubuf) - 1] = c;
  fp->_p = &fp->_ubuf[sizeof (fp->_ubuf) - 1];
  fp->_r = 1;
  _newlib_flockfile_end (fp);
  return c;
}

#ifndef _REENT_ONLY

/**
 * @brief Pushes byte back into input stream.
 *
 * @details Pushes the byte specified by @p c (converted
 * to an unsigned char) back onto the input stream pointed
 * to by stream. The pushed-back bytes is returned by subsequent
 * reads on that stream in the reverse order of their pushing. 
 *
 * @return Returns the byte pushed back after conversion. Otherwise,
 * returns EOF.
 */
int ungetc(int c,register FILE *fp)
{
  return _ungetc_r (_REENT, c, fp);
}

#endif /* !_REENT_ONLY */
