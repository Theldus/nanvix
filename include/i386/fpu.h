/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis <davidsondfgl@hotmail.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FPU_H_
#define FPU_H_
#ifndef _ASM_FILE_

	#include <nanvix/const.h>

	/* Forward definitions. */
	struct process;

	/**
	 * @brief FPU state.
	 */
	struct fpu
	{
		char dummy[512];
	} __attribute__((packed));

	EXTERN void fpu_init(void);
	EXTERN void fpu_save(struct process *);
	EXTERN void fpu_restore(struct process *);

#endif /* _ASM_FILE_ */
#endif /* FPU_H_ */
