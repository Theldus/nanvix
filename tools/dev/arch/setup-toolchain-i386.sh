# 
# Copyright(C)  2011-2018 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
#               2016-2018 Davidson Francis <davidsondfgl@gmail.com>
#               2016-2017 Subhra Sarkar    <rurtle.coder@gmail.com>
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
export CURDIR=`pwd`
export WORKDIR=$CURDIR/tools/dev/toolchain/i386
export TARGET=i386-elf-nanvix
unset LD_LIBRARY_PATH

# Retrieve the number of processor cores
num_cores=`grep -c ^processor /proc/cpuinfo`

# Get binutils, GDB and GCC.
if [ ! "$(ls -A $WORKDIR)" ]; then
	mkdir    $WORKDIR
	mkdir    $WORKDIR/src
	mkdir    $WORKDIR/build
	mkdir    $WORKDIR/download
	mkdir -p $WORKDIR/build/{binutils,gcc,gmp,mpfr,mpc}

	# Download binutils 2.27
	wget https://gcc.gnu.org/pub/binutils/releases/binutils-2.27.tar.bz2 -P \
		$WORKDIR/download

	# Download GCC 6.4.0
	wget https://gcc.gnu.org/pub/gcc/releases/gcc-6.4.0/gcc-6.4.0.tar.xz -P \
		$WORKDIR/download

	# Download GCC dependencies:
	wget https://gcc.gnu.org/pub/gcc/infrastructure/gmp-4.3.2.tar.bz2 -P \
		$WORKDIR/download
	wget https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-2.4.2.tar.bz2 -P \
		$WORKDIR/download
	wget https://gcc.gnu.org/pub/gcc/infrastructure/mpc-0.8.1.tar.gz -P \
		$WORKDIR/download

	cd $WORKDIR/src		

	## Extract downloaded files ##
	echo "Extracting binutils..."
	tar xf ../download/binutils*.tar.bz2

	echo "Extracting GCC..."
	tar xf ../download/gcc*.tar.xz

	echo "Extracting GCC deps..."
	tar xf ../download/gmp*.tar.bz2
	tar xf ../download/mpfr*.tar.bz2
	tar xf ../download/mpc*.tar.gz

	### Patches ###
	# target-*: changes in order to recognize i386-elf-nanvix as a valid target
	# nanvix-*: misc changes in order to build correctly in the Nanvix
	#           environment.

	echo "Applying binutils patches..."
	cd binutils-*
	dos2unix binutils/bfdtest2.c
	patch -p1 < ../../../../arch/patches/i386/binutils/target-binutils.patch
	patch -p1 < ../../../../arch/patches/i386/binutils/nanvix-binutils.patch

	echo "Applying GCC patches..."
	cd ../gcc-*
	patch -p1 < ../../../../arch/patches/i386/gcc/target-gcc.patch
	patch -p1 < ../../../../arch/patches/i386/gcc/nanvix-gcc.patch

	echo "Applying GCC deps patches..."
	cd ../gmp-*
	patch -p1 < ../../../../arch/patches/i386/gmp/target-gmp.patch
	patch -p1 < ../../../../arch/patches/i386/gmp/nanvix-gmp.patch

	cd ../mpfr-*
	patch -p1 < ../../../../arch/patches/i386/mpfr/target-mpfr.patch

	cd ../mpc-*
	patch -p1 < ../../../../arch/patches/i386/mpc/target-mpc.patch
fi

# If download-only mode, we do not proceed
#
# This is expected to happen when the user already have the cross-compiler
# properly installed but deleted the source code and now wants to build
# the Binutils/GCC Self-Hosted. In this case, download and patch the source
# code is enough ;-).
#
if [ "$1" = "--download-only" ];
then
	cd $CURDIR
	exit 0
fi

# Check if PREFIX was already set, if not, let us set our default path
if [ -z "$PREFIX" ]
then
	PREFIX="/usr/local/"
fi

PREFIX="$PREFIX/nanvix-toolchain/"
SYSROOT="$PREFIX"

sudo mkdir -p "$PREFIX"
sudo mkdir -p "$PREFIX/usr/lib"
sudo mkdir -p "$PREFIX/usr/include"

#### Build binutils. ####
cd $WORKDIR/build/binutils
../../src/binutils-2.27/configure \
	--target=$TARGET          \
	--prefix="$PREFIX"        \
	--with-sysroot="$SYSROOT" \
	--disable-nls             \
	--disable-initfini-array  \
	--disable-werror          \

make -j$num_cores
sudo make install

#### Build GCC deps. ####
cd $WORKDIR/build/gmp
../../src/gmp-4.3.2/configure --prefix="$PREFIX"
make -j$num_cores
sudo make install

cd $WORKDIR/build/mpfr
../../src/mpfr-2.4.2/configure --prefix="$PREFIX" --with-gmp="$PREFIX"
make -j$num_cores
sudo make install

cd $WORKDIR/build/mpc
../../src/mpc-0.8.1/configure \
	--prefix="$PREFIX"    \
	--with-gmp="$PREFIX"  \
	--with-mpfr="$PREFIX" \

make -j$num_cores
sudo make install

# Library path for libgmp, libmpc and libmpfr
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"$PREFIX/lib"

#### Build GCC. ####
cd $WORKDIR/build/gcc
../../src/gcc-6.4.0/configure \
	--target=$TARGET          \
	--prefix="$PREFIX"        \
	--with-sysroot="$SYSROOT" \
	--with-gmp="$PREFIX"      \
	--with-mpfr="$PREFIX"     \
	--with-mpc="$PREFIX"      \
	--disable-nls             \
	--enable-languages=c,c++  \
	--disable-libssp          \
	--with-newlib             \
	--without-headers         \
	--disable-__cxa_atexit    \
	--disable-initfini-array  \
	--disable-werror          \

make all-gcc -j$num_cores
make all-target-libgcc -j$num_cores
sudo make install-gcc
sudo make install-target-libgcc

### Build Nanvix Newlib's first and then, copy the library into sysroot
#   before proceeding building libstdc++.
export PATH=$PATH:"$PREFIX/bin"

## Check if GCC and Binutils were build first
if [[ ! -f "$PREFIX/bin/i386-elf-nanvix-as" ]] || \
	[[ ! -f "$PREFIX/bin/i386-elf-nanvix-gcc" ]] || \
	[[ ! -f "$PREFIX/bin/i386-elf-nanvix-g++" ]]
then
	echo "Something went wrong with GCC/Bintuils build, please re-check the steps"
	echo "above and try again"
	exit -1
fi

# Build Nanvix libraries and move them to the sysroot folder
pushd .
cd $CURDIR
TARGET=i386
make lib > /dev/null
sudo cp    $CURDIR/lib/*.a $PREFIX/usr/lib
sudo cp    $CURDIR/src/lib/libc/*.o $PREFIX/usr/lib
sudo cp -r $CURDIR/include/* $PREFIX/usr/include
TARGET=i386-elf-nanvix
popd

# Build libstdc++
make all-target-libstdc++-v3 -j$num_cores
sudo make install-target-libstdc++-v3

# Remove 'fixed' headers, for some reason GCC still using them
sudo rm $PREFIX/lib/gcc/i386-elf-nanvix/6.4.0/include-fixed/*

#### Check if succeed ####
#  If libstdc++ is present, the setup is 'likely' to be succeeded
if [[ -f "$PREFIX/i386-elf-nanvix/lib/libstdc++.a" ]]
then
	echo "Installation finished with success, please add $PREFIX to your path"
	echo "or paste the following lines in your ~/.bashrc:"
	echo "     export TARGET=i386"
	echo "     export PATH=\$PATH:$PREFIX/bin"
	echo "     export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$PREFIX/lib"
	echo ""
	echo "Its also recommended that the folder $WORKDIR not be deleted, as it"
	echo "may be required for libstdc++ rebuild or even for the self-hosted"
	echo "toolchain build."
else
	echo "Something went wrong with your setup, please re-check the steps"
	echo "above and try again"
fi

# Back to the current folder
cd $CURDIR
