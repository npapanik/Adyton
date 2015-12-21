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


#ifndef NOCC_H
	#define NOCC_H
	#include "NoCC.h"
#endif



NoCC::NoCC(int NID, PacketBuffer *Bf, Settings *S): CongestionControl(NID, Bf, S)
{
	return;
}


NoCC::~NoCC()
{
	return;
}


void NoCC::contactUp(double currTime, int recID)
{
	return;
}


void NoCC::contactDown(double currTime, int recID)
{
	return;
}


void NoCC::receivedBufferReq(double currentTime, int reqNode)
{
	return;
}


void NoCC::addBufferInfo(void *info)
{
	return;
}


void *NoCC::getExtraInfo(double currTime)
{
	return NULL;
}


void NoCC::setExtraInfo(void *extraInfo, double currTime)
{
	free(extraInfo);

	return;
}


void NoCC::addPacketInfo(struct ImportInformation *imp)
{
	free(imp);

	return;
}


int *NoCC::filterPackets(int *pktList)
{
	return pktList;
}


void NoCC::releaseInformation()
{
	return;
}


int NoCC::getBufferSize()
{
	return Buff->getBufferSize();
}


int NoCC::getBufferLength()
{
	return Buff->getBufferLength();
}
