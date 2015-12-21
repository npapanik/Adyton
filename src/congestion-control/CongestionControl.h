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
 *  Written by Dimitrios-Georgios Akestoridis.
 */


#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <cmath>

#ifndef PACKET_BUFFER_H
	#define PACKET_BUFFER_H
	#include "../core/PacketBuffer.h"
#endif

#define REMOVED_PACKET -1
//#define CC_DEBUG



struct CongestionInformation
{
	int recID;
	int recBufSize;
	int recBufLength;
};


struct ImportInformation
{
	int pktID;
	double senUtil;
	double recUtil;
	double currTime;
};


class CongestionControl
{
protected:
	PacketBuffer *Buff;
	Settings *Set;
	int nodeID;

public:
	CongestionControl(int NID, PacketBuffer *Bf, Settings *S);
	virtual ~CongestionControl();
	virtual void initCCImportInfo(struct ImportInformation *info);
	virtual void contactUp(double currTime, int recID) = 0;
	virtual void contactDown(double currTime, int recID) = 0;
	virtual void receivedBufferReq(double currentTime, int reqNode) = 0;
	virtual void addBufferInfo(void *info) = 0;
	virtual void *getExtraInfo(double currTime) = 0;
	virtual void setExtraInfo(void *extraInfo, double currTime) = 0;
	virtual void addPacketInfo(struct ImportInformation *imp) = 0;
	virtual int *filterPackets(int *pktList) = 0;
	virtual void releaseInformation() = 0;
	virtual int getBufferSize() = 0;
	virtual int getBufferLength() = 0;
};
