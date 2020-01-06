/*
 * Copyright(C) 2017-2017 Davidson Francis <davidsondfgl@gmail.com>
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
#include <i386/fpu.h>
#include <i386/pmc.h>

/**
 * Grabs CPU information from x86.
 * @param eax EAX register.
 * @param ebx EBX register.
 * @param ecx ECX register.
 * @param edx EDX register.
 */
PUBLIC void cpuid(
	unsigned *eax, unsigned *ebx,
	unsigned *ecx, unsigned *edx
)
{
	__asm__ __volatile__
	(
		"cpuid"
		: "=a" (*eax),
		  "=b" (*ebx),
		  "=c" (*ecx),
		  "=d" (*edx)
		: "0" (*eax), "2" (*ecx)
	);
}

/*
 * @brief Initializes the CPU resources.
 */
PUBLIC void cpu_init(void)
{
	pmc_init();
	fpu_init();
}
