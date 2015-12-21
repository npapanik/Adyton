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


#ifndef ASP_H
	#define ASP_H
	#include "ASP.h"
#endif

/* Description
 * -----------
 *  Average Separation Period
 *  This utility calculates the average separation period between two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network. Finally, the utility value is normalized
 *  to be inside [0,1].
 */


ASP::ASP(int ID, int NodeSize): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->numGaps = (int *) malloc(NodeSize * sizeof(int));
	this->activeContacts = (int *) malloc(NodeSize * sizeof(int));
	this->lastSeparationTime = (double *) malloc(NodeSize * sizeof(double));
	this->totalASP = (double *) malloc(NodeSize * sizeof(double));
	this->aspValue = (double *) malloc(NodeSize * sizeof(double));

	for(i = 0; i < NodeSize; i++)
	{
		this->numGaps[i] = -1;
		this->activeContacts[i] = 0;
		this->lastSeparationTime[i] = -1.0;
		this->totalASP[i] = 0.0;
		this->aspValue[i] = 0.0;
	}

	return;
}

ASP::~ASP()
{
	free(numGaps);
	free(activeContacts);
	free(lastSeparationTime);
	free(totalASP);
	free(aspValue);
	return;
}



void ASP::Update(int ID, double CurrentTime)
{
	return;
}


void ASP::ContactUp(int ID, double CurrentTime)
{
	/* Keep track of the active contacts */
	this->activeContacts[ID]++;
	if(this->activeContacts[ID] > 1)
	{
		/* The nodes were already in contact */
		return;
	}


	/* If this is not their first encounter, we can now calculate their previous separation period */
	numGaps[ID]++;
	if(numGaps[ID] > 0)
	{
		this->totalASP[ID] += CurrentTime - this->lastSeparationTime[ID];
		this->aspValue[ID] = this->totalASP[ID] / this->numGaps[ID];
	}

	return;
}


void ASP::ContactDown(int ID, double CurrentTime)
{
	/* Keep track of the active contacts */
	this->activeContacts[ID]--;
	if(this->activeContacts[ID] > 0)
	{
		/* The nodes are still in contact */
		return;
	}


	/* This is the beginning of their next separation period */
	this->lastSeparationTime[ID] = CurrentTime;

	return;
}


double ASP::get(int ID, double CurrentTime)
{
	if(this->numGaps[ID] < 1)
	{
		return 0.0;
	}
	else
	{
		return (1.0 / (1.0 + this->aspValue[ID]));
	}
}


double ASP::getActualASP(int ID, double CurrentTime)
{
	if(this->numGaps[ID] < 1)
	{
		return -1.0;
	}
	else
	{
		return aspValue[ID];
	}
}


double ASP::getTimeDisconnected(int ID, double CurrentTime)
{
	if(this->lastSeparationTime[ID] < 0)
	{
		return -1.0;
	}
	else
	{
		return CurrentTime - this->lastSeparationTime[ID];
	}
}
