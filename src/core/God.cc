/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015-2018  Nikolaos Papanikos,
 *  Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou
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


#ifndef GOD_H
	#define GOD_H
	#include "God.h"
#endif

#ifndef NODE_H
	#define NODE_H
	#include "Node.h"
#endif


/* Constructor:God
 * ---------------
 * God has access to all simulation components. It is useful in the early stages of a new routing 
 * protocol creation. For example, a node can get access to another node's utility values without
 * exchanging any control packets.
 */
God::God(Settings *S)
{
	int i;


	this->Set = S;
	this->SimNodes = Set->getNN();
	this->CurTime = 0.0;

	DeliveredPkts.clear();

	NodeFriends = (int **) malloc(this->SimNodes * sizeof(int *));
	for(i = 0; i < this->SimNodes; i++)
	{
		NodeFriends[i]=NULL;
	}

	this->pktStats.clear();

	this->EncodingsTypeA = 0;
	this->EncodingsTypeB = 0;

	if(S->ProfileExists())
	{
		if((this->outputFilename = Set->GetProfileAttribute("GodOutput")) != "none")
		{
			if(!(this->outFile = fopen(this->outputFilename.c_str(), "w")))
			{
				printf("\n[Error]: Could not open the file: %s\n\n", this->outputFilename.c_str());
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			this->outFile = NULL;
		}
	}
	else
	{
		this->outputFilename = "none";
		this->outFile = NULL;
	}

	return;
}


God::~God()
{
	vector<CHUtil*>::iterator it;


	free(NodeFriends);

	for (it = HCustodians.begin(); it != HCustodians.end(); ++it)
	{
		free((*it));
	}

	this->pktStats.clear();

	if(this->outputFilename != "none")
	{
		if(fclose(this->outFile))
		{
			printf("\n[Error]: Could not close the file: %s\n\n", this->outputFilename.c_str());
			exit(EXIT_FAILURE);
		}
	}

	return;
}


/* setSimTime
 * ----------
 * God should keep track of the simulation time. This method sets the current simulation time.
 */
void God::setSimTime(double SimTime)
{
	this->CurTime = SimTime;
}


/* getSimTime
 * ----------
 * Returns the simulation time that God has.
 */
double God::getSimTime(void)
{
	return this->CurTime;
}


/* InitDelivered
 * -------------
 * Initialization of data structure to keep the delivered packets.
 */
void God::InitDelivered(int NumberOfPackets )
{
	return;
}


/* AddDelivered
 * ------------
 * The simulator keeps track of the successfully delivered data packets.
 */
void God::AddDelivered(int pktID)
{
	DeliveredPkts.insert(pktID);
	return;
}


/* IsDelivered
 * -----------
 * Returns if the packet with pktID is delivered or not.
 */
bool God::IsDelivered(int pktID)
{
	set<int>::iterator it=DeliveredPkts.find(pktID);
	
	if(it == DeliveredPkts.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}


/* setActiveConnections
 * --------------------
 * Enables access to the ConnectionMap.
 */
void God::setActiveConnections(ConnectionMap *Map)
{
	this->ActiveConnections=Map;
	return;
}


/* setSimulationNodes
 * ------------------
 * Enables access to all simulation Nodes.
 */
void God::setSimulationNodes(std::vector< Node* > *N)
{
	this->SimulationNodes=N;
}


/* getAlternativeCustodian
 * -----------------------
 * 
 */
int God::getAlternativeCustodian(int congestedNode)
{
	int tmpNode;
	int alternativeCustodian;
	int alternativeSpace;


	alternativeCustodian = congestedNode;
	alternativeSpace = 0;

	for(tmpNode = 0; tmpNode < SimNodes; tmpNode++)
	{
		if(tmpNode != congestedNode)
		{
			if(ActiveConnections->AreConnected(congestedNode, tmpNode))
			{
				if((*SimulationNodes)[tmpNode]->Buffer->getAvailSpace() > alternativeSpace)
				{
					alternativeCustodian = tmpNode;
					alternativeSpace = (*SimulationNodes)[tmpNode]->Buffer->getAvailSpace();
				}
			}
		}
	}

	return alternativeCustodian;
}


/* getNumForwards
 * --------------
 * Returns statistics regarding the number of forwards made by node with nodeID.
 */
int God::getNumForwards(int nodeID)
{
	return (*SimulationNodes)[nodeID]->Stat->getForwards();
}


/* getSimilarity
 * -------------
 * Returns the similarity value between nodes nodeID and desID as perceived 
 * by node with nodeID.
 */
int God::getSimilarity(int nodeID, int destID)
{
	Routing *tmpPointer;


	tmpPointer = (*SimulationNodes)[nodeID]->RLogic;
	return ((SimBet *) tmpPointer)->Adja->getSim(destID);
}


/* getEgoNetworkSize
 * -----------------
 * Returns the size of the ego network (direct contacts) for node 
 * with nodeID.
 */
int God::getEgoNetworkSize(int nodeID)
{
	Routing *tmpPointer;


	tmpPointer = (*SimulationNodes)[nodeID]->RLogic;
	return ((SimBet *) tmpPointer)->Adja->getEgoNetworkSize();
}


/* getSimilarityRank
 * -----------------
 * Returns the rank of the similarity between nodeID and destID as perceived by
 * nodeID. The rank is calculated as the order of the similarity value among the
 * similarity values between nodeID and all other network nodes (as perceived by
 * nodeID).
 */
int God::getSimilarityRank(int nodeID, int destID)
{
	Routing *tmpPointer;


	tmpPointer = (*SimulationNodes)[nodeID]->RLogic;
	return ((SimBet *) tmpPointer)->Adja->getSimilarityRank(destID);
}


/* getProjNetGrowth
 * ------------
 * Returns the projected net growth in buffer occupancy over some interval for
 * node with nodeID.
 */
double God::getProjNetGrowth(int nodeID, double interval)
{
	return (*SimulationNodes)[nodeID]->Buffer->getProjNetGrowth(interval);
}


/* getMeanRTTL
 * -----------
 * Returns the mean remaining ttl value for the stored packets in the buffer over some interval
 * for node with nodeID.
 */
double God::getMeanRTTL(int nodeID, double interval)
{
	return (*SimulationNodes)[nodeID]->Buffer->getMeanRTTL(interval);
}


/* updateCustodian
 * ---------------
 * God keeps track for each packet the node that owns the packet and has the
 * highest utility value. This method updates this information.
 */
void God::updateCustodian(int PID,int NID,double Util)
{
	vector<CHUtil*>::iterator it;
	for (it = HCustodians.begin(); it != HCustodians.end(); ++it)
	{
		if((*it)->PID == PID)
		{
			if(Util > (*it)->Util)
			{
				//printf("Packet %d has now util %f\n",(*it)->PID,Util);
				(*it)->Util=Util;
				(*it)->Node=NID;
			}
			return;
		}
	}
	//Not found - create a new entry now
	CHUtil*tmp=(CHUtil*)malloc(sizeof(CHUtil));
	tmp->PID=PID;
	tmp->Node=NID;
	tmp->Util=Util;
	HCustodians.push_back(tmp);
	return;
}


/* getHighestUtil
 * --------------
 * God keeps track for each packet the node that owns the packet and has the
 * highest utility value. This method returns the highest utility value among
 * network nodes that own the packet with PID.
 */
double God::getHighestUtil(int PID)
{
	vector<CHUtil*>::iterator it;
	for (it = HCustodians.begin(); it != HCustodians.end(); ++it)
	{
		if((*it)->PID == PID)
		{
			//printf("Packet %d found and has max utility %f\n",(*it)->PID,(*it)->Util);
			return ((*it)->Util);
		}
	}
	printf("We have a problem :)\n");
	exit(1);
}


/* printCodingInfo
 * ---------------
 * Prints information about encoded packets.
 */
void God::printCodingInfo(void)
{
	printf("Number of type A Codings is %d\n",this->EncodingsTypeA);
	printf("Number of type B Codings is %d\n",this->EncodingsTypeB);
	printf("Transmissions of encoded packets are %d\n",this->EncodedTransmissions);
	return;
}


/* setFriends
 * ----------
 * Nodes can be friends using a specific criterion. This method updates the friends of
 * a node with NID.
 */
void God::setFriends(int NID, int *friends)
{
	free(this->NodeFriends[NID]);
	this->NodeFriends[NID]=friends;
}


/* getFriends
 * ----------
 * Returns an array that contains the friends of node with NID.
 */
int *God::getFriends(int NID)
{
	return this->NodeFriends[NID];
}


void God::generatedPkt(int pktID, int srcID, int dstID, int genTime)
{
	PacketStatistics tmpPktStats;


	tmpPktStats.pktID = pktID;
	tmpPktStats.srcID = srcID;
	tmpPktStats.dstID = dstID;
	tmpPktStats.genTime = genTime;
	tmpPktStats.numForwards = 0;
	tmpPktStats.numHops = 0;
	tmpPktStats.delTime = 0.0;
	tmpPktStats.reachedDst = false;

	this->pktStats.push_back(tmpPktStats);

	return;
}


void God::forwardedPkt(int pktID)
{
	list<PacketStatistics>::iterator it;


	for(it = this->pktStats.begin(); it != this->pktStats.end(); ++it)
	{
		if((*it).pktID == pktID)
		{
			(*it).numForwards++;

			return;
		}
	}

	printf("\n[Error]: (God::forwardedPkt) Could not find the packet with ID %d\n\n", pktID);
	exit(EXIT_FAILURE);
}


void God::deliveredPkt(int pktID, int numHops, double delTime)
{
	list<PacketStatistics>::iterator it;


	for(it = this->pktStats.begin(); it != this->pktStats.end(); ++it)
	{
		if((*it).pktID == pktID)
		{
			if(!(*it).reachedDst)
			{
				(*it).numHops = numHops;
				(*it).delTime = delTime;
				(*it).reachedDst = true;
			}

			return;
		}
	}

	printf("\n[Error]: (God::deliveredPkt) Could not find the packet with ID %d\n\n", pktID);
	exit(EXIT_FAILURE);
}


void God::updatePktStats(int pktID, int numHops, double delTime)
{
	list<PacketStatistics>::iterator it;


	for(it = this->pktStats.begin(); it != this->pktStats.end(); ++it)
	{
		if((*it).pktID == pktID)
		{
			(*it).numHops = numHops;
			(*it).delTime = delTime;

			return;
		}
	}

	printf("\n[Error]: (God::updatePktStats) Could not find the packet with ID %d\n\n", pktID);
	exit(EXIT_FAILURE);
}


int God::getNumHops(int pktID)
{
	list<PacketStatistics>::iterator it;


	for(it = this->pktStats.begin(); it != this->pktStats.end(); ++it)
	{
		if((*it).pktID == pktID)
		{
			return (*it).numHops;
		}
	}

	printf("\n[Error]: (God::getNumHops) Could not find the packet with ID %d\n\n", pktID);
	exit(EXIT_FAILURE);
}


double God::getDelTime(int pktID)
{
	list<PacketStatistics>::iterator it;


	for(it = this->pktStats.begin(); it != this->pktStats.end(); ++it)
	{
		if((*it).pktID == pktID)
		{
			return (*it).delTime;
		}
	}

	printf("\n[Error]: (God::getDelTime) Could not find the packet with ID %d\n\n", pktID);
	exit(EXIT_FAILURE);
}


void God::writePktStats(string fileName)
{
	list<PacketStatistics>::iterator it;
	FILE *pktStatsFile;


	this->pktStats.sort();

	if(!(pktStatsFile = fopen(fileName.c_str(), "w")))
	{
		printf("\n[Error]: Could not open the file: %s\n\n", fileName.c_str());
		exit(EXIT_FAILURE);
	}

	fprintf(pktStatsFile, "#1.Packet ID\t2.Source ID\t3.Destination ID\t4.Generation Time\t5.Number of Forwards\t6.Number of Hops\t7.Delivery Delay (seconds)\n");

	for(it = this->pktStats.begin(); it != this->pktStats.end(); ++it)
	{
		if(Set->getRT() == OPTIMAL_RT)
		{
			/* Optimal Routing would avoid any redundant transmission */
			(*it).numForwards = (*it).numHops;
		}

		if((*it).reachedDst)
		{
			fprintf(pktStatsFile, "%d\t%d\t%d\t%f\t%d\t%d\t%f\n", (*it).pktID, (*it).srcID, (*it).dstID, (*it).genTime, (*it).numForwards, (*it).numHops, (*it).delTime);
		}
		else
		{
			fprintf(pktStatsFile, "%d\t%d\t%d\t%f\t%d\tinf\tinf\n", (*it).pktID, (*it).srcID, (*it).dstID, (*it).genTime, (*it).numForwards);
		}
	}

	if(fclose(pktStatsFile))
	{
		printf("\n[Error]: Could not close the file: %s\n\n", fileName.c_str());
		exit(EXIT_FAILURE);
	}

	return;
}


/* writeString
 * -----------
 * Write a string to the output file, if the file pointer is open.

 */
void God::writeString(string outputData)
{
	if(this->outFile != NULL)
	{
		fprintf(this->outFile, "%s", outputData.c_str());
	}

	return;
}


/* deleteAllReplicas
 * -----------------
 * 
 */
void God::deleteAllReplicas(int pktID)
{
	int tmpNode;


	for(tmpNode = 0; tmpNode < SimNodes; tmpNode++)
	{
		if((*SimulationNodes)[tmpNode]->Buffer->PacketExists(pktID))
		{
			(*SimulationNodes)[tmpNode]->Buffer->removePkt(pktID);
		}
	}

	return;
}



/* optimizeDelay
 * -------------
 * 
 */
bool God::optimizeDelay()
{
	if(Set->ProfileExists())
	{
		if((Set->GetProfileAttribute("Optimal") == "none") || (Set->GetProfileAttribute("Optimal") == "Delay"))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}


/* optimizeForwards
 * ----------------
 * 
 */
bool God::optimizeForwards()
{
	if(Set->ProfileExists())
	{
		if(Set->GetProfileAttribute("Optimal") == "Forwards")
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
