/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
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


#ifndef TRAFFICGEN_H
	#define TRAFFICGEN_H
	#include "TrafficGen.h"
#endif

using namespace std;


TrafficGen::TrafficGen(int Seed, int Total, double duration, string presFile, EventList *SimL)
{
	srand(Seed);

	this->TotalNodes = Total;
	this->traceDuration = duration;
	this->SndPrdStart = 0.2 * this->traceDuration;
	this->SndPrdStop = 0.8 * this->traceDuration;
	this->presenceFilename = presFile;
	FillPresenceList(this->presenceFilename);

	this->Sim = SimL;

	return;
}


TrafficGen::~TrafficGen(void )
{
	this->Inactive.clear();

	free(inPresence);
	free(outPresence);

	return;
}


int *TrafficGen::generateTraffic(int trafficType, int NumPackets)
{
	if(trafficType == UNIFORM_TT)
	{
		return this->CreateUniformTraffic(NumPackets);
	}
	else if(trafficType == BURST_TT)
	{
		return this->CreateBurstTraffic(NumPackets);
	}
	else if(trafficType == SAMPLE_TT)
	{
		return this->CreateSampleTraffic(NumPackets);
	}
	else if(trafficType == PREDEFINED_TT)
	{
		return this->CreatePredefinedTraffic();
	}
	else
	{
		printf("\nError! Unknown type of traffic: %d\n\n", trafficType);
		exit(EXIT_FAILURE);
	}
}


/* SetTransPeriod
 * --------------
 * Sets the period in which traffic will be generated.
 */
void TrafficGen::SetTransPeriod(double SendPeriodStrt,double SendPeriodStop)
{
	this->SndPrdStart=SendPeriodStrt;
	this->SndPrdStop=SendPeriodStop;
	return;
}


/* FillPresenceList
 * ----------------
 * This method reads a special file from disk which contains the time of the first/last
 * contact (with some other node) for each network node. These time values correspond to
 * each node's presence during the trace duration. Two arrays (inPresence, outPresence)
 * are filled with the above information and a list with the IDs of the inactive nodes,
 * i.e. nodes without any contact.
 */
void TrafficGen::FillPresenceList(string Path)
{
	int i;


	this->inPresence = (double *) malloc(TotalNodes * sizeof(double));
	this->outPresence = (double *) malloc(TotalNodes * sizeof(double));

	for(i = 0; i < TotalNodes; i++)
	{
		this->inPresence[i] = -1.0;
		this->outPresence[i] = -1.0;
	}

	FILE *fp = NULL;

	if((fp = fopen(Path.c_str(), "r")) == NULL)
	{
		printf("\nError: Could not open the presence file \"%s\"!!!\n\n", Path.c_str());
		exit(1);
	}

	char A[50];
	char B[50];
	char C[50];
	int Node = 0;
	double inTime = -1.0;
	double outTime = -1.0;

// 	printf("File contents..\n");
	while(fscanf(fp, "%s\t%s\t%s\n", A, B, C) != EOF)
	{
// 		printf("%s\t%s\t%s\t",A,B,C);
		Node = atoi(A);
		if(Node < TotalNodes)
		{
			inTime = -1.0;
			outTime = -1.0;
			if(strcmp(B,"-"))
			{
				if(!strcmp(B,"0"))
				{
					inTime = 0.1;
				}
				else
				{
					inTime = atof(B);
				}

				if(!strcmp(C,"0"))
				{
					outTime = 0.1;
				}
				else
				{
					outTime = atof(C);
				}
			}
			this->inPresence[Node] = inTime;
			this->outPresence[Node] = outTime;
		}
		else
		{
			printf("Warning: Found node IDs inside \"%s\" that are larger than maximum ID permitted (ID:%d)!",Path.c_str(),TotalNodes-1);
		}
	}

	if(fclose(fp))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n",Path.c_str());
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < TotalNodes; i++)
	{
		if(this->inPresence[i] == -1.0 || this->outPresence[i] == -1.0)
		{
			this->Inactive.push_back(i);
		}
	}

	return;
}


/* ArePresent
 * ----------
 * Check if nodes "A" and "B" are both present in "Time". In other words,
 * check if both nodes have already made their first contact (with a network node "X") 
 * and also both of them have at least one remaining contact (with a network node "Y").  
 * Note that "X"/"Y" stands for any node in the network including "A" and "B".
 */
bool TrafficGen::ArePresent(int A, int B,double Time)
{
	if(this->inPresence == NULL && this->outPresence == NULL)
	{
		return true;
	}
// 	printf("Checking Node %d and Node %d for time %f\n",A,B,Time);
// 	printf("Presence time is %d -> %f and %d -> %f\n",A,this->Presence[A],B,this->Presence[B]);
// 	printf("Result:");
	if(this->inPresence[A] <= Time && this->inPresence[B] <= Time && this->inPresence[A] >= 0.0 && this->inPresence[B] >= 0.0 && this->outPresence[A] > Time && this->outPresence[B] > Time && this->outPresence[A] >= 0.0 && this->outPresence[B] >= 0.0)
	{
// 		printf("passed\n");
		return true;
	}
// 	printf("not passed\n");
	return false;
}


/* CreateUniformTraffic
 * --------------------
 * First, all active nodes are found using information gathered from the presence list.
 * These active nodes form all the pairs for which communication (traffic) is permitted.
 * Then, node pairs are selected randomly (uniformly) and a unidirectional message is generated 
 * among them. The time instance of the generated message is chosen uniformly, but the 
 * above should hold:
 * (i) Time instance should lie inside the traffic/transmission period defined previously.
 * (ii) Both nodes should be present in the network in the generated time instance. The
 * term "present node" indicates a node that has joined the network (has made a least one contact) 
 * and it has not disjointed yet (has at least a contact to make).
 */
int *TrafficGen::CreateUniformTraffic(int NumPackets)
{
	if(!this->Sim)
	{
		printf("Traffic generation is not initialized!Aborting..");
		exit(1);
	}
	
	int active=0;
	for (int i=0;i<this->TotalNodes;i++)
	{
		//printf("%d\t%f\t%f\n",i,inPresence[i],outPresence[i]);
		if(inPresence[i] > 0 && outPresence[i] > 0)
		{
			active++;
		}
	}
// 	printf("Active nodes: %d\n",active);
	int *activeNodes=(int *)malloc(sizeof(int)*active);
	int runner=0;
	for (int i=0;i<this->TotalNodes;i++)
	{
		if(inPresence[i] > 0 && outPresence[i] > 0)
		{
			activeNodes[runner]=i;
			runner++;
		}
	}
	int totalPairs=active*(active-1);
	trafficPair *activePairs=(trafficPair *)malloc(sizeof(trafficPair)*totalPairs);
	for(int i=0;i<totalPairs;i++)
	{
		activePairs[i].source=0;
		activePairs[i].dest=0;
	}
	runner=0;
	for(int i=0;i<active;i++)
	{
		for(int j=0;j<active;j++)
		{
			if(i == j)
			{
				continue;
			}
			activePairs[runner].source=activeNodes[i];
			activePairs[runner].dest=activeNodes[j];
			runner++;
		}
	}
// 	printf("All pairs are %d\n",runner);
// 	for(int i=0;i<totalPairs;i++)
// 	{
// 		printf("%d\t%d\n",activePairs[i].source,activePairs[i].dest);
// 	}
	int *PacketsFor=(int*)malloc(sizeof(int)*this->TotalNodes);
	for(int i=0;i<this->TotalNodes;i++)
	{
		PacketsFor[i]=0;
	}
	int Sender=0;
	int Receiver=0;
	Event *genPacket=NULL;
	double insTime=0.0;
	double commonIn=0.0;
	double commonOut=0.0;
	int skipped=0;
	int randomPair=-1;
	for(int i=0;i<NumPackets;i++)
	{
		randomPair=rand()%totalPairs;
		Sender=activePairs[randomPair].source;
		Receiver=activePairs[randomPair].dest;
		//Get the first time instance that both the sender and the receiver exist to the network  
		if(this->inPresence[Sender] > this->inPresence[Receiver])
		{
			commonIn=this->inPresence[Sender];
		}
		else
		{
			commonIn=this->inPresence[Receiver];
		}
		
		//Get the last time instance that both the sender and the receiver exist to the network
		if(this->outPresence[Sender] < this->outPresence[Receiver])
		{
			commonOut=this->outPresence[Sender];
		}
		else
		{
			commonOut=this->outPresence[Receiver];
		}
		//keep time interval inside warm-up and cool down periods
		if(commonIn < this->SndPrdStart)
		{
			commonIn=this->SndPrdStart;
		}
		
		if(commonOut > this->SndPrdStop)
		{
			commonOut=this->SndPrdStop;
		}
		//check if the two nodes selected have a common time interval that are both present in the network 
		if(commonIn >= commonOut)
		{
			//skip packet creation (we will create less packets than NumPackets)
// 			printf("Skipped: %d %d %f %f\n",Sender,Receiver,commonIn,commonOut);
			skipped++;
			continue;
		}
		insTime=(double)(rand()%(int)(commonOut-commonIn))+(int)commonIn;
// 		printf("Scheduled: %d %d %f\n",Sender,Receiver,insTime);
		genPacket=new PacketGeneration(insTime,Sender,Receiver,0);
		Sim->InsertEvent(genPacket);
		PacketsFor[Receiver]++;
	}
	totalStatPkts = NumPackets-skipped;
// 	printf("\nCreated %d packets (asked for %d)\n",totalStatPkts,NumPackets);
	//free memory
	free(activeNodes);
	free(activePairs);
	return PacketsFor;
}


int *TrafficGen::CreateBurstTraffic(int NumPackets)
{
	if(!this->Sim)
	{
		printf("Traffic generation is not initialized!Aborting..");
		exit(1);
	}
	
	int active=0;
	for (int i=0;i<this->TotalNodes;i++)
	{
		//printf("%d\t%f\t%f\n",i,inPresence[i],outPresence[i]);
		if(inPresence[i] > 0 && outPresence[i] > 0)
		{
			active++;
		}
	}
// 	printf("Active nodes: %d\n",active);
	int *activeNodes=(int *)malloc(sizeof(int)*active);
	int runner=0;
	for (int i=0;i<this->TotalNodes;i++)
	{
		if(inPresence[i] > 0 && outPresence[i] > 0)
		{
			activeNodes[runner]=i;
			runner++;
		}
	}
	int totalPairs=active*(active-1);
	trafficPair *activePairs=(trafficPair *)malloc(sizeof(trafficPair)*totalPairs);
	for(int i=0;i<totalPairs;i++)
	{
		activePairs[i].source=0;
		activePairs[i].dest=0;
	}
	runner=0;
	for(int i=0;i<active;i++)
	{
		for(int j=0;j<active;j++)
		{
			if(i == j)
			{
				continue;
			}
			activePairs[runner].source=activeNodes[i];
			activePairs[runner].dest=activeNodes[j];
			runner++;
		}
	}
// 	printf("All pairs are %d\n",runner);
// 	for(int i=0;i<totalPairs;i++)
// 	{
// 		printf("%d\t%d\n",activePairs[i].source,activePairs[i].dest);
// 	}
	int *PacketsFor=(int*)malloc(sizeof(int)*this->TotalNodes);
	for(int i=0;i<this->TotalNodes;i++)
	{
		PacketsFor[i]=0;
	}
	int Sender=0;
	int Receiver=0;
	Event *trans=NULL;
	double insTime=0.0;
	double commonIn=0.0;
	double commonOut=0.0;
	int skipped=0;
	int randomPair=-1;
	int flowsize=3;
	int flows=NumPackets/(2*flowsize); //each flow will be composed of 10 packets
	for(int i=0;i<flows;i++)
	{
		randomPair=rand()%totalPairs;
		Sender=activePairs[randomPair].source;
		Receiver=activePairs[randomPair].dest;
		//Get the first time instance that both the sender and the receiver exist to the network  
		if(this->inPresence[Sender] > this->inPresence[Receiver])
		{
			commonIn=this->inPresence[Sender];
		}
		else
		{
			commonIn=this->inPresence[Receiver];
		}
		
		//Get the last time instance that both the sender and the receiver exist to the network
		if(this->outPresence[Sender] < this->outPresence[Receiver])
		{
			commonOut=this->outPresence[Sender];
		}
		else
		{
			commonOut=this->outPresence[Receiver];
		}
		//keep time interval inside warm-up and cool down periods
		if(commonIn < this->SndPrdStart)
		{
			commonIn=this->SndPrdStart;
		}
		
		if(commonOut > this->SndPrdStop)
		{
			commonOut=this->SndPrdStop;
		}
		//check if the two nodes selected have a common time interval that are both present in the network 
		if(commonIn >= commonOut)
		{
			//skip packet creation (we will create less packets than NumPackets)
// 			printf("Skipped: %d %d %f %f\n",Sender,Receiver,commonIn,commonOut);
			skipped++;
			continue;
		}
		insTime=(double)(rand()%(int)(commonOut-commonIn))+(int)commonIn;
		for(int j=0;j<flowsize;j++)
		{
// 			printf("Scheduled: %d %d %f\n",Sender,Receiver,insTime);
			trans=new PacketGeneration(insTime+(j*7200),Sender,Receiver,0);
			this->Sim->InsertEvent(trans);
			PacketsFor[Receiver]++;
			
			trans=new PacketGeneration(insTime+(j*7200),Receiver,Sender,0);
			this->Sim->InsertEvent(trans);
			PacketsFor[Sender]++;
		}
	}
	totalStatPkts = NumPackets-skipped;
	printf("\nCreated %d packets (asked for %d)\n",totalStatPkts,NumPackets);
	return PacketsFor;
}


bool TrafficGen::isInactive(int nodeID)
{
	if(find(this->Inactive.begin(), this->Inactive.end(), nodeID) != this->Inactive.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}


int *TrafficGen::CreateSampleTraffic(int NumPackets)
{
	int i;
	int Sender;
	int Receiver;
	int *PacketsFor;
	int *presentNodes;
	double creationTime;
	Event *trans;


	if(!this->Sim)
	{
		printf("Traffic generation is not initialized!Aborting..");
		exit(1);
	}

	PacketsFor = (int *) malloc(this->TotalNodes * sizeof(int));
	
	for(i = 0; i < this->TotalNodes; i++)
	{
		PacketsFor[i] = 0;
	}

	Sender = 0;
	Receiver = 0;
	totalStatPkts = 0;
	trans = NULL;

	for(i = 0; i < NumPackets; i++)
	{
		creationTime = (double) (rand() % ((int) (this->traceDuration + 1.0)));

		presentNodes = getAllPresentNodes(creationTime);

		if(presentNodes[0] > 1)
		{
			Sender = presentNodes[(rand() % presentNodes[0]) + 1];
			Receiver = presentNodes[(rand() % (presentNodes[0] - 1)) + 1];

			if(Sender == Receiver)
			{
				Receiver = presentNodes[presentNodes[0]];
			}
			
			if((creationTime >= this->SndPrdStart) && (creationTime <= this->SndPrdStop))
			{
				totalStatPkts++;
			}
			trans = new PacketGeneration(creationTime, Sender, Receiver, 0);
			this->Sim->InsertEvent(trans);
			PacketsFor[Receiver]++;
		}

		free(presentNodes);
	}

	return PacketsFor;
}


int *TrafficGen::CreatePredefinedTraffic()
{
	int i;
	int field;
	int Sender;
	int Receiver;
	int *PacketsFor;
	double creationTime;
	char *word;
	char line[512];
	char input[] = "../utils/traffic-scenarios/Debugging-traffic.txt";
	Event *trans;
	FILE *filePointer;


	if(!this->Sim)
	{
		printf("Traffic generation is not initialized!Aborting..");
		exit(1);
	}
	
	PacketsFor = (int *) malloc(this->TotalNodes * sizeof(int));
	
	for(i = 0; i < this->TotalNodes; i++)
	{
		PacketsFor[i] = 0;
	}

	Sender = 0;
	Receiver = 0;
	totalStatPkts = 0;
	creationTime = 0.0;
	trans = NULL;


	/* Open the traffic trace file */
	if(!(filePointer = fopen(input, "r")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}


	while(fgets(line, 512, filePointer))
	{
		field = 0;
		word = strtok(line, "\t");

		while(word)
		{
			field++;

			/* Identify the type of the attribute */
			if(field == 1)
			{/* Creation Time */
				creationTime = atof(word);
			}
			else if(field == 2)
			{/* Sender */
				Sender = atoi(word);
			}
			else if(field == 3)
			{/* Receiver */
				Receiver = atoi(word);
			}

			word = strtok(NULL, "\t");
		}

		totalStatPkts++;
		trans = new PacketGeneration(creationTime, Sender, Receiver, 0);
		
		this->Sim->InsertEvent(trans);
		PacketsFor[Receiver]++;
	}


	/* Close the traffic trace file */
	if(fclose(filePointer))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}

	return PacketsFor;
}


int *TrafficGen::getAllPresentNodes(double creationTime)
{
	int i;
	int *presentNodes;


	presentNodes = (int *) malloc(sizeof(int));
	presentNodes[0] = 0;

	for(i = 0; i < this->TotalNodes; i++)
	{
		if((inPresence[i] <= creationTime) && (outPresence[i] >= creationTime))
		{
			presentNodes[0]++;
			presentNodes = (int *) realloc(presentNodes, (presentNodes[0] + 1) * sizeof(int));
			presentNodes[presentNodes[0]] = i;
		}
	}

	return presentNodes;
}
