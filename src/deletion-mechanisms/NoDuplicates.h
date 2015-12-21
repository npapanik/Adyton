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


#ifndef DELETION_MECHANISM_H
	#define DELETION_MECHANISM_H
	#include "DeletionMechanism.h"
#endif


class NoDuplicates:public DeletionMechanism
{
protected:
	int ID;
	set<int> Delivered;
public:
	NoDuplicates(int NID);
	~NoDuplicates();
	virtual void setAsDelivered(int pktID);
	virtual void CleanBuffer(PacketBuffer *PktBf);
	virtual void UpdateInfo(int *info);
	virtual int *GetInfo(void);
	virtual bool OffloaderDeletePkt(void);
	virtual bool DestinationDeletePkt(void);
	virtual bool isDelivered(int PID);
	virtual bool ExchangeDirectSummary(void);
	virtual bool NoDuplicatesSupport(void);
};
