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


#ifndef VACCINE_H
	#define VACCINE_H
	#include "Vaccine.h"
#endif

/* Constructor
 * -------------
 * Each node has its vaccine component, so NID differentiates each component.
 * VaccineInfo is a set that holds all known packets that have been delivered to
 * their destinations (including packets delivered to current node too).  
 */
Vaccine::Vaccine(int NID): DeletionMechanism()
{
	ID=NID;
	VaccineInfo.clear();
	return;
}

Vaccine::~Vaccine()
{
	return;
}

/* setAsDelivered
 * --------------
 * This method is called when a packet that is destined to current node is delivered
 * successfully.
 */
void Vaccine::setAsDelivered(int pktID)
{
	this->VaccineInfo.insert(pktID);
	return;
}

/* CleanBuffer
 * -----------
 * Removes from packet buffer packets that already have reached their destinations.
 * The deletion is made based on information stored in VaccineInfo set.
 */
void Vaccine::CleanBuffer(PacketBuffer* PktBf)
{
	bool flag=true;
	list<PacketEntry*>::iterator it;
	for(it=PktBf->Entries.begin(); it!=PktBf->Entries.end(); /* do nothing */)
	{
		if(!((*it)->Encoded))
		{
			flag=this->exists((*it)->pktID);
		}
		else
		{
			flag=true;
			if((*it)->mimic == -1)
			{
				for(int i=0;i<(*it)->CodingDepth;i++)
				{
					if(!(this->exists((*it)->EncodedPktIDs[i])))
					{
						flag=false;
						//break;
					}
					else
					{
						if(i == 0)
						{
							(*it)->mimic=1;
						}
						else
						{
							(*it)->mimic=0;
						}
					}
				}
			}
			else
			{
				flag=this->exists((*it)->EncodedPktIDs[(*it)->mimic]);
			}
			
		}
		if(flag)
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

/* isDelivered
 * -----------
 * Returns if the packet with id PID has been already received by the current node.
 */
bool Vaccine::isDelivered(int PID)
{
	bool res=exists(PID);
	return res;
}

/* exists
 * ------
 * This method checks whether packet with ID pktID exists in VaccineInfo set or not.
 */
bool Vaccine::exists(int pktID)
{
	set<int>::iterator it=VaccineInfo.find(pktID);
	
	if(it == VaccineInfo.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

/* UpdateInfo
 * ----------
 * Receives an array of packet IDs and inserts them into the VaccineInfo sets. 
 */
void Vaccine::UpdateInfo(int *info)
{
	int contents=info[0];
	for(int i=1;i<=contents;i++)
	{
		if(!this->exists(info[i]))
		{
			this->VaccineInfo.insert(info[i]);
		}
	}
	return;
}

/* GetInfo
 * -------
 * Returns an array consisting of all packet IDs that have been delivered and the
 * current node is informed about.
 */
int *Vaccine::GetInfo(void)
{
	int infoLength=(int)VaccineInfo.size();
	int *res=(int *)malloc(sizeof(int)*(infoLength+1));
	res[0]=infoLength;
	set<int>::iterator it;
	int pos=1;
	for (it=VaccineInfo.begin(); it!=VaccineInfo.end(); ++it)
	{
		res[pos]=(int)(*it);
		pos++;
	}
	return res;
}

/* PrintInfo
 * -------
 * Prints the contents of vaccine data structure.
 */
void Vaccine::PrintInfo(void)
{
	set<int>::iterator it;
	printf("Vaccine info for node %d has size %d: ",this->ID,(int)VaccineInfo.size());
	for (it=VaccineInfo.begin(); it!=VaccineInfo.end(); ++it)
	{
		printf("%d ",(int)(*it));
	}
	printf("\n");
}

/* OffloaderDeletePkt
 * ------------------
 * Returns if the offloading node (last hop node that transmits pkt to its destination)
 * should drop the packet.
 */
bool Vaccine::OffloaderDeletePkt(void )
{
	return true;
}

/* DestinationDeletePkt
 * --------------------
 * Returns if the destination node should drop the packet.
 */
bool Vaccine::DestinationDeletePkt(void )
{
	return true;
}

/* NoDuplicatesSupport
 * -------------------
 * Returns if the deletion mechanism is designed not to transmit a duplicate to the
 * destination node in case that the destination node has already received a packet 
 * replica.
 */
bool Vaccine::NoDuplicatesSupport(void )
{
	return true;
}
