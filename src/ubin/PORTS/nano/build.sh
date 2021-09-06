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

VERSION=2.7.2
num_cores=`grep -c ^processor /proc/cpuinfo`

## Required variables.
pushd .
export CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PREFIX="$CURDIR/binaries"

## Create folders
mkdir -p "$CURDIR"/src
mkdir -p "$CURDIR"/binaries/{home,bin}

## Download Nano source code and build

# Download
cd "$CURDIR"/src
echo "Downloading Nano v$VERSION source code..."
wget -q https://www.nano-editor.org/dist/v2.7/nano-$VERSION.tar.gz

# Extract
echo "Extracting..."
tar xf nano-*.tar.gz
mv nano-$VERSION*/* .
rm -rf nano-$VERSION*

# Patches
echo "Applying patches..."
patch -p1 < ../patch/nano.patch

# Build
echo "Building..."
CFLAGS="-DDISABLE_LINTER" ./configure \
	--host=i386-elf-nanvix \
	--enable-tiny   \
	--disable-nls   \
	--enable-color  \
	--enable-nanorc \
	--prefix="/"    \
	--enable-linenumbers

make -j$num_cores

# Move files to the right place
mv src/nano ../binaries/bin
cp ../patch/nanorc ../binaries/home/.nanorc

# Return
popd

# Success ;-)
echo -e "\nNano build with success..."
