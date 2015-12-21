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


#ifndef JUSTTTL_H
	#define JUSTTTL_H
	#include "JustTTL.h"
#endif

/* Constructor
 * -------------
 * No special access to one of the simulator's components or data structures is needed.  
 */
JustTTL::JustTTL(void): DeletionMechanism()
{
	return;
}

JustTTL::~JustTTL()
{
	return;
}

/* setAsDelivered
 * --------------
 * Is not used by "JustTTL".
 */
void JustTTL::setAsDelivered(int pktID)
{
	return;
}

/* CleanBuffer
 * -----------
 * Is not used by "JustTTL".
 */
void JustTTL::CleanBuffer(PacketBuffer* PktBf)
{
	return;
}

/* UpdateInfo
 * ----------
 * Is not used by "JustTTL".
 */
void JustTTL::UpdateInfo(int* info)
{
	return;
}

/* GetInfo
 * -------
 * Is not used by "JustTTL".
 */
int *JustTTL::GetInfo(void)
{
	return NULL;
}

/* OffloaderDeletePkt
 * ------------------
 * Returns if the offloading node (last hop node that transmits pkt to its destination)
 * should drop the packet.
 */
bool JustTTL::OffloaderDeletePkt(void )
{
	return false;
}

/* DestinationDeletePkt
 * --------------------
 * Returns if the destination node should drop the packet.
 */
bool JustTTL::DestinationDeletePkt(void )
{
	return true;
}

/* NoDuplicatesSupport
 * -------------------
 * Returns if the deletion mechanism is designed not to transmit a duplicate to the
 * destination node in case that the destination node has already received a packet 
 * replica.
 */
bool JustTTL::NoDuplicatesSupport(void )
{
	return false;
}

