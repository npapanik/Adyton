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
 *  Written by Dimitrios-Georgios Akestoridis.
 */


#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

#ifndef PACKET_BUFFER_H
	#define PACKET_BUFFER_H
	#include "../core/PacketBuffer.h"
#endif


using namespace std;


struct ExchangedInformation
{
	double senUtil;
	double recUtil;
};


class SchedulingPolicy
{
protected:
	int nodeID;
	PacketBuffer *Buff;

public:
	SchedulingPolicy(int NID, PacketBuffer *Bf);
	virtual ~SchedulingPolicy();
	virtual void initSchExchangedInfo(struct ExchangedInformation *info);
	virtual void addPacket(int PID, void *additionalInfo)=0;
	virtual int *getOutgoingPackets()=0;
};
