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


#ifndef STATS_H
	#define STATS_H
	#include "Statistics.h"
#endif


/* Constructor: Statistics
 * -----------------------
 * Initializing attributes.
 */
Statistics::Statistics(PacketPool *P, int ID, double duration, int trafType, God *G)
{
	this->me=ID;
	this->sumHops=0;
	this->sumDelay=0.0;
	this->Duplicates=0;
	this->Pool=P;
	this->ReceivedForMe=0;
	this->PktsForMe=0;
	this->ReceptionList=NULL;
	this->currentSlot=-1;
	this->Forwards=0;
	this->ReplicasCreated=0;
	this->timesAsRelayNode=0;
	this->handovers=0;
	this->pktsDropped=0;
	this->srcPktsDropped=0;
	this->relPktsDropped=0;
	this->pktsDied=0;
	this->sampleStartTime=0.2*duration;
	this->sampleEndTime=0.8*duration;
	this->traceDuration=duration;
	this->trafficType=trafType;
	this->SimGod = G;
}


Statistics::~Statistics()
{
	free(this->ReceptionList);

	return;
}


/* SetPktsForMe
 * -------------
 * Sets the total number of packets created and destined to the current
 * destination node. Also, proper initializations take place.
 */
void Statistics::SetPktsForMe(int pkts)
{
	int i;


	this->PktsForMe = pkts;
	this->ReceptionList = (int *) malloc(pkts * sizeof(int));
	this->currentSlot = 0;

	for(i = 0; i < pkts; i++)
	{
		this->ReceptionList[i]=0;
	}

	return;
}


/* PrintResults
 * ------------
 * Prints the per node stats.
 */
void Statistics::PrintResults(void)
{
	printf("Destination of %d packets\n",this->PktsForMe);
	printf("DR:%.2f%%\n",((double)this->ReceivedForMe/(double)this->PktsForMe)*100.0);
	printf("Hops:%f\n",(double)(this->sumHops/this->ReceivedForMe));
	printf("Delay:%f\n",(double)(this->sumDelay/this->ReceivedForMe));
	printf("Duplicates:%f%%\n",((double)this->Duplicates/(double)this->Duplicates+(double)this->ReceivedForMe)*100.0);
	printf("Forwards:%f\n",this->getAvgFw());
	return;
}


/* getAvgDR
 * --------
 * Returns the average delivery ratio in the current node.
 */
double Statistics::getAvgDR(void )
{
	//printf("%d\t%d\n",this->PktsForMe,this->ReceivedForMe);
	if(this->PktsForMe == 0)
	{
		return 0.0;
	}
	return ((double)this->ReceivedForMe/(double)this->PktsForMe)*100.0;
}


/* getAvgDelay
 * -----------
 * Returns the average packet delay in the current node.
 */
double Statistics::getAvgDelay(void )
{
	if(this->ReceivedForMe == 0)
	{
		return 0.0;
	}
	return (double)(this->sumDelay/this->ReceivedForMe);
}


/* getAvgHops
 * ----------
 * Returns the average number of hops (for the delivered packets) in the 
 * current node.
 */
double Statistics::getAvgHops(void)
{
	if(this->ReceivedForMe == 0)
	{
		return 0.0;
	}
	return (double)(this->sumHops/this->ReceivedForMe);
}


/* getAvgFw
 * --------
 * Returns the average number of forwards (for the delivered packets) in the 
 * current node.
 */
double Statistics::getAvgFw(void )
{
	if(this->PktsForMe == 0)
	{
		return 0.0;
	}
	return (double)((double)this->Forwards/(double)this->PktsForMe);
}


/* pktRec
 * ------
 * Informs all the data structures that store the node's stats about the reception of
 * packet p. Note that the updates are made only if packet p is not a duplicate of previous
 * received packet.
 */
void Statistics::pktRec(int hops, double del, Packet *p, double pktCreationTime, bool processDuplicates)
{
	int i;
	int pktID;
	bool alreadyReceived;


	if(isBackgroundTraffic(pktCreationTime))
	{/* Ignore this packet (background traffic) */
		return;
	}
	else if(p->getHeader()->GetDestination() != this->me)
	{/* Ignore this packet (I am not its destination) */
		return;
	}
	else
	{
		/* Sanity check */
		if(this->ReceptionList == NULL)
		{
			printf("\n[Error]: (Statistics::pktRec) The number of incoming packets is not set!\n\n");
			exit(EXIT_FAILURE);
		}

		if(p->getHeader()->IsDuplicate())
		{
			pktID = p->getHeader()->GetOriginal();
		}
		else
		{
	 		pktID = p->getID();
		}

		alreadyReceived = false;
		for(i = 0; i < this->currentSlot; i++)
		{
			if(this->ReceptionList[i] == pktID)
			{
				alreadyReceived = true;
			}
		}

		if(alreadyReceived)
		{
			this->Duplicates++;

			if(processDuplicates)
			{
				this->updateStats(pktID, hops, del);
			}
		}
		else
		{
			this->sumHops += hops;
			this->sumDelay += del;
			this->ReceivedForMe++;

			this->ReceptionList[this->currentSlot] = pktID;
			this->currentSlot++;

			this->SimGod->deliveredPkt(pktID, hops, del);
		}

		return;
	}
}


void Statistics::pktGen(int pktID, int srcID, int dstID, int genTime)
{
	if(!isBackgroundTraffic(genTime))
	{
		this->SimGod->generatedPkt(pktID, srcID, dstID, genTime);
	}

	return;
}


/* updateStats
 * -----------
 * Updates the statistics kept for packet duplicates.
 */
void Statistics::updateStats(int pktID, int dupHops, double dupDelay)
{
	int curHops;
	double curDelay;


	/* Get the current statistics of the packet */
	curHops = this->SimGod->getNumHops(pktID);
	curDelay = this->SimGod->getDelTime(pktID);


	/* Sanity check */
	if(curDelay - dupDelay > DBL_EPSILON)
	{
		printf("\n[Error]: (Statistics::updateStats) Received a duplicate packet with smaller delivery delay\n\n");
		exit(EXIT_FAILURE);
	}

	if((dupHops < 1) || (curHops < 1))
	{
		printf("\n[Error]: (Statistics::updateStats) Received a duplicate packet in %d hops, which has already been delivered in %d hops\n\n", dupHops, curHops);
		exit(EXIT_FAILURE);
	}


	/* Identify the version of Optimal Routing */
	if(this->SimGod->optimizeDelay())
	{/* Search for the fastest delivery path with the minimum number of hops */
		if(dupDelay - curDelay > DBL_EPSILON)
		{
			/* Received a duplicate packet with higher delivery delay */
			this->SimGod->deleteAllReplicas(pktID);
		}
		else
		{
			if(dupHops < curHops)
			{
				/* Received a duplicate packet with less number of hops and equal delivery delay */
				this->sumHops -= curHops;
				this->sumDelay -= curDelay;

				this->sumHops += dupHops;
				this->sumDelay += dupDelay;

				this->SimGod->updatePktStats(pktID, dupHops, dupDelay);
			}
		}
	}
	else if(this->SimGod->optimizeForwards())
	{/* Search for the shortest delivery path with the minimum delivery delay */
		if(dupHops < curHops)
		{
			/* Received a duplicate packet with less number of hops */
			this->sumHops -= curHops;
			this->sumDelay -= curDelay;

			this->sumHops += dupHops;
			this->sumDelay += dupDelay;

			this->SimGod->updatePktStats(pktID, dupHops, dupDelay);
		}
		else
		{
			if((dupDelay - curDelay > DBL_EPSILON) && (curHops == 1))
			{
				/* Received a duplicate packet with higher delivery delay, which has already been delivered directly */
				this->SimGod->deleteAllReplicas(pktID);
			}
		}
	}
	else
	{
		printf("\n[Error]: (Statistics::updateStats) Unknown version of Optimal Routing\n\n");
		exit(EXIT_FAILURE);
	}

	return;
}


/* incForwards
 * -----------
 * This method keeps track of the packet forwards occurred in the current node.
 */
void Statistics::incForwards(int pktID, double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->Forwards++;
		this->SimGod->forwardedPkt(pktID);
	}

	return;
}


/* incDuplicates
 * -------------
 * This method keeps track of the packet duplicates received by the current node.
 */
void Statistics::incDuplicates(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->Duplicates++;
	}

	return;
}


/* incReps
 * ------------
 * This method keeps track of the packet replicas created by the current node.
 */
void Statistics::incReps(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->ReplicasCreated++;
	}

	return;
}


/* incTimesAsRelayNode
 * -------------------
 * This method keeps track of the number of times the current node acted as relay.
 */
void Statistics::incTimesAsRelayNode(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->timesAsRelayNode++;
	}

	return;
}


/* incHandovers
 * ------------
 * This method keeps track of the number of hand-overs occurred. 
 */
void Statistics::incHandovers(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->handovers++;
	}

	return;
}


/* incPktsDropped
 * --------------
 * This method keeps track of the number of packet drops that occurred at this node.
 */
void Statistics::incPktsDropped(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->pktsDropped++;
	}

	return;
}


/* incPktsDied
 * -----------
 * This method keeps track of the number of packet drops that occurred due to TTL expiration.
 */
void Statistics::incPktsDied(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->pktsDied++;
	}

	return;
}


/* incSrcPktsDropped
 * --------------
 * This method keeps track of the number of source packet drops that occurred at this node.
 */
void Statistics::incSrcPktsDropped(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->srcPktsDropped++;
	}

	return;
}


/* incRelPktsDropped
 * --------------
 * This method keeps track of the number of relay packet drops that occurred at this node.
 */
void Statistics::incRelPktsDropped(double pktCreationTime)
{
	if(!isBackgroundTraffic(pktCreationTime))
	{
		this->relPktsDropped++;
	}

	return;
}


bool Statistics::isBackgroundTraffic(double pktCreationTime)
{
	if((this->trafficType == SAMPLE_TT) && !((pktCreationTime >= this->sampleStartTime) && (pktCreationTime <= this->sampleEndTime)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
