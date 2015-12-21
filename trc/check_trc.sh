#!/bin/bash

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

process_trace() {

	trc=$1;
		
	bounds=`awk 'BEGIN{min=10000;max=0;}{if($1<min){min=$1;}if($2<min){min=$2;}if($1>max){max=$1;}if($2>max){max=$2;}}END{printf("%d\t%d",min,max);}' ${trc}`;
	time_bounds=`awk 'BEGIN{min=10000;max=0;}{if($3<min){min=$3;}if($4<min){min=$4;}if($3>max){max=$3;}if($4>max){max=$4;}}END{printf("%d\t%d",min,max);}' ${trc}`;
	minID=$(echo ${bounds} | awk '{print $1;}');
	maxID=$(echo ${bounds} | awk '{print $2;}');
	NN=$(echo "scale=0;${maxID}+1" | bc -l);
	first_contact=$(echo ${time_bounds} | awk '{print $1;}');
	last_contact=$(echo ${time_bounds} | awk '{print $2;}');
	contacts=$(cat ${trc} | wc -l);
	active=`awk -v minID=${minID} -v maxID=${maxID} '{x[$1]++;x[$2]++;}END{for(i=minID;i<=maxID;i++){if(x[i]!=0){p++;}};printf("%d",p);}' ${trc}`;
#	inactive=`awk -v minID=${min} -v maxID=${max} '{x[$1]++;x[$2]++;}END{for(i=minID;i<=maxID;i++){if(x[i]==0){printf("%d ",i);}}}' ${trc}`;
	printf "| %-22s | %-7s | %-6s | %-6s | %-12s | %-10s | %-15s | %-15s |\n" "${trc}" "${NN}" "0" "${maxID}" "${active}" "${contacts}" "${first_contact}" "${last_contact}" >> "/tmp/trc_res.txt";
}


rm -rf "/tmp/trc_res.txt";
for file in *.txt;do
	if [[ ${file} != *"presence"* ]];then
		process_trace ${file} &
		spinner $! "Processing \"${file}\".. ";
		echo -e "${GREEN}[OK]${NC}";
	fi
done
printf "%s\n" "----------------------------------------------------------------------------------------------------------------------";
printf "| %-22s | %-7s | %-6s | %-6s | %-12s | %-10s | %-15s | %-15s |\n" "trace" "# nodes" "min ID" "max ID" "active nodes" "# contacts" "first contact" "duration";
printf "%s\n" "-------------------------|---------|--------|--------|--------------|------------|-----------------|-----------------|";
cat "/tmp/trc_res.txt";
printf "%s\n" "----------------------------------------------------------------------------------------------------------------------";

printf "%s\n\n" "This directory contains contact traces for the simulator." > ./README.md;
printf "%s\n" "----------------------------------------------------------------------------------------------------------------------" >> ./README.md;
printf "| %-22s | %-7s | %-6s | %-6s | %-12s | %-10s | %-15s | %-15s |\n" "trace" "# nodes" "min ID" "max ID" "active nodes" "# contacts" "first contact" "duration" >> ./README.md;
printf "%s\n" "-------------------------|---------|--------|--------|--------------|------------|-----------------|-----------------|" >> ./README.md;
cat "/tmp/trc_res.txt" >> ./README.md;
printf "%s\n" "----------------------------------------------------------------------------------------------------------------------" >> ./README.md;

echo "Updated file README.md..";
rm -rf "/tmp/trc_res.txt";

