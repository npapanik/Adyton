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


#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <list>
#include <algorithm>

#ifndef PACKET_ENTRY_H
	#define PACKET_ENTRY_H
	#include "PacketEntry.h"
#endif

#ifndef PACKET_RECORD_H
	#define PACKET_RECORD_H
	#include "PacketRecord.h"
#endif

#ifndef STATS_H
	#define STATS_H
	#include "Statistics.h"
#endif

#ifndef GOD_H
	#define GOD_H
	#include "God.h"
#endif

#ifndef DROPPING_POLICIES_H
	#define DROPPING_POLICIES_H
	#include "../dropping-policies/DroppingPolicies.h"
#endif

using namespace std;

struct PktsWon
{
	int PktID;
	int type; //0:won the utility 1:won the threshold
	struct PktsWon *next; 
};

class PacketBuffer
{
public:
	list<PacketEntry*> Entries;
	list<PacketRecord*> Records;
	Statistics *Stat;
	God *SGod;
	DroppingPolicy *DropPol;
	int PacketNum;
	int BufferSize;
	int EncodedNum;
	double TTL;
	int NID;
	bool bufferRecording;
	unsigned int numPacketDrops;

	PacketBuffer(int ID, Statistics *St, God *G, int BSize, int droppingPolicyID, double TimeToLive);
	~PacketBuffer();
	bool addPkt(int pktID, int Dest,int Src, double CurTime, int hops, int prev, double CrtTime);
	bool addPkt(int pktID, int Dest,int Src, double CurTime, int hops, int prev, double CrtTime,struct SimBetTSmetrics *Umetrics);
	bool removePkt(int pktID);
	bool PacketExists(int ID);
	bool NativePacketExists(int ID);
	int getDistanceFromBegin(int ID);
	int getDistanceFromEnd(int ID);
	bool forwardedPkt(int pktID);//SDP related
	bool isForwardedPkt(int pktID);//SDP related
	bool UpdatePrevHop(int ID, int prev);
	bool isFull(void);
	int getFrontPacket(void);
	int BufferStores(void);
	int GetHops(int pktID);
	void SetHops(int pktID, int updHops);
	int GetPrev(int pktID);
	int GetPktDestination(int pktID);
	double GetPktCreationTime(int pktID);
	double getRTTL(int pktID);
	double getProjNetGrowth(double interval);
	double getMeanRTTL(double interval);
	int *getAllPackets(void);
	int *getPacketsNotDestinedTo(int destination);
	struct PktIDandHops *getPktIDsandHopsNotDestinedTo(int destination);
	int *getAllNativePackets(void);
	int *GetAllPrev(int pktID);
	int *GetAllFW(int pktID);
	int *getAllDestinations(void);
	int *getAllDestExcCrntEnc(int crntEnc);
	int *getPackets(int *Destinations);
	int *getPackets(int Destination);
	int *getPackets(int *Destinations, double *Utils);
	int *getPacketsLowUtil(int *p, double *encUtils, bool *Owned);
	int *getPacketsLowUtilSimBetTS(int numReceived,struct PktMultiUtil *receivedPkts);
	int *getPacketsLowUtilSimBet(int numReceived,struct PktMultiUtil *receivedPkts);
	struct PktsWon *getPacketsLowUtilSimBetTS(int numReceived,struct PktMultiUtil *receivedPkts,struct SimBetTSmetrics *LocalU);
	struct PktsWon *getPacketsLowUtilSimBet(int numReceived,struct PktMultiUtil *receivedPkts,struct SimBetTSmetrics *LocalU);
// 	int *getPacketsLowUtilQuota(int *p, double* Utils, bool *Owned, double *CurrentUtils);
	struct PktsWon *getPacketsLowUtilQuota(int *p, double* Utils, bool *Owned, double *CurrentUtils);
	void DropDeadPackets(void);
	void PrintPkts(void);
	PacketEntry *getPacketData(int pktID);
	int getPacketNum(void){return this->PacketNum;};
	int getBufferSize(void){return this->BufferSize;};
	int getBufferLength(void){return (int)this->Entries.size();};
	int getAvailSpace(void);
	int *OrderFIFO(int *PIDs);
	void UpdateThresholdDD(int Destination, double Util);
	void UpdateThresholdDD(double *Utils);
	void UpdateThresholdDI(double Util);
	int getPacketDest(int PID);
	int getPacketSource(int PID);
	int getRepVal(int PID);
	bool removeNativePkt(int pktID);
	void UpdateLowUtil(int *p, double *Utils);
	void enableBufferRecording();
	void printBufferContents();
	unsigned int getNumPacketDrops();
	unsigned int getNumPacketDrops(double startTime, double endTime);
	double CalculateSimBetTSUtility(double SimA, double BetA, double FreqA, double IntimacyA, double RecencyA,double SimB, double BetB, double FreqB, double IntimacyB, double RecencyB);
	double CalculateSimBetUtility(double SimA, double BetA,double SimB, double BetB);
	void UpdateThresholdSimBetTS(struct SimBetTSmetrics *localMetrics);

	//methods that apply only to encoded packets
	bool addEncoded(int *pktID, int *Dest,int *Src, int depth, double CurTime, int prev,int mimic,double *CrtTime);
	bool addEncoded(int *pktID, int *Dest,int *Src, double *Utils,int *Rep, int depth, double CurTime, int prev,int mimic,double *CrtTime);
	void keepAsEncoded(int PID1,int PID2,double CurTime);
	bool removeEncoded(int *pktID, int depth);
	bool PacketExistsAsExtra(int ID);
	int getEncodedDepth(int PID);
	int *getEncodedIDs(int PID);
	int getEncRepVal(int PID,int pos);
	int getEncRepVal(int PID);
	void setEncRepVal(int PID,int pos,int val);
	void setEncRepVal(int PID,int val);
	int *getEncodedRepVals(int PID);
	bool isEncoded(int PID);
	int getMimic(int PID);
	void ActivatePackets(int *IDs);
	void ActivatePackets(int *IDs,double *Utils);
	void ActivatePackets(int *IDs,int *RV);

	bool isBackgroundTraffic(int pktID);
	void writeString(string outputData);

private:
	bool exists(int *PktList,int PID);
	bool exists(struct PktIDandHops *pktData, int PID);
	bool position(int *PktList,int PID);
	
	//methods that apply only to encoded packets
	bool match(int *pktID,int depth);
};
