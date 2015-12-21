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


#ifndef CONGESTION_H
	#define CONGESTION_H
	#include "CongestionControl.h"
#endif



CongestionControl::CongestionControl(int NID, PacketBuffer* Bf, Settings *S)
{
	this->nodeID = NID;
	this->Buff = Bf;
	this->Set = S;

	return;
}


CongestionControl::~CongestionControl()
{
	return;
}


void CongestionControl::initCCImportInfo(struct ImportInformation *info)
{
	info->pktID = 0;
	info->senUtil = 0.0;
	info->recUtil = 0.0;
	info->currTime = 0.0;

	return;
}
