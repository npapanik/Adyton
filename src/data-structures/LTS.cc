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


#ifndef LTS_H
	#define LTS_H
	#include "LTS.h"
#endif


LTS::LTS(int ID, int NodeSize): Utility(ID,NodeSize)
{
	int i;


	this->activeContacts = (int *) malloc(NSize * sizeof(int));
	this->LastTimeSeen = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		this->activeContacts[i] = 0;
		this->LastTimeSeen[i] = -1.0;
	}

	return;
}

LTS::~LTS()
{
	free(activeContacts);
	free(LastTimeSeen);
	return;
}



void LTS::Update(int ID, double CurrentTime)
{
	return;
}


void LTS::ContactUp(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("LTS utility: Node %d does not exist!\nAborting...\n", ID);
		exit(EXIT_FAILURE);
	}


	/* Keep track of the active contacts */
	this->activeContacts[ID]++;

	return;
}


void LTS::ContactDown(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("LTS utility: Node %d does not exist!\nAborting...\n", ID);
		exit(EXIT_FAILURE);
	}


	/* Keep track of the time this node was last seen */
	this->LastTimeSeen[ID] = CurrentTime;


	/* Keep track of the active contacts */
	this->activeContacts[ID]--;

	return;
}


double LTS::get(int ID, double CurrentTime)
{/* This method does not take into account the currently active contacts */
	double tmp;


	if(this->LastTimeSeen[ID] < 0.0)
	{
		return 0.0;
	}
	else
	{
		tmp = 1.0 / (1.0 + (CurrentTime - this->LastTimeSeen[ID]));

		if(!(tmp >= 0.0 && tmp <= 1.0))
		{
			printf("[%d->%d]\n", this->NID, ID);
			printf("Current Time: %f, Last Time Seen: %f\n", CurrentTime, this->LastTimeSeen[ID]);
			printf("Value of LTS utility is %f\n", tmp);
			printf("LTS Utility value is not between 0.0 and 1.0\nAborting...\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			return tmp;
		}
	}
}


double LTS::getUtility(int ID, double CurrentTime)
{
	double tmp;


	if(this->LastTimeSeen[ID] < 0.0)
	{
		return 0.0;
	}
	else if(activeContacts[ID] > 0)
	{
		return 1.0;
	}
	else
	{
		tmp = 1.0 / (1.0 + (CurrentTime - this->LastTimeSeen[ID]));

		if(!(tmp >= 0.0 && tmp <= 1.0))
		{
			printf("[%d->%d]\n", this->NID, ID);
			printf("Current Time: %f, Last Time Seen: %f\n", CurrentTime, this->LastTimeSeen[ID]);
			printf("Value of LTS utility is %f\n", tmp);
			printf("LTS Utility value is not between 0.0 and 1.0\nAborting...\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			return tmp;
		}
	}
}


double* LTS::getUtilities(double CurrentTime)
{
	int i;
	double* ltsUtilities;


	ltsUtilities = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		if(this->LastTimeSeen[i] < 0.0)
		{
			ltsUtilities[i] = 0.0;
		}
		else if(activeContacts[i] > 0)
		{
			ltsUtilities[i] = 1.0;
		}
		else
		{
			ltsUtilities[i] = 1.0 / (1.0 + (CurrentTime - this->LastTimeSeen[i]));
		}

		if(!(ltsUtilities[i] >= 0.0 && ltsUtilities[i] <= 1.0))
		{
			printf("[%d->%d]\n", this->NID, i);
			printf("Current Time: %f, Last Time Seen: %f\n", CurrentTime, this->LastTimeSeen[i]);
			printf("Value of LTS utility is %f\n", ltsUtilities[i]);
			printf("LTS Utility value is not between 0.0 and 1.0\nAborting...\n");
			exit(EXIT_FAILURE);
		}
	}

	return ltsUtilities;
}


double LTS::getActualLTS(int ID, double CurrentTime)
{
	if(this->LastTimeSeen[ID] < 0)
	{
		return -1.0;
	}
	else
	{
		return CurrentTime - this->LastTimeSeen[ID];
	}
}
