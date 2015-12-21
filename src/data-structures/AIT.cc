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


#ifndef AIT_H
	#define AIT_H
	#include "AIT.h"
#endif

/* Description
 * -----------
 *  Average Inter-meeting Time
 *  This utility calculates the average time between the contacts of two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network. Finally, the utility value is normalized
 *  to be inside [0,1].
 */

AIT::AIT(int ID,int NodeSize): Utility(ID,NodeSize)
{
	this->NGaps=(int *)malloc(sizeof(int)*NodeSize);
	this->ActiveContacts=(int *)malloc(sizeof(int)*NodeSize);
	this->TotalAIT=(double *)malloc(sizeof(double)*NodeSize);
	this->LastContactTime=(double *)malloc(sizeof(double)*NodeSize);
	this->AITval=(double *)malloc(sizeof(double)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->NGaps[i]=0;
		this->ActiveContacts[i]=0;
		this->LastContactTime[i]=0.0;
		this->TotalAIT[i]=0.0;
		this->AITval[i]=0.0;
	}
	return;
}

AIT::~AIT()
{
	free(NGaps);
	free(ActiveContacts);
	free(TotalAIT);
	free(LastContactTime);
	free(AITval);
	return;
}


void AIT::Update(int ID,double CurrentTime)
{
	return;
}

void AIT::ContactUp(int ID, double CurrentTime)
{
	//printf("%.1f:Contact between %d and %d is activated\n",CurrentTime,this->NID,ID);
	this->ActiveContacts[ID]++;
	if(this->ActiveContacts[ID] > 1)
	{
		//Case a contact between the two nodes is still up
		//Some traces have artifacts of this type 
		//printf("Problem: %f at node %d contact with %d is already up\n",CurrentTime,this->NID,ID);
		return;
	}
	this->NGaps[ID]++;
	if(this->NGaps[ID] <= 1)
	{
		//For the first contact occurrence
		//Last Contact Time between the two nodes does not exist
		return;
	}
	double diff=CurrentTime- this->LastContactTime[ID];
	this->TotalAIT[ID]+=diff;
	this->AITval[ID]=this->TotalAIT[ID]/(double)this->NGaps[ID];
	return;
}

void AIT::ContactDown(int ID, double CurrentTime)
{
	this->ActiveContacts[ID]--;
	if(this->ActiveContacts[ID])
	{
		return;
	}
	this->LastContactTime[ID]=CurrentTime;
	return;
}

double AIT::get(int ID,double CurrentTime)
{
	if(this->NGaps[ID] <= 1)
	{
		return 0.0;
	}
	return (1.0/(1.0+this->AITval[ID]));
}
