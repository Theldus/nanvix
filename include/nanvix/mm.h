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

#ifndef MM_H_
#define MM_H_
	
	#include <nanvix/const.h>
	#include <nanvix/hal.h>
	#include <nanvix/pm.h>
	#include <sys/types.h>
	
	/**
	 * Nanvix Memory Layout notes:
	 *
	 * Some terms:
	 * The term 'identity mapping' will be used here to also
	 * indicate when the physical address can be obtained
	 * by substracting KBASE_VIRT (i.e: 0xc000 0000).
	 *
	 * Dots indicate zeroes at right of the number until complete
	 * 32-bit unsigned wide, thus: 0xc000 0000 can be written as
	 * '0xc0...'.
	 *
	 * The following memory layout follows the notes below:
	 * - INITRD_VIRT _must_ be identity mapped with INITRD_PHYS
	 *
	 * - KPOOL_VIRT _should_ be placed after INITRD_VIRT _and_
	 *   after CMD_LINE, which occupies 4MiB. (CMD_LINE will be
	 *   mapped right after INITRD. (see boot.S for details).
	 *   Thus, 0xc0... + 0x050... + 0x004...
	 *
	 * - SERIAL_VIRT should be placed KPOOL_SIZE MiB after KPOOL_VIRT.
	 *   KPOOL_SIZE occupies 16MiB.
	 *
	 * - UBASE_PHYS _should_ be placed KPOOL_SIZE MiB after
	 *   KPOOL_PHYS.
	 *
	 * - CMD_LINE/KCML_SIZE mapps to the command line options passed
	 *   by boot and is only used during the kmain(). Afterwards, it's
	 *   usage is no longer needed.
	 */

	/* Kernel stack size. */
	#define KSTACK_SIZE PAGE_SIZE

	/* Virtual memory layout. */
	#define UBASE_VIRT   0x02000000 /* User base.        */
	#define KBASE_VIRT   0xc0000000 /* Kernel base.      */
	#define INITRD_VIRT  0xc1000000 /* Initial RAM disk. */
	#define KPOOL_VIRT   0xc5400000 /* Kernel page pool. */
	#define SERIAL_VIRT  0xc6400000 /* Serial port.      */
	
	/* Physical memory layout. */
	#define KBASE_PHYS   0x00000000 /* Kernel base.      */
	#define KPOOL_PHYS   0x05400000 /* Kernel page pool. */
	#define UBASE_PHYS   0x06400000 /* User base.        */
	
	/* User memory layout. */
	#define USTACK_ADDR 0xc0000000 /* User stack. */
	#define UHEAP_ADDR  0xa0000000 /* User heap.  */

	/* Kernel memory size: 16 MB. */
	#define KMEM_SIZE 0x01000000
	
	/* Kernel page pool size: 16 MB. */
	#define KPOOL_SIZE 0x01000000
	
	/* Kernel command line size: 4MB, (yeah, waste of space). */
	#define KCMDL_SIZE 0x00400000

	/* User memory size. */
	#define UMEM_SIZE (MEMORY_SIZE - \
		KMEM_SIZE   - \
		KPOOL_SIZE  - \
		INITRD_SIZE - \
		KCMDL_SIZE    \
	)

	/**
	 * @brief Asserts if an address lies on kernel space.
	 *
	 * @param addr Target address.
	 *
	 * @returns True if address lies on kernel space and false
	 * otherwise.
	 */
	#define IN_KERNEL(addr)               \
		(((addr_t)(addr) < UBASE_VIRT) || \
		 ((addr_t)(addr) >= KBASE_VIRT))

#ifndef _ASM_FILE_
	
	/* Buffers virt. */
	EXTERN unsigned const BUFFERS_VIRT;

	/* Forward definitions. */
	EXTERN int chkmem(const void *, size_t, mode_t);
	EXTERN int fubyte(const void *);
	EXTERN int fudword(const void *);
	EXTERN int crtpgdir(struct process *);
	EXTERN int pfault(addr_t);
	EXTERN int vfault(addr_t);
	EXTERN void dstrypgdir(struct process *);
	EXTERN void putkpg(void *);
	EXTERN void mm_init(void);
	EXTERN void *getkpg(int);

#endif /* _ASM_FILE_ */
	
#endif /* MM_H_ */
