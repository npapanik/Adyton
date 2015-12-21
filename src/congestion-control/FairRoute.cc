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


#ifndef FAIRROUTE_H
	#define FAIROUTE_H
	#include "FairRoute.h"
#endif



FairRoute::FairRoute(int NID, PacketBuffer* Bf, Settings *S): CongestionControl(NID, Bf, S)
{
	this->information = NULL;

	return;
}


FairRoute::~FairRoute()
{
	return;
}


void FairRoute::contactUp(double currTime, int recID)
{
	return;
}


void FairRoute::contactDown(double currTime, int recID)
{
	return;
}


void FairRoute::receivedBufferReq(double currentTime, int reqNode)
{
	return;
}


void FairRoute::addBufferInfo(void* info)
{
	if(!info)
	{
		printf("FairRoute error: No information added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	this->information = (struct CongestionInformation *) info;

	return;
}


void *FairRoute::getExtraInfo(double currTime)
{
	return NULL;
}


void FairRoute::setExtraInfo(void *extraInfo, double currTime)
{
	free(extraInfo);

	return;
}


void FairRoute::addPacketInfo(struct ImportInformation *imp)
{
	FairRoutePacketInformation newInfo;


	if(imp->recUtil - imp->senUtil <= DBL_EPSILON)
	{
		printf("FairRoute error: The utility of the receiver (%f) should be greater than the utility of the sender (%f)!\nExiting...\n", imp->recUtil, imp->senUtil);
		exit(EXIT_FAILURE);
	}

	newInfo.packetID = imp->pktID;
	newInfo.senUtil = imp->senUtil;
	newInfo.recUtil = imp->recUtil;

	pInfo.push_back(newInfo);
	free(imp);

	#ifdef CC_DEBUG
		printf("Added packet information with ID = %d, sender's utility = %f, and receiver's utility = %f\n", newInfo.packetID, newInfo.senUtil, newInfo.recUtil);
	#endif

	return;
}


int *FairRoute::filterPackets(int* pktList)
{
	int i;
	int counter;
	int senBufLength;
	int recBufLength;
	int recBufSize;
	int newPktNum;
	int *newPktList;


	if(!pktList)
	{
		releaseInformation();
		return NULL;
	}

	if(!this->information)
	{
		printf("FairRoute error: Needed information not added!\nExiting...\n");
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

	senBufLength = Buff->getBufferLength();
	recBufLength = information->recBufLength;
	recBufSize = information->recBufSize;
	newPktNum = 0;

	for(i = pktList[0]; i >= 1; i--)
	{
		if(recBufLength <= senBufLength)
		{
			#ifdef CC_DEBUG
				printf("#%d: The packet with ID %d will be forwarded because the receiver (%d) carries either equal or less packets than the sender (%d)!\n", newPktNum + 1, pktList[i], recBufLength, senBufLength);
			#endif

			senBufLength--;

			if(recBufLength < recBufSize)
			{
				recBufLength++;
			}

			newPktNum++;
		}
		else if((getSenUtil(pktList[i]) <= DBL_EPSILON) && (getRecUtil(pktList[i]) > DBL_EPSILON))
		{
			#ifdef CC_DEBUG
				printf("#%d: The packet with ID %d will be forwarded because the receiver (%f) has a greater-than-zero utility, unlike the sender (%f)!\n", newPktNum + 1, pktList[i], getRecUtil(pktList[i]), getSenUtil(pktList[i]));
			#endif

			senBufLength--;

			if(recBufLength < recBufSize)
			{
				recBufLength++;
			}

			newPktNum++;
		}
		else
		{
			#ifdef CC_DEBUG
				printf("#%d: The packet with ID %d will not be forwarded because it does not satisfy the forwarding condition!\n", newPktNum, pktList[i]);
			#endif

			pktList[i] = REMOVED_PACKET;
		}
	}

	releaseInformation();

	if(newPktNum == pktList[0])
	{
		#ifdef CC_DEBUG
			printf("\n>> Returning the following packet list: ");
			for(i = 1; i <= pktList[0]; i++)
			{
				printf("%d\t", pktList[i]);
			}
			printf("\n");
		#endif

		return pktList;
	}
	else
	{
		newPktList = (int *) malloc(sizeof(int) * (newPktNum + 1));
		newPktList[0] = newPktNum;
		counter = 1;

		for(i = 1; i <= pktList[0]; i++)
		{
			if(pktList[i] != REMOVED_PACKET)
			{
				newPktList[counter] = pktList[i];
				counter++;
			}
		}

		free(pktList);

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
}


void FairRoute::releaseInformation()
{
	free(this->information);
	this->information = NULL;
	pInfo.clear();

	return;
}


int FairRoute::getBufferSize()
{
	return Buff->getBufferSize();
}


int FairRoute::getBufferLength()
{
	return Buff->getBufferLength();
}


double FairRoute::getSenUtil(int PID)
{
	vector<FairRoutePacketInformation>::iterator it;


	for(it = pInfo.begin(); it != pInfo.end(); ++it)
	{
		if((*it).packetID == PID)
		{
			return (*it).senUtil;
		}
	}

	printf("FairRoute error: Could not find the packet with ID %d\nExiting...\n", PID);
	exit(EXIT_FAILURE);
}


double FairRoute::getRecUtil(int PID)
{
	vector<FairRoutePacketInformation>::iterator it;


	for(it = pInfo.begin(); it != pInfo.end(); ++it)
	{
		if((*it).packetID == PID)
		{
			return (*it).recUtil;
		}
	}

	printf("FairRoute error: Could not find the packet with ID %d\nExiting...\n", PID);
	exit(EXIT_FAILURE);
}
