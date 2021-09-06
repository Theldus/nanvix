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

#!/bin/bash

HASHGIT=5ffb6c5c03d0e9156db8f360599d4f0449bb16b9

## Required variables.
pushd .
export CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PREFIX="$CURDIR/binaries"

## Create folders
mkdir -p $CURDIR/src
mkdir -p $CURDIR/binaries/bin

## Download Nyancat source code and build

# Download
cd $CURDIR/src
echo "Downloading Nyancat source code..."
wget -q https://github.com/klange/nyancat/archive/"$HASHGIT".zip -O\
	nyancat.zip

# Extract
echo "Extracting..."
unzip -q nyancat.zip
mv nyancat*/* .
rm -rf nyancat-*
rm nyancat.zip

# Patches
echo "Applying patches..."
patch -p1 < ../patch/nyancat.patch

# Build
echo "Building..."
CC=i386-elf-nanvix-gcc make

# Move binary file to the right place
mv src/nyancat ../binaries/bin

# Return
popd

# Success ;-)
echo -e "\nNyancat build with success..."
