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

# Required variables.
pushd .
export CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export HOMEDIR=$(readlink -f $CURDIR/../../../../)
export TARGET=i386-elf-nanvix
unset PKG_CONFIG_LIBDIR

# TOOLCHAIN_PATH, if not set, lets us set the default value
if [ -z "$TOOLCHAIN_PATH" ]
then
	TOOLCHAIN_PATH="/usr/local/nanvix-toolchain"
fi

# TOOLCHAIN_SRC, if not set, lets us set the default value
if [ -z "$TOOLCHAIN_SRC" ]
then
	TOOLCHAIN_SRC=$HOMEDIR/tools/dev/toolchain/i386
fi

# Retrieve the number of processor cores
num_cores=`grep -c ^processor /proc/cpuinfo`

# Check if cross-compiler is in the path
if [ ! -x "$(command -v i386-elf-nanvix-gcc)" ]
then
	# Not in the path, let us check if in the default folder
	if [[ -f "$TOOLCHAIN_PATH/bin/i386-elf-nanvix-gcc" ]]
	then
		# Export to path
		export PATH=$PATH:"$TOOLCHAIN_PATH/bin/"
		export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$TOOLCHAIN_PATH/lib/"
	else
		echo "Could not find the cross-compiler in your system, please set the"
		echo "environment variable TOOLCHAIN_PATH to point to the appropriate"
		echo "folder, e.g:"
		echo "   export TOOLCHAIN_PATH=/path/to/the/toolchain"
		echo "and run this script again"
		exit -1
	fi
fi

# Check if the source is also available
if [ ! "$(ls -A $TOOLCHAIN_SRC)" ]
then
	echo "Toolchain source code could not be found in:"
	echo "   $TOOLCHAIN_SRC"
	echo ""
	echo "please set the environment variable TOOLCHAIN_SRC to point to the"
	echo "appropriate folder, e.g:"
	echo "   export TOOLCHAIN_SRC=/path/to/the/toolchain/src/"
	echo "and run this script again."
	echo ""
	echo "If you do not have the source code, you can invoke"
	echo "setup-toolchain-i386.sh in download-mode, e.g:"
	echo "   bash tools/dev/arch/setup-toolchain-i386.sh --download-only"
	echo ""
	echo "this will only download and patch the source code, after that"
	echo "run this script again"
	exit -1
fi

# Set our prefix
#
# Unlike the cross-compiler, self-hosted GCC will not be system-wide since
# the disk image script needs to know the path of ported programs in advance.
#
PREFIX="$CURDIR/binaries"
SYSROOT="$PREFIX"

# Create folders
mkdir -p $CURDIR/binaries
mkdir -p $CURDIR/build/{binutils,gcc,gmp,mpfr,mpc}
mkdir -p "$PREFIX/home"
mkdir -p "$PREFIX/usr/lib"
mkdir -p "$PREFIX/usr/include"

#### Build binutils. ####
cd $CURDIR/build/binutils
$TOOLCHAIN_SRC/src/binutils-2.27/configure \
	--host=$TARGET                  \
	--target=$TARGET                \
	--prefix="$PREFIX"              \
	--with-build-sysroot="$SYSROOT" \
	--with-sysroot=/                \
	--disable-nls                   \
	--disable-initfini-array        \
	--disable-werror                \

make -j$num_cores
make install

#### Build GCC deps. ####
cd $CURDIR/build/gmp
$TOOLCHAIN_SRC/src/gmp-4.3.2/configure \
	--host="$TARGET"   \
	--prefix="$PREFIX" \

make -j$num_cores
make install

cd $CURDIR/build/mpfr
$TOOLCHAIN_SRC/src/mpfr-2.4.2/configure \
	--host="$TARGET"     \
	--prefix="$PREFIX"   \
	--with-gmp="$PREFIX" \

make -j$num_cores
make install

cd $CURDIR/build/mpc
$TOOLCHAIN_SRC/src/mpc-0.8.1/configure \
	--host="$TARGET"      \
	--prefix="$PREFIX"    \
	--with-gmp="$PREFIX"  \
	--with-mpfr="$PREFIX" \

make -j$num_cores
make install

# libgcc needs the headers already in place, so lets put them
# where they're expected to be
cp -r $HOMEDIR/include/* $PREFIX/usr/include

#### Build GCC. ####
cd $CURDIR/build/gcc
$TOOLCHAIN_SRC/src/gcc-6.4.0/configure \
	--host=$TARGET                  \
	--target=$TARGET                \
	--prefix="$PREFIX"              \
	--with-build-sysroot="$SYSROOT" \
	--with-sysroot=/                \
	--with-gmp="$PREFIX"       \
	--with-mpfr="$PREFIX"      \
	--with-mpc="$PREFIX"       \
	--enable-languages=c       \
	--disable-libssp           \
	--disable-lto              \
	--disable-shared           \
	--disable-multilib         \
	--without-headers          \
	--without-isl              \
	--disable-nls              \
	--disable-hosted-libstdcxx \
	--disable-__cxa_atexit     \
	--disable-initfini-array   \
	--disable-werror           \

make all-gcc -j$num_cores
make all-target-libgcc -j$num_cores
make install-gcc
make install-target-libgcc

## Check if GCC and Binutils were build nicely.
if [[ ! -f "$PREFIX/bin/as" ]] || \
	[[ ! -f "$PREFIX/bin/gcc" ]]
then
	echo "Something went wrong with GCC/Binutils build, please re-check the steps"
	echo "above and try again"
	exit -1
fi

# Build Nanvix libraries and move them to the sysroot folder
echo "Building Nanvix libraries..."
pushd .
cd $HOMEDIR
TARGET=i386
make lib > /dev/null
cp    $HOMEDIR/lib/*.a $PREFIX/usr/lib
cp    $HOMEDIR/src/lib/libc/*.o $PREFIX/usr/lib
TARGET=i386-elf-nanvix
popd

# Remove 'fixed' headers, for some reason GCC still using them
rm $PREFIX/lib/gcc/i386-elf-nanvix/6.4.0/include-fixed/*

# Strip everything
echo "Stripping binaries..."
find $PREFIX -type f -executable -exec strip --strip-debug {} \; 2>/dev/null

# Remove some 'unnecessary'* things
#
# *The Minix filesystem is limited to up to 64MiB for files _and_ for the 
# filesystem, so, in order to save some space, I will purposely not include
# the whole Binutils/GCC into the Nanvix initrd.img.
#
# If you feel like some interesting tool are missing or something important is
# being deleted (if the latter case, let me know), please edit the following
# lines.
echo "Removing unnecessary things..."
pushd .
cd $PREFIX

# Some binaries
rm bin/addr2line bin/c++filt bin/elfedit bin/gcc-ar bin/gcc-nm bin/gcc-ranlib
rm bin/gcov* bin/gprof bin/i386-elf-nanvix* bin/nm bin/objcopy bin/ranlib
rm bin/size bin/strings bin/strip

# Not necessary
rm -rf i386-elf-nanvix/

# We will use our Nanvix Newlib's library, located at usr/include
rm -rf include/

# These libraries are also not necessary
rm lib/libbfd* lib/libgmp* lib/libmpc* lib/libmpfr* lib/libopcodes*

# We really need this?
rm -rf share/
popd

#### Check if succeed ####
#  If libc.a is present, the setup is 'likely' to be succeeded
if [[ -f "$PREFIX/usr/lib/libc.a" ]]
then
	# A little gift, ;-)
	echo "#include <stdio.h>"                         > "$PREFIX/home/hello.c"
	echo "int main(int argc, char **argv) {"         >> "$PREFIX/home/hello.c"
	echo "    printf(\"Hello, World $(whoami)\n\");" >> "$PREFIX/home/hello.c"
	echo "    return (0);"                           >> "$PREFIX/home/hello.c"
	echo "}"                                         >> "$PREFIX/home/hello.c"

	echo "Installation finished with success, you can now 'make image'"
	echo "in order to create your ISO image, ;-)."
else
	echo "Something went wrong with your setup, please re-check the steps"
	echo "above and try again"
fi

# Back to the current folder
popd
