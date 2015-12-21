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

#ifndef SCHEDULING_POLICY_H
	#define SCHEDULING_POLICY_H
	#include "SchedulingPolicy.h"
#endif


using namespace std;


struct FIFOPacket
{
	int packetID;
	int distanceFromBegin;

	bool operator < (const FIFOPacket& str) const
	{
		if(distanceFromBegin < str.distanceFromBegin)
		{
			return true;
		}
		else if(distanceFromBegin == str.distanceFromBegin)
		{
			printf("\n[Error]: (FIFOPacket) The packets with IDs %d and %d have the same distance from the beginning of the packet buffer\n\n", packetID, str.packetID);
			exit(EXIT_FAILURE);
		}
		else
		{
			return false;
		}
	}
};


class FIFO: public SchedulingPolicy
{
private:
	vector<FIFOPacket> schedulingPackets;

public:
	FIFO(int NID, PacketBuffer *Bf);
	~FIFO();
	virtual void addPacket(int PID, void *additionalInfo);
	virtual int *getOutgoingPackets();
};
