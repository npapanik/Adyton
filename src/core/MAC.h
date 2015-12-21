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


#include <stdlib.h>
#include <stdio.h>
#include <vector>

#ifndef EVENT_H
	#define EVENT_H
	#include "Event.h"
#endif

#ifndef EVENT_LIST_H
	#define EVENT_LIST_H
	#include "EventList.h"
#endif

#ifndef CONNECTIONMAP_H
	#define CONNECTIONMAP_H
	#include "ConnectionMap.h"
#endif

using namespace std;

/* Interface: MAC 
 * --------------
 */
class MAC
{
public:
	double Bandwidth; // in Mbps
	EventList *EvList;
	MAC(){return;};
	virtual ~MAC(){return;};
	virtual int BroadcastPkt(double Time,int Sender,int pktSize,int pktID)=0;
	virtual void SetBandwidth(double Mbps)=0;
	virtual void SendPkt(double Time,int Sender,int Rec,int pktSize,int pktID)=0;
	virtual double LastEvent(Event *e)=0;
	virtual int GetParallelConnections(int Sender)=0;
};

/* Ideal
 * -----
 * This class implements a simple ideal MAC layer without bandwidth or access control.
 */
class Ideal:public MAC
{
public:
	ConnectionMap *Connections;
	Ideal(double Mbps,EventList *EL,ConnectionMap *Conn);
	~Ideal();
	virtual int BroadcastPkt(double Time,int Sender,int pktSize,int pktID);
	virtual void SetBandwidth(double Mbps){this->Bandwidth=Mbps;};
	virtual void SendPkt(double Time,int Sender,int Rec,int pktSize,int pktID);
	virtual double LastEvent(Event *e){return 0.0;};
	virtual int GetParallelConnections(int Sender);
};
