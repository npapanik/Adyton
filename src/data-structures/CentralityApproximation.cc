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


#ifndef CENTRALITYAPPROXIMATION_H
	#define CENTRALITYAPPROXIMATION_H
	#include "CentralityApproximation.h"
#endif


CentralityApproximation::CentralityApproximation(int myID, int totalNodes)
{
	int i;


	nodeID = myID;
	numNodes = totalNodes;
	prevTimePeriod = 0;
	currTimePeriod = 0;
	timeUnit = SIX_HOURS;

	if((prevTimeSlot = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
    {
        printf("\nError!\nUnable to allocate memory for the previous time slot array of node %d\n\n", nodeID);
        exit(EXIT_FAILURE);
    }

	if((currTimeSlot = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
    {
        printf("\nError!\nUnable to allocate memory for the current time slot array of node %d\n\n", nodeID);
        exit(EXIT_FAILURE);
    }

	if((currConnectedNodes = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
    {
        printf("\nError!\nUnable to allocate memory for the currently connected nodes array of node %d\n\n", nodeID);
        exit(EXIT_FAILURE);
    }

	for(i = 0; i < numNodes; i++)
	{
		prevTimeSlot[i] = false;
		currTimeSlot[i] = false;
		currConnectedNodes[i] = false;
	}

	return;
}


CentralityApproximation::~CentralityApproximation()
{
	free(prevTimeSlot);
	free(currTimeSlot);
	free(currConnectedNodes);

	return;
}


void CentralityApproximation::connectionOccured(double currTime, int encID)
{
	updateTimeSlots(currTime);

	currConnectedNodes[encID] = true;
	currTimeSlot[encID] = true;

	return;
}


void CentralityApproximation::disconnectionOccured(double currTime, int encID)
{
	updateTimeSlots(currTime);

	currConnectedNodes[encID] = false;
	if(fmod(currTime, timeUnit) > 0.0)
	{
		currTimeSlot[encID] = true;
	}
	else
	{
		currTimeSlot[encID] = false;
	}

	return;
}


void CentralityApproximation::updateTimeSlots(double currTime)
{
	int i;
	unsigned long int newTimePeriod;


	newTimePeriod = (unsigned long int) (currTime / timeUnit);

	if(newTimePeriod == currTimePeriod)
	{
		return;
	}
	else if(newTimePeriod > currTimePeriod)
	{
		if(newTimePeriod == currTimePeriod + 1)
		{
			for(i = 0; i < numNodes; i++)
			{
				prevTimeSlot[i] = currTimeSlot[i];

				if(currConnectedNodes[i])
				{
					currTimeSlot[i] = true;
				}
				else
				{
					currTimeSlot[i] = false;
				}
			}

			prevTimePeriod = currTimePeriod;
			currTimePeriod = newTimePeriod;
		}
		else
		{
			for(i = 0; i < numNodes; i++)
			{
				if(currConnectedNodes[i])
				{
					prevTimeSlot[i] = true;
					currTimeSlot[i] = true;
				}
				else
				{
					prevTimeSlot[i] = false;
					currTimeSlot[i] = false;
				}
			}

			prevTimePeriod = newTimePeriod - 1;
			currTimePeriod = newTimePeriod;
		}

		return;
	}
	else
	{
		printf("\nError!\nTime is going backwards!\n\n");
		exit(EXIT_FAILURE);
	}
}



double CentralityApproximation::getLocalRank(double currTime, bool *myLocalCommunity)
{
	int i;
	double localRank;


	updateTimeSlots(currTime);


	/* Count my unique encounters with nodes in my local community */
	localRank = 0.0;
	if(prevTimePeriod != currTimePeriod)
	{
		for(i = 0; i < numNodes; i++)
		{
			if((myLocalCommunity[i]) && (prevTimeSlot[i]))
			{
				localRank++;
			}
		}
	}
	else
	{
		for(i = 0; i < numNodes; i++)
		{
			if((myLocalCommunity[i]) && (currTimeSlot[i]))
			{
				localRank++;
			}
		}
	}

	return localRank;
}


double CentralityApproximation::getGlobalRank(double currTime)
{
	int i;
	double globalRank;


	updateTimeSlots(currTime);


	/* Count my unique encounters with any node in the network */
	globalRank = 0.0;
	if(prevTimePeriod != currTimePeriod)
	{
		for(i = 0; i < numNodes; i++)
		{
			if(prevTimeSlot[i])
			{
				globalRank++;
			}
		}
	}
	else
	{
		for(i = 0; i < numNodes; i++)
		{
			if(currTimeSlot[i])
			{
				globalRank++;
			}
		}
	}

	return globalRank;
}
