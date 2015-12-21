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


#ifndef ACC_H
	#define ACC_H
	#include "ACC.h"
#endif



ACC::ACC(int NID, PacketBuffer* Bf, Settings *S): CongestionControl(NID, Bf, S)
{
	this->information = NULL;
	Buff->enableBufferRecording();

	return;
}


ACC::~ACC()
{
	return;
}


void ACC::contactUp(double currTime, int recID)
{
	return;
}


void ACC::contactDown(double currTime, int recID)
{
	return;
}


void ACC::receivedBufferReq(double currentTime, int reqNode)
{
	return;
}


void ACC::addBufferInfo(void* info)
{
	if(!info)
	{
		printf("ACC error: No information added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	this->information = (struct CongestionInformation *) info;

	return;
}


void *ACC::getExtraInfo(double currTime)
{
	return NULL;
}


void ACC::setExtraInfo(void *extraInfo, double currTime)
{
	free(extraInfo);

	return;
}


void ACC::addPacketInfo(struct ImportInformation *imp)
{
	struct ACCPacketInformation newInfo;


	newInfo.packetID = imp->pktID;
	newInfo.packetRTTL = Buff->getRTTL(newInfo.packetID);
	newInfo.projNetGrowth = Buff->SGod->getProjNetGrowth(information->recID, Buff->getRTTL(newInfo.packetID));
	newInfo.riskMeanRTTL = Buff->SGod->getMeanRTTL(information->recID, Buff->getRTTL(newInfo.packetID));

	pInfo.push_back(newInfo);
	free(imp);

	#ifdef CC_DEBUG
		printf("Added packet information with ID = %d, RTTL = %f, mean RTTL risk = %f, and projected net growth = %f\n", newInfo.packetID, newInfo.packetRTTL, newInfo.riskMeanRTTL, newInfo.projNetGrowth);
	#endif

	return;
}


int *ACC::filterPackets(int* pktList)
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
		printf("ACC error: Needed information not added!\nExiting...\n");
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
		if((recBufLength + 1) - recBufSize <= 0)
		{/* Rule 1 */
			if((getProjNetGrowth(pktList[i]) + recBufLength + 1) - recBufSize <= DBL_EPSILON)
			{/* Rule 2 */
				senBufLength--;

				if(recBufLength < recBufSize)
				{
					recBufLength++;
				}

				newPktNum++;

				#ifdef CC_DEBUG
					printf("#%d: The packet with ID %d will be forwarded because the projected net growth (%f) is low!\n", newPktNum, pktList[i], getProjNetGrowth(pktList[i]));
				#endif
			}
			else if(getPacketRTTL(pktList[i]) - getRiskMeanRTTL(pktList[i]) <= DBL_EPSILON)
			{/* Rule 3 */
				senBufLength--;

				if(recBufLength < recBufSize)
				{
					recBufLength++;
				}

				newPktNum++;

				#ifdef CC_DEBUG
					printf("#%d: The packet with ID %d will be forwarded because the mean RTTL risk (%f) is low!\n", newPktNum, pktList[i], getRiskMeanRTTL(pktList[i]));
				#endif
			}
			else
			{
				#ifdef CC_DEBUG
					printf("#%d: The packet with ID %d will not be forwarded because of high projected net growth (%f) and high mean RTTL risk (%f)!\n", newPktNum, pktList[i], getProjNetGrowth(pktList[i]), getRiskMeanRTTL(pktList[i]));
				#endif

				pktList[i] = REMOVED_PACKET;
			}
		}
		else
		{
			#ifdef CC_DEBUG
				printf("#%d: The packet with ID %d will not be forwarded because there are not enough resources!\n", newPktNum, pktList[i]);
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


void ACC::releaseInformation()
{
	free(this->information);
	this->information = NULL;
	pInfo.clear();

	return;
}


int ACC::getBufferSize()
{
	return Buff->getBufferSize();
}


int ACC::getBufferLength()
{
	return Buff->getBufferLength();
}


double ACC::getPacketRTTL(int PID)
{
	vector<ACCPacketInformation>::iterator it;


	for(it = pInfo.begin(); it != pInfo.end(); ++it)
	{
		if((*it).packetID == PID)
		{
			return (*it).packetRTTL;
		}
	}

	printf("ACC error: Could not find the packet with ID %d\nExiting...\n", PID);
	exit(EXIT_FAILURE);
}


double ACC::getProjNetGrowth(int PID)
{
	vector<ACCPacketInformation>::iterator it;


	for(it = pInfo.begin(); it != pInfo.end(); ++it)
	{
		if((*it).packetID == PID)
		{
			return (*it).projNetGrowth;
		}
	}

	printf("ACC error: Could not find the packet with ID %d\nExiting...\n", PID);
	exit(EXIT_FAILURE);
}


double ACC::getRiskMeanRTTL(int PID)
{
	vector<ACCPacketInformation>::iterator it;


	for(it = pInfo.begin(); it != pInfo.end(); ++it)
	{
		if((*it).packetID == PID)
		{
			return (*it).riskMeanRTTL;
		}
	}

	printf("ACC error: Could not find the packet with ID %d\nExiting...\n", PID);
	exit(EXIT_FAILURE);
}
