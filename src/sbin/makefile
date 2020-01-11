# 
# Copyright(C) 2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com> 
#              2017-2017 Romane Gallier   <romanegallier@gmail.com>
#              2018-2018 Davidson Francis <davidsondfgl@gmail.com>
#
# This file is part of Nanvix.
#
# Nanvix is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Nanvix is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Nanvix.  If not, see <http://www.gnu.org/licenses/>.
#

# Toolchain configuration.
export CC     = $(TARGET)-elf-nanvix-gcc
export CFLAGS = -Os -D_POSIX_C_SOURCE -D_POSIX_THREADS -U__STRICT_ANSI__

# Resolves conflicts
.PHONY: foobar
.PHONY: init
.PHONY: shutdown
.PHONY: test

# Builds everything.
all: init shutdown test

# Builds foobar.
foobar:
	$(CC) $(CFLAGS) foobar/*.c -o $(SBINDIR)/foobar

# Builds init.
init:
	$(CC) $(CFLAGS) init/*.c -o $(SBINDIR)/init

# Builds shutdown.
shutdown:
	$(CC) $(CFLAGS) shutdown/*.c -o $(SBINDIR)/shutdown

# Builds test.
test:
	$(CC) $(CFLAGS) test/*.c -o $(SBINDIR)/test
	
# Cleans compilations files.
clean:
	@rm -f $(SBINDIR)/foobar
	@rm -f $(SBINDIR)/init
	@rm -f $(SBINDIR)/shutdown
	@rm -f $(SBINDIR)/test
