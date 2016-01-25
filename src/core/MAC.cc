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
 *  Written by Nikolaos Papanikos.
 */


#ifndef MAC_H
	#define MAC_H
	#include "MAC.h"
#endif

/* Constructor: Ideal
 * ------------------
 * This class implements a simple ideal MAC layer without bandwidth
 * or access control
 */
Ideal::Ideal(double Mbps,EventList *EL,ConnectionMap *Conn)
{
	this->Bandwidth=Mbps;
	this->EvList=EL;
	this->Connections=Conn;
}

Ideal::~Ideal()
{
	return;
}

/* BroadcastPkt
 * ------------
 * This method broadcasts a packet with "pktID" and size "pktSize" to all 
 * one hop neighbors of Node "Sender" at time "Time"
 */
int Ideal::BroadcastPkt(double Time,int Sender,int pktSize,int pktID)
{
	int NumberNodes=this->Connections->NumberOfN(Sender);
	int *NList=this->Connections->GetN(Sender);
	Event *tmp=NULL;
	for(int i=0;i<NumberNodes;i++)
	{
		tmp=new Transmission(Time,Sender,NList[i],pktID);
		EvList->InsertEvent(tmp);
	}
	free(NList);
	return NumberNodes;
}

/* SendPkt
 * -------
 * This method sends a packet with "pktID" and size "pktSize" from 
 * node "Sender" to node "Receiver" at time "Time"
 */
void Ideal::SendPkt(double Time, int Sender, int Rec, int pktSize, int pktID)
{
	Event *tmp=new Transmission(Time,Sender,Rec,pktID);
	EvList->InsertEvent(tmp);
	return;
}

/* GetParallelConnections
 * ----------------------
 * This method broadcasts a packet with "pktID" and size "pktSize" to all 
 * one hop neighbors of Node "Sender" at time "Time".
 */
int Ideal::GetParallelConnections(int Sender)
{
	return (this->Connections->NumberOfN(Sender));
}


/* ---------------------------------------------
 * --Future implementation--
 * The above methods measures transmission delay
 * ---------------------------------------------
int Ideal::BroadcastPkt(double Time,int Sender,int pktSize,int pktID)
{
	int NumberNodes=this->Connections->NumberOfN(Sender);
	double Delay=(pktSize*8)/(this->Bandwidth*1000);
	Delay+=Time;

	int *NList=this->Connections->GetN(Sender);
	Event *tmp=NULL;
	for(int i=0;i<NumberNodes;i++)
	{
		tmp=new Transmission(Delay,Sender,NList[i],pktID);
		EvList->InsertEvent(tmp);
	}
	free(NList);
	return NumberNodes;
}

void Ideal::SendPkt(double Time, int Sender, int Rec, int pktSize, int pktID)
{
	double Delay=(pktSize*8)/(this->Bandwidth*1000);
	Delay+=Time;
	Event *tmp=new Transmission(Delay,Sender,Rec,pktID);
	EvList->InsertEvent(tmp);
	return;
}*/



