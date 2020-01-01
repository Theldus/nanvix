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

/*	$NetBSD: wcsstr.c,v 1.1 2000/12/23 23:14:37 itojun Exp $	*/

/*-
 * Copyright (c)1999 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	citrus Id: wcsstr.c,v 1.2 2000/12/21 05:07:25 itojun Exp
 */

#include <_ansi.h>
#include <stddef.h>
#include <wchar.h>

/**
 * @brief Finds a wide-character substring.
 *
 * @details Locates the first occurrence in the wide-character
 * string pointed to by @p big of the sequence of wide characters
 * (excluding the terminating null wide character) in the wide-character
 * string pointed to by @p little.
 *
 * @return Returns a pointer to the located wide-character string, or a 
 * null pointer if the wide-character string is not found.
 */
wchar_t *wcsstr(const wchar_t * big, const wchar_t * little)
{
  const wchar_t *p;
  const wchar_t *q;
  const wchar_t *r;

  if (!*little)
    {
      /* LINTED interface specification */
      return (wchar_t *) big;
    }
  if (wcslen (big) < wcslen (little))
    return NULL;

  p = big;
  q = little;
  while (*p)
    {
      q = little;
      r = p;
      while (*q)
	{
	  if (*r != *q)
	    break;
	  q++;
	  r++;
	}
      if (!*q)
	{
	  /* LINTED interface specification */
	  return (wchar_t *) p;
	}
      p++;
    }
  return NULL;
}
