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


#ifndef DPT_H
	#define DPT_H
	#include "DPT.h"
#endif


DPT::DPT(int nID, int nSize)
{
	int i;


	this->nodeID = nID;
	this->numNodes = nSize;

	this->Pinit = 0.5;
	this->Pmin = 0.1;
	this->Pmax = 0.7;
	this->beta = 0.9;
	this->gamma = 0.999;
	this->delta = 0.01;
	this->agingTimeUnit = 3600.0;
	this->lastTimeAged = 0.0;

	this->AverageSeparationPeriod = new ASP(this->nodeID, this->numNodes);
	this->myDPT = (double *) malloc(this->numNodes * sizeof(double));

	for(i = 0; i < this->numNodes; i++)
	{
		if(i == this->nodeID)
		{
			this->myDPT[i] = 1.0;
		}
		else
		{
			this->myDPT[i] = 0.0;
		}
	}

	return;
}

DPT::~DPT()
{
	delete AverageSeparationPeriod;
	free(myDPT);
	return;
}

DPTv1::DPTv1(int nID, int nSize): DPT(nID, nSize)
{
	return;
}

DPTv1::~DPTv1()
{
	return;
}

DPTv1point5::DPTv1point5(int nID, int nSize): DPT(nID, nSize)
{
	return;
}

DPTv1point5::~DPTv1point5()
{
	return;
}

DPTv2::DPTv2(int nID, int nSize): DPT(nID, nSize)
{
	return;
}

DPTv2::~DPTv2()
{
	return;
}

DPTv3::DPTv3(int nID, int nSize): DPT(nID, nSize)
{
	return;
}

DPTv3::~DPTv3()
{
	return;
}

void DPT::setPinit(double p)
{
	this->Pinit = p;

	return;
}


void DPT::setPmin(double p)
{
	this->Pmin = p;

	return;
}


void DPT::setPmax(double p)
{
	this->Pmax = p;

	return;
}


void DPT::setBeta(double b)
{
	this->beta = b;

	return;
}


void DPT::setGamma(double g)
{
	this->gamma = g;

	return;
}


void DPT::setDelta(double d)
{
	this->delta = d;

	return;
}


void DPT::setAgingTimeUnit(double numSeconds)
{
	this->agingTimeUnit = numSeconds;

	return;
}


void DPT::ContactUp(int encNode, double CurrentTime)
{
	this->AverageSeparationPeriod->ContactUp(encNode, CurrentTime);

	return;
}


void DPT::ContactDown(int encNode, double CurrentTime)
{
	this->AverageSeparationPeriod->ContactDown(encNode, CurrentTime);

	return;
}


double DPT::getDPto(int destNode, double currTime)
{
	if((destNode < 0) || (destNode >= this->numNodes))
	{
		printf("Error: Invalid destination node ID (must lie inside [0,%d] - given %d)!\nExiting...\n",this->numNodes,destNode);
		exit(1);
	}


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);

	return this->myDPT[destNode];
}


double* DPT::CloneDPT(double currTime)
{
	double *tmp;


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);

	tmp = (double *) malloc(this->numNodes * sizeof(double));
	memcpy(tmp, this->myDPT, this->numNodes * sizeof(double));

	return tmp;
}


double* DPT::CalcRepValue(double* encDPT, double currTime)
{
	int i;
	double *reps;


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);

	reps = (double *) malloc(this->numNodes * sizeof(double));
	for(i = 0; i < this->numNodes; i++)
	{
		if(this->myDPT[i] < encDPT[i])
		{
			reps[i] = encDPT[i] / (encDPT[i] + this->myDPT[i]);
		}
		else
		{
			reps[i] = 0.0;
		}
	}

	return reps;
}


int* DPT::CheckDPs(double* encDPT, double currTime)
{
	int i;
	int pos;
	int *dests;


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);

	pos = 0;
	dests = (int *) malloc(sizeof(int));
	dests[pos] = 0;
	for(i = 0; i < this->numNodes; i++)
	{
		if(encDPT[i] - this->myDPT[i] > DBL_EPSILON)
		{
			pos++;
			dests = (int *) realloc(dests, (pos + 1) * sizeof(int));
			dests[pos] = i;
			dests[0] = pos;
		}
	}

	return dests;
}


void DPT::Aging(double currTime)
{
	int i;
	double kappa;


	if(currTime < this->lastTimeAged)
	{
		printf("Error: Time is going backwards!\nExiting...\n");
		exit(1);
	}


	/* Calculate the number of time units that have elapsed since the last time the delivery predictabilities were aged */
	kappa = (currTime - this->lastTimeAged) / this->agingTimeUnit;

	if(kappa > DBL_EPSILON)
	{
		for(i = 0; i < this->numNodes; i++)
		{
			if(i == this->nodeID)
			{
				continue;
			}

			this->myDPT[i] = this->myDPT[i] * pow(this->gamma, kappa);
		}


		/* Keep track of the last time the delivery predictabilities were aged */
		this->lastTimeAged = currTime;
	}

	return;
}


void DPTv1::UpdateDPT(double* encDPT, int encNode, double currTime)
{
	int i;


	if((encNode < 0) || (encNode >= this->numNodes))
	{
		printf("Error: Invalid encountered node ID!\nExiting...\n");
		exit(1);
	}


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);


	/* Update the delivery predictability of the encountered node */
	this->myDPT[encNode] = this->myDPT[encNode] + ((1 - this->myDPT[encNode]) * this->Pinit);


	/* Update the delivery predictabilities of the other nodes due to the transitive property */
	for(i = 0; i < this->numNodes; i++)
	{
		if((i == this->nodeID) || (i == encNode))
		{
			continue;
		}

		this->myDPT[i] = this->myDPT[i] + ((1 - this->myDPT[i]) * this->myDPT[encNode] * encDPT[i] * this->beta);
	}

	return;
}


void DPTv1point5::UpdateDPT(double* encDPT, int encNode, double currTime)
{
	int i;
	double tmpProduct;


	if((encNode < 0) || (encNode >= this->numNodes))
	{
		printf("Error: Invalid encountered node ID!\nExiting...\n");
		exit(1);
	}


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);


	/* Update the delivery predictability of the encountered node */
	this->myDPT[encNode] = this->myDPT[encNode] + ((1 - this->myDPT[encNode]) * this->Pinit);


	/* Update the delivery predictabilities of the other nodes due to the transitive property */
	for(i = 0; i < this->numNodes; i++)
	{
		if((i == this->nodeID) || (i == encNode))
		{
			continue;
		}

		tmpProduct = encDPT[i] * this->myDPT[encNode] * this->beta;

		if(tmpProduct - this->myDPT[i] > DBL_EPSILON)
		{
			this->myDPT[i] = tmpProduct;
		}
	}

	return;
}


void DPTv2::UpdateDPT(double* encDPT, int encNode, double currTime)
{
	int i;
	double Penc;
	double intvl;
	double Ityp;
	double tmpProduct;


	if((encNode < 0) || (encNode >= this->numNodes))
	{
		printf("Error: Invalid encountered node ID!\nExiting...\n");
		exit(1);
	}


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);


	/* Calculate Penc */
	intvl = this->AverageSeparationPeriod->getTimeDisconnected(encNode, currTime);
	Ityp = this->AverageSeparationPeriod->getActualASP(encNode, currTime);

	if((Ityp == -1.0) || (intvl - Ityp > DBL_EPSILON))
	{
		Penc = this->Pmax;
	}
	else
	{
		Penc = this->Pmax * (intvl / Ityp);
	}


	/* Update the delivery predictability of the encountered node */
	this->myDPT[encNode] = this->myDPT[encNode] + ((1 - this->myDPT[encNode]) * Penc);


	/* Update the delivery predictabilities of the other nodes due to the transitive property */
	for(i = 0; i < this->numNodes; i++)
	{
		if((i == this->nodeID) || (i == encNode))
		{
			continue;
		}

		tmpProduct = encDPT[i] * this->myDPT[encNode] * this->beta;

		if(tmpProduct - this->myDPT[i] > DBL_EPSILON)
		{
			this->myDPT[i] = tmpProduct;
		}
	}

	return;
}


void DPTv3::UpdateDPT(double* encDPT, int encNode, double currTime)
{
	int i;
	double Penc;
	double intvl;
	double Ityp;
	double tmpProduct;


	if((encNode < 0) || (encNode >= this->numNodes))
	{
		printf("Error: Invalid encountered node ID!\nExiting...\n");
		exit(1);
	}


	/* Update the delivery predictabilities due to aging */
	this->Aging(currTime);


	/* Update the delivery predictability of the encountered node */
	if(this->Pmin - this->myDPT[encNode] > DBL_EPSILON)
	{
		this->myDPT[encNode] = this->Pinit;
	}
	else
	{
		intvl = this->AverageSeparationPeriod->getTimeDisconnected(encNode, currTime);
		Ityp = this->AverageSeparationPeriod->getActualASP(encNode, currTime);

		if((Ityp == -1.0) || (intvl - Ityp > DBL_EPSILON))
		{
			Penc = this->Pmax;
		}
		else
		{
			Penc = this->Pmax * (intvl / Ityp);
		}

		this->myDPT[encNode] = this->myDPT[encNode] + ((1 - this->delta - this->myDPT[encNode]) * Penc);
	}


	/* Update the delivery predictabilities of the other nodes due to the transitive property */
	for(i = 0; i < this->numNodes; i++)
	{
		if((i == this->nodeID) || (i == encNode))
		{
			continue;
		}

		tmpProduct = this->myDPT[encNode] * encDPT[i] * this->beta;

		if(tmpProduct - this->myDPT[i] > DBL_EPSILON)
		{
			this->myDPT[i] = tmpProduct;
		}
	}

	return;
}
