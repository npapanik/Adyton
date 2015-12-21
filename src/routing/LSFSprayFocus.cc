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


#ifndef LSFSPRAYFOCUS_H
	#define LSFSPRAYFOCUS_H
	#include "LSFSprayFocus.h"
#endif

// +----------------+
// | Protocol steps |
// +----------------+----------------------------------------------------------+
// | Node (a) encounters node (b)                                              |
// |===========================================================================|
// | <steps related to direct delivery are performed (check Routing.cc)>       |
// |===========================================================================|
// | (a): [method: AfterDirectTransfers()]                                     |
// |      - Creates a <summary packet> that contains a list of destinations    |
// |        for which node a has at least one packet for. Also, the LTS utility|
// |        for each destination is piggybacked to this <summary packet>.      |
// | (a)--------------summary packet (Destinations,LTS Utils) ---------->(b)   |
// |===========================================================================|
// | (b): [method: ReceptionUtilDest()]                                        |
// |      - Checks for which destinations it has better LTS utility than       |
// |        node (a).                                                          |
// |      - Creates a <request packet> that contains the destination IDs       |
// |        for which it is more capable than node (a).                        |
// | (a)<--------------request packet (destination IDs)------------------(b)   |
// |===========================================================================|
// | (a): [method: ReceptionAvailDest()]                                       |
// |      - Receives the <request packet>.                                     |
// |      - Gets a list of packets that their destination is inside the        |
// |        <request packet>.                                                  |
// |      - Creates a <summary packet> that contains the packet IDs of the     |
// |        packets that their destination is inside the <request packet>.     |
// | (a)--------------summary packet (packet IDs)----------------------->(b)   |
// |===========================================================================|
// | (b): [method:ReceptionSummary()]                                          |
// |      - Checks which packets are missing from its buffer.                  |
// |      - Creates a request with packet IDs that are not inside its buffer.  |
// | (a)<--------------request packet (packet IDs)-----------------------(b)   |
// |===========================================================================|
// | (a): [method: ReceptionRequest()]                                         |
// |      - Gets the requested packets from its buffer                         |
// |      - A replica for all packets that have replication value larger than 1| 
// |        is created (replication values change as needed). -Spray phase-    |
// |      - In the case that a stored packet has replica value equal to 1 and  | 
// |        node (b) has better LTS utility value, then the packet is          |  
// |        transmitted as id without creating a replica. -Focus phase-        |
// |      - Both packet replicas and original packets are transmitted.         |
// | (a)--------------packet replica 1 destined to b-------------------->(b)   |
// | (a)--------------packet replica 2 destined to b-------------------->(b)   |
// |                              ...                                          |
// | (a)--------------packet replica n destined to b-------------------->(b)   |
// |===========================================================================|
// | (b): [method: ReceptionData()]                                            |
// |      - Receives the transmitted packets one by one                        |
// +---------------------------------------------------------------------------+


LSFSprayFocus::LSFSprayFocus(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St, Settings* S, God* G): LSFSpray(PP, mc, Bf, NID, St,S,G)
{
	betterDests=NULL;
	return;
}


LSFSprayFocus::~LSFSprayFocus()
{
	return;
}


void LSFSprayFocus::ReceptionAvailDest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Received packet containing available destinations
	int *Requests=(int *)pkt->getContents();
	//Store temporarily the destinations for which the encountered node is better
	int elements=Requests[0]+1;
	if(elements > 1)
	{
		betterDests=(int *)malloc(sizeof(int)*elements);
		memcpy(betterDests,Requests,elements*sizeof(int));
	}
	//Get packets for destinations contained in the packet
// 	int *pktsToSend=Buf->getPackets(Requests);
	int *pktsToSend=Buf->getPacketsNotDestinedTo(hd->GetprevHop());
	//Create new summary packet
	Packet *SumPacket=new SummaryPacket(CurrentTime,0);
	SumPacket->setContents((void *)pktsToSend);
	Header *newhd=new BasicHeader(this->NodeID,hd->GetprevHop());
	SumPacket->setHeader(newhd);
	//Add packet to the packet pool
	pktPool->AddPacket(SumPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),SumPacket->getSize(),SumPacket->getID());
	#ifdef LSFSPRAYFOCUS_DEBUG
	printf("%f:Node %d generated new summary packet with ID:%d for Node %d\n",CurrentTime,this->NodeID,SumPacket->getID(),hd->GetprevHop());
	printf("Summary contents(%d):\n",summary[0]);
	for(int i=1;i<=summary[0];i++)
	{
		printf("%d ",summary[i]);
	}
	printf("\n");
	#endif
	//Delete summary packet to free memory
	pktPool->ErasePacket(PID);
	return;
}


void LSFSprayFocus::ReceptionRequest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Send half of the packet replicas
	int Rcurrent=0;
	int Rnew=0;
	double HowMany=0.0;
	int *rq=(int *)pkt->getContents();
	
	for(int i=1;i<=rq[0];i++)
	{
		sch->addPacket(rq[i],NULL);
	}
	//Apply scheduling
	int *outgoing=sch->getOutgoingPackets();
	//Apply congestion control
	outgoing=CC->filterPackets(outgoing);
	if(outgoing)
	{
		for(int i=1;i<=outgoing[0];i++)
		{
			//Get current replication value
			Rcurrent=(Buf->getPacketData(outgoing[i]))->GetReplicas();
			//Case replication value is 1
			if(Rcurrent == 1)
			{
				if(isEncNodeBetter((Buf->getPacketData(outgoing[i]))->getDestination()))
				{
					SendPacket(CurrentTime,outgoing[i],hd->GetprevHop(),1);
					Buf->removePkt(outgoing[i]);
				}
				continue;
			}
			#ifdef LSFSPRAYFOCUS_DEBUG
			printf("%d:For pkt %d Rcurrent is %d\n",this->NodeID,outgoing[i],Rcurrent);
			Buf->PrintPkts();
			#endif
			//Calculate new replication value
			HowMany=((double)Rcurrent)/2.0;
			Rnew=floor(HowMany);
			//Send packet
			Stat->incReps(Buf->GetPktCreationTime(outgoing[i]));
			SendPacket(CurrentTime,outgoing[i],hd->GetprevHop(),Rnew);
			if((Buf->getPacketData(outgoing[i]))->Destination == hd->GetprevHop())
			{
				Buf->removePkt(outgoing[i]);
			}
			else
			{
				//Update Replication value
				(Buf->getPacketData(outgoing[i]))->SetReplicas(Rcurrent-Rnew);
			}
		}
		free(outgoing);
	}
	//remove temporary stored destination for which the encountered node is better
	free(betterDests);
	betterDests=NULL;
	//Request packet
	#ifdef LSFSPRAYFOCUS_DEBUG
	printf("%f:Node %d received a request packet with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("Request contents(%d):\n",rq[0]);
	for(int i=1;i<=rq[0];i++)
	{
		printf("%d ",rq[i]);
	}
	printf("\n");
	#endif
	pktPool->ErasePacket(PID);
	return;
}


bool LSFSprayFocus::isEncNodeBetter(int destination)
{
	if(!betterDests)
	{
		return false;
	}
	for(int i=1;i<=betterDests[0];i++)
	{
		if(betterDests[i] == destination)
		{
			return true;
		}
	}
	return false;
}
