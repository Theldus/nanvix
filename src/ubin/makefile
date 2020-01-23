# 
# Copyright(C) 2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com> 
#              2015-2018 Davidson Francis <davidsondfgl@hotmail.com>
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
export CFLAGS = -Os -D_POSIX_C_SOURCE

# Conflicts.
.PHONY: cat
.PHONY: chgrp
.PHONY: chmod
.PHONY: chown
.PHONY: cp
.PHONY: date
.PHONY: echo
.PHONY: kill
.PHONY: ln
.PHONY: login
.PHONY: ls
.PHONY: mkdir
.PHONY: mv
.PHONY: nice
.PHONY: pwd
.PHONY: rm
.PHONY: rmdir
.PHONY: stat
.PHONY: sync
.PHONY: touch
.PHONY: tsh
.PHONY: ps
.PHONY: clear
.PHONY: nim
.PHONY: mount
.PHONY: unmount
.PHONY: mkfs

# Newlib considers some POSIX functions as not strict
export CFLAGS += -U__STRICT_ANSI__

# Builds everything.
all: cat chgrp chmod chown cp echo kill ln login ls mv nice pwd rm stat \
	sync tsh ps mount unmount mkfs clear

# Builds cat.
cat: 
	$(CC) $(CFLAGS) cat/*.c -o $(UBINDIR)/cat
	
# Builds chgrp.
chgrp: 
	$(CC) $(CFLAGS) chgrp/*.c -o $(UBINDIR)/chgrp
	
# Builds chmod.
chmod: 
	$(CC) $(CFLAGS) chmod/*.c -o $(UBINDIR)/chmod
	
# Builds chown.
chown: 
	$(CC) $(CFLAGS) chown/*.c -o $(UBINDIR)/chown
	
# Builds cp.
cp: 
	$(CC) $(CFLAGS) cp/*.c -o $(UBINDIR)/cp

# Builds date.
date: 
	$(CC) $(CFLAGS) date/*.c -o $(UBINDIR)/date

# Builds echo.
echo: 
	$(CC) $(CFLAGS) echo/*.c -o $(UBINDIR)/echo
	
# Builds kill.
kill: 
	$(CC) $(CFLAGS) kill/*.c -o $(UBINDIR)/kill
	
# Builds ln.
ln: 
	$(CC) $(CFLAGS) ln/*.c -o $(UBINDIR)/ln
	
# Builds login.
login: 
	$(CC) $(CFLAGS) login/*.c -o $(UBINDIR)/login
	
# Builds ls.
ls: 
	$(CC) $(CFLAGS) ls/*.c -o $(UBINDIR)/ls
	
# Builds mkdir.
mkdir: 
	$(CC) $(CFLAGS) mkdir/*.c -o $(UBINDIR)/mkdir
	
# Builds mv.
mv: 
	$(CC) $(CFLAGS) mv/*.c -o $(UBINDIR)/mv
	
# Builds nice.
nice: 
	$(CC) $(CFLAGS) nice/*.c -o $(UBINDIR)/nice
	
# Builds pwd.
pwd: 
	$(CC) $(CFLAGS) pwd/*.c -o $(UBINDIR)/pwd
	
# Builds rm.
rm: 
	$(CC) $(CFLAGS) rm/*.c -o $(UBINDIR)/rm
	
# Builds rmdir.
rmdir: 
	$(CC) $(CFLAGS) rmdir/*.c -o $(UBINDIR)/rmdir
	
# Builds stat.
stat: 
	$(CC) $(CFLAGS) stat/*.c -o $(UBINDIR)/stat
	
# Builds sync.
sync: 
	$(CC) $(CFLAGS) sync/*.c -o $(UBINDIR)/sync
	
# Builds touch.
touch: 
	$(CC) $(CFLAGS) touch/*.c -o $(UBINDIR)/touch
	
# Builds tsh.
tsh: 
	$(CC) $(CFLAGS) tsh/*.c -o $(UBINDIR)/tsh

# Builds ps.
ps: 
	$(CC) $(CFLAGS) ps/*.c -o $(UBINDIR)/ps

# Builds clear.
clear: 
	$(CC) $(CFLAGS) clear/*.c -o $(UBINDIR)/clear

# Builds nim.
nim: 
	$(CC) $(CFLAGS) nim/*.c -o $(UBINDIR)/nim

# Builds mount.
mount: 
	$(CC) $(CFLAGS) mount/*.c -o $(UBINDIR)/mount

# Builds unmount.
unmount: 
	$(CC) $(CFLAGS) unmount/*.c -o $(UBINDIR)/unmount

# Builds unmount.
mkfs: 
	$(CC) $(CFLAGS) mkfs/*.c -o $(UBINDIR)/mkfs

# Clean compilation files.
clean:
	@rm -f $(UBINDIR)/cat
	@rm -f $(UBINDIR)/chgrp
	@rm -f $(UBINDIR)/chmod
	@rm -f $(UBINDIR)/chown
	@rm -f $(UBINDIR)/cp
	@rm -f $(UBINDIR)/echo
	@rm -f $(UBINDIR)/kill
	@rm -f $(UBINDIR)/ln
	@rm -f $(UBINDIR)/login
	@rm -f $(UBINDIR)/ls
	@rm -f $(UBINDIR)/mkdir
	@rm -f $(UBINDIR)/mv
	@rm -f $(UBINDIR)/nice
	@rm -f $(UBINDIR)/pwd
	@rm -f $(UBINDIR)/rm
	@rm -f $(UBINDIR)/rmdir
	@rm -f $(UBINDIR)/stat
	@rm -f $(UBINDIR)/sync
	@rm -f $(UBINDIR)/touch
	@rm -f $(UBINDIR)/tsh
	@rm -f $(UBINDIR)/ps
	@rm -f $(UBINDIR)/clear
	@rm -f $(UBINDIR)/nim
	@rm -f $(UBINDIR)/mount
	@rm -f $(UBINDIR)/unmount
	@rm -f $(UBINDIR)/mkfs
