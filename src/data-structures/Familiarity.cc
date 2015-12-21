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


#ifndef FAMILIARITY_H
	#define FAMILIARITY_H
	#include "Familiarity.h"
#endif

/* Description
 * -----------
 *  Familiarity
 *  This utility calculates the exponential moving average
 *  of the total duration of contacts between two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network.
 */


Familiarity::Familiarity(int ID, int NodeSize): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->lastTimePeriod = 0;
	this->lastUpdate = 0.0;
	this->alpha = DEFAULT_ALPHA;
	this->windowSize = DEFAULT_WINDOWSIZE;

	this->activeContacts = (int *) malloc(NodeSize * sizeof(int));
	this->prevTotalContactDuration = (double *) malloc(NodeSize * sizeof(double));
	this->currFamiliarity = (double *) malloc(NodeSize * sizeof(double));

	for(i = 0; i < NodeSize; i++)
	{
		this->activeContacts[i] = 0;
		this->prevTotalContactDuration[i] = 0.0;
		this->currFamiliarity[i] = 0.0;
	}

	return;
}


Familiarity::Familiarity(int ID, int NodeSize, double a, double windowS): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->lastTimePeriod = 0;
	this->lastUpdate = 0.0;
	this->alpha = a;
	this->windowSize = windowS;

	this->activeContacts = (int *) malloc(NodeSize * sizeof(int));
	this->prevTotalContactDuration = (double *) malloc(NodeSize * sizeof(double));
	this->currFamiliarity = (double *) malloc(NodeSize * sizeof(double));

	for(i = 0; i < NodeSize; i++)
	{
		this->activeContacts[i] = 0;
		this->prevTotalContactDuration[i] = 0.0;
		this->currFamiliarity[i] = 0.0;
	}

	return;
}

Familiarity::~Familiarity()
{
	free(this->activeContacts);
	free(this->prevTotalContactDuration);
	free(this->currFamiliarity);
	return;
}


void Familiarity::Update(int ID, double CurrentTime)
{
	unsigned long int i;
	unsigned long int currTimePeriod;


	if(CurrentTime < this->lastUpdate)
	{ 
		printf("\nFamiliarity: Error! Time is going backwards!\n\n");
		exit(EXIT_FAILURE);
	}

	currTimePeriod = (unsigned long int) (CurrentTime / this->windowSize);

	if(currTimePeriod == this->lastTimePeriod)
	{
		updateDurations(CurrentTime);
	}
	else if(currTimePeriod > this->lastTimePeriod)
	{
		for(i = (currTimePeriod - this->lastTimePeriod); i >= 1; i--)
		{
			updateDurations((CurrentTime - fmod(CurrentTime, this->windowSize)) - ((i - 1) * windowSize));
			updateFamiliarities((CurrentTime - fmod(CurrentTime, this->windowSize)) - ((i - 1) * windowSize));
		}

		updateDurations(CurrentTime);
		this->lastTimePeriod = currTimePeriod;
	}
	else
	{
		printf("\nFamiliarity: Error! Time is going backwards!\n\n");
		exit(EXIT_FAILURE);
	}

	return;
}


void Familiarity::ContactUp(int ID, double CurrentTime)
{
	#ifdef FAMILIARITY_DEBUG
		printf("@%f Node %d: Contact-Up with node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);


	/* Keep track of the active contacts */
	this->activeContacts[ID]++;

	return;
}


void Familiarity::ContactDown(int ID, double CurrentTime)
{
	#ifdef FAMILIARITY_DEBUG
		printf("@%f Node %d: Contact-Down with node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);


	/* Keep track of the active contacts */
	this->activeContacts[ID]--;

	return;
}


double Familiarity::get(int ID, double CurrentTime)
{
	#ifdef FAMILIARITY_DEBUG
		printf("@%f Node %d: Get utility of node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);

	return this->currFamiliarity[ID];
}


double* Familiarity::getFamiliarities(double CurrentTime)
{
	int i;
	double* familiarityUtilities;


	/* Update the utility values */
	Update(-1, CurrentTime);

	familiarityUtilities = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		familiarityUtilities[i] = this->currFamiliarity[i];
	}

	return familiarityUtilities;
}


void Familiarity::updateFamiliarities(double CurrentTime)
{
	int i;


	for(i = 0; i < NSize; i++)
	{
		#ifdef FAMILIARITY_DEBUG
			printf("@%f Node %d: prevTotalContactDuration[%d] = %f, prevFamiliarity[%d] = %f", CurrentTime, this->NID, i, this->prevTotalContactDuration[i], i, this->currFamiliarity[i]);
		#endif

		this->currFamiliarity[i] = (this->alpha * this->prevTotalContactDuration[i]) + ((1 - this->alpha) * this->currFamiliarity[i]);
		this->prevTotalContactDuration[i] = 0.0;

		#ifdef FAMILIARITY_DEBUG
			printf(", currFamiliarity[%d] = %f\n", i, this->currFamiliarity[i]);
		#endif
	}

	#ifdef FAMILIARITY_DEBUG
		printf("\n");
	#endif

	return;
}


void Familiarity::updateDurations(double CurrentTime)
{
	int i;


	for(i = 0; i < NSize; i++)
	{
		if(this->activeContacts[i] > 0)
		{
			this->prevTotalContactDuration[i] += CurrentTime - this->lastUpdate;
		}
	}

	this->lastUpdate = CurrentTime;

	return;
}
