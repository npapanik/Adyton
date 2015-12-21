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
 *  Written by Dimitrios-Georgios Akestoridis.
 */


#ifndef LIFO_H
	#define LIFO_H
	#include "LIFO.h"
#endif


LIFO::LIFO(int NID, PacketBuffer* Bf): SchedulingPolicy(NID, Bf)
{
	return;
}


LIFO::~LIFO()
{
	schedulingPackets.clear();

	return;
}


void LIFO::addPacket(int PID, void *additionalInfo)
{
	LIFOPacket newPacket;

	/* LIFO does not need any additional information */
	if(additionalInfo)
	{
		free(additionalInfo);
	}


	/* obtain the packet ID and the distance from the end of the buffer */
	newPacket.packetID = PID;
	newPacket.distanceFromEnd = Buff->getDistanceFromEnd(newPacket.packetID);


	/* sanity check */
	if(newPacket.distanceFromEnd == -1)
	{
		printf("\n[Error]: (LIFO::addPacket) The packet with ID %d does not exist inside the packet buffer of node %d\n\n", PID, nodeID);
		exit(EXIT_FAILURE);
	}


	/* add the new packet in the vector of scheduling packets */
	schedulingPackets.push_back(newPacket);

	return;
}


int *LIFO::getOutgoingPackets()
{
	int *pktList;
	int counter;
	vector<LIFOPacket>::reverse_iterator rit;


	if(schedulingPackets.size() == 0)
	{
		return NULL;
	}

	sort(schedulingPackets.begin(), schedulingPackets.end());

	pktList = (int *) malloc(sizeof(int) * (schedulingPackets.size() + 1));
	pktList[0] = (int) schedulingPackets.size();

	counter = 1;
	for(rit = schedulingPackets.rbegin(); rit != schedulingPackets.rend(); ++rit)
	{
		pktList[counter] = (*rit).packetID;
		counter++;
	}

	schedulingPackets.clear();

	return pktList;
}
