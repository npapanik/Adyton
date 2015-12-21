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
#include <limits>
#include <google-sparsehash/google/dense_hash_map>

#ifndef PACKET_H
	#define PACKET_H
	#include "Packet.h"
#endif

const int min_int = std::numeric_limits<int>::min();
const int max_int = std::numeric_limits<int>::max();

using google::dense_hash_map;


/* PacketPool
 * ----------
 * This class implements a hash map of packets. Hash map
 * implementation has been chosen to enhance performance.
 */
class PacketPool
{
private:
	int numPkts;
	int originalIDs;
	int variousIDs;
	dense_hash_map<int,Packet*> packets;

public:
	PacketPool(int trafficLoad);
	~PacketPool();
	void AddOriginalPacket(Packet* pkt);
	void AddPacket(Packet *pkt);
	bool ErasePacket(int pktID);
	Packet *GetPacket(int pktID);
};
