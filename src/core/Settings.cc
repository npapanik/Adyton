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


#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "Settings.h"
#endif


Settings::Settings()
{
	/* Default settings */
	setTraceDirectory("../trc/");
	setContactTrace(MILANO_TR);
	setRT(DIRECT_RT);
	setCongestionControl(NOCC_CC);
	setSchedulingPolicy(FIFO_SP);
	setDroppingPolicy(DF_DP);
	setDM(JUSTTTL_DM);
	setOUT(T1_OUT);
	setTrafficType(UNIFORM_TT);
	setTrafficLoad(10000);
	setTTL(INFINITE);
	setBuffer(INFINITE);
	setReplicas(1);
	setSplit(100);
	setSeed(time(NULL));
	setResultsDirectory("../res/");
	setBatchmode(false);
	trcOriginalFilename="";

	this->ProfileName = "none";

	setGUI(false);
	this->copyMode = true;

	return;
}

Settings::~Settings()
{
	ProfileAttributes.clear();
	ProfileValues.clear();

	return;
}


void Settings::setTraceDirectory(string trcDir)
{
	if(trcDir.back() != '/')
	{
		this->traceDirectory = trcDir + "/";
	}
	else
	{
		this->traceDirectory = trcDir;
	}

	return;
}


void Settings::setContactTrace(int TRC)
{
	this->ContactTrace = TRC;

	switch(this->ContactTrace)
	{
		case MILANO_TR:
		{
			this->tracename.assign("Milano");
			this->contactFilename = this->traceDirectory + "Milano.txt";
			this->presenceFilename = this->traceDirectory + "Milano-presence.txt";
			this->NN = 50;
			this->ActiveNodes = 44;
			this->Lines = 11895;
			this->Duration = 1632979.0;
			this->scanningInterval = 1.0;
			this->processingTime="15-20 seconds";
			break;
		}
		case MITREALITY_TR:
		{
			this->tracename.assign("Reality");
			this->contactFilename = this->traceDirectory + "Reality.txt";
			this->presenceFilename = this->traceDirectory + "Reality-presence.txt";
			this->NN = 98;
			this->ActiveNodes = 97;
			this->Lines = 100955;
			this->Duration = 24428517.0;
			this->scanningInterval = 300.0;
			this->processingTime="20-25 minutes";
			break;
		}
		case INTEL_TR:
		{
			this->tracename.assign("Intel");
			this->contactFilename = this->traceDirectory + "Intel.txt";
			this->presenceFilename = this->traceDirectory + "Intel-presence.txt";
			this->NN = 10;
			this->ActiveNodes = 9;
			this->Lines = 1364;
			this->Duration = 359190.0;
			this->scanningInterval = 120.0;
			this->processingTime="15-20 seconds";
			break;
		}
		case HAGGLECAM_TR:
		{
			this->tracename.assign("Haggle-Cam");
			this->contactFilename = this->traceDirectory + "HaggleCam.txt";
			this->presenceFilename = this->traceDirectory + "HaggleCam-presence.txt";
			this->NN = 13;
			this->ActiveNodes = 12;
			this->Lines = 4228;
			this->Duration = 455609.0;
			this->scanningInterval = 120.0;
			this->processingTime="15-20 seconds";
			break;
		}
		case INFOCOM05_TR:
		{
			this->tracename.assign("Infocom05");
			this->contactFilename = this->traceDirectory + "Infocom05.txt";
			this->presenceFilename = this->traceDirectory + "Infocom05-presence.txt";
			this->NN = 42;
			this->ActiveNodes = 41;
			this->Lines = 22459;
			this->Duration = 254150.0;
			this->scanningInterval = 120.0;
			this->processingTime="15-30 seconds";
			break;
		}
		case INFOCOM06_TR:
		{
			this->tracename.assign("Infocom06");
			this->contactFilename = this->traceDirectory + "Infocom06.txt";
			this->presenceFilename = this->traceDirectory + "Infocom06-presence.txt";
			this->NN = 99;
			this->ActiveNodes = 98;
			this->Lines = 170593;
			this->Duration = 337418.0;
			this->scanningInterval = 120.0;
			this->processingTime="40-50 seconds";
			break;
		}
		case INFOCOM06SR_TR:
		{
			this->tracename.assign("Infocom06-SR");
			this->contactFilename = this->traceDirectory + "Infocom06-SR.txt";
			this->presenceFilename = this->traceDirectory + "Infocom06-SR-presence.txt";
			this->NN = 82;
			this->ActiveNodes = 81;
			this->Lines = 130483;
			this->Duration = 334720.0;
			this->scanningInterval = 120.0;
			this->processingTime="40-50 seconds";
			break;
		}
		case SASSY_TR:
		{
			this->tracename.assign("Sassy");
			this->contactFilename = this->traceDirectory + "Sassy.txt";
			this->presenceFilename = this->traceDirectory + "Sassy-presence.txt";
			this->NN = 26;
			this->ActiveNodes = 25;
			this->Lines = 52651;
			this->Duration = 6413284.0;
			this->scanningInterval = 7.0;
			this->processingTime="20-25 seconds";
			break;
		}
		case SIGCOMM_TR:
		{
			this->tracename.assign("Sigcomm09");
			this->contactFilename = this->traceDirectory + "Sigcomm09.txt";
			this->presenceFilename = this->traceDirectory + "Sigcomm09-presence.txt";
			this->NN = 77;
			this->ActiveNodes = 76;
			this->Lines = 35452;
			this->Duration = 320774.0;
			this->scanningInterval = 120.0;
			this->processingTime="10-15 minutes";
			break;
		}
		case CAMBRIDGE_TR:
		{
			this->tracename.assign("Cambridge");
			this->contactFilename = this->traceDirectory + "Cambridge.txt";
			this->presenceFilename = this->traceDirectory + "Cambridge-presence.txt";
			this->NN = 55;
			this->ActiveNodes = 52;
			this->Lines = 10873;
			this->Duration = 987529.0;
			this->scanningInterval = 600.0;
			this->processingTime="20-25 seconds";
			break;
		}
		case CAMBRIDGE_STUDENTS_TR:
		{
			this->tracename.assign("CambridgeStudents");
			this->contactFilename = this->traceDirectory + "CambridgeStudents.txt";
			this->presenceFilename = this->traceDirectory + "CambridgeStudents-presence.txt";
			this->NN = 37;
			this->ActiveNodes = 36;
			this->Lines = 10641;
			this->Duration = 987529.0;
			this->scanningInterval = 600.0;
			this->processingTime="20-25 seconds";
			break;
		}
		case ROLLERNET_TR:
		{
			this->tracename.assign("Rollernet");
			this->contactFilename = this->traceDirectory + "Rollernet.txt";
			this->presenceFilename = this->traceDirectory + "Rollernet-presence.txt";
			this->NN = 63;
			this->ActiveNodes = 62;
			this->Lines = 60146;
			this->Duration = 9976.0;
			this->scanningInterval = 15.0;
			this->processingTime="20-25 seconds";
			break;
		}
		case NODOBO_TR:
		{
			this->tracename.assign("Nodobo");
			this->contactFilename = this->traceDirectory + "Nodobo.txt";
			this->presenceFilename = this->traceDirectory + "Nodobo-presence.txt";
			this->NN = 28;
			this->ActiveNodes = 27;
			this->Lines = 99253;
			this->Duration = 12432109.0;
			this->scanningInterval = 60.0;
			this->processingTime="5-10 minutes";
			break;
		}
		case UCSD_TR:
		{
			this->tracename.assign("UCSD");
			this->contactFilename = this->traceDirectory + "UCSD.txt";
			this->presenceFilename = this->traceDirectory + "UCSD-presence.txt";
			this->NN = 276;
			this->ActiveNodes = 266;
			this->Lines = 2955871;
			this->Duration = 6739214.0;
			this->scanningInterval = 20.0;
			this->processingTime="11-12 hours";
			break;
		}
		case UCSD_3WEEKS_TR:
		{
			this->tracename.assign("UCSD3weeks");
			this->contactFilename = this->traceDirectory + "UCSD3weeks.txt";
			this->presenceFilename = this->traceDirectory + "UCSD3weeks-presence.txt";
			this->NN = 274;
			this->ActiveNodes = 237;
			this->Lines = 875984;
			this->Duration = 1814398.0;
			this->scanningInterval = 20.0;
			this->processingTime="11-12 hours";
			break;
		}
		case CABSPOTTING_TR:
		{
			this->tracename.assign("Cabspotting");
			this->contactFilename = this->traceDirectory + "Cabspotting.txt";
			this->presenceFilename = this->traceDirectory + "Cabspotting-presence.txt";
			this->NN = 536;
			this->ActiveNodes = 536;
			this->Lines = 8571225;
			this->Duration = 2070953.0;
			this->scanningInterval = 10.0;
			this->processingTime="3.5-4 hours";
			break;
		}
		case CABSPOTTING_2DAYS_TR:
		{
			this->tracename.assign("Cabspotting2days");
			this->contactFilename = this->traceDirectory + "Cabspotting2days.txt";
			this->presenceFilename = this->traceDirectory + "Cabspotting2days-presence.txt";
			this->NN = 536;
			this->ActiveNodes = 511;
			this->Lines = 775149;
			this->Duration = 172799.0;
			this->scanningInterval = 10.0;
			this->processingTime="3.5-4 hours";
			break;
		}
		case LYON_TR:
		{
			this->tracename.assign("Lyon");
			this->contactFilename = this->traceDirectory + "Lyon.txt";
			this->presenceFilename = this->traceDirectory + "Lyon-presence.txt";
			this->NN = 242;
			this->ActiveNodes = 242;
			this->Lines = 77521;
			this->Duration = 116920.0;
			this->scanningInterval = 20.0;
			this->processingTime="40-50 minutes";
			break;
		}
		case STANFORD_TR:
		{
			this->tracename.assign("Stanford");
			this->contactFilename = this->traceDirectory + "Stanford.txt";
			this->presenceFilename = this->traceDirectory + "Stanford-presence.txt";
			this->NN = 790;
			this->ActiveNodes = 788;
			this->Lines = 262159;
			this->Duration = 3091.0;
			this->scanningInterval = 20.0;
			this->processingTime="10-15 minutes";
			break;
		}
		case DIESELNET_TR:
		{
			this->tracename.assign("DieselNet");
			this->contactFilename = this->traceDirectory + "DieselNet.txt";
			this->presenceFilename = this->traceDirectory + "DieselNet-presence.txt";
			this->NN = 37;
			this->ActiveNodes = 37;
			this->Lines = 62107;
			this->Duration = 10635474.0;
			this->scanningInterval = 1.0;
			this->processingTime="20-25 seconds";
			break;
		}
		case DARTMOUTH_TR:
		{
			this->tracename.assign("Dartmouth");
			this->contactFilename = this->traceDirectory + "Dartmouth.txt";
			this->presenceFilename = this->traceDirectory + "Dartmouth-presence.txt";
			this->NN = 739;
			this->ActiveNodes = 739;
			this->Lines = 3808736;
			this->Duration = 10281543.0;
			this->scanningInterval = 1.0;
			this->processingTime="30-35 minutes";
			break;
		}
		case DARTMOUTH_2WEEKS_TR:
		{
			this->tracename.assign("Dartmouth2weeks");
			this->contactFilename = this->traceDirectory + "Dartmouth2weeks.txt";
			this->presenceFilename = this->traceDirectory + "Dartmouth2weeks-presence.txt";
			this->NN = 739;
			this->ActiveNodes = 738;
			this->Lines = 569116;
			this->Duration = 1209600.0;
			this->scanningInterval = 1.0;
			this->processingTime="30-35 minutes";
			break;
		}
		case DEBUG_TR:
		{
			this->tracename.assign("Debugging");
			this->contactFilename = this->traceDirectory + "Debugging.txt";
			this->presenceFilename = this->traceDirectory + "Debugging-presence.txt";
			this->NN = 13;
			this->ActiveNodes = 12;
			this->Lines = 82;
			this->Duration = 1800.0;
			this->scanningInterval = 5.0;
			this->processingTime="1 second";
			break;
		}
		case CUSTOM_TR:
		{
			this->tracename.assign("custom");
			this->contactFilename = "";
			this->presenceFilename = "";
			this->NN = 0;
			this->ActiveNodes = 0;
			this->Lines = 0;
			this->Duration = 0.0;
			this->scanningInterval = 0.0;
			this->processingTime="";
			break;
		}
		default:
		{
			printf("Error! Unknown contact trace identifier (%d)\nExiting...\n", TRC);
			exit(EXIT_FAILURE);
		}
	}

	return;
}

void Settings::setCustomTrcInfo(string trcIDname, string contactsFile, string presenceFile, int nodesNum, int actNodes, int contLines, double contDuration, double scan, string prTime, string originalFilename)
{
	tracename.assign(trcIDname);
	contactFilename.assign(contactsFile);
	presenceFilename.assign(presenceFile);
	NN = nodesNum;
	ActiveNodes = actNodes;
	Lines = contLines;
	Duration = contDuration;
	scanningInterval = scan;
	processingTime.assign(prTime);
	trcOriginalFilename.assign(originalFilename);
	return;
}

void Settings::setRT(int Rout)
{
	string profileAttribute;
	string suffix;


	this->RT = Rout;
	switch(this->RT)
	{
		case DIRECT_RT:
		{
			this->RTname.assign("Direct");
			break;
		}
		case EPIDEMIC_RT:
		{
			this->RTname.assign("Epidemic");
			break;
		}
		case PROPHET_RT:
		{
			this->RTname.assign("Prophet");
			break;
		}
		case SIMBET_RT:
		{
			this->RTname.assign("SimBet");
			break;
		}
		case SIMBETTS_RT:
		{
			this->RTname.assign("SimBetTS");
			break;
		}
		case BUBBLERAP_RT:
		{
			suffix = "";

			if(this->ProfileExists())
			{
				if((profileAttribute = this->GetProfileAttribute("kappa")) != "none")
				{
					suffix += ".kappa" + profileAttribute;
				}

				if((profileAttribute = this->GetProfileAttribute("familiarSetThreshold")) != "none")
				{
					suffix += ".familiarSetThreshold" + profileAttribute;
				}
			}

			this->RTname.assign("BubbleRap" + suffix);
			break;
		}
		case SW_RT:
		{
			this->RTname.assign("SprayWait");
			break;
		}
		case LSFSW_RT:
		{
			this->RTname.assign("LSF-Spray");
			break;
		}
		case MSFSW_RT:
		{
			this->RTname.assign("MSF-Spray");
			break;
		}
		case PROPHETSW_RT:
		{
			this->RTname.assign("Prophet-Spray");
			break;
		}
		case LSFSF_RT:
		{
			this->RTname.assign("LSF-SprayFocus");
			break;
		}
		case CNF_RT:
		{
			string UtilityType;
			if(this->ProfileExists() && (UtilityType=this->GetProfileAttribute("Utility")) != "none")
			{
				//remove all whitespaces
				UtilityType.erase( std::remove_if( UtilityType.begin(), UtilityType.end(), ::isspace ), UtilityType.end() );
				if(UtilityType == "LTS" || UtilityType == "DestEnc" || UtilityType == "Enc" || UtilityType == "AMT" || UtilityType == "AIT" || UtilityType == "SPM" || UtilityType == "Bet" || UtilityType == "Sim" || UtilityType == "LastContact" || UtilityType == "Prophet")
				{
					this->RTname.assign("CnF." + UtilityType);
				}
				else
				{
					this->RTname.assign("CnF.LTS");
				}
			}
			else
			{
				this->RTname.assign("CnF.LTS");
			}
			break;
		}
		case CNR_RT:
		{
			string UtilityType;
			if(this->ProfileExists() && (UtilityType=this->GetProfileAttribute("Utility")) != "none")
			{
				//remove all whitespaces
				UtilityType.erase( std::remove_if( UtilityType.begin(), UtilityType.end(), ::isspace ), UtilityType.end() );
				if(UtilityType == "LTS" || UtilityType == "DestEnc" || UtilityType == "Enc" || UtilityType == "AMT" || UtilityType == "AIT" || UtilityType == "SPM" || UtilityType == "Bet" || UtilityType == "Sim" || UtilityType == "LastContact" || UtilityType == "Prophet")
				{
					this->RTname.assign("CnR." + UtilityType);
				}
				else
				{
					this->RTname.assign("CnR.LTS");
				}
			}
			else
			{
				this->RTname.assign("CnR.LTS");
			}
			break;
		}
		case EBR_RT:
		{
			this->RTname.assign("EBR");
			break;
		}
		case DELEGATION_RT:
		{
			string UtilityType;
			if(this->ProfileExists() && (UtilityType=this->GetProfileAttribute("Utility")) != "none")
			{
				//remove all whitespaces
				UtilityType.erase( std::remove_if( UtilityType.begin(), UtilityType.end(), ::isspace ), UtilityType.end() );
				if(UtilityType == "LTS" || UtilityType == "DestEnc" || UtilityType == "Enc" || UtilityType == "AMT" || UtilityType == "AIT" || UtilityType == "SPM" || UtilityType == "Bet" || UtilityType == "Sim" || UtilityType == "SimBet" || UtilityType == "Prophet" || UtilityType == "LastContact" || UtilityType == "SimBetTS")
				{
					this->RTname.assign("Delegation." + UtilityType);
				}
				else
				{
					this->RTname.assign("Delegation.LTS");
				}
			}
			else
			{
				this->RTname.assign("Delegation.LTS");
			}
			break;
		}
		case COORD_RT:
		{
			string UtilityType;
			if(this->ProfileExists() && (UtilityType=this->GetProfileAttribute("Utility")) != "none")
			{
				//remove all whitespaces
				UtilityType.erase( std::remove_if( UtilityType.begin(), UtilityType.end(), ::isspace ), UtilityType.end() );
				if(UtilityType == "LTS" || UtilityType == "DestEnc" || UtilityType == "Enc" || UtilityType == "AMT" || UtilityType == "AIT" || UtilityType == "SPM" || UtilityType == "Bet" || UtilityType == "Sim" ||  UtilityType == "SimBet" || UtilityType == "Prophet" || UtilityType == "LastContact" || UtilityType == "Effi" || UtilityType == "SimBetTS")
				{
					this->RTname.assign("COORD." + UtilityType);
				}
				else
				{
					this->RTname.assign("COORD.LTS");
				}
			}
			else
			{
				this->RTname.assign("COORD.LTS");
			}
			break;
		}
		case OPTIMAL_RT:
		{
			if(this->ProfileExists())
			{
				if((profileAttribute = this->GetProfileAttribute("Optimal")) != "none")
				{
					if(profileAttribute == "Delay")
					{
						suffix = ".Delay";
					}
					else if(profileAttribute == "Forwards")
					{
						suffix = ".Forwards";
					}
					else
					{
						printf("\n[Error]: Unknown version of Optimal Routing (%s) \n\n", profileAttribute.c_str());
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					suffix = ".Delay";
				}
			}
			else
			{
				suffix = ".Delay";
			}

			this->RTname.assign("Optimal" + suffix);
			break;
		}
		default:
		{
			printf("Error! Unknown routing protocol identifier (%d)\nExiting...\n", Rout);
			exit(EXIT_FAILURE);
		}
	}

	return;
}


void Settings::setCongestionControl(int CC)
{
	string profileAttribute;
	string suffix;


	this->CongestionControlMechanism = CC;

	switch(this->CongestionControlMechanism)
	{
		case NOCC_CC:
		{
			this->CCname.assign("None");
			this->bufferInfoSupport = false;
			break;
		}
		case AVOIDOVERFLOW_CC:
		{
			this->CCname.assign("AvoidOverflow");
			this->bufferInfoSupport = true;
			break;
		}
		case ACC_CC:
		{
			this->CCname.assign("ACC");
			this->bufferInfoSupport = true;
			break;
		}
		case FAIRROUTE_CC:
		{
			this->CCname.assign("FairRoute");
			this->bufferInfoSupport = true;
			break;
		}
		case BSA_CC:
		{
			this->CCname.assign("BSA");
			this->bufferInfoSupport = true;
			break;
		}
		case CACC_CC:
		{
			this->CCname.assign("CACC");
			this->bufferInfoSupport = true;
			break;
		}
		case CCAF_CC:
		{
			if((profileAttribute = this->GetProfileAttribute("FixedDelta")) != "none")
			{
				suffix = ".FD=" + profileAttribute;
			}
			else if((profileAttribute = this->GetProfileAttribute("DeltaFunction")) != "none")
			{
				suffix = "." + profileAttribute;
			}
			else
			{
				suffix = ".Fam";
			}

			this->CCname.assign("CCAF" + suffix);
			this->bufferInfoSupport = true;
			break;
		}
		default:
		{
			printf("Error! Unknown congestion control identifier (%d)\nExiting...\n", CC);
			exit(EXIT_FAILURE);
		}
	}

	return;
}


void Settings::setSchedulingPolicy(int SP)
{
	this->SchedulingPolicy = SP;

	return;
}


void Settings::setDroppingPolicy(int DP)
{
	this->DropPol = DP;

	return;
}


void Settings::setDM(int deletionMechanism)
{
	this->DM = deletionMechanism;

	return;
}


void Settings::setOUT(int OUTval)
{
	this->outType = OUTval;

	return;
}


void Settings::setTrafficType(int type)
{
	this->TrafficType = type;

	return;
}


void Settings::setTrafficLoad(int pkts)
{
	this->TrafficLoad = pkts;

	return;
}


void Settings::setTTL(double time)
{
	this->TTL = time;

	return;
}


void Settings::setBuffer(int Buf)
{
	this->BufferSize = Buf;

	return;
}


void Settings::setReplicas(int Rep)
{
	this->Replicas = Rep;

	return;
}


void Settings::setSplit(long int spl)
{
	this->SplitValue = spl;

	return;
}


void Settings::setSeed(int S)
{
	this->Seed = S;

	return;
}


void Settings::setResultsDirectory(string resDir)
{
	if(resDir.back() != '/')
	{
		this->resultsDirectory = resDir + "/";
	}
	else
	{
		this->resultsDirectory = resDir;
	}

	return;
}


void Settings::setBatchmode(bool val)
{
	this->Batchmode = val;

	return;
}


int Settings::askToImportTrace()
{
	int status = 1;
	string decision = "";


	do
	{
		printf("The estimated processing time of the %s contact trace is %s\n", tracename.c_str(), processingTime.c_str());
		printf("Import the %s contact trace now? [y/n]: ", tracename.c_str());
		cin >> decision;
	}
	while((!cin.fail() && decision.at(0) != 'y' && decision.at(0) != 'n') || (decision.length() > 1));

	if(decision.at(0) == 'y')
	{
		status = system(("cd ../utils/trace-processing && chmod +x ./import-dataset.sh && ./import-dataset.sh " + tracename).c_str());
	}
	else
	{
		printf("\nYou can import the %s contact trace any time by executing the following commands:\n", tracename.c_str());
		printf("$ cd ../utils/trace-processing\n");
		printf("$ ./import-dataset.sh %s\n\n", tracename.c_str());
	}

	return status;
}


void Settings::lastCheck()
{
	/* Make sure that the requested contact trace has been imported */
	if((access(contactFilename.c_str(), F_OK) != 0) || (access(presenceFilename.c_str(), F_OK) != 0))
	{
		if(ContactTrace == CUSTOM_TR)
		{
			printf("[Error] The \"%s\" contact trace cannot be imported in the simulator\n", tracename.c_str());
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("[Error] The \"%s\" contact trace has not been imported in the simulator\n", tracename.c_str());

			if(askToImportTrace() != 0)
			{
				exit(EXIT_FAILURE);
			}
			else
			{
				printf("\n");
			}
		}
	}


	/* Check for combinations that are not supported */
	if((this->BufferSize == 0) && (this->CongestionControlMechanism != NOCC_CC))
	{
		if(!(this->GUI) && !(this->Batchmode))
		{
			printf("[Warning]: Cannot support a congestion control mechanism if the capacity of the buffer is infinite\n\n");
		}

		this->CongestionControlMechanism = NOCC_CC;
		this->bufferInfoSupport = false;
	}

	if((this->isSingleCopy()) && (this->DM != JUSTTTL_DM))
	{
		if(!(this->GUI) && !(this->Batchmode))
		{
			printf("[Warning]: Single-copy routing protocols do not need any deletion mechanism\n\n");
		}

		this->DM = JUSTTTL_DM;
	}

	if(this->RT == OPTIMAL_RT)
	{
		if(this->BufferSize > 0)
		{
			if(!(this->GUI) && !(this->Batchmode))
			{
				printf("[Error]: The current implementation of Optimal Routing works properly only with buffers of infinite size\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else if(!((this->outType == T1_OUT) || (this->outType == T2_OUT)))
		{
			if(!(this->GUI) && !(this->Batchmode))
			{
				printf("[Error]: The current implementation of Optimal Routing provides proper statistics only for the output types 1 and 2\n\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if(this->DM != JUSTTTL_DM)
			{
				if(!(this->GUI) && !(this->Batchmode))
				{
					printf("[Warning]: Optimal Routing does not need any deletion mechanism\n\n");
				}

				this->DM = JUSTTTL_DM;
			}
		}
	}

	//Check congestion control support
	if(!(this->CongestionControlMechanism == NOCC_CC) && !((this->RT == SIMBET_RT) || (this->RT == PROPHET_RT) || (this->RT == SIMBETTS_RT)))
	{
		printf("Error: Currently only PRoPHET, SimBet, and SimBetTS support the congestion control mechanisms!\nExiting...\n");
		exit(1);
	}

	//Check scheduling support
	if(!((this->SchedulingPolicy == FIFO_SP) || (this->SchedulingPolicy == LIFO_SP)) && !((this->RT == SIMBET_RT) || (this->RT == PROPHET_RT) || (this->RT == SIMBETTS_RT)))
	{
		printf("Error: Currently only PRoPHET, SimBet, and SimBetTS support the GRTRMax, GRTRSort, and HNUV scheduling policies!\nExiting...\n");
		exit(1);
	}

	return;
}


void Settings::printSettings()
{
	printf("Contact Trace: %s\n", this->tracename.c_str());
	if(ContactTrace == CUSTOM_TR)
	{
// 		printf("Contact Filename: \"%s\" (%ld lines) [extracted from %s]\n", contactFilename.c_str(), Lines, trcOriginalFilename.c_str());
// 		printf("Presence Filename: \"%s\" [extracted from %s]\n", presenceFilename.c_str(),trcOriginalFilename.c_str());
		printf("Contact Filename: \"%s\" (contacts: %ld lines)\n", trcOriginalFilename.c_str(), Lines);
	}
	else
	{
		printf("Contact Filename: \"%s\" (%ld lines)\n", this->contactFilename.c_str(), this->Lines);
		printf("Presence Filename: \"%s\"\n", this->presenceFilename.c_str());
	}
	printf("Total Number of Nodes: %d\n", this->NN);
	printf("Total Number of Active Nodes: %d\n", this->ActiveNodes);
	printf("Duration: %f days\n", this->Duration / 86400.0);
	printf("Routing Protocol: %s\n", this->RTname.c_str());
	printf("Replicas: %d\n", this->Replicas);
	printf("Congestion Control Mechanism: %s\n", this->CCname.c_str());

	printf("Buffer Size: ");
	if(this->BufferSize == 0)
	{
		printf("Infinite\n");
	}
	else
	{
		printf("%d packets\n", this->BufferSize);
	}

	if(this->BufferSize > 0)
	{
		printf("Scheduling Policy: ");
		printSchedulingPolicy(this->SchedulingPolicy);

		printf("Dropping Policy: ");
		printDroppingPolicy(this->DropPol);
	}

	printf("Deletion Mechanism: ");
	printDeletionMechanism(this->DM);

	printf("TTL: ");
	if(this->TTL == 0)
	{
		printf("Infinite\n");
	}
	else
	{
		printf("%f seconds\n", this->TTL);
	}
	
	printf("Bandwidth: Infinite\n");

	printf("Output: ");
	printOutputType(this->outType);

	printf("Traffic Type: ");
	printTrafficType(this->TrafficType);

	printf("Traffic Load: %d packets\n", this->TrafficLoad);

	printf("Seed: %d\n",this->Seed);

	if(this->ProfileExists())
	{
		printf("Profile: Loaded %zu attributes from \"%s\"\n", this->ProfileAttributes.size(), this->ProfileName.c_str());
		this->PrintProfile();
	}
	else
	{
		printf("Profile: None\n");
	}

	printf("Results Directory: \"%s\"\n", this->resultsDirectory.c_str());


	if(this->SplitValue != -1)
	{
		printf("The contact file will be uploaded into memory in %ld pieces\n", this->SplitValue);
	}

	printf("\n");

	return;
}


string *Settings::getContactFilename()
{
	return &this->contactFilename;
}


string Settings::getPresenceFilename()
{
	return this->presenceFilename;
}


bool Settings::ProfileExists(void)
{
	if(this->ProfileName == "none")
	{
		return false;
	}
	return true;
}


void Settings::printSchedulingPolicy(int type)
{
	switch(type)
	{
		case FIFO_SP:
		{
			printf("FIFO\n");
			break;
		}
		case LIFO_SP:
		{
			printf("LIFO\n");
			break;
		}
		case GRTRMAX_SP:
		{
			printf("GRTRMax\n");
			break;
		}
		case GRTRSORT_SP:
		{
			printf("GRTRSort\n");
			break;
		}
		case HNUV_SP:
		{
			printf("HNUV\n");
			break;
		}
		default:
		{
			printf("%d (Unknown)\n", type);
			break;
		}
	}
		
	return;
}


void Settings::printDroppingPolicy(int type)
{
	switch(type)
	{
		case DF_DP:
		{
			printf("Drop Front\n");
			break;
		}
		case DT_DP:
		{
			printf("Drop Tail\n");
			break;
		}
		case DO_DP:
		{
			printf("Drop Oldest\n");
			break;
		}
		case DY_DP:
		{
			printf("Drop Youngest\n");
			break;
		}
		case DFASP_DP:
		{
			printf("Drop Front - Avoid Source Packets\n");
			break;
		}
		case DTASP_DP:
		{
			printf("Drop Tail - Avoid Source Packets\n");
			break;
		}
		case DOASP_DP:
		{
			printf("Drop Oldest - Avoid Source Packets\n");
			break;
		}
		case DYASP_DP:
		{
			printf("Drop Youngest - Avoid Source Packets\n");
			break;
		}
		default:
		{
			printf("%d (Unknown)\n", type);
			break;
		}
	}
	return;
}


void Settings::printDeletionMechanism(int type)
{
	switch(type)
	{
		case JUSTTTL_DM:
		{
			printf("Just TTL\n");
			break;
		}
		case NODUPLICATES_DM:
		{
			printf("No Duplicates - Delete Replicas\n");
			break;
		}
		case NODUPSKEEPRELICATION_DM:
		{
			printf("No Duplicates - Keep Replicating\n");
			break;
		}
		case VACCINE_DM:
		{
			printf("Vaccine\n");
			break;
		}
		case CATACLYSM_DM:
		{
			printf("Cataclysm\n");
			break;
		}
		default:
		{
			printf("Unknown (%d)\n", type);
			break;
		}
	}
	return;
}


void Settings::printOutputType(int type)
{
	switch(type)
	{
		case T1_OUT:
		{
			printf("Type 1\n");
			break;
		}
		case T2_OUT:
		{
			printf("Type 2\n");
			break;
		}
		case T3_OUT:
		{
			printf("Type 3\n");
			break;
		}
		case T4_OUT:
		{
			printf("Type 4\n");
			break;
		}
		case T5_OUT:
		{
			printf("Type 5\n");
			break;
		}
		default:
		{
			printf("%d (Unknown)\n", type);
			break;
		}
	}
	return;
}


void Settings::printTrafficType(int type)
{
	switch(type)
	{
		case UNIFORM_TT:
		{
			printf("Uniform\n");
			break;
		}
		case BURST_TT:
		{
			printf("Burst\n");
			break;
		}
		case SAMPLE_TT:
		{
			printf("Sample\n");
			break;
		}
		case PREDEFINED_TT:
		{
			printf("Predefined\n");
			break;
		}
		default:
		{
			printf("%d (Unknown)\n", type);
			break;
		}
	}
	return;
}


bool Settings::isSingleCopy(void)
{
	switch(this->RT)
	{
		case DIRECT_RT:
		{
			return true;
		}
		case EPIDEMIC_RT:
		{
			return false;
		}
		case PROPHET_RT:
		{
			return true;
		}
		case SIMBET_RT:
		{
			return true;
		}
		case SIMBETTS_RT:
		{
			//this protocol when using 1 replica should be treated as single-copy
			if(this->Replicas == 1)
			{
				return true;
			}
			return false;
		}
		case BUBBLERAP_RT:
		{
			return this->copyMode;
		}
		case SW_RT:
		{
			return false;
		}
		case LSFSW_RT:
		{
			return false;
		}
		case MSFSW_RT:
		{
			return false;
		}
		case PROPHETSW_RT:
		{
			return false;
		}
		case LSFSF_RT:
		{
			return false;
		}
		case CNF_RT:
		{
			return true;
		}
		case CNR_RT:
		{
			return false;
		}
		case EBR_RT:
		{
			return false;
		}
		case DELEGATION_RT:
		{
			return false;
		}
		case COORD_RT:
		{
			return false;
		}
		case OPTIMAL_RT:
		{
			return false;
		}
		default:
		{
			printf("Error! Unknown routing protocol identifier (%d)\nExiting...\n", this->RT);
			exit(EXIT_FAILURE);
		}
	}
}


bool Settings::usesLimitedReplication(void)
{
	switch(this->RT)
	{
		case DIRECT_RT:
		{
			return false;
		}
		case EPIDEMIC_RT:
		{
			return false;
		}
		case PROPHET_RT:
		{
			return false;
		}
		case SIMBET_RT:
		{
			return false;
		}
		case SIMBETTS_RT:
		{
			return true;
		}
		case BUBBLERAP_RT:
		{
			return false;
		}
		case SW_RT:
		{
			return true;
		}
		case LSFSW_RT:
		{
			return true;
		}
		case MSFSW_RT:
		{
			return true;
		}
		case PROPHETSW_RT:
		{
			return true;
		}
		case LSFSF_RT:
		{
			return true;
		}
		case CNF_RT:
		{
			return false;
		}
		case CNR_RT:
		{
			return false;
		}
		case EBR_RT:
		{
			return true;
		}
		case DELEGATION_RT:
		{
			return false;
		}
		case COORD_RT:
		{
			if(this->Replicas == 1)
			{
				return false;
			}
			return true;
		}
		case OPTIMAL_RT:
		{
			return false;
		}
		default:
		{
			printf("Error! Unknown routing protocol identifier (%d)\nExiting...\n", this->RT);
			exit(EXIT_FAILURE);
		}
	}
}


void Settings::LoadProfile(string Fname)
{
	/* Future extensions: Ignoring comments, trim empty lines, check for duplicates, empty values
	Format: Two columns (All other columns are ignored)
	First column: Attribute name
	Second column: Attribute values */
	
	//Set values
	this->ProfileName=Fname;
	
	FILE *fp=NULL;
	
	if ((fp=fopen(Fname.c_str(), "r")) == NULL)
	{
		printf("Error reading routing protocol profile(%s)!\n",Fname.c_str());
		exit(1);
	}
	
	//Load Data
	char line[150];
	int LineNumber=1;
	char *tmp;
	ProfileAttribute tempATR;
	while(fgets(line,150,fp) != NULL)
	{
		//printf("%d. %s",LineNumber,line);
		tmp = strtok(line,"\t\n ");
		//tempATR=(ProfileAttribute *)malloc(sizeof(ProfileAttribute));
		int j=1;
		while(tmp != NULL)
		{
			//printf("tmp:%s\n",tmp);
			if(j == 1)
			{
				tempATR.Attribute=tmp;
			}
			else if(j == 2)
			{
				tempATR.Value=tmp;
			}
			tmp = strtok(NULL,"\t\n ");
			j++;
		}
		//Check if the line is blank
		if(j == 1)
		{
			LineNumber++;
			continue;
		}
		else if(j <= 2)
		{
			//Check how many columns exist in profile
			printf("Error - Line %d: One or more attributes inside profile (%s) haven't got a value!\nExiting..\n",LineNumber,Fname.c_str());
			exit(1);
		}
		this->ProfileAttributes.push_back(tempATR);
		LineNumber++;
	}
	
	if(fclose(fp)==EOF)
	{
		printf("ERROR: could not close file %s",Fname.c_str());
		exit(1);
	}
	return;
}


string Settings::GetProfileAttribute(string AttributeName)
{
	string val="none";
	vector<ProfileAttribute>::iterator it;
	
	for(it = this->ProfileAttributes.begin() ; it != this->ProfileAttributes.end(); ++it)
	{
		if(it->Attribute == AttributeName)
		{
			val=it->Value;
		}
	}
	return val;
}


void Settings::PrintProfile(void)
{
	printf("Profile Attributes: ");
	vector<ProfileAttribute>::iterator it;
	
	
	for(it = this->ProfileAttributes.begin() ; it != this->ProfileAttributes.end(); ++it)
	{
		printf("%s %s\t",it->Attribute.c_str(),it->Value.c_str());
	}
	printf("\n");
	return;
}
