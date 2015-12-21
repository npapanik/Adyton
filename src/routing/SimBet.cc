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

#ifndef SIMBET_H
	#define SIMBET_H
	#include "SimBet.h"
#endif

//#define SIMBET_DEBUG

// +----------------+
// | Protocol steps |
// +----------------+----------------------------------------------------------+
// | Node (a) encounters node (b)                                              |
// |===========================================================================|
// | <steps related to direct delivery are performed (check Routing.cc)>       |
// |===========================================================================|
// | (a): [method: AfterDirectTransfers()]                                     |
// |      - Sends a request for the contacts of node (b)                       |
// |                                                                           |
// |      (a)------------ contact request packet ----------------->(b)         |
// |                                                                           |
// |===========================================================================|
// | (b): [method: ReceptionRequestContacts()]                                 | 
// |      - Creates a <contact respone packet> that contains the contacts      | 
// |        of node (b).                                                       |
// |                                                                           |
// |      (a)<----------- contact response packet -- ------------- (b)         |
// |                                                                           |
// |===========================================================================|
// | (a): [method: ReceptionContacts()]                                        |
// |      - node (a) updates the contact (ego) graph according to              |
// |        the contacts received.                                             |
// |                                                                           |
// |      - Then, it Creates a <summary packet> that contains the destinations |
// |        for packets which reside inside its buffer. Also, the summary      |
// |        packet includes the SimBet utility for the respective destination. |
// |                                                                           |
// |      (a)-------- summary packet (Dests, SimBet Utils) -------->(b)        |
// |                                                                           |
// |===========================================================================|
// | (b): [method: ReceptionMetricsPerDest()]                                  | 
// |      - For each destination inside the <summary packet> node (b):         |
// |            * Calculates the SimBet utility for that destination.          |
// |      - Creates a <request packet> that contains the destinations for      |
// |        which node (b) has a better SimBet utility than node (a).          |
// |                                                                           |
// | (a)<---------- request packet (Dests, SimBet Utils)------------(b)        |
// |                                                                           |
// |===========================================================================|
// | (a): [method: ReceptionReqForDest()]                                      |
// |      - node (a) gets all packets that are destined to each destination    |
// |        included in the <request packet>. Then, each packet is scheduled   |
// |        for transmission.                                                  |
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


SimBet::SimBet(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St, Settings *S, God *G): Routing(PP, mc, Bf, NID, St, S, G)
{
	double DensityVal;
	string profileAttribute;

	//Check profile
	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("Density")) != "none")
	{
		DensityVal = atof(profileAttribute.c_str());

		if(DensityVal < 0.0 || DensityVal > 1.0)
		{
			printf("Error: Density value must be between 0 and 1. You gave %f\nExiting\n", DensityVal);
			exit(1);
		}

		//Create contact graph powered with contact aggregation
		if((profileAttribute = S->GetProfileAttribute("AggregationType")) == "MR")
		{
			//printf("Contact graph powered with MR contact aggregation\n");
			this->Adja = new Adjacency(NID, Set->getNN(), 2, DensityVal);
		}
		else
		{
			//set as default type
			//printf("Contact graph powered with MF contact aggregation\n");
			this->Adja = new Adjacency(NID, Set->getNN(), 1, DensityVal);
		}
	}
	else 
	{
		this->Adja = new Adjacency(NID,Set->getNN());
	}

	return;
}


SimBet::~SimBet()
{
	delete Adja;

	return;
}


void SimBet::NewContact(double CTime, int NID)
{
	Contact(CTime, NID);

	return;
}


void SimBet::Contact(double CTime, int NID)
{
	#ifdef CC_DEBUG
		printf("\n@%f: Node %d encountered node %d, while carrying the following packets:\n", CTime, this->NodeID, NID);
		Buf->printBufferContents();
	#endif


	/* Update the ego network */
	Adja->Update();
	Adja->SetConnection(this->NodeID, NID, CTime);
	Adja->ContactStart(NID, CTime);

	#ifdef SIMBET_DEBUG
		printf("%d: My adjacency matrix\n", this->NodeID);
		printf("-------------\n");
		Adja->PrintAdjacency();
	#endif

	CC->contactUp(CTime, NID);

	//First send all packets that have NID as destination
	SendDirectPackets(CTime, NID);

	return;
}


void SimBet::ContactRemoved(double CTime, int NID)
{
	Adja->ContactEnd(NID, CTime);
	CC->contactDown(CTime, NID);

	return;
}


void SimBet::recv(double rTime, int pktID)
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
		case CONTACTS_PACKET:
		{
			ReceptionContacts(h, p, pktID, rTime);
			break;
		}
		case DEST_PACKET:
		{
			ReceptionMetricsPerDest(h, p, pktID, rTime);
			break;
		}
		case REQ_DEST_PACKET:
		{
			ReceptionReqForDest(h, p, pktID, rTime);
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
			printf("Error: Unknown packet type! SimBet is not using this type of packet!\nAborting...\n");
			exit(1);
		}
	}

	return;
}


void SimBet::AfterDirectTransfers(double CTime, int NID)
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

	#ifdef SIMBET_DEBUG
		printf("%d: Sending a request packet to node %d\n", this->NodeID, NID);
	#endif

	return;
}


void SimBet::ReceptionRequestContacts(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int *con;
	Packet *responsePacket;
	Header *responseHeader;


	#ifdef SIMBET_DEBUG
		printf("%f: Node %d received a request packet for its contacts with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif
	
	/* Create a response packet containing my contacts */
	con = Adja->GetMyContacts();
	responsePacket = new Contacts(CurrentTime, 0);
	responsePacket->setContents((void *) con);
	responseHeader = new BasicHeader(this->NodeID, hd->GetprevHop());
	responsePacket->setHeader(responseHeader);
	pktPool->AddPacket(responsePacket);


	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), responsePacket->getSize(), responsePacket->getID());

	#ifdef SIMBET_DEBUG
		printf("%f: Node %d send its contacts with ID %d to node %d\n", CurrentTime, this->NodeID, responsePacket->getID(), hd->GetprevHop());
	#endif

	/* Delete the request packet to free memory */
	pktPool->ErasePacket(PID);
	return;
}


void SimBet::ReceptionContacts(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int i;
	int pos;
	int *info;
	int *DestInfo;
	struct DestSim *DList;
	Packet *Summary;
	Header *HSum;


	#ifdef SIMBET_DEBUG
		printf("%f: Node %d received a packet containing the contacts with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif


	/* Get packet contents */
	info = (int *) pkt->getContents();


	/* Update the ego matrix */
	for(i = 1; i <= info[0]; i++)
	{
		Adja->SetConnection(hd->GetprevHop(), info[i], CurrentTime);
	}

	/* Update my metrics */
	Adja->UpdateAll();


	/* Create a summary packet containing destinations and metrics */
	DestInfo = Buf->getAllDestinations();
	DList = (struct DestSim *) malloc(sizeof(struct DestSim) * DestInfo[0]);
	pos = 0;
	for(i = 1; i <= DestInfo[0]; i++)
	{
		DList[pos].Dest = DestInfo[i];
		DList[pos].Sim = Adja->getSim(DestInfo[i]);
		pos++;
	}
	Summary = new Destinations(CurrentTime, 0);
	Summary->setContents((void *) DList);
	((Destinations *) Summary)->setDest(DestInfo[0]);
	((Destinations *) Summary)->setBet(Adja->getBet());
	free(DestInfo);
	HSum = new BasicHeader(this->NodeID, hd->GetprevHop());
	Summary->setHeader(HSum);
	pktPool->AddPacket(Summary);


	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), Summary->getSize(), Summary->getID());

	#ifdef SIMBET_DEBUG
		printf("%f: Node %d sent a summary packet with ID %d to node %d\n", CurrentTime, this->NodeID, Summary->getID(), hd->GetprevHop());
	#endif
	
	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);
	return;
}


void SimBet::ReceptionMetricsPerDest(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int i;
	int ReqPos;
	int *Req;
	double mySim;
	double myBet;
	double mySimUtil;
	double myBetUtil;
	double mySimBetUtil;
	double otherSim;
	double otherBet;
	double otherSimUtil;
	double otherBetUtil;
	double otherSimBetUtil;
	double *mySimBetUtils;
	double *otherSimBetUtils;
	struct DestSim *Dst;
	Destinations *Dpkt;
	Packet *ReqDest;
	Header *HReq;


	#ifdef SIMBET_DEBUG
		printf("%f: Node %d received a summary packet with ID %d from node %d\n", CurrentTime, this->NodeID, pkt->getID(), hd->GetprevHop());
	#endif


	/* Initializations */
	Req = (int *) malloc(sizeof(int));
	Req[0] = 0;
	ReqPos = 0;
	Dpkt = (Destinations *) pkt;
	Dst = (struct DestSim *) pkt->getContents();
	mySimBetUtils = (double *) malloc(sizeof(double));
	otherSimBetUtils = (double *) malloc(sizeof(double));
	mySimBetUtils[0] = 0.0;
	otherSimBetUtils[0] = 0.0;


	#ifdef CC_DEBUG
		printf("\n@%f: Node %d received a summary packet from node %d, in order to request packets based on the following utility values:\n", CurrentTime, this->NodeID, hd->GetprevHop());
		printf(">> Node %d's Betweenness = %f\tNode %d's Betweenness = %f\n", this->NodeID, Adja->getBet(), hd->GetprevHop(), Dpkt->getBet());
		for(i = 0; i < Dpkt->getDest(); i++)
		{
			printf(">> Node %d's Similarity for node %d = %f\tNode %d's Similarity for node %d = %f\n", this->NodeID, Dst[i].Dest, Adja->getSim(Dst[i].Dest), hd->GetprevHop(), Dst[i].Dest, Dst[i].Sim);
		}
	#endif


	/* Calculate the betweenness utility values */
	myBet = Adja->getBet();
	otherBet = Dpkt->getBet();
	if(myBet + otherBet > 0)
	{
		myBetUtil = myBet / (myBet + otherBet);
		otherBetUtil = otherBet / (myBet + otherBet);
	}
	else
	{
		myBetUtil = 0.0;
		otherBetUtil = 0.0;
	}


	for(i = 0; i < Dpkt->getDest(); i++)
	{
		/* Calculate the similarity utility values */
		mySim = Adja->getSim(Dst[i].Dest);
		otherSim = Dst[i].Sim;
		if(mySim + otherSim > 0)
		{
			mySimUtil = mySim / (mySim + otherSim);
			otherSimUtil = otherSim / (mySim + otherSim);
		}
		else
		{
			mySimUtil = 0.0;
			otherSimUtil = 0.0;
		}

		/* Calculate the SimBet utility values */
		mySimBetUtil = (0.5 * mySimUtil) + (0.5 * myBetUtil);
		otherSimBetUtil = (0.5 * otherSimUtil) + (0.5 * otherBetUtil);


		#ifdef CC_DEBUG
			printf(">> Node %d's SimBetUtil for node %d = %f\tNode %d's SimBetUtil for node %d = %f\n", this->NodeID, Dst[i].Dest, mySimBetUtil, hd->GetprevHop(), Dst[i].Dest, otherSimBetUtil);
		#endif


		/* Compare the SimBet utility values */
		if(mySimBetUtil - otherSimBetUtil > DBL_EPSILON)
		{
			//insert this destination to packet request vector
			ReqPos++;

			Req = (int *) realloc(Req, (ReqPos + 1) * sizeof(int));
			Req[ReqPos] = Dst[i].Dest;
			Req[0] = ReqPos;

			mySimBetUtils = (double *) realloc(mySimBetUtils, (ReqPos + 1) * sizeof(double));
			otherSimBetUtils = (double *) realloc(otherSimBetUtils, (ReqPos + 1) * sizeof(double));
			mySimBetUtils[ReqPos] = mySimBetUtil;
			otherSimBetUtils[ReqPos] = otherSimBetUtil;
			mySimBetUtils[0] = ReqPos;
			otherSimBetUtils[0] = ReqPos;
		}
	}

	/* Create a response packet */
	ReqDest = new ReqDestinations(CurrentTime, 0);
	ReqDest->setContents((void *) Req);
	ReqDest->setExtraInfo(CC->getExtraInfo(CurrentTime));
	((ReqDestinations *) ReqDest)->setSenderUtils(otherSimBetUtils);
	((ReqDestinations *) ReqDest)->setReceiverUtils(mySimBetUtils);
	HReq = new BasicHeader(this->NodeID, hd->GetprevHop());
	ReqDest->setHeader(HReq);
	pktPool->AddPacket(ReqDest);

	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), ReqDest->getSize(), ReqDest->getID());

	#ifdef SIMBET_DEBUG
		printf("%f: Node %d sent a request packet containing destinations with ID %d to %d\n", CurrentTime, this->NodeID, ReqDest->getID(), hd->GetprevHop());
	#endif

	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);
	return;
}


void SimBet::ReceptionReqForDest(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int i;
	int j;
	int *dstRequests;
	int *pktsToSend;
	int *outgoing;
	double *mySimBetUtils;
	double *otherSimBetUtils;
	void *extraInfo;
	struct ExchangedInformation *schInfo;
	struct ImportInformation *ccInfo;


	#ifdef SIMBET_DEBUG
		printf("%f: Node %d received a request containing destinations with ID %d to node %d\n", CurrentTime, this->NodeID, pkt->getID(), hd->GetprevHop());
	#endif

	/* Get packet contents and SimBet utility values */
	dstRequests = (int *) pkt->getContents();
	mySimBetUtils = ((ReqDestinations *) pkt)->getSenderUtils();
	otherSimBetUtils = ((ReqDestinations *) pkt)->getReceiverUtils();
	extraInfo = pkt->getExtraInfo();

	CC->setExtraInfo(extraInfo, CurrentTime);


	/* Get packets for the destinations in the request packet */
	for(j = 1; j <= dstRequests[0]; j++)
	{
		pktsToSend = Buf->getPackets(dstRequests[j]);
		
		/* Obtain information in order to perform the scheduling and congestion control methods */
		for(i = 1; i <= pktsToSend[0]; i++)
		{
			schInfo = (struct ExchangedInformation *) malloc(sizeof(struct ExchangedInformation));
			sch->initSchExchangedInfo(schInfo);
			schInfo->senUtil = mySimBetUtils[j];
			schInfo->recUtil = otherSimBetUtils[j];
			sch->addPacket(pktsToSend[i], (void *) schInfo);

			ccInfo = (struct ImportInformation *) malloc(sizeof(struct ImportInformation));
			CC->initCCImportInfo(ccInfo);
			ccInfo->pktID = pktsToSend[i];
			ccInfo->senUtil = mySimBetUtils[j];
			ccInfo->recUtil = otherSimBetUtils[j];
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

	pktPool->ErasePacket(PID);

	return;
}


void SimBet::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	#ifdef SIMBET_DEBUG
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


void SimBet::SendPacket(double STime, int pktID, int nHop, int RepValue)
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
