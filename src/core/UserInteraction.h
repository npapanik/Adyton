/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <map>


#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "Settings.h"
#endif

using namespace std;

class UserInteraction
{
private:
	Settings *Set;

	string profileName;		/* filename (and path) of the file that contains extra attributes */
	string resDir;			/* name of the directory that will store the results of the simulation */
	string trcDir;			/* name of the directory that stores the contact trace files. */
	bool includedProfile;
	bool modifiedResDir;
	bool modifiedTrcDir;

	int TRC;			/* contact trace */
	int RT;				/* routing protocol */
	int CC;				/* congestion control mechanism */
	int SP;				/* scheduling policy */
	int DP;				/* dropping policy */
	int DM;				/* deletion mechanism */
	int OUT;			/* output type */
	int TT;				/* traffic type */
	int NP;				/* traffic load (number of packets) */
	double TTL;			/* time to live of each packet */
	int BUF;			/* buffer size */
	int REP;			/* replication number */
	long int SPLIT;		/* number of pieces to split the contact trace file */
	int SEED;			/* seed number */

	bool BTCH;			/* if true avoid printing results */
	bool GUI;

	map<string,int> Tracenames;
	map<string,int> RTnames;
	map<string,int> CongestionControlnames;
	map<string,int> Schedulingnames;
	map<string,int> DPnames;
	map<string,int> DMnames;
	map<string,int> OutputTypenames;
	map<string,int> TrafficTypenames;

public:
	UserInteraction();
	~UserInteraction();

	Settings *getSettings(int argc, char *argv[]);

	void ParseArgs(char *com, char *option, char *value);
	bool isNumber(const string& s);

	int convertTraceToID(const string& s);
	int convertProtoToID(const string& s);
	int convertCongestionControlToID(const string& s);
	int convertSchedulingToID(const string& s);
	int convertDPToID(const string& s);
	int convertDMToID(const string& s);
	int convertOutputTypeToID(const string& s);
	int convertTrafficTypeToID(const string& s);

	void initTracenames();
	void initRTnames();
	void initCongestionControlnames();
	void initSchedulingnames();
	void initDPnames();
	void initDMnames();
	void initOutputTypenames();
	void initTrafficTypenames();

	void printLicence();
	void printDetails(string param);
};
