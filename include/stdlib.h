/*
 * Copyright(C) 2017 Davidson Francis <davidsondfgl@gmail.com>
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
 * Copyright (c) 1994-2009  Red Hat, Inc. All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the BSD License.   This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
 * including the implied warranties of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  A copy of this license is available at 
 * http://www.opensource.org/licenses. Any Red Hat trademarks that are
 * incorporated in the source code or documentation are not subject to
 * the BSD License and may only be used or replicated with the express
 * permission of Red Hat, Inc.
 */

/*
 * Copyright (c) 1981-2000 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, 
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of the University nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software 
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

/**
 * @file
 * 
 * @brief Standard library definitions.
 */

#ifndef STDLIB_H_
#define STDLIB_H_

	#include <machine/ieeefp.h>
	#include "_ansi.h"

	#define __need_size_t
	#define __need_wchar_t
	#define __need_NULL
	#include <stddef.h>

	#include <sys/reent.h>
	#include <sys/cdefs.h>

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)

 	#include <limits.h>
	#include <math.h>
	#include <sys/wait.h>

#endif

 	/**
	 * @name Exit codes.
	 */
	/**@{*/
	#define EXIT_FAILURE 1 /**< Unsuccessful termination for exit(). */
	#define EXIT_SUCCESS 0 /**< Successful termination for exit().   */
	/**@}*/

	/**
	 * @brief Maximum value returned by rand().
	 */
	#define RAND_MAX __RAND_MAX

	/**
	 * @brief Maximum number of bytes in a locale character.
	 */
	#define MB_CUR_MAX 1

 	/**
	 * @brief Structure type returned by the div() function.
	 */
	typedef struct 
	{
	  int quot; /**< Quotient. */
	  int rem; /**<  Remainder. */
	} div_t;

	/**
	 * @brief Structure type returned by the ldiv() function.
	 */
	typedef struct 
	{
	  long quot; /**< Quotient. */
	  long rem; /**<  Remainder. */
	} ldiv_t;

	/**
	 * @brief Structure type returned by the lldiv() function.
	 */
	typedef struct
	{
	  long long int quot; /**< Quotient. */
	  long long int rem; /**<  Remainder. */
	} lldiv_t;


	/**
	 * @defgroup stdlib Standard Library
	 * 
	 * @brief Standard library definitions.
	 */
	/**@{*/

	/* Forward declarations. */
	extern void _Exit(int);
	extern void abort(void);
	extern int abs(int);
	extern int atexit(void (*)(void));
	extern double atof(const char *);
	extern int atoi(const char *);
	extern long atol(const char *);
	extern long long atoll(const char *);
	extern void *bsearch(const void *, const void *, size_t, size_t, 
		int (*)(const void *, const void *));
	extern void *calloc(size_t, size_t);
	extern div_t div(int, int);
	extern void exit(int);
	extern void free(void *);
	extern char *getenv(const char *);
	extern long labs(long);
	extern ldiv_t ldiv(long, long);
	extern long long llabs(long long);
	extern lldiv_t lldiv(long long, long long);
	extern void *malloc(size_t);
	extern int mblen(const char *, size_t);
	extern size_t mbstowcs(wchar_t *, const char *, size_t);
	extern int mbtowc(wchar_t *, const char *, size_t);
	extern void qsort(void *, size_t, size_t, 
		int (*)(const void *, const void *));
	extern int rand(void);
	extern void *realloc(void *, size_t);
	extern void srand(unsigned);
	extern double strtod(const char *, char **);
	extern float strtof(const char *, char **);
	extern long strtol(const char *, char **, int);
	extern long double strtold(const char *, char **);
	extern long long strtoll(const char *, char **, int);
	extern unsigned long strtoul(const char *, char **, int);
	extern unsigned long long strtoull(const char *, char **, int);
	extern int system(const char *);
	extern size_t wcstombs(char *, const wchar_t *, size_t);
	extern int wctomb(char *, wchar_t);

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
	extern int getsubopt(char **, char *const *, char **);
	extern char *mkdtemp(char *);
	extern int setenv(const char *, const char *, int);
	extern int unsetenv(const char *);
	extern int rand_r(unsigned *);
#endif

	extern long a64l(const char *);
	extern double drand48(void);
	extern double erand48(unsigned short [3]);
	extern long jrand48(unsigned short [3]);
	extern char *l64a(long);
	extern void lcong48(unsigned short [7]);
	extern long lrand48(void);
	extern long mrand48(void);
	extern long nrand48(unsigned short [3]);
	extern int putenv(char *);
	extern unsigned short *seed48(unsigned short [3]);
	extern void srand48(long);

	/* Newlib needs to those non-standard functions. */
	extern char *_dtoa_r(struct _reent *, double, int, int, int *, int *, char **);
	extern char *_findenv_r(struct _reent *, const char *, int *);
	extern char *_getenv_r(struct _reent *, const char *);
	extern double _strtod_r(struct _reent *, const char *, 
		char **);
	extern int mkstemp(char *);
	extern int _setenv_r(struct _reent *, const char *, const char *, int);
	extern int _unsetenv_r(struct _reent *, const char *);
	extern long _strtol_r(struct _reent *,const char *, char **,
		int);
	extern long long _strtoll_r(struct _reent *, const char *,
		char **, int);
	extern unsigned long _strtoul_r(struct _reent *,const char *,
		char **, int);
	extern unsigned long long _strtoull_r(struct _reent *, const char *,
		char **, int);
	extern void	_free_r(struct _reent *, void *);
	extern void *_calloc_r(struct _reent *, size_t, size_t);
	extern void *_malloc_r(struct _reent *, size_t);
	extern void *_realloc_r(struct _reent *, void *, size_t);

	/**@}*/

#endif /* STDLIB_H_ */
