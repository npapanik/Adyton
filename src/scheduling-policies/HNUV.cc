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


#ifndef HNUV_H
	#define HNUV_H
	#include "HNUV.h"
#endif


HNUV::HNUV(int NID, PacketBuffer* Bf): SchedulingPolicy(NID, Bf)
{
	return;
}


HNUV::~HNUV()
{
	schedulingPackets.clear();

	return;
}


void HNUV::addPacket(int PID, void *additionalInfo)
{
	HNUVPacket newPacket;


	/* HNUV needs each node's utility value for the packet */
	if(!additionalInfo)
	{
		printf("\n[Error]: (HNUV::addPacket) Each node's utility value for the packet with ID %d were not given\n\n", PID);
		exit(EXIT_FAILURE);
	}


	/* obtain the packet ID, the distance from the beginning of the buffer, and the difference between utility values of the nodes for the packet */
	newPacket.packetID = PID;
	newPacket.distanceFromBegin = Buff->getDistanceFromBegin(newPacket.packetID);
	if((((ExchangedInformation *) additionalInfo)->recUtil + ((ExchangedInformation *) additionalInfo)->senUtil) == 0)
	{
		printf("\n[Error]: (HNUV::addPacket) Both node's utility value is equal to zero for the packet with ID %d\n\n", PID);
		exit(EXIT_FAILURE);
	}
	else
	{
		newPacket.normalizedUtilValue = (((ExchangedInformation *) additionalInfo)->recUtil - ((ExchangedInformation *) additionalInfo)->senUtil) / (((ExchangedInformation *) additionalInfo)->recUtil + ((ExchangedInformation *) additionalInfo)->senUtil);
	}
	free(additionalInfo);


	/* sanity check */
	if(newPacket.distanceFromBegin == -1)
	{
		printf("\n[Error]: (HNUV::addPacket) The packet with ID %d does not exist inside the packet buffer of node %d\n\n", PID, nodeID);
		exit(EXIT_FAILURE);
	}


	/* add the new packet in the vector of scheduling packets */
	schedulingPackets.push_back(newPacket);

	return;
}


int *HNUV::getOutgoingPackets()
{
	int *pktList;
	int counter;
	vector<HNUVPacket>::reverse_iterator rit;


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
