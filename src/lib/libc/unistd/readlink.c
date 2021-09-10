/*
 * Copyright(C) 2021 Davidson Francis <davidsondfgl@gmail.com>
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

#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

/*
 * @brief Read value of a symbolic link.
 *
 * @param pathname Symbolic link path name.
 * @param buf Buffer.
 * @param bufsiz Buffer size.
 *
 * @return Returns 0 if success, -1 otherwise.
 */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
	((void)buf);
	((void)bufsiz);

	int ret;
	struct stat sbuf;

	errno = EINVAL;

	/* 
	 * Currently we do not support symbolic link, so we
	 * just kindly return errors accordingly if the file
	 * exists or not.
	 */
	ret = stat(pathname, &sbuf);
	if (ret == -1)
		errno = ENOENT;

	return (-1);
}
