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
 *  Written by Dimitrios-Georgios Akestoridis.
 */


#ifndef OPTIMAL_H
	#define OPTIMAL_H
	#include "Optimal.h"
#endif

//#define OPTIMAL_DEBUG


Optimal::Optimal(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics *St, Settings *S, God *G): Routing(PP, mc, Bf, NID, St, S, G)
{
	return;
}


Optimal::~Optimal()
{
	return;
}


void Optimal::NewContact(double CTime, int NID)
{
	this->Contact(CTime, NID);

	return;
}


void Optimal::Contact(double CTime, int NID)
{
	int *Information;


	Information = DM->GetInfo();
	if(Information != NULL)
	{
		printf("\n[Error]: (Optimal::Contact) Node %d received information about known delivered packets from node %d\n\n", this->NodeID, NID);
		exit(EXIT_FAILURE);
	}
	else
	{
		DM->CleanBuffer(this->Buf);
		if(DM->ExchangeDirectSummary())
		{
			printf("\n[Error]: (Optimal::Contact) Node %d is trying to send a summary packet for direct delivery to node %d\n\n", this->NodeID, NID);
			exit(EXIT_FAILURE);
		}
		else
		{
			SendDirectPackets(CTime, NID);
		}
	}

	return;
}


void Optimal::AfterDirectTransfers(double CTime, int NID)
{
	Header *h;
	Packet *EnhSumPkt;
	struct PktIDandHops *pktData;


	/* Create an enhanced summary packet with the IDs and hops of the packets that I carry */
	pktData = Buf->getPktIDsandHopsNotDestinedTo(NID);
	EnhSumPkt = new EnhancedSummaryPacket(CTime, 0);
	EnhSumPkt->setContents((void *) pktData);
	h = new BasicHeader(this->NodeID, NID);
	EnhSumPkt->setHeader(h);
	pktPool->AddPacket(EnhSumPkt);


	/* Send the packet to the encountered node */
	Mlayer->SendPkt(CTime, this->NodeID, NID, EnhSumPkt->getSize(), EnhSumPkt->getID());

	#ifdef OPTIMAL_DEBUG
		printf("%f: Node %d sent an enhanced summary packet with ID %d for node %d\n", CTime, this->NodeID, EnhSumPkt->getID(), NID);
	#endif

	return;
}


void Optimal::ContactRemoved(double CTime, int NID)
{
	return;
}


void Optimal::recv(double rTime, int pktID)
{
	int PacketID;
	Packet *p;
	Header *h;


	/* Sanity check */
	if((p = pktPool->GetPacket(pktID)) == NULL)
	{
		printf("\n[Error]: (Optimal::recv) Node %d received a packet with ID %d that does not exist in the packet pool\n\n", this->NodeID, pktID);
		exit(EXIT_FAILURE);
	}

	h = p->getHeader();

	if(h->IsDuplicate())
	{
		PacketID = h->GetOriginal();
	}
	else
	{
 		PacketID = pktID;
	}

	if((rTime < 0.0) || (p->GetStartTime() < 0.0))
	{
		printf("\n[Error]: (Optimal::recv) Node %d received a packet with ID %d from node %d at %f that was created at %f and its type is %d\n\n", this->NodeID, pktID, h->GetprevHop(), rTime, p->GetStartTime(), p->getType());
		exit(EXIT_FAILURE);
	}


	/* Identify the type of the packet */
	switch(p->getType())
	{
		case DATA_PACKET:
		{
			ReceptionData(h, p, pktID, rTime, PacketID);
			break;
		}
		case ENHANCED_SUMMARY_PACKET:
		{
			ReceptionEnhancedSummary(h, p, pktID, rTime);
			break;
		}
		case REQUEST_PACKET:
		{
			ReceptionRequest(h, p, pktID, rTime);
			break;
		}
		default:
		{
			printf("\n[Error]: (Optimal::recv) The type of the packet with ID %d that node %d received from node %d is unknown (%d)\n\n", pktID, this->NodeID, h->GetprevHop(), p->getType());
			exit(EXIT_FAILURE);
		}
	}

	return;
}


void Optimal::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	#ifdef OPTIMAL_DEBUG
		printf("%f: Node %d received a data packet with ID %d from node %d\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
	#endif


	if((hd->GetSource() == this->NodeID) && (hd->GetNextHop() == -1))
	{/* I am the creator of the packet (it came from the application layer) */
		Buf->addPkt(RealID, hd->GetDestination(), hd->GetSource(), CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
		Stat->pktGen(RealID, hd->GetSource(), hd->GetDestination(), CurrentTime);
	}
	else if(hd->GetNextHop() != this->NodeID)
	{/* I am not the next hop */
		if(!pkt->AccessPkt())
		{
			pktPool->ErasePacket(PID);
		}
	}
	else
	{/* I am the next hop */
		if(hd->GetDestination() == this->NodeID)
		{/* I am the destination of the packet */
			if(DM->NoDuplicatesSupport() || DM->isDelivered(RealID))
			{
				printf("\n[Error]: (Optimal::ReceptionData) Cannot find the fastest path with the minimum number of hops, because the nodes are not allowed to receive duplicate packets\n\n");
				exit(EXIT_FAILURE);
			}
			else
			{
				Stat->pktRec(hd->GetHops(), (CurrentTime - pkt->GetStartTime()), pkt, pkt->GetStartTime(), true);

				if(!pkt->AccessPkt())
				{
					pktPool->ErasePacket(PID);
				}
			}
		}
		else
		{/* I am a relay node */
			if(Buf->PacketExists(RealID))
			{
				printf("[Error]: (Optimal::ReceptionData) Node %d received a packet with ID %d from node %d that already exists in its buffer\n", this->NodeID, RealID, hd->GetprevHop());
				exit(EXIT_FAILURE);
			}
			else
			{
				Buf->addPkt(RealID, hd->GetDestination(), hd->GetSource(), CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
				Stat->incTimesAsRelayNode(pkt->GetStartTime());

				if(!pkt->AccessPkt())
				{
					pktPool->ErasePacket(PID);
				}
			}
		}
	}

	return;
}


void Optimal::ReceptionEnhancedSummary(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	int i;
	int numUnknownPkts;
	int *unknownPktIDs;
	struct PktIDandHops *pktData;
	Packet *ReqPacket;
	Header *head;


	#ifdef OPTIMAL_DEBUG
		printf("%f: Node %d received an enhanced summary packet with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif


	/* Get the contents of the enhanced summary packet */
	pktData = (struct PktIDandHops *) pkt->getContents();

	numUnknownPkts = 0;
	unknownPktIDs = (int *) malloc(sizeof(int));
	for(i = 1; i <= pktData[0].pktID; i++)
	{
		if(!(Buf->PacketExists(pktData[i].pktID)))
		{
			numUnknownPkts++;
			unknownPktIDs = (int *) realloc(unknownPktIDs, ((numUnknownPkts + 1) * sizeof(int)));
			unknownPktIDs[numUnknownPkts] = pktData[i].pktID;
		}
		else
		{
			/* Check if I could have received this packet with less hops */
			if(pktData[i].pktHops + 1 < Buf->GetHops(pktData[i].pktID))
			{
				Buf->SetHops(pktData[i].pktID, pktData[i].pktHops + 1);
			}
		}
	}
	unknownPktIDs[0] = numUnknownPkts;


	/* Create a packet with the IDs of the unknown packets */
	ReqPacket = new RequestPacket(CurrentTime, 0);
	ReqPacket->setContents((void *) unknownPktIDs);
	head = new BasicHeader(this->NodeID, hd->GetprevHop());
	ReqPacket->setHeader(head);
	pktPool->AddPacket(ReqPacket);


	/* Send the packet to the encountered node */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), ReqPacket->getSize(), ReqPacket->getID());

	pktPool->ErasePacket(PID);

	return;
}


void Optimal::ReceptionRequest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	int i;
	int *reqPktIDs;
	int *outgoing;


	#ifdef OPTIMAL_DEBUG
		printf("%f: Node %d received a request packet with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif


	/* Get the contents of the request packet */
	reqPktIDs = (int *) pkt->getContents();


	/* Apply scheduling and congestion control */
	for(i = 1; i <= reqPktIDs[0]; i++)
	{
		sch->addPacket(reqPktIDs[i], NULL);
	}

	outgoing = sch->getOutgoingPackets();
	outgoing = CC->filterPackets(outgoing);

	if(outgoing != NULL)
	{
		for(i = 1; i <= outgoing[0]; i++)
		{
			SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(), 1);
		}

		free(outgoing);
	}

	pktPool->ErasePacket(PID);

	return;
}


void Optimal::SendPacket(double STime, int pktID, int nHop, int RepValue)
{
	Packet *p;
	Packet *newPkt;
	int CurrentN;


	/* Sanity check */
	if((p = pktPool->GetPacket(pktID)) == NULL)
	{
		printf("\n[Error]: (Optimal::SendPacket) Node %d is trying to send to node %d a packet with ID %d that does not exist in the packet pool\n\n", this->NodeID, nHop, pktID);
		exit(EXIT_FAILURE);
	}


	/* Duplicate the packet */
	newPkt = p->Duplicate(Buf->GetHops(pktID));
	newPkt->getHeader()->SetNextHop(nHop);
	newPkt->getHeader()->SetprevHop(this->NodeID);
	newPkt->getHeader()->SetRep(RepValue);
	pktPool->AddPacket(newPkt);


	/* Broadcast the packet to my neighbors */
	CurrentN = Mlayer->BroadcastPkt(STime, this->NodeID, newPkt->getSize(), newPkt->getID());

	if(CurrentN > 0)
	{/* Set access attribute to safely delete the packet later */
		newPkt->SetRecipients(CurrentN);

		if(newPkt->getHeader()->GetDestination() == nHop)
		{
			Stat->incHandovers(Buf->GetPktCreationTime(pktID));
		}

		Stat->incForwards(pktID, Buf->GetPktCreationTime(pktID));
	}
	else
	{/* Cancel broadcast and delete packet (there are no neighbors) */
		pktPool->ErasePacket(newPkt->getID());
	}

	return;
}
