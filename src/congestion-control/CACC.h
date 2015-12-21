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


#ifndef CONGESTIONCONTROL_H
	#define CONGESTIONCONTROL_H
	#include "CongestionControl.h"
#endif

#ifndef AMT_H
	#define AMT_H
	#include "../data-structures/AMT.h"
#endif



struct CACCPacketInformation
{
	int packetID;
	double numContacts;
	double normNumContacts;
	double avgDuration;
	double normAvgDuration;
	double gainValue;
};


struct CACCExtraInformation
{
	double *recContactNumber;
	double *recAverageDuration;
};


class CACC: public CongestionControl
{
protected:
	struct CongestionInformation *information;
	struct CACCExtraInformation *extraInformation;
	vector<CACCPacketInformation> pInfo;
	AMT *AMTUtils;

public:
	CACC(int NID, PacketBuffer *Bf, Settings *S);
	~CACC();
	virtual void contactUp(double currTime, int recID);
	virtual void contactDown(double currTime, int recID);
	virtual void receivedBufferReq(double currentTime, int reqNode);
	virtual void addBufferInfo(void *info);
	virtual void *getExtraInfo(double currTime);
	virtual void setExtraInfo(void *extraInfo, double currTime);
	virtual void addPacketInfo(struct ImportInformation *imp);
	virtual int *filterPackets(int *pktList);
	virtual void releaseInformation();
	virtual int getBufferSize();
	virtual int getBufferLength();

private:
	double getGainValue(int ID);
};
