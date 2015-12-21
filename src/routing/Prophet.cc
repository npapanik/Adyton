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

#ifndef PROPHET_H
	#define PROPHET_H
	#include "Prophet.h"
#endif

//#define PROPHET_DEBUG

// +----------------+
// | Protocol steps |
// +----------------+----------------------------------------------------------+
// | Node (a) encounters node (b)                                              |
// |===========================================================================|
// | <steps related to direct delivery are performed (check Routing.cc)>       |
// |===========================================================================|
// | (a): [method: AfterDirectTransfers()]                                     |
// |      - Sends a request for the Delivery predictability table of           |
// |        node (b).                                                          |
// |                                                                           |
// |      (a)---------------- DPT request packet ----------------->(b)         |
// |                                                                           |
// |===========================================================================|
// | (b): [method: ReceptionRequestContacts()]                                 | 
// |      - Creates a <DPT respone packet> that contains the Delivery          | 
// |        predictability table of node (b).                                  |
// |                                                                           |
// |      (a)<----------- DPT response packet -- ------------------(b)         |
// |                                                                           |
// |===========================================================================|
// | (a): [method: ReceptionDPT()]                                             |
// |      - Updates node'a (a) Delivery predictability table according to      |
// |        the DPT information received.                                      |
// |      - For each packet that exists inside node's (a) buffer:              |
// |            * If node (b) has a better DPT value than node (a)             |
// |              transmit the packet.                                         |
// |                                                                           |
// | (a)------------------------packet 1 ---------------------->(b)            |
// | (a)------------------------packet 2 ---------------------->(b)            |
// |                             ...                                           |
// | (a)------------------------packet n ---------------------->(b)            |
// |                                                                           |
// |===========================================================================|
// |(b): [method: ReceptionData()]                                             |
// |     - Receives the transmitted packets one by one.                        |
// +---------------------------------------------------------------------------+


Prophet::Prophet(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St, Settings *S, God *G): Routing(PP, mc, Bf, NID, St, S, G)
{
	double Pinit;
	double Pmin;
	double Pmax;
	double beta;
	double gamma;
	double delta;
	double agingTimeUnit;
	string profileAttribute;

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT")) != "none")
	{
		if(profileAttribute == "v1")
		{
			MyDPT = new DPTv1(NID, Set->getNN());
		}
		else if(profileAttribute == "v1.5")
		{
			MyDPT = new DPTv1point5(NID, Set->getNN());
		}
		else if(profileAttribute == "v2")
		{
			MyDPT = new DPTv2(NID, Set->getNN());
		}
		else if(profileAttribute == "v3")
		{
			MyDPT = new DPTv3(NID, Set->getNN());
		}
		else
		{
			printf("Error: Unknown version of DPT (%s)\nExiting...", profileAttribute.c_str());
			exit(1);
		}
	}
	else
	{
		MyDPT = new DPTv3(NID, Set->getNN());
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_Pinit")) != "none")
	{
		Pinit = atof(profileAttribute.c_str());

		if(Pinit < 0.0 || Pinit > 1.0)
		{
			printf("Error: Invalid Pinit value (%f)\nExiting...", Pinit);
			exit(1);
		}

		MyDPT->setPinit(Pinit);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_Pmin")) != "none")
	{
		Pmin = atof(profileAttribute.c_str());

		if(Pmin < 0.0 || Pmin > 1.0)
		{
			printf("Error: Invalid Pmin value (%f)\nExiting...", Pmin);
			exit(1);
		}

		MyDPT->setPmin(Pmin);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_Pmax")) != "none")
	{
		Pmax = atof(profileAttribute.c_str());

		if(Pmax < 0.0 || Pmax > 1.0)
		{
			printf("Error: Invalid Pmax value (%f)\nExiting...", Pmax);
			exit(1);
		}

		MyDPT->setPmax(Pmax);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_beta")) != "none")
	{
		beta = atof(profileAttribute.c_str());

		if(beta < 0.0 || beta > 1.0)
		{
			printf("Error: Invalid beta value (%f)\nExiting...", beta);
			exit(1);
		}

		MyDPT->setBeta(beta);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_gamma")) != "none")
	{
		gamma = atof(profileAttribute.c_str());

		if(gamma < 0.0 || gamma > 1.0)
		{
			printf("Error: Invalid gamma value (%f)\nExiting...", gamma);
			exit(1);
		}

		MyDPT->setGamma(gamma);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_delta")) != "none")
	{
		delta = atof(profileAttribute.c_str());

		if(delta < 0.0 || delta > 1.0)
		{
			printf("Error: Invalid delta value (%f)\nExiting...", delta);
			exit(1);
		}

		MyDPT->setDelta(delta);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("DPT_agingTimeUnit")) != "none")
	{
		agingTimeUnit = atof(profileAttribute.c_str());

		if(agingTimeUnit <= 0.0)
		{
			printf("Error: Invalid aging time unit value (%f)\nExiting...", agingTimeUnit);
			exit(1);
		}

		MyDPT->setAgingTimeUnit(agingTimeUnit);
	}

	return;
}


Prophet::~Prophet()
{
	delete MyDPT;

	return;
}


void Prophet::NewContact(double CTime, int NID)
{
	Contact(CTime, NID);

	return;
}


void Prophet::Contact(double CTime, int NID)
{
	#ifdef CC_DEBUG
		printf("\n@%f: Node %d encountered node %d, while carrying the following packets:\n", CTime, this->NodeID, NID);
		Buf->printBufferContents();
	#endif

	MyDPT->ContactUp(NID, CTime);
	CC->contactUp(CTime, NID);


	/* Send all packets that are destined for node NID */
	SendDirectPackets(CTime, NID);

	return;
}


void Prophet::ContactRemoved(double CTime, int NID)
{
	MyDPT->ContactDown(NID, CTime);
	CC->contactDown(CTime, NID);

	return;
}


void Prophet::recv(double rTime, int pktID)
{
	int PacketID;
	Packet *p;
	Header *h;


	if((p = pktPool->GetPacket(pktID)) == NULL)
	{
		printf("Error: Packet %d doesn't exist in Packet Pool!\nAborting...\n",pktID);
		exit(1);
	}

	h = p->getHeader();

	if(h->IsDuplicate() == true)
	{
		PacketID = h->GetOriginal();
	}
	else
	{
 		PacketID = pktID;
	}

	/* Sanity check */
	if(rTime < 0 || p->GetStartTime() < 0)
	{
		printf("%f: Node %d received new packet with ID %d and type %d from %d\n", rTime, this->NodeID, p->getID(), p->getType(), h->GetprevHop());
		exit(1);
	}

	/* Identify the type of the packet */
	switch(p->getType())
	{
		case DATA_PACKET:
		{
			ReceptionData(h, p, pktID, rTime, PacketID);
			break;
		}
		case REQUEST_CONTACTS_PACKET:
		{
			ReceptionRequestContacts(h, p, pktID, rTime);
			break;
		}
		case DEST_PREDICT_PACKET:
		{
			ReceptionDPT(h, p, pktID, rTime);
			break;
		}
		case REQUEST_BUFFER_INFO:
		{
			ReceptionBufferReq(h, p, pktID, rTime);
			break;
		}
		case BUFFER_INFO:
		{
			ReceptionBufferRsp(h, p, pktID, rTime);
			break;
		}
		default:
		{
			printf("Error: Unknown packet type! Prophet is not using this type of packet..Aborting...\n");
			exit(1);
		}
	}
	return;
}


void Prophet::AfterDirectTransfers(double CTime, int NID)
{
	Packet *ReqPacket;
	Header *h;


	#ifdef CC_DEBUG
		printf("\n@%f: Node %d carries the following packets, after directly delivering the packets destined for node %d:\n", CTime, this->NodeID, NID);
		Buf->printBufferContents();
	#endif


	/* Create a new request contacts packet */
	ReqPacket = new ReqContacts(CTime, 0);
	h = new BasicHeader(this->NodeID, NID);
	ReqPacket->setHeader(h);
	pktPool->AddPacket(ReqPacket);


	/* Send the packet to the new contact */
	Mlayer->SendPkt(CTime, this->NodeID, NID, ReqPacket->getSize(), ReqPacket->getID());

	#ifdef PROPHET_DEBUG
		printf("%d: Sending a request packet to node %d\n", this->NodeID, NID);
	#endif

	return;
}


void Prophet::ReceptionRequestContacts(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	double *myDPT;
	Packet *responsePacket;
	Header *responseHeader;


	#ifdef PROPHET_DEBUG
		printf("%f: Node %d received a request packet for its DPT with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif
	
	/* Create a response packet containing my DPT (and any other utility metric) */
	myDPT = MyDPT->CloneDPT(CurrentTime);

	responsePacket = new DPs(CurrentTime, 0);
	responsePacket->setContents((void *) myDPT);
	responsePacket->setExtraInfo(CC->getExtraInfo(CurrentTime));
	responseHeader = new BasicHeader(this->NodeID, hd->GetprevHop());
	responsePacket->setHeader(responseHeader);
	pktPool->AddPacket(responsePacket);


	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), responsePacket->getSize(), responsePacket->getID());

	#ifdef PROPHET_DEBUG
		printf("%f: Node %d send its DPT with ID %d to node %d\n", CurrentTime, this->NodeID, responsePacket->getID(), hd->GetprevHop());
	#endif
	/* Delete the request packet to free memory */
	pktPool->ErasePacket(PID);
	return;
}


void Prophet::ReceptionDPT(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int i;
	int j;
	int *dstNodes;
	int *pktsToSend;
	int *outgoing;
	void *extraInfo;
	double *otherDPT;
	struct ExchangedInformation *schInfo;
	struct ImportInformation *ccInfo;


	#ifdef PROPHET_DEBUG
		printf("%f: Node %d received a packet containing a DPT with ID %d from node %d\n", CurrentTime, this->NodeID, pkt->getID(), hd->GetprevHop());
	#endif

	/* Get packet contents */
	otherDPT = (double *) pkt->getContents();
	extraInfo = pkt->getExtraInfo();


	/* Update my DPT */
	MyDPT->UpdateDPT(otherDPT, hd->GetprevHop(), CurrentTime);
	CC->setExtraInfo(extraInfo, CurrentTime);


	/* Get the destination nodes that the encountered node is better than me */
	dstNodes = MyDPT->CheckDPs(otherDPT, CurrentTime);


	/* Get packets for those destination nodes */
	for(j = 1; j <= dstNodes[0]; j++)
	{
		pktsToSend = Buf->getPackets(dstNodes[j]);
		
		/* Obtain information in order to perform the scheduling and congestion control methods */
		for(i = 1; i <= pktsToSend[0]; i++)
		{
			schInfo = (struct ExchangedInformation *) malloc(sizeof(struct ExchangedInformation));
			sch->initSchExchangedInfo(schInfo);
			schInfo->senUtil = MyDPT->getDPto(dstNodes[j], CurrentTime);
			schInfo->recUtil = otherDPT[dstNodes[j]];
			sch->addPacket(pktsToSend[i], (void *) schInfo);

			ccInfo = (struct ImportInformation *) malloc(sizeof(struct ImportInformation));
			CC->initCCImportInfo(ccInfo);
			ccInfo->pktID = pktsToSend[i];
			ccInfo->senUtil = MyDPT->getDPto(dstNodes[j], CurrentTime);
			ccInfo->recUtil = otherDPT[dstNodes[j]];
			ccInfo->currTime = CurrentTime;
			CC->addPacketInfo(ccInfo);
		}

		free(pktsToSend);
	}

	//Apply scheduling
	outgoing = sch->getOutgoingPackets();

	#ifdef CC_DEBUG
		printf("\n@%f: Node %d takes the following congestion control decisions for the contact with node %d:\n", CurrentTime, this->NodeID, hd->GetprevHop());
	#endif

	//Apply congestion control
	outgoing = CC->filterPackets(outgoing);

	if(outgoing)
	{
		for(i = 1; i <= outgoing[0]; i++)
		{
			SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(), 1);
			Buf->removePkt(outgoing[i]);
		}

		free(outgoing);
	}

	free(dstNodes);

	pktPool->ErasePacket(PID);

	return;
}


void Prophet::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	#ifdef PROPHET_DEBUG
		printf("%f: Node %d received new data packet with ID %d from %d\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
	#endif


	/* Check if I am the packet creator (packet comes from the application layer) */
	if((hd->GetSource() == this->NodeID) && (hd->GetNextHop() == -1))
	{
		/* Update buffer */
		Buf->addPkt(RealID, hd->GetDestination(), hd->GetSource(), CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
		Stat->pktGen(RealID, hd->GetSource(), hd->GetDestination(), CurrentTime);

		return;
	}


	/* Check if I am not the next hop */
	if(hd->GetNextHop() != this->NodeID)
	{
		/* Garbage collection */
		if(pkt->AccessPkt() == false)
		{
			pktPool->ErasePacket(PID);
		}

		return;
	}


	/* Check if I am the destination of the packet */
	if(hd->GetDestination() == this->NodeID)
	{
		#ifdef CC_DEBUG
			printf("\n@%f: Node %d received a delivery packet with ID %d from node %d\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
		#endif

		/* Update statistics */
		Stat->pktRec(hd->GetHops(), (CurrentTime - pkt->GetStartTime()), pkt, pkt->GetStartTime(), false);

		/* Garbage collection */
		if(pkt->AccessPkt() == false)
		{
			pktPool->ErasePacket(PID);
		}

		return;
	}


	/* I am the next hop */
	if(Buf->PacketExists(RealID))
	{
		printf("[Error]: Node %d received a packet with ID %d from node %d that already exists in its buffer\n", this->NodeID, RealID, hd->GetprevHop());
		exit(EXIT_FAILURE);
	}
	else
	{
		/* Update buffer */
		Buf->addPkt(RealID, hd->GetDestination(), hd->GetSource(), CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());


		/* Update statistics */
		Stat->incTimesAsRelayNode(pkt->GetStartTime());

		#ifdef CC_DEBUG
			printf("\n@%f: Node %d received a relay packet with ID %d from node %d, so now it carries the following packets:\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
			Buf->printBufferContents();
		#endif
	}


	/* Garbage collection */
	if(pkt->AccessPkt() == false)
	{
		pktPool->ErasePacket(PID);
	}

	return;
}


void Prophet::SendPacket(double STime, int pktID, int nHop, int RepValue)
{
	int CurrentN;
	Packet *p;
	Packet *newPkt;


	if((p = pktPool->GetPacket(pktID)) == NULL)
	{
		printf("Error: Packet %d doesn't exist in Packet Pool!\nAborting...\n", pktID);
		exit(1);
	}


	/* Duplicate the packet */
	newPkt = p->Duplicate(Buf->GetHops(pktID));
	newPkt->getHeader()->SetNextHop(nHop);
	newPkt->getHeader()->SetprevHop(this->NodeID);
	newPkt->getHeader()->SetRep(RepValue);
	pktPool->AddPacket(newPkt);


	/* Inform the current neighbors about the new packet */
	CurrentN = Mlayer->BroadcastPkt(STime, this->NodeID, newPkt->getSize(), newPkt->getID());


	/* Garbage collection */
	if(CurrentN > 0)
	{
		/* Set access attribute to safely delete the packet later */
		newPkt->SetRecipients(CurrentN);


		/* Update statistics */
		if(newPkt->getHeader()->GetDestination() == nHop)
		{
			Stat->incHandovers(Buf->GetPktCreationTime(pktID));
		}

		Stat->incForwards(pktID, Buf->GetPktCreationTime(pktID));
	}
	else
	{
		/* Cancel broadcast and delete packet (there are no neighbors) */
		pktPool->ErasePacket(newPkt->getID());
	}

	return;
}
