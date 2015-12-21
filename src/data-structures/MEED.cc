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


#ifndef MEED_H
	#define MEED_H
	#include "MEED.h"
#endif

/* Description
 * -----------
 *  Minimum Estimated Expected Delay
 *  This utility calculates the minimum estimated expected delay between two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network. Finally, the utility value is normalized
 *  to be inside [0,1].
 */


MEED::MEED(int ID, int NodeSize): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->activeContacts = (int *) malloc(NodeSize * sizeof(int));
	this->firstContactTime = (double *) malloc(NodeSize * sizeof(double));
	this->lastSeparationTime = (double *) malloc(NodeSize * sizeof(double));
	this->sumSquared = (double *) malloc(NodeSize * sizeof(double));
	this->prevMEED = (double *) malloc(NodeSize * sizeof(double));

	for(i = 0; i < NodeSize; i++)
	{
		this->activeContacts[i] = 0;
		this->firstContactTime[i] = -1.0;
		this->lastSeparationTime[i] = -1.0;
		this->sumSquared[i] = 0.0;
		this->prevMEED[i] = 0.0;
	}

	return;
}

MEED::~MEED()
{
	free(activeContacts);
	free(firstContactTime);
	free(lastSeparationTime);
	free(sumSquared);
	free(prevMEED);
	return;
}

void MEED::Update(int ID, double CurrentTime)
{
	return;
}


void MEED::ContactUp(int ID, double CurrentTime)
{
	/* Keep track of the active contacts */
	this->activeContacts[ID]++;
	if(this->activeContacts[ID] > 1)
	{
		/* The nodes were already in contact */
		return;
	}


	/* Check if this is their first contact */
	if(this->firstContactTime[ID] == -1.0)
	{
		/* This is the initial time where the observations began */
		this->firstContactTime[ID] = CurrentTime;
	}
	else
	{
		/* Calculate their previous disconnection time squared and add it up to the sum */
		this->sumSquared[ID] += pow(CurrentTime - this->lastSeparationTime[ID], 2);
	}

	return;
}


void MEED::ContactDown(int ID, double CurrentTime)
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


	/* Keep track of the metric when the contact was last available */
	prevMEED[ID] = this->sumSquared[ID] / (2 * (CurrentTime - this->firstContactTime[ID]));

	return;
}


double MEED::get(int ID, double CurrentTime)
{
	double currMEED;


	if(this->sumSquared[ID] == 0.0)
	{
		return 0.0;
	}
	else
	{
		currMEED = (this->sumSquared[ID] + pow(CurrentTime - this->lastSeparationTime[ID], 2)) / (2 * (CurrentTime - this->firstContactTime[ID]));

		if(this->prevMEED[ID] > currMEED)
		{
			return 1.0 / (1.0 + this->prevMEED[ID]);
		}
		else
		{
			return 1.0 / (1.0 + currMEED);
		}
	}
}


double MEED::getActualMEED(int ID, double CurrentTime)
{
	double currMEED;


	if(this->sumSquared[ID] == 0.0)
	{
		return -1.0;
	}
	else
	{
		currMEED = (this->sumSquared[ID] + pow(CurrentTime - this->lastSeparationTime[ID], 2)) / (2 * (CurrentTime - this->firstContactTime[ID]));

		if(this->prevMEED[ID] > currMEED)
		{
			return this->prevMEED[ID];
		}
		else
		{
			return currMEED;
		}
	}
}


double* MEED::getUtilities(double CurrentTime)
{
	int i;
	double currMEED;
	double* meedUtilities;


	meedUtilities = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		if(this->sumSquared[i] == 0.0)
		{
			meedUtilities[i] = 0.0;
		}
		else
		{
			currMEED = (this->sumSquared[i] + pow(CurrentTime - this->lastSeparationTime[i], 2)) / (2 * (CurrentTime - this->firstContactTime[i]));

			if(this->prevMEED[i] > currMEED)
			{
				meedUtilities[i] = 1.0 / (1.0 + this->prevMEED[i]);
			}
			else
			{
				meedUtilities[i] = 1.0 / (1.0 + currMEED);
			}
		}
	}

	return meedUtilities;
}
