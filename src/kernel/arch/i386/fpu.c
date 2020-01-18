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
#include <nanvix/klib.h>
#include <i386/fpu.h>

/* External declarations. */
extern void cpuid(unsigned *, unsigned *,
	unsigned *, unsigned *);

/**
 * Aligned buffer for temporarily save the FPU state.
 */
PUBLIC struct fpu fpu_temp __attribute__((aligned(16)));

/*
 * @brief Initializes the FPU.
 */
PUBLIC void fpu_init(void)
{
	unsigned eax;
	unsigned ebx;
	unsigned ecx;
	unsigned edx;	

	/* Enable FPU. */
	__asm__ __volatile("fninit");

	/* 
	 * Check if supported and enable SSE.
	 */
	eax = 1;
	cpuid(&eax, &ebx, &ecx, &edx);
	if (edx & (1 << 25))
	{
		__asm__ __volatile
		(
			"movl %%cr0, %%eax\n"
			"andl $0xFFFFFFFB, %%eax\n" /* Clear CR0[EM] x87 Emulation */
			"movl %%eax, %%cr0\n"       /* Set CR0[MP] Co Processor Monitoring. */
			
			"movl %%cr4,  %%eax\n"
			"orl  $3<<9, %%eax\n"       /* Set bits 9,10: OSFXSR and OSXMMEXCPT. */
			"movl %%eax,  %%cr4\n"
			:
			:
			: "eax"
		);	
	}
}

/**
 * @brief Saves the current FPU/SIMD state for a given
 * process @p.
 */
PUBLIC void fpu_save(struct process *p)
{
	/* Temporarily save into our aligned buffer. */
	__asm__ __volatile__("fxsave %0" :: "m" (fpu_temp));
	
	/* Copy to the per-proc structure. */
	kmemcpy(&p->simd_state, &fpu_temp,
		sizeof(p->simd_state));	
}

/**
 * @brief Restores the latest FPU/SIMD state for a given
 * process @p.
 */
PUBLIC void fpu_restore(struct process *p)
{
	/* Copy to our temp aligned buffer. */
	kmemcpy(&fpu_temp, &p->simd_state,
		sizeof(p->simd_state));
	
	/* Restore state. */
	__asm__ __volatile__("fxrstor %0" :: "m" (fpu_temp));
}
