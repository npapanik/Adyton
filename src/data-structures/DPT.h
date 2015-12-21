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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>


#ifndef ASP_H
	#define ASP_H
	#include "ASP.h"
#endif



class DPT
{
protected:
	int nodeID;
	int numNodes;

	double Pinit;
	double Pmin;
	double Pmax;
	double beta;
	double gamma;
	double delta;
	double agingTimeUnit;
	double lastTimeAged;

	ASP *AverageSeparationPeriod;
	double *myDPT;
	
public:
	DPT(int nID, int nSize);
	virtual ~DPT();
	void setPinit(double p);
	void setPmin(double p);
	void setPmax(double p);
	void setBeta(double b);
	void setGamma(double g);
	void setDelta(double d);
	void setAgingTimeUnit(double numSeconds);
	void ContactUp(int encNode, double CurrentTime);
	void ContactDown(int encNode, double CurrentTime);
	double getDPto(int destNode, double currTime);
	double *CloneDPT(double currTime);
	double *CalcRepValue(double *encDPT, double currTime);
	int *CheckDPs(double *encDPT, double currTime);
	void Aging(double currTime);
	virtual void UpdateDPT(double *encDPT, int encNode, double currTime) = 0;
};


class DPTv1:public DPT
{
public:
	DPTv1(int nID, int nSize);
	~DPTv1();
	virtual void UpdateDPT(double *encDPT, int encNode, double currTime);
};


class DPTv1point5:public DPT
{
public:
	DPTv1point5(int nID, int nSize);
	~DPTv1point5();
	virtual void UpdateDPT(double *encDPT, int encNode, double currTime);
};


class DPTv2:public DPT
{
public:
	DPTv2(int nID, int nSize);
	~DPTv2();
	virtual void UpdateDPT(double *encDPT, int encNode, double currTime);
};


class DPTv3:public DPT
{
public:
	DPTv3(int nID, int nSize);
	~DPTv3();
	virtual void UpdateDPT(double *encDPT, int encNode, double currTime);
};
