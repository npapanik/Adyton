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


#ifndef ROUTING_H
	#define ROUTING_H
	#include "Routing.h"
#endif

// #define ROUTING_DEBUG

Routing::Routing(PacketPool* PP, MAC* mc, PacketBuffer* Bf,int NID,Statistics *St,Settings *S,God *G)
{
	this->Set=S;
	this->pktPool=PP;
	this->Mlayer=mc;
	this->Buf=Bf;
	this->NodeID=NID;
	this->Stat=St;
	this->SimGod=G;
	this->NCenabled=false;
	
	switch(S->getDM())
	{
		case JUSTTTL_DM:
		{
			DM = new JustTTL();
			break;
		}
		case NODUPLICATES_DM:
		{
			DM = new NoDuplicates(NID);
			break;
		}
		case VACCINE_DM:
		{
			DM = new Vaccine(NID);
			break;
		}
		case CATACLYSM_DM:
		{
			DM = new Cataclysm(S->getNN(),this->SimGod);
			break;
		}
		case NODUPSKEEPRELICATION_DM:
		{
			DM = new NoDupsKeepReplication(NID);
			break;
		}
		default:
		{
			printf("Error! Unknown deletion mechanism identifier (%d)\nExiting...\n", S->getDM());
			exit(EXIT_FAILURE);
		}
	}
	switch(S->getSchedulingPolicy())
	{
		case FIFO_SP:
		{
			sch = new FIFO(NID, this->Buf);
			break;
		}
		case LIFO_SP:
		{
			sch = new LIFO(NID, this->Buf);
			break;
		}
		case GRTRMAX_SP:
		{
			sch = new GRTRMax(NID, this->Buf);
			break;
		}
		case GRTRSORT_SP:
		{
			sch = new GRTRSort(NID, this->Buf);
			break;
		}
		case HNUV_SP:
		{
			sch = new HNUV(NID, this->Buf);
			break;
		}
		default:
		{
			printf("Error! Unknown scheduling policy identifier (%d)\nExiting...\n", S->getSchedulingPolicy());
			exit(EXIT_FAILURE);
		}
	}
	switch(S->getCongestionControl())
	{
		case NOCC_CC:
		{
			CC = new NoCC(NID, this->Buf, S);
			break;
		}
		case AVOIDOVERFLOW_CC:
		{
			CC = new AvoidOverflow(NID, this->Buf, S);
			break;
		}
		case ACC_CC:
		{
			CC = new ACC(NID, this->Buf, S);
			break;
		}
		case FAIRROUTE_CC:
		{
			CC = new FairRoute(NID, this->Buf, S);
			break;
		}
		case BSA_CC:
		{
			CC = new BSA(NID, this->Buf, S);
			break;
		}
		case CACC_CC:
		{
			CC = new CACC(NID, this->Buf, S);
			break;
		}
		case CCAF_CC:
		{
			//CC = new CCAF(NID, this->Buf, S);
			printf("\n[Error]: The CCAF congestion control mechanism is temporarily not available\n\n");
			exit(EXIT_FAILURE);
			break;
		}
		default:
		{
			printf("Error! Unknown congestion control identifier (%d)\nExiting...\n", S->getCongestionControl());
			exit(EXIT_FAILURE);
		}
	}

	return;
}

Routing::~Routing()
{
	delete DM;
	delete sch;
	delete CC;
	return;
}

void Routing::Finalize(void )
{
	//printf("Node %d buffer stores %d packets\n",this->NodeID,Buf->BufferStores());
	return;
}

void Routing::NoCostRecv(PacketPool *PP,MAC *mc,PacketBuffer *Bf,int NID,Statistics *St,Settings *S)
{
	return;
}

void Routing::ReceptionAntipacket(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	//Get packet contents and update vaccine data structure
	int *Dinfo=(int *)pkt->getContents();
	DM->UpdateInfo(Dinfo);
	//Remove packets that have reached their destinations from buffer
	DM->CleanBuffer(this->Buf);
	//Create an antipacket response
	Packet *antiResp=new AntiPacketResponse(CurrentTime,0);
	Header *h=new BasicHeader(this->NodeID,hd->GetprevHop());
	antiResp->setHeader(h);
	int *info=DM->GetInfo();
	antiResp->setContents((void *)info);
	//Add packet to the packet pool
	pktPool->AddPacket(antiResp);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),antiResp->getSize(),antiResp->getID());
	#ifdef ROUTING_DEBUG
	printf("%f:Node %d generated an antipacket response with ID:%d for Node %d\n",CurrentTime,this->NodeID,antiResp->getID(),hd->GetprevHop());
	printf("Antipacket response contents(%d):\n",info[0]);
	for(int i=1;i<=info[0];i++)
	{
		printf("%d ",info[i]);
	}
	printf("\n");
	#endif
	//Delete antipacket to free memory
	pktPool->ErasePacket(PID);
	return;
}

void Routing::ReceptionAntipacketResponse(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	//Get packet contents and update vaccine data structure
	int *Dinfo=(int *)pkt->getContents();
	DM->UpdateInfo(Dinfo);
	//Remove packets that have reached their destinations from buffer
	DM->CleanBuffer(this->Buf);
	//Delete antipacket response to free memory
	SendDirectPackets(CurrentTime,hd->GetprevHop());
	pktPool->ErasePacket(PID);
	return;
}

void Routing::SendDirectPackets(double CTime, int NID)
{
	//First send all packets that have NID as destination
	int *pkts=Buf->getPackets(NID);
	for(int i=1;i<=pkts[0];i++)
	{
		if(DM->isDelivered(pkts[i]))
		{
			continue;
		}
		SendPacket(CTime,pkts[i],NID,1);
// 		printf("That was direct!\n");
		DM->setAsDelivered(pkts[i]);
		if(DM->OffloaderDeletePkt() || Set->isSingleCopy())
		{
			Buf->removePkt(pkts[i]);
		}
	}
	free(pkts);
	//for NC enabled protocols
	if(DM->OffloaderDeletePkt() && this->NCenabled)
	{
		DM->CleanBuffer(this->Buf);
	}

	if(Set->needsBufferInformation())
	{
		SendBufferReq(CTime,NID);
	}
	else
	{
		AfterDirectTransfers(CTime,NID);
	}
	return;
}

void Routing::SendDirectSummary(double CTime, int NID)
{
	//Prepare the summary vector for direct delivery
	int *summary=Buf->getPackets(NID);
	//Create a new summary packet
	Packet *SumPacket=new DirectSummaryPacket(CTime,0);
	SumPacket->setContents((void *)summary);
	Header *h=new BasicHeader(this->NodeID,NID);
	SumPacket->setHeader(h);
	//Add packet to the packet pool
	pktPool->AddPacket(SumPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,SumPacket->getSize(),SumPacket->getID());
	#ifdef DEBUG
	printf("%f:Node %d generated a new summary packet (for direct delivery) with ID:%d for Node %d\n",CTime,this->NodeID,SumPacket->getID(),NID);
	printf("Summary contents(%d):\n",summary[0]);
	for(int i=1;i<=summary[0];i++)
	{
		printf("%d ",summary[i]);
	}
	printf("\n");
	#endif
	return;
}


void Routing::SendBufferReq(double CTime, int NID)
{
	//Create a new buffer request packet
	Packet *bufRq=new BufferRequest(CTime,0);
	Header *h=new BasicHeader(this->NodeID,NID);
	bufRq->setHeader(h);
	//Add packet to the packet pool
	pktPool->AddPacket(bufRq);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,bufRq->getSize(),bufRq->getID());
	#ifdef DEBUG
	printf("%f:Node %d generated a new buffer request packet with ID:%d for Node %d\n",CTime,this->NodeID,bufRq->getID(),NID);
	#endif
	return;
}

void Routing::ReceptionBufferReq(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	//Create a buffer response
	Packet *bufRsp=new BufferReply(CurrentTime,0);
	Header *h=new BasicHeader(this->NodeID,hd->GetprevHop());
	bufRsp->setHeader(h);
	struct BufferInformation *info=(struct BufferInformation *)malloc(sizeof(struct BufferInformation));
	CC->receivedBufferReq(CurrentTime, hd->GetprevHop());
	info->bSize=CC->getBufferSize();
	info->bLength=CC->getBufferLength();
	bufRsp->setContents((void *)info);
	//Add packet to the packet pool
	pktPool->AddPacket(bufRsp);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),bufRsp->getSize(),bufRsp->getID());
	#ifdef ROUTING_DEBUG
	printf("%f:Node %d generated a buffer response with ID:%d for Node %d\n",CurrentTime,this->NodeID,bufRsp->getID(),hd->GetprevHop());
	printf("Contents:buffer size:%d\tbuffer length:%d\n",info->bSize,info->bLength);
	#endif
	//Delete the buffer request packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void Routing::ReceptionBufferRsp(Header* hd, Packet* pkt, int PID, double CurrentTime)
{
	struct BufferInformation *pktInfo=(struct BufferInformation *)pkt->getContents();
	#ifdef ROUTING_DEBUG
	printf("%f:Node %d received a buffer response packet with ID:%d from Node %d\n",CurrentTime,this->NodeID,pkt->getID(),hd->GetprevHop());
	printf("Contents: buffer size:%d\tbuffer length:%d\n",pktInfo->bSize,pktInfo->bLength);
	#endif
	struct CongestionInformation *info=(struct CongestionInformation *)malloc(sizeof(struct CongestionInformation));
	info->recID=hd->GetprevHop();
	info->recBufSize=pktInfo->bSize;
	info->recBufLength=pktInfo->bLength;
	CC->addBufferInfo((void *)info);
	AfterDirectTransfers(CurrentTime,hd->GetprevHop());
	//Delete the buffer reply packet to free memory
	pktPool->ErasePacket(PID);
	return;
}



void Routing::AfterDirectTransfers(double CTime, int NID)
{
	return;
}

void Routing::SendVaccine(double CTime,int NID,int *info)
{
	//Create a new antipacket
	Packet *antip=new AntiPacket(CTime,0);
	antip->setContents((void *)info);
	Header *h=new BasicHeader(this->NodeID,NID);
	antip->setHeader(h);
	//Add packet to the packet pool
	pktPool->AddPacket(antip);
	//Send packet to the new contact
	Mlayer->SendPkt(CTime,this->NodeID,NID,antip->getSize(),antip->getID());
	#ifdef ROUTING_DEBUG
	printf("%f:Node %d generated a new antipacket with ID:%d for Node %d\n",CTime,this->NodeID,antip->getID(),NID);
	printf("Antipacket contents(%d):\n",info[0]);
	for(int i=1;i<=info[0];i++)
	{
		printf("%d ",info[i]);
	}
	printf("\n");
	#endif
	return;
}

void Routing::ReceptionDirectSummary(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	//Summary packet for direct delivery
	int *Directreq=(int*)malloc(sizeof(int));
	int DirectunKnown=0;
	//Get packet contents
	int *Directsummary=(int *)pkt->getContents();
	for(int i=1;i<=Directsummary[0];i++)
	{
		if(!Buf->PacketExists(Directsummary[i]) && !DM->isDelivered(Directsummary[i]))
		{//Packet does not exist in node's buffer
			DirectunKnown++;
			//Insert it to the list of unknown packets
			Directreq=(int*)realloc(Directreq,(sizeof(int)*(DirectunKnown+1)));
			Directreq[DirectunKnown]=Directsummary[i];
		}
	}
	Directreq[0]=DirectunKnown;
	//Create a packet request as a response
	Packet *DirectReqPacket=new DirectRequestPacket(CurrentTime,0);
	Header *Directhead=new BasicHeader(this->NodeID,hd->GetprevHop());
	DirectReqPacket->setHeader(Directhead);
	DirectReqPacket->setContents((void *)Directreq);
	//Add packet to the packet pool
	pktPool->AddPacket(DirectReqPacket);
	//Send packet to the new contact
	Mlayer->SendPkt(CurrentTime,this->NodeID,hd->GetprevHop(),DirectReqPacket->getSize(),DirectReqPacket->getID());
	
	#ifdef ROUTING_DEBUG
	printf("%f:Node %d received a summary packet (for direct delivery) with ID:%d from %d\n",CurrentTime,this->NodeID,PID,hd->GetprevHop());
	printf("Summary contents(%d):\n",Directsummary[0]);
	for(int i=1;i<=Directsummary[0];i++)
	{
		printf("%d ",Directsummary[i]);
	}
	printf("\n");
	printf("Packets unknown:%d\n",DirectunKnown);
	#endif
	//Delete summary packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void Routing::ReceptionDirectRequest(Header *hd,Packet *pkt,int PID,double CurrentTime)
{
	//schedule the transmission of requested packets 
	int *Directrq=(int *)pkt->getContents();
	for(int i=1;i<=Directrq[0];i++)
	{
		#ifdef ROUTING_DEBUG
		printf("%d:Sending packet %d to %d (final destination:%d)\n",this->NodeID,Directrq[i],hd->GetprevHop(),Buf->GetPktDestination(Directrq[i]));
		#endif
		SendPacket(CurrentTime,Directrq[i],hd->GetprevHop(),1);
		//Buf->removePkt(Directrq[i]);
	}
	if(DM->OffloaderDeletePkt() || Set->isSingleCopy())
	{
		//Delete all packets that are destined to the node in contact
		int *summary=Buf->getPackets(hd->GetprevHop());
		for(int i=1;i<=summary[0];i++)
		{
			#ifdef ROUTING_DEBUG
			printf("Removing %d...",summary[i]);
			#endif
			Buf->removePkt(summary[i]);
		}
		free(summary);
	}
	if(Set->needsBufferInformation())
	{
		SendBufferReq(CurrentTime,hd->GetprevHop());
	}
	else
	{
		AfterDirectTransfers(CurrentTime,hd->GetprevHop());
	}
	//Delete Direct delivery request packet to free memory
	pktPool->ErasePacket(PID);
	return;
}

void Routing::SendPacket(double STime, int pktID, int nHop, int RepValue)
{
	return;
}

