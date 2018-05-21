#!/bin/bash

#  Adyton: A Network Simulator for Opportunistic Networks
#  Copyright (C) 2015-2018  Nikolaos Papanikos,
#  Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou
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
#  Written by Nikolaos Papanikos.


#---------------
# How it works |
#---------------
# Step 1: Clean the dataset. Remove all external noise that is the set of devices with ID larger than 62.
# 
# Step 2: Two lists are created containing device IDs. The first list contains inactive device IDs, that do not
# have any contact entry in the contact file. The second list consists of active devices that participate at 
# least to one contact event. 
#
# Step 3: The contact file is sorted based on the contact start time (3rd column).
#
# Step 4: Normalize the event times in order to get the starting event (first contact of the trace file) at time 0.
#
# Step 5: Remove contacts that occur between nodes with same id.
#
# Step 6: The contact file read line by line. If a line occurs multiple times, then all instances are removed except one.
#
# Step 7: Get the time that each device appears in the trace. That is needed because every device participates in the
# experiment at different time instances. It can be used by the simulator at the traffic generation phase in order to 
# avoid genearating packets destined to nodes that have not joined the experiment (at a given time).
#
# Step 8: Write the summary file that contains the lowest and the highest event time in the trace file.

export LC_NUMERIC="en_US.UTF-8";

RED='\033[0;31m';
LRED='\033[1;31m';
GREEN='\033[0;32m';
LGREEN='\033[1;32m';
BLUE='\033[0;34m';
LBLUE='\033[1;34m';
ORANGE='\033[0;33m';
NC='\033[0m'; # No Color

NN=62;
final=./Rollernet.txt;
input=../original/imote-traces-RollerNet/contacts.dat;
summary=./summary.txt;
arrival=./Rollernet-presence.txt;
temp=./tmp.txt;
active=./active-nodes.txt;
inactive=./inactive-nodes.txt;
dups=./Rollernet-before-dups-removed.txt
rm -rf ${final};
rm -rf ${summary};
rm -rf ${arrival};
rm -rf ${temp};
rm -rf ${active};
rm -rf ${inactive};
rm -rf ${dups};

#Remove nodes that do not participate in the experiment
#Remove extra comlumns that are not needed
echo -n " Step 1: Removing external nodes.. ";
awk -v N=${NN} '{if($1 <= N && $2 <= N ){printf("%d\t%d\t%d\t%d\n",$1,$2,$3,$4);}}' ${input} > ${final};
echo -e "${GREEN}[OK]${NC}";

#search for inactive devices
echo -n " Step 2: Searching for inactive nodes.. ";
for i in `seq 0 ${NN}`;
do
	awk -v dev=${i} 'BEGIN{found=0;}{if(!found && ($1 == dev || $2 == dev)){found=1;printf("Device %d found at line %d\n",dev,NR);}}' ${final} >> ${active};
	awk -v dev=${i} 'BEGIN{found=0;}{if(!found && ($1 == dev || $2 == dev)){found=1;}}END{if(!found){printf("%d\n",dev);}}' ${final} >> ${inactive};
done
echo -e "${GREEN}[OK]${NC}";

#Sort file
echo -n " Step 3: Sorting file.. ";
sort -t$'\t' -n -k3,3 -k 4,4 ${final} > ${temp};
mv ${temp} ${final};
echo -e "${GREEN}[OK]${NC}";

#normalize the contact time values (substract least time from all contact times)
echo -n " Step 4: Normalizing contact time.. "
awk 'NR==1{least=$3;printf("%d\t%d\t%d\t%d\n",$1,$2,$3-$3,$4-$3);}NR!=1{printf("%d\t%d\t%d\t%d\n",$1,$2,$3-least,$4-least);}' ${final} > ${temp};
mv ${temp} ${final};
echo -e "${GREEN}[OK]${NC}";

#Remove the cases where one node comes in contact with itself
echo -n " Step 5: Searching for self-contacts.. "
cp ${final} ${dups};
cat ${dups} | awk '{if($1!=$2){print $0;}}' > ${final};
echo -e "${GREEN}[OK]${NC}";

#Remove duplicates (contacts that overlap in time - both start/end time is equal) - Keep just one instance
echo -n " Step 6: Searching for contact duplicates.. "
awk '!a[$0]++' ${final} > ${temp};
cp ${temp} ${final};
echo -e "${GREEN}[OK]${NC}";

#create the "first presence in the network" list
echo -n " Step 7: Calculating node arrival time in the experiment.. "
echo -e -n "00%";
for j in `seq 0 ${NN}`;
do
	echo -n -e "${j}\t" >> ${arrival};
	awk -v N=${j} 'BEGIN{mintime=100000000;maxtime=0;}{if($1 == N || $2 == N){if($3 < mintime){mintime=$3;}if($4 > maxtime){maxtime=$4;}}}END{if(mintime!=100000000){printf("%d\t%d\n",mintime,maxtime);}else{printf("-\t-\n");}}' ${final} >> ${arrival};
	status=$(echo "scale=2; (${j}/${NN})*100" | bc);	
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done
echo -e "\b\b\b\b${GREEN}[OK]${NC}";

#Search for lowest/highest event time
echo -n " Step 8: Writing summary.. "
min=`awk 'NR==1{min=$3;}NR!=1{if($3<min){min=$3;}}END{printf("%d",min);}' ${final}`;
max=`awk 'NR==1{max=$4;}NR!=1{if($4>max){max=$4;}}END{printf("%d",max);}' ${final}`;
echo "- First contact starts at ${min} sec" >> ${summary};
echo "- Last contact ends at ${max} sec" >> ${summary};
echo -e "${GREEN}[OK]${NC}";


