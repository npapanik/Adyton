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


#ifndef DESTENC_H
	#define DESTENC_H
	#include "DestEnc.h"
#endif

/* Description
 * -----------
 *  Destination Encounters
 *  This utility calculates the number of contacts between two nodes.
 *  It is destination dependent, so each node calculates this value
 *  for every other node in the network.
 */

DestEnc::DestEnc(int ID,int NodeSize,double a,double W): Utility(ID,NodeSize)
{
	this->alpha=a;
	this->Window=W;
	this->stage=0;
	this->LastUpdated=(double *)malloc(sizeof(double)*NodeSize);
	this->EV=(double *)malloc(sizeof(double)*NodeSize);
	this->CWC=(double *)malloc(sizeof(double)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->LastUpdated[i]=0.0;
		this->EV[i]=0.0;
		this->CWC[i]=0.0;
	}
	this->DecayEnabled=true;
	return;
}

DestEnc::DestEnc(int ID,int NodeSize): Utility(ID,NodeSize)
{
	this->alpha=0.0;
	this->Window=0.0;
	this->stage=0;
	this->LastUpdated=(double *)malloc(sizeof(double)*NodeSize);
	this->EV=(double *)malloc(sizeof(double)*NodeSize);
	this->CWC=(double *)malloc(sizeof(double)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->LastUpdated[i]=0.0;
		this->EV[i]=0.0;
		this->CWC[i]=0.0;
	}
	this->DecayEnabled=false;
	return;
}

DestEnc::~DestEnc()
{
	free(LastUpdated);
	free(EV);
	free(CWC);
	return;
}


void DestEnc::Update(int ID,double CurrentTime)
{
	return;
}

void DestEnc::ContactUp(int ID, double CurrentTime)
{
	if(Reset(ID,CurrentTime) && this->DecayEnabled)
	{
		this->EV[ID]=(this->alpha*this->CWC[ID])+((1-this->alpha)*this->EV[ID]);
		this->CWC[ID]=0.0;
		this->stage++;
	}
	this->CWC[ID]++;
}


double DestEnc::get(int ID,double CurrentTime)
{
	if(!(this->stage) || !(this->DecayEnabled))
	{
		return this->CWC[ID];
	}
	else
	{
		return this->EV[ID];
	}
}

bool DestEnc::Reset(int ID,double CurrentTime)
{
	if((CurrentTime - LastUpdated[ID]) > this->Window)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isChosen(int *set,int num,int cand)
{
	for(int i=0;i<num;i++)
	{
		if(set[i] == cand)
		{
			return true;
		}
	}
	return false;
}

int *DestEnc::topValues(int num)
{
	int *highest=(int *)malloc(sizeof(int)*num);
	double *values=(double *)malloc(sizeof(double)*num);
	
	for(int i=0;i<num;i++)
	{
		highest[i]=-1;
	}
	
	int ID=0;
	double maxim=0.0;
	for(int j=0;j<num;j++)
	{
		ID=-1;
		maxim=0.0;
		for(int i=0;i<NSize;i++)
		{
			if(isChosen(highest,num,i) || i == NID)
			{
				continue;
			}
			if(maxim < this->CWC[i])
			{
				maxim=CWC[i];
				ID=i;
			}
		}
		if(maxim == 0.0)
		{
			break;
		}
		else
		{
			highest[j]=ID;
			values[j]=maxim;
		}
	}
	
	//printf("Friend nodes are: ");
	for(int j=0;j<num;j++)
	{
		if(highest[j] == -1)
		{
			break;
		}
		//printf("%d(val:%.1f) ",highest[j],values[j]);
	}
	//printf("\n");
	free(values);
	
	return highest;
}
