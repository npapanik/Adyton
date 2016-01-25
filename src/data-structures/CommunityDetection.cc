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
 *  Written by Dimitrios-Georgios Akestoridis.
 */


#ifndef COMMUNITYDETECTION_H
	#define COMMUNITYDETECTION_H
	#include "CommunityDetection.h"
#endif


CommunityDetection::CommunityDetection(int myID, int totalNodes)
{
	int i;
	int j;


	nodeID = myID;
	numNodes = totalNodes;
	kappa = DEFAULT_KAPPA;
	familiarSetThreshold = DEFAULT_FAMILIAR_SET_THRESHOLD;
	prevUpdate = 0.0;

	if((cumulativeContactDurations = (double *) malloc(numNodes * sizeof(double))) == NULL)
	{
		printf("\nError!\nUnable to allocate memory for the cumulativeContactDurations array of node %d\n\n", nodeID);
		exit(EXIT_FAILURE);
	}

	if((currConnectedNodes = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
	{
		printf("\nError!\nUnable to allocate memory for the currConnectedNodes array of node %d\n\n", nodeID);
		exit(EXIT_FAILURE);
	}

	if((myLocalCommunity = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
	{
		printf("\nError!\nUnable to allocate memory for the myLocalCommunity array of node %d\n\n", nodeID);
		exit(EXIT_FAILURE);
	}

	if((myFamiliarSets = (bool **) malloc(numNodes * sizeof(bool *))) == NULL)
	{
		printf("\nError!\nUnable to allocate memory for the approximatedFamiliarSets array of node %d\n\n", nodeID);
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < numNodes; i++)
	{
		if((myFamiliarSets[i] = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
		{
		    printf("\nError!\nUnable to allocate memory for the approximatedFamiliarSets array of node %d\n\n", nodeID);
		    exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < numNodes; i++)
	{
		cumulativeContactDurations[i] = 0.0;
		currConnectedNodes[i] = false;

		if(i != nodeID)
		{
			myLocalCommunity[i] = false;
		}
		else
		{
			myLocalCommunity[i] = true;
		}

		for(j = 0; j < numNodes; j++)
		{
			myFamiliarSets[i][j] = false;
		}
	}

	return;
}


CommunityDetection::~CommunityDetection()
{
	int i;


	for(i = 0; i < numNodes; i++)
	{
		free(myFamiliarSets[i]);
	}

	free(myFamiliarSets);
	free(myLocalCommunity);
	free(currConnectedNodes);
	free(cumulativeContactDurations);

	return;
}


void CommunityDetection::setKappa(int value)
{
	kappa = value;

	return;
}


void CommunityDetection::setFamiliarSetThreshold(double threshold)
{
	familiarSetThreshold = threshold;

	return;
}


void CommunityDetection::connectionOccured(double currTime, int encID)
{
	updateLCandFS(currTime);
	currConnectedNodes[encID] = true;

	return;
}


void CommunityDetection::disconnectionOccured(double currTime, int encID)
{
	updateLCandFS(currTime);
	currConnectedNodes[encID] = false;

	return;
}


void CommunityDetection::updateLCandFS(double currTime)
{
	int i;


	for(i = 0; i < numNodes; i++)
	{
		if(currConnectedNodes[i])
		{
			cumulativeContactDurations[i] += currTime - prevUpdate;

			if((cumulativeContactDurations[i] > familiarSetThreshold) && (!myFamiliarSets[nodeID][i]))
			{
				myFamiliarSets[nodeID][i] = true;
				myLocalCommunity[i] = true;
			}
		}
	}

	prevUpdate = currTime;

	return;
}


void CommunityDetection::updateBubble(bool *encLocalCommunity, bool **encFamiliarSets, int encID, double currentTime)
{
	int i;
	int j;
	int commonMembers;


	updateLCandFS(currentTime);


	/* Check if the encounter node is not in my local community */
	if(!myLocalCommunity[encID])
	{
		commonMembers = 0;

		for(i = 0; i < numNodes; i++)
		{
			if((encFamiliarSets[encID][i]) && (myLocalCommunity[i]))
			{
				commonMembers++;
			}
		}


		if(commonMembers >= kappa - 1)
		{
			myLocalCommunity[encID] = true;
		}
	}


	/* Check if the encounter node is in my local community */
	if(myLocalCommunity[encID])
	{
		for(i = 0; i < numNodes; i++)
		{
			/* Update my approximation of the familiar set of the encountered node */
			myFamiliarSets[encID][i] = encFamiliarSets[encID][i];


			/* Check the node in the local community of the encountered node */
			if((encLocalCommunity[i]) && (!myLocalCommunity[i]))
			{
				commonMembers = 0;

				for(j = 0; j < numNodes; j++)
				{
					if((encFamiliarSets[i][j]) && (myLocalCommunity[j]))
					{
						commonMembers++;
					}
				}

				if(commonMembers >= kappa - 1)
				{
					myLocalCommunity[i] = true;

					/* Update my approximation of the familiar set of the new community member */
					for(j = 0; j < numNodes; j++)
					{
						myFamiliarSets[i][j] = encFamiliarSets[i][j];
					}
				}
			}
		}
	}

	return;
}


bool *CommunityDetection::cloneLocalCommunity(double currentTime)
{
	bool *tmp;
	
	updateLCandFS(currentTime);

	if((tmp = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
	{
		printf("\nError!\nUnable to allocate memory to clone the myLocalCommunity array of node %d\n\n", nodeID);
		exit(EXIT_FAILURE);
	}

	memcpy(tmp, myLocalCommunity, numNodes * sizeof(bool));

	return tmp;
}


bool **CommunityDetection::cloneFamiliarSets(double currentTime)
{
	int i;
	bool **tmp;


	updateLCandFS(currentTime);

	if((tmp = (bool **) malloc(numNodes * sizeof(bool *))) == NULL)
	{
		printf("\nError!\nUnable to allocate memory to clone the approximatedFamiliarSets array of node %d\n\n", nodeID);
		exit(EXIT_FAILURE);
	}

	memcpy(tmp, myFamiliarSets, numNodes * sizeof(bool*));

	for(i = 0; i < numNodes; i++)
	{
		if((tmp[i] = (bool *) malloc(numNodes * sizeof(bool))) == NULL)
		{
		    printf("\nError!\nUnable to allocate memory to clone the approximatedFamiliarSets array of node %d\n\n", nodeID);
		    exit(EXIT_FAILURE);
		}

		memcpy(tmp[i], myFamiliarSets[i], numNodes * sizeof(bool));
	}

	return tmp;
}
