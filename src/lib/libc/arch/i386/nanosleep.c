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

#include <nanvix/syscall.h>
#include <nanvix/clock.h>
#include <sys/types.h>
#include <errno.h>
#include <reent.h>

/*
 * Suspends  the execution of the process until either at least
 * the time specified in @p *rqtp has elapsed, or the delivery
 * of a signal that triggers the invocation of a handler in the
 * calling process or that terminates the process.

 * If the call is interrupted by a signal handler, nanosleep()
 * returns -1, sets  errno  to EINTR, and writes the remaining
 * time into the structure pointed to by @p rmtp unless rmtp is
 * NULL. The value of *rmtp can then be used to call nanosleep()
 * again and complete the specified pause.
 *
 * @param rqtp Requested time to sleep.
 * @param rmtp If awakened by a signal, saves the remaining time to
 *        sleep if rmtp is non-null.
 *
 * @return Returns 0 if success and -1 if error. In the latter case,
 * errno contains the reason.
 */
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
	int ret;        /* Return value.         */
	int tv_sec;     /* Seconds to sleep.     */
	int tv_nsec;    /* Nanoseconds to sleep. */

	tv_sec = rqtp->tv_sec;
	tv_nsec = rqtp->tv_nsec;

	__asm__ volatile (
		"int $0x80"
		: "=a" (ret)
		: "0" (NR_nanosleep),
		  "b" (tv_sec),
		  "c" (tv_nsec)
	);

	/* Error. */
	if (ret != 0)
	{
		/* Invalid values */
		if (ret < 0)
		{
			errno = -ret;
			_REENT->_errno = -ret;
			return (-1);
		}

		/* Interrupted by a signal. */
		errno = EINTR;
		_REENT->_errno = EINTR;

		/* Shall we update rmtp with the remaining time?. */
		if (rmtp != NULL)
		{
			rmtp->tv_sec  = ret/CLOCK_FREQ;
			rmtp->tv_nsec = (ret % CLOCK_FREQ) * CLOCK_INTERVAL_PER_MS;
		}

		return (-1);
	}

	return (0);
}
