# 
# Copyright(C) 2020-2020 Davidson Francis <davidsondfgl@gmail.com>
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

# Paths
CURDIR="$( cd "$(dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PORTS_DIR=$CURDIR

# For each folder in the PORTS directory, invoke the clean.sh
for folder in $(ls -d $PORTS_DIR/*/)
do
	# Skip folders that do not contains the 'clean.sh' file inside
	if [ ! -f "$folder/build.sh" ]
	then
		echo "Attention: $folder do not contains the required clean.sh"
		echo "script in order to clean up, maybe this is an wrong port?"
		continue
	fi

	# Invoking the clean.sh script
	echo "Cleaning $(basename $folder) port..."
	echo "------------------------------------"
	/bin/bash $folder/clean.sh
	echo ""
done
