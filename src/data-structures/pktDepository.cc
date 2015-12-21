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


#ifndef DEPOSITORIES_H
	#define DEPOSITORIES_H
	#include "pktDepository.h"
#endif


pktDepository::pktDepository(int nodeID, int maxSize)
{
	limit=maxSize;
	NID=nodeID;
	return;
}

unlimitedDepository::unlimitedDepository(int nodeID, int maxSize): pktDepository(nodeID,maxSize)
{
	return;
}


void unlimitedDepository::init(void )
{
	Pkts.clear();
	return;
}


void unlimitedDepository::insert(int ID)
{
	Pkts.insert(ID);
	return;
}


bool unlimitedDepository::lookUp(int ID)
{
	set<int>::iterator it=Pkts.find(ID);
	if(it == Pkts.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bloomDepository::bloomDepository(int nodeID,int maxSize): pktDepository(nodeID,maxSize)
{
	numElements=0;
	filter=NULL;
	return;
}

double calcBloomSize(double n,double k,double p)
{
	double m = 0.0;
	double numerator = (- k * n);
    double denominator = std::log(1.0 - std::pow(p, 1.0 / k));
    m = numerator / denominator;
	return m;
}

void bloomDepository::init(double n, double k, double p,int numF)
{
	limit=(int)n;
	//How many elements roughly do we expect to insert?
	parameters.projected_element_count = (unsigned long long int)n;
	//How many hash functions will be used 
	parameters.optimal_parameters.number_of_hashes=(unsigned int)k;
	//Maximum tolerable false positive probability? (0,1)
	parameters.false_positive_probability = p;
	//Simple randomizer (optional)
	parameters.random_seed = 0xA5A5A5A5;
	if (!parameters)
	{
		printf("Error - Invalid set of bloom filter parameters!\n");
		exit(1);
	}
	//Calculate the bloom filter size
	double bloomSize=calcBloomSize(n,k,p);
	parameters.optimal_parameters.table_size=(unsigned long long int)bloomSize;
	parameters.optimal_parameters.table_size += (((parameters.optimal_parameters.table_size % bits_per_char) != 0) ? (bits_per_char - (parameters.optimal_parameters.table_size % bits_per_char)) : 0);
	//Calculate best number of hash functions in order to minimize the bloom filter size
	//parameters.compute_optimal_parameters();
	//Instantiate Bloom Filter
	filter=new bloom_filter(parameters);
	if(!NID)
	{
		printf("Number of elements: %.0f\n",n);
		printf("False positive probability: %f\n",p);
		printf("Number of hashes: %.0f\n",k);
		printf("Bloom filter size: %.0f bits - %.0f Bytes - %.2f KB\n",ceil(bloomSize),ceil(bloomSize/8.0),bloomSize/(8.0*1024.0));
	}
	return;
}


void bloomDepository::init(void)
{
	double n=1000;
	double k=5;
	double p=0.001;// 1 in 1000
	init(n,k,p,1);
}


void bloomDepository::insert(int ID)
{
	//first check if ID is already inserted
	if(lookUp(ID))
	{
		return;
	}
	if(numElements > limit)
	{
		filter->clear();
		numElements=0;
	}
	size_t element=(size_t)ID;
	filter->insert(element);
	numElements++;
	#ifdef BLOOM_DEBUG
	printf("Inserted PID %d\n",element);
	#endif
	return;
}

bool bloomDepository::lookUp(int ID)
{
	size_t element=(size_t)ID;
	#ifdef BLOOM_DEBUG
	printf("Looking for PID %d..",element);
	#endif
	if(filter->contains(element))
	{
		#ifdef BLOOM_DEBUG
		printf("found!\n");
		#endif
		return true;
	}
	else
	{
		#ifdef BLOOM_DEBUG
		printf("not found!\n");
		#endif
		return false;
	}
}

multiBloomDepository::multiBloomDepository(int nodeID,int maxSize): pktDepository(nodeID,maxSize)
{
	numFilters=0;
	enabledFilter=0;
	numElements=NULL;
	return;
}

void multiBloomDepository::init(double n, double k, double p,int numF)
{
	limit=(int)n;
	//How many elements roughly do we expect to insert?
	parameters.projected_element_count = (unsigned long long int)n;
	//How many hash functions will be used 
	parameters.optimal_parameters.number_of_hashes=(unsigned int)k;
	//Maximum tolerable false positive probability? (0,1)
	parameters.false_positive_probability = p;
	//Simple randomizer (optional)
	parameters.random_seed = 0xA5A5A5A5;
	if (!parameters)
	{
		printf("Error - Invalid set of bloom filter parameters!\n");
		exit(1);
	}
	//Calculate the bloom filter size
	double bloomSize=calcBloomSize(n,k,p);
	parameters.optimal_parameters.table_size=(unsigned long long int)bloomSize;
	parameters.optimal_parameters.table_size += (((parameters.optimal_parameters.table_size % bits_per_char) != 0) ? (bits_per_char - (parameters.optimal_parameters.table_size % bits_per_char)) : 0);
	
	//Instantiate the Bloom Filters
	numFilters=numF;
	numElements=(int *)malloc(sizeof(int)*numF);
	bloom_filter *tmp=NULL;
	for(int i=0;i<numF;i++)
	{
		tmp=new bloom_filter(parameters);
		filters.push_back(tmp);
		numElements[i]=0;
	}
	if(!NID)
	{
		printf("Number of bloom filters: %d\n",numF);
		printf("Number of elements: %.0f\n",n);
		printf("False positive probability: %f\n",p);
		printf("Number of hashes: %.0f\n",k);
		printf("Total Bloom filter size: %d x %.0f bits - %d x %.0f Bytes - %d x %.2f KB\n",numF,ceil(bloomSize),numF,ceil(bloomSize/8.0),numF,bloomSize/(8.0*1024.0));
	}
	return;
}


void multiBloomDepository::init(void)
{
	double n=250;
	double k=2;
	double p=0.001;// 1 in 1000
	numFilters=2;//should be always >= 1
	
	init(n,k,p,numFilters);
}

void multiBloomDepository::insert(int ID)
{
	//first check if ID is already inserted
	if(lookUp(ID))
	{
		return;
	}
	if(numElements[enabledFilter] > limit)
	{
		enabledFilter++;
		enabledFilter=enabledFilter%numFilters;
		if(numElements[enabledFilter] > limit)
		{
			filters[enabledFilter]->clear();
			numElements[enabledFilter]=0;
		}
	}
	size_t element=(size_t)ID;
	filters[enabledFilter]->insert(element);
	numElements[enabledFilter]++;
	return;
}

bool multiBloomDepository::lookUp(int ID)
{
	size_t element=(size_t)ID;
	#ifdef BLOOM_DEBUG
	printf("Looking for PID %d inside %d bloom filters\n",element);
	#endif
	for(int i=0;i<numFilters;i++)
	{
		#ifdef BLOOM_DEBUG
		printf("Checking %d bloom filter..",i+1);
		#endif
		if(filters[i]->contains(element))
		{
			#ifdef BLOOM_DEBUG
			printf("found!\n");
			#endif
			return true;
		}
		else
		{
			#ifdef BLOOM_DEBUG
			printf("not found!\n");
			#endif
		}
	}
	return false;
}






