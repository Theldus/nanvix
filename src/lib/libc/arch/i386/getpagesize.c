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
#include <errno.h>
#include <reent.h>

/*
 * Gets the current pagesize for the underlying architecture.
 *
 * @return Return the current page size for i386 arch.
 *
 * @note Although this is intended to be a syscall, there is
 * no need to do that, since the x86 target will always use
 * 4 KiB here.
 */
int getpagesize(void)
{
	return (4096);
}
