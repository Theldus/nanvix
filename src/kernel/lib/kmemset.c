/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
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
#include <nanvix/hal.h>
#include <nanvix/klib.h>
#include <sys/types.h>

/**
 * @brief Sets bytes in memory.
 * 
 * @param ptr Pointer to target memory area.
 * @param c   Character to use.
 * @param n   Number of bytes to be set.
 * 
 * @returns A pointer to the target memory area. 
 */
PUBLIC void *kmemset(void *ptr, int c, size_t n)
{
    unsigned char *p;
    addr_t *addr;
   	dword_t buff;

    p = ptr;

    /* Copy until address be aligned. */
    while (!ALIGNED(p, sizeof(addr_t)))
    {
    	if (n--)
			*p++ = (unsigned char) c;
		else
			return (ptr);
	}

	/* Copy 4-bytes at a time. */
	buff = (c << 24) | (c << 16) | (c << 8) | c;
	addr = (addr_t *)p;

	while (n >= sizeof(addr_t))
	{
		*addr++ = buff;
		n -= sizeof(addr_t);
	}

	/* Copy remaining bytes if any. */
	p = (unsigned char *)addr;
	while (n-- > 0)
		*p++ = (unsigned char) c;

	return (ptr);	
}
