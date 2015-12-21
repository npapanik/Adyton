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


#ifndef ADJA_H
	#define ADJA_H
	#include "Adjacency.h"
#endif

// #define ADJ_DEBUG 


DrawCandidate::DrawCandidate(int NodeID, int Cover, bool Old,int Deg,int pos)
{
	this->ID=NodeID;
	this->CoverSize=Cover;
	this->IsOldFW=Old;
	this->Degree=Deg;
	this->position=pos;
	return;
}


void PrintBitSet(dynamic_bitset<> bs)
{
	for(unsigned int i=0;i<bs.size();i++)
	{
		if(bs.test(i))
		{
			printf(" 1");
		}
		else
		{
			printf(" 0");
		}
	}
	printf("\n");
	return;
}


Adjacency::Adjacency(int ID, int NodeSize)
{
	init(ID,NodeSize, 0, 0.0);

	return;
}


Adjacency::Adjacency(int ID, int NodeSize, int AggregationType, double argument)
{
	init(ID, NodeSize, AggregationType, argument);

	return;
}

Adjacency::~Adjacency()
{
	free(Similarities);
	free(DisSimilarities);
	free(numEncounters);
	free(activeContacts);
	free(timeConnected);
	free(lastTimeSeen);
	
	for(int i = 0; i < NSize; i++)
	{
		Nodes[i].clear();
	}
	Nodes.clear();
	delete CA;
	
	return;
}

void Adjacency::init(int ID,int NodeSize,int AggregationType,double argument)
{
	int i;


	this->NSize = NodeSize;
	this->NodeID = ID;
	this->Bet = 0.0;
	this->Effi = 0.0;
	this->totalNumEncounters = 0;
	this->totalTimeConnected = 0.0;
	this->lastUpdate = 0.0;
	this->Similarities = (int *) malloc(NodeSize * sizeof(int));
	this->DisSimilarities = (int *) malloc(NodeSize * sizeof(int));
	this->numEncounters = (int *) malloc(NodeSize * sizeof(int));
	this->activeContacts = (int *) malloc(NodeSize * sizeof(int));
	this->timeConnected = (double *) malloc(NodeSize * sizeof(double));
	this->lastTimeSeen = (double *) malloc(NodeSize * sizeof(double));
	
	for(i = 0; i < NodeSize; i++)
	{
		this->Similarities[i] = 0;
		this->DisSimilarities[i] = 0;
		this->numEncounters[i] = 0;
		this->timeConnected[i] = 0.0;
		this->lastTimeSeen[i] = -1.0;
		this->activeContacts[i] = 0;
	}
	
	for (int i = 0; i < NodeSize; i++) 
	{
		Nodes.push_back(dynamic_bitset<>(NodeSize));
	}
	
	for(i = 0; i < NodeSize; i++)
	{
		Nodes[i].resize(NodeSize);
		Nodes[i].reset();
	}
	
	switch(AggregationType)
	{
		case 0:
		{
			//Growing Time Window
			this->CA = new GrowingTimeWindow(&Nodes);
			break;
		}
		case 1:
		{
			//Most Frequent Contacts
			this->CA = new MFaggregation(NodeSize, &Nodes, argument);
			break;
		}
		case 2:
		{
			//Most Recent Contacts
			this->CA = new MRaggregation(NodeSize, &Nodes, argument);
			break;
		}
		case 3:
		{
			//Sliding Time Window
			this->CA = new SlidingWindow(&Nodes, NodeSize, argument);
			break;
		}
		default:
		{
			//Growing Time Window
			this->CA = new GrowingTimeWindow(&Nodes);
		}
	}

	return;
}


AdjacencyWeights::AdjacencyWeights(int ID,int NodeSize): Adjacency(ID, NodeSize)
{
	this->IntimacyWeight=(double**)malloc(sizeof(double*)*NodeSize);
	this->FreqWeight=(double**)malloc(sizeof(double*)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->IntimacyWeight[i]=(double *)malloc(sizeof(double)*NodeSize);
		this->FreqWeight[i]=(double *)malloc(sizeof(double)*NodeSize);
		for(int j=0;j<NodeSize;j++)
		{
			this->IntimacyWeight[i][j]=0.0;
			this->FreqWeight[i][j]=0.0;
		}
	}
	return;
}

AdjacencyWeights::~AdjacencyWeights()
{
	for(int i=0;i<NSize;i++)
	{
		free(IntimacyWeight[i]);
		free(FreqWeight[i]);
	}
	free(IntimacyWeight);
	free(FreqWeight);
	return;
}



void Adjacency::Update()
{
	this->CA->Update();

	return;
}


void Adjacency::SetConnection(int Node1, int Node2, double ConTime)
{
	this->CA->Connect(Node1, Node2, ConTime);

	return;
}


int *Adjacency::GetMyContacts()
{
	unsigned int i;
	int runner;
	int EgoN;
	int *Contacts;


	dynamic_bitset<> MyN(Nodes[this->NodeID]);
	EgoN = MyN.count();
	Contacts = (int *) malloc((EgoN + 1) * sizeof(int));
	Contacts[0] = EgoN;
	
	runner = 1;
	for(i = 0; i < MyN.size(); i++)
	{
		if(MyN.test(i))
		{
			Contacts[runner] = i;
			runner++;
		}
	}

	return Contacts;
}



double* Adjacency::getAllSim(void )
{
	double *Sims=(double *)malloc(sizeof(double)*this->NSize);
	for(int i=0;i<this->NSize;i++)
	{
		Sims[i]=(double)this->Similarities[i];
	}
	return Sims;
}



void Adjacency::ContactStart(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}


	/* Update the Intimacy/Closeness utility */
	updateIntimacy(CurrentTime);


	/* Update the Frequency utility */
	updateFrequency(ID);


	/* Keep track of the active contacts */
	this->activeContacts[ID]++;

	return;
}


void Adjacency::ContactEnd(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}


	/* Update the Intimacy/Closeness utility */
	updateIntimacy(CurrentTime);


	/* Update the Recency utility */
	updateRecency(ID, CurrentTime);


	/* Keep track of the active contacts */
	this->activeContacts[ID]--;

	return;
}


void Adjacency::updateFrequency(int ID)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}

	/* Increase the number of encounters with node ID and with any node */
	this->numEncounters[ID]++;
	this->totalNumEncounters++;

	return;
}


void Adjacency::updateIntimacy(double CurrentTime)
{
	int i;


	/* Update the amount of time that this node has been connected to other nodes */
	for(i = 0; i < this->NSize; i++)
	{
		if(this->activeContacts[i] > 0)
		{
			this->timeConnected[i] += CurrentTime - this->lastUpdate;
			this->totalTimeConnected += CurrentTime - this->lastUpdate;
		}
	}

	this->lastUpdate = CurrentTime;

	return;
}


void Adjacency::updateRecency(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}


	/* Keep track of the time that node ID was last seen */
	this->lastTimeSeen[ID] = CurrentTime;

	return;
}


double Adjacency::getFreq(int ID)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}

	if(this->numEncounters[ID] == 0)
	{
		return 0.0;
	}
	
	if(this->numEncounters[ID] == this->totalNumEncounters)
	{
		return 1.0;
	}
	
	return (((double) this->numEncounters[ID]) / ((double) (this->totalNumEncounters - this->numEncounters[ID])));
}


double Adjacency::getNormalizedFrequency(int ID)
{
	//sanity check
	if(this->numEncounters[ID] == 0)
	{
		return 0.0;
	}
	
	if(this->numEncounters[ID] == this->totalNumEncounters)
	{
		return 1.0;
	}
	
	return (double)((double)this->numEncounters[ID]/((double)this->totalNumEncounters));
}


double Adjacency::getIntimacy(int ID)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}
	
	if(this->timeConnected[ID] == 0.0)
	{
		return 0.0;
	}
	
	if(this->timeConnected[ID] == this->totalTimeConnected)
	{
		return 1.0;
	}
	
	return (this->timeConnected[ID] / (this->totalTimeConnected - this->timeConnected[ID]));
}


double Adjacency::getNormalizedIntimacy(int ID)
{
	//sanity check
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting..\n",ID);
		exit(1);
	}
	
	if(this->timeConnected[ID] == 0.0)
	{
		return 0.0;
	}
	
	if(this->timeConnected[ID] == this->totalTimeConnected)
	{
		return 1.0;
	}
	
	return (this->timeConnected[ID]/(this->totalTimeConnected));
}


double Adjacency::getRecency(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("Node %d that does not exist!\nAborting...\n", ID);
		exit(1);
	}
	
	if(this->lastTimeSeen[ID] < 0)
	{
		return 0.0;
	}
	
	if((this->lastTimeSeen[ID] == CurrentTime) || (activeContacts[ID] > 0))
	{
		return CurrentTime;
	}
	
	return (this->lastTimeSeen[ID] / ((CurrentTime - this->lastTimeSeen[ID]) + 1));
}


double Adjacency::CalculateTieStrength(int ID, double CurrentTime)
{
	return (this->getFreq(ID) + this->getIntimacy(ID) + this->getRecency(ID, CurrentTime));
}


double* Adjacency::CalculateAllTieStrengths(double CurrentTime)
{
	int i;
	double *TS;


	TS = (double *) malloc(this->NSize * sizeof(double));

	for(i = 0; i < this->NSize; i++)
	{
		TS[i] = this->getFreq(i) + this->getIntimacy(i) + this->getRecency(i, CurrentTime);
	}

	return TS;
}


double Adjacency::NormalizedTieStrength(int ID, double CurrentTime)
{
// 	double tmp=this->getNormalizedFrequency(ID);
	double tmp=(1.0/2.0)*this->getNormalizedFrequency(ID) + (1.0/2.0)*this->getNormalizedIntimacy(ID);
// 	double tmp=(1.0/3.0)*this->getFreq(ID) + (1.0/3.0)*this->getIntimacy(ID) + (1.0/3.0)*this->getRecency(ID,CurrentTime);
	//printf("%f\t%f\t%f\t%f\n",this->getFreq(ID),this->getIntimacy(ID),this->getRecency(ID,CurrentTime),tmp);
	if(!(tmp >= 0.0 && tmp <= 1.0))
	{
		printf("Value %f is not between 0 and 1!\n",tmp);
		printf("Frequency is %f\n",this->getNormalizedFrequency(ID));
		printf("Intimacy is %f\n",this->getNormalizedIntimacy(ID));
		printf("Aborting...\n");
		exit(1);
	}
	return tmp;
}


double Adjacency::GetLSFUtility(int ID,double CurrentTime)
{
	if(this->lastTimeSeen[ID] < 0)
	{
			return 0.0;
	}
	double diff=CurrentTime-this->lastTimeSeen[ID];
	double tmp=1.0/(1.0+diff);
	
	if(!(tmp >= 0.0 && tmp <=1.0))
	{
		printf("[%d->%d]\n",this->NodeID,ID);
		printf("Current Time:%f, Last Time Seen:%f\n",CurrentTime,this->lastTimeSeen[ID]);
		printf("Value of LFS utility is %f\n",tmp);
		printf("LFS Utility is not between 0.0 and 1.0\nAborting..\n");
		exit(1);
	}
	
	return tmp;
}


int Adjacency::CalculateSimilarity(int toDestination)
{
	dynamic_bitset<> DestN(Nodes[toDestination]);
	
	#ifdef ADJ_DEBUG
	printf("Destination's(%d) bit set:",toDestination);
	PrintBitSet(DestN);
	printf("My(%d) bit set:",this->NodeID);
	PrintBitSet(Nodes[this->NodeID]);
	#endif
	
	DestN&=Nodes[this->NodeID];
	
	#ifdef ADJ_DEBUG
	printf("Result of AND:");
	PrintBitSet(DestN);
	printf("Similarity value:%d\n",DestN.count());
	#endif
	
	return DestN.count();
}

int Adjacency::CalculateDisSimilarity(int toDestination)
{
	dynamic_bitset<> myTotal(Nodes[this->NodeID]);
	
	for(unsigned int i=0;i<Nodes[this->NodeID].size();i++)
	{
		if((Nodes[this->NodeID].test(i)) && (i != (unsigned int) toDestination))
		{
			//printf("1-hop:%d\n",i);
			dynamic_bitset<> indirect(Nodes[i]);
			myTotal|=indirect;
		}
	}
	myTotal[this->NodeID].flip();
	myTotal[toDestination].flip();
	int myContacts=(int)myTotal.count();
	dynamic_bitset<> DestContacts(Nodes[toDestination]);
	myTotal&=DestContacts;
	int common=(int)myTotal.count();
	int dissimilar=myContacts-common;
	return dissimilar;
}


void Adjacency::UpdateBetweenness(void)
{
	int EgoNeighbors=Nodes[this->NodeID].count()+1;
	//Who are the participating nodes
	//Find 1-hop contacts and copy their ids in a matrix called paricipants
	int runner=0;
	int *participants=(int *)malloc(sizeof(int)*(EgoNeighbors-1));
	for(unsigned int i=0;i<Nodes[this->NodeID].size();i++)
	{
		if(Nodes[this->NodeID].test(i))
		{
			participants[runner]=i;
			runner++;
		}
	}
	
	#ifdef ADJ_DEBUG
	printf("My(%d) bit set:",this->NodeID);
	PrintBitSet(Nodes[this->NodeID]);
	printf("Or Nodes:");
	for(int i=0;i<(EgoNeighbors-1);i++)
	{
		printf(" %d",participants[i]);
	}
	printf("\n");
	#endif
	
	
	//For each pair of contacts calculate the number of paths connecting them 
	double sum=0.0;
	int paths=0;
	for(int i=0;i<(EgoNeighbors-1);i++)
	{
		dynamic_bitset<> Dest(Nodes[participants[i]]);
		for(int j=i+1;j<(EgoNeighbors-1);j++)
		{
			paths=0;
			//Exception: 1 Hop contacts - number of paths is zero
			if(Nodes[participants[i]].test(participants[j]))
			{
				#ifdef ADJ_DEBUG
				printf("Nodes %d and %d are directly connected!\n",participants[i],participants[j]);
				#endif
				continue;
			}
			
			dynamic_bitset<> Source(Nodes[participants[j]]);
			Source&=Dest;
			paths=Source.count();
			#ifdef ADJ_DEBUG
			printf("From %d to %d path number is %d\n",participants[i],participants[j],paths);
			#endif
			//Calculate egocentric betweeneess centrality (eq. 3 - SimBet paper)
			if(paths != 0)
			{
				sum+=(double)(1.0/paths);
			}
		}
	}
	this->Bet=sum;
	//free memory
	free(participants);
	return;
}

void Adjacency::UpdateEfficiency(void )
{
	int EgoNeighbors=Nodes[this->NodeID].count();
	if(!EgoNeighbors)
	{
		this->Effi=0.0;
		return;
	}
	//Who are the participating nodes
	//Find 1-hop contacts and copy their ids in a matrix called paricipants
	int runner=0;
	int *participants=(int *)malloc(sizeof(int)*EgoNeighbors);
	for(unsigned int i=0;i<Nodes[this->NodeID].size();i++)
	{
		if(Nodes[this->NodeID].test(i))
		{
			participants[runner]=i;
			runner++;
		}
	}
	#ifdef ADJ_DEBUG
	printf("I have %d direct contacts\n",EgoNeighbors);
	#endif
	int redundancy=0;
	double total=0.0;
	for(int i=0;i<EgoNeighbors;i++)
	{
		dynamic_bitset<> MyDirect(Nodes[this->NodeID]);
		dynamic_bitset<> InvDirect(Nodes[participants[i]]);
		MyDirect&=InvDirect;
		redundancy=MyDirect.count();
		#ifdef ADJ_DEBUG
		printf("Redundancy with node %d is %d/%d\n",participants[i],redundancy,EgoNeighbors);
		#endif
		total+=((double)redundancy/(double)EgoNeighbors);
	}
	double effSize=(double)EgoNeighbors-total;
	double efficiency=(effSize/EgoNeighbors)*100.0;
	#ifdef ADJ_DEBUG
	printf("Total redundancy is %f\n",total);
	printf("Efficiency size is %f\n",effSize);
	printf("Efficiency is %f\n",efficiency);
	#endif
	free(participants);
	this->Effi=efficiency;
	return;
}

double Adjacency::getCopEffi(int nodeA, int nodeB)
{
	#ifdef ADJ_DEBUG
	printf("Calculating the efficiency metric when nodes %d and %d are infected\n",nodeA,nodeB);
	#endif
	if(!Nodes[nodeA].test(nodeB) && !Nodes[nodeB].test(nodeA))
	{
		//printf("getCopEffi error:The two nodes are not in contact!\n");
		return -1.0;
	}
	//calculate the denominator
	dynamic_bitset<> DirectB(Nodes[nodeB]);
	dynamic_bitset<> U(Nodes[nodeA]);
	U|=DirectB;
	int denominator=U.count()-2;
	
	int pnumber=0;
	int runner=0;
	int *participants=NULL;
	
	//get common direct contacts
	dynamic_bitset<> inquiryCommon(Nodes[nodeA]);
	inquiryCommon&=DirectB;
	pnumber=inquiryCommon.count();
	participants=(int *)malloc(sizeof(int)*pnumber);
	runner=0;
	#ifdef ADJ_DEBUG
	printf("Common direct contacts: ");
	#endif
	for(unsigned int i=0;i<inquiryCommon.size();i++)
	{
		if(inquiryCommon.test(i))
		{
			#ifdef ADJ_DEBUG
			printf("%d\t",i);
			#endif
			participants[runner]=i;
			runner++;
		}
	}
	#ifdef ADJ_DEBUG
	printf("\n");
	#endif
	//calculate the redundancy
	int redundancy=0;
	double total=0.0;
	for(int i=0;i<pnumber;i++)
	{
		dynamic_bitset<> nodeCheck(Nodes[participants[i]]);
		nodeCheck&=U;
		redundancy=nodeCheck.count();
		#ifdef ADJ_DEBUG
		printf("Redundancy with node %d is %d/%d\n",participants[i],redundancy,denominator);
		#endif
		total+=((double)redundancy/(double)denominator);
	}
	//get direct contacts for nodeA excluding the common direct contacts with nodeB
	dynamic_bitset<> inquiryOnlyNodeA(Nodes[nodeA]);
	inquiryOnlyNodeA.flip(nodeB);
	inquiryOnlyNodeA^=inquiryCommon;
	pnumber=inquiryOnlyNodeA.count();
	free(participants);
	participants=(int *)malloc(sizeof(int)*pnumber);
	runner=0;
	#ifdef ADJ_DEBUG
	printf("Only node %d direct contacts: ",nodeA);
	#endif
	for(unsigned int i=0;i<inquiryOnlyNodeA.size();i++)
	{
		if(inquiryOnlyNodeA.test(i))
		{
			#ifdef ADJ_DEBUG
			printf("%d\t",i);
			#endif
			participants[runner]=i;
			runner++;
		}
	}
	#ifdef ADJ_DEBUG
	printf("\n");
	#endif
	redundancy=0.0;
	for(int i=0;i<pnumber;i++)
	{
		dynamic_bitset<> nodeCheck(Nodes[participants[i]]);
		nodeCheck&=U;
		redundancy=nodeCheck.count()-1;
		#ifdef ADJ_DEBUG
		printf("Redundancy with node %d is %d/%d\n",participants[i],redundancy,denominator);
		#endif
		total+=((double)redundancy/(double)denominator);
	}
	//get direct contacts for nodeB excluding the common direct contacts with nodeA
	dynamic_bitset<> inquiryOnlyNodeB(Nodes[nodeB]);
	inquiryOnlyNodeB.flip(nodeA);
	inquiryOnlyNodeB^=inquiryCommon;
	pnumber=inquiryOnlyNodeB.count();
	free(participants);
	participants=(int *)malloc(sizeof(int)*pnumber);
	runner=0;
	#ifdef ADJ_DEBUG
	printf("Only node %d direct contacts: ",nodeB);
	#endif
	for(unsigned int i=0;i<inquiryOnlyNodeB.size();i++)
	{
		if(inquiryOnlyNodeB.test(i))
		{
			#ifdef ADJ_DEBUG
			printf("%d\t",i);
			#endif
			participants[runner]=i;
			runner++;
		}
	}
	#ifdef ADJ_DEBUG
	printf("\n");
	#endif
	redundancy=0.0;
	for(int i=0;i<pnumber;i++)
	{
		dynamic_bitset<> nodeCheck(Nodes[participants[i]]);
		nodeCheck&=U;
		redundancy=nodeCheck.count()-1;
		#ifdef ADJ_DEBUG
		printf("Redundancy with node %d is %d/%d\n",participants[i],redundancy,denominator);
		#endif
		total+=((double)redundancy/(double)denominator);
	}
	double effSize=(double)denominator-total;
	double efficiency=(effSize/denominator)*100.0;
	#ifdef ADJ_DEBUG
	printf("Total redundancy is %f\n",total);
	printf("Efficiency size is %f\n",effSize);
	printf("Efficiency is %f\n",efficiency);
	#endif
	return efficiency;
}



void Adjacency::UpdateSimilarity(int Dest)
{
	if(Dest == this->NodeID)
	{
		return;
	}
	this->Similarities[Dest]=CalculateSimilarity(Dest);
	return;
}

void Adjacency::UpdateDisSimilarity(int Dest)
{
	if(Dest == this->NodeID)
	{
		return;
	}
	this->DisSimilarities[Dest]=CalculateDisSimilarity(Dest);
	return;
}


void Adjacency::PrintAdjacency()
{
	int i;
	int j;


	for(i = 0; i < NSize; i++)
	{
		printf("Node %d:", i);
		for(j = 0; j < NSize; j++)
		{
			if(Nodes[i].test(j))
			{
				printf(" 1");
			}
			else
			{
				printf(" 0");
			}
			
		}
		printf("\n");
	}

	return;
}


void Adjacency::UpdateAll()
{
	int i;


	for(i = 0; i < this->NSize; i++)
	{
		if(i == this->NodeID)
		{
			continue;
		}

		UpdateSimilarity(i);
// 		UpdateDisSimilarity(i);
	}

	UpdateBetweenness();
// 	UpdateEfficiency();

	return;
}


void PrintSet(dynamic_bitset<> *tmp)
{
	if(!tmp->any())
	{
		printf("Empty\n");
		return;
	}
	for(unsigned int i=0;i<tmp->size();i++)
	{
		if(tmp->test(i))
		{
			printf("%d ",i);
		}
	}
	printf("\n");
	return;
}

int* Adjacency::GetForwardingSet(int prev, int Dest)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	InDirectContacts.reset();
	int ConSize=0;
	if(prev == this->NodeID)
	{
		ConSize=DirectContacts.count();
	}
	else
	{
		ConSize=DirectContacts.count()-1;
	}
	int *Con=(int *)malloc(sizeof(int)*ConSize);
	int pos=0;
	for(int i=0;i< (int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && i != prev)
		{
			Con[pos]=i;
			pos++;
			InDirectContacts|=Nodes[i];
		}
	}
	if(pos != ConSize)
	{
		printf("Problem: pos=%d and ConSize=%d\n",pos,ConSize);
		exit(1);
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(unsigned int i=0;i<DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from indirect contacts
	InDirectContacts.set(prev,false);
	//Remove indirect contacts that can be reached through prev hop
	dynamic_bitset<> tmp(Nodes[prev]);
	for(unsigned int i=0;i<tmp.size();i++)
	{
		if(tmp.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
 	#ifdef ADJ_DEBUG
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int position=0;
	while(!InDirectContacts.none())
	{
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Node %d covers: ",Con[i]);
			PrintSet(&tmp);
			#endif
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Node %d Covers:%d\n",Con[i],(int)tmp.count());
			#endif
		}
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//Write node to forwarders
		position++;
		forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
		forwarders[position]=Con[which];
		forwarders[0]=position;
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
		
	}
	free(Con);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}


int* Adjacency::GetForwardingSet(int prev, int* Excluding)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	InDirectContacts.reset();
	int ConSize=0;
	if(prev == this->NodeID)
	{
		ConSize=DirectContacts.count();
	}
	else
	{
		ConSize=DirectContacts.count()-1;
	}
	int *Con=(int *)malloc(sizeof(int)*ConSize);
	int pos=0;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && i != prev)
		{
			Con[pos]=i;
			pos++;
			InDirectContacts|=Nodes[i];
		}		
	}
	//********* The only difference **********
	for(int i=1;i<=Excluding[0];i++)
	{
		InDirectContacts.set(Excluding[i],false);
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(unsigned int i=0;i<DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from indirect contacts
	InDirectContacts.set(prev,false);
	//Remove indirect contacts that can be reached through prev hop
	dynamic_bitset<> tmp(Nodes[prev]);
	for(unsigned int i=0;i<tmp.size();i++)
	{
		if(tmp.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
 	#ifdef ADJ_DEBUG
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int position=0;
	while(!InDirectContacts.none())
	{
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Node %d covers: ",Con[i]);
			PrintSet(&tmp);
			#endif
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Node %d Covers:%d\n",Con[i],(int)tmp.count());
			#endif
		}
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//Write node to forwarders
		position++;
		forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
		forwarders[position]=Con[which];
		forwarders[0]=position;
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
		
	}
	free(Con);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}


int *Adjacency::GetForwardingSet(int *prev,int *Excluding)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	InDirectContacts.reset();
	int ConSize=0;
	bool found=false;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] == this->NodeID)
		{
			found=true;
			break;
		}
	}
	
	if(found)
	{
		ConSize=DirectContacts.count()-prev[0]+1;
	}
	else
	{
		ConSize=DirectContacts.count()-prev[0];
	}
	int *Con=(int *)malloc(sizeof(int)*ConSize);
	int pos=0;
	found=false;
	for(unsigned int i=0;i<DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			found=false;
			for(int j=1;j<=prev[0];j++)
			{
				if(prev[j] == (int) i)
				{
					found=true;
					break;
				}
			}
			if(!found)
			{
				Con[pos]=i;
				pos++;
				InDirectContacts|=Nodes[i];
			}
		}		
	}
	//********* The only difference **********
	for(int i=1;i<=Excluding[0];i++)
	{
		InDirectContacts.set(Excluding[i],false);
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(unsigned int i=0;i<DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from indirect contacts
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	tmp.reset();
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
		//Remove indirect contacts that can be reached through prev hop
		tmp=Nodes[prev[j]];
		for(unsigned int i=0;i<tmp.size();i++)
		{
			if(tmp.test(i))
			{
				InDirectContacts.set(i,false);
			}
		}
	}
 	#ifdef ADJ_DEBUG
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int position=0;
	while(!InDirectContacts.none())
	{
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Node %d covers: ",Con[i]);
			PrintSet(&tmp);
			#endif
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Node %d Covers:%d\n",Con[i],(int)tmp.count());
			#endif
		}
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//Write node to forwarders
		position++;
		forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
		forwarders[position]=Con[which];
		forwarders[0]=position;
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
		
	}
	free(Con);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}

int *Adjacency::GetForwardingSet(int *prev,int *Excluding, int *FWs)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	InDirectContacts.reset();
	int ConSize=0;
	bool found=false;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] == this->NodeID)
		{
			found=true;
			break;
		}
	}
	
	if(found)
	{
		ConSize=DirectContacts.count()-prev[0]+1;
	}
	else
	{
		ConSize=DirectContacts.count()-prev[0];
	}
	int *Con=(int *)malloc(sizeof(int)*ConSize);
	int pos=0;
	found=false;
	for(unsigned int i=0;i<DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			found=false;
			for(int j=1;j<=prev[0];j++)
			{
				if(prev[j] == (int) i)
				{
					found=true;
					break;
				}
			}
			if(!found)
			{
				Con[pos]=i;
				pos++;
				InDirectContacts|=Nodes[i];
			}
		}		
	}
	//********* The only difference **********
	for(int i=1;i<=Excluding[0];i++)
	{
		InDirectContacts.set(Excluding[i],false);
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(unsigned int i=0;i<DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from indirect contacts
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	tmp.reset();
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
		//Remove indirect contacts that can be reached through prev hop
		tmp=Nodes[prev[j]];
		for(unsigned int i=0;i<tmp.size();i++)
		{
			if(tmp.test(i))
			{
				InDirectContacts.set(i,false);
			}
		}
	}
	//Remove Fws from indirect contacts
	tmp.reset();
	for(int j=1;j<=FWs[0];j++)
	{
		InDirectContacts.set(FWs[j],false);
		//Remove indirect contacts that can be reached through prev hop
		tmp=Nodes[FWs[j]];
		for(unsigned int i=0;i<tmp.size();i++)
		{
			if(tmp.test(i))
			{
				InDirectContacts.set(i,false);
			}
		}
	}
 	#ifdef ADJ_DEBUG
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int position=0;
	while(!InDirectContacts.none())
	{
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Node %d covers: ",Con[i]);
			PrintSet(&tmp);
			#endif
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Node %d Covers:%d\n",Con[i],(int)tmp.count());
			#endif
		}
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//Write node to forwarders
		position++;
		forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
		forwarders[position]=Con[which];
		forwarders[0]=position;
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
		
	}
	free(Con);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}

int* Adjacency::GetForwardingSet(int prev, int* Excluding, int *FWs)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	InDirectContacts.reset();
	int ConSize=0;
	if(prev == this->NodeID)
	{
		ConSize=DirectContacts.count();
	}
	else
	{
		ConSize=DirectContacts.count()-1;
	}
	int *Con=(int *)malloc(sizeof(int)*ConSize);
	int pos=0;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && i != prev)
		{
			Con[pos]=i;
			pos++;
			InDirectContacts|=Nodes[i];
		}		
	}
	//********* The only difference **********
	for(int i=1;i<=Excluding[0];i++)
	{
		InDirectContacts.set(Excluding[i],false);
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from indirect contacts
	InDirectContacts.set(prev,false);
	//Remove indirect contacts that can be reached through prev hop
	dynamic_bitset<> tmp(Nodes[prev]);
	for(int i=0;i<(int)tmp.size();i++)
	{
		if(tmp.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove Fws from indirect contacts
	tmp.reset();
	for(int j=1;j<=FWs[0];j++)
	{
		InDirectContacts.set(FWs[j],false);
		//Remove indirect contacts that can be reached through prev hop
		tmp=Nodes[FWs[j]];
		for(int i=0;i<(int)tmp.size();i++)
		{
			if(tmp.test(i))
			{
				InDirectContacts.set(i,false);
			}
		}
	}
 	#ifdef ADJ_DEBUG
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int position=0;
	while(!InDirectContacts.none())
	{
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Node %d covers: ",Con[i]);
			PrintSet(&tmp);
			#endif
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Node %d Covers:%d\n",Con[i],(int)tmp.count());
			#endif
		}
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//Write node to forwarders
		position++;
		forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
		forwarders[position]=Con[which];
		forwarders[0]=position;
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
		
	}
	free(Con);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}


bool IsInside(int who,int *where)
{
	for(int i=1;i<=where[0];i++)
	{
		if(who == where[i])
		{
			return true;
		}
	}
	return false;
}

bool Adjacency::NotOldExists(std::vector<DrawCandidate*> DCan)
{
	for(unsigned int i=0;i<DCan.size();i++)
	{
		if(!DCan[i]->IsOldFW)
		{
			return true;
		}
	}
	return false;
}

bool Adjacency::OldExists(std::vector< DrawCandidate* > DCan)
{
	for(unsigned int i=0;i<DCan.size();i++)
	{
		if(DCan[i]->IsOldFW)
		{
			return true;
		}
	}
	return false;
}


int Adjacency::GetHighestDegree(vector<DrawCandidate*> DCan)
{
	int MaxDegree=-1;
	int pos=-1;
	vector<DrawCandidate*>::iterator it;
	for (it=DCan.begin(); it!=DCan.end(); ++it)
	{
		if((*it)->Degree > MaxDegree)
		{
			MaxDegree=(*it)->Degree;
			pos=(*it)->position;
		}
	}
	return pos;
}

int Adjacency::GetMostRecent(std::vector< DrawCandidate* > DCan,double Ctime)
{
	double MostRecent=-1.0;
	int pos=-1;
	vector<DrawCandidate*>::iterator it;
	for (it=DCan.begin(); it!=DCan.end(); ++it)
	{
		if(this->getRecency((*it)->ID,Ctime) > MostRecent)
		{
			MostRecent=this->getRecency((*it)->ID,Ctime);
			pos=(*it)->position;
		}
	}
	return pos;
}

int Adjacency::GetBetterTieStrength(std::vector< DrawCandidate* > DCan,double Ctime)
{
	double BetterTie=-1.0;
	int pos=-1;
	vector<DrawCandidate*>::iterator it;
	for (it=DCan.begin(); it!=DCan.end(); ++it)
	{
		//printf("%f\n",this->NormalizedTieStrength((*it)->ID,Ctime));
		if(this->NormalizedTieStrength((*it)->ID,Ctime) > BetterTie)
		{
			BetterTie=this->NormalizedTieStrength((*it)->ID,Ctime);
			pos=(*it)->position;
		}
	}
	return pos;
}


int Adjacency::GetMostFrequent(std::vector< DrawCandidate* > DCan)
{
	double MostFrequent=-1.0;
	int pos=-1;
	vector<DrawCandidate*>::iterator it;
	for (it=DCan.begin(); it!=DCan.end(); ++it)
	{
		if(this->getFreq((*it)->ID) > MostFrequent)
		{
			MostFrequent=this->getFreq((*it)->ID);
			pos=(*it)->position;
		}
	}
	return pos;
}



void Adjacency::PrintDraw(vector<DrawCandidate*> DCan)
{
	printf("Draw Candidates are:");
	for(unsigned int i=0;i<DCan.size();i++)
	{
		if(DCan[i]->IsOldFW)
		{
			printf("%d,old\t",DCan[i]->ID);
		}
		else
		{
			printf("%d\t",DCan[i]->ID);
		}
	}
	printf("\n");
	return;
}

bool IsOld(DrawCandidate *Can)
{
	return Can->IsOldFW;
}


bool IsNew(DrawCandidate *Can)
{
	return !(Can->IsOldFW);
}


int *Adjacency::GetForwardingSet(int *oldFWs,int prev)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with prev
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	if(prev != this->NodeID)
	{
		for(int i=0;i<(int) DirectContacts.size();i++)
		{
			if(DirectContacts.test(i) && Nodes[prev].test(i))
			{
				pos++;
				common=(int *)realloc(common,sizeof(int)*(pos+1));
				common[pos]=i;
				common[0]=pos;
			}
		}
	}
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int)DirectContacts.size();i++)
	{
		if(DirectContacts.test(i)) //&& !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int)DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from Indirect contacts
	InDirectContacts.set(prev,false);
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//priority to newcomers
		/*if(NotOldExists(Draw))
		{
			//At least one not-old-FWer exists in the candidate set
			//Remove all old FWers (priority to newcomers)
			vector<DrawCandidate*>::iterator it;
			for ( it = Draw.begin(); it != Draw.end(); )
			{
				if((*it)->IsOldFW)
				{
					delete * it;  
					it = Draw.erase(it);
				}
				else 
				{
					++it;
				}
			}

			//Draw.erase(std::remove_if(Draw.begin(),Draw.end(),IsOld), Draw.end());
		}*/
		
		
		//priority to old forwarders
		if(OldExists(Draw))
		{
			//At least one old-FWer exists in the candidate set
			//Remove all new FWers (priority to old FWers)
			vector<DrawCandidate*>::iterator it;
			for ( it = Draw.begin(); it != Draw.end(); )
			{
				if(!((*it)->IsOldFW))
				{
					delete *it;  
					it = Draw.erase(it);
				}
				else 
				{
					++it;
				}
			}
			//Draw.erase(std::remove_if(Draw.begin(),Draw.end(),IsNew), Draw.end());
		}
		
		
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		which=GetHighestDegree(Draw);
		
		//Draw.clear();
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}

int *Adjacency::GetForwardingSet2(int *oldFWs,int prev)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with prev
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	if(prev != this->NodeID)
	{
		for(int i=0;i<(int)DirectContacts.size();i++)
		{
			if(DirectContacts.test(i) && Nodes[prev].test(i))
			{
				pos++;
				common=(int *)realloc(common,sizeof(int)*(pos+1));
				common[pos]=i;
				common[0]=pos;
			}
		}
	}
	//Remove old forwarders from direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && IsInside(i,oldFWs))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hop from Indirect contacts
	InDirectContacts.set(prev,false);
	//Remove old forwarders from indirect contacts
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts
	/*tmp.reset();
	tmp=InDirectContacts & Nodes[prev];
	InDirectContacts^=tmp;*/
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		which=GetHighestDegree(Draw);
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}




int *Adjacency::GetForwardingSet3(int *oldFWs,int *prev)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with previous nodes (nodes that try to forward the packet to me)
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] != this->NodeID)
		{
			for(int i=0;i<(int) DirectContacts.size();i++)
			{
				if(DirectContacts.test(i) && Nodes[prev[j]].test(i))
				{
					pos++;
					common=(int *)realloc(common,sizeof(int)*(pos+1));
					common[pos]=i;
					common[0]=pos;
				}
			}
		}
	}
	
	//Remove old forwarders from direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && IsInside(i,oldFWs))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	
	//Remove prev hops from Indirect contacts
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	//Remove old forwarders from indirect contacts
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		which=GetHighestDegree(Draw);
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}

int *Adjacency::GetForwardingSetNew(int *oldFWs,int *prev)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	tmp.reset();
	
	//Remove old forwarders + previous pkt holders from direct contacts
	//B(u)=N(u) - (OLDFW + PREV)
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && (IsInside(i,oldFWs) || IsInside(i,prev)))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//The following steps calculate Indirect contacts: U(u)= N(N(u)) - u - OLDFW - PREV -N(u) - N(OLDFW) - N(PREV) 
	//Construct indirect contacts - N(N(u))
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
	}
	//Remove myself from Indirect contacts - u
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts - N(u)
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	
	//Remove prev hops from Indirect contacts - PREV
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	//Remove old forwarders from indirect contacts - OLDFW
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts - N(OLDFW)
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops) - N(PREV)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		which=GetHighestDegree(Draw);
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}


int* Adjacency::GetForwardingSet5(int* oldFWs, int* prev, double Ctime)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with previous nodes (nodes that try to forward the packet to me)
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] != this->NodeID)
		{
			for(int i=0;i<(int) DirectContacts.size();i++)
			{
				if(DirectContacts.test(i) && Nodes[prev[j]].test(i))
				{
					pos++;
					common=(int *)realloc(common,sizeof(int)*(pos+1));
					common[pos]=i;
					common[0]=pos;
				}
			}
		}
	}
	
	//Remove old forwarders from direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && IsInside(i,oldFWs))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	
	//Remove prev hops from Indirect contacts
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	//Remove old forwarders from indirect contacts
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		which=GetBetterTieStrength(Draw,Ctime);
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}

int* Adjacency::GetForwardingSetRecent(int* oldFWs, int* prev, double Ctime)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with previous nodes (nodes that try to forward the packet to me)
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] != this->NodeID)
		{
			for(int i=0;i<(int) DirectContacts.size();i++)
			{
				if(DirectContacts.test(i) && Nodes[prev[j]].test(i))
				{
					pos++;
					common=(int *)realloc(common,sizeof(int)*(pos+1));
					common[pos]=i;
					common[0]=pos;
				}
			}
		}
	}
	
	//Remove old forwarders from direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && IsInside(i,oldFWs))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	
	//Remove prev hops from Indirect contacts
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	//Remove old forwarders from indirect contacts
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	double HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	double *Candidates=(double *)malloc(sizeof(double)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0.0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1.0;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			double price=this->NormalizedTieStrength(Con[i],Ctime)*(double)tmp.count();
			//Candidates[i]=(int)tmp.count();
			Candidates[i]=price;
// 			printf("%f\n",price);
			if(HowMany < price)
			{
				HowMany=price;
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		//which=GetMostRecent(Draw,Ctime);
		//which=GetMostFrequent(Draw);
		which=GetBetterTieStrength(Draw,Ctime);
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}


int* Adjacency::GetForwardingSetPriority(int* oldFWs, int* prev, int NodeinContact)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with previous nodes (nodes that try to forward the packet to me)
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] != this->NodeID)
		{
			for(int i=0;i<(int) DirectContacts.size();i++)
			{
				if(DirectContacts.test(i) && Nodes[prev[j]].test(i))
				{
					pos++;
					common=(int *)realloc(common,sizeof(int)*(pos+1));
					common[pos]=i;
					common[0]=pos;
				}
			}
		}
	}
	
	//Remove old forwarders from direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && IsInside(i,oldFWs))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	
	//Remove prev hops from Indirect contacts
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	//Remove old forwarders from indirect contacts
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		bool flag=false;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				if(Candidates[i] == NodeinContact)
				{
					flag=true;
					which=i;
					break;
				}
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		if(!flag)
		{
			which=GetHighestDegree(Draw);
		}
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}




int *Adjacency::GetForwardingSet4(int *oldFWs,int *prev)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with previous nodes (nodes that try to forward the packet to me)
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] != this->NodeID)
		{
			for(int i=0;i<(int) DirectContacts.size();i++)
			{
				if(DirectContacts.test(i) && Nodes[prev[j]].test(i))
				{
					pos++;
					common=(int *)realloc(common,sizeof(int)*(pos+1));
					common[pos]=i;
					common[0]=pos;
				}
			}
		}
	}
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Remove prev hops from Indirect contacts
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	int HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	int *Candidates=(int *)malloc(sizeof(int)*ConSize);
	int position=0;
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Keep track of draws
			Candidates[i]=(int)tmp.count();
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//priority to newcomers
		/*if(NotOldExists(Draw))
		{
			//At least one not-old-FWer exists in the candidate set
			//Remove all old FWers (priority to newcomers)
			vector<DrawCandidate*>::iterator it;
			for ( it = Draw.begin(); it != Draw.end(); )
			{
				if((*it)->IsOldFW)
				{
					delete * it;  
					it = Draw.erase(it);
				}
				else 
				{
					++it;
				}
			}

			//Draw.erase(std::remove_if(Draw.begin(),Draw.end(),IsOld), Draw.end());
		}*/
		
		
		//priority to old forwarders
		if(OldExists(Draw))
		{
			//At least one old-FWer exists in the candidate set
			//Remove all new FWers (priority to old FWers)
			vector<DrawCandidate*>::iterator it;
			for ( it = Draw.begin(); it != Draw.end(); )
			{
				if(!((*it)->IsOldFW))
				{
					delete *it;  
					it = Draw.erase(it);
				}
				else 
				{
					++it;
				}
			}
			//Draw.erase(std::remove_if(Draw.begin(),Draw.end(),IsNew), Draw.end());
		}
		
		
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		which=GetHighestDegree(Draw);
		
		//Draw.clear();
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}

int *Adjacency::GetBestFriends(int *Candidates)
{
	int *friends=(int *)malloc(sizeof(int));
	friends[0]=0;
	if(Candidates[0] == 0)
	{
		return friends;
	}
	//Take 2-Hop contacts
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	for(int i=1;i<=Candidates[0];i++)
	{
		tmp.reset();
		tmp=Nodes[Candidates[i]];
		InDirectContacts|=tmp;
	}
	
	InDirectContacts.set(this->NodeID,false);
	for(int i=1;i<=Candidates[0];i++)
	{
		InDirectContacts.set(Candidates[i],false);
	}
	#ifdef ADJ_DEBUG
	printf("Total 2 hop contacts:%d\n",(int)InDirectContacts.count());
	#endif
	int HowMany=0;
	int which=-1;
	int position=0;
	while(!InDirectContacts.none())
	{
		HowMany=0;
		for(int i=1;i<=Candidates[0];i++)
		{
			if(Candidates[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Candidates[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Node %d covers %d nodes\n",Candidates[i],(int)tmp.count());
			#endif
			if(HowMany < (int)tmp.count())
			{
				HowMany=(int)tmp.count();
				which=i;
			}
			
		}
		tmp.reset();
		tmp=InDirectContacts & Nodes[Candidates[which]];
		InDirectContacts^=tmp;
		position++;
		friends=(int *)realloc(friends,sizeof(int)*(position+1));
		friends[position]=Candidates[which];
		friends[0]=position;
		#ifdef ADJ_DEBUG
		printf("Node %d with %d covering wins!\n",Candidates[which],HowMany);
		#endif
		Candidates[which]=-1;
	}
	return friends;
}



int *Adjacency::GetIndirectContacts(int throughID)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[throughID]);
	
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Create matrix to return
	int num=InDirectContacts.count();
	int *res=(int *)malloc(sizeof(int)*(num+1));
	res[0]=num;
	int pos=1;
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			res[pos]=i;
			pos++;
		}
	}
	return res;
}
int *Adjacency::GetIndirectContactsOnlyThroughID(int throughID)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[throughID]);
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Create matrix to return

	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if (i==throughID)
		{continue;}
		if(DirectContacts.test(i))
		{	dynamic_bitset<> temp(InDirectContacts);
			dynamic_bitset<> InDirectContactstemp(Nodes[i]);
			//Remove myself from Indirect contacts
			InDirectContactstemp.set(this->NodeID,false);
			//Also, remove direct contacts
// 			for(int i=0;i<(int) DirectContacts.size();i++)
// 			{
// 				if(DirectContacts.test(i))
// 				{
// 					InDirectContactstemp.set(i,false);
// 				}
// 			}
			InDirectContacts^=InDirectContactstemp;
			InDirectContacts&=temp;
		}
	}
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	int num=InDirectContacts.count();
	int *res=(int *)malloc(sizeof(int)*(num+1));
	res[0]=num;
	int pos=1;
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			res[pos]=i;
			pos++;
		}
	}
	return res;
}

int Adjacency::HowManyCovered(int throughID)
{
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[throughID]);
	
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	//Create matrix to return
	return (int)InDirectContacts.count();
}


bool Adjacency::IsDirectContact(int ID)
{
	if(this->Nodes[this->NodeID].test(ID))
	{
		return true;
	}
	return false;
}


bool Adjacency::AreNodeContactsCovered(int ID,int *NL)
{
	dynamic_bitset<> tmp(Nodes[ID]);
	for(int i=1;i<=NL[0];i++)
	{
		tmp.set(NL[i],false);
		tmp&=Nodes[NL[i]];
	}
	return (!(tmp.any()));
}

bool Adjacency::Is2HopCovered(int ID, int* FWers, int* prevs)
{
	dynamic_bitset<> Contacts2Hop(Nodes[ID]);
	dynamic_bitset<> tmp(Nodes[ID]);
	
	for(int i=1;i<=FWers[0];i++)
	{
		Contacts2Hop.set(FWers[i],false);
		tmp.reset();
		tmp=Contacts2Hop & Nodes[FWers[i]];
		Contacts2Hop^=tmp;
	}
	
	for(int i=1;i<=prevs[0];i++)
	{
		Contacts2Hop.set(prevs[i],false);
		tmp.reset();
		tmp=Contacts2Hop & Nodes[prevs[i]];
		Contacts2Hop^=tmp;
	}
	
	return (!(Contacts2Hop.any()));
}


bool AdjacencyWeights::Is2HopCoveredv2(int ID, int* FWers, int* prevs)
{
	dynamic_bitset<> Contacts2Hop(Nodes[ID]);
	dynamic_bitset<> tmp(Nodes[ID]);
	int num=(int)Contacts2Hop.count();
	int *IDs2Hop=(int *)malloc(sizeof(int)*num);
	
	int pos=0;
	//Get IDs of 2-hop contacts
	for(int i=0;i<(int) Contacts2Hop.size();i++)
	{
		if(Contacts2Hop.test(i))
		{
			IDs2Hop[pos]=i;
			pos++;
		}
	}
	
	double maxUtil=0.0;
	for(int i=0;i<num;i++)
	{
		maxUtil=0.0;
		//Check if a previous forwarder reaches node IDs2Hop[i]
		//If there are some nodes, get the higher util value
		for(int j=1;j<=FWers[0];j++)
		{
			tmp.reset();
			tmp=Nodes[FWers[j]];
			if(tmp.test(IDs2Hop[i]))
			{
				if(maxUtil < this->GetFreqWeight(FWers[j],IDs2Hop[i]))
				{
					maxUtil=this->GetFreqWeight(FWers[j],IDs2Hop[i]);
				}
			}
		}
		
		
		//Check if a previous hop reaches node IDs2Hop[i]
		//If there are some nodes, get the higher util value
		for(int j=1;j<=prevs[0];j++)
		{
			tmp.reset();
			tmp=Nodes[prevs[j]];
			if(tmp.test(IDs2Hop[i]))
			{
				if(maxUtil < this->GetFreqWeight(prevs[j],IDs2Hop[i]))
				{
					maxUtil=this->GetFreqWeight(prevs[j],IDs2Hop[i]);
				}
			}
		}
		
		//Finally, check if the new util (offered by the new contact) is
		//better than older util - If that is the case, return that 2 hop
		// neighborhood is not covered properly
		if(maxUtil < this->GetFreqWeight(ID,IDs2Hop[i]))
		{
			return false;
		}
		
	}
	return true;
}




/* IsIndirectContact
 * -----------------
 * Returns the list of direct contacts that lead to indirect
 * contact with id "ID"
 */
int* Adjacency::IsIndirectContact(int ID)
{
	int *FS=(int *)malloc(sizeof(int));
	FS[0]=0;
	int pos=0;
	for(int i=0;i<(int) Nodes[this->NodeID].size();i++)
	{
		if(Nodes[this->NodeID].test(i))
		{//This is a direct contact
			if(Nodes[i].test(ID))
			{//Node with ID is indirect contact through Node i
				pos++;
				FS=(int *)realloc(FS,sizeof(int)*(pos+1));
				FS[pos]=i;
				FS[0]=pos;
			}
		}
	}
	return FS;
}


int Adjacency::GetNumDirect(void )
{
	int res=0;
	for(int i=0;i<(int) Nodes[this->NodeID].size();i++)
	{
		if(Nodes[this->NodeID].test(i))
		{
			res++;
		}
	}
	return res;
}


void Adjacency::PrintSimilarities(void)
{
	printf("Similarities of node %d:",this->NodeID);
	for(int i=0;i<NSize;i++)
	{
		if(i == this->NodeID)
		{
			continue;
		}
		printf(" to node %d:%d,",i,this->Similarities[i]);
	}
	printf("\n");
}


int Adjacency::getEgoNetworkSize(void)
{
	dynamic_bitset<> MyN(Nodes[this->NodeID]);

	return MyN.count();
}


int Adjacency::getSimilarityRank(int destID)
{
	int i;
	int rank;

	rank = 0;
	for(i = 0; i < this->NSize; i++)
	{
		if(this->Similarities[i] > this->Similarities[destID])
		{
			rank++;
		}
	}
	rank++;

	return rank;
}


AdjacencyWindow::AdjacencyWindow(int ID, int NodeSize, double Window): Adjacency(ID, NodeSize)
{
	if(Window > 0)
	{
		this->WindowSize=Window;
		//Allocate memory
		this->ContactTime=(double**)malloc(sizeof(double*)*(this->NSize));
		for(int i=0;i<this->NSize;i++)
		{
			this->ContactTime[i]=(double*)malloc(sizeof(double)*(this->NSize));
		}
		//Initialize
		for(int i=0;i<this->NSize;i++)
		{
			for(int j=0;j<this->NSize;j++)
			{
				this->ContactTime[i][j]=0.0;
			}
		}
	}
	else if(Window == 0)
	{
		printf("Window size could not be zero!Please use class Adjacency for an implementation of Adjacency without window!Aborting\n");
		exit(1);
	}
	else
	{
		printf("Window Size must be positive!Aborting..\n");
		exit(1);
	}
}

AdjacencyWindow::~AdjacencyWindow()
{
	free(ContactTime);
		
	return;
}



void AdjacencyWindow::SetConnection(int Node1, int Node2, double CurrentTime)
{
	Nodes[Node1].set(Node2);
	Nodes[Node2].set(Node1);
	
	//Update window
	if(this->WindowSize != 0)
	{
		this->ContactTime[Node1][Node2]=CurrentTime;
		this->ContactTime[Node2][Node1]=CurrentTime;
	}
	return;
}


void AdjacencyWindow::UpdateAll(double CurrentTime)
{
	this->UpdateWindow(CurrentTime);
	Adjacency::UpdateAll();
	return;
}

void AdjacencyWindow::UpdateWindow(double CTime)
{
	for(int i=0;i<this->NSize;i++)
	{//For every 1-hop contact
		for(int j=0;j<this->NSize;j++)
		{//For every 2-hop contact
			if(Nodes[i].test(j))
			{//Connection exists
				if((CTime-(this->ContactTime[i][j])) > this->WindowSize)
				{//Time is expired
					this->ContactTime[i][j]=0.0;
					Nodes[i].reset(j);
				}
			}
		}
	}
	return;
}


void AdjacencyWeights::UpdateFreqWeight(int Neighbor,int Contact, double value)
{
	//sanity check
// 	if(!(value >= 0.0 && value <= 1.0))
// 	{
// 		printf("Metric must be between 0.0 and 1.0\nAborting..\n");
// 		exit(1);
// 	}
	this->FreqWeight[Neighbor][Contact]=value;
	return;
}


double AdjacencyWeights::GetFreqWeight(int Neighbor, int Contact)
{
	return (this->FreqWeight[Neighbor][Contact]);
}

void AdjacencyWeights::UpdateIntWeight(int Neighbor,int Contact, double value)
{
	//sanity check
	if(!(value >= 0.0 && value <= 1.0))
	{
		printf("Metric must be between 0.0 and 1.0\nAborting..\n");
		exit(1);
	}
	this->IntimacyWeight[Neighbor][Contact]=value;
	return;
}


double AdjacencyWeights::GetIntWeight(int Neighbor, int Contact)
{
	return (this->IntimacyWeight[Neighbor][Contact]);
}

double AdjacencyWeights::GetWeight(int Neighbor, int Contact)
{
	double tmp=((1.0/2.0)*this->GetIntWeight(Neighbor,Contact))+((1.0/2.0)*this->GetFreqWeight(Neighbor,Contact));
	if(!(tmp>=0.0 && tmp <= 1.0))
	{
		printf("Stored metric is not between 0.0 and 1.0!\nAborting..\n");
		exit(1);
	}
	return(tmp);
}


void AdjacencyWeights::PrintWeights(void )
{
	printf("Weights stored in node %d\n",this->NodeID);
	printf("==========================\n");

	dynamic_bitset<> MyN(Nodes[this->NodeID]);
	dynamic_bitset<> MyContactsN(Nodes[this->NodeID]);
	for(int i=0;i<(int) MyN.size();i++)
	{
		if(MyN.test(i))
		{
			printf("From 1-hop contact %d to: ",i);
			MyContactsN.reset();
			MyContactsN=Nodes[i];
			for(int j=0;j<(int) MyContactsN.size();j++)
			{
				if(MyContactsN.test(j))
				{
					printf("%d f:%f i:%f ",j,this->GetFreqWeight(i,j),this->GetIntWeight(i,j));
				}
			}
			printf("\n");
		}
	}
}

int* AdjacencyWeights::GetForwardingSetRecent(int* oldFWs, int* prev, double Ctime)
{
	vector<DrawCandidate*> Draw;
	//Take 2-Hop contacts
	dynamic_bitset<> DirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> InDirectContacts(Nodes[this->NodeID]);
	dynamic_bitset<> tmp(Nodes[this->NodeID]);
	InDirectContacts.reset();
	tmp.reset();
	
	//Get common neighbors with previous nodes (nodes that try to forward the packet to me)
	int *common=(int *)malloc(sizeof(int));
	common[0]=0;
	int pos=0;
	for(int j=1;j<=prev[0];j++)
	{
		if(prev[j] != this->NodeID)
		{
			for(int i=0;i<(int) DirectContacts.size();i++)
			{
				if(DirectContacts.test(i) && Nodes[prev[j]].test(i))
				{
					pos++;
					common=(int *)realloc(common,sizeof(int)*(pos+1));
					common[pos]=i;
					common[0]=pos;
				}
			}
		}
	}
	
	//Remove old forwarders from direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && IsInside(i,oldFWs))
		{
			DirectContacts.set(i,false);
		}
	}
	
	//Construct indirect contacts
	int ConSize=0;
	int *Con=NULL;
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i) && !IsInside(i,common))
		{
			ConSize++;
			Con=(int *)realloc(Con,sizeof(int)*(ConSize));
			Con[ConSize-1]=i;
			InDirectContacts|=Nodes[i];
		}
		else
		{
			DirectContacts.set(i,false);
		}
	}
	//Remove myself from Indirect contacts
	InDirectContacts.set(this->NodeID,false);
	//Also, remove direct contacts
	for(int i=0;i<(int) DirectContacts.size();i++)
	{
		if(DirectContacts.test(i))
		{
			InDirectContacts.set(i,false);
		}
	}
	
	//Remove prev hops from Indirect contacts
	for(int j=1;j<=prev[0];j++)
	{
		InDirectContacts.set(prev[j],false);
	}
	//Remove old forwarders from indirect contacts
	for(int i=0;i<(int) InDirectContacts.size();i++)
	{
		if(InDirectContacts.test(i))
		{
			for(int j=1;j<=oldFWs[0];j++)
			{
				if(oldFWs[j] == i)
				{
					InDirectContacts.set(i,false);
					break;
				}
			}
		}
		
	}
	//Remove the neighbors of old forwarders from indirect contacts
	for(int j=1;j<=oldFWs[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[oldFWs[j]];
		InDirectContacts^=tmp;
	}
	
	//Remove prev neighbors from indirect contacts (for all previous hops)
	for(int j=1;j<=prev[0];j++)
	{
		tmp.reset();
		tmp=InDirectContacts & Nodes[prev[j]];
		InDirectContacts^=tmp;
	}	
	
	
 	#ifdef ADJ_DEBUG
	printf("Common Contacts:");
	for(int i=1;i<=common[0];i++)
	{
		printf("%d\t",common[i]);
	}
	printf("\n");
	printf("Direct Contacts:");
	PrintSet(&DirectContacts);
	printf("Indirect Contacts:");
	PrintSet(&InDirectContacts);
	#endif
	int which=-1;
	double HowMany=-1;
	int *forwarders=(int *)malloc(sizeof(int));
	forwarders[0]=0;
	double *Candidates=(double *)malloc(sizeof(double)*ConSize);
	int position=0;
	double price=0.0;
	double weightSum=0.0;
	dynamic_bitset<> TwoHopContacts(Nodes[this->NodeID]);
	//printf("Node %d running GSC!\n",this->NodeID);
	//printf("-------------------------\n");
	while(!InDirectContacts.none())
	{
		//reset candidates
		//printf("Candidates:");
		for(int i=0;i<ConSize;i++)
 		{
			//printf("%d\t",Con[i]);
			Candidates[i]=0.0;
		}
		//printf("\n");
		which=-1;
		HowMany=-1.0;
		for(int i=0;i<ConSize;i++)
 		{
			if(Con[i] == -1)
			{
				continue;
			}
			tmp.reset();
			tmp=Nodes[Con[i]] & InDirectContacts;
			#ifdef ADJ_DEBUG
			printf("Nodes that Node %d covers are: ",Con[i]);
			PrintSet(&tmp);
			#endif
			//Calculate Sum of weights between 1-hop contact and 2-hop contacts 
			//(available through this 1-hop contact)
			weightSum=0.0;
			TwoHopContacts.reset();
			TwoHopContacts=Nodes[Con[i]];
			for(int k=0;k<(int) TwoHopContacts.size();k++)
			{
				if(TwoHopContacts.test(k))
				{
					weightSum+=this->GetWeight(Con[i],k);
				}
			}
			price=this->NormalizedTieStrength(Con[i],Ctime)*weightSum;
			//price=this->GetLSFUtility(Con[i],Ctime)*weightSum;
			
			Candidates[i]=price;
// 			printf("%f\n",price);
			if(HowMany < price)
			{
				HowMany=price;
				which=i;
			}
			#ifdef ADJ_DEBUG
			printf("Number of Nodes  that Node %d covers is %d\n",Con[i],(int)tmp.count());
			#endif
		}
		//Case of draw
		#ifdef ADJ_DEBUG
		printf("Checking draws..\n");
		#endif
		
		//Create Draw Candidates
		DrawCandidate *temp=NULL;
		for(int i=0;i<ConSize;i++)
		{
			if(Candidates[i] == HowMany)
			{
				tmp.reset();
				tmp=Nodes[Con[i]];
				temp=new DrawCandidate(Con[i],Candidates[i],IsInside(Con[i],oldFWs),(int)tmp.count(),i);
				Draw.push_back(temp);
			}
		}
		//printf("Covered %d nodes\n",HowMany);
		//PrintDraw(Draw);
		
		vector<DrawCandidate*>::iterator it;
		//printf("Getting priority!!\n");
		//Give priority to the one that has the best degree
		//which=GetMostRecent(Draw,Ctime);
		//which=GetMostFrequent(Draw);
		which=GetBetterTieStrength(Draw,Ctime);
		
		for ( it = Draw.begin(); it != Draw.end(); )
		{
			delete *it;  
			it = Draw.erase(it);
			
		}
		Draw.clear();
		//printf("\n");
		#ifdef ADJ_DEBUG
		printf("Node %d that covers %d wins!\n",Con[which],HowMany);
		#endif
		tmp.reset();
		tmp=InDirectContacts & Nodes[Con[which]];
		InDirectContacts^=tmp;
		//if(!IsInside(Con[which],oldFWs))
		//{
			//Add node to forwarders
			position++;
			forwarders=(int *)realloc(forwarders,sizeof(int)*(position+1));
			forwarders[position]=Con[which];
			forwarders[0]=position;
		//}
		//printf("Node %d wins!\n",Con[which]);
		Con[which]=-1;
		#ifdef ADJ_DEBUG
		printf("Indirect contacts now become: ");
		PrintSet(&InDirectContacts);
		#endif
	}
	free(Con);
	free(common);
	#ifdef ADJ_DEBUG
	printf("The forwarders are %d:",forwarders[0]);
	for(int i=1;i<=forwarders[0];i++)
	{
		printf(" %d",forwarders[i]);
	}
	printf("\n");
	#endif
	return forwarders;
}


ContactAggregation::ContactAggregation(vector< dynamic_bitset<> > *p2Nodes)
{
	this->CG=p2Nodes;
}

ContactAggregation::~ContactAggregation()
{
	CG->clear();
	return;
}


GrowingTimeWindow::GrowingTimeWindow(vector< dynamic_bitset<> > *p2Nodes): ContactAggregation(p2Nodes)
{
	//printf("Growing Time Window object created successfully!\n");
	return;
}

GrowingTimeWindow::~GrowingTimeWindow()
{
	return;
}


void GrowingTimeWindow::Connect(int Node1, int Node2, double ConTime)
{
	(*CG)[Node1].set(Node2);
	(*CG)[Node2].set(Node1);

	return;
}


void GrowingTimeWindow::Update()
{
	return;
}


SlidingWindow::SlidingWindow(vector< dynamic_bitset<> > *p2Nodes,int NodeSize,double window): ContactAggregation(p2Nodes)
{
	this->WindowSize=window;
	this->NN=NodeSize;
	this->Oldest=-1.0;
	//Allocate memory
	this->ContactTime=(double**)malloc(sizeof(double*)*(this->NN));
	for(int i=0;i<this->NN;i++)
	{
		this->ContactTime[i]=(double*)malloc(sizeof(double)*(this->NN));
	}
	//Initialize
	for(int i=0;i<this->NN;i++)
	{
		for(int j=0;j<this->NN;j++)
		{
			this->ContactTime[i][j]=-1.0;
		}
	}
	return;
}

SlidingWindow::~SlidingWindow()
{
	for(int i=0;i<this->NN;i++)
	{
		free(ContactTime[i]);
	}
	free(ContactTime);
	return;
}


void SlidingWindow::Connect(int Node1,int Node2,double ConTime)
{
	this->ContactTime[Node1][Node2]=ConTime;
	this->ContactTime[Node2][Node1]=ConTime;
	
	//Case all edges are inside the window - update is not needed 
	if((this->Oldest > 0.0) && (ConTime - this->Oldest) <= this->WindowSize)
	{
		return;
	}
	this->ReCreateContactGraph(ConTime);
}

void SlidingWindow::ReCreateContactGraph(double ConTime)
{
	//Reset Contact Graph
	for(int i=0;i<this->NN;i++)
	{
		(*CG)[i].reset();
	}
	
	double min=-1.0;
	for(int i=0;i<this->NN;i++)
	{
		for(int j=0;j<i;j++)
		{
			//skip edges that are not set
			if(this->ContactTime[i][j] < -1.0)
			{
				continue;
			}
			//Remove the old edges
			if((ConTime-this->ContactTime[i][j]) > this->WindowSize)
			{	
				this->ContactTime[i][j]=-1.0;
				this->ContactTime[j][i]=-1.0;
			}
			else
			{
				//Find oldest contact time for the remaining edges
				if(min > this->ContactTime[i][j] || min < 0.0)
				{
					min=this->ContactTime[i][j];
				}
				//Update the contact graph
				(*CG)[i].set(j,true);
				(*CG)[j].set(i,true);
			}
		}
	}
	//Finally set the value of the oldest edge
	this->Oldest=min;
	return;
}

void SlidingWindow::Update(void)
{
	return;
}

MFaggregation::MFaggregation(int NodeSize,vector< dynamic_bitset<> > *p2Nodes,double Density):ContactAggregation(p2Nodes)
{
	this->NN=NodeSize;
	this->FreqLimit=0.0;
	this->CurrentDensity=0.0;
	this->TargetDensity=Density;
	this->ActiveEdges=0;
	this->NumEdgesLimit=0;
	this->AllEdges=(NodeSize*(NodeSize-1))/2;
	this->MostFrequent=(double **)malloc(sizeof(double *)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->MostFrequent[i]=(double *)malloc(sizeof(double)*NodeSize);
		for(int j=0;j<NodeSize;j++)
		{
			this->MostFrequent[i][j]=0.0;
		}
	}
	return;
}

MFaggregation::~MFaggregation()
{
	for(int i=0;i<NN;i++)
	{
		free(MostFrequent[i]);
	}
	free(MostFrequent);
	return;
}


void MFaggregation::Connect(int Node1,int Node2,double ConTime)
{
	//Get the old frequency value of the pair of nodes that is going to be updated
	double OldFreq=this->MostFrequent[Node1][Node2];
	//Set Frequency - Number of meetings (total number of contacts)
	this->MostFrequent[Node1][Node2]++;
	this->MostFrequent[Node2][Node1]++;
	
	/*case: edge between node1 and node 2 is already active
	 * and hasn't got weight value equal to the weight limit
	 */
	if(OldFreq > this->FreqLimit && this->FreqLimit != 0.0)
	{
		return;
	}
	
	/*case: edge between node1 and node 2 is not active,
	 * hasn't got weight value greater than the weight limit and
	 * the density cannot be increased
	 */
	double NewDensity=(double)(this->ActiveEdges+1)/(double)this->AllEdges;
	if(this->MostFrequent[Node1][Node2] < this->FreqLimit && NewDensity > this->TargetDensity)
	{
		return;
	}
	
	this->ReCreateContactGraph();
	
	#ifdef ADJ_DEBUG
	printf("Active Edges:%d\n",this->ActiveEdges);
	printf("Current Density:%f\tTarget Density:%f\n",this->CurrentDensity,this->TargetDensity);
	#endif
	return;
}

bool Sortingfunction (double i,double j) { return (i>j); }

void MFaggregation::ReCreateContactGraph(void)
{
	//Reset Contact Graph
	for(int i=0;i<this->NN;i++)
	{
		(*CG)[i].reset();
	}
	
	int EdgestoChoose=(int)(this->TargetDensity*this->AllEdges);
	
	//Create a vector with all weights and sort it
	vector<double> Weights;
	for(int i=0;i<this->NN;i++)
	{
		for(int j=0;j<i;j++)
		{
			Weights.push_back(this->MostFrequent[i][j]);
		}
	}
	sort(Weights.begin(),Weights.end(),Sortingfunction);
	//Take the least weight value (according to the given density)
	double LeastW=Weights.at((EdgestoChoose-1));
	//Update the FreqLimit attribute that "remembers" the last frequency limit
	this->FreqLimit=LeastW;
	double MaxW=Weights.at(0);
	#ifdef ADJ_DEBUG
	printf("Leastw:%f\tMaxW:%f\n",LeastW,MaxW);
	#endif
	if(!(MaxW != 0.0))
	{
		return;
	}
	
	//Create the contact graph (containing 0s and 1s)
	this->ActiveEdges=0;
	this->NumEdgesLimit=0;
	for(int i=0;i<this->NN;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(this->MostFrequent[i][j] >= LeastW && this->MostFrequent[i][j] != 0.0)
			{
				if(this->MostFrequent[i][j] == LeastW)
				{
					this->NumEdgesLimit++;
				}
				this->ActiveEdges++;
				(*CG)[i].set(j,true);
				(*CG)[j].set(i,true);
			}
		}
	}
// 	this->ActiveEdges=this->ActiveEdges/2;
// 	this->NumEdgesLimit=this->NumEdgesLimit/2;
	this->CurrentDensity=(double)((double)this->ActiveEdges/(double)this->AllEdges);
	return;
}

void MFaggregation::Update(void)
{
	return;
}


MRaggregation::MRaggregation(int NodeSize,vector< dynamic_bitset<> > *p2Nodes,double Density):ContactAggregation(p2Nodes)
{
	this->NN=NodeSize;
	this->RecencyLimit=0.0;
	this->CurrentDensity=0.0;
	this->TargetDensity=Density;
	this->ActiveEdges=0;
	this->NumEdgesLimit=0;
	this->AllEdges=(NodeSize*(NodeSize-1))/2;
	this->LastSeen=(double **)malloc(sizeof(double *)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->LastSeen[i]=(double *)malloc(sizeof(double)*NodeSize);
		for(int j=0;j<NodeSize;j++)
		{
			this->LastSeen[i][j]=0.0;
		}
	}
	return;
}

MRaggregation::~MRaggregation()
{
	for(int i=0;i<NN;i++)
	{
		free(LastSeen[i]);
	}
	free(LastSeen);
	return;
}


void MRaggregation::Connect(int Node1,int Node2,double ConTime)
{
	//Set Recency - Last contact time between Node1 and Node2
	this->LastSeen[Node1][Node2]=ConTime;
	this->LastSeen[Node2][Node1]=ConTime;
	
// 	double NewDensity=(double)(this->ActiveEdges+1)/(double)this->AllEdges;
// 	if(this->MostFrequent[Node1][Node2] < this->FreqLimit && NewDensity > this->TargetDensity)
// 	{
// 		return;
// 	}
	
	this->ReCreateContactGraph(ConTime);
	
	#ifdef ADJ_DEBUG
	printf("Active Edges:%d\n",this->ActiveEdges);
	printf("Current Density:%f\tTarget Density:%f\n",this->CurrentDensity,this->TargetDensity);
	#endif
	return;
}


void MRaggregation::ReCreateContactGraph(double ConTime)
{
	//Reset Contact Graph
	for(int i=0;i<this->NN;i++)
	{
		(*CG)[i].reset();
	}
	
	int EdgestoChoose=(int)(this->TargetDensity*this->AllEdges);
	
	//Create a vector with all weights and sort it
	vector<double> Weights;
	double diff=0;
	for(int i=0;i<this->NN;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(this->LastSeen[i][j] != 0.0)
			{
				diff=ConTime-this->LastSeen[i][j];
				diff=(1.0/(1.0+diff));
			}
			else
			{
				diff=0.0;
			}
			Weights.push_back(diff);
		}
	}
	sort(Weights.begin(),Weights.end(),Sortingfunction);
	//Take the least weight value (according to the given density)
	double LeastW=Weights.at((EdgestoChoose-1));
	//Update the RecencyLimit attribute that "remembers" the last Recency limit
	this->RecencyLimit=LeastW;
	double MaxW=Weights.at(0);
	#ifdef ADJ_DEBUG
	printf("Leastw:%f\tMaxW:%f\n",LeastW,MaxW);
	#endif
	if(!(MaxW != 0.0))
	{
		return;
	}
	
	//Create the contact graph (containing 0s and 1s)
	this->ActiveEdges=0;
	this->NumEdgesLimit=0;
	diff=0.0;
	for(int i=0;i<this->NN;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(this->LastSeen[i][j] != 0.0)
			{
				diff=ConTime-this->LastSeen[i][j];
				diff=(1.0/(1.0+diff));
			}
			else
			{
				diff=0.0;
			}
			if(diff >= LeastW && this->LastSeen[i][j] != 0.0)
			{
				if(diff == LeastW)
				{
					this->NumEdgesLimit++;
				}
				this->ActiveEdges++;
				(*CG)[i].set(j,true);
				(*CG)[j].set(i,true);
			}
		}
	}
	this->CurrentDensity=(double)((double)this->ActiveEdges/(double)this->AllEdges);
	return;
}

void MRaggregation::Update(void)
{
	return;
}
