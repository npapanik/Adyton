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


#ifndef PROPHETSPRAY_H
	#define PROPHETSPRAY_H
	#include "ProphetSpray.h"
#endif

//#define PROPHETSPRAY_DEBUG

ProphetSpray::ProphetSpray(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St,Settings *S,God *G): Routing(PP, mc, Bf, NID, St,S,G)
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

	this->RepValues = NULL;

	return;
}

ProphetSpray::~ProphetSpray()
{
	delete MyDPT;
	free(RepValues);
	return;
}

void ProphetSpray::NewContact(double CTime, int NID)
{
	Contact(CTime,NID);
	return;
}


void ProphetSpray::ContactRemoved(double CTime,int NID)
{
	MyDPT->ContactDown(NID, CTime);
	return;
}

void ProphetSpray::SendPacket(double STime, int pktID,int nHop,int RepValue)
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

void ProphetSpray::recv(double rTime, int pktID)
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
		case REQUEST_CONTACTS_PACKET:
		{
			ReceptionRequestContacts(h,p,pktID,rTime);
			break;
		}
		case DEST_PREDICT_PACKET:
		{
			ReceptionDPT(h,p,pktID,rTime);
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
			printf("Error: Unknown packet type!ProphetSpray Delivery(RT) is not using this type of packet..Aborting...\n");
			exit(1);
		}
	}
	return;
}


void ProphetSpray::Contact(double CTime, int NID)
{
	MyDPT->ContactUp(NID, CTime);
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

void ProphetSpray::AfterDirectTransfers(double CTime, int NID)
{
	//Create new request packet (DPT request)
	Packet *ReqPacket=new ReqContacts(CTime,0);
	Header *h=new BasicHeader(this->NodeID,NID);
	ReqPacket->setHeader(h);
	//Add packet to the packet pool
	pktPool->AddPacket(ReqPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,ReqPacket->getSize(),ReqPacket->getID());
	#ifdef PROPHETSPRAY_DEBUG
	printf("%d:Send a request for DPT to node %d\n",this->NodeID,NID);
	#endif
	return;
}



void ProphetSpray::ReceptionData(Header* hd, Packet* pkt, int PID, double CurrentTime, int RealID)
{
	//Data packet
	#ifdef PROPHETSPRAY_DEBUG
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
		#ifdef PROPHETSPRAY_DEBUG
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

void ProphetSpray::ReceptionRequestContacts(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//request for a DPT packet
	#ifdef PROPHETSPRAY_DEBUG
	printf("%f:Node %d received a request for DPT with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	#endif

	//Create response containing My DPT table
	double *delivpre=MyDPT->CloneDPT(CurrentTime);
	Packet *Response=new DPs(CurrentTime,0);
	Response->setContents((void *)delivpre);
	Header *reshd=new BasicHeader(this->NodeID,hd->GetprevHop());
	Response->setHeader(reshd);
	//Add packet to the packet pool
	pktPool->AddPacket(Response);
	//Send packet
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),Response->getSize(),Response->getID());
	#ifdef PROPHETSPRAY_DEBUG
	printf("%f:Node %d send its DPT with ID:%d to %d\n",CurrentTime,this->NodeID,Response->getID(),hd->GetprevHop());
	printf("DPT contents(%d):\n",Set->getNN());
	for(int i=0;i<Set->getNN();i++)
	{
		printf("%d->%f ",i,delivpre[i]);
	}
	printf("\n");
	#endif
	
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void ProphetSpray::ReceptionDPT(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//reception of a packet that contains a node's DPT
	//Get packet contents
	double *info=(double *)pkt->getContents();
	#ifdef PROPHETSPRAY_DEBUG
	printf("%f:Node %d received a packet containing a DPT with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("DPT contents(%d):\n",Set->getNN());
	for(int i=0;i<Set->getNN();i++)
	{
		printf("%d->%f ",i,info[i]);
	}
	printf("\n");
	#endif
	
	MyDPT->UpdateDPT(info,hd->GetprevHop(),CurrentTime);
	//Get the destinations for which node in contact is better than me
	int *exchangeDests=MyDPT->CheckDPs(info,CurrentTime);
	free(RepValues);
	RepValues=NULL;
	RepValues=MyDPT->CalcRepValue(info,CurrentTime);
	#ifdef PROPHETSPRAY_DEBUG
	printf("Have to exchange packets that are destined to: ");
	for(int i=1;i<=exchangeDests[0];i++)
	{
		printf("%d ",exchangeDests[i]);
	}
	printf("\n");
	#endif
	
	
	//Support for other deletion mechanisms
	int validDests=0;
	for(int j=1;j<=exchangeDests[0];j++)
	{
		if(exchangeDests[j] != hd->GetprevHop())
		{
			validDests++;
		}
	}
	
	int *pkts[validDests];
	int pktsout=0;
	int runner=0;
	for(int j=1;j<=exchangeDests[0];j++)
	{
		if(exchangeDests[j] == hd->GetprevHop())
		{
			continue;
		}
		pkts[runner]=Buf->getPackets(exchangeDests[j]);
		pktsout+=pkts[runner][0];
		runner++;
	}
	
	//Send pkts -- old code (left to check validity)
// 	int *pkts[exchangeDests[0]];
// 	int pktsout=0;
// 	for(int j=1;j<=exchangeDests[0];j++)
// 	{
// 		pkts[j-1]=Buf->getPackets(exchangeDests[j]);
// 		pktsout+=pkts[j-1][0];
// 	}
	
	int *Finalpkts=(int *)malloc(sizeof(int)*(pktsout+1));
	Finalpkts[0]=pktsout;
	
	int pos=1;
	runner=0;
	for(int j=1;j<=exchangeDests[0];j++)
	{
		if(exchangeDests[j] == hd->GetprevHop())
		{
			continue;
		}
		for(int k=1;k<=pkts[runner][0];k++)
		{
			Finalpkts[pos]=pkts[runner][k];
			pos++;
		}
		free(pkts[runner]);
		runner++;
	}
	
	//old code (left to check validity)
// 	int runner=1;
// 	for(int j=1;j<=exchangeDests[0];j++)
// 	{
// 		for(int k=1;k<=pkts[j-1][0];k++)
// 		{
// 			Finalpkts[runner]=pkts[j-1][k];
// 			runner++;
// 		}
// 		free(pkts[j-1]);
// 	}
	
	//Free memory
	free(exchangeDests);
	
	//Create new summary packet
	Packet *SumPacket=new SummaryPacket(CurrentTime,0);
	SumPacket->setContents((void *)Finalpkts);
	Header *newhd=new BasicHeader(this->NodeID,hd->GetprevHop());
	SumPacket->setHeader(newhd);
	//Add packet to the packet pool
	pktPool->AddPacket(SumPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),SumPacket->getSize(),SumPacket->getID());

	#ifdef PROPHETSPRAY_DEBUG
	printf("%f:Node %d generated new summary packet with ID:%d for Node %d\n",CurrentTime,this->NodeID,SumPacket->getID(),hd->GetprevHop());
	printf("Summary contents(%d):\n",Finalpkts[0]);
	for(int i=1;i<=Finalpkts[0];i++)
	{
		printf("%d ",Finalpkts[i]);
	}
	printf("\n");
	#endif
	
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void ProphetSpray::ReceptionSummary(Header* hd, Packet* pkt, int PID, double CurrentTime)
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
	
	#ifdef PROPHETSPRAY_DEBUG
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

void ProphetSpray::ReceptionRequest(Header* hd, Packet* pkt, int PID, double CurrentTime)
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
		PacketEntry *tmpPkt=NULL;
		for(int i=1;i<=outgoing[0];i++)
		{
			tmpPkt=Buf->getPacketData(outgoing[i]);
			//Get current replication value
			Rcurrent=tmpPkt->GetReplicas();
			//Case replication value is 1
			if(Rcurrent == 1)
			{
				continue;
			}
			#ifdef PROPHETSPRAY_DEBUG
			printf("%d:For pkt %d Rcurrent is %d\n",this->NodeID,rq[i],Rcurrent);
			Buf->PrintPkts();
			#endif
			//Calculate new replication value
			HowMany=((double)Rcurrent)/2.0;
			Rnew=floor(HowMany);
			//HowMany=Rcurrent*this->RepValues[tmpPkt->Destination];
			//Rnew=floor(HowMany);
			//Send packet
			SendPacket(CurrentTime,outgoing[i],hd->GetprevHop(),Rnew);
			//Update Replication value
			(Buf->getPacketData(outgoing[i]))->SetReplicas(Rcurrent-Rnew);
		}
		free(RepValues);
		RepValues=NULL;
		free(outgoing);
	}
	//Request packet
	#ifdef PROPHETSPRAY_DEBUG
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


