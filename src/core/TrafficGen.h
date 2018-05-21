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
#include <fstream>

#ifndef EVENT_H
	#define EVENT_H
	#include "Event.h"
#endif

#ifndef EVENT_LIST_H
	#define EVENT_LIST_H
	#include "EventList.h"
#endif

#ifndef IDENTIFICATION_H
	#define IDENTIFICATION_H
	#include "Identification.h"
#endif

using namespace std;


struct Nodepair
{
	int source;
	int dest;
};
typedef struct Nodepair trafficPair;


class TrafficGen
{
protected:
	int TotalNodes;
	double traceDuration;
	double SndPrdStart;
	double SndPrdStop;
	EventList *Sim;
	int totalStatPkts;
	vector<int> Inactive;
	double *inPresence;
	double *outPresence;
	string presenceFilename;

public:
	TrafficGen(int Seed, int Total, double duration, string presFile, EventList *SimL);
	~TrafficGen(void);
	int *generateTraffic(int trafficType, int NumPackets);
	int *CreateUniformTraffic(int NumPackets);
	int *CreateBurstTraffic(int NumPackets);
	int *CreateSampleTraffic(int NumPackets);
	int *CreatePredefinedTraffic();
	void SetTransPeriod(double SendPeriodStrt,double SendPeriodStop);
	int getTotalStatPkts(void){return totalStatPkts;};
	void FillPresenceList(string Path);
	bool ArePresent(int A, int B,double Time);
	bool isInactive(int nodeID);
	int *getAllPresentNodes(double creationTime);
};
