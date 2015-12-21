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

/* Interface: Event 
 * ----------------
 */
class Event
{
	
protected:
	int NodeA;
	int NodeB;
	double EventTime;
	int EventID;
public:
	class Event *Next;
	virtual double getEventTime(void){return EventTime;};
	virtual int getEventID(void){return EventID;};
	int getNodeA(void){return NodeA;};
	int getNodeB(void){return NodeB;};
	virtual ~Event(void){};
	virtual void printEvent(void)=0;
};

/* ContactUp
 * ---------
 * This event describes a connection between two nodes.
 */
class ContactUp:public Event
{ 
public:
	ContactUp(double Time,int A,int B);
	~ContactUp(void);
	virtual void printEvent(void);
};

/* ContactDown
 * ---------
 * This event describes a disconnection between two nodes.
 */
class ContactDown:public Event
{
public:
	ContactDown(double Time,int A,int B);
	~ContactDown(void);
	virtual void printEvent(void);
};


/* Transmission
 * ---------
 * This event describes a packet transmission, that takes place between two
 * nodes. Note that when one node transmits a packet to its one hop neighborhood
 * multiple objects of class transmission are being created.
 * 
 * Note: When Sender is "-1" then the packet is coming from the application layer
 * of the receiver node.
 */
class Transmission:public Event
{
protected:
	int pktID;
	bool valid;
public:
	Transmission(double Time,int S,int R,int ID);
	~Transmission(void);
	int getSender(void){return this->NodeA;};
	int getReceiver(void){return this->NodeB;};
	int getpktID(void){return pktID;};
	virtual void printEvent(void);
	bool getState(void){return valid;};
	void setState(bool status){this->valid=status;};
};

/* CheckPoint
 * ---------
 * This event specifies that a new contact import from the trace file should 
 * take place. 
 */
class CheckPoint:public Event
{
protected:
	int LineStopped;
public:
	CheckPoint(double Time,int A,int B);
	~CheckPoint(void);
	virtual void printEvent(void);
	int GetLine(void);
};
