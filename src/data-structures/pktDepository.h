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


#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <cmath>
#include <partow/bloom_filter.hpp>

using namespace std;

class pktDepository
{
protected:
	int limit;
	int NID;
public:
	pktDepository(int nodeID, int maxSize);
	virtual void init(double n,double k,double p,int numF)=0;
	virtual void init(void)=0;
	virtual void insert(int ID)=0;
	virtual bool lookUp(int ID)=0;
};


class unlimitedDepository:public pktDepository
{
protected:
	set<int> Pkts;
public:
	unlimitedDepository(int nodeID,int maxSize);
	virtual void init(double n,double k,double p,int numF){return;};
	virtual void init(void);
	virtual void insert(int ID);
	virtual bool lookUp(int ID);
};


class bloomDepository:public pktDepository
{
protected:
	bloom_parameters parameters;
	bloom_filter *filter;
	int numElements;
public:
	bloomDepository(int nodeID,int maxSize);
	virtual void init(double n,double k,double p,int numF);
	virtual void init(void);
	virtual void insert(int ID);
	virtual bool lookUp(int ID);
};

class multiBloomDepository:public pktDepository
{
protected:
	int numFilters;
	int enabledFilter;
	bloom_parameters parameters;
	vector<bloom_filter*> filters;
	int *numElements;
public:
	multiBloomDepository(int nodeID,int maxSize);
	virtual void init(double n,double k,double p,int numF);
	virtual void init(void);
	virtual void insert(int ID);
	virtual bool lookUp(int ID);
};
