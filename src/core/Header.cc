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


#ifndef HEADER_H
	#define HEADER_H
	#include "Header.h"
#endif

Header::Header(void)
{
	return;
}

Header::~Header(void)
{
// 	printf("Destructor of Header is being called!\n");
	free(Uwon);
	return;
}


/* Constructor:SimpleHeader
 * ------------------------
 * Initialization of the simple header used with data packets either original or
 * replicas.
 */
SimpleHeader::SimpleHeader(int Source, int Destination, int pHop):Header()
{
	this->Type=1;
	this->Source=Source;
	this->Destination=Destination;
	this->prevHop=pHop;
	this->Hops=1;
	this->OriginalPacket=0;
	this->HeaderSize=10;
	this->nextHop=-1;
	this->Replication=1;
	this->GSC=false;
	this->DirectDelivery=false;
	this->UtilWon=0.0;
	this->Uwon=NULL;
	return;
}

SimpleHeader::~SimpleHeader(void)
{
// 	printf("Destructor of SimpleHeader is being called!\n");
	return;
}


/* Constructor:BasicHeader
 * -----------------------
 * Initialization of the basic header used with control packets exchanged during 
 * a contact between nodes. These control packets contain information regarding 
 * topology, utility values, buffer contents etc.
 */
BasicHeader::BasicHeader(int Source, int Destination):Header()
{
	this->Type=2;
	this->Source=Source;
	this->Destination=Destination;
	this->prevHop=Source;
	this->Hops=1;
	this->OriginalPacket=0;
	this->HeaderSize=10;
	this->nextHop=-1;
	this->Replication=1;
	this->GSC=false;
	this->DirectDelivery=false;
	this->UtilWon=0.0;
	this->Uwon=NULL;
	return;
}

BasicHeader::~BasicHeader(void)
{
// 	printf("Destructor of BasicHeader is being called!\n");
	return;
}


/* SetOriginal
 * -----------
 * Sets the ID of the original packet. For each transmission a new packet duplicate is
 * constructed (both for single-copy and multi-copy routing protocols). All packets 
 * are linked to the original packet that holds all the information about the packet.
 */
void SimpleHeader::SetOriginal(int ID)
{
	this->OriginalPacket=ID;
	return;
}

/* GetOriginal
 * -----------
 * Gets the ID of the original packet.
 */
int SimpleHeader::GetOriginal(void )
{
	return this->OriginalPacket;
}

/* IsDuplicate
 * -----------
 * Returns if the data packet that has the current header is a duplicate or the original
 * packet. 
 */
bool SimpleHeader::IsDuplicate(void )
{
	if(this->OriginalPacket == 0)
	{
		return false;
	}
	return true;
}
