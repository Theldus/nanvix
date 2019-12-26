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
 *
 *	@(#)stdio.h	5.3 (Berkeley) 3/15/86
 */

#ifndef STDIO_H_
#define STDIO_H_

	#include "_ansi.h"

	#define __need_size_t
	#define __need_NULL
	#include <sys/cdefs.h>
	#include <stddef.h>

	#include <limits.h>
	/*
	 * GCC tends to use its own stdarg when using
	 * the cross compiler. For some reason, if
	 * defined __need___va_list, GCC does not
	 * export it... bug?
	 */
#ifndef __nanvix__
	#define __need___va_list
#endif
	#include <stdarg.h>

	#include <sys/reent.h>
	#include <sys/types.h>

	/**
	 * @brief File stream.
	 */
#ifndef FILE_T
	#define FILE_T
	typedef __FILE FILE;
#endif

	/**
	 * @brief File position.
	 */
	typedef _fpos_t fpos_t;

	#include <sys/stdio.h>

	/**
	 * @brief Standard buffer size.
	 */
	#define BUFSIZ 1024

	/**
	 * @brief End of file.
	 */
	#define EOF (-1)

	/**
	 * @brief Maximum size in bytes of the longest pathname.
	 */
	#define FILENAME_MAX NAME_MAX

	/**
	 * @brief Maximum number of opened file streams.
	 */
	#define FOPEN_MAX OPEN_MAX

	/**
	 * @brief Minimum number of unique filenames generated by tmpnam().
	 */
	#define TMP_MAX 26

	/**
	 * @brief Newlib flags.
	 */
	/**@{*/
	#define	__SLBF 0x0001 /**< Line buffered.                                 */
	#define	__SNBF 0x0002 /**< Unbuffered.                                    */
	#define	__SRD  0x0004 /**< OK to read.                                    */
	#define	__SWR  0x0008 /**< OK to write.                                   */
	#define	__SRW  0x0010 /**< Open for reading & writing.                    */
	#define	__SEOF 0x0020 /**< Found EOF.                                     */
	#define	__SERR 0x0040 /**< Found error.                                   */
	#define	__SMBF 0x0080 /**< _buf is from malloc.                           */
	#define	__SAPP 0x0100 /**< Fdopen()ed in append mode.                     */
	#define	__SSTR 0x0200 /**< This is an sprintf/snprintf string.            */
	#define	__SOPT 0x0400 /**< Do fseek() optimisation.                       */
	#define	__SNPT 0x0800 /**< Do not do fseek() optimisation.                */
	#define	__SOFF 0x1000 /**< Set iff _offset is in fact correct.            */
	#define	__SORD 0x2000 /**< True => stream orientation decided.            */
	#define	__SL64 0x8000 /**< Is 64-bit offset large file.                   */
	#define	__SNLK 0x0001 /**< Stdio functions do not lock streams themselves.*/
	#define	__SWID 0x2000 /**< True => stream orientation wide.               */
	/**@}*/

	/**
	 * @brief File stream flags.
	 */
	/**@{*/
	#define	_IOFBF 0 /**< Fully buffered? */
	#define	_IOLBF 1 /**< Line buffered?  */
	#define	_IONBF 2 /**< Unbuffered?     */
	/**@}*/

	/**
	 * @brief File offsets.
	 */
	/**@{*/
	#define SEEK_SET 0 /**< Set file offset to offset.              */
	#define SEEK_CUR 1 /**< Set file offset to current plus offset. */
	#define SEEK_END 2 /**< Set file offset to EOF plus offset.     */
	/**@}*/

	/**
	 * @brief Maximum size of character array to hold tmpnam() output.
	 */    
	#define L_tmpnam FILENAME_MAX

	/**
	 * @brief Default directory prefix for tempnam().
	 */
	#define P_tmpdir "/tmp"

	/**
	 * @brief Standard file streams.
	 */
	/**@{*/
	#define stdin  (_REENT->_stdin)  /**< Standard input.  */
	#define stdout (_REENT->_stdout) /**< Standard output. */
	#define stderr (_REENT->_stderr) /**< Standard error.  */
	/**@}*/

	/**
	 * @brief Reentrant standard file streams.
	 */
	/**@{*/
	#define _stdin_r(x)	 ((x)->_stdin)  /**< Standard input.  */
	#define _stdout_r(x) ((x)->_stdout) /**< Standard output. */
	#define _stderr_r(x) ((x)->_stderr) /**< Standard error.  */
	/**@}*/

	/**
	 * @defgroup stdio Stdio library.
	 *
	 * @brief Standard buffered input/output.
	 */
	/**@{*/

	/* Forward declarations. */
	extern void clearerr(FILE *);
	extern int fclose(FILE *);
	extern int feof(FILE *);
	extern int ferror(FILE *);
	extern int fflush(FILE *);
	extern int fgetc(FILE *);
	extern int fgetpos(FILE *, fpos_t *);
	extern char *fgets(char *, int, FILE *);
	extern FILE *fopen(const char *, const char *);
	extern int fprintf(FILE *, const char *, ...);
	extern int fputc(int, FILE *);
	extern int fputs(const char *, FILE *);
	extern size_t fread(void *, size_t, size_t, FILE *);
	extern FILE *freopen(const char *, const char *,
		FILE *);
	extern int fscanf(FILE *, const char *, ...);
	extern int fseek(FILE *, long, int);
	extern int fsetpos(FILE *, const fpos_t *);
	extern long ftell(FILE *);
	extern size_t fwrite(const void *, size_t, size_t, FILE *);
	extern int getc(FILE *);
	extern int getchar(void);
	extern void perror(const char *);
	extern int printf(const char *, ...);
	extern int putc(int, FILE *);
	extern int putchar(int);
	extern int puts(const char *);
	extern int remove(const char *);
	extern int rename(const char *, const char *);
	extern void rewind(FILE *);
	extern int scanf(const char *, ...);
	extern void setbuf(FILE *, char *);
	extern int setvbuf(FILE *, char *, int, size_t);
	extern int snprintf(char *, size_t, const char *, ...);
	extern int sprintf(char *, const char *, ...);
	extern int sscanf(const char *, const char *, ...);
	extern FILE *tmpfile(void);
	extern int ungetc(int, FILE *);
	extern int vfprintf(FILE *, const char *, va_list);
	extern int vfscanf(FILE *, const char *, va_list);
	extern int vprintf(const char *, va_list);
	extern int vscanf(const char *, va_list);
	extern int vsnprintf(char *, size_t, const char *, va_list);
	extern int vsprintf(char *, const char *, va_list);
	extern int vsscanf(const char *, const char *, va_list);

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)

	extern int dprintf(int, const char *, ...);
	extern FILE *fdopen(int, const char *);
	extern int fileno(FILE *);
	extern void flockfile(FILE *);
	extern FILE *fmemopen(void *, size_t, const char *);
	extern int fseeko(FILE *, off_t, int);
	extern off_t ftello(FILE *);
	extern int ftrylockfile(FILE *);
	extern void funlockfile(FILE *);
	extern int getc_unlocked(FILE *);
	extern int getchar_unlocked(void);
	extern FILE *open_memstream(char **, size_t *);
	extern int putc_unlocked(int, FILE *);
	extern int putchar_unlocked(int);
	extern int vdprintf(int, const char *, va_list);

#endif

	/* Newlib needs to those non-standard functions. */
	extern int __srget_r(struct _reent *, FILE *);
	extern int __swbuf_r(struct _reent *, int, FILE *);
	extern int _fclose_r(struct _reent *, FILE *);
	extern FILE *_fdopen_r(struct _reent *, int, const char *);
	extern int _fflush_r(struct _reent *, FILE *);
	extern int _fputc_r(struct _reent *, int, FILE *);
	extern size_t _fread_r(struct _reent *, void *, size_t _size,
		size_t _n, FILE *);
	extern int _fseek_r(struct _reent *, FILE *, long, int);
	extern int _fseeko_r(struct _reent *, FILE *, _off_t, int);
	extern long _ftell_r(struct _reent *, FILE *);
	extern _off_t _ftello_r(struct _reent *, FILE *);
	extern int _getc_r(struct _reent *, FILE *);
	extern int _getc_unlocked_r(struct _reent *, FILE *);
	extern int _putc_r(struct _reent *, int, FILE *);
	extern int _rename(const char *, const char *);
	extern int _remove_r(struct _reent *, const char *);	
	extern int _sprintf_r(struct _reent *, char *, const char *,
		...);
	extern char *_tmpnam_r(struct _reent *, char *);
	extern int _ungetc_r(struct _reent *, int, FILE *);
	extern char *_vasnprintf_r(struct _reent*, char *, size_t *, const char *,
		va_list);
	extern int _vdprintf_r(struct _reent *, int, const char *, va_list);
	extern int _vfiprintf_r(struct _reent *, FILE *, const char *, va_list);
	extern int _vfprintf_r(struct _reent *, FILE *, const char *,
		va_list);
	extern int _vfscanf_r(struct _reent *, FILE *, const char *,
		va_list);
	extern int fiprintf(FILE *, const char *, ...);
	extern int siprintf(char *, const char *, ...);
	extern int siscanf(const char *, const char *, ...);
	extern int sniprintf(char *, size_t, const char *, ...);

	/**@}*/

#endif /* STDIO_H_ */
