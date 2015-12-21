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


#ifndef LSFSPRAY_H
	#define LSFSPRAY_H
	#include "LSFSpray.h"
#endif

// #define LSFSPRAY_DEBUG

// +----------------+
// | Protocol steps |
// +----------------+----------------------------------------------------------+
// | Node (a) encounters node (b)                                              |
// |===========================================================================|
// | <steps related to direct delivery are performed (check Routing.cc)>       |
// |===========================================================================|
// |  (a): [method: AfterDirectTransfers()]                                    |
// |       - Creates a <summary packet> that contains a list of destinations   |
// |         for which node a has at least one packet for. Also, the LTS       |
// |         utility for each destination is piggybacked to                    |
// |         this <summary packet>.                                            |
// |  (a)-----------summary packet (Destinations,LTS Utils) -------->(b)       |
// |===========================================================================|
// |  (b): [method: ReceptionUtilDest()]                                       |
// |       - Checks for which destinations it has better LTS utility than      |
// |          node (a).                                                        |
// |       - Creates a <request packet> that contains the destination IDs      |
// |         for which it is more capable than node (a).                       |
// |  (a)<------------request packet (destination IDs) --------------(b)       |
// |===========================================================================|
// |  (a): [method: ReceptionAvailDest()]                                      |
// |       - Receives the <request packet>.                                    |
// |       - Gets a list of packets that their destination is inside           |
// |         the <request packet>.                                             |
// |       - Creates a <summary packet> that contains the packet IDs of        |
// |         the packets that their destination is inside the <request packet>.|
// |  (a)--------------summary packet (packet IDs)------------------>(b)       |
// |===========================================================================|
// |  (b): [method:ReceptionSummary()]                                         |
// |       - Checks which packets are missing from its buffer.                 |
// |       - Creates a request with packet IDs that are not inside its buffer. |
// |  (a)<--------------request packet (packet IDs)------------------(b)       |
// |===========================================================================|
// |  (a): [method: ReceptionRequest()]                                        |
// |       - Gets the requested packets from its buffer.                       |
// |       - A replica for all packets that have replication value larger      |
// |         than 1 is created (replication values change as needed).          |
// |       - Packet replicas are transmitted.                                  |
// |  (a)--------------packet replica 1 destined to b--------------->(b)       |
// |  (a)--------------packet replica 2 destined to b--------------->(b)       |
// |                               ...                                         |
// |  (a)--------------packet replica n destined to b--------------->(b)       |
// |===========================================================================|
// |  (b): [method: ReceptionData()]                                           |
// |       - Receives the transmitted packets one by one.                      |
// +---------------------------------------------------------------------------+


LSFSpray::LSFSpray(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St,Settings *S,God *G): Routing(PP, mc, Bf, NID, St,S,G)
{
	LSFUtil=new LTS(NID,Set->getNN());
	return;
}


LSFSpray::~LSFSpray()
{
	delete LSFUtil;
	return;
}


void LSFSpray::NewContact(double CTime, int NID)
{
	Contact(CTime,NID);
	return;
}


void LSFSpray::Contact(double CTime, int NID)
{
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


void LSFSpray::ContactRemoved(double CTime,int NID)
{
	LSFUtil->ContactDown(NID,CTime);
	return;
}


void LSFSpray::AfterDirectTransfers(double CTime, int NID)
{
	//Create a packet containing destinations and metrics (summary packet)
	//int *DestInfo=Buf->getAllDestinations();
	int *DestInfo=Buf->getAllDestExcCrntEnc(NID);
	struct DestUtil *DList=(struct DestUtil *)malloc(sizeof(struct DestUtil)*DestInfo[0]);
	int pos=0;
	for(int i=1;i<=DestInfo[0];i++)
	{
		DList[pos].Dest=DestInfo[i];
		DList[pos].Util=LSFUtil->get(DestInfo[i],CTime);
		pos++;
	}
	Packet *OtherSummary=new DstUtils(CTime,0);
	OtherSummary->setContents((void *)DList);
	((DstUtils *)OtherSummary)->setDestNum(DestInfo[0]);
	//free memory
	free(DestInfo);
	Header *HSum=new BasicHeader(this->NodeID,NID);
	OtherSummary->setHeader(HSum);
	//Add packet to the packet pool
	pktPool->AddPacket(OtherSummary);
	//Send packet
	Mlayer->SendPkt(CTime,this->NodeID,NID,OtherSummary->getSize(),OtherSummary->getID());
	#ifdef LSFSPRAY_DEBUG
	printf("%f:Node %d send a summary packet with ID:%d to %d\n",CTime,this->NodeID,Summary->getID(),NID);
	printf("Summary contents(%d):\n",((DstUtils *)OtherSummary)->getDestNum());
	((DstUtils *)OtherSummary)->PrintContents();
	printf("\n");
	#endif
	return;
}


void LSFSpray::recv(double rTime, int pktID)
{
	int PacketID=0;
	//Check if packet is a duplicate
	Packet *p=pktPool->GetPacket(pktID);
	if(p == NULL)
	{//packet isn't found in the packet pool
		printf("Error: Packet %d doesn't exist in Packet Pool!Aborting..\n",pktID);
		exit(1);
	}
	Header *h=p->getHeader();
	if(h->IsDuplicate() == true)
	{
		PacketID=h->GetOriginal();
	}
	else
	{
 		PacketID=pktID;
	}
	//Sanity check
	if(rTime < 0 || p->GetStartTime() < 0)
	{
		printf("%f:Node %d received new packet with ID:%d and type %d from %d\n",rTime,this->NodeID,p->getID(),p->getType(),h->GetprevHop());
		exit(1);
	}
	//Recognize packet type
	switch(p->getType())
	{
		case DATA_PACKET:
		{
			ReceptionData(h,p,pktID,rTime,PacketID);
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
		case UTIL_DEST_PACKET:
		{
			ReceptionUtilDest(h,p,pktID,rTime);
			break;
		}
		case AVAIL_DEST_PACKET:
		{
			ReceptionAvailDest(h,p,pktID,rTime);
			break;
		}
		case SUMMARY_PACKET:
		{
			ReceptionSummary(h,p,pktID,rTime);
			break;
		}
		case REQUEST_PACKET:
		{	
			ReceptionRequest(h,p,pktID,rTime);
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
			printf("Error: Unknown packet type!LSFSpray Delivery(RT) is not using this type of packet..Aborting...\n");
			exit(1);
		}
	}
	return;
}


void LSFSpray::ReceptionUtilDest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Summary packet (containing destinations and utils)
	//Delete packet to free memory
	#ifdef LSFSPRAY_DEBUG
	printf("%f:Node %d received a summary packet with ID:%d from %d\n",CurrentTime,this->NodeID,pkt->getID(),hd->GetprevHop());
	printf("Summary contents(%d):\n",((DstUtils *)pkt)->getDestNum());
	((DstUtils *)p)->PrintContents();
	printf("\n");
	#endif
	
	double MyUtil=0.0;
	int *Req=(int *)malloc(sizeof(int));
	Req[0]=0;
	int ReqPos=0;
	DstUtils *Dpkt=(DstUtils *)pkt;
	double OtherUtil=0.0;
	struct DestUtil *Dst=(struct DestUtil *)pkt->getContents();
	//printf("%d:Give me packets for dests: ", this->NodeID);
	for(int i=0;i<(Dpkt->getDestNum());i++)
	{
		//Calculate my Util
		MyUtil=LSFUtil->get(Dst[i].Dest,CurrentTime);
		//Calculate metrics for other node
		OtherUtil=Dst[i].Util;
		if(MyUtil > OtherUtil)
		//if(MyUtil-OtherUtil > comparison_precision)
		{
			//printf("%d ",Dst[i].Dest);
			//insert this destination to packet request vector
			ReqPos++;
			Req=(int *)realloc(Req,(ReqPos+1)*sizeof(int));
			Req[ReqPos]=Dst[i].Dest;
			Req[0]=ReqPos;
		}
	}
	//printf("\n");
	//prepare response packet
	Packet *AvailDest=new DstAvail(CurrentTime,0);
	AvailDest->setContents((void *)Req);
	Header *HReq=new BasicHeader(this->NodeID,hd->GetprevHop());
	AvailDest->setHeader(HReq);
	//Add packet to the packet pool
	pktPool->AddPacket(AvailDest);
	//Send packet
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),AvailDest->getSize(),AvailDest->getID());
	#ifdef LSFSPRAY_DEBUG
	printf("%f:Node %d send a packet with ID:%d indicating that is available (better utility) for specific destinations to %d\n",CurrentTime,this->NodeID,AvailDest->getID(),hd->GetprevHop());
	printf("Destinations are ...(%d):\n",Req[0]);
	for(int i=1;i<=Req[0];i++)
	{
		printf("%d) ",Req[i]);
	}
	printf("\n");
	#endif
	//free memory
	pktPool->ErasePacket(PID);
	return;
}


void LSFSpray::ReceptionAvailDest(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Received packet containing available destinations
	int *Requests=(int *)pkt->getContents();
	//Get packets for destinations contained in the packet
	int *pktsToSend=Buf->getPackets(Requests);
	//Create new summary packet
	Packet *SumPacket=new SummaryPacket(CurrentTime,0);
	SumPacket->setContents((void *)pktsToSend);
	Header *newhd=new BasicHeader(this->NodeID,hd->GetprevHop());
	SumPacket->setHeader(newhd);
	//Add packet to the packet pool
	pktPool->AddPacket(SumPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),SumPacket->getSize(),SumPacket->getID());
	#ifdef SPRAYWAIT_DEBUG
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


void LSFSpray::ReceptionSummary(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Summary packet
	int *req=(int*)malloc(sizeof(int));
	int unKnown=0;
	//Get packet contents
	int *summary=(int *)pkt->getContents();
	for(int i=1;i<=summary[0];i++)
	{
		if(!Buf->PacketExists(summary[i]))
		{//Packet does not exist in node's buffer
			unKnown++;
			//Insert it to the list of unknown packets
			req=(int*)realloc(req,(sizeof(int)*(unKnown+1)));
			req[unKnown]=summary[i];
		}
	}
	req[0]=unKnown;
	//Create a packet request as a response
	Packet *ReqPacket=new RequestPacket(CurrentTime,0);
	Header *head=new BasicHeader(this->NodeID,hd->GetprevHop());
	ReqPacket->setHeader(head);
	ReqPacket->setContents((void *)req);
	//Add packet to the packet pool
	pktPool->AddPacket(ReqPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),ReqPacket->getSize(),ReqPacket->getID());
	
	#ifdef LSFSPRAY_DEBUG
	printf("%f:Node %d received a summary packet with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("Summary contents(%d):\n",summary[0]);
	for(int i=1;i<=summary[0];i++)
	{
		printf("%d ",summary[i]);
	}
	printf("\n");
	printf("Packets unknown:%d\n",unKnown);
	#endif
	
	//Delete summary packet to free memory
	pktPool->ErasePacket(PID);
	return;

}


void LSFSpray::ReceptionRequest(Header* hd, Packet* pkt, int PID, double CurrentTime)
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
				continue;
			}
			#ifdef LSFSPRAY_DEBUG
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
	//Request packet
	#ifdef LSFSPRAY_DEBUG
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


void LSFSpray::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	//Data packet
	#ifdef LSFSPRAY_DEBUG
	printf("%f:Node %d received new data packet with ID:%d from %d\n",CurrentTime,this->NodeID,RealID,hd->GetprevHop());
	#endif
	//case I am the packet originator (packet comes from application layer)
	if(hd->GetSource() == this->NodeID && hd->GetNextHop() == -1)
	{
		Buf->addPkt(RealID,hd->GetDestination(),hd->GetSource(),CurrentTime,hd->GetHops(),hd->GetprevHop(),pkt->GetStartTime());
		(Buf->getPacketData(RealID))->SetReplicas(hd->GetRep());
		Stat->pktGen(RealID, hd->GetSource(), hd->GetDestination(), CurrentTime);
		return;
	}
	//If I'm not the next hop
	if(hd->GetNextHop() != this->NodeID)
	{
		//Update Statistics
		//Stat->incDuplicates();
		if(pkt->AccessPkt() == false)
		{
			pktPool->ErasePacket(PID);
		}
		return;
	}
	//Check if the destination of the packet is me
	if(hd->GetDestination() == this->NodeID)
	{
		//The above lines enable checking for duplicates 
		if(DM->NoDuplicatesSupport()  && DM->isDelivered(RealID))
		{
			printf("Problem: Packet %d has been already delivered!\n",RealID);
			exit(1);
		}
		DM->setAsDelivered(RealID);
		//Update Statistics
		Stat->pktRec(hd->GetHops(),(CurrentTime-pkt->GetStartTime()),pkt, pkt->GetStartTime(), false);
		//Garbage Collecting
		if(pkt->AccessPkt() == false)
		{
			pktPool->ErasePacket(PID);
		}
		return;
	}
	//printf("%d:Received packet %d from %d destined to %d\n",this->NodeID,RealID,hd->GetprevHop(),hd->GetDestination());
	//printf("Replicas set to:%d\n",(Buf->getPacketData(PacketID))->GetReplicas());
	//I am the next hop
	if(Buf->PacketExists(RealID))
	{
		#ifdef LSFSPRAY_DEBUG
		printf("%d:Received packet %d from %d destined to %d\n",this->NodeID,RealID,hd->GetprevHop(),hd->GetDestination());
		#endif
		(Buf->getPacketData(RealID))->IncReplicas(hd->GetRep());
		Stat->incTimesAsRelayNode(pkt->GetStartTime());
	}
	else
	{
		Buf->addPkt(RealID,hd->GetDestination(),hd->GetSource(),CurrentTime,hd->GetHops(),hd->GetprevHop(),pkt->GetStartTime());
		(Buf->getPacketData(RealID))->SetReplicas(hd->GetRep());
		Stat->incTimesAsRelayNode(pkt->GetStartTime());
	}
	if(pkt->AccessPkt() == false)
	{
		pktPool->ErasePacket(PID);
	}
	return;
}


void LSFSpray::SendPacket(double STime, int pktID,int nHop,int RepValue)
{
	Packet *p=pktPool->GetPacket(pktID);
	if(p == NULL)
	{//packet isn't found in the packet pool
		printf("Error: Packet %d doesn't exist in Packet Pool!Aborting..\n",pktID);
		exit(1);
	}
	//Duplicate the packet first
	Packet *newPkt=p->Duplicate(Buf->GetHops(pktID));
	newPkt->getHeader()->SetNextHop(nHop);
	newPkt->getHeader()->SetprevHop(this->NodeID);
	newPkt->getHeader()->SetRep(RepValue);
	pktPool->AddPacket(newPkt);
	//Then, inform current neighbors about the new packet
	int CurrentN=Mlayer->BroadcastPkt(STime,this->NodeID,newPkt->getSize(),newPkt->getID());
	
	//Garbage Collecting
	if(CurrentN > 0)
	{//Set access attribute to safely delete packet later
		newPkt->SetRecipients(CurrentN);
		//Update statistics
		if(newPkt->getHeader()->GetDestination() == nHop)
		{
			Stat->incHandovers(Buf->GetPktCreationTime(pktID));
		}
		Stat->incForwards(pktID, Buf->GetPktCreationTime(pktID));
	}
	else
	{//Cancel Broadcast and delete packet - case there are no neighbors
		pktPool->ErasePacket(newPkt->getID());
	}
	return;
}
