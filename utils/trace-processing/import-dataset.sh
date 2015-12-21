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

RED='\033[0;31m';
LRED='\033[1;31m';
GREEN='\033[0;32m';
LGREEN='\033[1;32m';
BLUE='\033[0;34m';
LBLUE='\033[1;34m';
ORANGE='\033[0;33m';
NC='\033[0m'; # No Color

trace[1]="MIT Reality Mining";
trace[2]="Infocom 2005";
trace[3]="Infocom 2006";
trace[4]="Sigcomm 2009";
trace[5]="Haggle-Cam";
trace[6]="Intel";
trace[7]="Rollernet";
trace[8]="Sassy";
trace[9]="Cambridge";
trace[10]="Milano";
trace[11]="Cabspotting";
trace[12]="Dartmouth";
trace[13]="DieselNet";
trace[14]="Nodobo";
trace[15]="Lyon";
trace[16]="Stanford";
trace[17]="UCSD";

link[1]="http://crawdad.org/download/mit/reality/realitymining.tar.gz";
link[2]="http://crawdad.org/download/cambridge/haggle/imote-trace3.tar.gz";
link[3]="http://crawdad.org/download/cambridge/haggle/Exp6.tar.gz";
link[4]="http://crawdad.org/download/thlab/sigcomm2009/sigcomm2009.tar.gz";
link[5]="http://crawdad.org/download/cambridge/haggle/imote-trace2.tar.gz";
link[6]="http://crawdad.org/download/cambridge/haggle/imote-trace1.tar.gz";
link[7]="http://crawdad.org/download/upmc/rollernet/imote-traces-RollerNet.tar.gz";
link[8]="http://crawdad.org/download/st_andrews/sassy/dsn.csv.gz";
link[9]="http://crawdad.org/download/upmc/content/imote-traces-cambridge.tar.gz";
link[10]="http://crawdad.org/download/unimi/pmtr/pmtr.txt.gz";
link[11]="http://crawdad.org/download/epfl/mobility/cabspottingdata.tar.gz";
link[12]="http://crawdad.org/download/dartmouth/campus/movement/movement-v1.3.tar.gz";
link[13]="http://crawdad.org/download/umass/diesel/DieselNetTracesSpring2006.tar.gz";
link[14]="http://crawdad.org/download/strath/nodobo/nodobo-csv.zip";
link[15]="http://www.sociopatterns.org/wp-content/uploads/2015/09/primaryschool.csv.gz";
link[16]="http://www.salathegroup.com/data/school_2010/moteFiles.zip";
link[17]="http://sysnet.ucsd.edu/wtd/data_download/wtd_data_release.tgz";

filename[1]="realitymining.tar.gz";
filename[2]="imote-trace3.tar.gz";
filename[3]="Exp6.tar.gz";
filename[4]="sigcomm2009.tar.gz";
filename[5]="imote-trace2.tar.gz";
filename[6]="imote-trace1.tar.gz";
filename[7]="imote-traces-RollerNet.tar.gz";
filename[8]="dsn.csv.gz";
filename[9]="imote-traces-cambridge.tar.gz";
filename[10]="pmtr.txt.gz";
filename[11]="cabspottingdata.tar.gz";
filename[12]="movement-v1.3.tar.gz";
filename[13]="DieselNetTracesSpring2006.tar.gz";
filename[14]="nodobo-csv.zip";
filename[15]="primaryschool.csv.gz";
filename[16]="moteFiles.zip";
filename[17]="wtd_data_release.tgz";

md5sum_original[1]="f74474a30fc5da6e3e5a4175ab9703bc";
md5sum_original[2]="50d8a4a73c5041a6bb05ba32668d66fb";
md5sum_original[3]="2c0dd1ff05d416133fd8013f5363c462";
md5sum_original[4]="c472f97492f79d1857047a31c3156deb";
md5sum_original[5]="d6163d0d716d3f8d90cff2174cf3d2a2";
md5sum_original[6]="476bbbf29e22090b2d6504b61bae6712";
md5sum_original[7]="8eac3ece9dd13c8c6c81118b1be96db3";
md5sum_original[8]="e7846248be3b25bc2c8475785792f6b4";
md5sum_original[9]="a5e10aef6751da0df267d66f9d3ff2ee";
md5sum_original[10]="4bfd394f34aa1aedac41c0e5fc473121";
md5sum_original[11]="8891f0bc3b2128ee903e9f9cb3018ce1";
md5sum_original[12]="b01a050a846e724122a55ea803473980";
md5sum_original[13]="1fa445b94084fc90844d5ae2c5eff720";
md5sum_original[14]="1ff9a236b96f9c3dfe7873d01e9b2204";
md5sum_original[15]="cf9e578f370cbaa907668a63a7bccbf0";
md5sum_original[16]="1d1386ab5aea87ab77bd9f6d444f532b";
md5sum_original[17]="39025a7dda7cc2a55dd875ec54fef17c";

directory[1]="Reality";
directory[2]="Infocom05";
directory[3]="Infocom06";
directory[4]="Sigcomm09";
directory[5]="Haggle-Cam";
directory[6]="Intel";
directory[7]="Rollernet";
directory[8]="Sassy";
directory[9]="Cambridge";
directory[10]="PMTR-Milan";
directory[11]="Cabspotting";
directory[12]="Dartmouth";
directory[13]="DieselNet";
directory[14]="Nodobo";
directory[15]="Lyon";
directory[16]="Stanford";
directory[17]="UCSD";

final_dataset[1]="Reality.txt";
final_dataset[2]="Infocom05.txt";
final_dataset[3]="Infocom06.txt";
final_dataset[4]="Sigcomm09.txt";
final_dataset[5]="HaggleCam.txt";
final_dataset[6]="Intel.txt";
final_dataset[7]="Rollernet.txt";
final_dataset[8]="Sassy.txt";
final_dataset[9]="Cambridge.txt";
final_dataset[10]="Milano.txt";
final_dataset[11]="Cabspotting.txt";
final_dataset[12]="Dartmouth.txt";
final_dataset[13]="DieselNet.txt";
final_dataset[14]="Nodobo.txt";
final_dataset[15]="Lyon.txt";
final_dataset[16]="Stanford.txt";
final_dataset[17]="UCSD.txt";

additional_dataset[1]="none";
additional_dataset[2]="none";
additional_dataset[3]="Infocom06-SR.txt";
additional_dataset[4]="none";
additional_dataset[5]="none";
additional_dataset[6]="none";
additional_dataset[7]="none";
additional_dataset[8]="none";
additional_dataset[9]="CambridgeStudents.txt";
additional_dataset[10]="none";
additional_dataset[11]="Cabspotting2days.txt";
additional_dataset[12]="Dartmouth2weeks.txt";
additional_dataset[13]="none";
additional_dataset[14]="none";
additional_dataset[15]="none";
additional_dataset[16]="none";
additional_dataset[17]="UCSD3weeks.txt";

presence_dataset[1]="Reality-presence.txt";
presence_dataset[2]="Infocom05-presence.txt";
presence_dataset[3]="Infocom06-presence.txt";
presence_dataset[4]="Sigcomm09-presence.txt";
presence_dataset[5]="HaggleCam-presence.txt";
presence_dataset[6]="Intel-presence.txt";
presence_dataset[7]="Rollernet-presence.txt";
presence_dataset[8]="Sassy-presence.txt";
presence_dataset[9]="Cambridge-presence.txt";
presence_dataset[10]="Milano-presence.txt";
presence_dataset[11]="Cabspotting-presence.txt";
presence_dataset[12]="Dartmouth-presence.txt";
presence_dataset[13]="DieselNet-presence.txt";
presence_dataset[14]="Nodobo-presence.txt";
presence_dataset[15]="Lyon-presence.txt";
presence_dataset[16]="Stanford-presence.txt";
presence_dataset[17]="UCSD-presence.txt";

additional_presence[1]="none";
additional_presence[2]="none";
additional_presence[3]="Infocom06-SR-presence.txt";
additional_presence[4]="none";
additional_presence[5]="none";
additional_presence[6]="none";
additional_presence[7]="none";
additional_presence[8]="none";
additional_presence[9]="CambridgeStudents-presence.txt";
additional_presence[10]="none";
additional_presence[11]="Cabspotting2days-presence.txt";
additional_presence[12]="Dartmouth2weeks-presence.txt";
additional_presence[13]="none";
additional_presence[14]="none";
additional_presence[15]="none";
additional_presence[16]="none";
additional_presence[17]="UCSD3weeks-presence.txt";

md5sum_final[1]="865222711256e1f072213d60ea09871b";
md5sum_final[2]="f429dae8616d2f3c82e3f17b1724ba22";
md5sum_final[3]="037601d79f4c4c4b2cae1baf5e1d0149";
md5sum_final[4]="3e744945a8795648bd08ece0ec9a392d";
md5sum_final[5]="93cd25bd72298e0bfa4663d1ee9918da";
md5sum_final[6]="96adc493193f4ffe3221781cadf668f5";
md5sum_final[7]="3738649470a9bd21832d2091cdf40b56";
md5sum_final[8]="799a17dac3c6c28438ddcd0ee4538299";
md5sum_final[9]="d1b007ca0c3e6eb1b77b26636077188b";
md5sum_final[10]="df374391fad0bc44a869753ec84ce222";
md5sum_final[11]="1172b92652b163d8775bb211876b21f8";
md5sum_final[12]="a9ac3c18a76e1c06b94da43c8ecb0dc0";
md5sum_final[13]="4be4f22d4af6227fecf464b77bf46c25";
md5sum_final[14]="6b5b6d7f7d5f5bd60fe41ea23a77be9a";
md5sum_final[15]="dd140318b1f756ea91e595e3d44724e3";
md5sum_final[16]="2da3c95e3181430cd31a88cb33e468ae";
md5sum_final[17]="00857628b3d8e106f050b48082612e8e";

md5sum_additional[1]="0";
md5sum_additional[2]="0";
md5sum_additional[3]="f51a09a81fac096c28282f6f1191a3ed";
md5sum_additional[4]="0";
md5sum_additional[5]="0";
md5sum_additional[6]="0";
md5sum_additional[7]="0";
md5sum_additional[8]="0";
md5sum_additional[9]="ab2db08c0add57c4977536047a1a6ca6";
md5sum_additional[10]="0";
md5sum_additional[11]="8fe0c5ebb4f990b71f96c32899307ea1";
md5sum_additional[12]="72391c656ebc998e433dee3d203a8af7";
md5sum_additional[13]="0";
md5sum_additional[14]="0";
md5sum_additional[15]="0";
md5sum_additional[16]="0";
md5sum_additional[17]="ccf2f5218efbb3eeaebfc6cbb01e54b3";

md5sum_presence[1]="5872c5fdbf63ef60d506acbb026c298d";
md5sum_presence[2]="d731b75d9485fe8cec2ea4136367d75c";
md5sum_presence[3]="b7d75e8b0afec2f664e81214ad4994dc";
md5sum_presence[4]="fd5afb61dfb9f5e53d2942dba641e9c8";
md5sum_presence[5]="cb333530444464ca93d00c9707eb9d76";
md5sum_presence[6]="775307b3df7a5564ba27d8c5f31d5342";
md5sum_presence[7]="bc7bdfa49018d2fcd683ecc222b4a24e";
md5sum_presence[8]="c21072699f1c78f68a8e944b2071e187";
md5sum_presence[9]="2daf95b035a4e2c41d6a7e831af45330";
md5sum_presence[10]="39012e6ac4d14647603765bd29d9c013";
md5sum_presence[11]="504c8ab7e8e8525c0aa56ae38bd0ec9d";
md5sum_presence[12]="36ff593b28a939d6d5e1f6e25744ff64";
md5sum_presence[13]="308ddd3020c7dbdd95ff66992aedf944";
md5sum_presence[14]="45429aa3c1542c2f516a13869aabc2ab";
md5sum_presence[15]="4c082d7cbc8d80b14e0ea1c6c3e1ad61";
md5sum_presence[16]="e19c5bbb630988cbb7a50bb1f7848f8d";
md5sum_presence[17]="1cd47bced905609403e06a7989c6c619";

md5sum_additional_presence[1]="0";
md5sum_additional_presence[2]="0";
md5sum_additional_presence[3]="8c03cf93c9d0eb133c2eb085d78a47d1";
md5sum_additional_presence[4]="0";
md5sum_additional_presence[5]="0";
md5sum_additional_presence[6]="0";
md5sum_additional_presence[7]="0";
md5sum_additional_presence[8]="0";
md5sum_additional_presence[9]="9a17186d1b2466f1c6abbdd22caf28dd";
md5sum_additional_presence[10]="0";
md5sum_additional_presence[11]="ee7e722d2b8a4ab1a7c96222a3d5fbfe";
md5sum_additional_presence[12]="2ceec3117fa54be0b64d2970605f40d6";
md5sum_additional_presence[13]="0";
md5sum_additional_presence[14]="0";
md5sum_additional_presence[15]="0";
md5sum_additional_presence[16]="0";
md5sum_additional_presence[17]="bf4f5d4877bdd22245ea3bd76c5a723c";

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

delete_all() {
	for trcID in {1..17}
	do
		delete_files ${trcID};
	done
	rm -rf ${ddir};
}

delete_files() {
	
	local ds=$1;

	cd ${base_dir};
	if [ ! -f "import-dataset.sh" ];then
		echo -e "${RED}** ERROR: \"import-dataset.sh\" has not been executed from the correct path.. **${NC}";
		return;
	fi
	cd ${directory[${ds}]};
	cd original;
	for i in `ls | grep -v "README"`;do rm -rf $i; done
	cd ../processing;
	for i in `ls | grep -v "process.sh\|process_Infocom06.sh\|match.awk\|remove-dups.awk\|process1\|process2\|process3\|process4\|process5\|results"`;do rm -rf $i; done
	if [ "${ds}" == "9" ] || [ "${ds}" == "13" ] || [ "${ds}" == "16" ];then
		for fold in {1..2}
		do
			cd process${fold};			
			rm -rf *.txt *.out;
			cd ..;
		done
		cd results;
		rm -rf *.txt;
		cd ..;
	fi
	if [ "${ds}" == "11" ] || [ "${ds}" == "12" ] || [ "${ds}" == "14" ] || [ "${ds}" == "17" ];then
		for fold in {1..5}
		do
			cd process${fold};			
			rm -rf *.txt *.out;
			cd ..;
		done
		cd results;
		rm -rf *.txt;
		cd ..;
	fi
	if [ "${ds}" == "15" ];then
		for fold in {1..3}
		do
			cd process${fold};			
			rm -rf *.txt *.out;
			cd ..;
		done
		cd results;
		rm -rf *.txt;
		cd ..;
	fi
	cd ../..;
}

function ctrl_c() {
	if [ "${gui}" == "on" ];then
		clear;
	fi
#       echo -e "\n Terminating dataset processing..\n";
	cd ${base_dir};
	delete_all;
	rm -rf ${ddir};
	rm -rf ${OUTPUT};
	rm -f ${passdata};
	# Get our process group id
     	PGID=$(ps -o pgid= $$ | grep -o [0-9]*);

      	# Kill it in a new new process group
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
	printf " [-] %s" "$infotext";
}

check_md5sum_original() {
	
	local ds=$1;

	md5=$(md5sum "${directory[${ds}]}/original/${filename[${ds}]}" | awk '{print $1}');
	sleep 1 &
	spinner $! "Checking the md5sum of the downloaded original file.. ";
	if [ "${md5}" == "${md5sum_original[${ds}]}" ];then
		md5status="ok";		
		echo -e "${GREEN}[OK]${NC}";
	else
		md5status="failed";
		echo -e "${RED}[FAILED]${NC}";
	fi
}

check_md5sum_final_dataset() {
	
	local ds=$1;

	md5=$(md5sum "${directory[${ds}]}/processing/${final_dataset[${ds}]}" | awk '{print $1}');
	sleep 1 &
	spinner $! "Checking the md5sum of \"${final_dataset[${ds}]}\".. ";
	if [ "${md5}" == "${md5sum_final[${ds}]}" ];then
		md5status="ok";		
		echo -e "${GREEN}[OK]${NC}";
	else
		md5status="failed";
		echo -e "${RED}[FAILED]${NC}";
	fi
}

check_md5sum_additional_dataset() {
	
	local ds=$1;

	if [ "${additional_dataset[${ds}]}" == "none" ];then
		md5status="ok";
		return;
	fi

	md5=$(md5sum "${directory[${ds}]}/processing/${additional_dataset[${ds}]}" | awk '{print $1}');
	sleep 1 &
	spinner $! "Checking the md5sum of \"${additional_dataset[${ds}]}\".. ";
	if [ "${md5}" == "${md5sum_additional[${ds}]}" ];then
		md5status="ok";		
		echo -e "${GREEN}[OK]${NC}";
	else
		md5status="failed";
		echo -e "[FAILED]";
	fi
}

check_md5sum_presence() {
	
	local ds=$1;

	md5=$(md5sum "${directory[${ds}]}/processing/${presence_dataset[${ds}]}" | awk '{print $1}');
	sleep 1 &
	spinner $! "Checking the md5sum of \"${presence_dataset[${ds}]}\".. ";
	if [ "${md5}" == "${md5sum_presence[${ds}]}" ];then
		md5status="ok";		
		echo -e "${GREEN}[OK]${NC}";
	else
		md5status="failed";
		echo -e "${RED}[FAILED]${NC}";
	fi
}

check_md5sum_additional_presence() {
	
	local ds=$1;

	if [ "${additional_presence[${ds}]}" == "none" ];then
		md5status="ok";
		return;
	fi

	md5=$(md5sum "${directory[${ds}]}/processing/${additional_presence[${ds}]}" | awk '{print $1}');
	sleep 1 &
	spinner $! "Checking the md5sum of \"${additional_presence[${ds}]}\".. ";
	if [ "${md5}" == "${md5sum_additional_presence[${ds}]}" ];then
		md5status="ok";		
		echo -e "${GREEN}[OK]${NC}";
	else
		md5status="failed";
		echo -e "${RED}[FAILED]${NC}";
	fi
}

download_dataset() {

	local trc=$1;
	cd ${ddir};
	while true
	do
		
		wget --quiet ${link[${trc}]} &
		spinner $! "Getting dataset ${trace[${trc}]}.. ";
		if [ -f "${filename[${trc}]}" ];then
			size=$(du -h "${filename[${trc}]}" | awk '{print $1}'); 
			echo -e "${GREEN}${size}B downloaded!${NC}";
			suc_download="ok";
			break;
		else
			echo -e "${RED}[FAILED]${NC}";
			echo -e " ${RED}**ERROR: Download failed! Either server is unavailable or there is a network problem. **${NC}";
			echo -ne "${BLUE}Try again? [y,n]: ${NC}";
			read answer;
			if [ "${answer}" != "y" ];then
				echo " Skipping ${trace[${trc}]} dataset download..";
				suc_download="failed";
				cd ..;
				return;
			fi
		fi
	done
	cp -f ${filename[${trc}]} ../${directory[${trc}]}/original/.;
	cd ..;
}

download_crawdad_dataset() {

	local trc=$1;

	cd ${ddir};
	
	if [ "${valid_user}" == "" ];then
		echo -e "\n * Requirement *";
		echo -e " In order to get the original datasets you should join the CRAWDAD community. The\n CRAWDAD registration is free and can be completed through the CRAWDAD registration \n page: <http://crawdad.org/joinup.html>. After creating a CRAWDAD account please run \n again this script and provide your credentials in order to download the datasets \n from CRAWDAD archive.\n";

		echo -ne " ${BLUE}Have you acquired CRAWDAD credentials? [y,n]: ${NC}";
		read answer;
		if [ "${answer}" != "y" ];then
			echo " Skipping ${trace[${trc}]} dataset download..";
			suc_download="failed";
			cd ..;
			return;
		fi
	fi

	while true
	do
		if [ "${valid_user}" == "" ];then
			echo -e "\n Please provide your CRAWDAD credentials in order to download data";
			echo -ne " username: ";
			read user;
			echo -ne " password: ";
			read -s pass;
			echo -e "\n";
			#get file from CRAWDAD archive usign new credentials
			wget --quiet --user ${user} --password ${pass} ${link[${trc}]} &
		else
			echo " Using the same CRAWDAD credentials..";
			#get file from CRAWDAD archive usign new credentials
			wget --quiet --user ${valid_user} --password ${valid_pass} ${link[${trc}]} &
		fi
		
		
		#sleep 2 &
		spinner $! "Getting dataset ${trace[${trc}]}.. ";
		if [ -f "${filename[${trc}]}" ];then
			size=$(du -h "${filename[${trc}]}" | awk '{print $1}'); 
			echo -e "${GREEN}${size}B downloaded!${NC}";
			suc_download="ok";
			if [ "${gui}" == "off" ];then
				valid_user="${user}";
				valid_pass="${pass}";
			fi
			break;
		else
			echo -e "${RED}[FAILED]${NC}";
			echo -e " ${RED}**ERROR: Download failed! Either CRAWDAD is either down or your credentials are incorrect. **${NC}";
			echo -ne "${BLUE}Try again? [y,n]: ${NC}";
			read answer;
			if [ "${answer}" != "y" ];then
				echo " Skipping ${trace[${trc}]} dataset download..";
				suc_download="failed";
				cd ..;
				return;
			else
				valid_user="";
				valid_pass="";
			fi
		fi
	done
	cp -f ${filename[${trc}]} ../${directory[${trc}]}/original/.;
	cd ..;
}

process_dataset() {
	
	local trc=$1;

	size=${#trace[${trc}]};
	size=$(echo "${size}+2" | bc -l);
	echo -ne "\n+";
	for i in $(seq 1 ${size});do
		echo -n "-";
	done
	echo "+";
	echo "| ${trace[${trc}]} |";
	echo -ne "+";
	for i in $(seq 1 ${size});do
		echo -n "-";
	done
	echo "+";

	if [ "${trc}" == "4" ];then
		echo -e "\n * NOTICE *";
		echo " The processing of this trace requires a significant amount of time.";
		echo -ne " ${BLUE}Continue? [y,n]: ${NC}";
		read answ;
		if [ ${answ} != "y" ];then
			echo -e "\n Processing of the ${trace[${trc}]} dataset aborted.";
			return;
		fi
	fi

	#download the dataset first
	if [ ! -f "${directory[${trc}]}/original/${filename[${trc}]}" ];then
		if [ "${gui}" == "off" ];then
			echo -e "${ORANGE}** WARNING: File ${filename[${trc}]} cannot be found. **${NC}";
			if [ ${trc} -gt 14 ];then
				echo -ne "${BLUE}Download the ${trace[${trc}]} trace now? [y,n]: ${NC}";
			else
				echo -ne "${BLUE}Download the ${trace[${trc}]} trace now (CRAWDAD credentials required)? [y,n]: ${NC}";
			fi
			read answer;
			if [ "${answer}" == "y" ];then
				if [ ${trc} -gt 14 ];then
					download_dataset ${trc};
				else
					download_crawdad_dataset ${trc};
				fi
				if [ ${suc_download} != "ok" ];then
					return;
				fi
			else
				echo " Skipping ${trace[${trc}]} dataset download..";
				return;
			fi
		else
			if [ ${trc} -gt 14 ];then
				download_dataset ${trc};
			else
				download_crawdad_dataset ${trc};
			fi
			if [ ${suc_download} != "ok" ];then
				return;
			fi	
		fi
	fi

	#check the md5sum of the downloaded original file
	while true
	do
		check_md5sum_original ${trc};
		if [ "${md5status}" != "ok" ];then
			echo -ne " md5 check sum for file ${filename[${trc}]} failed. ${BLUE}Download it again? [y,n]: ${NC}"
			read answer;
			if [ "${answer}" != "y" ];then
				return;
			else
				if [ ${trc} -gt 14 ];then
					download_dataset ${trc};
				else
					download_crawdad_dataset ${trc};
				fi
				if [ "${suc_download}" != "ok" ];then
					return;
				fi
			fi
		else
			break;
		fi
	done

	cd "${directory[${trc}]}"/original;
	if [ "${trc}" == "8" ] || [ "${trc}" == "10" ] || [ "${trc}" == "15" ];then
		gzip -kdf ${filename[${trc}]} &
	elif [ "${trc}" == "14" ] || [ "${trc}" == "16" ];then
		unzip ${filename[${trc}]} &> /dev/null &
	elif [ "${trc}" == "17" ];then
		tar -xzf ${filename[${trc}]};
		gzip -kdf "wtd_data_release/wtd.csv.gz" &
	else
		tar -xzf ${filename[${trc}]} &
	fi
	spinner $! "Extracting original dataset.. ";
	echo -e "${GREEN}[OK]${NC}";
	cd ../processing;
	chmod +x process.sh;
	/bin/bash ./process.sh

	cd ../..;
#	echo -e "\n+------------+";
#	echo "| Last check |";
#	echo "+------------+";
	#Check if the processed files exist
	if [ -f "${directory[${trc}]}/processing/${final_dataset[${trc}]}" ];then
		#Check the md5sum
		check_md5sum_final_dataset ${trc};
		if [ "${md5status}" != "ok" ];then
			echo -e " ${ORANGE}** WARNING: \"${final_dataset[${trc}]}\" failed to be validated using md5sum.. **${NC}"
		fi
		#copy to final location
		cp -f "${directory[${trc}]}/processing/${final_dataset[${trc}]}" "../../trc/${final_dataset[${trc}]}";
	else
		echo -e " ${RED}** ERROR: Processing of \"${final_dataset[${trc}]}\" failed. File does not exist!.. **${NC}";
	fi

	if [ -f "${directory[${trc}]}/processing/${presence_dataset[${trc}]}" ];then
		#Check the md5sum
		check_md5sum_presence ${trc};
		if [ "${md5status}" != "ok" ];then
			echo -e " ${ORANGE}** WARNING: \"${presence_dataset[${trc}]}\" failed to be validated using md5sum.. **${NC}"
		fi
		#copy to final location
		cp -f "${directory[${trc}]}/processing/${presence_dataset[${trc}]}" "../../trc/${presence_dataset[${trc}]}";
	else
		echo -e " ${RED}** ERROR: Processing of \"${presence_dataset[${trc}]}\" failed. File does not exist!.. **${NC}";
	fi

	if [ "${additional_dataset[${trc}]}" != "none" ];then
		if [ -f "${directory[${trc}]}/processing/${additional_dataset[${trc}]}" ];then
			#Check the md5sum
			check_md5sum_additional_dataset ${trc};
			if [ "${md5status}" != "ok" ];then
				echo -e " ${ORANGE}** WARNING: \"${additional_dataset[${trc}]}\" failed to be validated using md5sum.. **${NC}"
			fi
			#copy to final location
			cp -f "${directory[${trc}]}/processing/${additional_dataset[${trc}]}" "../../trc/${additional_dataset[${trc}]}";
		else
			echo -e " ${RED}** ERROR: Processing of \"${additional_dataset[${trc}]}\" failed. File does not exist!.. **${NC}";
		fi
	fi

	if [ "${additional_presence[${trc}]}" != "none" ];then
		if [ -f "${directory[${trc}]}/processing/${additional_presence[${trc}]}" ];then
			#Check the md5sum
			check_md5sum_additional_presence ${trc};
			if [ "${md5status}" != "ok" ];then
				echo -e " ${ORANGE}** WARNING: \"${additional_presence[${trc}]}\" failed to be validated using md5sum.. **${NC}"
			fi
			#copy to final location
			cp -f "${directory[${trc}]}/processing/${additional_presence[${trc}]}" "../../trc/${additional_presence[${trc}]}";
		else
			echo -e " ${RED}** ERROR: Processing of \"${additional_presence[${trc}]}\" failed. File does not exist!.. **${NC}";
		fi
	fi
	delete_files ${trc} &
	spinner $! "Deleting temp data.. ";
	echo -e "${GREEN}[OK]${NC}";	
}

process_reality() {

	echo -e "\n+-------------+";
	echo "| MIT Reality |";
	echo "+-------------+";
	echo -e "\n * NOTICE *";
	echo " The processing of this trace requires the MySQL package and valid credentials.";
	echo -ne " ${BLUE}Continue? [y,n]: ${NC}";
	read answ;
	if [ ${answ} != "y" ];then
		echo -e "\n Processing of the MIT Reality dataset aborted.";
		return;
	fi
	echo "";
	
	rm -rf ./Reality/original/reality.txt;
	randomName=$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c 32);

	#check if mysql is installed and running
	UP=$(pgrep mysql | wc -l);
	if [ "$UP" -ne 1 ];
	then
      	  	echo " MySQL is either not installed or down.";
		rm -rf ${ddir};
		exit 1;

	else
       		echo " MySQL is up!";
	fi

	#download dataset if needed
	if [ ! -f "${directory[1]}/original/${filename[1]}" ];then
		if [ "${gui}" == "off" ];then
			echo -e " ${ORANGE}** WARNING: File ${filename[1]} cannot be found. **${NC}";
			echo -ne " ${BLUE}Download the ${trace[1]} trace now (CRAWDAD credentials required)? [y,n]: ${NC}";
			read answer;
			if [ "${answer}" == "y" ];then
				download_crawdad_dataset 1;
				if [ ${suc_download} != "ok" ];then
					return;
				fi
			else
				echo " Skipping ${trace[1]} dataset download..";
				return;
			fi
		else
			download_crawdad_dataset 1;
			if [ ${suc_download} != "ok" ];then
				return;
			fi
		fi
	fi

	#check md5 sum
	while true
	do
		check_md5sum_original 1;
		if [ "${md5status}" != "ok" ];then
			echo -ne " md5 check sum for file ${filename[1]} failed. ${BLUE}Download it again? [y,n]: ${NC}"
			read answer;
			if [ "${answer}" != "y" ];then
				return;
			else
				download_crawdad_dataset 1;
				if [ "${suc_download}" != "ok" ];then
					return;
				fi
			fi
		else
			break;
		fi
	done

	#extract contents
	cd "${directory[1]}"/original;
	tar -xzf ${filename[1]} &
	spinner $! "Extracting original dataset.. ";
	echo -e "${GREEN}[OK]${NC}";
	cd ../..;		
	
	#get MySQL credentials
	echo -e "\n Please provide the credentials for MySQL to import the Reality dataset.";
	echo -ne " username: ";
	read user;
	echo -ne " password: ";
	read -s pass;
	echo "";

	#check mysql connectivity
	while ! mysql --user="${user}" --password="${pass}" -e ";" ; do
       		echo -ne " Invalid MySQL credentials..${BLUE}Try again? [y,n]: ${NC}";
		read answer;
		if [ "${answer}" != "y" ];then
			rm -rf ${ddir};
			exit 1;
		fi
		echo -ne " username: ";
		read user;
		echo -ne " password: ";
		read -s pass;
		echo "";
	done


	echo -ne "\n\n Inserting the Reality dataset to MySQL.. ";
	mysql --user="${user}" --password="${pass}"<< eof 
DROP DATABASE IF EXISTS REALITY_TRACE;
CREATE DATABASE REALITY_TRACE;
USE REALITY_TRACE;
SOURCE ./Reality/original/dump-anonymized.sql;
eof
	echo -e "${GREEN}[OK]${NC}";

	echo -ne " Exporting the Reality dataset from MySQL to a txt file.. ";
mysql --user="${user}" --password="${pass}"<< eof 
USE REALITY_TRACE;
SELECT * FROM \`devicespan\` INTO OUTFILE "/tmp/${randomName}" FIELDS TERMINATED BY "\t" LINES TERMINATED BY "\n";
eof
	echo -e "${GREEN}[OK]${NC}";

	cp /tmp/${randomName} ./Reality/original/reality.txt;
	cd ./Reality/processing;
	chmod +x process.sh;
	/bin/bash ./process.sh

	cd ../..;
	trc=1;
	echo -e "\n+------------+";
	echo "| Last check |";
	echo "+------------+";
	#Check if the processed files exist
	if [ -f "${directory[${trc}]}/processing/${final_dataset[${trc}]}" ];then
		#Check the md5sum
		check_md5sum_final_dataset ${trc};
		if [ "${md5status}" != "ok" ];then
			echo -e " ${ORANGE}** WARNING: \"${final_dataset[${trc}]}\" failed to be validated using md5sum.. **${NC}"
		fi
		#copy to final location
		cp -f "${directory[${trc}]}/processing/${final_dataset[${trc}]}" "../../trc/${final_dataset[${trc}]}";
	else
		echo -e " ${RED}** ERROR: Processing of \"${final_dataset[${trc}]}\" failed. File does not exist!.. **${NC}";
	fi

	if [ -f "${directory[${trc}]}/processing/${presence_dataset[${trc}]}" ];then
		#Check the md5sum
		check_md5sum_presence ${trc};
		if [ "${md5status}" != "ok" ];then
			echo -e " ${ORANGE}** WARNING: \"${presence_dataset[${trc}]}\" failed to be validated using md5sum.. **${NC}"
		fi
		#copy to final location
		cp -f "${directory[${trc}]}/processing/${presence_dataset[${trc}]}" "../../trc/${presence_dataset[${trc}]}";
	else
		echo -e " ${RED}** ERROR: Processing of \"${presence_dataset[${trc}]}\" failed. File does not exist!.. **${NC}";
	fi

	if [ "${additional_dataset[${trc}]}" != "none" ];then
		if [ -f "${directory[${trc}]}/processing/${additional_dataset[${trc}]}" ];then
			#Check the md5sum
			check_md5sum_additional_dataset ${trc};
			if [ "${md5status}" != "ok" ];then
				echo -e " ${ORANGE}** WARNING: \"${additional_dataset[${trc}]}\" failed to be validated using md5sum.. **${NC}"
			fi
			#copy to final location
			cp -f "${directory[${trc}]}/processing/${additional_dataset[${trc}]}" "../../trc/${additional_dataset[${trc}]}";
		else
			echo -e " ${RED}** ERROR: Processing of \"${additional_dataset[${trc}]}\" failed. File does not exist!.. **${NC}";
		fi
	fi

	if [ "${additional_presence[${trc}]}" != "none" ];then
		if [ -f "${directory[${trc}]}/processing/${additional_presence[${trc}]}" ];then
			#Check the md5sum
			check_md5sum_additional_presence ${trc};
			if [ "${md5status}" != "ok" ];then
				echo -e " ${ORANGE}** WARNING: \"${additional_presence[${trc}]}\" failed to be validated using md5sum.. **${NC}"
			fi
			#copy to final location
			cp -f "${directory[${trc}]}/processing/${additional_presence[${trc}]}" "../../trc/${additional_presence[${trc}]}";
		else
			echo -e " ${RED}** ERROR: Processing of \"${additional_presence[${trc}]}\" failed. File does not exist!.. **${NC}";
		fi
	fi
	delete_files 1 &
	spinner $! "Deleting temp data.. ";
	echo -e "${GREEN}[OK]${NC}";
}

gui_get_credentials() {

	OUTPUT="/tmp/input.txt";
	>${OUTPUT};
	
	# show an inputbox
		dialog --title "Please provide your CRAWDAD credentials in order to download data" \
		       --inputbox "Enter your username " 8 90 2>$OUTPUT
		# get respose
		response=$?
		# get data stored in $OUPUT using input redirection
		test_username=$(<$OUTPUT);
		# make a decsion 
		case ${response} in
  			0) 
  				#echo ${tmp_usename};
				passdata="/tmp/lala.off";
				>${passdata};
				# get password
				dialog --title "Please provide your CRAWDAD credentials in order to download data" \
				       --clear \
				       --insecure \
				       --passwordbox "Enter your password" 8 90 2> $passdata

				ret=$?
				test_pass=$(<$passdata);
				# make decision
				case ${ret} in
  					1)
						rm -f ${passdata};
						echo "Trace downloading and processing has been canceled.";
						;;
				  	255)
						echo "[ESC] key pressed.";
						rm -f ${passdata};
				esac
				rm -f ${passdata}; 
  				;;
  			1) 
  				echo "Trace downloading and processing has been canceled.";
				ctrl_c
				exit 0;
  				;;
  			255) 
   				echo "[ESC] key pressed.";
				ctrl_c;
				exit 0;
		esac
}

check_credentials() {

	local check_user=$1;
	local check_pass=$2;
	local test_trc=6;

	cd "/tmp";
	wget --quiet --user ${check_user} --password ${check_pass} ${link[${test_trc}]} &

	echo 10 | dialog --gauge "Checking credentials.." 10 50 0;
	step=25;
	while [ "$(ps a | awk '{print $1}' | grep $!)" ]; do
		sleep 1;		
		echo ${step} | dialog --gauge "Checking credentials.." 10 50 0;
		step=$(echo "scale=0;${step}+25" | bc -l);
	done
	echo 100 | dialog --gauge "Checking credentials.." 10 50 0;
	
	if [ -f "${filename[${test_trc}]}" ];then
		rm -rf "/tmp/${filename[${test_trc}]}";
		suc_cred="ok";		
	else
		dialog --title "Error" --clear --yesno "Invalid credentials. Try again?" 10 41;
		response=$?
		case ${response} in
   			0) echo "File deleted.";;
   			1) clear;ctrl_c;exit 0;;
   			255) clear;ctrl_c;exit 0;;
		esac
		suc_cred="failed";
	fi
	cd ${base_dir};
}

if [ "$#" == "0" ]; then
	echo -e "\nUsage: $0 <options>";
	echo "+-------------------+";	
	echo "| Available options |";
	echo "+-------------------+";
	echo -e "-gui: runs with a graphical interface";
	echo -e "-clean: cleans all temporary data";
	echo -e "<dataset name> .. <dataset name>: processes the datasets given";
	echo -e "\n+--------------------+";
	echo "| Available datasets |";
	echo "+--------------------+";
	printf "%-14s  %-14s  %-14s\n" "MIT Reality" "Haggle-Cam" "Intel";
	printf "%-14s  %-14s  %-14s\n" "Milano" "Rollernet" "UCSD";
	printf "%-14s  %-14s  %-14s\n" "Cambridge" "Cabspotting" "Stanford";
	printf "%-14s  %-14s  %-14s\n" "Infocom05" "Sassy" "Lyon";
	printf "%-14s  %-14s  %-14s\n" "Infocom06" "Dartmouth" "Nodobo";
	printf "%-14s  %-14s  %-14s\n" "Sigcomm09" "DieselNet";
	exit 0;
fi

base_dir=$(pwd);
if [ ! -f "import-dataset.sh" ];then
	echo -e "${RED}** ERROR: \"import-dataset.sh\" has not been executed from the correct path.. **${NC}";
	exit 1;
fi
ddir="temp_data";
rm -rf ${ddir};
mkdir ${ddir};
valid_user="";
valid_pass="";
gui="off";

for i in $*; do
	if [ "${i^^}" == "-GUI" ] || [ "${i^^}" == "-DIALOG" ];then
		gui="on";
		cmd=(dialog --separate-output --checklist "Select traces to download and process:" 22 76 16)
		options=(1 "MIT Reality Mining (estimated time: 20-25 minutes)" off    # any option can be set to default to "on"
        	 2 "Infocom 2005 (estimated time: 15-30 seconds)" off
        	 3 "Infocom 2006 (estimated time: 40-50 seconds)" off
		 4 "Sigcomm 2009 (estimated time: 10-15 minutes)" off
		 5 "Haggle-Cam (estimated time: 15-20 seconds)" off
	    	 6 "Intel (estimated time: 15-20 seconds)" off
		 7 "Rollernet (estimated time: 20-25 seconds)" off
		 8 "Sassy (estimated time: 20-25 seconds)" off
		 9 "Cambridge (estimated time: 20-25 seconds)" off
		 10 "Milano (estimated time: 15-20 seconds)" off
		 11 "Cabspotting (estimated time: 3.5-4 hours)" off
		 12 "Dartmouth (estimated time: 30-35 minutes)" off
		 13 "DieselNet (estimated time: 20-25 seconds)" off
		 14 "Nodobo (estimated time: 5-10 minutes)" off
		 15 "Lyon (estimated time: 40-50 minutes)" off
		 16 "Stanford (estimated time: 10-15 minutes)" off
       	  	 17 "UCSD (estimated time: 11-12 hours)" off)
		choices=$("${cmd[@]}" "${options[@]}" 2>&1 >/dev/tty)

		if [ "${choices}" == "" ];then
			clear;
			ctrl_c;
			exit 0;
		fi

		suc_cred="failed";
		while [ "${suc_cred}" != "ok" ];do
			gui_get_credentials
			check_credentials ${test_username} ${test_pass}
		done
		valid_user=${test_username};
		valid_pass=${test_pass};
		clear;
		for c in ${choices};do
			if [ "${c}" == "1" ];then
				process_reality;
			else
				process_dataset ${c};
			fi
		done
		delete_all;
		exit 0;
	fi
done

for i in $*; do
	if [ "${i^^}" == "-ALL" ];then
		process_dataset 2;
		process_dataset 3;
		process_dataset 5;
		process_dataset 6;
		process_dataset 7;
		process_dataset 8;
		process_dataset 9;
		process_dataset 10;
		process_dataset 11;
		process_dataset 12;
		process_dataset 4;
		process_dataset 13;
		process_dataset 14;
		process_dataset 15;
		process_dataset 16;
		process_dataset 17;
		process_reality;
		rm -rf ${ddir};
		exit 0;
	fi
done

for i in $*; do
	if [ "${i^^}" == "CLEAN" ] || [ "${i^^}" == "-CLEAN" ] || [ "${i^^}" == "-C" ];then
		echo "Deleting all downloaded and unnecessary data.."
		delete_all;
		exit 0;
	fi
done

for i in $*; do
	if [ "${i^^}" == "INFOCOM05" ];then
		process_dataset 2;
	elif [ "${i^^}" == "INFOCOM06" ] || [ "${i^^}" == "INFOCOM06-SR" ];then
		process_dataset 3;
	elif [ "${i^^}" == "SIGCOMM09" ];then
		process_dataset 4;
	elif [ "${i^^}" == "HAGGLE-CAM" ];then
		process_dataset 5;
	elif [ "${i^^}" == "INTEL" ];then
		process_dataset 6;
	elif [ "${i^^}" == "ROLLERNET" ];then
		process_dataset 7;
	elif [ "${i^^}" == "SASSY" ];then
		process_dataset 8;
	elif [ "${i^^}" == "CAMBRIDGE" ] || [ "${i^^}" == "CAMBRIDGESTUDENTS" ];then
		process_dataset 9;
	elif [ "${i^^}" == "MILANO" ] || [ "${i^^}" == "PMTR" ];then
		process_dataset 10;
	elif [ "${i^^}" == "CABSPOTTING" ] || [ "${i^^}" == "CABS" ] || [ "${i^^}" == "CABSPOTTING2DAYS" ];then
		process_dataset 11;
	elif [ "${i^^}" == "DARTMOUTH" ] || [ "${i^^}" == "DARTMOUTH2WEEKS" ];then
		process_dataset 12;
	elif [ "${i^^}" == "DIESELNET" ];then
		process_dataset 13;
	elif [ "${i^^}" == "NODOBO" ];then
		process_dataset 14;
	elif [ "${i^^}" == "LYON" ];then
		process_dataset 15;
	elif [ "${i^^}" == "STANFORD" ];then
		process_dataset 16;
	elif [ "${i^^}" == "UCSD" ] || [ "${i^^}" == "UCSD3WEEKS" ];then
		process_dataset 17;
	elif [ "${i^^}" == "REALITY" ] || [ "${i^^}" == "MIT" ];then
		process_reality;
	else
		echo "Dataset $i is not supported.";
	fi
done

#finalize
rm -rf ${ddir};

