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


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <boost/lexical_cast.hpp>

#ifndef STATS_H
	#define STATS_H
	#include "Statistics.h"
#endif

#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "Settings.h"
#endif

#ifndef TRAFFICGEN_H
	#define TRAFFICGEN_H
	#include "TrafficGen.h"
#endif

#ifndef NODE_H
	#define NODE_H
	#include "Node.h"
#endif


class Results
{
protected:
	vector<Statistics*> *Stats;
	vector<Node*> *Nodes;
	Settings *Set;
	God *SimGod;
	TrafficGen *Traffic;
	double LastUpdateDR;
	double LastUpdateFW;
	double LastUpdateDrops;
	double LastUpdateBO;
	double LastValDR;
	double LastValFW;
	double LastValDrops;
	double LastValBO;
	double LastTimeUpdated;
	FILE *FPTimeDR;
	FILE *FPTimeFW;
	FILE *FPTimeDrops;
	FILE *FPTimeBO;
	char NameTimeDR[150];
	char NameTimeFW[150];
	char NameTimeDrops[150];
	char NameTimeBO[150];
	bool CommitDR;
	bool CommitFW;
	bool CommitDrops;
	bool CommitBO;
	unsigned long long int simID;

public:
	Results(vector<Statistics*> *St, vector<Node*> *N, Settings* Setng, God *SGod, TrafficGen *TrfGen);
	void writeSimulationSettings();

	void writeSimulationResults();
	void writeBasicResults();
	void writeDetailedResults();
	void writePacketAnalysis();
	void writeFairnessIndices();
	void writeDistributions();

	void printBasicRes(bool writeToFile);
	void printSynopsis();
	void printStatsPerNode();

	void openFiles();
	void closeFiles();
	void initAll();
	void initDR();
	void initFW();
	void initDrops();
	void initBO();
	void updateAll(double currTime);
	void updateDR(double currTime);
	void updateFW(double currTime);
	void updateDrops(double currTime);
	void updateBO(double currTime);
	void updateTime(double currTime);
	void finalizeAll();
	void finalizeDR();
	void finalizeFW();
	void finalizeDrops();
	void finalizeBO();
};
