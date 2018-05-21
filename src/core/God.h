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


#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <vector>
#include <list>

#ifndef CONNECTIONMAP_H
	#define CONNECTIONMAP_H
	#include "ConnectionMap.h"
#endif

#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "Settings.h"
#endif


class Node;
using namespace std;

struct CustodianHighestUtil
{
	int PID;
	int Node;
	double Util;
};
typedef CustodianHighestUtil CHUtil;


struct PacketStatistics
{
	int pktID;
	int srcID;
	int dstID;
	double genTime;
	int numForwards;
	int numHops;
	double delTime;
	bool reachedDst;

	bool operator < (const PacketStatistics& p2) const
	{
		if(pktID < p2.pktID)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};



class God
{
private:
	Settings *Set;
	int SimNodes;
	double CurTime;

	set<int> DeliveredPkts;

	ConnectionMap *ActiveConnections;
	vector<Node*> *SimulationNodes;
	vector<CHUtil*> HCustodians;

	int **NodeFriends;

	list<PacketStatistics> pktStats;

	int EncodingsTypeA;
	int EncodingsTypeB;
	int EncodedTransmissions;

	string outputFilename;
	FILE *outFile;
public:
	God(Settings *S);
	~God();
	void setActiveConnections(ConnectionMap *Map);
	void setSimulationNodes(vector<Node*> *N);
	void setSimTime(double SimTime);
	double getSimTime(void);
	void InitDelivered(int NumberOfPackets);
	void AddDelivered(int pktID);
	bool IsDelivered(int pktID);
	int getAlternativeCustodian(int congestedNode);
	int getNumForwards(int nodeID);
	int getSimilarity(int nodeID, int destID);
	int getEgoNetworkSize(int nodeID);
	int getSimilarityRank(int nodeID, int destID);
	double getProjNetGrowth(int nodeID, double interval);
	double getMeanRTTL(int nodeID, double interval);

	void increaseEncodingsA(void){EncodingsTypeA++;};
	void increaseEncodingsB(void){EncodingsTypeB++;};
	void increaseCodedTransmissions(void){EncodedTransmissions++;};
	void printCodingInfo(void);

	//Custodian methods
	void updateCustodian(int PID,int NID,double Util);
	double getHighestUtil(int PID);

	void setFriends(int NID,int *friends);
	int *getFriends(int NID);

	void generatedPkt(int pktID, int srcID, int dstID, int genTime);
	void forwardedPkt(int pktID);
	void deliveredPkt(int pktID, int numHops, double delTime);
	void updatePktStats(int pktID, int numHops, double delTime);
	int getNumHops(int pktID);
	double getDelTime(int pktID);

	void writePktStats(string fileName);
	void writeString(string outputData);

	void deleteAllReplicas(int pktID);
	bool optimizeDelay();
	bool optimizeForwards();
};

