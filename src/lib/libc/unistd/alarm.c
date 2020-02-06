/*
 * Copyright(C) 2019-2019 Davidson Francis <davidsondfgl@gmail.com>
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

#include <nanvix/syscall.h>
#include <unistd.h>
#include <errno.h>

/*
 * @brief Generates a SIGALRM signal for the process after the number
 * of seconds specified by @p seconds have elapsed.
 *
 * @param seconds Seconds to generate the SIGALRM signal.
 *
 * @return Returns the number of seconds remaining until any previously
 * scheduled alarm was due to be delivered, or zero if there was no
 * previously scheduled alarm.
 */
unsigned int alarm(unsigned int seconds)
{
	int ret;

	__asm__ volatile (
		"int $0x80"
		: "=a" (ret)
		: "0" (NR_alarm),
		  "b" (seconds)
	);

	/* Error. */
	if (ret < 0)
	{
		errno = -ret;
		_REENT->_errno = -ret;
		return (-1);
	}

	return (ret);
}
