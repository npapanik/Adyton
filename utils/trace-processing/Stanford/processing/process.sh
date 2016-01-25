#!/bin/bash

#  Adyton: A Network Simulator for Opportunistic Networks
#  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
#  and Evangelos Papapetrou
#
#  This file is part of Adyton.
#
#  Adyton is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Adyton is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Adyton.  If not, see <http://www.gnu.org/licenses/>.
#
#  Written by Dimitrios-Georgios Akestoridis.

RED='\033[0;31m';
LRED='\033[1;31m';
GREEN='\033[0;32m';
LGREEN='\033[1;32m';
BLUE='\033[0;34m';
LBLUE='\033[1;34m';
ORANGE='\033[0;33m';
NC='\033[0m'; # No Color

function spinner {
	local pid=$1;
	local delay=0.175;
	local spinstr='|/-\';
	local infotext=$2;
	while [ "$(ps a | awk '{print $1}' | grep $pid)" ]; do
		local temp=${spinstr#?};
		printf " [%c] %s" "$spinstr" "$infotext";
		local spinstr=$temp${spinstr%"$temp"};
		sleep $delay;
		printf "\b\b\b\b\b\b";
		for i in $(seq 1 ${#infotext}); do
			printf "\b";
		done
	done
	printf " \b\b\b\b";
	printf " [-] %s" "$infotext";
}

cd process1
g++ -Wall connect.cc
./a.out > "/tmp/log_stanford.txt" &
spinner $! "Step 1 of 3.. ";
echo -e "${GREEN}[OK]${NC}"
rm a.out

cd ../process2
chmod +x sort.sh
./sort.sh >> "/tmp/log_stanford.txt" &
spinner $! "Step 2 of 3.. ";
echo -e "${GREEN}[OK]${NC}"

cd ../results
g++ -Wall generateSimulationFiles.cc
./a.out Stanford.txt Stanford >> "/tmp/log_stanford.txt" &
spinner $! "Step 3 of 3.. ";
echo -e "${GREEN}[OK]${NC}"
cat "/tmp/log_stanford.txt"
rm a.out

cd ..
mv ./results/Stanford.txt .;
mv ./results/Stanford-presence.txt .;

