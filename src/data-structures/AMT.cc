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
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#ifndef AMT_H
	#define AMT_H
	#include "AMT.h"
#endif


/* Description
 * -----------
 *  Average Meeting Time
 *  This utility calculates the average meeting time of two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network. Finally, the utility value is normalized
 *  to be inside [0,1].
 */

AMT::AMT(int ID,int NodeSize): Utility(ID,NodeSize)
{
	int i;


	this->Ncontacts = (int *) malloc(NodeSize * sizeof(int));
	this->ActiveContacts = (int *) malloc(NodeSize * sizeof(int));
	this->TotalAMT = (double *) malloc(NodeSize * sizeof(double));
	this->ContactStart = (double *) malloc(NodeSize * sizeof(double));
	this->AMTval = (double *) malloc(NodeSize * sizeof(double));

	for(i = 0; i < NodeSize; i++)
	{
		this->ActiveContacts[i] = 0;
		this->ContactStart[i] = 0.0;
		this->TotalAMT[i] = 0.0;
		this->AMTval[i] = 0.0;
		this->Ncontacts[i] = 0;
	}

	return;
}


AMT::~AMT()
{
	free(Ncontacts);
	free(ActiveContacts);
	free(TotalAMT);
	free(ContactStart);
	free(AMTval);

	return;
}


void AMT::Update(int ID,double CurrentTime)
{
	return;
}


void AMT::ContactUp(int ID, double CurrentTime)
{
	if(this->ActiveContacts[ID])
	{
		this->ActiveContacts[ID]++;
		return;
	}

	this->ActiveContacts[ID] = 1;
	this->ContactStart[ID] = CurrentTime;

	return;
}

void AMT::ContactDown(int ID, double CurrentTime)
{
	double diff;


	if(!this->ActiveContacts[ID])
	{
		printf("AMT:%.1f-Error contact with node %d never started!(base node:%d)\n", CurrentTime, ID, this->NID);
		exit(1);
	}

	this->ActiveContacts[ID]--;

	if(this->ActiveContacts[ID] > 0)
	{
		return;
	}

	diff = CurrentTime - this->ContactStart[ID];
	this->TotalAMT[ID] += diff;
	this->Ncontacts[ID]++;
	this->AMTval[ID] = this->TotalAMT[ID] / ((double) Ncontacts[ID]);

	//reset contact data
	this->ActiveContacts[ID]=0;
	this->ContactStart[ID]=0.0;

	return;
}

double AMT::get(int ID,double CurrentTime)
{
	if(this->Ncontacts[ID] == 0)
	{
		return 0.0;
	}

	return (this->AMTval[ID]);
}

double AMT::getTotalDuration(int NID)
{
	return this->TotalAMT[NID];
}

double AMT::getDstContacts(int NID)
{
	return (double) this->Ncontacts[NID];
}

double* AMT::getDstContacts()
{
	int i;
	double* dstContacts;


	dstContacts = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		dstContacts[i] = (double) this->Ncontacts[i];
	}

	return dstContacts;
}

double AMT::getTotalContacts()
{
	int i;
	double totContacts;


	totContacts = 0;
	for(i = 0; i < NSize; i++)
	{
		totContacts += (double) this->Ncontacts[i];
	}

	return totContacts;
}

double AMT::getFriendshipIndex(int NID, double CurrentTime)
{
	if(CurrentTime == 0.0)
	{
		return 0.0;
	}
	else
	{
		return this->TotalAMT[NID] / CurrentTime;
	}
}


double* AMT::getFriendshipIndices(double CurrentTime)
{
	int i;
	double* friendshipIndices;


	friendshipIndices = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		if(CurrentTime == 0.0)
		{
			friendshipIndices[i] = 0.0;
		}
		else
		{
			friendshipIndices[i] = this->TotalAMT[i] / CurrentTime;
		}
	}

	return friendshipIndices;
}


double AMT::getAMTval(int NID)
{
	if(this->Ncontacts[NID] == 0)
	{
		return 0.0;
	}

	return (this->AMTval[NID]);
}


double* AMT::getAverageDurations()
{
	int i;
	double* avgDuration;


	avgDuration = (double *) malloc(NSize * sizeof(double));

	for(i = 0; i < NSize; i++)
	{
		avgDuration[i] = this->AMTval[i];
	}

	return avgDuration;
}
