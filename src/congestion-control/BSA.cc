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


#ifndef BSA_H
	#define BSA_H
	#include "BSA.h"
#endif



BSA::BSA(int NID, PacketBuffer *Bf, Settings *S): CongestionControl(NID, Bf, S)
{
	this->information = NULL;
	this->lastUpdate = 0.0;
	this->scanningInterval = S->getScanningInterval();
	this->congestionThreshold = INITIAL_THRESHOLD;
	Buff->enableBufferRecording();

	if(this->scanningInterval < 0.0)
	{
		printf("BSA error: Invalid scanning interval (%f)!\nExiting...\n", this->scanningInterval);
		exit(EXIT_FAILURE);
	}

	return;
}


BSA::~BSA()
{
	return;
}


void BSA::contactUp(double currTime, int recID)
{
	return;
}


void BSA::contactDown(double currTime, int recID)
{
	return;
}


void BSA::receivedBufferReq(double currentTime, int reqNode)
{
	#ifdef CC_DEBUG
		printf("@%f Node %d: Current congestion threshold = %f\n", lastUpdate, nodeID, congestionThreshold);
	#endif


	while(currentTime - (lastUpdate + scanningInterval) > DBL_EPSILON)
	{
		if(Buff->getNumPacketDrops(lastUpdate, lastUpdate + scanningInterval) == 0)
		{
			if(MAXIMUM_THRESHOLD - congestionThreshold > DBL_EPSILON)
			{
				congestionThreshold = congestionThreshold + ADDITIVE_INCREASE;
			}
		}
		else if(Buff->getNumPacketDrops(lastUpdate, lastUpdate + scanningInterval) > 0)
		{
			if(congestionThreshold - MINIMUM_THRESHOLD > DBL_EPSILON)
			{
				congestionThreshold = congestionThreshold * MULTIPLICATIVE_DECREASE;
			}
		}
		else
		{
			printf("BSA error: The number of packet drops decreased over time!\nExiting...\n");
			exit(EXIT_FAILURE);
		}

		lastUpdate += scanningInterval;

		#ifdef CC_DEBUG
			printf("@%f Node %d: Updated congestion threshold = %f\n", lastUpdate, nodeID, congestionThreshold);
		#endif
	}

	return;
}


void BSA::addBufferInfo(void *info)
{
	if(!info)
	{
		printf("BSA error: No information added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	this->information = (struct CongestionInformation *) info;

	return;
}


void *BSA::getExtraInfo(double currTime)
{
	return NULL;
}


void BSA::setExtraInfo(void *extraInfo, double currTime)
{
	free(extraInfo);

	return;
}


void BSA::addPacketInfo(struct ImportInformation *imp)
{
	free(imp);

	return;
}


int *BSA::filterPackets(int *pktList)
{
	int i;
	int remainingStorage;
	int counter;
	int startingPoint;
	int *newPktList;


	if(!pktList)
	{
		releaseInformation();
		return NULL;
	}

	if(!this->information)
	{
		printf("BSA error: Needed information not added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	#ifdef CC_DEBUG
		printf("\n>> Received the following packet list: ");
		for(i = 1; i <= pktList[0]; i++)
		{
			printf("%d\t", pktList[i]);
		}
		printf("\n");
	#endif
	
	remainingStorage = this->information->recBufSize - this->information->recBufLength;

	if(pktList[0] <= remainingStorage)
	{
		#ifdef CC_DEBUG
			printf("\n>> Returning the following packet list: ");
			for(i = 1; i <= pktList[0]; i++)
			{
				printf("%d\t", pktList[i]);
			}
			printf("\n");
		#endif

		releaseInformation();
		return pktList;
	}
	else if(remainingStorage > 0)
	{
		newPktList = (int *) malloc((remainingStorage + 1) * sizeof(int));
		newPktList[0] = remainingStorage;
		counter = 1;
		startingPoint = (pktList[0] - remainingStorage) + 1;

		for(i = startingPoint; i <= pktList[0]; i++)
		{
			newPktList[counter] = pktList[i];
			counter++;
		}
	}
	else
	{
		newPktList = (int *) malloc(sizeof(int));
		newPktList[0] = 0;
	}

	free(pktList);
	releaseInformation();

	#ifdef CC_DEBUG
		printf("\n>> Returning the following packet list: ");
		for(i = 1; i <= newPktList[0]; i++)
		{
			printf("%d\t", newPktList[i]);
		}
		printf("\n");
	#endif

	return newPktList;
}


void BSA::releaseInformation()
{
	free(this->information);
	this->information = NULL;

	return;
}


int BSA::getBufferSize()
{
	double advertisedBufferSize;


	advertisedBufferSize = congestionThreshold * Buff->getBufferSize();

	return ((int) advertisedBufferSize);
}


int BSA::getBufferLength()
{
	return Buff->getBufferLength();
}
