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
#  Written by Nikolaos Papanikos.

export LC_NUMERIC="en_US.UTF-8";

RED='\033[0;31m';
LRED='\033[1;31m';
GREEN='\033[0;32m';
LGREEN='\033[1;32m';
BLUE='\033[0;34m';
LBLUE='\033[1;34m';
ORANGE='\033[0;33m';
NC='\033[0m'; # No Color

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
	# Get our process group id
     	PGID=$(ps -o pgid= $$ | grep -o [0-9]*);
      	# Kill it in a new process group
      	setsid kill -- -$PGID;
	exit 1;
}

spinner() {
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
	printf " ${GREEN}[OK]${NC} %s" "$infotext";
}

#Usage function
usage() {
	echo "usage: $0 [-h] [-n scenario] [-p scen_path] [-b bm_path] [-r range] [-o outfile]"
	echo "	-n scenario	specify scenario name (without extension)"
	echo "	-p scen_path	specify path to scenario data"
	echo "	-b bm_path	specify path to BonnMotion executable"
	echo "	-r range	specify the transmission range (defines when two nodes are in contact)"
	echo "	-o outfile	specify output file"
	echo "	-h		display help"
	exit 1;
}

NUMARGS=$#
if [ $NUMARGS -eq 0 ]; then
  usage;
fi

scenario="-";
bm_executable="bm";
scenario_path=".";
outfile="-";
range="50";
while getopts :n:p:b:r:o:h FLAG; do
  case $FLAG in
	n)
		scenario=$OPTARG;
		;;
	p)
		scenario_path=$OPTARG;
		;;
	b)
		bm_executable=$OPTARG;
		;;
	r)
		range=$OPTARG;
		;;
	o)
		outfile=$OPTARG;
		;;
	h)  #show help
      		usage;
      		;;
    	\?) #unrecognized option - show help
      		echo -e \\n"Option ${RED}-$OPTARG${NC} is not allowed."
     		usage;
      		;;
  esac
done
shift $(($OPTIND -1));

#check if bonnmotion is properly installed
bm_executable=$(readlink -m "${bm_executable}");
inst=$("${bm_executable}" 2>/dev/null | grep "BonnMotion" | wc -l);
if [ ${inst} -ne 1 ];then
	echo -e " ${RED}[ERROR]${NC} \"${bm_executable}\" is not a valid BonnMotion executable. Please check BonnMotion's installation. Exiting!";
	exit 1;
else
	echo -e " ${GREEN}[OK]${NC} BonnMotion is properly installed: \"${bm_executable}\""
fi

#check if BonnMotion mobility scenario exists
if [ ${scenario} = "-" ];then
	echo -e " ${RED}[ERROR]${NC} The name of the mobility scenario is mandatory. Use the -n option. Exiting!";
	exit 1;
else
	scenario_path=$(readlink -m "${scenario_path}");
	if [ ! -f "${scenario_path}/${scenario}.params" ];then
		echo -e " ${RED}[ERROR]${NC} Scenario data ${RED}\"${scenario_path}/${scenario}.params\"${NC} cannot be found. Use the ${ORANGE}-p option${NC} to provide the path of the directory containing the scenario data. Exiting!";
		exit 1;
	elif [ ! -f "${scenario_path}/${scenario}.movements.gz" ];then
		echo -e " ${RED}[ERROR]${NC} Scenario data ${RED}\"${scenario_path}/${scenario}.movements.gz\"${NC} cannot be found. Use the ${ORANGE}-p option${NC} to provide the path of the directory containing the scenario data. Exiting!";
		exit 1;
	else
		echo -e " ${GREEN}[OK]${NC} Scenario data exist: \"${scenario_path}\"";
	fi
fi

#select output filename
if [ ${outfile} = "-" ];then
	outfile=${scenario};	
fi

current_dir=$(pwd);
linkDump="/tmp/linkDump.txt";
outContacts="/tmp/${outfile}.ady";
outPresence="/tmp/${outfile}.presence";
outAdyton="${outfile}.adyton";

#run the BonnMotion's LinkDump application
${bm_executable} LinkDump -f ${scenario_path}/${scenario} -r ${range} > ${linkDump} &
spinner $! "Analyzing contacts using transmission range equal to ${range} m ";
echo -e "";

#Convert trace into a new format supported by Adyton
cat ${linkDump} | tail -n +10 | head -n -2 | awk -F'[-\t ]' '{for(i=3;i<=NF;i+=2){printf("%s\t%s\t%s\t%s\n",$1,$2,$i,$(i+1));}}' | sort -t$'\t' -n -k3,3 -k 4,4 > ${outContacts} &
spinner $! "Converting trace file to Adyton's format ";
#echo -e "${BLUE}-- \"${outContacts}\" created  ${NC}";
echo -e "";

#Calculate the presence information for each node
rm -rf "${outPresence}";
NN=$(awk -F'[=]' '{if($1=="nn"){printf("%s\n",$2);}}' "${scenario_path}/${scenario}.params");
maxID=$(echo "scale=0;${NN}-1" | bc -l);
msg="Calculating node presence in the trace ";
echo -e -n " [00%] ${msg}";
checkpoint=$(echo "scale=0;${maxID}/100" | bc -l);
progress=${checkpoint};
for j in `seq 0 ${maxID}`;
do
	echo -n -e "${j}\t" >> "${outPresence}";
	awk -v N=${j} 'BEGIN{mintime=100000000;maxtime=0;}{if($1 == N || $2 == N){if($3 < mintime){mintime=$3;}if($4 > maxtime){maxtime=$4;}}}END{if(mintime!=100000000){printf("%d\t%d\n",mintime,maxtime);}else{printf("-\t-\n");}}' "${outContacts}" >> "${outPresence}";

	if [ ${j} -eq ${progress} ];then
		status=$(echo "scale=2; (${j}/${maxID})*100" | bc);	
		echo -e -n "\b\b\b\b\b\b\b\b";
		for i in $(seq 1 ${#msg}); do
			printf "\b";
		done
		printf " [%02.0f%%] %s" ${status} "${msg}";
		progress=$(echo "scale=0;${j}+${checkpoint}" | bc -l);
	fi
done
echo -e -n "\b\b\b\b\b\b\b\b";
for i in $(seq 1 ${#msg}); do
	printf "\b";
done
printf " ${GREEN}[OK]${NC} %s" "${msg}";
#echo -e "${BLUE}-- \""${outPresence}"\" created ${NC}";
echo -e "";


#Write final file
#Write first part: Basic trace information
echo -e "# Basic info " > ${outAdyton};
#Total number of nodes
echo -e "NN\t${NN}" >> ${outAdyton};
#Number of nodes that participate in at least one contact
active=$(awk -v nn=${NN} 'BEGIN{cnt=0;}{if($2 == "-" && $3 == "-"){cnt++;}}END{printf("%d",nn-cnt);}' "${outPresence}");
echo -e "ACT\t${active}" >> ${outAdyton};
#Number of lines
nol=$(cat "${outContacts}" | wc -l);
echo -e "NoL\t${nol}" >> ${outAdyton};
#duration
dur=$(awk 'BEGIN{maxtime=0;}{if($3>maxtime){maxtime=$3;}if($4>maxtime){maxtime=$4;}}END{printf("%f",maxtime);}' "${outContacts}");
echo -e "DUR\t${dur}" >> ${outAdyton};
#scanning
echo -e "SCAN\t1.0" >> ${outAdyton};
#processing time
echo -e "PROC\t1sec" >> ${outAdyton};

#Write second part: Presence information
echo -e "# Presence info " >> ${outAdyton};
cat ${outPresence} >> ${outAdyton};

#Write third part: Contacts between network nodes
echo -e "# Node contacts " >> ${outAdyton};
cat ${outContacts} >> ${outAdyton};

echo -e "${BLUE} -- \""${outAdyton}"\" created successfully --${NC}";

#clean up
rm -rf ${outPresence};
rm -rf ${outContacts};

