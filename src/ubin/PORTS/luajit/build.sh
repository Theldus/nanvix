# 
# Copyright(C)  2020-2020 Davidson Francis <davidsondfgl@gmail.com>
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

#!/bin/bash

HASHGIT=99d1268823914d0c1f46df0e85bf8fce50292252
num_cores=`grep -c ^processor /proc/cpuinfo`

## Required variables.
pushd .
export CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PREFIX="$CURDIR/binaries"

## Create folders
mkdir -p $CURDIR/src
mkdir -p $CURDIR/binaries/{home,bin}

## Download LuaJIT source code and build

# Download
cd $CURDIR/src
echo "Downloading LuaJIT source code..."
wget -q https://github.com/LuaJIT/LuaJIT/archive/"$HASHGIT".zip -O\
	luajit.zip

# Extract
echo "Extracting..."
unzip -q luajit.zip
mv LuaJIT*/* .
rm -rf LuaJIT-*
rm luajit.zip

# Patches
echo "Applying patches..."
patch -p1 < ../patch/luajit.patch

# Build
echo "Building..."
make \
	CC="i386-elf-nanvix-gcc" \
	STATIC_CC="i386-elf-nanvix-gcc" \
	HOST_CC="gcc -m32" \
	CFLAGS="-DLUAJIT_USE_SYSMALLOC -DLUAJIT_DISABLE_PROFILE" \
	LDFLAGS="--static" -j$num_cores

# Move binary file to the right place
mv src/luajit ../binaries/bin

# A little gift, ;-)
echo "print(\"Hello, World $(whoami)\")" > "$PREFIX/home/hello.lua"

# Return
popd

# Success ;-)
echo -e "\nLuaJIT build with success..."
