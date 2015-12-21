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


#ifndef MSFSPRAY_H
	#define MSFSPRAY_H
	#include "MSFSpray.h"
#endif

//#define MSFSPRAY_DEBUG


MSFSpray::MSFSpray(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St,Settings *S,God *G): Routing(PP, mc, Bf, NID, St,S,G)
{
	Adja=NULL;
	SB=new Sociability(NID,Set->getNN());
	
	string TCinfo;
	if(S->ProfileExists() && (TCinfo=S->GetProfileAttribute("TC")) == "on")
	{
		this->TCstatus=true;
	}
	else
	{
		this->TCstatus=false;
	}
	return;
}

MSFSpray::~MSFSpray()
{
	delete Adja;
	delete SB;
	return;
}



void MSFSpray::Contact(double CTime, int NID)
{
	//Update Sociability data
	SB->Update(NID,CTime);
	
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
}


void MSFSpray::NewContact(double CTime, int NID)
{
	Contact(CTime,NID);
	return;
}


void MSFSpray::ContactRemoved(double CTime,int NID)
{
	return;
}


void MSFSpray::AfterDirectTransfers(double CTime, int NID)
{
	//Prepare the summary vector
// 	int *OtherSummary=Buf->getAllPackets();
	int *OtherSummary=Buf->getPacketsNotDestinedTo(NID);
	if(OtherSummary[0] == 0)
	{//No packet inside buffer
		free(OtherSummary);
		return;
	}
	//Create new summary packet
	Packet *SumPacket=new SocialSummary(CTime,0);
	SumPacket->setContents((void *)OtherSummary);
	((SocialSummary *)SumPacket)->SetSocialMetric(SB->get(this->NodeID,CTime));
	Header *Directhead=new BasicHeader(this->NodeID,NID);
	SumPacket->setHeader(Directhead);
	//Add packet to the packet pool
	pktPool->AddPacket(SumPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,SumPacket->getSize(),SumPacket->getID());
	
	#ifdef MSFSPRAY_DEBUG
	printf("%f:Node %d generated new summary packet with ID:%d for Node %d\n",CTime,this->NodeID,SumPacket->getID(),NID);
	printf("Sociability metric:%f\n",SB->get(this->NodeID,CTime));
	printf("Summary contents(%d):\n",OtherSummary[0]);
	for(int i=1;i<=OtherSummary[0];i++)
	{
		printf("%d ",OtherSummary[i]);
	}
	printf("\n");
	#endif
	return;
}


void MSFSpray::ReceptionUtilSummary(Header* hd, Packet* pkt, int PID, double CurrentTime)
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
	
	#ifdef MSFSPRAY_DEBUG
	double otherSocialMetric=((SocialSummary *)pkt)->GetSocialMetric();
	double mySocialMetric=SB->get(this->NodeID,CurrentTime);
	printf("%f:Node %d received a summary packet with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("My Social metric:%f - Other Social metric: %f\n",mySocialMetric,otherSocialMetric);
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

void MSFSpray::ReceptionRequest(Header* hd, Packet* pkt, int PID, double CurrentTime)
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
			#ifdef MSFSPRAY_DEBUG
			printf("%d:For pkt %d Rcurrent is %d\n",this->NodeID,outgoing[i],Rcurrent);
			Buf->PrintPkts();
			#endif
			//Calculate new replication value
			HowMany=((double)Rcurrent)/2.0;
			Rnew=floor(HowMany);
			//Send packet
			Stat->incReps(Buf->GetPktCreationTime(outgoing[i]));
			SendPacket(CurrentTime,outgoing[i],hd->GetprevHop(),Rnew);
			//Update Replication value
			(Buf->getPacketData(outgoing[i]))->SetReplicas(Rcurrent-Rnew);
		}
		free(outgoing);
	}
	//Request packet
	#ifdef MSFSPRAY_DEBUG
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

void MSFSpray::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	//Data packet
	#ifdef MSFSPRAY_DEBUG
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
		if(DM->NoDuplicatesSupport() && DM->isDelivered(RealID))
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
	//printf("Replicas set to:%d\n",(Buf->getPacketData(PacketID))->GetReplicas());
	//I am the next hop
	if(Buf->PacketExists(RealID))
	{
		#ifdef MSFSPRAY_DEBUG
		printf("%d:Received packet %d from %d destined to %d\n",this->NodeID,PacketID,hd->GetprevHop(),hd->GetDestination());
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

void MSFSpray::SendPacket(double STime, int pktID,int nHop,int RepValue)
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


void MSFSpray::recv(double rTime, int pktID)
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
		case UTIL_SUMMARY_PACKET:
		{
			ReceptionUtilSummary(h,p,pktID,rTime);
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
			printf("Error: Unknown packet type!MSFSpray Delivery(RT) is not using this type of packet..Aborting...\n");
			exit(1);
		}
	}
	return;
}
