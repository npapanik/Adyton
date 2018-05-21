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
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#ifndef PACKET_H
	#define PACKET_H
	#include "Packet.h"
#endif

/* -------------
 * PACKET TYPES |
 * -------------
 * 1: DataPacket: Data packet (used by epidemic, SimBet)
 * 2: SummaryPacket: Summary packet (used by epidemic)
 * 3: RequestPacket: Request packet (used by epidemic)
 * 4: ReqContacts: Request contacts packet (used by SimBet) 
 * 5: Contacts: Packet containing contacts (used by SimBet)
 * 6: Destinations: Packet containing destinations (used by SimBet)
 * 7: ReqDestinations: Request Packet containing destinations (used by SimBet)
 * 8: DPs: Packet containing the Destination Predictability values (used by Prophet)
 * 9: IntiFreq: Inti-freq packet Containing the (normalized) metrics: Frequency and intimacy
 * 10: DstUtils: Destinations PACKET containing destination-Util values (used by LSFSpray)
 * 11: DstAvail: Available for specific Destinations Packet (used by LSFSpray)
 * 12: SocialSummary: Summary packet + Util (DI-Destination Independent) (used by MSFSpray)
 * 13: RequestPacketUtils: Request packet + utility that won
 * 14: DPsEnhanced: DPs packet containing the Destination Predictability values + 1 hop contacts
 * 15: PktMultiUtils: Packet IDs and Multiple Utility Values
 * 16: MarkedRequestPacket: Request packet + some of them are marked (eg. hold a property)
 * 17: EnhancedSummaryPacket: Packet IDs and their current number of hops
 * 18: NOT IN USE ANYMORE
 * 19: DirectSummaryPacket: Summary packet for direct delivery packets (used by all multi-copy algorithms)
 * 20: DirectRequestPacket: Request packet for direct delivery packets (used by all multi-copy algorithms)
 * 21: NOT IN USE ANYMORE
 * 22: NOT IN USE ANYMORE
 * 23: AntiPacket: An AntiPacket (potential used by all algorithms)
 * 24: AntiPacketResponse: An AntiPacket response (potential used by all algorithms)
 * 25: PktUtils: Packet IDs + Utility per packet (used by Delegation Forwarding)
 * 26: ReqLCandFS: Request packet Familiar Set and Local Community (used by Bubble Rap)
 * 27: LCandFS: Packet containing Familiar Set and Local Community (used by Bubble Rap)
 * 28: PktDests: Packet IDs + Destination per packet (used by Delegation Forwarding)
 * 29: EBRequest: Request Packet IDs + Current utility of the node (encounter value) - used by EBR
 * 30: BubbleSummary: Packet containing bubble information and summary vector (used by Bubble Rap)
 * 31: REQ_RSPM: Request the Indirect SPM values (Friendship based routing)
 * 32: IND_RSPM: Packet containing destinations and RSPM values that can be reached through the sender of the packet (Friendship based routing)
 * 33: MultUtils: Packet containing Utilities both Destination Depended and destination Independent along with packet ids 
 * 34: PktUpd: Contains utility for sended packet, used for algorithms that copy packets even if their util is lower(not delegation)
 * 35: AckPack: Packet containing ids of packets received
 * 36: Encoded: Network coding related packets
 * 37: ActivationPacket: Information about packets that need to be activated by the receipting node (it stores the packets in an encoded form)
 * 38: EnRequestPacket: 
 * 39: BufferRequest: Request for the receiver's buffer status
 * 40: BufferReply: Information about the sender's buffer status
 * ------------------------------------------
 * Note: Please keep the above list updated. |
 * ------------------------------------------
 */

Header *CopyHeader(Header *old)
{
	Header *h=NULL;
	switch(old->GetType())
	{
		case 1:
			h=new SimpleHeader(old->GetSource(),old->GetDestination(),old->GetprevHop());
			h->SetHeaderSize(old->GetHeaderSize());
			h->SetRep(1);
			h->SetDeliveryStatus(old->GetDeliveryStatus());
			break;
		case 2:
			h=new BasicHeader(old->GetSource(),old->GetDestination());
			h->SetHeaderSize(old->GetHeaderSize());
			h->SetDeliveryStatus(old->GetDeliveryStatus());
			break;
	}
	return h;
}

Packet::Packet(double Time,int PID)
{
	this->UniqueID=PID;
	this->StartTime=Time;
	this->HD=NULL;
	this->extraInfo=NULL;
	this->SubType=0;
}

Packet::~Packet(void )
{
	//printf("Destructor of Packet is called!\n");
	return;
}


void Packet::setSize(int PSize)
{
	this->Pktsize=PSize;
	return;
}


void Packet::setExtraInfo(void *info)
{
	this->extraInfo = info;
}


void *Packet::getExtraInfo()
{
	return this->extraInfo;
}


void Packet::PrintPkt(void )
{
	printf("Time:%f\tPkt ID:%d\n",this->StartTime,this->UniqueID);
	return;
}

DataPacket::DataPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=DATA_PACKET;
	this->Pktsize=256;
	this->access=0;
	return;
}

DataPacket::~DataPacket(void)
{
// 	printf("Data packet %d is being deleted!\n",this->UniqueID);
	delete HD;
	return;
}


bool DataPacket::AccessPkt(void)
{
	if(this->HD->IsDuplicate() == true)
	{
		this->access--;
		if(this->access == 0)
		{
			//This packet must be deleted - no future use
			return false;
		}
	}
	return true;
}

void DataPacket::SetRecipients(int N)
{
	this->access=N;
	return;
}

Packet *DataPacket::Duplicate(int hops)
{
	if(this->HD == NULL)
	{
		printf("Cannot duplicate a packet without header!\n");
		exit(1);
	}
	Packet *p=new DataPacket(this->StartTime,0);
	Header *h=CopyHeader(this->HD);

	//Check if original packet is a duplicate too
	if(this->HD->IsDuplicate() == true)
	{
		h->SetOriginal(this->HD->GetOriginal());
	}
	else
	{
		h->SetOriginal(this->getID());
	}
	h->SetHops(hops+1);
	p->setHeader(h);
	return p;
}


Encoded::Encoded(double Time, int PID): Packet(Time, PID)
{
	this->Type=ENCODED_PACKET;
	this->Pktsize=256;
	this->access=0;
	this->mimic=-1;
	this->PIDs=NULL;
	this->RepVals=NULL;
	return;
}

Encoded::~Encoded(void )
{
	delete HD;
	free(this->PIDs);
	free(this->RepVals);
	return;
}


bool Encoded::AccessPkt(void)
{
	if(this->HD->IsDuplicate() == true)
	{
		this->access--;
		if(this->access == 0)
		{
			//This packet must be deleted - no future use
			return false;
		}
	}
	return true;
}

void Encoded::SetRecipients(int N)
{
	this->access=N;
	return;
}


void Encoded::setContents(void* data)
{
	this->PIDs=(int *)data;
	return;
}

void* Encoded::getContents(void )
{
	return (void *)this->PIDs;
}


Packet *Encoded::Duplicate(int hops)
{
	return NULL;
}


SummaryPacket::SummaryPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=SUMMARY_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	return;
}

SummaryPacket::~SummaryPacket(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void SummaryPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* SummaryPacket::getContents(void )
{
	return (void *)this->IDs;
}


EnhancedSummaryPacket::EnhancedSummaryPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type = ENHANCED_SUMMARY_PACKET;
	this->Pktsize = 30;
	this->pktData = NULL;

	return;
}


EnhancedSummaryPacket::~EnhancedSummaryPacket()
{
	delete HD;
	free(pktData);

	return;
}


void EnhancedSummaryPacket::setContents(void* data)
{
	this->pktData = (struct PktIDandHops *) data;

	return;
}


void *EnhancedSummaryPacket::getContents()
{
	return (void *) this->pktData;
}


ActivationPacket::ActivationPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=ACTIVATION_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	this->RVs=NULL;
	return;
}

ActivationPacket::~ActivationPacket(void )
{
	delete HD;
	free(this->IDs);
	free(this->RVs);
	return;
}


void ActivationPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* ActivationPacket::getContents(void )
{
	return (void *)this->IDs;
}

DirectSummaryPacket::DirectSummaryPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=DIRECT_SUMMARY_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	return;
}

DirectSummaryPacket::~DirectSummaryPacket(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void DirectSummaryPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* DirectSummaryPacket::getContents(void)
{
	return (void *)this->IDs;
}


SocialSummary::SocialSummary(double Time, int PID): Packet(Time, PID)
{
	this->Type=UTIL_SUMMARY_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	return;
}

SocialSummary::~SocialSummary(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void SocialSummary::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* SocialSummary::getContents(void )
{
	return (void *)this->IDs;
}

EBRequest::EBRequest(double Time, int PID): Packet(Time, PID)
{
	this->Type=EBR_REQ;
	this->Pktsize=30;
	this->IDs=NULL;
	return;
}

EBRequest::~EBRequest(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void EBRequest::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* EBRequest::getContents(void )
{
	return (void *)this->IDs;
}


RequestPacket::RequestPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQUEST_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	return;
}

RequestPacket::~RequestPacket(void )
{
	delete HD;
	free(this->IDs);
	return;
}

void* RequestPacket::getContents(void )
{
	return (void *)this->IDs;
}

void RequestPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}


MarkedRequestPacket::MarkedRequestPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=MARKED_REQUEST_PKTS;
	this->Pktsize=30;
	this->IDs=NULL;
	this->marked=NULL;
	return;
}

MarkedRequestPacket::~MarkedRequestPacket(void )
{
	delete HD;
	free(this->IDs);
	free(this->marked);
	return;
}

void *MarkedRequestPacket::getContents(void )
{
	return (void *)this->IDs;
}

void MarkedRequestPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

EnRequestPacket::EnRequestPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=ENHANCED_REQUEST_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	this->Extras=NULL;
	return;
}

EnRequestPacket::~EnRequestPacket(void )
{
	delete HD;
	free(this->IDs);
	free(this->Extras);
	return;
}

void* EnRequestPacket::getContents(void )
{
	return (void *)this->IDs;
}

void EnRequestPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

DirectRequestPacket::DirectRequestPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=DIRECT_REQUEST_PACKET;
	this->Pktsize=30;
	this->IDs=NULL;
	return;
}

DirectRequestPacket::~DirectRequestPacket(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void* DirectRequestPacket::getContents(void )
{
	return (void *)this->IDs;
}

void DirectRequestPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

ReqContacts::ReqContacts(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQUEST_CONTACTS_PACKET;
	this->Pktsize=20;
	return;
}

ReqContacts::~ReqContacts(void )
{
	delete HD;
	return;
}

Contacts::Contacts(double Time, int PID): Packet(Time, PID)
{
	this->Type = CONTACTS_PACKET;
	this->Pktsize = 20;
	this->NodeIDs = NULL;
	return;
}

Contacts::~Contacts(void)
{
	delete HD;
	free(this->NodeIDs);
	return;
}

void Contacts::setContents(void* data)
{
	this->NodeIDs = (int *)data;
	return;
}


void* Contacts::getContents(void )
{
	return (void *)this->NodeIDs;
}


DstAvail::DstAvail(double Time, int PID): Packet(Time, PID)
{
	this->Type = AVAIL_DEST_PACKET;
	this->Pktsize = 20;
	this->NodeIDs = NULL;
	return;
}

DstAvail::~DstAvail(void )
{
	delete HD;
	free(this->NodeIDs);
	return;
}

void DstAvail::setContents(void* data)
{
	this->NodeIDs=(int *)data;
	return;
}


void* DstAvail::getContents(void )
{
	return (void *)this->NodeIDs;
}

PktDests::PktDests(double Time, int PID): Packet(Time, PID)
{
	this->Type=PKTS_DESTS;
	this->Pktsize=30;
	this->PktInfo=NULL;
	return;
}

PktDests::~PktDests(void )
{
	delete HD;
	free((struct PktDest *)this->PktInfo);
	return;
}

void PktDests::setContents(void *data)
{
	this->PktInfo=(struct PktDest *)data;
}

void* PktDests::getContents(void )
{
	return (void *)this->PktInfo;
}

PktUtils::PktUtils(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQUEST_PKTS_UTILS;
	this->Pktsize=30;
	this->PktInfo=NULL;
	return;
}

PktUtils::~PktUtils(void )
{
	delete HD;
	free((struct PktUtil *)this->PktInfo);
	return;
}

void PktUtils::setContents(void* data)
{
	this->PktInfo=(struct PktUtil *)data;
}

void *PktUtils::getContents(void )
{
	return (void *)this->PktInfo;
}

PktMultiUtils::PktMultiUtils(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQUEST_PKTS_MULTI_UTILS;
	this->Pktsize=30;
	this->PktInfo=NULL;
	return;
}

PktMultiUtils::~PktMultiUtils(void )
{
	delete HD;
	free((struct PktMultiUtil *)this->PktInfo);
	return;
}

void PktMultiUtils::setContents(void* data)
{
	this->PktInfo=(struct PktMultiUtil *)data;
}

void *PktMultiUtils::getContents(void )
{
	return (void *)this->PktInfo;
}

DPs::DPs(double Time, int PID): Packet(Time, PID)
{
	this->Type=DEST_PREDICT_PACKET;
	this->Pktsize=20;
	DP=NULL;
	return;
}

DPs::~DPs(void )
{
	delete HD;
	free(DP);
	return;
}

void DPs::setContents(void* data)
{
	this->DP=(double *)data;
	return;
}

void* DPs::getContents(void)
{
	return (void *)this->DP;
}


DPsEnhanced::DPsEnhanced(double Time, int PID): Packet(Time, PID)
{
	this->Type=DPV_CONTACTS_PACKET;
	this->Pktsize=20;
	this->DP=NULL;
	this->IDs=NULL;
	this->DirectDP=NULL;
	return;
}

DPsEnhanced::~DPsEnhanced(void )
{
	delete HD;
	free(this->DP);
	free(this->IDs);
	free(this->DirectDP);
	return;
}

void DPsEnhanced::setContents(void* data)
{
	this->DP=(double *)data;
	return;
}

void* DPsEnhanced::getContents(void)
{
	return (void *)this->DP;
}

void DPsEnhanced::setIDs(int* data)
{
	this->IDs=data;
	return;
}


int* DPsEnhanced::getIDs(void )
{
	return this->IDs;
}

double* DPsEnhanced::getDirectDPs(void )
{
	return this->DirectDP;
}

void DPsEnhanced::setDirectDPs(double* data)
{
	this->DirectDP=data;
}


IntiFreq::IntiFreq(double Time, int PID): Packet(Time, PID)
{
	this->Type=INTIM_FREQ_PACKET;
	this->Pktsize=20;
	this->FreqIntimacy=NULL;
	return;
}

IntiFreq::~IntiFreq(void )
{
	delete HD;
	free(this->FreqIntimacy);
	return;
}

void IntiFreq::setContents(void* data)
{
	this->FreqIntimacy=(struct ConFreqIntim*)data;
	return;
}

void* IntiFreq::getContents(void)
{
	return (void *)this->FreqIntimacy;
}

Destinations::Destinations(double Time, int PID): Packet(Time, PID)
{
	this->Type=DEST_PACKET;
	this->Pktsize=30;
	this->DestInfo=NULL;
	return;
}

Destinations::~Destinations(void )
{
	delete HD;
	free(this->DestInfo);
	return;
}


void Destinations::setContents(void* data)
{
	this->DestInfo=(struct DestSim *)data;
}

void* Destinations::getContents(void )
{
	return (void *)this->DestInfo;
}

void Destinations::PrintContents(void )
{
	printf("Betweenness is %f\n",this->Betweenness);
	for(int i=0;i<this->NumDest;i++)
	{
		printf("For destination %d Similarity is %f and TieStrength is %f\n",this->DestInfo[i].Dest,this->DestInfo[i].Sim,this->DestInfo[i].TieStrength);
	}
	return;
}

DstUtils::DstUtils(double Time, int PID): Packet(Time, PID)
{
	this->Type=UTIL_DEST_PACKET;
	this->Pktsize=30;
	this->DestInfo=NULL;
	return;
}

DstUtils::~DstUtils(void )
{
	delete HD;
	free(this->DestInfo);
	return;
}

void DstUtils::setContents(void* data)
{
	this->DestInfo=(struct DestUtil *)data;
}

void* DstUtils::getContents(void )
{
	return (void *)this->DestInfo;
}


RequestPacketUtils::RequestPacketUtils(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQUEST_UTIL_WON_PACKET;
	this->Pktsize=30;
	this->DestInfo=NULL;
	return;
}

RequestPacketUtils::~RequestPacketUtils(void )
{
	delete HD;
	free(this->DestInfo);
	return;
}

void RequestPacketUtils::setContents(void* data)
{
	this->DestInfo=(struct DestUtil *)data;
}

void* RequestPacketUtils::getContents(void )
{
	return (void *)this->DestInfo;
}


ReqDestinations::ReqDestinations(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQ_DEST_PACKET;
	this->Pktsize=30;
	this->pktsForDest=NULL;
	this->AlreadyOwned=NULL;
	this->senderUtils=NULL;
	this->receiverUtils=NULL;

	return;
}


ReqDestinations::~ReqDestinations(void )
{
	delete HD;
	free(this->pktsForDest);
	free(this->AlreadyOwned);
	free(this->senderUtils);
	free(this->receiverUtils);

	return;
}


void ReqDestinations::setContents(void* data)
{
	this->pktsForDest=(int *)data;
	return;
}


void* ReqDestinations::getContents(void )
{
	return (void *)this->pktsForDest;
}


void ReqDestinations::SetOwned(int* p)
{
	this->AlreadyOwned=p;
	return;
}


int* ReqDestinations::GetOwned(void)
{
	return this->AlreadyOwned;
}


void ReqDestinations::setSenderUtils(double *U)
{
	this->senderUtils = U;
}


double *ReqDestinations::getSenderUtils()
{
	return this->senderUtils;
}


void ReqDestinations::setReceiverUtils(double *U)
{
	this->receiverUtils = U;
}


double *ReqDestinations::getReceiverUtils()
{
	return this->receiverUtils;
}


ReqLCandFS::ReqLCandFS(double Time, int PID): Packet(Time, PID)
{
	this->Type = REQUEST_LC_FS;
	this->Pktsize = 20;

	return;
}

ReqLCandFS::~ReqLCandFS(void )
{
	delete HD;
	return;
}


LCandFS::LCandFS(double Time, bool *LC, bool **FS, int PID): Packet(Time, PID)
{
	this->Type = LC_FS;
	this->Pktsize = 20;
	this->localCommunity = LC;
	this->familiarSets = FS;

	maxN=0;
	return;
}

LCandFS::~LCandFS(void)
{
	delete HD;
	for(int i = 0; i < maxN; i++)
	{
		free(this->familiarSets[i]);
	}
	free(this->familiarSets);
	free(this->localCommunity);
	return;
}


BubbleSummary::BubbleSummary(double Time, int numPkts, struct PktDest* sumVec, bool *LC, double LR, double GR, int PID): Packet(Time, PID)
{
	this->Type = BUBBLE_SUMMARY;
	this->Pktsize = 30;
	this->numPackets = numPkts;
	this->summaryVector = sumVec;
	this->localCommunity = LC;
	this->localRank = LR;
	this->globalRank = GR;

	return;
}


BubbleSummary::~BubbleSummary(void)
{
	delete HD;
	free(this->summaryVector);
	free(this->localCommunity);
	return;
}


AntiPacket::AntiPacket(double Time, int PID): Packet(Time, PID)
{
	this->Type=ANTIPACKET;
	this->Pktsize=10;
	this->IDs=NULL;
	return;
}

AntiPacket::~AntiPacket(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void AntiPacket::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* AntiPacket::getContents(void )
{
	return (void *)this->IDs;
}

AntiPacketResponse::AntiPacketResponse(double Time, int PID): Packet(Time, PID)
{
	this->Type=ANTIPACKET_RESPONSE;
	this->Pktsize=10;
	this->IDs=NULL;
	return;
}

AntiPacketResponse::~AntiPacketResponse(void )
{
	delete HD;
	free(this->IDs);
	return;
}


void AntiPacketResponse::setContents(void* data)
{
	this->IDs=(int *)data;
	return;
}

void* AntiPacketResponse::getContents(void )
{
	return (void *)this->IDs;
}

ReqRSPM::ReqRSPM(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQ_RSPM;
	this->Pktsize=20;
	return;
}

ReqRSPM::~ReqRSPM(void )
{
	delete HD;
	return;
}

IndRSPM::IndRSPM(double Time, int PID): Packet(Time, PID)
{
	this->Type = IND_RSPM;
	this->Pktsize = 20;
	this->Dests = NULL;
	return;
}

IndRSPM::~IndRSPM(void)
{
	delete HD;
	free(this->Dests);
	return;
}

void IndRSPM::setContents(void* data)
{
	this->Dests = (double *)data;
	return;
}

void* IndRSPM::getContents(void )
{
	return (void *)this->Dests;
}

BufferRequest::BufferRequest(double Time, int PID): Packet(Time, PID)
{
	this->Type=REQUEST_BUFFER_INFO;
	this->Pktsize=10;
	return;
}

BufferRequest::~BufferRequest(void )
{
	delete HD;
	return;
}


BufferReply::BufferReply(double Time, int PID): Packet(Time, PID)
{
	this->Type=BUFFER_INFO;
	this->Pktsize=10;
	this->bufInfo=NULL;
	return;
}

BufferReply::~BufferReply(void )
{
	delete HD;
	free(this->bufInfo);
	return;
}


void BufferReply::setContents(void *data)
{
	this->bufInfo=(struct BufferInformation *)data;
	return;
}

void *BufferReply::getContents(void )
{
	return (void *)this->bufInfo;
}

PktUpd::PktUpd(double Time, int PID): Packet(Time, PID)
{
	this->Type=PKTUPD;
	this->Pktsize=30;
	this->PktInfo=NULL;
	return;
}

PktUpd::~PktUpd(void )
{
	delete HD;
	free((struct PktUtilDDandDI *)this->PktInfo);
	return;
}

void PktUpd::setContents(void* data)
{
	this->PktInfo=(struct PktUtilDDandDI *)data;
}

void* PktUpd::getContents(void )
{
	return (void *)this->PktInfo;
}

MultUtils::MultUtils(double Time, int PID): Packet(Time, PID)
{
	this->Type=MULTUTILS;
	this->Pktsize=30;
	this->PktInfo=NULL;
	return;
}

MultUtils::~MultUtils(void )
{
	delete HD;
	free((struct PktUtilDDandDI *)this->PktInfo);
	return;
}

void MultUtils::setContents(void* data)
{
	this->PktInfo=(struct PktUtilDDandDI *)data;
}

void* MultUtils::getContents(void )
{
	return (void *)this->PktInfo;
}

AckPack::AckPack(double Time, int PID): Packet(Time, PID)
{
	this->Type=ACKPACK;
	this->Pktsize=20;
	this->PktInfo=NULL;
	return;
}
AckPack::~AckPack(void )
{
	delete HD;
	free((struct Kouf_util *)this->PktInfo);
	return;
}

void AckPack::setContents(void* data)
{
	this->PktInfo=(struct PktUtilDDandDI *)data;
}
void* AckPack::getContents(void )
{
	return (void *)this->PktInfo;
}
