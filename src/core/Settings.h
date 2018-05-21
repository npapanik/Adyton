/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015-2018  Nikolaos Papanikos,
 *  Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou
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


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>

#ifdef __linux__ 
	#include <unistd.h>
#elif _WIN32
	#define F_OK    0 
	#include <io.h>
#endif

#ifndef IDENTIFICATION_H
	#define IDENTIFICATION_H
	#include "Identification.h"
#endif



using namespace std;

struct ProfileAttribute
{
	string Attribute;
	string Value;
};


class Settings
{
protected:
	string traceDirectory;
	string trcOriginalFilename;
	int ContactTrace;
	int RT;
	int CongestionControlMechanism;
	int SchedulingPolicy;
	int DropPol;
	int DM;
	int outType;
	int TrafficType;
	int TrafficLoad;
	double TTL;
	int BufferSize;
	int Replicas;
	long int SplitValue;
	int Seed;
	string resultsDirectory;
	bool Batchmode;

	string ProfileName; /* profile filename + path */
	vector<ProfileAttribute> ProfileAttributes; /* Holds the names of the extra attributes (for the protocol) */
	vector<string> ProfileValues; /* Holds the values of the extra attributes (for the protocol)*/

	bool GUI;
	bool copyMode;//single(true) or multi-copy(false)

	int NN;
	int ActiveNodes;
	string tracename;
	string contactFilename;
	string presenceFilename;
	long int Lines;
	double Duration;
	double scanningInterval;

	string RTname;

	string CCname;
	bool bufferInfoSupport;
	string processingTime;
	
	int askToImportTrace();

public:
	Settings();
	~Settings();

	void setTraceDirectory(string trcDir);
	void setContactTrace(int TRC);
	void setRT(int Rout);
	void setCongestionControl(int CC);
	void setSchedulingPolicy(int SP);
	void setDroppingPolicy(int DP);
	void setDM(int deletionMechanism);
	void setOUT(int OUTval);
	void setTrafficType(int type);
	void setTrafficLoad(int pkts);
	void setTTL(double time);
	void setBuffer(int Buf);
	void setReplicas(int Rep);
	void setSplit(long int spl);
	void setSeed(int S);
	void setResultsDirectory(string resDir);
	void setBatchmode(bool val);
	void setCustomTrcInfo(string trcIDname, string contactsFile, string presenceFile, int nodesNum, int actNodes, int contLines, double contDuration, double scan, string prTime, string originalFilename);
	void lastCheck();

	void setGUI(bool status){this->GUI=status;};
	void setCopyMode(bool val){this->copyMode=val;};

	int getContactTrace(){return this->ContactTrace;};
	int getRT(){return this->RT;};
	int getCongestionControl(){return this->CongestionControlMechanism;};
	int getSchedulingPolicy(){return this->SchedulingPolicy;};
	int getDroppingPolicy(){return this->DropPol;};
	int getDM(){return this->DM;};
	int getOUT(){return this->outType;};
	int getTrafficType(){return this->TrafficType;};
	int getTrafficLoad(){return this->TrafficLoad;};
	double getTTL(){return this->TTL;};
	int getBuffer(){return this->BufferSize;};
	int getReplicas(){return this->Replicas;};
	long int getSplit(){return this->SplitValue;};
	int getSeed(){return this->Seed;};
	string getProfileName(){return this->ProfileName;};
	string getResultsDirectory(){return this->resultsDirectory;};
	string getTraceDirectory(){return this->traceDirectory;};
	bool getBatchmode(){return this->Batchmode;};

	void printSettings();

	int getNN(void){return NN;};
	int getActiveNodes(void){return ActiveNodes;}
	string *getContactFilename();
	string getPresenceFilename();
	int getLines(void){return Lines;};
	string *getTraceName(void){return &this->tracename;};
	string *getRTname(void){return &this->RTname;};
	string *getCCname(void){return &this->CCname;};
	void printSchedulingPolicy(int type);
	void printDroppingPolicy(int type);
	void printDeletionMechanism(int type);
	void printOutputType(int type);
	void printTrafficType(int type);
	void LoadProfile(string Fname); /* this method reads the file contents and fills the ProfileAttributes table */
	string GetProfileAttribute(string AttributeName); /* this method returns the value of a ProfileAttribute - if the attribute does not exist then NULL is returned */
	void PrintProfile(void);
	bool ProfileExists(void);
	double getTraceDuration(void){return this->Duration;};
	double getScanningInterval(void){return this->scanningInterval;};
	bool isSingleCopy(void);
	bool usesLimitedReplication(void);
	bool needsBufferInformation(void){return this->bufferInfoSupport;};
	bool GUIon(void){return this->GUI;};
	bool getCopyMode(void){return this->copyMode;};
};
