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
 *  Written by Nikolaos Papanikos.
 */


#include <stdio.h>
#include <stdlib.h>

#ifndef PACKET_BUFFER_H
	#define PACKET_BUFFER_H
	#include "../core/PacketBuffer.h"
#endif

#ifndef PACKET_ENTRY_H
	#define PACKET_ENTRY_H
	#include "PacketEntry.h"
#endif

#ifndef GOD_H
	#define GOD_H
	#include "../core/God.h"
#endif

struct Opportunity
{
	int PID1;
	int PID2;
	struct Opportunity *next;
};
typedef struct Opportunity Opportunities;

class Coding
{
private:
	PacketBuffer *NodeBuf;
	int NID;
	God *SimGod;
public:
	Coding(int NodeID,PacketBuffer *Buf,God *gd);
	int *cleanPackets(int *Selected);
	bool checkOpportunity(int PID1,int PID2);
	bool checkFriendlyOpportunity(int PID1,int PID2);
	void deleteOpportunities(Opportunities *Opps);
	Opportunities *searchCodingOpportunities(int *Candidates);
	Opportunities *searchFriendlyCodingOpportunities(int *Candidates);
	Opportunities *forceCoding(int *Candidates,int *NotRequested);
	Opportunities *forceFriendlyCoding(int *Candidates, int *NoPriority);
	void printOpportunities(Opportunities *Opps);
	//xCnR
	int *getMyPackets(int *Selected);
	int *getAlreadyEncoded(int *Selected);
	int *getRejected(int *Req,int *Chosen,int *owned);
	int *excludePairs(int *Selected);
	bool removePID(int *Selected,int ID);
	//xDF
	int *getPktsBetterFW(int *req, double *Utils, bool *owned);
	int *getPktsNotBetterFW(int *req, double *Utils, bool *owned);
	int *getPktsForActivation(int *req, double *Utils, bool *owned, int *isExtra);
	int *getMyPackets(int *req, double *Utils, bool *owned);
	int *getAlreadyEncoded(int *req, double *Utils, bool *owned);
	//xSpray
	int *getPktsBetterFW(int *req, bool *isExtra);
	int *getPktsNotBetterFW(int *req, bool *isExtra);
	int *getMyPackets(int *req, bool *isExtra);
	int *getAlreadyEncoded(int *req, bool *isExtra);
	int *getPktsForActivation(int *req, bool *isExtra);
	//Compacting buffer
	int *getCodingCandidates(void);
};
