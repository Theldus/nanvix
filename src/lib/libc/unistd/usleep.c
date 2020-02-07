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

#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

/*
 * @brief Suspend execution for an interval of @p usec microseconds.
 *
 * @param usec Microseconds to sleep.
 *
 * @return Returns 0 if success, otherwise, returns -1 and set
 * the errno variable.
 */
int usleep(useconds_t usec)
{
	struct timespec time;

	/* Fill timespec structure. */
	time.tv_sec = usec / 1000000;
	time.tv_nsec = (usec % 1000000) * 1000;

	/*
	 * Nanosleep already returns 0 if success, -1 if error
	 * and already sets the errno variable, so nothing
	 * special here.
	 */
	return (nanosleep(&time, NULL));
}
