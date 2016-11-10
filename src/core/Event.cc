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


#ifndef EVENT_H
	#define EVENT_H
	#include "Event.h"
#endif


/* Constructor:ContactUp
 * ---------------------
 * Time: Connection time
 * A: Node A
 * B: Node B
 */
ContactUp::ContactUp(double Time,int A,int B)
{
	this->EventID=CONTACTUP;
	this->EventTime=Time;
	this->NodeA=A;
	this->NodeB=B;
	this->Next=NULL;
}

/* Destructor:ContactUp
 * --------------------
 * Nothing so far!
 */
ContactUp::~ContactUp(void )
{
	return;
}


/* printEvent
 * ----------
 * For debugging purposes.
 */
void ContactUp::printEvent(void )
{
	printf("%f:Contact up between %d and %d\n",this->EventTime,this->NodeA,this->NodeB);
	return;
}


/* Constructor:ContactDown
 * -----------------------
 * Time: Disconnection time
 * A: Node A
 * B: Node B
 */
ContactDown::ContactDown(double Time,int A,int B)
{
	this->EventID=CONTACTDOWN;
	this->EventTime=Time;
	this->NodeA=A;
	this->NodeB=B;
	this->Next=NULL;
}

/* Destructor:ContactDown
 * ----------------------
 * Nothing so far!
 */
ContactDown::~ContactDown(void )
{
	return;
}

/* printEvent
 * ----------
 * For debugging purposes.
 */
void ContactDown::printEvent(void )
{
	printf("%f:Contact down between %d and %d\n",this->EventTime,this->NodeA,this->NodeB);
	return;
}


/* Constructor:Transmission
 * ------------------------
 * Time: When transmission starts
 * S: Sender
 * R: Receiver
 */
Transmission::Transmission(double Time,int S,int R,int ID)
{
	this->EventID=TRANSMISSION;
	this->EventTime=Time;
	this->NodeA=S;
	this->NodeB=R;
	this->pktID=ID;
	this->Next=NULL;
	this->valid=true;
}

/* Destructor:Transmission
 * -----------------------
 * Nothing so far!
 */
Transmission::~Transmission(void )
{
	return;
}

/* printEvent
 * ----------
 * For debugging purposes.
 */
void Transmission::printEvent(void)
{
	printf("%f:%d transmitting to %d\n",this->EventTime,this->NodeA,this->NodeB);
	return;
}

/* Constructor:CheckPoint
 * ----------------------
 * Only the second argument is used and corresponds to the line number of the last
 * contact read from the trace file.
 */
CheckPoint::CheckPoint(double Time, int A, int B)
{
	this->EventID=CHECKPOINT;
	this->EventTime=Time;
	this->LineStopped=A;
	this->Next=NULL;
	this->NodeA=-1;
	this->NodeB=-1;
}

/* Constructor:CheckPoint
 * ----------------------
 * Nothing so far!
 */
CheckPoint::~CheckPoint(void )
{
	return;
}

/* printEvent
 * ----------
 * For debugging purposes.
 */
void CheckPoint::printEvent(void )
{
	printf("%f:Stopped at line %d\n",this->EventTime,this->LineStopped);
}

/* printEvent
 * ----------
 * Returns the line number of the trace file that corresponds to the last contact imported the
 * simulator's event list.  
 */
int CheckPoint::GetLine(void )
{
	return this->LineStopped;
}


/* Constructor:PacketGeneration
 * ----------------------------
 * Time: When packet is genarated
 * S: Sender
 * R: Receiver
 */
PacketGeneration::PacketGeneration(double Time,int S,int R,int ID)
{
	this->EventID=PACKETGENERATION;
	this->EventTime=Time;
	this->NodeA=S;
	this->NodeB=R;
	this->pktID=ID;
	this->Next=NULL;
	this->valid=true;
}

/* Destructor:PacketGeneration
 * -----------------------
 * Nothing so far!
 */
PacketGeneration::~PacketGeneration(void )
{
	return;
}

/* printEvent
 * ----------
 * For debugging purposes.
 */
void PacketGeneration::printEvent(void)
{
	printf("%f: a new packet with ID %d generated at %d and is destined to %d\n",this->EventTime,this->pktID,this->NodeA,this->NodeB);
	return;
}