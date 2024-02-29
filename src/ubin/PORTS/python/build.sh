#!/bin/bash

#
# Copyright(C) 2022 Davidson Francis <davidsondfgl@gmail.com>
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

VERSION=3.6.15
num_cores=`grep -c ^processor /proc/cpuinfo`

# Fancy colors =)
RED="\033[0;31m"
GREEN="\033[0;32m"
NC="\033[0m"

## Required variables.
pushd . >/dev/null
export CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export SRCDIR="$CURDIR/src"
export PREFIX="$CURDIR/binaries"

## Create folders
mkdir -p "$CURDIR"/src/prefix_host
mkdir -p "$CURDIR"/binaries/bin
mkdir -p "$CURDIR"/binaries/home

cd "$SRCDIR/"

# Info
step() {
	printf "${GREEN}[+]${NC} $1\n"
}
fail() {
	printf "${RED}[-]${NC} $1\n"
	printf "Aborting...\n"
	exit 1
}

## Download Python source code and build
extract () {
	step "Extracting source code..."
	tar xf Python-$VERSION.tar.xz
	step "Applying patches..."
	cd Python-$VERSION/
	patch -p1 < ../../patch/0001-PATCH-Python-Port.patch
	patch -p1 < ../../patch/0002-Replace-fgets-to-linenoise.patch
	cp ../../patch/linenoise/linenoise.c Parser/
	cp ../../patch/linenoise/linenoise.h Parser/
	cp ../../patch/linenoise/aqua.c Parser/
	cp ../../patch/linenoise/aqua.h Parser/
}

download() {
	if [ -f Python-$VERSION.tar.xz ]; then
		step "Python tarball already exists, skipping download..."
		extract
	else
		step "Downloading Python v$VERSION source code..."
		wget -q https://www.python.org/ftp/python/$VERSION/\
Python-$VERSION.tar.xz
	fi
}

check_python_host() {
	step "Checking if Python host works..."
	"$SRCDIR/prefix_host/bin/python3.6" -c "import sys; sys.exit(0)" &> .out_pyhost
	if [ "$?" -eq 0 ]; then
		step "  Yes, it works"
		rm .out_pyhost
	else
		fail "  Nope, does not work, please check .out_pyhost for more informations"
	fi
}

post_build_install() {
	if [ ! -f "python" ]; then
		fail "Failed to build Python..."
	fi

	# Check if already installed
	if [ -f "$PREFIX/bin/python3" ] && [ ! -f "$PREFIX/bin/python3.6m" ]; then
		cp ./python "$PREFIX/bin/python3"
		i386-elf-nanvix-strip --strip-unneeded "$PREFIX/bin/python3"
		return 0
	fi

	#
	# Otherwise, cleanup some stuff.
	#
	# The Minix filesystem is very small: up to 64MB (total).
	# So there is really a need to reduce as much as possible
	# space to have room for other things.
	#
	echo ""
	step "Minor adjustments..."

	cd "$PREFIX"
	rm -rf ./include # Some headers...
	rm -rf ./share   # Manpages too
	cd bin/
	find . -not -name "python3.6" -delete # Delete everything but python
	mv python3.6 python3
	i386-elf-nanvix-strip --strip-unneeded python3
	cd ../lib/
	rm -f ./libpython3.6m.a
	rm -rf ./pkgconfig
	cd python3.6/
	# Remove all cache
	find . -type d -name __pycache__ -prune -exec rm -rf {} \;
	# Remove big and not useful (at the moment) folders
	rm -rf distutils/ test/ config-3.6m/ ensurepip/ idlelib/ lib2to3/
	rm -rf tkinter/ unittest/ asyncio/ email/ sqlite3/

	# A little gift
	echo "print(\"Hello, World $(whoami)!\\n\")" > "$PREFIX/home/hello.py"
}

echo "(This may take a few minutes, please be patient)"

# Download
step "Checking if src already exists..."
if [ -d "Python-$VERSION" ]; then
	step "  source already exists, skipping download..."
else
	download
fi

step "Checking if host Python is present..."

if [ -f "$SRCDIR/prefix_host/bin/python3.6" ]; then

	step "  Yes, exists"
	check_python_host

# Not present, we should built Python host
else
	step "Building Python host... this should take a while to complete..."
	cd "$SRCDIR/Python-$VERSION/"
	./configure --prefix="$SRCDIR/prefix_host"
	make -j$num_cores
	make install
	check_python_host
fi

echo ""
step "Build completed with success, now building Nanvix's Python"
echo "(some errors might occur due to Python's attempts to build"
echo " incompatible modules, do not worry =) )"
echo ""

export PATH="$PATH:$SRCDIR/prefix_host/bin"

cd "$SRCDIR/Python-$VERSION/"

make distclean &> /dev/null

./configure \
	LDFLAGS="-static"  \
	CPPFLAGS="-static" \
	--prefix=/         \
	--disable-shared   \
	--host=i386-elf-nanvix      \
	--target=i386-elf-nanvix    \
	--build=x86_64-pc-linux-gnu \
	--with-threads=no \
	ac_cv_file__dev_ptmx=no \
	ac_cv_file__dev_ptc=no  \
	ac_cv_func_unsetenv=no  \
	ac_cv_var_putenv=no     \
	ac_cv_var_tzname=no

#
# Check if was previously installed, if so, we do not need
# to fully build&install again, just copy the updated binary
# after build
#
if [ -f "$PREFIX/bin/python3" ]; then
	make -j$num_cores

	post_build_install
else
	make \
		DESTDIR="$PREFIX" \
		commoninstall     \
		bininstall        \
		-j$num_cores

	post_build_install
fi

# Success ;-)
step "Python build with success..."

popd &> /dev/null
