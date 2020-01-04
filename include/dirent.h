/*
 * Copyright(C) 2011-2017 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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

#ifndef DIRENT_H_
#define DIRENT_H_

	#include <sys/types.h>
	#include <limits.h>
	
 	#ifndef MAXNAMLEN
	#define MAXNAMLEN 30
 	#endif

	/*
	 * Directory entry.
	 */
	struct dirent
	{
		ino_t d_ino;           /* File serial number. */
		char d_name[NAME_MAX]; /* Name of entry.      */
	};
	
	/* Directory stream buffer size. */
	#define _DIR_BUFSIZ ((1024/sizeof(struct dirent))*sizeof(struct dirent))

	/* Directory stream flags. */
	#define _DIR_VALID 001 /* Valid directory?  */
	#define _DIR_EOD   002 /* End of directory? */

	/*
	 * Directory stream.
	 */
	typedef struct
	{
		int fd;             /* Underlying file descriptor.       */
		int flags;          /* Flags (see above).                */
		int count;          /* Valid entries left in the buffer. */
		struct dirent *ptr; /* Next valid entry in the buffer.   */
		struct dirent *buf; /* Buffer of directory entries.      */
	} DIR;
	
	/*
	 * Closes a directory stream.
	 */
	extern int closedir(DIR *dirp);
	
	/*
	 * Opens a directory stream.
	 */
	extern DIR *opendir(const char *dirname);
	
	/*
	 * Reads a directory.
	 */
	extern struct dirent *readdir(DIR *dirp);
	
	/*
	 * Rewinds a directory stream.
	 */
	extern void rewinddir(DIR *dirp);

#endif /* DIRENT_H_ */
