/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2016-2016 Davidson Francis <davidsonfrancis@gmail.com>
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "minix.h"
#include "util.h"

/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("usage: gcp.minix <input file> <source file>\n");
	exit(EXIT_SUCCESS);
}

/**
 * @brief Copies a file to a Minix file system.
 */
int main(int argc, char **argv)
{
	int fd;                            /* File ID of source file.  */
	char filename[MINIX_NAME_MAX + 1]; /* Name of the file.        */
	char *buf;                         /* Buffer used for copying. */
	uint16_t num;                      /* Working inode number.    */
	struct d_inode *ip;                /* Working inode pointer.   */
	size_t len;                        /* Bytes read.              */
	
	/* Wrong usage. */
	if (argc != 3)
		usage();

	minix_mount(argv[1]);
	num = minix_inode_dname(argv[2],filename);
	ip  = minix_inode_read(num); 
	fd  = sopen(filename, O_RDWR | O_CREAT);

	buf = smalloc(ip->i_size);

	/* Read file. */
	len = minix_read(num, buf, ip->i_size);
	swrite(fd, buf, len);
	
	minix_umount();
	sclose(fd);
	free(buf);
	
	return (EXIT_SUCCESS);
}
