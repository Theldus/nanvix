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

#include <signal.h>
#include <time.h>
#include <unistd.h>

/* Time before sleeping. */
time_t start_seconds;

/* Signal received */
volatile int signal_recv = SIGALRM;

/* SIGALRM handler. */
static void sig_handler(int no)
{
	signal_recv = no;
}

/*
 * @brief Suspend execution for an interval of @p seconds.
 *
 * @param seconds Seconds to sleep.
 *
 * @return Returns 0 if success, otherwise, the number of seconds
 * remaining to sleep.
 */
unsigned int sleep(unsigned int seconds)
{
	signal(SIGALRM, sig_handler);
	alarm(seconds);

	/* Save current time. */
	start_seconds = time(NULL);

	/* Go sleep and wait for a signal. */
	pause();

	/*
	 * If error, returns the number of seconds remaining to sleep.
	 */
	if (signal_recv != SIGALRM)
	{
		return (
			(unsigned)(start_seconds) + seconds - (unsigned)(time(NULL))
			);
	}

	return (0);
}
