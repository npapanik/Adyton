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
 *  Written by Nikolaos Papanikos.
 */


#ifndef SOCIABILITY_H
	#define SOCIABILITY_H
	#include "Sociability.h"
#endif

/* Description
 * -----------
 *  Sociability
 *  This utility calculates the exponential moving average
 *  of the total number of unique encounters during a time period.
 *  It is destination independent, so each node calculates the same value
 *  for every other node in the network.
 */


Sociability::Sociability(int ID, int NodeSize): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->lastTimePeriod = 0;
	this->lastUpdate = 0.0;
	this->alpha = DEFAULT_ALPHA;
	this->windowSize = DEFAULT_WINDOWSIZE;

	this->currSociability = 0.0;
	this->encounteredNodes = (bool *) malloc(NodeSize * sizeof(bool));

	for(i = 0; i < NodeSize; i++)
	{
		this->encounteredNodes[i] = false;
	}

	return;
}


Sociability::Sociability(int ID, int NodeSize, double a, double windowS): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->lastTimePeriod = 0;
	this->lastUpdate = 0.0;
	this->alpha = a;
	this->windowSize = windowS;

	this->currSociability = 0.0;
	this->encounteredNodes = (bool *) malloc(NodeSize * sizeof(bool));

	for(i = 0; i < NodeSize; i++)
	{
		this->encounteredNodes[i] = false;
	}

	return;
}

Sociability::~Sociability()
{
	free(encounteredNodes);
	return;
}



void Sociability::Update(int ID, double CurrentTime)
{
	unsigned long int i;
	unsigned long int currTimePeriod;


	if(CurrentTime < this->lastUpdate)
	{ 
		printf("\nSociability: Error! Time is going backwards!\n\n");
		exit(EXIT_FAILURE);
	}

	currTimePeriod = (unsigned long int) (CurrentTime / this->windowSize);

	if(currTimePeriod == this->lastTimePeriod)
	{
		/* Do nothing */
	}
	else if(currTimePeriod > this->lastTimePeriod)
	{
		for(i = (currTimePeriod - this->lastTimePeriod); i >= 1; i--)
		{
			updateSociability((CurrentTime - fmod(CurrentTime, this->windowSize)) - ((i - 1) * windowSize));
		}

		this->lastTimePeriod = currTimePeriod;
	}
	else
	{
		printf("\nSociability: Error! Time is going backwards!\n\n");
		exit(EXIT_FAILURE);
	}

	return;
}


void Sociability::ContactUp(int ID, double CurrentTime)
{
	#ifdef SOCIABILITY_DEBUG
		printf("@%f Node %d: Contact-Up with node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility value */
	Update(ID, CurrentTime);


	/* Keep track of the encountered nodes */
	this->encounteredNodes[ID] = true;

	return;
}


void Sociability::ContactDown(int ID, double CurrentTime)
{
	#ifdef SOCIABILITY_DEBUG
		printf("@%f Node %d: Contact-Down with node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility value */
	Update(ID, CurrentTime);

	return;
}


double Sociability::get(int ID, double CurrentTime)
{
	#ifdef SOCIABILITY_DEBUG
		printf("@%f Node %d: Get utility of node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility value */
	Update(ID, CurrentTime);

	return currSociability;
}


void Sociability::updateSociability(double CurrentTime)
{
	int i;
	unsigned long int uniqueEncounters;


	uniqueEncounters = 0;
	for(i = 0; i < NSize; i++)
	{
		if(this->encounteredNodes[i])
		{
			uniqueEncounters++;
		}
	}

	#ifdef SOCIABILITY_DEBUG
		printf("@%f Node %d: uniqueEncounters = %lu, prevSociability = %f", CurrentTime, this->NID, uniqueEncounters, this->currSociability);
	#endif

	this->currSociability = (this->alpha * (((double) uniqueEncounters) / windowSize)) + ((1 - this->alpha) * this->currSociability);

	for(i = 0; i < NSize; i++)
	{
		this->encounteredNodes[i] = false;
	}

	#ifdef SOCIABILITY_DEBUG
		printf(", currSociability = %f\n\n", this->currSociability);
	#endif

	return;
}
