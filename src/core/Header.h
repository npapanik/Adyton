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

#ifndef PACKET_ENTRY_H
	#define PACKET_ENTRY_H
	#include "PacketEntry.h"
#endif


/* Header
 * ------
 * 
 * 
 */
class Header
{
protected:
	int Type;
	int Source;
	int Destination;
	int prevHop;
	int nextHop;
	int Hops;
	int HeaderSize;
	int OriginalPacket;
	int Replication;
	bool GSC;
	bool DirectDelivery;
	double UtilWon;
	double DIUtilWon;
	struct SimBetTSmetrics *Uwon;
	
public:
	Header(void);
	virtual ~Header(void);
	virtual void SetOriginal(int ID){return;};
	virtual int GetOriginal(void){return 0;};
	virtual bool IsDuplicate(void){return false;};
	virtual int GetprevHop(void){return this->prevHop;};
	virtual void SetprevHop(int ID){this->prevHop=ID;};
	virtual int GetHops(void){return this->Hops;};
	virtual void SetHops(int H){this->Hops=H;};
	virtual int GetDestination(void){return this->Destination;};
	virtual int GetHeaderSize(void){return this->HeaderSize;};
	virtual void SetHeaderSize(int length){this->HeaderSize=length;};
	virtual int GetSource(void){return this->Source;};
	virtual int GetType(void){return this->Type;};
	virtual int GetNextHop(void){return this->nextHop;};
	virtual void SetNextHop(int next){this->nextHop=next;};
	virtual int GetRep(void){return Replication;};
	virtual void SetRep(int R){Replication=R;};
	virtual void SetGSCon(void){this->GSC=true;};
	virtual bool GetGSC(void){return this->GSC;};
	virtual void SetDeliveryStatus(bool state){this->DirectDelivery=state;};
	virtual bool GetDeliveryStatus(void){return this->DirectDelivery;};
	virtual double getUtilWon(void){return this->UtilWon;};
	virtual void setUtilWon(double u){this->UtilWon=u;};
	virtual double getDIUtilWon(void){return this->DIUtilWon;};
	virtual void setDIUtilWon(double u){this->DIUtilWon=u;};
	virtual struct SimBetTSmetrics *getUtilsWon(void){return this->Uwon;};
	virtual void setUtilsWon(struct SimBetTSmetrics *U){this->Uwon=U;};
};

class SimpleHeader:public Header
{
public:
	SimpleHeader(int Source,int Destination,int pHop);
	~SimpleHeader(void);
	virtual void SetOriginal(int ID);
	virtual int GetOriginal(void);
	virtual bool IsDuplicate(void);
};


class BasicHeader:public Header
{
public:
	BasicHeader(int Source,int Destination);
	~BasicHeader(void);
};
