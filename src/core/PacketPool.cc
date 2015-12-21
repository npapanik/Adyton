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


#ifndef PACKETPOOL_H
	#define PACKETPOOL_H
	#include "PacketPool.h"
#endif


/* Constructor: PacketPool
 * ----------------------
 * The "originalIDs" correspond to the packets that were generated at the source
 * nodes, while "variousIDs" correspond to replicas of these packets and
 * other control packets.
 */
PacketPool::PacketPool(int trafficLoad)
{
	this->numPkts = trafficLoad;
	this->originalIDs = 0;
	this->variousIDs = trafficLoad + 1;

	packets.set_empty_key(-1);
	packets.set_deleted_key(-2);

	return;
}


PacketPool::~PacketPool()
{
	dense_hash_map<int,Packet*>::iterator it;


	for(it = packets.begin(); it != packets.end(); ++it)
	{
		delete (it->second);
	}

	packets.clear();

	return;
}


/* AddOriginalPacket
 * -----------------
 * Adds a new packet "pkt" (given as argument) inside the hash map. This packet
 * is treated as a new arrival (first instance inside the hash map). Also, a global
 * identifier is assigned to the given packet.
 */
void PacketPool::AddOriginalPacket(Packet* pkt)
{
	if(pkt->getHeader() == NULL)
	{
		printf("\n[Error]: (PacketPool::AddOriginalPacket) Cannot add a packet without a header in the packet pool\n\n");
		exit(EXIT_FAILURE);
	}

	this->originalIDs++;

	if(this->originalIDs > this->numPkts)
	{
		printf("\n[Error]: (PacketPool::AddOriginalPacket) The number of generated packets exceeded the expected traffic load (%d)\n\n", this->numPkts);
		exit(EXIT_FAILURE);
	}
	else
	{
		pkt->setID(this->originalIDs);
		this->packets[this->originalIDs] = pkt;

		return;
	}
}


/* AddPacket
 * ---------
 * Adds a new packet "pkt" (given as argument) inside the hash map. This packet
 * is treated as a packet copy of an original packet (is not the first instance).
 * Also, a global identifier is assigned to the given packet.
 */
void PacketPool::AddPacket(Packet* pkt)
{
	if(pkt->getHeader() == NULL)
	{
		printf("\n[Error]: (PacketPool::AddPacket) Cannot add a packet without a header in the packet pool\n\n");
		exit(EXIT_FAILURE);
	}

	this->variousIDs++;

	if(this->variousIDs == max_int)
	{
		printf("\n[Error]: (PacketPool::AddPacket) The number of packets that have been added in the packet pool has reached the maximum finite value (%d)\n\n", max_int);
		exit(EXIT_FAILURE);
	}
	else
	{
		pkt->setID(this->variousIDs);
		this->packets[this->variousIDs] = pkt;

		return;
	}
}


/* ErasePacket
 * -----------
 * Packet with ID "pktID" (given as argument) is erased from the hash map.
 */
bool PacketPool::ErasePacket(int pktID)
{
	Packet *pkt;


	pkt = this->packets[pktID];

	if(pkt == NULL)
	{
		return false;
	}
	else
	{
		this->packets.erase(pktID);
		delete pkt;

		return true;
	}
}


/* GetPacket
 * ---------
 * A pointer to the packet with ID "pktID" that exists in the hash map is returned.
 */
Packet* PacketPool::GetPacket(int pktID)
{
	return this->packets[pktID];
}
