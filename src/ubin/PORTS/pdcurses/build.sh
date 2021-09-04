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

set -e

num_cores=$(grep -c ^processor /proc/cpuinfo)

## Required variables.
pushd .
export CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PREFIX="$CURDIR/binaries"

## Create folders
mkdir -p "$CURDIR"/binaries/{bin,usr/include,lib}

# Build
echo "Building..."
cd "$CURDIR"/src/nanvix
make -j"$num_cores"
make demos -j"$num_cores"

# Copy binaries files to the right place
mv libpdcurses.a "$PREFIX"/lib
mv firework ozdemo ptest rain testcurs tuidemo worm xmas xmas_nvx \
	"$PREFIX"/bin

cp ../curses.h ../curspriv.h ../panel.h "$PREFIX"/usr/include

#
# Since it is a library, we should also attempt to copy it to the Nanvix's
# host compiler sysroot path
#
GCC_PATH=$(i386-elf-nanvix-gcc -v |& grep -Po "sysroot=(.+)--disable-nls" | \
	cut -d'=' -f2 | sed "s/ --disable-nls//g")

cp "$PREFIX"/lib/libpdcurses.a "$GCC_PATH"/usr/lib
cp "$PREFIX"/usr/include/curses.h "$GCC_PATH"/usr/include
cp "$PREFIX"/usr/include/curspriv.h "$GCC_PATH"/usr/include
cp "$PREFIX"/usr/include/panel.h "$GCC_PATH"/usr/include

# Return
popd

# Success ;-)
echo -e "\nPDCurses built with success..."
