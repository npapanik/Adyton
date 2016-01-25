/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
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


#include <stdlib.h>
#include <stdio.h>

#ifndef HEADER_H
	#define HEADER_H
	#include "Header.h"
#endif


#define DATA_PACKET 1
#define SUMMARY_PACKET 2
#define REQUEST_PACKET 3
#define REQUEST_CONTACTS_PACKET 4
#define CONTACTS_PACKET 5
#define DEST_PACKET 6
#define REQ_DEST_PACKET 7
#define DEST_PREDICT_PACKET 8
#define INTIM_FREQ_PACKET 9
#define UTIL_DEST_PACKET 10
#define AVAIL_DEST_PACKET 11
#define UTIL_SUMMARY_PACKET 12
#define REQUEST_UTIL_WON_PACKET 13
#define DPV_CONTACTS_PACKET 14
#define REQUEST_PKTS_MULTI_UTILS 15
#define MARKED_REQUEST_PKTS 16
#define ENHANCED_SUMMARY_PACKET 17
//empty slot 18
#define DIRECT_SUMMARY_PACKET 19
#define DIRECT_REQUEST_PACKET 20
//empty slot 21
//empty slot 22
#define ANTIPACKET 23
#define ANTIPACKET_RESPONSE 24
#define REQUEST_PKTS_UTILS 25
#define REQUEST_LC_FS 26
#define LC_FS 27
#define PKTS_DESTS 28
#define EBR_REQ 29
#define BUBBLE_SUMMARY 30
#define REQ_RSPM 31
#define IND_RSPM 32
#define MULTUTILS 33
#define PKTUPD 34
#define ACKPACK 35
#define ENCODED_PACKET 36
#define ACTIVATION_PACKET 37
#define ENHANCED_REQUEST_PACKET 38
#define REQUEST_BUFFER_INFO 39
#define BUFFER_INFO 40


struct DestSim
{
	int Dest;
	double Sim;
	double TieStrength;
};

struct ConFreqIntim
{
	int Contact;
	double Intimacy;
	double Frequency;
};

struct DestUtil
{
	int Dest;
	double Util;
};


struct PktIDandHops
{
	int pktID;
	int pktHops;
};



struct PktUtil
{
	int PID;
	double Util;
	bool Exists;
	int mode;//used by coding based algorithms(0:Exists as Native, 1:Exists as Encoded)
};

struct PktMultiUtil
{
	int PID;
	double Sim;
	double Bet;
	double Frequency;
	double Intimacy;
	double Recency;
	bool Exists;
	int mode;//used by coding based algorithms(0:Exists as Native, 1:Exists as Encoded)
};

struct PktUtilDDandDI
{
	int PID;
	double DDUtil;
	double DIUtil;
	bool Exists;
};

struct Ackpacks
{
	int PID;
	int Dest;
	double Util;
};

struct PktDest
{
	int PID;
	int Dest;
};


struct BufferInformation
{
	int bSize;
	int bLength;
};
/* Packet
 * ------
 * Class that supports basic packet handling
 */
class Packet
{
protected:
	int UniqueID;
	int Type;
	int SubType;
	int Pktsize;//in Bytes
	double StartTime;
	Header *HD;
	void *extraInfo;
public:
	Packet(double Time,int PID);
	virtual ~Packet(void);
	virtual void setContents(void *data)=0;
	virtual void *getContents(void)=0;
	void setSize(int PSize);
	int getSize(void){return this->Pktsize;};
	int getID(void){return this->UniqueID;};
	void setID(int ID){this->UniqueID=ID;};
	int getType(void){return this->Type;};
	void PrintPkt(void);
	void setHeader(Header *head){this->HD=head;};
	Header *getHeader(void){return this->HD;};
	void setExtraInfo(void *info);
	void *getExtraInfo();
	virtual Packet *Duplicate(int hops)=0;
	virtual bool AccessPkt(void)=0;
	virtual void SetRecipients(int N){return;};
	virtual double GetStartTime(void){return this->StartTime;};
	virtual void SetSubType(int sub){this->SubType=sub;};
	virtual int GetSubType(void){return this->SubType;};
	virtual int getAvailSpace(void){return 0;};
	virtual double *getQWvalues(void){return NULL;};
};

/* DataPacket
 * ----------
 * Class for data packets. Contents are NULL
 */
class DataPacket:public Packet
{
protected:
	int access;
public:
	DataPacket(double Time, int PID);
	~DataPacket(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops);
	virtual bool AccessPkt(void);
	virtual void SetRecipients(int N);
};

/* SummaryPacket
 * -------------
 * This packet type holds information about packets that 
 * one node holds in its packet buffer
 */
class SummaryPacket:public Packet
{
protected:
	int *IDs;
public:
	SummaryPacket(double Time, int PID);
	~SummaryPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* EnhancedSummaryPacket
 * -------------
 * This packet type holds information about packets that 
 * one node holds in its packet buffer and their current
 * number of hops.
 */
class EnhancedSummaryPacket: public Packet
{
protected:
	struct PktIDandHops *pktData;
public:
	EnhancedSummaryPacket(double Time, int PID);
	~EnhancedSummaryPacket();
	virtual void setContents(void *data);
	virtual void *getContents();
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(){return true;};
};

/* DirectSummaryPacket
 * -------------
 * This packet type holds information about packets that 
 * one node holds in its packet buffer - this packet are
 * destined to the node in contact
 */
class DirectSummaryPacket:public Packet
{
protected:
	int *IDs;
public:
	DirectSummaryPacket(double Time, int PID);
	~DirectSummaryPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};



class SocialSummary:public Packet
{
protected:
	int *IDs;
	double SocialMetric;
public:
	SocialSummary(double Time, int PID);
	~SocialSummary(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void SetSocialMetric(double val){this->SocialMetric=val;};
	double GetSocialMetric(void){return this->SocialMetric;};
};

/* RequestPacket
 * -------------
 * This packet type holds information about packets that
 * are missing from a node's packet buffer
 * 
 * Note: Currently the implementation is the same as
 * SummaryPacket. In future this can be changed to 
 * support priority packet requests.
 */
class RequestPacket:public Packet
{
protected:
	int *IDs;
public:
	RequestPacket(double Time, int PID);
	~RequestPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* MarkedRequestPacket
 * -------------
 * This packet type holds information about packets that
 * are missing from a node's packet buffer. Furthermore,
 * this packet type contains a boolean value for each packet
 * that can be used to mark a subset of packets.
 */
class MarkedRequestPacket:public Packet
{
protected:
	int *IDs;
	bool *marked;
public:
	MarkedRequestPacket(double Time, int PID);
	~MarkedRequestPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setMarked(bool *mark){this->marked=mark;};
	bool *getMarked(void){return this->marked;};
};

/* DirectRequestPacket
 * -------------
 * This packet type holds information about packets that
 * are missing from a node's packet buffer and is used as
 * the response to the reception of a DirectSummaryPacket 
 * 
 * Note: Currently the implementation is the same as
 * DirectSummaryPacket. In future this can be changed to 
 * support priority packet requests.
 */
class DirectRequestPacket:public Packet
{
protected:
	int *IDs;
public:
	DirectRequestPacket(double Time, int PID);
	~DirectRequestPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};


class ReqContacts:public Packet
{
public:
	ReqContacts(double Time, int PID);
	~ReqContacts(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};


class Contacts:public Packet
{
protected:
	int *NodeIDs;
public:
	Contacts(double Time, int PID);
	~Contacts(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

class DstAvail:public Packet
{
protected:
	int *NodeIDs;
public:
	DstAvail(double Time, int PID);
	~DstAvail(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

class DPs:public Packet
{
protected:
	double *DP;
public:
	DPs(double Time, int PID);
	~DPs(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

class DPsEnhanced:public Packet
{
protected:
	double *DP;
	int *IDs;
	double *DirectDP;
public:
	DPsEnhanced(double Time, int PID);
	~DPsEnhanced(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setIDs(int *data);
	int *getIDs(void);
	void setDirectDPs(double *data);
	double *getDirectDPs(void);
};

class IntiFreq:public Packet
{
protected:
	int NumContacts;
	struct ConFreqIntim *FreqIntimacy;
public:
	IntiFreq(double Time, int PID);
	~IntiFreq(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setCon(int num){this->NumContacts=num;};
	int getCon(void){return this->NumContacts;};
};

class Destinations:public Packet
{
protected:
	int NumDest;
	struct DestSim *DestInfo;
	double Betweenness;
public:
	Destinations(double Time, int PID);
	~Destinations(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setBet(double bet){this->Betweenness=bet;};
	double getBet(void){return this->Betweenness;};
	void setDest(int num){this->NumDest=num;};
	int getDest(void){return this->NumDest;};
	void PrintContents(void);
};

class DstUtils:public Packet
{
protected:
	int NumDest;
	struct DestUtil *DestInfo;
public:
	 DstUtils(double Time, int PID);
	~DstUtils(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setDestNum(int num){this->NumDest=num;};
	int getDestNum(void){return this->NumDest;};
	void PrintContents(void){return;};
};

class RequestPacketUtils:public Packet
{
protected:
	int NumDest;
	struct DestUtil *DestInfo;
public:
	RequestPacketUtils(double Time, int PID);
	~RequestPacketUtils(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setDestNum(int num){this->NumDest=num;};
	int getDestNum(void){return this->NumDest;};
	void PrintContents(void){return;};
};

class ReqDestinations:public Packet
{
protected:
	int *pktsForDest;
	int *AlreadyOwned;
	double *senderUtils;
	double *receiverUtils;

public:
	ReqDestinations(double Time, int PID);
	~ReqDestinations(void);
	
	virtual void setContents(void *data);
	virtual void *getContents(void);
	
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void SetOwned(int *p);
	int *GetOwned(void);
	void setSenderUtils(double *U);
	double *getSenderUtils();
	void setReceiverUtils(double *U);
	double *getReceiverUtils();
};


class ReqLCandFS:public Packet
{
public:
	ReqLCandFS(double Time, int PID);
	~ReqLCandFS(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};


class LCandFS:public Packet
{
protected:
	bool *localCommunity;
	bool **familiarSets;
	int maxN;
public:
	LCandFS(double Time, bool *LC, bool **FS, int PID);
	~LCandFS(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};

	bool *getLocalCommunity(void){return this->localCommunity;};
	bool **getFamiliarSets(void){return this->familiarSets;};
	
	void setmaxN(int N){this->maxN=N;};
};


class BubbleSummary:public Packet
{
protected:
	int numPackets;
	struct PktDest *summaryVector;
	bool *localCommunity;
	double localRank;
	double globalRank;
public:
	BubbleSummary(double Time, int numPkts, struct PktDest* sumVec, bool *LC, double LR, double GR, int PID);
	~BubbleSummary(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};

	int getNumPackets(void){return this->numPackets;};
	struct PktDest *getSummaryVector(void){return this->summaryVector;};
	bool *getLocalCommunity(void){return this->localCommunity;};
	double getLocalRank(void){return this->localRank;};
	double getGlobalRank(void){return this->globalRank;};
};


/* PktDests
 * --------
 * This packet type holds information about the packets that a node 
 * currently owns. In addition to packet IDs the packet contains its 
 * destination.
 */
class PktDests:public Packet
{
protected:
	int NumPkts;
	struct PktDest *PktInfo;
public:
	PktDests(double Time, int PID);
	~PktDests(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setPktNum(int num){this->NumPkts=num;};
	int getPktNum(void){return this->NumPkts;};
	void PrintContents(void){return;};
};

/* PktUtils
 * --------
 * This packet type holds information about the packets that a node 
 * requests (it is a request packet in response to a summary packet).
 * In addition to packet IDs the packet contains a utility per packet
 * that corresponds to the node's ability to deliver the packet. The 
 * utility can be either DI (Destination Independent) or DD (Destination
 * Dependent) 
 */
class PktUtils:public Packet
{
protected:
	int NumPkts;
	struct PktUtil *PktInfo;
	double DIutil;
public:
	PktUtils(double Time, int PID);
	~PktUtils(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setPktNum(int num){this->NumPkts=num;};
	int getPktNum(void){return this->NumPkts;};
	void PrintContents(void){return;};
	void setDIutil(double u){this->DIutil=u;};
	double getDIutil(void){return this->DIutil;};
};


/* PktMultiUtils
 * -------------
 * This packet type holds information about the packets that a node 
 * requests (it is a request packet in response to a summary packet).
 * In addition to packet IDs the packet contains a list  of utilities 
 * per packet: 
 * 1. Similarity
 * 2. Ego Betweenness centrality
 * 3. Recency
 * 4. Intimacy
 * 5. Frequency
 * All utilities correspond to the node's ability to deliver the packet.
 */
class PktMultiUtils:public Packet
{
protected:
	int NumPkts;
	struct PktMultiUtil *PktInfo;
public:
	PktMultiUtils(double Time, int PID);
	~PktMultiUtils(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setPktNum(int num){this->NumPkts=num;};
	int getPktNum(void){return this->NumPkts;};
	void PrintContents(void){return;};
};

/* EBRequest
 * ---------
 * Used by: The EBR algorithm.
 * This packet type holds the IDs of packets that a node requests from
 * another node. It is used as a response to a summary packet. In addition
 * to packet IDs the encounter utility value of the node is also inserted to
 * the packet. This value will be used by the other node in order to estimate
 * the proper amount of replicas to send. 
 */

class EBRequest:public Packet
{
protected:
	int *IDs;
	double Encounters;
public:
	EBRequest(double Time, int PID);
	~EBRequest(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void SetEncounters(double val){this->Encounters=val;};
	double GetEncounters(void){return this->Encounters;};
};

/* AntiPacket
 * ----------
 * This packet type holds information about the packets that have been
 * delivered to their destinations (and the sender knows)
 */
class AntiPacket:public Packet
{
protected:
	int *IDs;
public:
	AntiPacket(double Time, int PID);
	~AntiPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* AntiPacketResponse
 * -------------
 * This packet type holds information about the packets that have been
 * delivered to their destinations (and the sender knows). It is used as
 * a response to an antipacket reception.
 */
class AntiPacketResponse:public Packet
{
protected:
	int *IDs;
public:
	AntiPacketResponse(double Time, int PID);
	~AntiPacketResponse(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* ReqRSPM
 * -------------
 * An empty packet just to ask for indirect RSPM values.
 */
class ReqRSPM:public Packet
{
public:
	ReqRSPM(double Time, int PID);
	~ReqRSPM(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* IndRSPM
 * -------------
 * This packet type holds information about the destinations for which the sender
 * can be used as a relay node. Also, for the above destination the RSPM value is
 * given (Friendship based routing)
 */
class IndRSPM:public Packet
{
protected:
	double *Dests;
public:
	IndRSPM(double Time, int PID);
	~IndRSPM(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* BufferRequest
 * -------------
 * This packet requests for information about the receiver's
 * buffer status
 */
class BufferRequest:public Packet
{
public:
	BufferRequest(double Time, int PID);
	~BufferRequest(void);
	virtual void setContents(void *data){return;};
	virtual void *getContents(void){return NULL;};
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* BufferReply
 * -------------
 * This packet holds information about the buffer status of the
 * sending node
 */
class BufferReply:public Packet
{
protected:
	struct BufferInformation *bufInfo;
public:
	BufferReply(double Time, int PID);
	~BufferReply(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};


//--- FUTURE: Network coding related packets --
/* Encoded
 * ----------
 * Class for encoded data packets
 */
class Encoded:public Packet
{
protected:
	int access;
	int *PIDs;
	int *RepVals;
	int CodingDepth;
	int mimic;
public:
	Encoded(double Time, int PID);
	~Encoded(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops);
	virtual bool AccessPkt(void);
	virtual void SetRecipients(int N);
	void setCodingDepth(int cd){this->CodingDepth=cd;};
	int getCodingDepth(void){return this->CodingDepth;};
	void setMimic(int pos){this->mimic=pos;};
	int getMimic(void){return this->mimic;};
	void setRep(int *RVs){this->RepVals=RVs;};
	int *getRep(void){return this->RepVals;};
};

class EnRequestPacket:public Packet
{
protected:
	int *IDs;
	bool *Extras;
public:
	EnRequestPacket(double Time, int PID);
	~EnRequestPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	bool *getExtraContents(void){return Extras;};
	void setExtraContents(bool *extras){this->Extras=extras;};
};


/* ActivationPacket
 * -------------
 * This packet type holds information about packets that need to be activated  
 * by the receipting node (it stores the packets in an encoded form)
 */
class ActivationPacket:public Packet
{
protected:
	int *IDs;
	int *RVs;
public:
	ActivationPacket(double Time, int PID);
	~ActivationPacket(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	int *getRep(void){return this->RVs;};
	void setRep(int *Reps){this->RVs=Reps;};
};


class PktUpd:public Packet
{
protected:
	int NumPkts;
	struct PktUtilDDandDI *PktInfo;
public:
	PktUpd(double Time, int PID);
	~PktUpd(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setPktNum(int num){this->NumPkts=num;};
	int getPktNum(void){return this->NumPkts;};
	void PrintContents(void){return;};
};

class AckPack:public Packet
{
protected:
	
	struct PktUtilDDandDI *PktInfo;
public:
	AckPack(double Time, int PID);
	~AckPack(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
};

/* MultUtils
 * ---------
 * This packet type holds information about the packets that a node 
 * requests (it is a request packet in response to a summary packet).
 * In addition to packet IDs the packet contains a utility per packet
 * that corresponds to the node's ability to deliver the packet. The 
 * utility can be either DI (Destination Independent) and DD (Destination
 * Dependent) 
 */
class MultUtils:public Packet
{
protected:
	int NumPkts;
	struct PktUtilDDandDI *PktInfo;
public:
	MultUtils(double Time, int PID);
	~MultUtils(void);
	virtual void setContents(void *data);
	virtual void *getContents(void);
	virtual Packet *Duplicate(int hops){return NULL;};
	virtual bool AccessPkt(void){return true;};
	void setPktNum(int num){this->NumPkts=num;};
	int getPktNum(void){return this->NumPkts;};
	void PrintContents(void){return;};
};
