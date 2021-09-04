#!/bin/bash

#
# Copyright(C) 2021 Davidson Francis <davidsondfgl@gmail.com>
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

# Required variables.
CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# We really do not need to do smart things here, just delete everything =)
rm -rf "$CURDIR"/binaries
rm -rf "$CURDIR"/src/nanvix/*.o

#
# Uninstall from the Nanvix GCC sysroot path too
#
GCC_PATH=$(i386-elf-nanvix-gcc -v |& grep -Po "sysroot=(.+)--disable-nls" | \
	cut -d'=' -f2 | sed "s/ --disable-nls//g")

rm "$GCC_PATH"/usr/lib/libpdcurses.a
rm "$GCC_PATH"/usr/include/{curses.h,curspriv.h,panel.h}

echo "PDCurses successfully deleted"
