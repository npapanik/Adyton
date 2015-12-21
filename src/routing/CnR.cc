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


#ifndef CNR_H
	#define CNR_H
	#include "CnR.h"
#endif

// #define CNR_DEBUG



CnR::CnR(PacketPool* PP, MAC* mc, PacketBuffer* Bf, int NID, Statistics* St, Settings* S, God* G):Routing(PP, mc, Bf, NID, St,S,G)
{
	string UtilityType;
	double agingTimeUnit=1.0;
	string profileAttribute;
	Util=NULL;
	Adja=NULL;
	MyDPT=NULL;
	if(S->ProfileExists() && (UtilityType=S->GetProfileAttribute("Utility")) != "none")
	{
		//remove all whitespaces
		UtilityType.erase( std::remove_if( UtilityType.begin(), UtilityType.end(), ::isspace ), UtilityType.end() );
		if(UtilityType == "LTS")
		{
			Util=new LTS(NID,Set->getNN());
		}
		else if(UtilityType == "LastContact")
		{
			Util=new LastContact(NID,Set->getNN());
		}
		else if(UtilityType == "DestEnc")
		{
			//Util=new DestEnc(NID,Set->getNN(),0.85,3600.0*6.0);
			Util=new DestEnc(NID,Set->getNN());
		}
		else if(UtilityType == "Enc")
		{
			//Util=new Enc(NID,Set->getNN(),0.85,3600.0*8.0);
			Util=new Enc(NID,Set->getNN());
		}
		else if(UtilityType == "AMT")
		{
			Util=new AMT(NID,Set->getNN());
		}
		else if(UtilityType == "AIT")
		{
			Util=new AIT(NID,Set->getNN());
		}
		else if(UtilityType == "SPM")
		{
			Util=new SPM(NID,Set->getNN());
		}
		else if(UtilityType == "Bet")
		{
			Adja=new Adjacency(NID,Set->getNN());
		}
		else if(UtilityType == "Sim")
		{
			Adja=new Adjacency(NID,Set->getNN());
		}	
		else if(UtilityType == "Prophet")
		{
			//MyDPT=new DPTv2(NodeID,Set->getNN());
			//MyDPT=new DPT(NodeID,Set->getNN());
			MyDPT=new DPTv3(NodeID,Set->getNN());
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
		}
		else
		{
			printf("Please select a proper utility!\n");
			exit(1);
		}
	}
	else
	{
		//set as default the LTS Utility
		Util=new LTS(NID,Set->getNN());
	}
	this->UType=UtilityType;
	return;
}

CnR::~CnR()
{
	delete Util;
	delete Adja;
	delete MyDPT;
	return;
}

void CnR::NewContact(double CTime, int NID)
{
	Contact(CTime,NID);
	return;
}


void CnR::ContactRemoved(double CTime,int NID)
{
	//printf("%d:Contact %d removed\n",this->NodeID,NID);
	if(Adja != NULL)
	{
		Adja->ContactEnd(NID,CTime);
	}
	if(Util != NULL)
	{
		Util->ContactDown(NID,CTime);
	}
	if(MyDPT != NULL)
	{
		MyDPT->ContactDown(NID, CTime);
	}
	return;
}


void CnR::SendPacket(double STime, int pktID,int nHop,int RepValue)
{
	Packet *p=pktPool->GetPacket(pktID);
	if(p == NULL)
	{//packet isn't found in the packet pool
		printf("Error: Packet %d doesn't exist in Packet Pool!Aborting..\n",pktID);
		exit(1);
	}
// 	printf("%d:Sending the native packet %d(S:%d,D:%d) to node %d\n",this->NodeID,pktID,p->getHeader()->GetSource(),p->getHeader()->GetDestination(),nHop);
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

void CnR::Contact(double CTime, int NID)
{
	//printf("%d:Contact with %d\n",this->NodeID,NID);
	if(Adja != NULL)
	{
		//Update Ego network
		Adja->SetConnection(this->NodeID,NID,CTime);
		//Update data structures that collect data to estimate metrics
		Adja->ContactStart(NID,CTime);
	}
	if(Util != NULL)
	{
		Util->ContactUp(NID,CTime);
	}
	if(MyDPT)
	{
		MyDPT->ContactUp(NID, CTime);
	}
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

void CnR::recv(double rTime, int pktID)
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
		case CONTACTS_PACKET:
		{
			ReceptionContacts(h,p,pktID,rTime);
			break;
		}
		case DEST_PREDICT_PACKET:
		{
			ReceptionDPT(h,p,pktID,rTime);
			break;
		}
		case REQ_RSPM:
		{
			ReceptionRequestRSPM(h,p,pktID,rTime);
			break;
		}
		case IND_RSPM:
		{
			ReceptionRSPM(h,p,pktID,rTime);
			break;
		}
		case PKTS_DESTS:
		{
			ReceptionSummary(h,p,pktID,rTime);
			break;
		}
		case REQUEST_PKTS_UTILS:
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
			printf("Error: Unknown packet type!Delegation Forwarding protocol is not using this type of packet..Aborting...\n");
			exit(1);
		}
	}
	return;
}

void CnR::AfterDirectTransfers(double CTime, int NID)
{
	
	if(MyDPT || Adja)
	{
		SendContactRequest(CTime,NID);
	}
	else
	{
		SendSummary(CTime,NID);
	}
	return;
}

/* ReceptionDPT
 * ------------
 * This method is called when a packet, that contains the delivery probabilities of the node in
 * contact, is received. The proper updates in the receiving node's delivery probability table
 * take place.
 */
void CnR::ReceptionDPT(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//packet containing DPT
	//Get packet contents
	double *info=(double *)pkt->getContents();
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a packet containing a DPT with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("DPT contents(%d):\n",Set->getNN());
	for(int i=0;i<Set->getNN();i++)
	{
		printf("%d->%f ",i,info[i]);
	}
	printf("\n");
	#endif
	
	MyDPT->UpdateDPT(info,hd->GetprevHop(),CurrentTime);
	//Reply with a packet summary
	SendSummary(CurrentTime,hd->GetprevHop());
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void CnR::SendSummary(double CTime, int NID)
{
	//Prepare the summary vector for all other packets
//  	int *OtherSummary=Buf->getAllPackets();
	int *OtherSummary=Buf->getPacketsNotDestinedTo(NID);

	
	struct PktDest *Sum=(struct PktDest *)malloc(sizeof(struct PktDest)* OtherSummary[0]);
	for(int i=1;i<=OtherSummary[0];i++)
	{
		Sum[i-1].PID=OtherSummary[i];
		Sum[i-1].Dest=Buf->GetPktDestination(OtherSummary[i]);
	}
	//Create new summary packet
	Packet *SumPacket=new PktDests(CTime,0);
	SumPacket->setContents((void *)Sum);
	Header *SumHeader=new BasicHeader(this->NodeID,NID);
	SumPacket->setHeader(SumHeader);
	((PktDests*)SumPacket)->setPktNum(OtherSummary[0]);
	//Add packet to the packet pool
	pktPool->AddPacket(SumPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,SumPacket->getSize(),SumPacket->getID());
// 	printf("N%d: Sum %d\n",this->NodeID,OtherSummary[0]);
	#ifdef CNR_DEBUG
	printf("%f:Node %d generated new summary packet with ID:%d for Node %d\n",CTime,this->NodeID,SumPacket->getID(),NID);
	printf("Summary contents(%d):\n",OtherSummary[0]);
	for(int i=0;i<OtherSummary[0];i++)
	{
		printf("%d(Dest:%d) ",Sum[i].PID,Sum[i].Dest);
	}
	printf("\n");
	#endif
	free(OtherSummary);
	return;
}

void CnR::SendContactRequest(double CTime, int NID)
{
	//Create new request contacts packet
	Packet *ReqPacket=new ReqContacts(CTime,0);
	Header *Reqh=new BasicHeader(this->NodeID,NID);
	ReqPacket->setHeader(Reqh);
	//Add packet to the packet pool
	pktPool->AddPacket(ReqPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,ReqPacket->getSize(),ReqPacket->getID());
	#ifdef CNR_DEBUG
	printf("%d:Send a request for contacts to node %d\n",this->NodeID,NID);
	#endif
	return;
}

void CnR::SendRSPMRequest(double CTime, int NID)
{
	//Create new request contacts packet
	Packet *ReqPacket=new ReqRSPM(CTime,0);
	Header *Reqh=new BasicHeader(this->NodeID,NID);
	ReqPacket->setHeader(Reqh);
	//Add packet to the packet pool
	pktPool->AddPacket(ReqPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,ReqPacket->getSize(),ReqPacket->getID());
	#ifdef CNR_DEBUG
	printf("%d:Send a request for RSPM values to node %d\n",this->NodeID,NID);
	#endif
	return;
}

void CnR::ReceptionRequestContacts(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//request for contacts packet
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a request for contacts with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	#endif
	if(this->UType == "Prophet")
	{
		//Create response containing contacts
		double *delivpre=MyDPT->CloneDPT(CurrentTime);
		Packet *Response=new DPs(CurrentTime,0);
		Response->setContents((void *)delivpre);
		Header *reshd=new BasicHeader(this->NodeID,hd->GetprevHop());
		Response->setHeader(reshd);
		//Add packet to the packet pool
		pktPool->AddPacket(Response);
		//Send packet
		Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),Response->getSize(),Response->getID());
		#ifdef CNR_DEBUG
		printf("%f:Node %d send its DPT with ID:%d to %d\n",CurrentTime,this->NodeID,Response->getID(),hd->GetprevHop());
		printf("DPT contents(%d):\n",Set->getNN());
		for(int i=0;i<Set->getNN();i++)
		{
			printf("%d->%f ",i,delivpre[i]);
		}
		printf("\n");
		#endif
	}
	else
	{
		//Create response containing contacts
		int *con=Adja->GetMyContacts();
		Packet *Response=new Contacts(CurrentTime,0);
		Response->setContents((void *)con);
		Header *reshd=new BasicHeader(this->NodeID,hd->GetprevHop());
		Response->setHeader(reshd);
		//Add packet to the packet pool
		pktPool->AddPacket(Response);
		//Send packet
		Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),Response->getSize(),Response->getID());
		#ifdef CNR_DEBUG
		printf("%f:Node %d send its contacts with ID:%d to %d\n",CurrentTime,this->NodeID,Response->getID(),hd->GetprevHop());
		printf("Contact contents(%d):\n",con[0]);
		for(int i=1;i<=con[0];i++)
		{
			printf("%d ",con[i]);
		}
		printf("\n");
		#endif
	}
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void CnR::ReceptionRequestRSPM(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//request for RSPM values
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a request for RSPM values with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	#endif
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	//Create response containing contacts
	double *val=((SPM *)Util)->getRSPMfor(hd->GetprevHop());
	Packet *Response=new IndRSPM(CurrentTime,0);
	Response->setContents((void *)val);
	Header *reshd=new BasicHeader(this->NodeID,hd->GetprevHop());
	Response->setHeader(reshd);
	//Add packet to the packet pool
	pktPool->AddPacket(Response);
	//Send packet
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),Response->getSize(),Response->getID());
	#ifdef CNR_DEBUG
	printf("%f:Node %d send its RSPM values (regarding node %d) with ID:%d to %d\n",CurrentTime,this->NodeID,hd->GetprevHop(),Response->getID(),hd->GetprevHop());
	printf("RSPM values: ");
	if(val)
	{
		for(int i=0;i<Set->getNN();i++)
		{
			printf("%d(%f) ",i,val[i]);
		}
		printf("\n");
	}
	else
	{
		printf("None\n");
	}
	#endif
	return;
}

void CnR::ReceptionContacts(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Packet with contacts
	//Get packet contents
	int *info=(int *)pkt->getContents();
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a packet containing contacts with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("Contacts contents(%d):\n",info[0]);
	for(int i=1;i<=info[0];i++)
	{
		printf("%d ",info[i]);
	}
	printf("\n");
	#endif
	//Update Ego matrix
	for(int i=1;i<=info[0];i++)
	{
		Adja->SetConnection(hd->GetprevHop(),info[i],CurrentTime);
	}
	//Update metrics
	Adja->UpdateAll();
	//Reply with a packet summary
	SendSummary(CurrentTime,hd->GetprevHop());
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void CnR::ReceptionRSPM(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Packet with RSPM values
	//Get packet contents
	double *info=(double *)pkt->getContents();
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a packet containing RSPM values with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("RSPM values: ");
	if(info)
	{
		for(int i=0;i<Set->getNN();i++)
		{
			printf("%d(%f) ",i,info[i]);
		}
		printf("\n");
	}
	else
	{
		printf("None\n");
	}
	#endif
	//Update my RSPM values
	((SPM *)Util)->updateRSPM(hd->GetprevHop(),info);
	//Delete packet to free memory
	pktPool->ErasePacket(PID);
	//Reply with a packet summary
	SendSummary(CurrentTime,hd->GetprevHop());
	return;
}


void CnR::ReceptionSummary(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	//Summary packet
	int *req=(int*)malloc(sizeof(int));
	int *reqDest=(int*)malloc(sizeof(int));
	int unKnown=0;
	//Get packet contents
	struct PktDest *summary=(struct PktDest *)pkt->getContents();
	int numContents=((PktDests *)pkt)->getPktNum();
	for(int i=0;i<numContents;i++)
	{
		if(!Buf->PacketExists(summary[i].PID))
		{//Packet does not exist in node's buffer
			unKnown++;
			//Insert it to the list of unknown packets
			req=(int*)realloc(req,(sizeof(int)*(unKnown+1)));
			req[unKnown]=summary[i].PID;
			reqDest=(int*)realloc(reqDest,(sizeof(int)*(unKnown+1)));
			reqDest[unKnown]=summary[i].Dest;
		}
	}
	req[0]=unKnown;
	reqDest[0]=unKnown;
	double MyBet=0.0;
	if(Adja != NULL)
	{
		if(this->UType == "Bet")
		{
			MyBet=Adja->getBet();
		}
	}
	struct PktUtil *RList=(struct PktUtil *)malloc(sizeof(struct PktUtil)*req[0]);
	double util=0.0;
	for(int i=0;i<req[0];i++)
	{
		RList[i].PID=req[i+1];
		if(MyDPT && this->UType == "Prophet")
		{
			util=MyDPT->getDPto(reqDest[i+1],CurrentTime);
		}
		else if(Adja)
		{
			if(this->UType == "Bet")
			{
				util=MyBet;
			}
			else
			{
				util=Adja->getSim(reqDest[i+1]);
			}
		}
		if(Util != NULL)
		{
			util=Util->get(reqDest[i+1],CurrentTime);
		}
		RList[i].Util=util;
		RList[i].Exists=false;
		RList[i].mode=0;
	}
	free(req);
	free(reqDest);
	//Create a packet request as a response
	Packet *ReqPacket=new PktUtils(CurrentTime,0);
	Header *head=new BasicHeader(this->NodeID,hd->GetprevHop());
	ReqPacket->setHeader(head);
	ReqPacket->setContents((void *)RList);
	((PktUtils *)ReqPacket)->setPktNum(unKnown);
	//Add packet to the packet pool
	pktPool->AddPacket(ReqPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),ReqPacket->getSize(),ReqPacket->getID());
// 	printf("N%d: Req %d\n",this->NodeID,numContents);
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a summary packet with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("Summary contents(%d):\n",numContents);
	for(int i=0;i<numContents;i++)
	{
		printf("%d(Dest:%d) ",summary[i].PID,summary[i].Dest);
	}
	printf("\n");
	#endif
	//Delete summary packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void CnR::ReceptionRequest(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	//Received packet containing pkts requested and their utilities
	struct PktUtil *RqList=(struct PktUtil *)pkt->getContents();
	int PktNum=((PktUtils *)pkt)->getPktNum();
	//split packet contents to two arrays
	int *Requests=(int *)malloc(sizeof(int)*(PktNum+1));
	double *Utils=(double *)malloc(sizeof(double)*(PktNum+1));
	bool *OwnedByOther=(bool *)malloc(sizeof(bool)*(PktNum+1));
	
	Requests[0]=PktNum;
	Utils[0]=PktNum;
	OwnedByOther[0]=false;
	for(int i=1;i<=PktNum;i++)
	{
		Requests[i]=RqList[i-1].PID;
		Utils[i]=RqList[i-1].Util;
		OwnedByOther[i]=false;
	}
	
	double myUtil=0.0;
	int dest=-1;
	int total=0;
	for(int i=1;i<=PktNum;i++)
	{
		dest=Buf->getPacketDest(Requests[i]);
		if(MyDPT && this->UType == "Prophet")
		{
			myUtil=MyDPT->getDPto(dest,CurrentTime);
		}
		else if(Adja)
		{
			if(this->UType == "Bet")
			{
				myUtil=Adja->getBet();
			}
			else if(this->UType == "Sim")
			{
				myUtil=Adja->getSim(dest);
			}
			else
			{
				myUtil=0.0;
			}
		}
		else
		{
			myUtil=Util->get(dest,CurrentTime);
		}
		if(Utils[i] - myUtil > DBL_EPSILON)
		{
			total++;
		}
	}
	int *BetterFw=(int *)malloc(sizeof(int)*(total+1));
	BetterFw[0]=total;
	int runner=0;
	for(int i=1;i<=PktNum;i++)
	{
		dest=Buf->getPacketDest(Requests[i]);
		if(MyDPT && this->UType == "Prophet")
		{
			myUtil=MyDPT->getDPto(dest,CurrentTime);
		}
		else if(Adja)
		{
			if(this->UType == "Bet")
			{
				myUtil=Adja->getBet();
			}
			else if(this->UType == "Sim")
			{
				myUtil=Adja->getSim(dest);
			}
			else
			{
				myUtil=0.0;
			}
		}
		else
		{
			myUtil=Util->get(dest,CurrentTime);
		}
		if(Utils[i] - myUtil > DBL_EPSILON)
		{
			runner++;
			BetterFw[runner]=Requests[i];
		}
	}
	#ifdef CNR_DEBUG
	printf("Node %d will send the following packets (%d)\n",this->NodeID,BetterFw[0]);
	for(int i=1;i<=BetterFw[0];i++)
	{
		printf("%d(D:%d) ",BetterFw[i],Buf->getPacketDest(BetterFw[i]));
	}
	printf("\n");
	#endif
// 	printf("N%d:%d\n",this->NodeID,BetterFw[0]);
	
	for(int i=1;i<=BetterFw[0];i++)
	{
		sch->addPacket(BetterFw[i],NULL);
	}
	free(BetterFw);
	//Apply scheduling
	int *outgoing=sch->getOutgoingPackets();
	//Apply congestion control
	outgoing=CC->filterPackets(outgoing);
	//Send packets
	if(outgoing)
	{
		for(int i=1;i<=outgoing[0];i++)
		{
			SendPacket(CurrentTime,outgoing[i],hd->GetprevHop(),1);
		}
		free(outgoing);
	}
	#ifdef CNR_DEBUG
	printf("%f:Node %d received a request packet with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("Request contents(%d):\n",PktNum);
	for(int i=0;i<PktNum;i++)
	{
		printf("%d(Util:%f) ",Requests[i+1],Utils[i+1]);
	}
	printf("\n");
	#endif
	//free memory
	free(Requests);
	free(Utils);
	free(OwnedByOther);
	pktPool->ErasePacket(PID);
	return;
}

void CnR::ReceptionData(Header *hd,Packet *pkt,int PID,double CurrentTime,int RealID)
{
	//Data packet
	#ifdef CNR_DEBUG
	printf("%f:Node %d received new data packet with ID:%d from %d\n",CurrentTime,this->NodeID,RealID,hd->GetprevHop());
	#endif
	//case I am the packet originator (packet comes from application layer)
	if(hd->GetSource() == this->NodeID && hd->GetNextHop() == -1)
	{
		Buf->addPkt(RealID,hd->GetDestination(),hd->GetSource(),CurrentTime,hd->GetHops(),hd->GetprevHop(),pkt->GetStartTime());
		if(Adja != NULL)
		{
			if(this->UType == "Bet")
			{
				double MyBet=Adja->getBet();
				(Buf->getPacketData(RealID))->SetMaxUtil(MyBet);
			}
			else
			{
				double MySim=Adja->getSim(hd->GetDestination());
				(Buf->getPacketData(RealID))->SetMaxUtil(MySim);
			}
		}
		if(Util != NULL)
		{
			(Buf->getPacketData(RealID))->SetMaxUtil(Util->get(hd->GetDestination(),CurrentTime));
		}
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
	//I am the next hop
	if(Buf->PacketExists(RealID))
	{
		printf("[Error]: Node %d received a packet with ID %d from node %d that already exists in its buffer\n", this->NodeID, RealID, hd->GetprevHop());
		exit(EXIT_FAILURE);
	}
	else
	{
		Buf->addPkt(RealID,hd->GetDestination(),hd->GetSource(),CurrentTime,hd->GetHops(),hd->GetprevHop(),pkt->GetStartTime());
		Stat->incTimesAsRelayNode(pkt->GetStartTime());
		if(Adja != NULL)
		{
			if(this->UType == "Bet")
			{
				(Buf->getPacketData(RealID))->SetMaxUtil(Adja->getBet());
			}
			else
			{
				(Buf->getPacketData(RealID))->SetMaxUtil(Adja->getSim(hd->GetDestination()));
			}
		}
		if(Util != NULL)
		{
			(Buf->getPacketData(RealID))->SetMaxUtil(Util->get(hd->GetDestination(),CurrentTime));
		}
	}
	if(pkt->AccessPkt() == false)
	{
		pktPool->ErasePacket(PID);
	}
	return;
}
