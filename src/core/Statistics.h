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


#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#ifndef PACKETPOOL_H
	#define PACKETPOOL_H
	#include "PacketPool.h"
#endif

#ifndef GOD_H
	#define GOD_H
	#include "God.h"
#endif

#ifndef IDENTIFICATION_H
	#define IDENTIFICATION_H
	#include "Identification.h"
#endif


class Statistics
{
protected:
	int me;
	double sumHops;
	double sumDelay;
	int Duplicates;
	int PktsForMe;
	int ReceivedForMe;

	int *ReceptionList;
	int currentSlot;//pos in reception list

	PacketPool *Pool;
	int Forwards;

	int ReplicasCreated;

	int trafficType;

	unsigned int timesAsRelayNode;
	unsigned int handovers;

	double sampleStartTime;
	double sampleEndTime;
	double traceDuration;

	unsigned int pktsDropped;
	unsigned int srcPktsDropped;
	unsigned int relPktsDropped;
	unsigned int pktsDied;

	God *SimGod;

public:
	Statistics(PacketPool *P, int ID, double duration, int trafType, God *G);
	~Statistics();
	void PrintResults(void);
	void SetPktsForMe(int pkts);

	double getAvgDelay(void);
	double getAvgDR(void);
	double getAvgHops(void);
	double getDelay(void){return this->sumDelay;};
	double getHops(void){return this->sumHops;};
	int getTotalDelivered(void){return this->ReceivedForMe;};
	int getIdealDelivered(void){return this->PktsForMe;};
	int getForwards(void){return this->Forwards;};
	double getAvgFw(void);
	int getTransmissions(void){return this->Forwards;};
	int getDuplicates(void){return this->Duplicates;};
	int getRepCreated(void){return this->ReplicasCreated;};
	unsigned int getTimesAsRelayNode(void){return(timesAsRelayNode);};
	unsigned int getHandovers(void){return(handovers);};
	unsigned int getPktsDropped(void){return(pktsDropped);};
	unsigned int getSrcPktsDropped(void){return(srcPktsDropped);};
	unsigned int getRelPktsDropped(void){return(relPktsDropped);};
	unsigned int getPktsDied(void){return(pktsDied);};
	int getcurrentSlot(void){return this->currentSlot;};

	void pktRec(int hops, double del, Packet *p, double pktCreationTime, bool processDuplicates);
	void pktGen(int pktID, int srcID, int dstID, int genTime);
	void incForwards(int pktID, double pktCreationTime);
	void incDuplicates(double pktCreationTime);
	void incReps(double pktCreationTime);
	void incTimesAsRelayNode(double pktCreationTime);
	void incHandovers(double pktCreationTime);
	void incPktsDropped(double pktCreationTime);
	void incPktsDied(double pktCreationTime);
	void incSrcPktsDropped(double pktCreationTime);
	void incRelPktsDropped(double pktCreationTime);

	void updateStats(int PID, int hops, double delay);
	bool isBackgroundTraffic(double pktCreationTime);
};
