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


#ifndef CATACLYSM_H
	#define CATACLYSM_H
	#include "Cataclysm.h"
#endif

/* Constructor
 * -------------
 * Every node has a "Cataclysm" component. Every component is the same for all nodes
 * and communicates with god class to learn about successful packet receptions by
 * final nodes.  
 */
Cataclysm::Cataclysm(int NumberOfNodes, God* SimGod): DeletionMechanism()
{
	NodeNum=NumberOfNodes;
	Gd=SimGod;
	Gd->InitDelivered(NumberOfNodes);
}

Cataclysm::~Cataclysm()
{
	return;
}

/* setAsDelivered
 * --------------
 * This method is called when a packet that is destined to current node is delivered
 * successfully. God class is informed about successful delivery, too. 
 */
void Cataclysm::setAsDelivered(int pktID)
{
	Gd->AddDelivered(pktID);
	return;
}

/* CleanBuffer
 * -----------
 * Removes from packet buffer packets that already have reached their destinations.
 * The deletion is made based on information stored in God class.
 */
void Cataclysm::CleanBuffer(PacketBuffer* PktBf)
{
	list<PacketEntry*>::iterator it;
	for(it=PktBf->Entries.begin(); it!=PktBf->Entries.end(); /* do nothing */)
	{
		if(Gd->IsDelivered((*it)->pktID))
		{
			delete (*it);
			it=PktBf->Entries.erase(it);
			PktBf->PacketNum--;
			//ToDo
			//Stat->incPktsDied();
			//Stat->decPktsStored();
		}
		else
		{
			++it;
		}
	}
	return;
}

/* UpdateInfo
 * ----------
 * Is not used by "Cataclysm".
 */
void Cataclysm::UpdateInfo(int* info)
{
	return;
}

/* GetInfo
 * ----------
 * Is not used by "Cataclysm".
 */
int* Cataclysm::GetInfo(void)
{
	return NULL;
}

/* OffloaderDeletePkt
 * ------------------
 * Returns if the offloading node (last hop node that transmits pkt to its destination)
 * should drop the packet.
 */
bool Cataclysm::OffloaderDeletePkt(void )
{
	return true;
}

/* DestinationDeletePkt
 * --------------------
 * Returns if the destination node should drop the packet.
 */
bool Cataclysm::DestinationDeletePkt(void )
{
	return true;
}

/* NoDuplicatesSupport
 * -------------------
 * Returns if the deletion mechanism is designed not to transmit a duplicate to the
 * destination node in case that the destination node has already received a packet 
 * replica.
 */
bool Cataclysm::NoDuplicatesSupport(void )
{
	return true;
}

