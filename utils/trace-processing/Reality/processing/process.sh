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
# Step 1: Clean the dataset. Remove all external noise that is the set of devices with ID larger than 97. Also, remove all
# time events that are out of the experiment duration. The time limits of the experiment are from 2004-07-26 00:00:00
# to 2005-05-05  23:59:59.
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
# Step 6: Read the contact file  line by line. If a line occurs multiple times, then all instances are removed except one.
#
# Step 7: Get the time that each device appears in the trace. That is needed because every device participates in the
# experiment at different time instances. It can be used by the simulator at the traffic generation phase in order to 
# avoid generating packets destined to nodes that have not joined the experiment (at a given time). 
#
#Notes: First Node has ID 1

RED='\033[0;31m';
LRED='\033[1;31m';
GREEN='\033[0;32m';
LGREEN='\033[1;32m';
BLUE='\033[0;34m';
LBLUE='\033[1;34m';
ORANGE='\033[0;33m';
NC='\033[0m'; # No Color

NN=97;
bug=./trace-anomalies.txt;
final=./Reality.txt;
input=../original/reality.txt;
summary=./summary.txt;
arrival=./Reality-presence.txt;
corr=./correspondence.txt;
dups=./reality-before-dups-removed.txt;
candidates=./candidates.txt;
temp=./tmp.txt;
active=./active-nodes.txt;
inactive=./inactive-nodes.txt;
rm -rf ${bug};
rm -rf ${final};
rm -rf ${summary};
rm -rf ${arrival};
rm -rf ${corr};
rm -rf ${dups};
rm -rf ${candidates};
rm -rf ${temp};
rm -rf ${active};
rm -rf ${inactive};

if [ ! -f ${input} ]; then
	echo "File ${final} not found!";
	exit 1;
fi

EXP_START=`date -d "2004-07-26 00:00:00" +%s`;
EXP_END=`date -d "2005-05-05  23:59:59" +%s`;

NLines=`cat ${input} | wc -l`
#echo "Input: ${input}(${NLines} lines)"

echo -e -n " Step 1: Cleaning contacts.. 00%";
linecnt=0;
Nodefails=0;
STimefails=0;
ETimefails=0;
ValidContacts=0;
while read -r line
do
	let linecnt=${linecnt}+1;
	IFS='	', read -r f1 f2 f3 f4 f5 <<<"$line"
	#echo -e "${f1}\t${f2}\t${f3}\t${f4}\t${f5}"

	#Clean dataset - Remove Nodes that have ID greater than 97	
	if [ ${f4} -gt ${NN} ] || [ ${f5} -gt ${NN} ];then
		let Nodefails=${Nodefails}+1;
		echo "Line ${linecnt} removed because at least one node ID is above 97" >> ${bug}
		continue;
	fi
	if [ ${f4} = ${f5} ];then
		let Nodefails=${Nodefails}+1;
		echo "Line ${linecnt} removed because it had the same ID for both nodes" >> ${bug}
		continue;
	fi
	ContactStart=`echo "${f3}" | tr -d '"'`;
	Current=`date -d "${ContactStart}" +%s`;
	CurrentStart=${Current};
	let StartDIFF=${Current}-${EXP_START};
	#Clean dataset - Remove contacts that have starting time before "2004-07-26 00:00:00"
	if [ ${StartDIFF} -lt 0 ];then
		let STimefails=${STimefails}+1;
		echo "Line ${linecnt} removed because contact start time ${f3} is before \"2004-07-26 00:00:00\"" >> ${bug}
		continue;
	fi
	let DIFF=${Current}-${EXP_END};
	#Clean dataset - Remove contacts that have starting time after "2005-05-05  23:59:59"
	if [ ${DIFF} -gt 0 ];then
		let STimefails=${STimefails}+1;
		echo "Line ${linecnt} removed because contact start time ${f3} is after \"2005-05-05  23:59:59\"" >> ${bug}
		continue;
	fi

	ContactEnd=`echo "${f2}" | tr -d '"'`;
	Current=`date -d "${ContactEnd}" +%s`;
	CurrentEnd=${Current};
	let EndDIFF=${Current}-${EXP_START};
	#Clean dataset - Remove contacts that have ending time before "2004-07-26 00:00:00"
	if [ ${EndDIFF} -lt 0 ];then
		let ETimefails=${ETimefails}+1;
		echo "Line ${linecnt} removed because contact end time ${f2} is before \"2004-07-26 00:00:00\"" >> ${bug}
		continue;
	fi
	let DIFF=${Current}-${EXP_END};
	#Clean dataset - Remove contacts that have ending time after "2005-05-05  23:59:59"
	if [ ${DIFF} -gt 0 ];then
		let ETimefails=${ETimefails}+1;
		echo "Line ${linecnt} removed because contact end time ${f2} is after \"2005-05-05  23:59:59\"" >> ${bug}
		continue;
	fi
	#Check if start time is lower than end time
	if [ ${CurrentStart} -gt ${CurrentEnd} ];then
		let Logicfails=${Logicfails}+1;
		echo "Line ${linecnt} removed because contact start time ${f3} is larger than contact end time ${f2}" >> ${bug};
		continue;
	fi
	#Write results
	let ValidContacts=${ValidContacts}+1;
	echo -e -n "${f4}\t${f5}\t${StartDIFF}\t${EndDIFF}\n" >> ${final}
	echo -e "${ValidContacts}\t${linecnt}" > ${corr}

	status=$(echo "scale=2; (${linecnt}/${NLines})*100" | bc);
	echo -e -n "\b\b\b";
	printf "%02.0f%%" ${status};
done <"$input"

#change status
echo -e "\b\b\b${GREEN}[OK]${NC}";

#search for inactive devices
echo -n " Step 2: Searching for inactive nodes.. ";
for i in `seq 0 ${NN}`;
do
	awk -v dev=${i} 'BEGIN{found=0;}{if(!found && ($1 == dev || $2 == dev)){found=1;printf("Device %d found at line %d\n",dev,NR);}}' ${input} >> ${active};
	awk -v dev=${i} 'BEGIN{found=0;}{if(!found && ($1 == dev || $2 == dev)){found=1;}}END{if(!found){printf("%d\n",dev);}}' ${input} >> ${inactive};
done
echo -e "${GREEN}[OK]${NC}";

#Sort file
echo -n " Step 3: Sorting file.. "
sort -t$'\t' -n -k3,3 -k 4,4 ${final} > ./tmp.txt;
mv ./tmp.txt ${final};
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

#create the "first/last presence in the network" list
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
min=`awk 'NR==1{min=$3;}NR!=1{if($3<min){min=$3;}}END{printf("%d",min);}' ${final}`;
max=`awk 'NR==1{max=$4;}NR!=1{if($4>max){max=$4;}}END{printf("%d",max);}' ${final}`;

#create summary
percNodefails=$(echo "scale=2; (${Nodefails}/${NLines})*100" | bc);
percSTimefails=$(echo "scale=2; (${STimefails}/${NLines})*100" | bc);
percETimefails=$(echo "scale=2; (${ETimefails}/${NLines})*100" | bc);
echo " Summary";
echo " -------";
echo " - Created a file that contains ${ValidContacts} contacts" >> ${summary};
echo " - First contact starts at ${min} sec" >> ${summary};
echo " - Last contact ends at ${max} sec" >> ${summary};
echo " - ${Nodefails} contact(s) removed because of the participants' Node IDs - ${percNodefails}% of the original contacts" >> ${summary};
echo " - ${STimefails} contact(s) removed because of their start time - ${percSTimefails}% of the original contacts" >> ${summary};
echo " - ${ETimefails} contact(s) removed because of their end time - ${percETimefails}% of the original contacts" >> ${summary};
echo " - ${Logicfails} contact(s) removed because of they had larger start time (than end time)" >> ${summary};
cat ${summary};

