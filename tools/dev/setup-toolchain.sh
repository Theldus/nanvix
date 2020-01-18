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

# Get required packages.
DIST=$(uname -rv)

case ${DIST,,} in
    *"ubuntu"*|*"debian"*)
        apt-get install g++ ddd genisoimage texinfo flex bison libncurses5-dev \
			dos2unix make -y
        ;;
    *"arch"*)
        pacman -S gcc cdrtools texinfo flex bison ncurses --needed
        ;;
    *)
        echo "Warning: your distro is not officially supported or tested by us"
esac
