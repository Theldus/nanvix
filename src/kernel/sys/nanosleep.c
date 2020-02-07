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

#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <nanvix/clock.h>
#include <nanvix/pm.h>
#include <errno.h>
#include <signal.h>

/*
 * @brief Puts the current process to sleep for a given
 * amount of seconds and nanoseconds.
 *
 * @param tv_sec Seconds to sleep.
 * @param tv_nsec Nanoseconds to sleep.
 *
 * @return Returns 0 if success, otherwise, the number of remaining
 * ticks to sleep.
 *
 * @note: Although Nanvix appears to support nanosecond precision,
 * it actually supports millisecond precision. If tv_nsec is less
 * than the supported precision, the process will sleep for the
 * shortest supported time (currently 10ms).
 */
PUBLIC int sys_nanosleep(int tv_sec, int tv_nsec)
{
	int curr_ticks;
	int tv_msec;
	curr_ticks = ticks;

	/* Values range. */
	if (tv_sec < 0 || tv_nsec < 0 || tv_sec >= 1000000000 || tv_nsec >= 1000000000)
		return (-EINVAL);

	/* Nanvix only supports ms precision. */
	tv_msec = tv_nsec/1000000;
	curr_proc->ns_ticks = curr_ticks + tv_sec*CLOCK_FREQ;

	/*
	 * Check if Nanvix is able to proceed with the given granularity (ms),
	 * if not, lets sleep within the minimal amount of time supported.
	 */
	if (tv_msec >= CLOCK_INTERVAL_PER_MS)
		curr_proc->ns_ticks += (unsigned) tv_msec/CLOCK_INTERVAL_PER_MS;
	else if (tv_nsec != 0)
		curr_proc->ns_ticks += 1;

	/* Susped process. */
	sleep(&curr_proc->ns_chain, PRIO_USER);

	/*  Wakeup on signal receipt. */
	if (issig() != SIGNULL)
		return (curr_proc->ns_ticks - ticks);

	return (0);
}
