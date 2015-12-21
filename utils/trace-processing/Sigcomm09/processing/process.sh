#!/bin/bash

#  Adyton: A Network Simulator for Opportunistic Networks
#  Copyright (C) 2015  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
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
#  Written by Nikolaos Papanikos.


#---------------
# How it works |
#---------------
# Step 1: Read the original trace and create the scan file for each device that participates in the 
# experiment (ID <= 76). For each device a file will be created inside the folder ./tmp/scans. The
# file format is "(scanning)time device(that scans) device(scanned)".
#
# Step 2: Read the scan file for each device and create a discovery file inside the folder ./tmp/discoveries.
# A disovery file will be created for each device. The contents of the discovery file are the devices that are 
# scanned during the scanning procedure of the main device. 
# The file format is "(scanning)time dev1 dev2 ... devN". Note that N is different from each time instance.
#
# Step 3: Read the discovery file of each device and create a contact file inside folder ./tmp/contacts.
# For each device ID that exists in each line find the line (below that line) that the ID does not exist. 
# The start line matches to the start contact time and the end line matches to the end contact time. Using this
# method, the duration between contacts is at least equal to one scanning interval (~120 secs). 
#
# Step 4: The contact file for every device is read line by line. If a line occurs multiple times, then all 
# instances are removed except one. The results for each device ID are stored inside folder ./tmp/clean-contacts.
#
# Step 5: All device contacts are aggregated to one file. 
# 
# Step 6: Two lists are created containing device IDs. The first list contains inactive device IDs, that do not
# have any contact entry in the contact file. The second list consists of active devices that participate at 
# least to one contact event. 
#
# Step 7: The contact file is sorted based on the contact start time (3rd column).
#
# Step 8: Normalize the event times in order to get the starting event (first contact of the trace file) at time 0.
#
# Step 9: Get the time that each device appears in the trace. That is needed because every device participates in the
# experiment at different time instances. It can be used by the simulator at the traffic generation phase in order to 
# avoid genearating packets destined to nodes that have not joined the experiment (at a given time).
#
# Step 10: Write the summary file that contains the lowest and the highest event time in the trace file.
#
#Notes: Nodes with ID over 100 do not participate in the experiment
#       First Node has ID 1
#	A contact has a duration that is larger or equal to one scanning interval (~120 secs)

RED='\033[0;31m';
LRED='\033[1;31m';
GREEN='\033[0;32m';
LGREEN='\033[1;32m';
BLUE='\033[0;34m';
LBLUE='\033[1;34m';
ORANGE='\033[0;33m';
NC='\033[0m'; # No Color

NN=76;
input=../original/sigcomm2009/proximity.csv;
final=./Sigcomm09.txt;
temp=./tmp.txt;
active=./active-nodes.txt;
inactive=./inactive-nodes.txt;
arrival=./Sigcomm09-presence.txt;
summary=./summary.txt;
rm -rf ${final};
rm -rf ${temp};
rm -rf ${active};
rm -rf ${inactive};
rm -rf ${arrival};
rm -rf ${summary};
rm -rf tmp;

#Step 1 - Split
echo -n " Step 1: Split.. "
#split scanning for each device seperately
mkdir -p ./tmp/scans;
echo -e -n "00%";
for j in `seq 1 ${NN}`;
do
	awk -v dev=${j} -v N=${NN} -F ";" '{if($2 == dev && $2 <= N && $3 <= N){printf("%d\t%d\t%d\n",$1,$2,$3);}}' ${input} > ./tmp/scans/scan-${j}.txt;
	status=$(echo "scale=2; (${j}/${NN})*100" | bc);
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done
echo -e "\b\b\b\b${GREEN}[OK]${NC}";

#Step 2 - Group by discovery time
echo -n " Step 2: Group by discovery time.. "
#get discovered nodes at each time instance
mkdir -p ./tmp/discoveries;
echo -e -n "00%";
for j in `seq 1 ${NN}`;
do
	cp ./tmp/scans/scan-${j}.txt active-file.txt ;
	while [ -s "active-file.txt" ];
	do
		awk 'BEGIN{flag=0;time=0;}{if(flag == 1){if($1 == time){printf("%d\t",$3)}}else{flag=1;time=$1;printf("%d\t%d\t",time,$3);}}END{printf("\n");}' ./active-file.txt >> ./tmp/discoveries/disc-${j}.txt;
		awk 'BEGIN{flag=0;time=0;}{if(flag == 1){if($1 != time){printf("%d\t%d\t%d\n",$1,$2,$3)}}else{flag=1;time=$1;}}' active-file.txt > ${temp};
		mv ${temp} active-file.txt;
	done
	status=$(echo "scale=2; (${j}/${NN})*100" | bc);
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done
echo -e "\b\b\b\b${GREEN}[OK]${NC}";

#Step 3 - Finding End Time of each node discovered
echo -n " Step 3: Maching time pairs.. "
mkdir -p ./tmp/contacts;
echo -e -n "00%";
for j in `seq 1 ${NN}`;
do
	cp ./tmp/discoveries/disc-${j}.txt active-file.txt ;
	while [ -s "active-file.txt" ];
	do
		awk -v NID=${j} -f ./match.awk ./active-file.txt >> ./tmp/contacts/contacts-${j}.txt;
		awk 'BEGIN{flag=0;}{if(flag == 1){for(i=1;i<=NF;i++){printf("%d\t",$i);}printf("\n");}else{flag=1;}}' active-file.txt > ${temp};
		mv ${temp} active-file.txt;
	done
	status=$(echo "scale=2; (${j}/${NN})*100" | bc);
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done
echo -e "\b\b\b\b${GREEN}[OK]${NC}";

#Step 4 - Cleaning duplicate contact entries
echo -n " Step 4: Cleaning duplicates.. "
mkdir -p ./tmp/clean-contacts;
echo -e -n "00%";
for j in `seq 1 ${NN}`;
do
	cp ./tmp/contacts/contacts-${j}.txt active-file.txt ;
	while [ -s "active-file.txt" ];
	do
		awk 'BEGIN{flag=0;}{if(flag==0){flag=1;printf("%d\t%d\t%d\t%d\n",$1,$2,$3,$4);}}' ./active-file.txt >> ./tmp/clean-contacts/contacts-${j}.txt;
		awk -f ./remove-dups.awk ./active-file.txt > ${temp};
		mv ${temp} active-file.txt;
	done
	status=$(echo "scale=2; (${j}/${NN})*100" | bc);
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done
rm -rf active-file.txt;
echo -e "\b\b\b\b${GREEN}[OK]${NC}";

#Step 5 - Gathering Results
echo -n " Step 5: Gathering Results.. "
echo -e -n "00%";
for j in `seq 1 ${NN}`;
do
	cat ./tmp/clean-contacts/contacts-${j}.txt >> ${final}
	status=$(echo "scale=2; (${j}/${NN})*100" | bc);
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done
echo -e "\b\b\b\b${GREEN}[OK]${NC}";

#Step 6 - search for inactive devices
echo -n " Step 6: Searching for inactive nodes.. ";
for i in `seq 1 ${NN}`;
do
	awk -v dev=${i} 'BEGIN{found=0;}{if(!found && ($1 == dev || $2 == dev)){found=1;printf("Device %d found at line %d\n",dev,NR);}}' ${final} >> ${active};
	awk -v dev=${i} 'BEGIN{found=0;}{if(!found && ($1 == dev || $2 == dev)){found=1;}}END{if(!found){printf("%d\n",dev);}}' ${final} >> ${inactive};
done
echo -e "${GREEN}[OK]${NC}";

#Step 7 - sort contacts according to start time (3rd column)
echo -n " Step 7: Sorting contacts.. "
sort -t$'\t' -n -k3,3 -k 4,4 ${final} > ${temp};
mv ${temp} ${final};
echo -e "${GREEN}[OK]${NC}";

#Step 8 - normalize the contact time values (substract least time from all contact times)
echo -n " Step 8: Normalizing contact time.. "
awk 'NR==1{least=$3;printf("%d\t%d\t%d\t%d\n",$1,$2,$3-$3,$4-$3);}NR!=1{printf("%d\t%d\t%d\t%d\n",$1,$2,$3-least,$4-least);}' ${final} > ${temp};
mv ${temp} ${final};
echo -e "${GREEN}[OK]${NC}";

#Step 9 - create the "first presence in the network" list
echo -n " Step 9: Calculating node arrival time in the experiment.. "
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
echo -n " Step 10: Writing summary.. "
min=`awk 'NR==1{min=$3;}NR!=1{if($3<min){min=$3;}}END{printf("%d",min);}' ${final}`;
max=`awk 'NR==1{max=$4;}NR!=1{if($4>max){max=$4;}}END{printf("%d",max);}' ${final}`;
echo "- First contact starts at ${min} sec" >> ${summary};
echo "- Last contact ends at ${max} sec" >> ${summary};
echo -e "${GREEN}[OK]${NC}";
