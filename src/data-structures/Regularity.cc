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


#ifndef REGULARITY_H
	#define REGULARITY_H
	#include "Regularity.h"
#endif

/* Description
 * -----------
 *  Regularity
 *  This utility calculates the exponential moving average
 *  of the total duration of contacts between two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network.
 */


Regularity::Regularity(int ID, int NodeSize): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->lastTimePeriod = 0;
	this->lastUpdate = 0.0;
	this->alpha = DEFAULT_ALPHA;
	this->windowSize = DEFAULT_WINDOWSIZE;

	this->prevNumberContacts = (unsigned long int *) malloc(NodeSize * sizeof(unsigned long int));
	this->currRegularity = (double *) malloc(NodeSize * sizeof(double));
	this->EBRutility = 0.0;

	for(i = 0; i < NodeSize; i++)
	{
		this->prevNumberContacts[i] = 0;
		this->currRegularity[i] = 0.0;
	}

	return;
}


Regularity::Regularity(int ID, int NodeSize, double a, double windowS): Utility(ID, NodeSize)
{
	int i;


	/* Initializations */
	this->lastTimePeriod = 0;
	this->lastUpdate = 0.0;
	this->alpha = a;
	this->windowSize = windowS;

	this->prevNumberContacts = (unsigned long int *) malloc(NodeSize * sizeof(unsigned long int));
	this->currRegularity = (double *) malloc(NodeSize * sizeof(double));
	this->EBRutility = 0.0;

	for(i = 0; i < NodeSize; i++)
	{
		this->prevNumberContacts[i] = 0;
		this->currRegularity[i] = 0.0;
	}

	return;
}

Regularity::~Regularity()
{
	free(prevNumberContacts);
	free(currRegularity);
	return;
}

void Regularity::Update(int ID, double CurrentTime)
{
	unsigned long int i;
	unsigned long int currTimePeriod;


	if(CurrentTime < this->lastUpdate)
	{ 
		printf("\nRegularity: Error! Time is going backwards!\n\n");
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
			updateRegularities((CurrentTime - fmod(CurrentTime, this->windowSize)) - ((i - 1) * windowSize));
		}

		this->lastTimePeriod = currTimePeriod;
	}
	else
	{
		printf("\nRegularity: Error! Time is going backwards!\n\n");
		exit(EXIT_FAILURE);
	}

	return;
}


void Regularity::ContactUp(int ID, double CurrentTime)
{
	#ifdef REGULARITY_DEBUG
		printf("@%f Node %d: Contact-Up with node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);


	/* Count the number of contacts in this window */
	this->prevNumberContacts[ID]++;

	return;
}


void Regularity::ContactDown(int ID, double CurrentTime)
{
	#ifdef REGULARITY_DEBUG
		printf("@%f Node %d: Contact-Down with node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);

	return;
}


double Regularity::get(int ID, double CurrentTime)
{
	#ifdef REGULARITY_DEBUG
		printf("@%f Node %d: Get Regularity utility of node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);

	return currRegularity[ID];
}


double Regularity::getEBRutility(int ID, double CurrentTime)
{
	#ifdef REGULARITY_DEBUG
		printf("@%f Node %d: Get EBR utility of node %d\n", CurrentTime, this->NID, ID);
	#endif


	/* Update the utility values */
	Update(ID, CurrentTime);

	return EBRutility;
}


void Regularity::updateRegularities(double CurrentTime)
{
	int i;
	unsigned long int prevSumContacts;


	prevSumContacts = 0;
	for(i = 0; i < NSize; i++)
	{
		#ifdef REGULARITY_DEBUG
			printf("@%f Node %d: prevNumberContacts[%d] = %u, prevRegularity[%d] = %f", CurrentTime, this->NID, i, this->prevNumberContacts[i], i, this->currRegularity[i]);
		#endif

		this->currRegularity[i] = (this->alpha * this->prevNumberContacts[i]) + ((1 - this->alpha) * this->currRegularity[i]);
		prevSumContacts += this->prevNumberContacts[i];
		this->prevNumberContacts[i] = 0;

		#ifdef REGULARITY_DEBUG
			printf(", currRegularity[%d] = %f\n", i, this->currRegularity[i]);
		#endif
	}

	this->EBRutility = (this->alpha * ((double) prevSumContacts)) + ((1 - this->alpha) * this->EBRutility);

	#ifdef REGULARITY_DEBUG
		printf("\n");
	#endif

	return;
}
