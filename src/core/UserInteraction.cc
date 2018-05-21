/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
 *  and Evangelos Papapetrou
 *
 *  This file is part of Adyton.
 *
 *  Adyton is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Adyton is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Adyton.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#ifndef USERINTERACTION_H
	#define USERINTERACTION_H
	#include "UserInteraction.h"
#endif



UserInteraction::UserInteraction()
{
	profileName="";
	resDir ="";
	trcDir = "";
	customTrcPath="";
	includedProfile = false;
	modifiedResDir = false;
	modifiedTrcDir = false;

	TRC = NOTSET;
	RT = NOTSET;
	CC = NOTSET;
	SP = NOTSET;
	DP = NOTSET;
	DM = NOTSET;
	OUT = NOTSET;
	TT = NOTSET;
	NP = NOTSET;
	TTL = NOTSET;
	BUF = NOTSET;
	REP = NOTSET;
	SPLIT = NOTSET;
	SEED = NOTSET;

	BTCH = false;
	GUI = false;

	initRTnames();
	initTracenames();
	initSchedulingnames();
	initCongestionControlnames();
	initOutputTypenames();
	initTrafficTypenames();
	initDMnames();
	initDPnames();

	return;
}


UserInteraction::~UserInteraction()
{
	delete Set;

	Tracenames.clear();
	RTnames.clear();
	CongestionControlnames.clear();
	Schedulingnames.clear();
	DPnames.clear();
	DMnames.clear();
	OutputTypenames.clear();
	TrafficTypenames.clear();

	return;
}

string remove_ws( const std::string& str )
{
    std::string str_no_ws ;
    for( char c : str ) if( !isspace(c) ) str_no_ws += c ;
    return str_no_ws ;
}


Settings *UserInteraction::getSettings(int argc, char *argv[])
{
	int i;


	/* Parse the input arguments */
	printf("\n");
	if(argc == 1)
	{
		printf("Loading the default settings...\n\n");
	}
	else if(argc == 2)
	{
		/* Transform the argument in upper case */
		string details(argv[1]);
		transform(details.begin(), details.end(), details.begin(), ::toupper);

		if((!details.compare("WARRANTY")) || (!details.compare("W")) || (!details.compare("-WARRANTY")) || (!details.compare("-W")))
		{
			if(system("cat ../COPYING | head -n 598 | tail -n 8 | more") < 0)
			{
				printf("\nAn error occured while executing the command: cat ../COPYING | head -n 598 | tail -n 8 | more\n\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				exit(EXIT_SUCCESS);
			}
		}
		else if((!details.compare("CONDITIONS")) || (!details.compare("C")) || (!details.compare("-CONDITIONS")) || (!details.compare("-C")))
		{
			if(system("cat ../COPYING | head -n 621 | tail -n 551 | more") < 0)
			{
				printf("\nAn error occured while executing the command: cat ../COPYING | head -n 621 | tail -n 551 | more\n\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				exit(EXIT_SUCCESS);
			}
		}
		else if((!details.compare("HELP")) || (!details.compare("H")) || (!details.compare("-HELP")) || (!details.compare("-H")))
		{
			printLicence();
			printf("Available simulation parameters:\n");
			printf("./Adyton -TRACE <contact trace> -RT <routing protocol> -CC <congestion control mechanism> -SP <scheduling policy> -DP <dropping policy> -DM <deletion mechanism> -OUTPUT <output type> -TRAFFIC_TYPE <traffic type> -TRAFFIC_LOAD <number of packets> -TTL <time to live> -BUFFER <buffer size> -REP <replication number> -SPLIT <number of pieces> -SEED <seed number> -PROFILE <text file> -RES_DIR <results directory> -TRC_DIR <trace directory> -BATCH <batch mode>\n");
			printf("\nFor more information about the available options of a parameter, type `./Adyton -help <parameter>' or `./Adyton -h <parameter>'.\n\n");
			exit(EXIT_SUCCESS);
		}
		else
		{
			printLicence();
			printf("\nInvalid command-line arguments.\n");
			printf("For details about how to use the simulator, type `./Adyton -help' or `./Adyton -h'.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		/* Transform the argument in upper case */
		string details(argv[1]);
		transform(details.begin(), details.end(), details.begin(), ::toupper);
		
		if((!details.compare("HELP")) || (!details.compare("H")) || (!details.compare("-HELP")) || (!details.compare("-H")))
		{
			printLicence();
			string parameter;

			/* Parse each argument */
			for(i = 2; i < argc; i++)
			{
				parameter = string(argv[i]);
				transform(parameter.begin(), parameter.end(), parameter.begin(), ::toupper);
				printDetails(parameter);
			}

			exit(EXIT_SUCCESS);
		}
		else if((argc % 2) == 1)
		{
			/* Parse each argument */
			for(i = 1; i < argc; i += 2)
			{
				ParseArgs(argv[i], argv[i + 1]);
			}
		}
		else
		{
			printLicence();
			printf("\nInvalid command-line arguments.\n");
			printf("For details about how to use the simulator, type `./Adyton -help' or `./Adyton -h'.\n\n");
			exit(EXIT_FAILURE);
		}
	}


	/* Load the default settings */
	this->Set = new Settings();


	/* Load the profile arguments */
	if(includedProfile)
	{
		Set->LoadProfile(profileName);
	}


	/* Set the trace directory */
	if(modifiedTrcDir)
	{
		Set->setTraceDirectory(trcDir);
	}


	/* Set the contact trace */
	if(TRC != NOTSET)
	{
		Set->setContactTrace(TRC);
		if(TRC == CUSTOM_TR)
		{
			processCustomtrc();
		}
	}


	/* Set the routing protocol */
	if(RT != NOTSET)
	{
		Set->setRT(RT);
	}


	/* Set the congestion control mechanism */
	if(CC != NOTSET)
	{
		Set->setCongestionControl(CC);
	}


	/* Set the scheduling policy */
	if(SP != NOTSET)
	{
		Set->setSchedulingPolicy(SP);
	}


	/* Set the dropping policy */
	if(DP != NOTSET)
	{
		Set->setDroppingPolicy(DP);
	}


	/* Set the deletion mechanism */
	if(DM != NOTSET)
	{
		Set->setDM(DM);
	}


	/* Set the output type */
	if(OUT != NOTSET)
	{
		Set->setOUT(OUT);
	}


	/* Set the traffic type */
	if(TT != NOTSET)
	{
		Set->setTrafficType(TT);
	}


	/* Set the traffic load */
	if(NP != NOTSET)
	{
		Set->setTrafficLoad(NP);
	}


	/* Set the time to live */
	if(TTL != NOTSET)
	{
		Set->setTTL(TTL);
	}


	/* Set the buffer size */
	if(BUF != NOTSET)
	{
		Set->setBuffer(BUF);
	}


	/* Set the replication number */
	if(REP != NOTSET)
	{
		Set->setReplicas(REP);
	}


	/* Set the number of pieces to split the contact trace file */
	if(SPLIT != NOTSET)
	{
		Set->setSplit(SPLIT);
	}


	/* Set the seed number */
	if(SEED != NOTSET)
	{
		Set->setSeed(SEED);
	}


	/* Set the results directory */
	if(modifiedResDir)
	{
		Set->setResultsDirectory(resDir);
	}


	/* Set the batch mode */
	Set->setBatchmode(BTCH);


	/* The GUI support is still experimental */
	Set->setGUI(GUI);


	/* Check for combinations that are not supported */
	Set->lastCheck();


	/* Print the simulation settings */
	if(!BTCH && !GUI)
	{
		Set->printSettings();
	}

	return Set;
}

void UserInteraction::processCustomtrc(void)
{
	string workingtrc="";
	//Check if trace file exists
	ifstream test1(customTrcPath);
	ifstream test2("./trc/"+customTrcPath);
	bool modDir=false;
	if(modifiedTrcDir)
	{
		ifstream test3(trcDir+"/"+customTrcPath);
		if(test3)
		{
			modDir=true;
			workingtrc.assign(trcDir+"/"+customTrcPath);
		}
	}
	if(test1)
	{
		workingtrc.assign(customTrcPath);
	}
	if(test2)
	{
		workingtrc.assign("./trc/"+customTrcPath);
	}
	if (!test1 && !test2 && !modDir)
	{
		printf("\n[Error] Cannot open file \"%s\". Please make sure this trace file exists.\n",customTrcPath.c_str());
		printf("Tried the following locations:\n");
		printf("- \"%s\"\n",customTrcPath.c_str());
		printf("- \"./trc/%s\"\n",customTrcPath.c_str());
		if(modifiedTrcDir)
		{
			printf("\t- \"%s/%s\"\n",trcDir.c_str(),customTrcPath.c_str());
		}
		printf("\n");
		exit(EXIT_FAILURE);
	}
	//Get the trace name
	string base_filename = workingtrc.substr(workingtrc.find_last_of("/\\") + 1);
	string::size_type const p(base_filename.find_last_of('.'));
	string scenario_name = base_filename.substr(0, p);
	printf("Loading data from \"%s\".. ",workingtrc.c_str());
	fflush(stdout);
	
	string contacts = "./trc/CustomContacts";
	string presence = "./trc/CustomPresence";
	ofstream contactsFile;
	ofstream presenceFile;
	string tmpContactsName(contacts + "_" + scenario_name + "_adyton.txt");
	string tmpPresenceName(presence + "_" + scenario_name + "_adyton.txt");
	contactsFile.open(tmpContactsName);
	presenceFile.open(tmpPresenceName);
	int readNN=-1;
	int readACT=-1;
	int readNoL=-1;
	double readDUR=0.0;
	double readSCAN=0.0;
	string readPROCtime="";
	int part=0;
	string line="";
	ifstream infile;
	infile.open(workingtrc);
	//count the number of lines
	int totalLines=0;
	while(getline(infile,line))
	{
		totalLines++;
	}
	int oldprogress=0;
	double progress =0.0;
	int fraction=0;
	string progressIndicator("[0%]");
	printf("%s",progressIndicator.c_str());
	int currentLine=0;
	infile.clear();
	infile.seekg(0, infile.beg);
	while(getline(infile,line))
	{
		if(line[0] == '#')
		{
			part++;
			continue;
		}
		switch(part)
		{
			case 1:
			{
				string::size_type position = line.find("NN");
				if (position != string::npos) 
				{
					line.erase(position,position+2);
					line=remove_ws(line);
					readNN=atoi(line.c_str());
					//printf("%s\n",line.c_str());
				}
				position=line.find("ACT");
				if (position != string::npos) 
				{
					line.erase(position,position+3);
					line=remove_ws(line);
					readACT=atoi(line.c_str());
					//printf("%s\n",line.c_str());
				}
				position=line.find("NoL");
				if (position != string::npos) 
				{
					line.erase(position,position+3);
					line=remove_ws(line);
					readNoL=atoi(line.c_str());
					//printf("%s\n",line.c_str());
				}
				position=line.find("DUR");
				if (position != string::npos) 
				{
					line.erase(position,position+3);
					line=remove_ws(line);
					readDUR=atof(line.c_str());
					//printf("%s\n",line.c_str());
				}
				position=line.find("SCAN");
				if (position != string::npos) 
				{
					line.erase(position,position+4);
					line=remove_ws(line);
					readSCAN=atof(line.c_str());
					//printf("%s\n",line.c_str());
				}
				position=line.find("PROC");
				if (position != string::npos)
				{
					line.erase(position,position+4);
					line=remove_ws(line);
					readPROCtime.assign(line);
					//printf("%s\n",line.c_str());
				}
				break;
			}
			case 2:
			{
				presenceFile << line << endl;
				break;
			}
			case 3:
			{
				contactsFile << line << endl;
				break;
			}
			default:
			{
				printf("\n[Error] The provided trace file \"%s\" does not have a supported format!\n\n",workingtrc.c_str());
			exit(EXIT_FAILURE);
			}
		}
		currentLine++;
		progress=((double)currentLine/(double)totalLines)*100.0;
		fraction=(int)progress;
		if(fraction > (oldprogress+4))
		{
			for(int i=0;i<(int)progressIndicator.length();i++)
			{
				printf("\b");
			}
			progressIndicator="["+ to_string(fraction) +"%]";
			printf("%s",progressIndicator.c_str());
			oldprogress=fraction;
			fflush(stdout);
		}
	}
	infile.close();
	contactsFile.close();
	presenceFile.close();
	Set->setCustomTrcInfo(scenario_name, tmpContactsName, tmpPresenceName, readNN, readACT, readNoL, readDUR, readSCAN, readPROCtime, workingtrc);
	for(int i=0;i<(int)progressIndicator.length();i++)
	{
		printf("\b");
	}
	printf("[OK] \n");
	return;
}



void UserInteraction::ParseArgs(char *option, char *value)
{
	int convertedValue=-1;
	string optionType="";
	string userInput="";
	string userInputLowerCase="";
	
	/* Initializations */
	optionType.assign(option);
	userInput.assign(value);
	userInputLowerCase.assign(value);
	
	/* Transform the arguments into uppercase */
	transform(optionType.begin(), optionType.end(), optionType.begin(), ::toupper);
	transform(userInput.begin(), userInput.end(), userInput.begin(), ::toupper);


	/* Identify the type of the option */
	if(!optionType.compare("-TRACE"))
	{
		if(TRC == NOTSET)
		{
			if(!isNumber(userInput))
			{
				if(userInput.substr(userInput.find_last_of(".") + 1) == "ADY" || userInput.substr(userInput.find_last_of(".") + 1) == "ADYTON")
				{
					customTrcPath=userInputLowerCase;
					convertedValue=CUSTOM_TR;
				}
				else
				{
					convertedValue = convertTraceToID(userInput);
				}
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_TR))
			{
				TRC = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-TRACE\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help TRACE' or `./Adyton -h TRACE'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-TRACE\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-RT"))
	{
		if(RT == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertProtoToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_RT))
			{
				RT = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-RT\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help RT' or `./Adyton -h RT'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-RT\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-CC"))
	{
		if(CC == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertCongestionControlToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_CC))
			{
				CC = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-CC\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help CC' or `./Adyton -h CC'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-CC\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-SP"))
	{
		if(SP == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertSchedulingToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_SP))
			{
				SP = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-SP\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help SP' or `./Adyton -h SP'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-SP\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-DP"))
	{
		if(DP == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertDPToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_DP))
			{
				DP = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-DP\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help DP' or `./Adyton -h DP'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-DP\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-DM"))
	{
		if(DM == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertDMToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_DM))
			{
				DM = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-DM\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help DM' or `./Adyton -h DM'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-DM\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-OUTPUT"))
	{
		if(OUT == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertOutputTypeToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_OUT))
			{
				OUT = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-OUTPUT\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help OUTPUT' or `./Adyton -h OUTPUT'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-OUTPUT\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-TRAFFIC_TYPE"))
	{
		if(TT == NOTSET)
		{
			if(!isNumber(userInput))
			{
				convertedValue = convertTrafficTypeToID(userInput);
			}
			else
			{
				convertedValue = atoi(value);
			}

			if((convertedValue > 0) && (convertedValue < LAST_ENTRY_TT))
			{
				TT = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-TRAFFIC_TYPE\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help TRAFFIC_TYPE' or `./Adyton -h TRAFFIC_TYPE'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-TRAFFIC_TYPE\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-TRAFFIC_LOAD"))
	{
		if(NP == NOTSET)
		{
			convertedValue = atoi(value);

			if(convertedValue > 0)
			{
				NP = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-TRAFFIC_LOAD\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help TRAFFIC_LOAD' or `./Adyton -h TRAFFIC_LOAD'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-TRAFFIC_LOAD\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-TTL"))
	{
		if(TTL == NOTSET)
		{
			convertedValue = atoi(value);

			if(convertedValue > 0)
			{
				TTL = convertedValue;
			}
			else if(convertedValue <= 0)
			{
				TTL = INFINITE;
			}
			else
			{
				printf("\nError! Invalid value for the \"-TTL\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help TTL' or `./Adyton -h TTL'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-TTL\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-BUFFER"))
	{
		if(BUF == NOTSET)
		{
			convertedValue = atoi(value);

			if(convertedValue > 0)
			{
				BUF = convertedValue;
			}
			else if(convertedValue <= 0)
			{
				BUF = INFINITE;
			}
			else
			{
				printf("\nError! Invalid value for the \"-BUFFER\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help BUFFER' or `./Adyton -h BUFFER'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-BUFFER\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-REP"))
	{
		if(REP == NOTSET)
		{
			convertedValue = atoi(value);

			if(convertedValue > 0)
			{
				REP = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-REP\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help REP' or `./Adyton -h REP'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-REP\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-SPLIT"))
	{
		if(SPLIT == NOTSET)
		{
			convertedValue = atoi(value);

			if(convertedValue > 0)
			{
				SPLIT = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-SPLIT\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help SPLIT' or `./Adyton -h SPLIT'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-SPLIT\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-SEED"))
	{
		if(SEED == NOTSET)
		{
			convertedValue = atoi(value);

			if(convertedValue >= 0)
			{
				SEED = convertedValue;
			}
			else
			{
				printf("\nError! Invalid value for the \"-SEED\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help SEED' or `./Adyton -h SEED'.\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-SEED\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-PROFILE"))
	{
		if(!includedProfile)
		{
			if(access(value, F_OK) != 0)
			{
				printf("\nError! Invalid value for the \"-PROFILE\" option: %s\n", value);
				printf("For details about the available options, type `./Adyton -help PROFILE' or `./Adyton -h PROFILE'.\n\n");
				exit(EXIT_FAILURE);
			}

			profileName.assign(value);
			includedProfile = true;
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-PROFILE\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-RES_DIR"))
	{
		if(!modifiedResDir)
		{
			resDir.assign(value);
			modifiedResDir = true;
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-RES_DIR\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-TRC_DIR"))
	{
		if(!modifiedTrcDir)
		{
			trcDir.assign(value);
			modifiedTrcDir = true;
		}
		else
		{
			printf("\nError! More than one values have been given for the \"-TRC_DIR\" option.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-BATCH"))
	{
		if(!userInput.compare("ON"))
		{
			BTCH = true;
		}
		else if(!userInput.compare("OFF"))
		{
			BTCH = false;
		}
		else
		{
			printf("\nError! Invalid value for the \"-BATCH\" option: %s\n", value);
			printf("For details about the available options, type `./Adyton -help BATCH' or `./Adyton -h BATCH'.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else if(!optionType.compare("-GUI"))
	{/* Experimental use */
		if(!userInput.compare("ON"))
		{
			GUI = true;
		}
		else if(!userInput.compare("OFF"))
		{
			GUI = false;
		}
		else
		{
			printf("\nError! Invalid value for the \"-GUI\" option: %s\n", value);
			printf("For details about how to use the simulator, type `./Adyton -help' or `./Adyton -h'.\n\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		printf("\nError! Unknown option: %s\n", option);
		printf("For details about how to use the simulator, type `./Adyton -help' or `./Adyton -h'.\n\n");
		exit(EXIT_FAILURE);
	}

	return;
}


bool UserInteraction::isNumber(const string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}


int UserInteraction::convertTraceToID(const string& s)
{
	map<string,int>::iterator it;
	it = Tracenames.find(s);


	if (it != Tracenames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertProtoToID(const string& s)
{
	map<string,int>::iterator it;
	it = RTnames.find(s);


	if (it != RTnames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertCongestionControlToID(const string& s)
{
	map<string,int>::iterator it;
	it = CongestionControlnames.find(s);


	if (it != CongestionControlnames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertSchedulingToID(const string& s)
{
	map<string,int>::iterator it;
	it = Schedulingnames.find(s);


	if (it != Schedulingnames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertDPToID(const string& s)
{
	map<string,int>::iterator it;
	it = DPnames.find(s);


	if (it != DPnames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertDMToID(const string& s)
{
	map<string,int>::iterator it;
	it = DMnames.find(s);


	if (it != DMnames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertOutputTypeToID(const string& s)
{
	map<string,int>::iterator it;
	it = OutputTypenames.find(s);


	if (it != OutputTypenames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


int UserInteraction::convertTrafficTypeToID(const string& s)
{
	map<string,int>::iterator it;
	it = TrafficTypenames.find(s);


	if (it != TrafficTypenames.end())
	{
		return (int)it->second;
	}
	else
	{
		return NOTSET;
	}
}


void UserInteraction::initTracenames()
{
	Tracenames["PMTR"] = MILANO_TR;
	Tracenames["MILAN"] = MILANO_TR;
	Tracenames["MILANO"] = MILANO_TR;
	Tracenames["UNIMI"] = MILANO_TR;
	Tracenames["MILAN PMTR"] = MILANO_TR;
	Tracenames["MILAN-PMTR"] = MILANO_TR;
	Tracenames["MILAN_PMTR"] = MILANO_TR;
	Tracenames["MILANPMTR"] = MILANO_TR;
	Tracenames["MILANO PMTR"] = MILANO_TR;
	Tracenames["MILANO-PMTR"] = MILANO_TR;
	Tracenames["MILANO_PMTR"] = MILANO_TR;
	Tracenames["MILANOPMTR"] = MILANO_TR;
	Tracenames["UNIMI PMTR"] = MILANO_TR;
	Tracenames["UNIMI-PMTR"] = MILANO_TR;
	Tracenames["UNIMI_PMTR"] = MILANO_TR;
	Tracenames["UNIMIPMTR"] = MILANO_TR;

	Tracenames["MIT"] = MITREALITY_TR;
	Tracenames["REALITY"] = MITREALITY_TR;
	Tracenames["REALITY MINING"] = MITREALITY_TR;
	Tracenames["REALITY-MINING"] = MITREALITY_TR;
	Tracenames["REALITY_MINING"] = MITREALITY_TR;
	Tracenames["REALITYMINING"] = MITREALITY_TR;
	Tracenames["MIT REALITY"] = MITREALITY_TR;
	Tracenames["MIT-REALITY"] = MITREALITY_TR;
	Tracenames["MIT_REALITY"] = MITREALITY_TR;
	Tracenames["MITREALITY"] = MITREALITY_TR;
	Tracenames["MIT REALITY MINING"] = MITREALITY_TR;
	Tracenames["MIT-REALITY-MINING"] = MITREALITY_TR;
	Tracenames["MIT_REALITY_MINING"] = MITREALITY_TR;
	Tracenames["MITREALITYMINING"] = MITREALITY_TR;

	Tracenames["INTEL"] = INTEL_TR;
	Tracenames["IMOTE INTEL"] = INTEL_TR;
	Tracenames["IMOTE-INTEL"] = INTEL_TR;
	Tracenames["IMOTE_INTEL"] = INTEL_TR;
	Tracenames["IMOTEINTEL"] = INTEL_TR;
	Tracenames["HAGGLE INTEL"] = INTEL_TR;
	Tracenames["HAGGLE-INTEL"] = INTEL_TR;
	Tracenames["HAGGLE_INTEL"] = INTEL_TR;
	Tracenames["HAGGLEINTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE-INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE_INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGEINTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE IMOTE INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE-IMOTE-INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE_IMOTE_INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGEIMOTEINTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE HAGGLE INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE-HAGGLE-INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE_HAGGLE_INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGEHAGGLEINTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE HAGGLE IMOTE INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE-HAGGLE-IMOTE-INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGE_HAGGLE_IMOTE_INTEL"] = INTEL_TR;
	Tracenames["CAMBRIDGEHAGGLEIMOTEINTEL"] = INTEL_TR;

	Tracenames["CAM"] = HAGGLECAM_TR;
	Tracenames["IMOTE CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["IMOTE-CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["IMOTE_CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["IMOTECAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["HAGGLE CAM"] = HAGGLECAM_TR;
	Tracenames["HAGGLE-CAM"] = HAGGLECAM_TR;
	Tracenames["HAGGLE_CAM"] = HAGGLECAM_TR;
	Tracenames["HAGGLECAM"] = HAGGLECAM_TR;
	Tracenames["HAGGLE CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["HAGGLE-CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["HAGGLE_CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["HAGGLECAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE-CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE_CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGECAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE IMOTE CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE-IMOTE-CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE_IMOTE_CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGEIMOTECAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE HAGGLE CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE-HAGGLE-CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE_HAGGLE_CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGEHAGGLECAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE HAGGLE IMOTE CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE-HAGGLE-IMOTE-CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGE_HAGGLE_IMOTE_CAMBRIDGE"] = HAGGLECAM_TR;
	Tracenames["CAMBRIDGEHAGGLEIMOTECAMBRIDGE"] = HAGGLECAM_TR;

	Tracenames["INFOCOM 2005"] = INFOCOM05_TR;
	Tracenames["INFOCOM-2005"] = INFOCOM05_TR;
	Tracenames["INFOCOM_2005"] = INFOCOM05_TR;
	Tracenames["INFOCOM2005"] = INFOCOM05_TR;
	Tracenames["INFOCOM 05"] = INFOCOM05_TR;
	Tracenames["INFOCOM-05"] = INFOCOM05_TR;
	Tracenames["INFOCOM_05"] = INFOCOM05_TR;
	Tracenames["INFOCOM05"] = INFOCOM05_TR;
	Tracenames["INFOCOM '05"] = INFOCOM05_TR;
	Tracenames["INFOCOM-'05"] = INFOCOM05_TR;
	Tracenames["INFOCOM_'05"] = INFOCOM05_TR;
	Tracenames["INFOCOM'05"] = INFOCOM05_TR;
	Tracenames["INFOCOM 5"] = INFOCOM05_TR;
	Tracenames["INFOCOM-5"] = INFOCOM05_TR;
	Tracenames["INFOCOM_5"] = INFOCOM05_TR;
	Tracenames["INFOCOM5"] = INFOCOM05_TR;
	Tracenames["INFOCOM"] = INFOCOM05_TR;
	Tracenames["IMOTE INFOCOM"] = INFOCOM05_TR;
	Tracenames["IMOTE-INFOCOM"] = INFOCOM05_TR;
	Tracenames["IMOTE_INFOCOM"] = INFOCOM05_TR;
	Tracenames["IMOTEINFOCOM"] = INFOCOM05_TR;
	Tracenames["HAGGLE INFOCOM"] = INFOCOM05_TR;
	Tracenames["HAGGLE-INFOCOM"] = INFOCOM05_TR;
	Tracenames["HAGGLE_INFOCOM"] = INFOCOM05_TR;
	Tracenames["HAGGLEINFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE-INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE_INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGEINFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE IMOTE INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE-IMOTE-INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE_IMOTE_INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGEIMOTEINFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE HAGGLE INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE-HAGGLE-INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE_HAGGLE_INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGEHAGGLEINFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE HAGGLE IMOTE INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE-HAGGLE-IMOTE-INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGE_HAGGLE_IMOTE_INFOCOM"] = INFOCOM05_TR;
	Tracenames["CAMBRIDGEHAGGLEIMOTEINFOCOM"] = INFOCOM05_TR;

	Tracenames["INFOCOM 2006"] = INFOCOM06_TR;
	Tracenames["INFOCOM-2006"] = INFOCOM06_TR;
	Tracenames["INFOCOM_2006"] = INFOCOM06_TR;
	Tracenames["INFOCOM2006"] = INFOCOM06_TR;
	Tracenames["INFOCOM 06"] = INFOCOM06_TR;
	Tracenames["INFOCOM-06"] = INFOCOM06_TR;
	Tracenames["INFOCOM_06"] = INFOCOM06_TR;
	Tracenames["INFOCOM06"] = INFOCOM06_TR;
	Tracenames["INFOCOM '06"] = INFOCOM06_TR;
	Tracenames["INFOCOM-'06"] = INFOCOM06_TR;
	Tracenames["INFOCOM_'06"] = INFOCOM06_TR;
	Tracenames["INFOCOM'06"] = INFOCOM06_TR;
	Tracenames["INFOCOM 6"] = INFOCOM06_TR;
	Tracenames["INFOCOM-6"] = INFOCOM06_TR;
	Tracenames["INFOCOM_6"] = INFOCOM06_TR;
	Tracenames["INFOCOM6"] = INFOCOM06_TR;

	Tracenames["INFOCOM 2006 SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-2006-SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_2006_SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM2006SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM 06 SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-06-SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_06_SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM06SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM '06 SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-'06-SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_'06_SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM'06SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM 6 SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-6-SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_6_SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM6SR"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM 2006 SHORT RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-2006-SHORT-RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_2006_SHORT_RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM2006SHORTRANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM 06 SHORT RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-06-SHORT-RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_06_SHORT_RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM06SHORTRANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM '06 SHORT RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-'06-SHORT-RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_'06_SHORT_RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM'06SHORTRANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM 6 SHORT RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM-6-SHORT-RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM_6_SHORT_RANGE"] = INFOCOM06SR_TR;
	Tracenames["INFOCOM6SHORTRANGE"] = INFOCOM06SR_TR;

	Tracenames["SASSY"] = SASSY_TR;
	Tracenames["ST ANDREWS"] = SASSY_TR;
	Tracenames["ST_ANDREWS"] = SASSY_TR;
	Tracenames["ST_ANDREWS SASSY"] = SASSY_TR;
	Tracenames["ST_ANDREWS-SASSY"] = SASSY_TR;
	Tracenames["ST_ANDREWS_SASSY"] = SASSY_TR;
	Tracenames["ST_ANDREWSSASSY"] = SASSY_TR;

	Tracenames["THLAB"] = SIGCOMM_TR;
	Tracenames["SIGCOMM"] = SIGCOMM_TR;
	Tracenames["SIGCOMM 2009"] = SIGCOMM_TR;
	Tracenames["SIGCOMM-2009"] = SIGCOMM_TR;
	Tracenames["SIGCOMM_2009"] = SIGCOMM_TR;
	Tracenames["SIGCOMM2009"] = SIGCOMM_TR;
	Tracenames["SIGCOMM 09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM-09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM_09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM '09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM-'09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM_'09"] = SIGCOMM_TR;
	Tracenames["SIGCOMM'09"] = SIGCOMM_TR;
	Tracenames["THLAB SIGCOMM2009"] = SIGCOMM_TR;
	Tracenames["THLAB-SIGCOMM2009"] = SIGCOMM_TR;
	Tracenames["THLAB_SIGCOMM2009"] = SIGCOMM_TR;
	Tracenames["THLABSIGCOMM2009"] = SIGCOMM_TR;

	Tracenames["CONTENT"] = CAMBRIDGE_TR;
	Tracenames["CAMBRIDGE"] = CAMBRIDGE_TR;
	Tracenames["CAMBRIDGE CITY"] = CAMBRIDGE_TR;
	Tracenames["CAMBRIDGE-CITY"] = CAMBRIDGE_TR;
	Tracenames["CAMBRIDGE_CITY"] = CAMBRIDGE_TR;
	Tracenames["CAMBRIDGECITY"] = CAMBRIDGE_TR;
	Tracenames["UPMC CONTENT"] = CAMBRIDGE_TR;
	Tracenames["UPMC-CONTENT"] = CAMBRIDGE_TR;
	Tracenames["UPMC_CONTENT"] = CAMBRIDGE_TR;
	Tracenames["UPMCCONTENT"] = CAMBRIDGE_TR;

	Tracenames["CONTENT STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGE STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGE CITY STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["UPMC CONTENT STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CONTENT-STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGE-STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGE-CITY-STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["UPMC-CONTENT-STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CONTENT_STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGE_STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGE_CITY_STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["UPMC_CONTENT_STUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CONTENTSTUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGESTUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["CAMBRIDGECITYSTUDENTS"] = CAMBRIDGE_STUDENTS_TR;
	Tracenames["UPMCCONTENTSTUDENTS"] = CAMBRIDGE_STUDENTS_TR;

	Tracenames["ROLLERNET"] = ROLLERNET_TR;
	Tracenames["UPMC ROLLERNET"] = ROLLERNET_TR;
	Tracenames["UPMC-ROLLERNET"] = ROLLERNET_TR;
	Tracenames["UPMC_ROLLERNET"] = ROLLERNET_TR;
	Tracenames["UPMCROLLERNET"] = ROLLERNET_TR;

	Tracenames["NODOBO"] = NODOBO_TR;
	Tracenames["STRATH"] = NODOBO_TR;
	Tracenames["STRATH NODOBO"] = NODOBO_TR;
	Tracenames["STRATH-NODOBO"] = NODOBO_TR;
	Tracenames["STRATH_NODOBO"] = NODOBO_TR;
	Tracenames["STRATHNODOBO"] = NODOBO_TR;

	Tracenames["WTD"] = UCSD_TR;
	Tracenames["SYSNET"] = UCSD_TR;
	Tracenames["UCSD"] = UCSD_TR;
	Tracenames["WIRELESS TOPOLOGY DISCOVERY"] = UCSD_TR;
	Tracenames["WIRELESS-TOPOLOGY-DISCOVERY"] = UCSD_TR;
	Tracenames["WIRELESS_TOPOLOGY_DISCOVERY"] = UCSD_TR;
	Tracenames["WIRELESSTOPOLOGYDISCOVERY"] = UCSD_TR;
	Tracenames["UCSD WTD"] = UCSD_TR;
	Tracenames["UCSD-WTD"] = UCSD_TR;
	Tracenames["UCSD_WTD"] = UCSD_TR;
	Tracenames["UCSDWTD"] = UCSD_TR;
	Tracenames["SYSNET UCSD"] = UCSD_TR;
	Tracenames["SYSNET-UCSD"] = UCSD_TR;
	Tracenames["SYSNET_UCSD"] = UCSD_TR;
	Tracenames["SYSNETUCSD"] = UCSD_TR;
	Tracenames["SYSNET UCSD WTD"] = UCSD_TR;
	Tracenames["SYSNET-UCSD-WTD"] = UCSD_TR;
	Tracenames["SYSNET_UCSD_WTD"] = UCSD_TR;
	Tracenames["SYSNETUCSDWTD"] = UCSD_TR;

	Tracenames["WTD 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WTD-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WTD_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WTD3WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET3WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD3WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WIRELESS TOPOLOGY DISCOVERY 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WIRELESS-TOPOLOGY-DISCOVERY-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WIRELESS_TOPOLOGY_DISCOVERY_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["WIRELESSTOPOLOGYDISCOVERY3WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD WTD 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD-WTD-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSD_WTD_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["UCSDWTD3WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET UCSD 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET-UCSD-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET_UCSD_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNETUCSD3WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET UCSD WTD 3 WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET-UCSD-WTD-3-WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNET_UCSD_WTD_3_WEEKS"] = UCSD_3WEEKS_TR;
	Tracenames["SYSNETUCSDWTD3WEEKS"] = UCSD_3WEEKS_TR;

	Tracenames["EPFL"] = CABSPOTTING_TR;
	Tracenames["CABS"] = CABSPOTTING_TR;
	Tracenames["MOBILITY"] = CABSPOTTING_TR;
	Tracenames["CABSPOTTING"] = CABSPOTTING_TR;
	Tracenames["SAN FRANCISCO"] = CABSPOTTING_TR;
	Tracenames["SAN-FRANCISCO"] = CABSPOTTING_TR;
	Tracenames["SAN_FRANCISCO"] = CABSPOTTING_TR;
	Tracenames["SANFRANCISCO"] = CABSPOTTING_TR;
	Tracenames["EPFL MOBILITY"] = CABSPOTTING_TR;
	Tracenames["EPFL-MOBILITY"] = CABSPOTTING_TR;
	Tracenames["EPFL_MOBILITY"] = CABSPOTTING_TR;
	Tracenames["EPFLMOBILITY"] = CABSPOTTING_TR;

	Tracenames["EPFL 2 DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABS 2 DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["MOBILITY 2 DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABSPOTTING 2 DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["SAN FRANCISCO 2 DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFL MOBILITY 2 DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFL-2-DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABS-2-DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["MOBILITY-2-DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABSPOTTING-2-DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["SAN-FRANCISCO-2-DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFL-MOBILITY-2-DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFL_2_DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABS_2_DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["MOBILITY_2_DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABSPOTTING_2_DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["SAN_FRANCISCO_2_DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFL_MOBILITY_2_DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFL2DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABS2DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["MOBILITY2DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["CABSPOTTING2DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["SANFRANCISCO2DAYS"] = CABSPOTTING_2DAYS_TR;
	Tracenames["EPFLMOBILITY2DAYS"] = CABSPOTTING_2DAYS_TR;

	Tracenames["LYON"] = LYON_TR;
	Tracenames["SOCIOPATTERNS"] = LYON_TR;
	Tracenames["LYON PRIMARY SCHOOL"] = LYON_TR;
	Tracenames["LYON-PRIMARY-SCHOOL"] = LYON_TR;
	Tracenames["LYON_PRIMARY_SCHOOL"] = LYON_TR;
	Tracenames["LYONPRIMARYSCHOOL"] = LYON_TR;
	Tracenames["SOCIOPATTERNS LYON"] = LYON_TR;
	Tracenames["SOCIOPATTERNS-LYON"] = LYON_TR;
	Tracenames["SOCIOPATTERNS_LYON"] = LYON_TR;
	Tracenames["SOCIOPATTERNSLYON"] = LYON_TR;
	Tracenames["SOCIOPATTERNS LYON PRIMARY SCHOOL"] = LYON_TR;
	Tracenames["SOCIOPATTERNS-LYON-PRIMARY-SCHOOL"] = LYON_TR;
	Tracenames["SOCIOPATTERNS_LYON_PRIMARY_SCHOOL"] = LYON_TR;
	Tracenames["SOCIOPATTERNSLYONPRIMARYSCHOOL"] = LYON_TR;

	Tracenames["STANFORD"] = STANFORD_TR;
	Tracenames["SALATHE"] = STANFORD_TR;
	Tracenames["SALATHE STANFORD"] = STANFORD_TR;
	Tracenames["SALATHE-STANFORD"] = STANFORD_TR;
	Tracenames["SALATHE_STANFORD"] = STANFORD_TR;
	Tracenames["SALATHESTANFORD"] = STANFORD_TR;
	Tracenames["STANFORD HIGH SCHOOL"] = STANFORD_TR;
	Tracenames["STANFORD-HIGH-SCHOOL"] = STANFORD_TR;
	Tracenames["STANFORD_HIGH_SCHOOL"] = STANFORD_TR;
	Tracenames["STANFORDHIGHSCHOOL"] = STANFORD_TR;
	Tracenames["SALATHE STANFORD HIGH SCHOOL"] = STANFORD_TR;
	Tracenames["SALATHE-STANFORD-HIGH-SCHOOL"] = STANFORD_TR;
	Tracenames["SALATHE_STANFORD_HIGH_SCHOOL"] = STANFORD_TR;
	Tracenames["SALATHESTANFORDHIGHSCHOOL"] = STANFORD_TR;

	Tracenames["DIESEL"] = DIESELNET_TR;
	Tracenames["DIESELNET"] = DIESELNET_TR;
	Tracenames["UMASS DIESEL"] = DIESELNET_TR;
	Tracenames["UMASS-DIESEL"] = DIESELNET_TR;
	Tracenames["UMASS_DIESEL"] = DIESELNET_TR;
	Tracenames["UMASSDIESEL"] = DIESELNET_TR;
	Tracenames["UMASS DIESELNET"] = DIESELNET_TR;
	Tracenames["UMASS-DIESELNET"] = DIESELNET_TR;
	Tracenames["UMASS_DIESELNET"] = DIESELNET_TR;
	Tracenames["UMASSDIESELNET"] = DIESELNET_TR;

	Tracenames["DART"] = DARTMOUTH_TR;
	Tracenames["DARTMOUTH"] = DARTMOUTH_TR;
	Tracenames["DARTMOUTH CAMPUS"] = DARTMOUTH_TR;
	Tracenames["DARTMOUTH-CAMPUS"] = DARTMOUTH_TR;
	Tracenames["DARTMOUTH_CAMPUS"] = DARTMOUTH_TR;
	Tracenames["DARTMOUTHCAMPUS"] = DARTMOUTH_TR;

	Tracenames["DART 2 WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DART-2-WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DART_2_WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DART2WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH 2 WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH-2-WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH_2_WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH2WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH CAMPUS 2 WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH-CAMPUS-2-WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTH_CAMPUS_2_WEEKS"] = DARTMOUTH_2WEEKS_TR;
	Tracenames["DARTMOUTHCAMPUS2WEEKS"] = DARTMOUTH_2WEEKS_TR;

	Tracenames["DEBUG"] = DEBUG_TR;
	Tracenames["DEBUGGING"] = DEBUG_TR;

	return;
}


void UserInteraction::initRTnames()
{
	RTnames["DIRECT"] = DIRECT_RT;
	RTnames["DIRECT DELIVERY"] = DIRECT_RT;
	RTnames["DIRECT-DELIVERY"] = DIRECT_RT;
	RTnames["DIRECT_DELIVERY"] = DIRECT_RT;
	RTnames["DIRECTDELIVERY"] = DIRECT_RT;
	RTnames["DIRECT ROUTING"] = DIRECT_RT;
	RTnames["DIRECT-ROUTING"] = DIRECT_RT;
	RTnames["DIRECT_ROUTING"] = DIRECT_RT;
	RTnames["DIRECTROUTING"] = DIRECT_RT;
	RTnames["DIRECT TRANSMISSION"] = DIRECT_RT;
	RTnames["DIRECT-TRANSMISSION"] = DIRECT_RT;
	RTnames["DIRECT_TRANSMISSION"] = DIRECT_RT;
	RTnames["DIRECTTRANSMISSION"] = DIRECT_RT;

	RTnames["EPIDEMIC"] = EPIDEMIC_RT;
	RTnames["EPIDEMIC ROUTING"] = EPIDEMIC_RT;
	RTnames["EPIDEMIC-ROUTING"] = EPIDEMIC_RT;
	RTnames["EPIDEMIC_ROUTING"] = EPIDEMIC_RT;
	RTnames["EPIDEMICROUTING"] = EPIDEMIC_RT;

	RTnames["PROPHET"] = PROPHET_RT;

	RTnames["SIMBET"] = SIMBET_RT;

	RTnames["SIMBETTS"] = SIMBETTS_RT;

	RTnames["BUBBLE"] = BUBBLERAP_RT;
	RTnames["BUBBLE RAP"] = BUBBLERAP_RT;
	RTnames["BUBBLE-RAP"] = BUBBLERAP_RT;
	RTnames["BUBBLE_RAP"] = BUBBLERAP_RT;
	RTnames["BUBBLERAP"] = BUBBLERAP_RT;

	RTnames["SNW"] = SW_RT;
	RTnames["SAW"] = SW_RT;
	RTnames["S&W"] = SW_RT;
	RTnames["SW"] = SW_RT;
	RTnames["SPRAY AND WAIT"] = SW_RT;
	RTnames["SPRAY-AND-WAIT"] = SW_RT;
	RTnames["SPRAY_AND_WAIT"] = SW_RT;
	RTnames["SPRAYANDWAIT"] = SW_RT;
	RTnames["SPRAY & WAIT"] = SW_RT;
	RTnames["SPRAY-&-WAIT"] = SW_RT;
	RTnames["SPRAY_&_WAIT"] = SW_RT;
	RTnames["SPRAY&WAIT"] = SW_RT;
	RTnames["SPRAY WAIT"] = SW_RT;
	RTnames["SPRAY-WAIT"] = SW_RT;
	RTnames["SPRAY_WAIT"] = SW_RT;
	RTnames["SPRAYWAIT"] = SW_RT;

	RTnames["LSF SNW"] = LSFSW_RT;
	RTnames["LSF SAW"] = LSFSW_RT;
	RTnames["LSF S&W"] = LSFSW_RT;
	RTnames["LSF SW"] = LSFSW_RT;
	RTnames["LSF SPRAY AND WAIT"] = LSFSW_RT;
	RTnames["LSF SPRAY & WAIT"] = LSFSW_RT;
	RTnames["LSF SPRAY WAIT"] = LSFSW_RT;
	RTnames["LSF-SAW"] = LSFSW_RT;
	RTnames["LSF-SNW"] = LSFSW_RT;
	RTnames["LSF-S&W"] = LSFSW_RT;
	RTnames["LSF-SW"] = LSFSW_RT;
	RTnames["LSF-SPRAY-AND-WAIT"] = LSFSW_RT;
	RTnames["LSF-SPRAY-&-WAIT"] = LSFSW_RT;
	RTnames["LSF-SPRAY-WAIT"] = LSFSW_RT;
	RTnames["LSF_SAW"] = LSFSW_RT;
	RTnames["LSF_SNW"] = LSFSW_RT;
	RTnames["LSF_S&W"] = LSFSW_RT;
	RTnames["LSF_SW"] = LSFSW_RT;
	RTnames["LSF_SPRAY_AND_WAIT"] = LSFSW_RT;
	RTnames["LSF_SPRAY_&_WAIT"] = LSFSW_RT;
	RTnames["LSF_SPRAY_WAIT"] = LSFSW_RT;
	RTnames["LSFSAW"] = LSFSW_RT;
	RTnames["LSFSNW"] = LSFSW_RT;
	RTnames["LSFS&W"] = LSFSW_RT;
	RTnames["LSFSW"] = LSFSW_RT;
	RTnames["LSFSPRAYANDWAIT"] = LSFSW_RT;
	RTnames["LSFSPRAY&WAIT"] = LSFSW_RT;
	RTnames["LSFSPRAYWAIT"] = LSFSW_RT;

	RTnames["MSF SNW"] = MSFSW_RT;
	RTnames["MSF SAW"] = MSFSW_RT;
	RTnames["MSF S&W"] = MSFSW_RT;
	RTnames["MSF SW"] = MSFSW_RT;
	RTnames["MSF SPRAY AND WAIT"] = MSFSW_RT;
	RTnames["MSF SPRAY & WAIT"] = MSFSW_RT;
	RTnames["MSF SPRAY WAIT"] = MSFSW_RT;
	RTnames["MSF-SAW"] = MSFSW_RT;
	RTnames["MSF-SNW"] = MSFSW_RT;
	RTnames["MSF-S&W"] = MSFSW_RT;
	RTnames["MSF-SW"] = MSFSW_RT;
	RTnames["MSF-SPRAY-AND-WAIT"] = MSFSW_RT;
	RTnames["MSF-SPRAY-&-WAIT"] = MSFSW_RT;
	RTnames["MSF-SPRAY-WAIT"] = MSFSW_RT;
	RTnames["MSF_SAW"] = MSFSW_RT;
	RTnames["MSF_SNW"] = MSFSW_RT;
	RTnames["MSF_S&W"] = MSFSW_RT;
	RTnames["MSF_SW"] = MSFSW_RT;
	RTnames["MSF_SPRAY_AND_WAIT"] = MSFSW_RT;
	RTnames["MSF_SPRAY_&_WAIT"] = MSFSW_RT;
	RTnames["MSF_SPRAY_WAIT"] = MSFSW_RT;
	RTnames["MSFSAW"] = MSFSW_RT;
	RTnames["MSFSNW"] = MSFSW_RT;
	RTnames["MSFS&W"] = MSFSW_RT;
	RTnames["MSFSW"] = MSFSW_RT;
	RTnames["MSFSPRAYANDWAIT"] = MSFSW_RT;
	RTnames["MSFSPRAY&WAIT"] = MSFSW_RT;
	RTnames["MSFSPRAYWAIT"] = MSFSW_RT;

	RTnames["PROPHET SNW"] = PROPHETSW_RT;
	RTnames["PROPHET SAW"] = PROPHETSW_RT;
	RTnames["PROPHET S&W"] = PROPHETSW_RT;
	RTnames["PROPHET SW"] = PROPHETSW_RT;
	RTnames["PROPHET SPRAY AND WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET SPRAY & WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET SPRAY WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET-SAW"] = PROPHETSW_RT;
	RTnames["PROPHET-SNW"] = PROPHETSW_RT;
	RTnames["PROPHET-S&W"] = PROPHETSW_RT;
	RTnames["PROPHET-SW"] = PROPHETSW_RT;
	RTnames["PROPHET-SPRAY-AND-WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET-SPRAY-&-WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET-SPRAY-WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET_SAW"] = PROPHETSW_RT;
	RTnames["PROPHET_SNW"] = PROPHETSW_RT;
	RTnames["PROPHET_S&W"] = PROPHETSW_RT;
	RTnames["PROPHET_SW"] = PROPHETSW_RT;
	RTnames["PROPHET_SPRAY_AND_WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET_SPRAY_&_WAIT"] = PROPHETSW_RT;
	RTnames["PROPHET_SPRAY_WAIT"] = PROPHETSW_RT;
	RTnames["PROPHETSAW"] = PROPHETSW_RT;
	RTnames["PROPHETSNW"] = PROPHETSW_RT;
	RTnames["PROPHETS&W"] = PROPHETSW_RT;
	RTnames["PROPHETSW"] = PROPHETSW_RT;
	RTnames["PROPHETSPRAYANDWAIT"] = PROPHETSW_RT;
	RTnames["PROPHETSPRAY&WAIT"] = PROPHETSW_RT;
	RTnames["PROPHETSPRAYWAIT"] = PROPHETSW_RT;

	RTnames["LSF SNF"] = LSFSF_RT;
	RTnames["LSF SAF"] = LSFSF_RT;
	RTnames["LSF S&F"] = LSFSF_RT;
	RTnames["LSF SF"] = LSFSF_RT;
	RTnames["LSF SPRAY AND FOCUS"] = LSFSF_RT;
	RTnames["LSF SPRAY & FOCUS"] = LSFSF_RT;
	RTnames["LSF SPRAY FOCUS"] = LSFSF_RT;
	RTnames["LSF-SAF"] = LSFSF_RT;
	RTnames["LSF-SNF"] = LSFSF_RT;
	RTnames["LSF-S&F"] = LSFSF_RT;
	RTnames["LSF-SF"] = LSFSF_RT;
	RTnames["LSF-SPRAY-AND-FOCUS"] = LSFSF_RT;
	RTnames["LSF-SPRAY-&-FOCUS"] = LSFSF_RT;
	RTnames["LSF-SPRAY-FOCUS"] = LSFSF_RT;
	RTnames["LSF_SAF"] = LSFSF_RT;
	RTnames["LSF_SNF"] = LSFSF_RT;
	RTnames["LSF_S&F"] = LSFSF_RT;
	RTnames["LSF_SF"] = LSFSF_RT;
	RTnames["LSF_SPRAY AND FOCUS"] = LSFSF_RT;
	RTnames["LSF_SPRAY & FOCUS"] = LSFSF_RT;
	RTnames["LSF_SPRAY FOCUS"] = LSFSF_RT;
	RTnames["LSFSAF"] = LSFSF_RT;
	RTnames["LSFSNF"] = LSFSF_RT;
	RTnames["LSFS&F"] = LSFSF_RT;
	RTnames["LSFSF"] = LSFSF_RT;
	RTnames["LSFSPRAYANDFOCUS"] = LSFSF_RT;
	RTnames["LSFSPRAY&FOCUS"] = LSFSF_RT;
	RTnames["LSFSPRAYFOCUS"] = LSFSF_RT;

	RTnames["CNF"] = CNF_RT;
	RTnames["CAF"] = CNF_RT;
	RTnames["C&F"] = CNF_RT;
	RTnames["CF"] = CNF_RT;
	RTnames["COMPARE AND FORWARD"] = CNF_RT;
	RTnames["COMPARE-AND-FORWARD"] = CNF_RT;
	RTnames["COMPARE_AND_FORWARD"] = CNF_RT;
	RTnames["COMPAREANDFORWARD"] = CNF_RT;
	RTnames["COMPARE & FORWARD"] = CNF_RT;
	RTnames["COMPARE-&-FORWARD"] = CNF_RT;
	RTnames["COMPARE_&_FORWARD"] = CNF_RT;
	RTnames["COMPARE&FORWARD"] = CNF_RT;
	RTnames["COMPARE FORWARD"] = CNF_RT;
	RTnames["COMPARE-FORWARD"] = CNF_RT;
	RTnames["COMPARE_FORWARD"] = CNF_RT;
	RTnames["COMPAREFORWARD"] = CNF_RT;

	RTnames["CNR"] = CNR_RT;
	RTnames["CAR"] = CNR_RT;
	RTnames["C&R"] = CNR_RT;
	RTnames["CR"] = CNR_RT;
	RTnames["COMPARE AND REPLICATE"] = CNR_RT;
	RTnames["COMPARE-AND-REPLICATE"] = CNR_RT;
	RTnames["COMPARE_AND_REPLICATE"] = CNR_RT;
	RTnames["COMPAREANDREPLICATE"] = CNR_RT;
	RTnames["COMPARE & REPLICATE"] = CNR_RT;
	RTnames["COMPARE-&-REPLICATE"] = CNR_RT;
	RTnames["COMPARE_&_REPLICATE"] = CNR_RT;
	RTnames["COMPARE&REPLICATE"] = CNR_RT;
	RTnames["COMPARE REPLICATE"] = CNR_RT;
	RTnames["COMPARE-REPLICATE"] = CNR_RT;
	RTnames["COMPARE_REPLICATE"] = CNR_RT;
	RTnames["COMPAREREPLICATE"] = CNR_RT;

	RTnames["EBR"] = EBR_RT;
	RTnames["ENCOUNTER-BASED ROUTING"] = EBR_RT;
	RTnames["ENCOUNTER-BASED-ROUTING"] = EBR_RT;
	RTnames["ENCOUNTER-BASED_ROUTING"] = EBR_RT;
	RTnames["ENCOUNTER-BASEDROUTING"] = EBR_RT;
	RTnames["ENCOUNTER BASED ROUTING"] = EBR_RT;
	RTnames["ENCOUNTER_BASED_ROUTING"] = EBR_RT;
	RTnames["ENCOUNTERBASEDROUTING"] = EBR_RT;

	RTnames["DF"] = DELEGATION_RT;
	RTnames["DELEGATION"] = DELEGATION_RT;
	RTnames["DELEGATION FORWARDING"] = DELEGATION_RT;
	RTnames["DELEGATION-FORWARDING"] = DELEGATION_RT;
	RTnames["DELEGATION_FORWARDING"] = DELEGATION_RT;
	RTnames["DELEGATIONFORWARDING"] = DELEGATION_RT;

	RTnames["COORD"] = COORD_RT;
	RTnames["COORDINATED"] = COORD_RT;

	RTnames["OPTIMAL"] = OPTIMAL_RT;
	RTnames["OPTIMAL ROUTING"] = OPTIMAL_RT;
	RTnames["OPTIMAL-ROUTING"] = OPTIMAL_RT;
	RTnames["OPTIMAL_ROUTING"] = OPTIMAL_RT;
	RTnames["OPTIMALROUTING"] = OPTIMAL_RT;

	return;
}


void UserInteraction::initCongestionControlnames()
{
	CongestionControlnames["NONE"] = NOCC_CC;
	CongestionControlnames["NULL"] = NOCC_CC;
	CongestionControlnames["DISABLED"] = NOCC_CC;
	CongestionControlnames["NO CC"] = NOCC_CC;
	CongestionControlnames["NO-CC"] = NOCC_CC;
	CongestionControlnames["NO_CC"] = NOCC_CC;
	CongestionControlnames["NOCC"] = NOCC_CC;

	CongestionControlnames["AO"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["AVOID OVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["AVOID-OVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["AVOID_OVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["AVOIDOVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["NO OVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["NO-OVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["NO_OVERFLOW"] = AVOIDOVERFLOW_CC;
	CongestionControlnames["NOOVERFLOW"] = AVOIDOVERFLOW_CC;

	CongestionControlnames["ACC"] = ACC_CC;
	CongestionControlnames["AUTONOMOUS CONGESTION CONTROL"] = ACC_CC;
	CongestionControlnames["AUTONOMOUS-CONGESTION-CONTROL"] = ACC_CC;
	CongestionControlnames["AUTONOMOUS_CONGESTION_CONTROL"] = ACC_CC;
	CongestionControlnames["AUTONOMOUSCONGESTIONCONTROL"] = ACC_CC;

	CongestionControlnames["FR"] = FAIRROUTE_CC;
	CongestionControlnames["FAIR ROUTE"] = FAIRROUTE_CC;
	CongestionControlnames["FAIR-ROUTE"] = FAIRROUTE_CC;
	CongestionControlnames["FAIR_ROUTE"] = FAIRROUTE_CC;
	CongestionControlnames["FAIRROUTE"] = FAIRROUTE_CC;
	CongestionControlnames["FAIR ROUTING"] = FAIRROUTE_CC;
	CongestionControlnames["FAIR-ROUTING"] = FAIRROUTE_CC;
	CongestionControlnames["FAIR_ROUTING"] = FAIRROUTE_CC;
	CongestionControlnames["FAIRROUTING"] = FAIRROUTE_CC;

	CongestionControlnames["BSA"] = BSA_CC;
	CongestionControlnames["BUFFER SPACE ADVERTISEMENTS"] = BSA_CC;
	CongestionControlnames["BUFFER-SPACE-ADVERTISEMENTS"] = BSA_CC;
	CongestionControlnames["BUFFER_SPACE_ADVERTISEMENTS"] = BSA_CC;
	CongestionControlnames["BUFFERSPACEADVERTISEMENTS"] = BSA_CC;

	CongestionControlnames["CACC"] = CACC_CC;
	CongestionControlnames["CONTEXT-AWARE CONGESTION CONTROL"] = CACC_CC;
	CongestionControlnames["CONTEXT-AWARE-CONGESTION-CONTROL"] = CACC_CC;
	CongestionControlnames["CONTEXT-AWARE_CONGESTION_CONTROL"] = CACC_CC;
	CongestionControlnames["CONTEXT-AWARECONGESTIONCONTROL"] = CACC_CC;
	CongestionControlnames["CONTEXT AWARE CONGESTION CONTROL"] = CACC_CC;
	CongestionControlnames["CONTEXT_AWARE_CONGESTION_CONTROL"] = CACC_CC;
	CongestionControlnames["CONTEXTAWARECONGESTIONCONTROL"] = CACC_CC;

	CongestionControlnames["CCAF"] = CCAF_CC;
	CongestionControlnames["CONGESTION CONTROL WITH ADJUSTABLE FAIRNESS"] = CCAF_CC;
	CongestionControlnames["CONGESTION-CONTROL-WITH-ADJUSTABLE-FAIRNESS"] = CCAF_CC;
	CongestionControlnames["CONGESTION_CONTROL_WITH_ADJUSTABLE_FAIRNESS"] = CCAF_CC;
	CongestionControlnames["CONGESTIONCONTROLWITHADJUSTABLEFAIRNESS"] = CCAF_CC;

	return;
}


void UserInteraction::initSchedulingnames()
{
	Schedulingnames["FIFO"] = FIFO_SP;
	Schedulingnames["FIRST IN FIRST OUT"] = FIFO_SP;
	Schedulingnames["FIRST-IN-FIRST-OUT"] = FIFO_SP;
	Schedulingnames["FIRST_IN_FIRST_OUT"] = FIFO_SP;
	Schedulingnames["FIRSTINFIRSTOUT"] = FIFO_SP;

	Schedulingnames["LIFO"] = LIFO_SP;
	Schedulingnames["LAST IN FIRST OUT"] = LIFO_SP;
	Schedulingnames["LAST-IN-FIRST-OUT"] = LIFO_SP;
	Schedulingnames["LAST_IN_FIRST_OUT"] = LIFO_SP;
	Schedulingnames["LASTINFIRSTOUT"] = LIFO_SP;

	Schedulingnames["GRTRMAX"] = GRTRMAX_SP;
	Schedulingnames["GREATER AND MAXIMUM"] = GRTRMAX_SP;
	Schedulingnames["GREATER-AND-MAXIMUM"] = GRTRMAX_SP;
	Schedulingnames["GREATER_AND_MAXIMUM"] = GRTRMAX_SP;
	Schedulingnames["GREATERANDMAXIMUM"] = GRTRMAX_SP;

	Schedulingnames["GRTRSORT"] = GRTRSORT_SP;
	Schedulingnames["GREATER AND SORTED"] = GRTRSORT_SP;
	Schedulingnames["GREATER-AND-SORTED"] = GRTRSORT_SP;
	Schedulingnames["GREATER_AND_SORTED"] = GRTRSORT_SP;
	Schedulingnames["GREATERANDSORTED"] = GRTRSORT_SP;

	Schedulingnames["HNUV"] = HNUV_SP;
	Schedulingnames["HIGHEST NORMALIZED UTILITY VALUE"] = HNUV_SP;
	Schedulingnames["HIGHEST-NORMALIZED-UTILITY-VALUE"] = HNUV_SP;
	Schedulingnames["HIGHEST_NORMALIZED_UTILITY_VALUE"] = HNUV_SP;
	Schedulingnames["HIGHESTNORMALIZEDUTILITYVALUE"] = HNUV_SP;

	return;
}


void UserInteraction::initDPnames()
{
	DPnames["DF"] = DF_DP;
	DPnames["Drop Front"] = DF_DP;
	DPnames["Drop-Front"] = DF_DP;
	DPnames["Drop_Front"] = DF_DP;
	DPnames["DropFront"] = DF_DP;

	DPnames["DT"] = DT_DP;
	DPnames["Drop Tail"] = DT_DP;
	DPnames["Drop-Tail"] = DT_DP;
	DPnames["Drop_Tail"] = DT_DP;
	DPnames["DropTail"] = DT_DP;

	DPnames["DO"] = DO_DP;
	DPnames["Drop Oldest"] = DO_DP;
	DPnames["Drop-Oldest"] = DO_DP;
	DPnames["Drop_Oldest"] = DO_DP;
	DPnames["DropOldest"] = DO_DP;

	DPnames["DY"] = DY_DP;
	DPnames["Drop Youngest"] = DY_DP;
	DPnames["Drop-Youngest"] = DY_DP;
	DPnames["Drop_Youngest"] = DY_DP;
	DPnames["DropYoungest"] = DY_DP;

	DPnames["DF ASP"] = DFASP_DP;
	DPnames["DF-ASP"] = DFASP_DP;
	DPnames["DF_ASP"] = DFASP_DP;
	DPnames["DFASP"] = DFASP_DP;
	DPnames["Drop Front ASP"] = DFASP_DP;
	DPnames["Drop-Front-ASP"] = DFASP_DP;
	DPnames["Drop_Front_ASP"] = DFASP_DP;
	DPnames["DropFrontASP"] = DFASP_DP;
	DPnames["Drop Front Avoid Source Packets"] = DFASP_DP;
	DPnames["Drop-Front-Avoid-Source-Packets"] = DFASP_DP;
	DPnames["Drop_Front_Avoid_Source_Packets"] = DFASP_DP;
	DPnames["DropFrontAvoidSourcePackets"] = DFASP_DP;

	DPnames["DT ASP"] = DTASP_DP;
	DPnames["DT-ASP"] = DTASP_DP;
	DPnames["DT_ASP"] = DTASP_DP;
	DPnames["DTASP"] = DTASP_DP;
	DPnames["Drop Tail ASP"] = DTASP_DP;
	DPnames["Drop-Tail-ASP"] = DTASP_DP;
	DPnames["Drop_Tail_ASP"] = DTASP_DP;
	DPnames["DropTailASP"] = DTASP_DP;
	DPnames["Drop Tail Avoid Source Packets"] = DTASP_DP;
	DPnames["Drop-Tail-Avoid-Source-Packets"] = DTASP_DP;
	DPnames["Drop_Tail_Avoid_Source_Packets"] = DTASP_DP;
	DPnames["DropTailAvoidSourcePackets"] = DTASP_DP;

	DPnames["DO ASP"] = DOASP_DP;
	DPnames["DO-ASP"] = DOASP_DP;
	DPnames["DO_ASP"] = DOASP_DP;
	DPnames["DOASP"] = DOASP_DP;
	DPnames["Drop Oldest ASP"] = DOASP_DP;
	DPnames["Drop-Oldest-ASP"] = DOASP_DP;
	DPnames["Drop_Oldest_ASP"] = DOASP_DP;
	DPnames["DropOldestASP"] = DOASP_DP;
	DPnames["Drop Oldest Avoid Source Packets"] = DOASP_DP;
	DPnames["Drop-Oldest-Avoid-Source-Packets"] = DOASP_DP;
	DPnames["Drop_Oldest_Avoid_Source_Packets"] = DOASP_DP;
	DPnames["DropOldestAvoidSourcePackets"] = DOASP_DP;

	DPnames["DY ASP"] = DYASP_DP;
	DPnames["DY-ASP"] = DYASP_DP;
	DPnames["DY_ASP"] = DYASP_DP;
	DPnames["DYASP"] = DYASP_DP;
	DPnames["Drop Youngest ASP"] = DYASP_DP;
	DPnames["Drop-Youngest-ASP"] = DYASP_DP;
	DPnames["Drop_Youngest_ASP"] = DYASP_DP;
	DPnames["DropYoungestASP"] = DYASP_DP;
	DPnames["Drop Youngest Avoid Source Packets"] = DYASP_DP;
	DPnames["Drop-Youngest-Avoid-Source-Packets"] = DYASP_DP;
	DPnames["Drop_Youngest_Avoid_Source_Packets"] = DYASP_DP;
	DPnames["DropYoungestAvoidSourcePackets"] = DYASP_DP;

	return;
}


void UserInteraction::initDMnames()
{
	DMnames["JUST TTL"] = JUSTTTL_DM;
	DMnames["JUST-TTL"] = JUSTTTL_DM;
	DMnames["JUST_TTL"] = JUSTTTL_DM;
	DMnames["JUSTTTL"] = JUSTTTL_DM;
	DMnames["JUSTTL"] = JUSTTTL_DM;

	DMnames["NO DUPS"] = NODUPLICATES_DM;
	DMnames["NO-DUPS"] = NODUPLICATES_DM;
	DMnames["NO_DUPS"] = NODUPLICATES_DM;
	DMnames["NODUPS"] = NODUPLICATES_DM;
	DMnames["NO DUPLICATES"] = NODUPLICATES_DM;
	DMnames["NO-DUPLICATES"] = NODUPLICATES_DM;
	DMnames["NO_DUPLICATES"] = NODUPLICATES_DM;
	DMnames["NODUPLICATES"] = NODUPLICATES_DM;

	DMnames["KEEP REP"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEP-REP"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEP_REP"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEPREP"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEP REPLICATION"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEP-REPLICATION"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEP_REPLICATION"] = NODUPSKEEPRELICATION_DM;
	DMnames["KEEPREPLICATION"] = NODUPSKEEPRELICATION_DM;

	DMnames["VACCINE"] = VACCINE_DM;

	DMnames["CATACLYSM"] = CATACLYSM_DM;

	return;
}


void UserInteraction::initOutputTypenames()
{
	OutputTypenames["T 1"] = T1_OUT;
	OutputTypenames["T-1"] = T1_OUT;
	OutputTypenames["T_1"] = T1_OUT;
	OutputTypenames["T1"] = T1_OUT;
	OutputTypenames["TYPE 1"] = T1_OUT;
	OutputTypenames["TYPE-1"] = T1_OUT;
	OutputTypenames["TYPE_1"] = T1_OUT;
	OutputTypenames["TYPE1"] = T1_OUT;

	OutputTypenames["T 2"] = T2_OUT;
	OutputTypenames["T-2"] = T2_OUT;
	OutputTypenames["T_2"] = T2_OUT;
	OutputTypenames["T2"] = T2_OUT;
	OutputTypenames["TYPE 2"] = T2_OUT;
	OutputTypenames["TYPE-2"] = T2_OUT;
	OutputTypenames["TYPE_2"] = T2_OUT;
	OutputTypenames["TYPE2"] = T2_OUT;

	OutputTypenames["T 3"] = T3_OUT;
	OutputTypenames["T-3"] = T3_OUT;
	OutputTypenames["T_3"] = T3_OUT;
	OutputTypenames["T3"] = T3_OUT;
	OutputTypenames["TYPE 3"] = T3_OUT;
	OutputTypenames["TYPE-3"] = T3_OUT;
	OutputTypenames["TYPE_3"] = T3_OUT;
	OutputTypenames["TYPE3"] = T3_OUT;

	OutputTypenames["T 4"] = T4_OUT;
	OutputTypenames["T-4"] = T4_OUT;
	OutputTypenames["T_4"] = T4_OUT;
	OutputTypenames["T4"] = T4_OUT;
	OutputTypenames["TYPE 4"] = T4_OUT;
	OutputTypenames["TYPE-4"] = T4_OUT;
	OutputTypenames["TYPE_4"] = T4_OUT;
	OutputTypenames["TYPE4"] = T4_OUT;

	OutputTypenames["T 5"] = T5_OUT;
	OutputTypenames["T-5"] = T5_OUT;
	OutputTypenames["T_5"] = T5_OUT;
	OutputTypenames["T5"] = T5_OUT;
	OutputTypenames["TYPE 5"] = T5_OUT;
	OutputTypenames["TYPE-5"] = T5_OUT;
	OutputTypenames["TYPE_5"] = T5_OUT;
	OutputTypenames["TYPE5"] = T5_OUT;

	return;
}


void UserInteraction::initTrafficTypenames()
{
	TrafficTypenames["UNIFORM"] = UNIFORM_TT;
	TrafficTypenames["UNIFORM TRAFFIC"] = UNIFORM_TT;
	TrafficTypenames["UNIFORM-TRAFFIC"] = UNIFORM_TT;
	TrafficTypenames["UNIFORM_TRAFFIC"] = UNIFORM_TT;
	TrafficTypenames["UNIFORMTRAFFIC"] = UNIFORM_TT;

	TrafficTypenames["BURST"] = BURST_TT;
	TrafficTypenames["BURST TRAFFIC"] = BURST_TT;
	TrafficTypenames["BURST-TRAFFIC"] = BURST_TT;
	TrafficTypenames["BURST_TRAFFIC"] = BURST_TT;
	TrafficTypenames["BURSTTRAFFIC"] = BURST_TT;

	TrafficTypenames["SAMPLE"] = SAMPLE_TT;
	TrafficTypenames["SAMPLE TRAFFIC"] = SAMPLE_TT;
	TrafficTypenames["SAMPLE-TRAFFIC"] = SAMPLE_TT;
	TrafficTypenames["SAMPLE_TRAFFIC"] = SAMPLE_TT;
	TrafficTypenames["SAMPLETRAFFIC"] = SAMPLE_TT;

	TrafficTypenames["PREDEFINED"] = PREDEFINED_TT;
	TrafficTypenames["PREDEFINED TRAFFIC"] = PREDEFINED_TT;
	TrafficTypenames["PREDEFINED-TRAFFIC"] = PREDEFINED_TT;
	TrafficTypenames["PREDEFINED_TRAFFIC"] = PREDEFINED_TT;
	TrafficTypenames["PREDEFINEDTRAFFIC"] = PREDEFINED_TT;

	return;
}


void UserInteraction::printLicence()
{
	printf("     _       _       _              \n");
	printf("    / \\   __| |_   _| |_ ___  _ __  \n");
	printf("   / _ \\ / _` | | | | __/ _ \\| '_ \\ \n");
	printf("  / ___ \\ (_| | |_| | || (_) | | | |\n");
	printf(" /_/   \\_\\__,_|\\__, |\\__\\___/|_| |_|\n");
	printf("               |___/                \n");

	/* Print a short notice about the software license */
	printf("\nAdyton  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,\n");
	printf("and Evangelos Papapetrou\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY; for details type `./Adyton warranty'.\n");
	printf("This is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions; type `./Adyton conditions' for details.\n\n");

	return;
}

void UserInteraction::printDetails(string param)
{
	if(!param.compare("-TRACE") || !param.compare("TRACE"))
	{
		printf("+------------------+\n");
		printf("| -TRACE parameter |\n");
		printf("+------------------+-------------------------------------------------------+\n");
		printf("| %-30s | %-16s | %-20s |\n","Contact Trace","Arithmetic Value","String Value");
		printf("----------------------------------------------------------------------------\n");
		printf("| %-30s | %-16s | %-20s |\n","Milan PMTR","1","MILAN");
		printf("| %-30s | %-16s | %-20s |\n","MIT Reality Mining","2","REALITY");
		printf("| %-30s | %-16s | %-20s |\n","Intel","3","INTEL");
		printf("| %-30s | %-16s | %-20s |\n","Haggle-Cam","4","HAGGLE-CAM");
		printf("| %-30s | %-16s | %-20s |\n","INFOCOM 2005","5","INFOCOM-2005");
		printf("| %-30s | %-16s | %-20s |\n","INFOCOM 2006","6","INFOCOM-2006");
		printf("| %-30s | %-16s | %-20s |\n","INFOCOM 2006 (Short Range)","7","INFOCOM-2006-SR");
		printf("| %-30s | %-16s | %-20s |\n","SASSY","8","SASSY");
		printf("| %-30s | %-16s | %-20s |\n","SIGCOMM 2009","9","SIGCOMM-2009");
		printf("| %-30s | %-16s | %-20s |\n","Cambridge","10","CAMBRIDGE");
		printf("| %-30s | %-16s | %-20s |\n","Cambridge Students","11","CAMBRIDGE-STUDENTS");
		printf("| %-30s | %-16s | %-20s |\n","RollerNet","12","ROLLERNET");
		printf("| %-30s | %-16s | %-20s |\n","Nodobo","13","NODOBO");
		printf("| %-30s | %-16s | %-20s |\n","UCSD","14","UCSD");
		printf("| %-30s | %-16s | %-20s |\n","UCSD (3 Weeks)","15","UCSD-3-WEEKS");
		printf("| %-30s | %-16s | %-20s |\n","Cabspotting","16","CABS");
		printf("| %-30s | %-16s | %-20s |\n","Cabspotting (2 Days)","17","CABS-2-DAYS");
		printf("| %-30s | %-16s | %-20s |\n","Lyon Primary School","18","LYON");
		printf("| %-30s | %-16s | %-20s |\n","Stanford High School","19","STANFORD");
		printf("| %-30s | %-16s | %-20s |\n","DieselNet","20","DIESELNET");
		printf("| %-30s | %-16s | %-20s |\n","Dartmouth","21","DARTMOUTH");
		printf("| %-30s | %-16s | %-20s |\n","Dartmouth (2 Weeks)","22","DARTMOUTH-2-WEEKS");
		printf("| %-30s | %-16s | %-20s |\n","Debugging","23","DEBUG");
		printf("| %-30s | %-16s | %-20s |\n","Custom","24","/path/to/custom.ady");
		printf("+--------------------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-RT") || !param.compare("RT"))
	{
		printf("+---------------+\n");
		printf("| -RT parameter |\n");
		printf("+---------------+---------------------------------------------------------------+\n");
		printf("| %-35s | %-16s | %-20s |\n","Routing Protocol","Arithmetic Value","String Value");
		printf("---------------------------------------------------------------------------------\n");
		printf("| %-35s | %-16s | %-20s |\n","Direct Delivery","1","DIRECT");
		printf("| %-35s | %-16s | %-20s |\n","Epidemic Routing","2","EPIDEMIC");
		printf("| %-35s | %-16s | %-20s |\n","PRoPHET","3","PROPHET");
		printf("| %-35s | %-16s | %-20s |\n","SimBet","4","SIMBET");
		printf("| %-35s | %-16s | %-20s |\n","SimBetTS","5","SIMBETTS");
		printf("| %-35s | %-16s | %-20s |\n","Bubble Rap","6","BUBBLE");
		printf("| %-35s | %-16s | %-20s |\n","Spray and Wait","7","SW");
		printf("| %-35s | %-16s | %-20s |\n","LSF Spray and Wait","8","LSF-SNW");
		printf("| %-35s | %-16s | %-20s |\n","MSF Spray and Wait","9","MSF-SNW");
		printf("| %-35s | %-16s | %-20s |\n","PRoPHET Spray and Wait","10","PROPHET-SNW");
		printf("| %-35s | %-16s | %-20s |\n","LSF Spray and Focus","11","LSF-SNF");
		printf("| %-35s | %-16s | %-20s |\n","Compare and Forward","12","CNF");
		printf("| %-35s | %-16s | %-20s |\n","Compare and Replicate","13","CNR");
		printf("| %-35s | %-16s | %-20s |\n","Encounter-Based Routing","14","EBR");
		printf("| %-35s | %-16s | %-20s |\n","Delegation Forwarding","15","DF");
		printf("| %-35s | %-16s | %-20s |\n","Coordinated Delegation Forwarding","16","COORD");
		printf("| %-35s | %-16s | %-20s |\n","Optimal Routing","17","OPTIMAL");
		printf("+-------------------------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-CC") || !param.compare("CC"))
	{
		printf("+---------------+\n");
		printf("| -CC parameter |\n");
		printf("+---------------+----------------------------------------------------------+\n");
		printf("| %-30s | %-16s | %-20s |\n","Congestion Control Mechanism","Arithmetic Value","String Value");
		printf("----------------------------------------------------------------------------\n");
		printf("| %-30s | %-16s | %-20s |\n","None","1","NONE");
		printf("| %-30s | %-16s | %-20s |\n","Avoid Overflow","2","AVOID-OVERFLOW");
		printf("| %-30s | %-16s | %-20s |\n","ACC","3","ACC");
		printf("| %-30s | %-16s | %-20s |\n","FairRoute","4","FR");
		printf("| %-30s | %-16s | %-20s |\n","BSA","5","BSA");
		printf("| %-30s | %-16s | %-20s |\n","CACC","6","CACC");
		printf("| %-30s | %-16s | %-20s |\n","CCAF","7","CCAF");
		printf("+--------------------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-SP") || !param.compare("SP"))
	{
		printf("+---------------+\n");
		printf("| -SP parameter |\n");
		printf("+---------------+------------------------------------------------+\n");
		printf("| %-20s | %-16s | %-20s |\n","Scheduling Policy","Arithmetic Value","String Value");
		printf("------------------------------------------------------------------\n");
		printf("| %-20s | %-16s | %-20s |\n","FIFO","1","FIFO");
		printf("| %-20s | %-16s | %-20s |\n","LIFO","2","LIFO");
		printf("| %-20s | %-16s | %-20s |\n","GRTRMax","3","GRTRMAX");
		printf("| %-20s | %-16s | %-20s |\n","GRTRSort","4","GRTRSORT");
		printf("| %-20s | %-16s | %-20s |\n","HNUV","5","HNUV");
		printf("+----------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-DP") || !param.compare("DP"))
	{
		printf("+---------------+\n");
		printf("| -DP parameter |\n");
		printf("+---------------+--------------------------------------------------------------------+\n");
		printf("| %-40s | %-16s | %-20s |\n","Dropping Policy","Arithmetic Value","String Value");
		printf("--------------------------------------------------------------------------------------\n");
		printf("| %-40s | %-16s | %-20s |\n","Drop Front","1","DF");
		printf("| %-40s | %-16s | %-20s |\n","Drop Tail","2","DT");
		printf("| %-40s | %-16s | %-20s |\n","Drop Oldest","3","DO");
		printf("| %-40s | %-16s | %-20s |\n","Drop Youngest","4","DY");
		printf("| %-40s | %-16s | %-20s |\n","Drop Front - Avoid Source Packets","5","DF-ASP");
		printf("| %-40s | %-16s | %-20s |\n","Drop Tail - Avoid Source Packets","6","DT-ASP");
		printf("| %-40s | %-16s | %-20s |\n","Drop Oldest - Avoid Source Packets","7","DO-ASP");
		printf("| %-40s | %-16s | %-20s |\n","Drop Youngest - Avoid Source Packets","8","DY-ASP");
		printf("+------------------------------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-DM") || !param.compare("DM"))
	{
		printf("+---------------+\n");
		printf("| -DM parameter |\n");
		printf("+---------------+---------------------------------------------------------------+\n");
		printf("| %-35s | %-16s | %-20s |\n","Deletion Mechanism","Arithmetic Value","String Value");
		printf("---------------------------------------------------------------------------------\n");
		printf("| %-35s | %-16s | %-20s |\n","Just TTL","1","JUST-TTL");
		printf("| %-35s | %-16s | %-20s |\n","No Duplicates - Delete Replicas","2","NO-DUPS");
		printf("| %-35s | %-16s | %-20s |\n","No Duplicates - Keep Replicating","3","KEEP-REP");
		printf("| %-35s | %-16s | %-20s |\n","Vaccine","4","VACCINE");
		printf("| %-35s | %-16s | %-20s |\n","Cataclysm","5","CATACLYSM");
		printf("+-------------------------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-OUTPUT") || !param.compare("OUTPUT"))
	{
		printf("+-------------------+\n");
		printf("| -OUTPUT parameter |\n");
		printf("+-------------------+--------------------------------------------+\n");
		printf("| %-20s | %-16s | %-20s |\n","Output Type","Arithmetic Value","String Value");
		printf("------------------------------------------------------------------\n");
		printf("| %-20s | %-16s | %-20s |\n","Type 1","1","T1");
		printf("| %-20s | %-16s | %-20s |\n","Type 2","2","T2");
		printf("| %-20s | %-16s | %-20s |\n","Type 3","3","T3");
		printf("| %-20s | %-16s | %-20s |\n","Type 4","4","T4");
		printf("| %-20s | %-16s | %-20s |\n","Type 5","5","T5");
		printf("+----------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-TRAFFIC_TYPE") || !param.compare("TRAFFIC_TYPE"))
	{
		printf("+-------------------------+\n");
		printf("| -TRAFFIC_TYPE parameter |\n");
		printf("+-------------------------+--------------------------------------+\n");
		printf("| %-20s | %-16s | %-20s |\n","Traffic Type","Arithmetic Value","String Value");
		printf("------------------------------------------------------------------\n");
		printf("| %-20s | %-16s | %-20s |\n","Uniform","1","UNIFORM");
		printf("| %-20s | %-16s | %-20s |\n","Burst","2","BURST");
		printf("| %-20s | %-16s | %-20s |\n","Sample","3","SAMPLE");
		printf("| %-20s | %-16s | %-20s |\n","Predefined","4","PREDEFINED");
		printf("+----------------------------------------------------------------+\n\n");
	}
	else if(!param.compare("-TRAFFIC_LOAD") || !param.compare("TRAFFIC_LOAD"))
	{
		printf("+-------------------------+\n");
		printf("| -TRAFFIC_LOAD parameter |\n");
		printf("+-------------------------+\n");
		printf("Number of packets to generate (integer value).\n\n");
	}
	else if(!param.compare("-TTL") || !param.compare("TTL"))
	{
		printf("+----------------+\n");
		printf("| -TTL parameter |\n");
		printf("+----------------+\n");
		printf("Maximum amount of time that a packet is permitted to exist in the network.\n\n");
	}
	else if(!param.compare("-BUFFER") || !param.compare("BUFFER"))
	{
		printf("+-------------------+\n");
		printf("| -BUFFER parameter |\n");
		printf("+-------------------+\n");
		printf("Storage capacity of each node.\n\n");
	}
	else if(!param.compare("-REP") || !param.compare("REP"))
	{
		printf("+----------------+\n");
		printf("| -REP parameter |\n");
		printf("+----------------+\n");
		printf("Number of replicas to use (only for multi-copy routing protocols).\n\n");
	}
	else if(!param.compare("-SPLIT") || !param.compare("SPLIT"))
	{
		printf("+------------------+\n");
		printf("| -SPLIT parameter |\n");
		printf("+------------------+\n");
		printf("All contacts and simulation events are loaded and processed in pieces (renders simulation faster). The split value sets the number of pieces.\n\n");
	}
	else if(!param.compare("-SEED") || !param.compare("SEED"))
	{
		printf("+-----------------+\n");
		printf("| -SEED parameter |\n");
		printf("+-----------------+\n");
		printf("An integer to be used as the basis for the pseudo-random function.\n\n");
	}
	else if(!param.compare("-PROFILE") || !param.compare("PROFILE"))
	{
		printf("+--------------------+\n");
		printf("| -PROFILE parameter |\n");
		printf("+--------------------+\n");
		printf("Adyton supports per protocol parameterization through the -PROFILE argument. Similar to the basic parameterization, the -PROFILE argument is given at the command line after the Adyton execulable. After the -PROFILE argument the full system path pointing to a text file follows. This text file contains the additional per protocol parameters represented in a two column format. The first column is used for the parameter’s ID, while the second column is used for the corresponding value. The two columns must be separated using a tab or a space delimiter.\n");
		printf("More information can be found at 'doc/'.\n\n");
	}
	else if(!param.compare("-RES_DIR") || !param.compare("RES_DIR"))
	{
		printf("+--------------------+\n");
		printf("| -RES_DIR parameter |\n");
		printf("+--------------------+\n");
		printf("Name of the directory that will store the results of the simulation.\n\n");
	}
	else if(!param.compare("-TRC_DIR") || !param.compare("TRC_DIR"))
	{
		printf("+--------------------+\n");
		printf("| -TRC_DIR parameter |\n");
		printf("+--------------------+\n");
		printf("Name of the directory that stores the contact trace files.\n\n");
	}
	else if(!param.compare("-BATCH") || !param.compare("BATCH"))
	{
		printf("+------------------+\n");
		printf("| -BATCH parameter |\n");
		printf("+------------------+\n");
		printf("Print simulation output to the console or not (on/off).\n\n");
	}
	else
	{
		printf("%s: unknown parameter\n",param.c_str());
	}

	return;
}
