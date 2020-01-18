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

#include <stdio.h>
#include <sys/syslimits.h>
#include <sys/unistd.h>
#include <time.h>

/* Some values. */
#define SYSCONF_MAX_VALUE    9
#define SYSCONF_UNSUPPORTED -1
#define SYSCONF_LATE        -2

/* Initializers. */
extern void setup_pagesize(void);

/*
 * Sysconf table.
 */
long sysconf_values[SYSCONF_MAX_VALUE] =
{
	[_SC_ARG_MAX]      = ARG_MAX,
	[_SC_CHILD_MAX]    = CHILD_MAX,
	[_SC_CLK_TCK]      = CLOCKS_PER_SEC,
	[_SC_NGROUPS_MAX]  = SYSCONF_UNSUPPORTED,
	[_SC_OPEN_MAX]     = OPEN_MAX,
	[_SC_JOB_CONTROL]  = SYSCONF_UNSUPPORTED,
	[_SC_SAVED_IDS]    = SYSCONF_UNSUPPORTED,
	[_SC_VERSION]      = 199009L,
	[_SC_PAGESIZE]     = SYSCONF_LATE
};

/*
 * Values initialized late.
 *
 * @note Although this could lead to a waste of space,
 * this table can be pretty fast when retrieving
 * values that needs to issue a syscall.
 */
void (*sysconf_initializers[SYSCONF_MAX_VALUE])(void) =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	setup_pagesize
};

/* Setup page size initializer. */
void setup_pagesize() {sysconf_values[_SC_PAGESIZE] = getpagesize();}

/*
 * Get configurable system variables
 *
 * @param name Value to be retrieved.
 *
 * @return Returns the value for the specified @p name.
 */
long sysconf(int name)
{
	int ret; /* Return value. */

	/* Max supported values. */
	if (name >= SYSCONF_MAX_VALUE)
	{
		printf("sysconf name: %d, exceeds the values supported!\n",
			name);
		return (-1);
	}

	/* If unsupported. */
	if ((ret = sysconf_values[name]) == SYSCONF_UNSUPPORTED)
		printf("sysconf name: %d, not supported yet!\n", name);
		
	/* If late initialized. */
	if (ret == SYSCONF_LATE)
	{
		sysconf_initializers[name]();
		ret = sysconf_values[name];
	}

	return (ret);
}
