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


#ifndef SIMULATOR_H
	#define SIMULATOR_H
	#include "Simulator.h"
#endif

//#define SIMULATOR_DEBUG


/* Constructor: Simulator
 * ----------------------
 * Initialization of attributes and simulation modules.
 */
Simulator::Simulator(Settings *S)
{
	this->Set=S;
	CurrentTime=0.0;
	//create God
	SimGod=new God(Set);
	//create the packet pool
	Pool=new PacketPool(Set->getTrafficLoad());
	//create a new event list
	SimList=new EventList();
	//create the traffic generator
	Traffic = new TrafficGen(Set->getSeed(), Set->getNN(), Set->getTraceDuration(), Set->getPresenceFilename(), SimList);
	//create a new Connection Map
	CMap=new ConnectionMap(Set->getNN());
	//create a new mac layer (ideal)
	MAClayer=new Ideal(2,SimList,CMap);
	//create Nodelist
	Node *tmp=NULL;
	Statistics *st=NULL;
	for(int i=0;i<Set->getNN();i++)
	{
		st=new Statistics(Pool, i, Set->getTraceDuration(), Set->getTrafficType(), SimGod);
		Stats.push_back(st);
		
		tmp=new Node(i,Pool,MAClayer,st,Set,SimGod);
		Nodes.push_back(tmp);
	}
	//give access to God
	//first to Connection Map
	SimGod->setActiveConnections(CMap);
	//then, give access to all Nodes
	SimGod->setSimulationNodes(&Nodes);
	
	Res=new Results(&Stats,&Nodes,S,SimGod,Traffic);
	//set attributes for partial trace reading
	if(S->getSplit() != -1)
	{
		this->CurrentReadBlock=0;
		this->NumberReadBlocks=S->getSplit();
		this->ReadBlockSize=(S->getLines()-(S->getLines()%S->getSplit()))/S->getSplit();
	}
	else
	{
		this->NumberReadBlocks=0;
		this->ReadBlockSize=0;
	}
	return;
}


Simulator::~Simulator()
{
	delete SimGod;
	delete Pool;
	delete SimList;
	delete CMap;
	delete MAClayer;
	delete Res;
	delete Traffic;

	for(int i=0;i<Set->getNN();i++)
	{
		delete Stats[i];
		delete Nodes[i];
	}
	Stats.clear();
	Nodes.clear();

	return;
}


/* performSimulation
 * -----------------
 * Writes the simulation settings, loads the traffic scenario and the contacts
 * of the trace, and then starts the simulation.
 */
void Simulator::performSimulation()
{
	Res->writeSimulationSettings();

	this->LoadTraffic();
	this->LoadContacts();

	this->startSimulation();

	return;
}


/* startSimulation
 * ---------------
 * Basically this method starts the simulation.
 */
void Simulator::startSimulation()
{
	Event *e=SimList->GetTop();
	Node *Current=NULL;

	if(!Set->GUIon())
	{
		if(Set->getOUT() == T5_OUT)
		{
			Res->initAll();
		}
	}

	while(e != NULL)
	{
		if(Set->getOUT() == T5_OUT)
		{
			Res->updateAll(e->getEventTime());
		}
		//Update time
		this->CurrentTime=e->getEventTime();
		//Inform God about the new Simulation Time
		this->SimGod->setSimTime(this->CurrentTime);
		//printf("Sim Time:%f\n",this->CurrentTime);
		if(e->getEventID() == 1)
		{//Contact up
			//Check if already connected
			bool oldstateA=this->CMap->AreConnected(((ContactUp *)e)->getNodeA(),((ContactUp *)e)->getNodeB());
			//bool oldstateB=this->CMap->AreConnected(((ContactUp *)e)->getNodeB(),((ContactUp *)e)->getNodeA());
			//Inform Connection Map
			this->CMap->SetConnection(((ContactUp *)e)->getNodeA(),((ContactUp *)e)->getNodeB());
			//bidirectional
			this->CMap->SetConnection(((ContactUp *)e)->getNodeB(),((ContactUp *)e)->getNodeA());
			#ifdef SIMULATOR_DEBUG
			//print connections
			this->CMap->PrintConnections(((ContactUp *)e)->getNodeA());
			this->CMap->PrintConnections(((ContactUp *)e)->getNodeB());
			#endif
			//Inform Node
			Current=Nodes[((ContactUp *)e)->getNodeA()];
			Current->ConUpdate(this->CurrentTime,((ContactUp *)e)->getNodeB(),true,oldstateA);
		}
		else if(e->getEventID() == 3)
		{//Contact down
			//Inform Connection Map
			this->CMap->UnSetConnection(((ContactDown *)e)->getNodeA(),((ContactDown *)e)->getNodeB());
			//bidirectional
			this->CMap->UnSetConnection(((ContactDown *)e)->getNodeB(),((ContactDown *)e)->getNodeA());
			#ifdef SIMULATOR_DEBUG
			//print connections
			this->CMap->PrintConnections(((ContactDown *)e)->getNodeA());
			this->CMap->PrintConnections(((ContactDown *)e)->getNodeB());
			#endif
			//Inform Node
			Current=Nodes[((ContactDown *)e)->getNodeA()];
			Current->ConUpdate(this->CurrentTime,((ContactDown *)e)->getNodeB(),false,false);
		}
		else if(e->getEventID() == 2)
		{//Transmission
			#ifdef SIMULATOR_DEBUG
			printf("%f:Transmission event Sender:%d Receiver:%d packet id:%d\n",this->CurrentTime,((Transmission *)e)->getSender(),((Transmission *)e)->getReceiver(),((Transmission *)e)->getpktID());
			#endif
			Current=Nodes[((Transmission *)e)->getReceiver()];
			if(((Transmission *)e)->getSender() == APPLICATION)
			{//The packet comes from application layer
				//The packet ID holds information about final destination of the packet
				//Note: This is done only for application layer packets
				Current->recvFromApp(this->CurrentTime,((Transmission *)e)->getpktID());
			}
			else
			{//Normal packet transmission
				if(Check((Transmission *)e))
				{
					Current->recv(((Transmission *)e)->getpktID(),this->CurrentTime);
				}
			}
		}
		else
		{
			#ifdef SIMULATOR_DEBUG
			printf("Continue loading trace file into memory..\n");
			#endif
			//printf("Check Point Reached. Have to continue uploading from line %d\n",((CheckPoint *)e)->GetLine());
			//SimList->PrintList();
			//printf("---------------\n");
			this->LoadPartialContacts();
		}
		delete e;
		e=SimList->GetTop();
	}

	for(int i=0;i<Set->getNN();i++)
	{
		Nodes[i]->Finalize();
	}

	if(!Set->GUIon())
	{
		if(Set->getOUT() == T5_OUT)
		{
			Res->finalizeAll();
		}

		Res->writeSimulationResults();

		if(Set->getBatchmode())
		{
			Res->printSynopsis();
		}
		else
		{
			Res->printBasicRes(false);
			printf("\nSimulation Ended!\n");
			fflush(stdout);
		}
	}
	else
	{
		Res->printBasicRes(true);
	}

	return;
}


/* LoadContacts
 * ------------
 * Loads contacts from disk. If the split method is enabled, then 
 * contacts are partially uploaded into the memory in order to make
 * simulation faster.
 */
void Simulator::LoadContacts()
{
	if(Set->getSplit() == NOTSET || Set->getSplit() == 1)
	{
		if(!Set->getBatchmode() && !Set->GUIon())
		{
			printf("Loading Contacts ");
			fflush(stdout);
		}
		
		LoadCompleteContacts();
		
		if(!Set->getBatchmode() && !Set->GUIon())
		{
			printf("Starting Simulation...\n");
			fflush(stdout);
		}
	}
	else
	{
		if(!Set->getBatchmode() && !Set->GUIon())
		{
			printf("Starting Simulation...\n");
			if(!Set->getBatchmode())
			{
				printf("Loading piece ");
			}
			fflush(stdout);
		}
		
		LoadPartialContacts();
	}
}


/* LoadCompleteContacts
 * --------------------
 * This method reads the complete trace file from disk 
 * and updates the event list.
 */
void Simulator::LoadCompleteContacts()
{
	FILE *fp=NULL;
	long int Lines=0;
	if(!Set->getBatchmode() && !Set->GUIon())
	{
		printf("(%s)... ",Set->getTraceName()->c_str());
		fflush(stdout);
	}
	if ((fp=fopen(Set->getContactFilename()->c_str(), "r")) == NULL)
	{
		printf("\nError: Could not open the contact file \"%s\"!!!\n\n", Set->getContactFilename()->c_str());
		exit(1);
	}
	Lines=Set->getLines();
	
	int NodeA=0,NodeB=0;
	double Begin=0.0,End=0.0;
	long int current=0;
	long double pos=0.0;
	int oldProgress=0;
	int progress=0;
	char frac[10];
	Event *Up=NULL;
	Event *Down=NULL;
	if(!Set->getBatchmode() && !Set->GUIon())
	{
		fflush(stdout);
		sprintf(frac,"%.1Lf%%",pos);
		printf("%s",frac);
		fflush(stdout);
	}
	if(Set->GUIon())
	{
		progress=(int)pos;
		printf("%d",progress);
		oldProgress=progress;
		fflush(stdout);
	}
	while(fscanf(fp,"%d\t%d\t%lg\t%lg\n",&NodeA,&NodeB,&Begin,&End) != EOF)
	{
		//printf("%d\t%d\t%.10g\t%.10g\n",NodeA,NodeB,Begin,End);
		Up=new ContactUp(Begin,NodeA,NodeB);
		Down=new ContactDown(End,NodeA,NodeB);
		SimList->InsertEvent(Down);
		SimList->InsertEvent(Up);
		//Bidirectional - Proper way to insert bidirectional connection
		Up=new ContactUp(Begin,NodeB,NodeA);
		Down=new ContactDown(End,NodeB,NodeA);
		SimList->InsertEvent(Down);
		SimList->InsertEvent(Up);
		
		if(!Set->getBatchmode() && !Set->GUIon())
		{
			for(unsigned int i=0;i<strlen(frac);i++)
			{
				printf("\b");
			}
			fflush(stdout);
			current++;
			pos=((long double)current/(long double)Lines)*100.0;
			sprintf(frac,"%.1Lf%%",pos);
			printf("%s",frac);
			fflush(stdout);
		}
		if(Set->GUIon())
		{
			current++;
			pos=((long double)current/(long double)Lines)*100.0;
			progress=(int)pos;
			if(progress > oldProgress)
			{
				printf("%d",progress);
				fflush(stdout);
				oldProgress=progress;
			}
		}
	}
	if(!Set->getBatchmode() && !Set->GUIon())
	{
		printf("\n");
		fflush(stdout);
	}
	//SimList->PrintList();
	if(fclose(fp)==EOF)
	{
		printf("ERROR: could not close file");
		exit(1);
	}
	
	return;
}


/* LoadPartialContacts
 * -------------------
 * This method reads the trace file from disk in chunks and updates the event list.
 * Only one chunk can exist in the main memory a given time. A special event type 
 * called "CheckPoint" marks the end of the current chunk in memory (event list).
 * The simulation runs in a chunk basis till a "CheckPoint" event is occurred. Then, 
 * the next chunk is loaded.
 */
void Simulator::LoadPartialContacts()
{
	FILE *fp=NULL;
	long int Lines=Set->getLines();
	long int StartLine=0;
	long int StopLine=0;
	static int oldVal=-1;
	//Check if all lines can be uploaded to memory
	if(StopLine > Lines)
	{
		printf("Line requested is out of range (Line:%ld)!Exiting..\n",StopLine);
		abort();
	}
	//Go to next read block
	this->CurrentReadBlock++;
	char piece[10];
	if(!Set->getBatchmode() && !Set->GUIon())
	{
		sprintf(piece,"%ld/%ld",this->CurrentReadBlock,this->NumberReadBlocks);
		printf("%s",piece);
		fflush(stdout);
	}
	if(Set->GUIon())
	{
		int progress=(int)(((float)this->CurrentReadBlock/(float)this->NumberReadBlocks)*100.0);
		if(progress > oldVal)
		{
			oldVal=progress;
			printf("%d",progress);
			fflush(stdout);
		}
	}
	StartLine=((this->CurrentReadBlock-1)*this->ReadBlockSize)+1;
	//Check if this is the last block
	if(this->CurrentReadBlock == this->NumberReadBlocks)
	{
		StopLine=Lines;
	}
	else
	{
		StopLine=(this->CurrentReadBlock*this->ReadBlockSize);
	}
	//Check if start line exists :) 
	if(StartLine >= Lines)
	{
		printf("Problem with partial reading of the trace file!Exiting..\n");
		abort();
	}
	//printf("Reading lines:%d to %d\n",StartLine,StopLine);
	if ((fp=fopen(Set->getContactFilename()->c_str(), "r")) == NULL)
	{
		printf("\nError: Could not open the contact file \"%s\"!!!\n\n", Set->getContactFilename()->c_str());
		exit(1);
	}
	int NodeA,NodeB;
	double Begin,End;
	//Skip lines that have been already uploaded to memory
	for(int i=1;i<StartLine;i++)
	{
		if(fscanf(fp,"%d\t%d\t%lg\t%lg\n",&NodeA,&NodeB,&Begin,&End) == EOF)
		{
			printf("End of File occurred..Exiting\n");
			abort();
		}
	}
	//printf("Skipped lines from 1 to %d\n",(StartLine-1));
	Event *Up=NULL;
	Event *Down=NULL;
	for(int i=StartLine;i<=StopLine;i++)
	{
		if(fscanf(fp,"%d\t%d\t%lg\t%lg\n",&NodeA,&NodeB,&Begin,&End) == EOF)
		{
			printf("End of File occurred..Exiting\n");
			abort();
		}
		//printf("%d %d %lg %lg\n",NodeA,NodeB,Begin,End);
		Up=new ContactUp(Begin,NodeA,NodeB);
		Down=new ContactDown(End,NodeA,NodeB);
		SimList->InsertEvent(Down);
		SimList->InsertEvent(Up);
		//Bidirectional - Proper way to insert bidirectional connection
		Up=new ContactUp(Begin,NodeB,NodeA);
		Down=new ContactDown(End,NodeB,NodeA);
		SimList->InsertEvent(Down);
		SimList->InsertEvent(Up);
	}
	//check if that was the last parse
	if(this->CurrentReadBlock < this->NumberReadBlocks)
	{
		//get the begin time of next event in the trace file
		if(fscanf(fp,"%d\t%d\t%lg\t%lg\n",&NodeA,&NodeB,&Begin,&End) == EOF)
		{
			printf("End of File occurred..Exiting\n");
			abort();
		}
		//insert checkpoint into event list
		Event *CP=new CheckPoint(Begin,StopLine,StopLine);
		SimList->InsertEvent(CP);
		//SimList->PrintList();
	}
	else
	{
		if(!Set->getBatchmode() && !Set->GUIon())
		{
			printf("\n");
			fflush(stdout);
		}
	}
	if(fclose(fp)==EOF)
	{
		printf("ERROR: could not close file");
		exit(1);
	}
	if(!Set->getBatchmode() && !Set->GUIon())
	{
		for(unsigned int i=0;i<strlen(piece);i++)
		{
			printf("\b");
		}
		fflush(stdout);
	}
	return;
}


/* LoadTraffic
 * -----------
 * This method creates traffic according to the user input. Also,
 * it updates the "statistics" structure to correctly handle the
 * created messages.
 */
void Simulator::LoadTraffic()
{
	int *PacketsFor = NULL;


	if(!Set->getBatchmode() && !Set->GUIon())
	{
		printf("Loading Traffic Scenario...");
		fflush(stdout);
	}

	PacketsFor = Traffic->generateTraffic(Set->getTrafficType(), Set->getTrafficLoad());

	if(!PacketsFor)
	{
		//Initialize statistics
		for(int i=0;i<Set->getNN();i++)
		{
			Stats[i]->SetPktsForMe(0);
		}
	}
	else
	{
		//Initialize statistics
		for(int i=0;i<Set->getNN();i++)
		{
			Stats[i]->SetPktsForMe(PacketsFor[i]);
		}
	}
	free(PacketsFor);

	if(!Set->getBatchmode() && !Set->GUIon())
	{
		printf(" Done!\nTotal Number of Packets in the Traffic Sample: %d\n\n", Traffic->getTotalStatPkts());
		fflush(stdout);
	}

	return;
}


/* Check
 * -----
 * This method checks if the transmission is still valid. For example a reception event may be
 * scheduled, but in the mean time connection between nodes may have been lost.
 */
bool Simulator::Check(Transmission* trans)
{
	return CMap->AreConnected(trans->getSender(),trans->getReceiver());
}
