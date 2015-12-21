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


#ifndef SIMBETTS_H
	#define SIMBETTS_H
	#include "SimBetTS.h"
#endif

//#define SIMBETTS_DEBUG


SimBetTS::SimBetTS(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St, Settings *S, God *G): Routing(PP, mc, Bf, NID, St, S, G)
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

SimBetTS::~SimBetTS()
{
	delete Adja;

	return;
}

void SimBetTS::NewContact(double CTime, int NID)
{
	Contact(CTime, NID);

	return;
}


void SimBetTS::ContactRemoved(double CTime, int NID)
{
	Adja->ContactEnd(NID, CTime);
	CC->contactDown(CTime, NID);

	return;
}


void SimBetTS::Contact(double CTime, int NID)
{
	int *Information;


	#ifdef CC_DEBUG
		printf("\n@%f: Node %d encountered node %d, while carrying the following packets:\n", CTime, this->NodeID, NID);
		Buf->printBufferContents();
	#endif


	/* Update the ego network */
	Adja->Update();
	Adja->SetConnection(this->NodeID, NID, CTime);
	Adja->ContactStart(NID, CTime);

	#ifdef SIMBETTS_DEBUG
		printf("%d: My adjacency matrix\n", this->NodeID);
		printf("-------------\n");
		Adja->PrintAdjacency();
	#endif

	CC->contactUp(CTime, NID);

	//Get information about known delivered packet
	Information = DM->GetInfo();
	if(Information != NULL)
	{
		//Create a vaccine information packet
		SendVaccine(CTime, NID, Information);
	}
	else
	{
		//Clean buffer using Deletion method (Delivered pkts)
		DM->CleanBuffer(this->Buf);
		if(DM->ExchangeDirectSummary())
		{
			SendDirectSummary(CTime, NID);
		}
		else
		{
			SendDirectPackets(CTime, NID);
		}
	}

	return;
}


void SimBetTS::AfterDirectTransfers(double CTime, int NID)
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

	#ifdef SIMBETTS_DEBUG
		printf("%d: Sending a request packet to node %d\n", this->NodeID, NID);
	#endif

	return;
}


void SimBetTS::recv(double rTime, int pktID)
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
		case DIRECT_SUMMARY_PACKET:
		{
			ReceptionDirectSummary(h, p, pktID, rTime);
			break;
		}
		case DIRECT_REQUEST_PACKET:
		{
			ReceptionDirectRequest(h, p, pktID, rTime);
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
		case ANTIPACKET:
		{
			ReceptionAntipacket(h, p, pktID, rTime);
			break;
		}
		case ANTIPACKET_RESPONSE:
		{
			ReceptionAntipacketResponse(h, p, pktID, rTime);
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
			printf("Error: Unknown packet type! SimBetTS is not using this type of packet\nAborting...\n");
			exit(1);
		}
	}
	return;
}


void SimBetTS::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	#ifdef SIMBETTS_DEBUG
		printf("%f: Node %d received new data packet with ID %d from %d\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
	#endif


	/* Check if I am the packet creator (packet comes from the application layer) */
	if((hd->GetSource() == this->NodeID) && (hd->GetNextHop() == -1))
	{
		/* Update buffer */
		Buf->addPkt(RealID, hd->GetDestination(), hd->GetSource(), CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
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
		#ifdef CC_DEBUG
			printf("\n@%f: Node %d received a delivery packet with ID %d from node %d\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
		#endif

		/* Update deletion mechanism */
		if(DM->NoDuplicatesSupport() && DM->isDelivered(RealID))
		{
			printf("Problem: Packet %d has been already delivered!\n",RealID);
			exit(1);
		}
		DM->setAsDelivered(RealID);

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
		(Buf->getPacketData(RealID))->IncReplicas(hd->GetRep());
		Stat->incTimesAsRelayNode(pkt->GetStartTime());
	}
	else
	{
		Buf->addPkt(RealID, hd->GetDestination(), hd->GetSource(), CurrentTime, hd->GetHops(), hd->GetprevHop(), pkt->GetStartTime());
		(Buf->getPacketData(RealID))->SetReplicas(hd->GetRep());
		Stat->incTimesAsRelayNode(pkt->GetStartTime());
	}


	#ifdef CC_DEBUG
		printf("\n@%f: Node %d received a relay packet with ID %d from node %d, so now it carries the following packets:\n", CurrentTime, this->NodeID, RealID, hd->GetprevHop());
		Buf->printBufferContents();
	#endif


	/* Garbage collection */
	if(pkt->AccessPkt() == false)
	{
		pktPool->ErasePacket(PID);
	}

	return;
}


void SimBetTS::ReceptionRequestContacts(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	int *con;
	Packet *responsePacket;
	Header *responseHeader;


	#ifdef SIMBETTS_DEBUG
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

	#ifdef SIMBETTS_DEBUG
		printf("%f: Node %d send its contacts with ID %d to node %d\n", CurrentTime, this->NodeID, responsePacket->getID(), hd->GetprevHop());
	#endif
		
	/* Delete the request packet to free memory */
	pktPool->ErasePacket(PID);
	
	return;
}


void SimBetTS::ReceptionContacts(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	int i;
	int pos;
	int *info;
	int *DestInfo;
	struct DestSim *DList;
	Packet *Summary;
	Header *HSum;


	#ifdef SIMBETTS_DEBUG
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
	//DestInfo = Buf->getAllDestinations();
	DestInfo = Buf->getAllDestExcCrntEnc(hd->GetprevHop());
	
	DList = (struct DestSim *) malloc(sizeof(struct DestSim) * DestInfo[0]);
	pos = 0;
	for(i = 1; i <= DestInfo[0]; i++)
	{
		DList[pos].Dest = DestInfo[i];
		DList[pos].Sim = Adja->getSim(DestInfo[i]);
		DList[pos].TieStrength = Adja->CalculateTieStrength(DestInfo[i], CurrentTime);
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

	#ifdef SIMBETTS_DEBUG
		printf("%f: Node %d sent a summary packet with ID %d to node %d\n", CurrentTime, this->NodeID, Summary->getID(), hd->GetprevHop());
	#endif
		
	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);


	return;
}


void SimBetTS::ReceptionMetricsPerDest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	int i;
	int ReqPos;
	int *Req;
	int *OwnedPackets;
	double mySim;
	double myBet;
	double myTS;
	double mySimUtil;
	double myBetUtil;
	double myTSUtil;
	double mySimBetTSUtil;
	double otherSim;
	double otherBet;
	double otherTS;
	double otherSimUtil;
	double otherBetUtil;
	double otherTSUtil;
	double otherSimBetTSUtil;
	double *mySimBetTSUtils;
	double *otherSimBetTSUtils;
	struct DestSim *Dst;
	Destinations *Dpkt;
	Packet *ReqDest;
	Header *HReq;


	#ifdef SIMBETTS_DEBUG
		printf("%f: Node %d received a summary packet with ID %d from node %d\n", CurrentTime, this->NodeID, pkt->getID(), hd->GetprevHop());
	#endif


	/* Initializations */
	Req = (int *) malloc(sizeof(int));
	Req[0] = 0;
	ReqPos = 0;
	Dpkt = (Destinations *) pkt;
	Dst = (struct DestSim *) pkt->getContents();
	mySimBetTSUtils = (double *) malloc(sizeof(double));
	otherSimBetTSUtils = (double *) malloc(sizeof(double));
	mySimBetTSUtils[0] = 0.0;
	otherSimBetTSUtils[0] = 0.0;


	#ifdef CC_DEBUG
		printf("\n@%f: Node %d received a summary packet from node %d, in order to request packets based on the following utility values:\n", CurrentTime, this->NodeID, hd->GetprevHop());
		printf(">> Node %d's Betweenness = %f\tNode %d's Betweenness = %f\n", this->NodeID, Adja->getBet(), hd->GetprevHop(), Dpkt->getBet());
		for(i = 0; i < Dpkt->getDest(); i++)
		{
			printf(">> Node %d's Similarity for node %d = %f\tNode %d's Similarity for node %d = %f\n", this->NodeID, Dst[i].Dest, Adja->getSim(Dst[i].Dest), hd->GetprevHop(), Dst[i].Dest, Dst[i].Sim);
			printf(">> Node %d's Tie Strength for node %d = %f\tNode %d's Tie Strength for node %d = %f\n", this->NodeID, Dst[i].Dest, Adja->CalculateTieStrength(Dst[i].Dest, CurrentTime), hd->GetprevHop(), Dst[i].Dest, Dst[i].TieStrength);
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


		/* Calculate the tie strength utility values */
		myTS = Adja->CalculateTieStrength(Dst[i].Dest, CurrentTime);
		otherTS = Dst[i].TieStrength;
		if(myTS + otherTS > 0)
		{
			myTSUtil = myTS / (myTS + otherTS);
			otherTSUtil = otherTS / (myTS + otherTS);
		}
		else
		{
			myTSUtil = 0.0;
			otherTSUtil = 0.0;
		}


		/* Calculate the SimBetTS utility values */
		mySimBetTSUtil = mySimUtil + myBetUtil + myTSUtil;
		otherSimBetTSUtil = otherSimUtil + otherBetUtil + otherTSUtil;


		/* Compare the SimBetTS utility values */
		if(mySimBetTSUtil - otherSimBetTSUtil > DBL_EPSILON)
		{
			//insert this destination to packet request vector
			ReqPos++;

			Req = (int *) realloc(Req, (ReqPos + 1) * sizeof(int));
			Req[ReqPos] = Dst[i].Dest;
			Req[0] = ReqPos;

			mySimBetTSUtils = (double *) realloc(mySimBetTSUtils, (ReqPos + 1) * sizeof(double));
			otherSimBetTSUtils = (double *) realloc(otherSimBetTSUtils, (ReqPos + 1) * sizeof(double));
			mySimBetTSUtils[ReqPos] = mySimBetTSUtil;
			otherSimBetTSUtils[ReqPos] = otherSimBetTSUtil;
			mySimBetTSUtils[0] = ReqPos;
			otherSimBetTSUtils[0] = ReqPos;
		}
	}
	
	/* Create a response packet */
	ReqDest = new ReqDestinations(CurrentTime, 0);
	ReqDest->setContents((void *) Req);
	ReqDest->setExtraInfo(CC->getExtraInfo(CurrentTime));
	((ReqDestinations *) ReqDest)->setSenderUtils(otherSimBetTSUtils);
	((ReqDestinations *) ReqDest)->setReceiverUtils(mySimBetTSUtils);
	OwnedPackets = Buf->getPackets(Req);
	((ReqDestinations *)ReqDest)->SetOwned(OwnedPackets);
	HReq = new BasicHeader(this->NodeID, hd->GetprevHop());
	ReqDest->setHeader(HReq);
	pktPool->AddPacket(ReqDest);

	/* Send the packet with the response */
	Mlayer->SendPkt(CurrentTime, this->NodeID, hd->GetprevHop(), ReqDest->getSize(), ReqDest->getID());

	#ifdef SIMBETTS_DEBUG
		printf("%f: Node %d sent a request packet containing destinations with ID %d to %d\n", CurrentTime, this->NodeID, ReqDest->getID(), hd->GetprevHop());
	#endif
	/* Delete the packet to free memory */
	pktPool->ErasePacket(PID);
	
	return;
}


void SimBetTS::ReceptionReqForDest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	int i;
	int j;
	int pos;
	int myReplicas;
	int otherReplicas;
	int *dstRequests;
	int *AlreadyOwned;
	int *pktsToSend;
	int *outgoing;
	double *mySimBetTSUtils;
	double *otherSimBetTSUtils;
	void *extraInfo;
	PacketEntry *pData;
	struct ExchangedInformation *schInfo;
	struct ImportInformation *ccInfo;


	#ifdef SIMBETTS_DEBUG
		printf("%f: Node %d received a request containing destinations with ID %d to node %d\n", CurrentTime, this->NodeID, pkt->getID(), hd->GetprevHop());
	#endif

	/* Get packet contents and SimBetTS utility values */
	dstRequests = (int *) pkt->getContents();
	AlreadyOwned = ((ReqDestinations *) pkt)->GetOwned();
	mySimBetTSUtils = ((ReqDestinations *) pkt)->getSenderUtils();
	otherSimBetTSUtils = ((ReqDestinations *) pkt)->getReceiverUtils();
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
			schInfo->senUtil = mySimBetTSUtils[j];
			schInfo->recUtil = otherSimBetTSUtils[j];
			sch->addPacket(pktsToSend[i], (void *) schInfo);

			ccInfo = (struct ImportInformation *) malloc(sizeof(struct ImportInformation));
			CC->initCCImportInfo(ccInfo);
			ccInfo->pktID = pktsToSend[i];
			ccInfo->senUtil = mySimBetTSUtils[j];
			ccInfo->recUtil = otherSimBetTSUtils[j];
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
			if(PacketExists(AlreadyOwned, outgoing[i]))
			{
				continue;
			}

			pData = Buf->getPacketData(outgoing[i]);

			if(pData->GetReplicas() == 1)
			{
				SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(), 1);
				Buf->removePkt(outgoing[i]);
			}
			else
			{
				/* Find the SimBetTS utilities */
				pos = -1;
				for(j = 1; j <= dstRequests[0]; j++)
				{
					if(dstRequests[j] == Buf->GetPktDestination(outgoing[i]))
					{
						pos = j;
						break;
					}
				}

				if(pos == -1)
				{
					printf("Error: The destination node (%d) of packet %d is not in the list of packets to be forwarded to node %d\nAborting...\n", Buf->GetPktDestination(outgoing[i]), outgoing[i], hd->GetprevHop());
					exit(1);
				}


				/* Divide the number of replicas between the two nodes based on their SimBetTS utilities */
				myReplicas = (int) (((double) pData->GetReplicas()) * (mySimBetTSUtils[pos] / (mySimBetTSUtils[pos] + otherSimBetTSUtils[pos])));
				otherReplicas = pData->GetReplicas() - myReplicas;


				#ifdef SIMBETTS_DEBUG
					printf("Previous Replicas = %d, Sender's Replicas = %d, Receiver's Replicas = %d\n", pData->GetReplicas(), myReplicas, otherReplicas);
				#endif

				if(otherReplicas > 0)
				{
					if(otherReplicas > 1)
					{
						Stat->incReps(outgoing[i]);
					}

					SendPacket(CurrentTime, outgoing[i], hd->GetprevHop(), otherReplicas);
				}

				if(myReplicas > 0)
				{
					pData->SetReplicas(myReplicas);
				}
				else
				{
					Buf->removePkt(outgoing[i]);
				}
			}
		}

		free(outgoing);
	}

	pktPool->ErasePacket(PID);

	return;
}


bool SimBetTS::PacketExists(int* PList, int p)
{
	int i;


	for(i = 1; i <= PList[0]; i++)
	{
		if(PList[i] == p)
		{
			return true;
		}
	}

	return false;
}


void SimBetTS::SendPacket(double STime, int pktID, int nHop, int RepValue)
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
