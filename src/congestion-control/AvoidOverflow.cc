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


#ifndef AVOIDOVERFLOW_H
	#define AVOIDOVERFLOW_H
	#include "AvoidOverflow.h"
#endif



AvoidOverflow::AvoidOverflow(int NID, PacketBuffer *Bf, Settings *S): CongestionControl(NID, Bf, S)
{
	this->information = NULL;

	return;
}


AvoidOverflow::~AvoidOverflow()
{
	return;
}


void AvoidOverflow::contactUp(double currTime, int recID)
{
	return;
}


void AvoidOverflow::contactDown(double currTime, int recID)
{
	return;
}


void AvoidOverflow::receivedBufferReq(double currentTime, int reqNode)
{
	return;
}


void AvoidOverflow::addBufferInfo(void *info)
{
	if(!info)
	{
		printf("AvoidOverflow error: No information added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	this->information = (struct CongestionInformation *) info;

	return;
}


void *AvoidOverflow::getExtraInfo(double currTime)
{
	return NULL;
}


void AvoidOverflow::setExtraInfo(void *extraInfo, double currTime)
{
	free(extraInfo);

	return;
}


void AvoidOverflow::addPacketInfo(struct ImportInformation *imp)
{
	free(imp);

	return;
}


int *AvoidOverflow::filterPackets(int *pktList)
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
		printf("AvoidOverflow error: Needed information not added!\nExiting...\n");
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
	else
	{
		newPktList = (int *) malloc(sizeof(int));
		newPktList[0] = 0;

		#ifdef CC_DEBUG
			printf("\n>> Returning the following packet list: ");
			for(i = 1; i <= newPktList[0]; i++)
			{
				printf("%d\t", newPktList[i]);
			}
			printf("\n");
		#endif

		free(pktList);
		releaseInformation();
		return newPktList;
	}
}


void AvoidOverflow::releaseInformation()
{
	free(this->information);
	this->information = NULL;

	return;
}


int AvoidOverflow::getBufferSize()
{
	return Buff->getBufferSize();
}


int AvoidOverflow::getBufferLength()
{
	return Buff->getBufferLength();
}
