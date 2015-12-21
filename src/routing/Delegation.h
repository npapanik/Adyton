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


#include <stdio.h>
#include <stdlib.h>

#ifndef ROUTING_H
	#define ROUTING_H
	#include "Routing.h"
#endif

#ifndef UTILITY_H
	#define UTILITY_H
	#include "../data-structures/Utility.h"
#endif

#ifndef LTS_H
	#define LTS_H
	#include "../data-structures/LTS.h"
#endif

#ifndef ENC_H
	#define ENC_H
	#include "../data-structures/Enc.h"
#endif

#ifndef DESTENC_H
	#define DESTENC_H
	#include "../data-structures/DestEnc.h"
#endif

#ifndef AMT_H
	#define AMT_H
	#include "../data-structures/AMT.h"
#endif

#ifndef AIT_H
	#define AIT_H
	#include "../data-structures/AIT.h"
#endif

#ifndef SPM_H
	#define SPM_H
	#include "../data-structures/SPM.h"
#endif

#ifndef LAST_CONTACT_H
	#define LAST_CONTACT_H
	#include "../data-structures/LastContact.h"
#endif

#ifndef ADJA_H
	#define ADJA_H
	#include "../data-structures/Adjacency.h"
#endif

#ifndef DPT_H
	#define DPT_H
	#include "../data-structures/DPT.h"
#endif


class Delegation:public Routing
{
public:
	Delegation(PacketPool* PP, MAC* mc, PacketBuffer* Bf,int NID,Statistics *St,Settings *S,God *G);
	~Delegation();
	virtual void recv(double rTime,int pktID);
	virtual void NewContact(double CTime,int NID);
	virtual void ContactRemoved(double CTime,int NID);
	virtual void Contact(double CTime,int NID);
protected:
	Utility *Util;
	DPT *MyDPT;
	Adjacency *Adja;
	string UType;
	bool UUpdate;
	virtual void AfterDirectTransfers(double CTime,int NID);
	virtual void SendPacket(double STime, int pktID,int nHop,int RepValue);
private:
	void ReceptionData(Header *hd,Packet *pkt,int PID,double CurrentTime,int RealID);
	void ReceptionSummary(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void ReceptionRequest(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void SendSummary(double CTime,int NID);
	void SendContactRequest(double CTime,int NID);
	void SendRSPMRequest(double CTime,int NID);
	void ReceptionDPT(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void ReceptionRequestContacts(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void ReceptionContacts(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void ReceptionRequestRSPM(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void ReceptionRSPM(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void ReceptionRequestSimBetTS(Header *hd,Packet *pkt,int PID,double CurrentTime);
	void prepareReq(double CurrentTime,int encID, int numContents, int* req, int* reqDest, bool* haveIt);
	void prepareReqSimBetTS(double CurrentTime,int encID, int numContents, int* req, int* reqDest, bool* haveIt);
};
