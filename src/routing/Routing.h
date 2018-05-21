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
 *  Written by Nikolaos Papanikos.
 */


#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#ifndef MAC_H
	#define MAC_H
	#include "../core/MAC.h"
#endif

#ifndef PACKET_BUFFER_H
	#define PACKET_BUFFER_H
	#include "../core/PacketBuffer.h"
#endif

#ifndef PACKET_H
	#define PACKET_H
	#include "../core/Packet.h"
#endif

#ifndef STATS_H
	#define STATS_H
	#include "../core/Statistics.h"
#endif

#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "../core/Settings.h"
#endif

#ifndef GOD_H
	#define GOD_H
	#include "../core/God.h"
#endif

#ifndef DELETION_MECHANISMS_H
	#define DELETION_MECHANISMS_H
	#include "../deletion-mechanisms/DeletionMechanisms.h"
#endif

#ifndef SCHEDULING_POLICIES_H
	#define SCHEDULING_POLICIES_H
	#include "../scheduling-policies/SchedulingPolicies.h"
#endif

#ifndef CONGESTION_MECHANISMS_H
	#define CONGESTION_MECHANISMS_H
	#include "../congestion-control/CongestionControlMechanisms.h"
#endif


class Routing
{
public:
	Routing(PacketPool *PP,MAC *mc,PacketBuffer *Bf,int NID,Statistics *St,Settings *S,God *G);
	virtual ~Routing();
	virtual void recv(double rTime,int pktID)=0;
	virtual void NewContact(double CTime,int NID)=0;
	virtual void ContactRemoved(double CTime,int NID)=0;
	virtual void Contact(double CTime,int NID)=0;
	virtual void Finalize(void);
	virtual void NoCostRecv(PacketPool *PP,MAC *mc,PacketBuffer *Bf,int NID,Statistics *St,Settings *S);

protected:
	PacketPool *pktPool;
	MAC *Mlayer;
	PacketBuffer *Buf;
	Statistics *Stat;
	int NodeID;
	God *SimGod;
	DeletionMechanism *DM;
	Settings *Set;
	SchedulingPolicy *sch;
	CongestionControl *CC;
	bool NCenabled;
	virtual void ReceptionAntipacket(Header *hd,Packet *pkt,int PID,double CurrentTime);
	virtual void ReceptionAntipacketResponse(Header *hd,Packet *pkt,int PID,double CurrentTime);
	virtual void SendDirectPackets(double CTime,int NID);
	virtual void SendVaccine(double CTime,int NID,int *info);
	virtual void SendDirectSummary(double CTime,int NID);
	virtual void AfterDirectTransfers(double CTime,int NID);
	virtual void ReceptionDirectSummary(Header *hd,Packet *pkt,int PID,double CurrentTime);
	virtual void ReceptionDirectRequest(Header *hd,Packet *pkt,int PID,double CurrentTime);
	virtual void SendPacket(double STime, int pktID,int nHop,int RepValue);
	virtual void SendBufferReq(double CTime,int NID);
	virtual void ReceptionBufferReq(Header *hd,Packet *pkt,int PID,double CurrentTime);
	virtual void ReceptionBufferRsp(Header *hd,Packet *pkt,int PID,double CurrentTime);
};


