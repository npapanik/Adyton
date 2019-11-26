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
 *  Written by Dimitrios-Georgios Akestoridis and Evangelos Papapetrou.
 */


#ifndef BUBBLERAP_H
	#define BUBBLERAP_H
	#include "BubbleRap.h"
#endif

//#define BUBBLERAP_DEBUG


BubbleRap::BubbleRap(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St, Settings *S, God *G) : Routing(PP, mc, Bf, NID, St, S, G)
{
	string profileAttribute;
	InterCopyOn=false;
	IntraCopyOn=false;
	SprayAndFocusIntra = false;
	double familiarthreshold;
	int kappavalue;

	ranking = new CentralityApproximation(NID, Set->getNN());
	labeling = new CommunityDetection(NID, Set->getNN());
	
	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("familiarSetThreshold")) != "none")
	{
		familiarthreshold = atof(profileAttribute.c_str());
		
		if(familiarthreshold <= 0.0)
		{
			printf("Error: Invalid familiarSetThreshold value (%f)\nExiting...", familiarthreshold);
			exit(1);
		}
		
		labeling->setFamiliarSetThreshold(familiarthreshold);
	}

	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("kappa")) != "none")
	{
		kappavalue = atoi(profileAttribute.c_str());
		
		if(kappavalue <= 0)
		{
			printf("Error: Invalid kappa value (%d)\nExiting...", kappavalue);
			exit(1);
		}		
		
		labeling->setKappa(kappavalue);
	}
	
	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("multi-copy")) != "none")
	{
		Set->setCopyMode(false);
		if(profileAttribute == "all")
		{
			IntraCopyOn=true;
			InterCopyOn=true;
		}
		else if(profileAttribute == "inter")
		{
			IntraCopyOn=false;
			InterCopyOn=true;
		}
		else
		{
			printf("Currently two multi-copy modes are supported: (1) all  (2) inter\n");
			exit(1);
		}
	}
	
	if(S->ProfileExists() && (profileAttribute = S->GetProfileAttribute("SprayAndFocus")) != "none")
	{
		if(profileAttribute == "intra")
		{
			SprayAndFocusIntra = true;
		}
		else
		{
			printf("Use 'intra' for confining the spray opretation inside the destinations community.\n");
			exit(1);
		}
	}		
	return;
}

BubbleRap::~BubbleRap()
{
	delete ranking;
	delete labeling;
	return;
}

void BubbleRap::NewContact(double CTime, int NID)
{
	ranking->connectionOccured(CTime, NID);
	labeling->connectionOccured(CTime, NID);

	Contact(CTime, NID);

	return;
}


void BubbleRap::ContactRemoved(double CTime, int NID)
{
	ranking->disconnectionOccured(CTime, NID);
	labeling->disconnectionOccured(CTime, NID);

	return;
}


void BubbleRap::Contact(double CTime, int NID)
{
	//First send all packets that have NID as destination
	//SendDirectPackets(CTime,NID);
	
	//Get information about known delivered packets
	int *Information=DM->GetInfo();
	if(Information != NULL)
	{
		//Create a vaccine information packet
		SendVaccine(CTime,NID,Information);
	}
	else
	{
		//Clean buffer using Deletion method (Delivered pkts)
		DM->CleanBuffer(this->Buf);
		if(DM->ExchangeDirectSummary())
		{
			SendDirectSummary(CTime,NID);
		}
		else
		{
			SendDirectPackets(CTime,NID);
		}
	}
	return;
}

void BubbleRap::AfterDirectTransfers(double CTime, int NID)
{
	Packet *ReqPacket;
	Header *h;
	/* Create a new request packet (Local Community and Familiar Sets request) */
	ReqPacket = new ReqLCandFS(CTime, 0);
	h = new BasicHeader(this->NodeID, NID);
	ReqPacket->setHeader(h);
	pktPool->AddPacket(ReqPacket);
	/* Send the packet to the new contact */
	Mlayer->SendPkt(CTime, this->NodeID, NID, ReqPacket->getSize(), ReqPacket->getID());
	#ifdef BUBBLERAP_DEBUG
		printf("%d: Sending a request packet to node %d for its Familiar Set and its Local Community\n", this->NodeID, NID);
	#endif
}



void BubbleRap::recv(double rTime, int pktID)
{
	int PacketID;
	Packet *p;
	Header *h;


	if((p = pktPool->GetPacket(pktID)) == NULL)
	{
		printf("Error: Packet %d doesn't exist in Packet Pool! Aborting...\n", pktID);
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
		printf("%f: Node %d received new packet with ID %d and type %d from node %d\n", rTime, this->NodeID, p->getID(), p->getType(), h->GetprevHop());
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
		case DIRECT_SUMMARY_PACKET:
		{
			ReceptionDirectSummary(h,p,pktID,rTime);
			break;
		}
		case DIRECT_REQUEST_PACKET:
		{
			ReceptionDirectRequest(h,p,pktID,rTime);
			break;
		}
		case REQUEST_LC_FS:
		{
			ReceptionReqLCandFS(h, p, pktID, rTime);
			break;
		}
		case LC_FS:
		{
			ReceptionLCandFS(h, p, pktID, rTime);
			break;
		}
		case BUBBLE_SUMMARY:
		{
			ReceptionBubbleSummary(h, p, pktID, rTime);
			break;
		}
		case MARKED_REQUEST_PKTS:
		{
			ReceptionRequestVector(h, p, pktID, rTime);
			break;
		}
		case ANTIPACKET:
		{
			ReceptionAntipacket(h,p,pktID,rTime);
			break;
		}
		case ANTIPACKET_RESPONSE:
		{
			ReceptionAntipacketResponse(h,p,pktID,rTime);
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
			printf("Error: Unknown packet type #%d\nBubble Rap is not using this type of packet... Aborting...\n", p->getType());
			exit(1);
		}
	}

	return;
}


void BubbleRap::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d received new data packet with ID %d from node %d\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
	#endif


	/* Check if I am the packet creator (packet comes from the application layer) */
	if((hd->GetSource() == this->NodeID) && (hd->GetNextHop() == -1))
	{
		Buf->addPkt(RealID, hd->GetDestination(),this->NodeID, CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
		(Buf->getPacketData(RealID))->SetReplicas(hd->GetRep());
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
		//The above lines enable checking for duplicates 
		if(DM->NoDuplicatesSupport() && DM->isDelivered(RealID))
		{
			printf("Problem: Packet %d has been already delivered!\n",RealID);
			exit(1);
		}
		DM->setAsDelivered(RealID);
		/* Update statistics */
		Stat->pktRec(hd->GetHops(), CurrentTime - pkt->GetStartTime(), pkt, pkt->GetStartTime(), false);


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
		Buf->addPkt(RealID, hd->GetDestination(),hd->GetSource(),CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
		(Buf->getPacketData(RealID))->SetReplicas(hd->GetRep());

		/* Update statistics */
		Stat->incTimesAsRelayNode(pkt->GetStartTime());
	}


	/* Garbage collection */
	if(pkt->AccessPkt() == false)
	{
		pktPool->ErasePacket(PID);
	}

	return;
}


void BubbleRap::ReceptionReqLCandFS(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	bool *myLocalCommunity;
	bool **myFamiliarSets;
	Packet *responsePacket;
	Header *responseHeader;


	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d received a request packet for its Local Community and its Familiar Sets with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif
		
	/* Create a response packet containing my Local Community and my Familiar Sets */
	myLocalCommunity = labeling->cloneLocalCommunity(CurrentTime);
	myFamiliarSets = labeling->cloneFamiliarSets(CurrentTime);
	responsePacket = new LCandFS(CurrentTime, myLocalCommunity, myFamiliarSets, 0);
	responseHeader = new BasicHeader(this->NodeID, hd->GetprevHop());
	responsePacket->setHeader(responseHeader);
	((LCandFS *)responsePacket)->setmaxN(Set->getNN());
	pktPool->AddPacket(responsePacket);


	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), responsePacket->getSize(), responsePacket->getID());

	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d sent its Local Community and its Familiar Sets with ID %d to node %d\n", CurrentTime, this->NodeID, responsePacket->getID(), hd->GetprevHop());
	#endif
	
	/* Delete the request packet to free memory */
	pktPool->ErasePacket(PID);

	return;
}


void BubbleRap::ReceptionLCandFS(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	int i;
	int *allPackets;
	double myLocalRank;
	double myGlobalRank;
	bool *myLocalCommunity;
	bool *encLocalCommunity;
	bool **encFamiliarSets;
	struct PktDest *mySummaryVector;
	Packet *responsePacket;
	Header *responseHeader;


	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d received a packet containing the Local Community and the Familiar Sets with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif


	/* Get packet contents */
	encLocalCommunity = ((LCandFS *) pkt)->getLocalCommunity();
	encFamiliarSets = ((LCandFS *) pkt)->getFamiliarSets();
	

	/* Update my bubble */
	labeling->updateBubble(encLocalCommunity, encFamiliarSets, hd->GetprevHop(), CurrentTime);


	/* Create my summary vector */
// 	allPackets = Buf->getAllPackets();
	allPackets = Buf->getPacketsNotDestinedTo(hd->GetprevHop());
	mySummaryVector = (struct PktDest *) malloc(sizeof(struct PktDest) * allPackets[0]);
	for(i = 1; i <= allPackets[0]; i++)
	{
		mySummaryVector[i-1].PID = allPackets[i];
		mySummaryVector[i-1].Dest = Buf->GetPktDestination(allPackets[i]);
	}


	/* Get my bubble information */
	myLocalCommunity = labeling->cloneLocalCommunity(CurrentTime);
	myLocalRank = ranking->getLocalRank(CurrentTime, myLocalCommunity);
	myGlobalRank = ranking->getGlobalRank(CurrentTime);
	
	/* Create a response packet containing bubble information and summary vector */
	responsePacket = new BubbleSummary(CurrentTime, allPackets[0], mySummaryVector, myLocalCommunity, myLocalRank, myGlobalRank, 0);
	responseHeader = new BasicHeader(this->NodeID, hd->GetprevHop());
	responsePacket->setHeader(responseHeader);
	pktPool->AddPacket(responsePacket);


	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), responsePacket->getSize(), responsePacket->getID());


	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d sent its bubble information and its summary vector with ID %d to node %d\n", CurrentTime, this->NodeID, responsePacket->getID(), hd->GetprevHop());
	#endif
	
	free(allPackets);
	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);
	return;
}


void BubbleRap::ReceptionBubbleSummary(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int i=0;
	int reqPos=0;
	int encNumPkts=0;
	int *myRequestVector=NULL;
	bool *myRequestMarks=NULL;
	bool encBetterCarrier=true;
	bool IamBetterGlobally=false;
	bool *encLocalCommunity=NULL;
	bool *myLocalCommunity=NULL;
	double encLocalRank=0.0;
	double myLocalRank=0.0;
	double encGlobalRank=0.0;
	double myGlobalRank=0.0;
	struct PktDest *encSummaryVector=NULL;
	Packet *responsePacket=NULL;
	Header *responseHeader=NULL;


	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d received a packet containing bubble information and summary vector with ID %d from node %d\n", CurrentTime, this->NodeID, PID, hd->GetprevHop());
	#endif


	/* Get packet contents */
	encNumPkts = ((BubbleSummary *) pkt)->getNumPackets();
	encSummaryVector = ((BubbleSummary *) pkt)->getSummaryVector();
	encLocalCommunity = ((BubbleSummary *) pkt)->getLocalCommunity();
	encLocalRank = ((BubbleSummary *) pkt)->getLocalRank();
	encGlobalRank = ((BubbleSummary *) pkt)->getGlobalRank();


	/* Get my bubble information */
	myLocalCommunity = labeling->cloneLocalCommunity(CurrentTime);
	myLocalRank = ranking->getLocalRank(CurrentTime, myLocalCommunity);
	myGlobalRank = ranking->getGlobalRank(CurrentTime);


	/* Initialize the request vector */
	myRequestVector = (int *) malloc(sizeof(int));
	myRequestMarks = (bool *)malloc(sizeof(bool));
	myRequestVector[0] = 0;
	myRequestMarks[0]=true;
	reqPos = 0;


	/* Bubble Rap Forwarding */
	for(i = 0; i < encNumPkts; i++)
	{
		if(Buf->PacketExists(encSummaryVector[i].PID))
		{
			continue;
		}

		encBetterCarrier = true;
		IamBetterGlobally = false;

		if(myLocalCommunity[encSummaryVector[i].Dest])
		{
			if((encLocalCommunity[encSummaryVector[i].Dest]) && (encLocalRank >= myLocalRank))
			{
				encBetterCarrier = true;
			}
			else if((encLocalCommunity[encSummaryVector[i].Dest]) && (encLocalRank < myLocalRank))
			{
				encBetterCarrier = false;
			}
			else if(!encLocalCommunity[encSummaryVector[i].Dest])
			{
				encBetterCarrier = false;
			}
		}
		else
		{
			if(encLocalCommunity[encSummaryVector[i].Dest])
			{
				encBetterCarrier = true;
			}
			else if(encGlobalRank >= myGlobalRank)
			{
				encBetterCarrier = true;
			}
			else if(encGlobalRank < myGlobalRank)
			{
				encBetterCarrier = false;
				IamBetterGlobally = true;
			}
		}

		if(!encBetterCarrier)
		{
			/* Add the packet in the request vector */
			reqPos++;

			myRequestVector=(int *) realloc(myRequestVector, (reqPos + 1)*sizeof(int));
			myRequestVector[reqPos] = encSummaryVector[i].PID;
			myRequestVector[0] = reqPos;
			
			myRequestMarks=(bool *) realloc(myRequestMarks, (reqPos + 1)*sizeof(bool));
			myRequestMarks[reqPos] = IamBetterGlobally;
		}
	}
	free(myLocalCommunity);

	/* Create a response packet containing the requested packets */
	responsePacket = new MarkedRequestPacket(CurrentTime, 0);
	responsePacket->setContents((void *) myRequestVector);
	((MarkedRequestPacket *)responsePacket)->setMarked(myRequestMarks);
	responseHeader = new BasicHeader(this->NodeID, hd->GetprevHop());
	responsePacket->setHeader(responseHeader);
	pktPool->AddPacket(responsePacket);

	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), responsePacket->getSize(), responsePacket->getID());


	#ifdef BUBBLERAP_DEBUG
		printf("%f: Node %d sent its request for packets with ID %d to node %d\n", CurrentTime, this->NodeID, responsePacket->getID(), hd->GetprevHop());
	#endif
	
	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);
	return;
}


bool BubbleRap::winsGlobally(int PID, int* Requests, bool* mark)
{
	for(int i = 1; i <= Requests[0]; i++)
	{
		if(Requests[i] == PID)
		{
			return mark[i];
		}
	}
	//We should not reach this point
	printf("(BubbleRap error) %d: The packet %d never requested by node in contact!Exiting..\n",this->NodeID,PID);
	exit(1);
}


void BubbleRap::ReceptionRequestVector(Header *hd, Packet *pkt, int PID, double CurrentTime)
{
	int i=0;
	int *encRequestVector=NULL;
	bool *encBetterGlobally=NULL;

	int Rcurrent=0;
	int Rnew=0;
	double HowMany=0.0;

	/* Send the packets in the request vector */
	encRequestVector = (int *) pkt->getContents();
	encBetterGlobally = ((MarkedRequestPacket *)pkt)->getMarked();
	for(i = 1; i <= encRequestVector[0]; i++)
	{
		sch->addPacket(encRequestVector[i],NULL);
	}
	//Apply scheduling
	int *outgoing=sch->getOutgoingPackets();
	//Apply congestion control
	outgoing=CC->filterPackets(outgoing);
	//Send packets
	if(outgoing)
	{
		for(int i=1;i<=outgoing[0];i++)
		{
			
			if(Set->getCopyMode()){//The single-copy case
				SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(),1);
				Buf->removePkt(outgoing[i]);
			} else {//The multi-copy case
				if (Set->getReplicas() > 1){//Multi-copy with spray-based dissemination (activated when choosing -REP k, k>1 + PROFILE (multi-copy)
					
					Rcurrent=(Buf->getPacketData(outgoing[i]))->GetReplicas();
					HowMany=((double)Rcurrent)/2.0;
					Rnew=ceil(HowMany);
					
					if (SprayAndFocusIntra && (Rcurrent == 1) && winsGlobally(outgoing[i],encRequestVector,encBetterGlobally)) Rnew = 0;
					
					if (!IntraCopyOn && InterCopyOn && !winsGlobally(outgoing[i],encRequestVector,encBetterGlobally)) Rnew = 1;
					
					if (Rnew > 0) {
						SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(),Rnew);
						if (Rnew > 1) Stat->incReps(outgoing[i]);
						if (Rcurrent - Rnew > 0){
							(Buf->getPacketData(outgoing[i]))->SetReplicas(Rcurrent-Rnew);
						} else {
							Buf->removePkt(outgoing[i]);
						}
					}
					
				} else {//Multi-copy with dynamic replication (activated when -REP 1 + PROFILE (multicopy)
					SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(),1);
					if(!IntraCopyOn && InterCopyOn)
					{
						if(!winsGlobally(outgoing[i],encRequestVector,encBetterGlobally))
						{
							Buf->removePkt(outgoing[i]);
						}
					}
				}
			}
		}
		free(outgoing);
	}
	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);

	return;
}


void BubbleRap::SendPacket(double STime, int pktID,int nHop,int RepValue)
{
	int CurrentN;
	Packet *p;
	Packet *newPkt;


	if((p = pktPool->GetPacket(pktID)) == NULL)
	{
		printf("Error: Packet %d doesn't exist in Packet Pool! Aborting...\n", pktID);
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
