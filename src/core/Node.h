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

#ifndef PACKETPOOL_H
	#define PACKETPOOL_H
	#include "PacketPool.h"
#endif

#ifndef MAC_H
	#define MAC_H
	#include "MAC.h"
#endif

#ifndef PACKET_BUFFER_H
	#define PACKET_BUFFER_H
	#include "PacketBuffer.h"
#endif

#ifndef STATS_H
	#define STATS_H
	#include "Statistics.h"
#endif

#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "Settings.h"
#endif

#ifndef ROUTING_PROTOCOLS_H
	#define ROUTING_PROTOCOLS_H
	#include "../routing/RoutingProtocols.h"
#endif



/* Node
 * ----
 * This class implements the functionality of the network nodes.
 */
class Node
{
public:
	int ID;
	PacketPool *Pool;
	PacketBuffer *Buffer;
	MAC *macLayer;
	Routing *RLogic;
	Statistics *Stat;
	Settings *SimSet;
	God *Gd;

	Node(int NodeID, PacketPool *pktPool, MAC *mlayer, Statistics *St, Settings *S, God *G);
	virtual ~Node();

	virtual void ConUpdate(double CTime, int NodeID, bool status, bool history);
	virtual void recvFromApp(double CurTime, int Dest);
	virtual void recv(int pktID, double CurTime);
	virtual void PrintBuffer(void);
	virtual void Finalize(void){this->RLogic->Finalize();};
	virtual PacketBuffer *getPacketBuffer(void){return this->Buffer;};
};
