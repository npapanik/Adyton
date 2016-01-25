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
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#ifndef PACKET_BUFFER_H
	#define PACKET_BUFFER_H
	#include "PacketBuffer.h"
#endif

//#define PACKET_BUFFER_DEBUG


int swapPos(int pos)
{
	if(pos)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* Constructor:PacketBuffer
 * ------------------------
 * Creates a new local buffer to store incoming packets. When the buffer gets full, a packet is 
 * discarded according to the dropping policy given as user input.
 */
PacketBuffer::PacketBuffer(int ID, Statistics *St, God *G, int BSize, int droppingPolicyID, double TimeToLive)
{
	this->Stat = St;
	this->PacketNum = 0;
	this->EncodedNum = 0;
	this->BufferSize = BSize;
	this->TTL = TimeToLive;
	this->SGod = G;
	this->NID = ID;
	this->bufferRecording = false;
	this->numPacketDrops = 0;

	switch(droppingPolicyID)
	{
		case DF_DP:
		{
			this->DropPol = new DropFront(&Entries, this->Stat);
			break;
		}
		case DT_DP:
		{
			this->DropPol = new DropTail(&Entries, this->Stat);
			break;
		}
		case DO_DP:
		{
			this->DropPol = new DropOldest(&Entries, this->Stat);
			break;
		}
		case DY_DP:
		{
			this->DropPol = new DropYoungest(&Entries, this->Stat);
			break;
		}
		case DFASP_DP:
		{
			this->DropPol = new DropFrontASP(&Entries, this->Stat);
			break;
		}
		case DTASP_DP:
		{
			this->DropPol = new DropTailASP(&Entries, this->Stat);
			break;
		}
		case DOASP_DP:
		{
			this->DropPol = new DropOldestASP(&Entries, this->Stat);
			break;
		}
		case DYASP_DP:
		{
			this->DropPol = new DropYoungestASP(&Entries, this->Stat);
			break;
		}
		default:
		{
			this->DropPol = new DropFront(&Entries, this->Stat);
			break;
		}
	}

	return;
}

PacketBuffer::~PacketBuffer()
{	
// 	printf("Deleting entries..\n");

	while(!Entries.empty()) 
	{
		delete Entries.front();
		Entries.pop_front();
	}
	Entries.clear();
 	
	while(!Records.empty())
	{
		delete Records.front();
		Records.pop_front();
	}
	Records.clear();
	
	delete DropPol;
	return;
}

/* enableBufferRecording
 * ---------------------
 * If buffer recording is enabled, then more information regarding packet forwards/drops is
 * recorded in order to enable additional statistics.
 */
void PacketBuffer::enableBufferRecording()
{
	this->bufferRecording = true;
	return;
}

/* addPkt
 * ------
 * Adds a new packet to the buffer. In the case of full buffer a packet is discarded 
 * according to the dropping policy given as user input.
 */
bool PacketBuffer::addPkt(int pktID, int Dest,int Src,double CurTime, int hops, int prev, double CrtTime)
{
	double drpCrtTime;
	PacketEntry *tmp;
	PacketRecord *rec1;
	PacketRecord *rec2;
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();


	/* Check if the packet exists */
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return false;
		}
	}


	/* In case that the buffer is finite, check if it is full */
	if(isFull())
	{
		drpCrtTime = DropPol->Drop();
		PacketNum--;
		numPacketDrops++;

		if(bufferRecording)
		{
			/* Keep a record of this action */
			if(TTL == 0)
			{
				rec1 = new PacketRecord(SGod->getSimTime(), -1, false, true, false);
				Records.push_back(rec1);
			}
			else
			{
				rec1 = new PacketRecord(SGod->getSimTime(), TTL - (SGod->getSimTime() - drpCrtTime), false, true, false);
				Records.push_back(rec1);
			}
		}
	}


	/* Add the new packet */
	tmp = new PacketEntry(pktID, Dest,Src, CurTime, hops, prev, CrtTime);
	Entries.push_back(tmp);
	PacketNum++;
// 	printf("%d:Added packet %d to the buffer\n",this->NID,pktID);

	if(bufferRecording)
	{
		/* Keep a record of this action */
		rec2 = new PacketRecord(SGod->getSimTime(), getRTTL(pktID), true, false, false);
		Records.push_back(rec2);
	}

	return true;
}

/* addPkt
 * ------
 * Adds a new packet to the buffer keeping additional metrics (SimBeTS utils)/
 */
bool PacketBuffer::addPkt(int pktID, int Dest, int Src, double CurTime, int hops, int prev, double CrtTime, SimBetTSmetrics* Umetrics)
{
	double drpCrtTime;
	PacketEntry *tmp;
	PacketRecord *rec1;
	PacketRecord *rec2;
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();


	/* Check if the packet exists */
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return false;
		}
	}


	/* In case that the buffer is finite, check if it is full */
	if(isFull())
	{
		drpCrtTime = DropPol->Drop();
		PacketNum--;
		numPacketDrops++;

		if(bufferRecording)
		{
			/* Keep a record of this action */
			if(TTL == 0)
			{
				rec1 = new PacketRecord(SGod->getSimTime(), -1, false, true, false);
				Records.push_back(rec1);
			}
			else
			{
				rec1 = new PacketRecord(SGod->getSimTime(), TTL - (SGod->getSimTime() - drpCrtTime), false, true, false);
				Records.push_back(rec1);
			}
		}
	}


	/* Add the new packet */
	tmp = new PacketEntry(pktID, Dest,Src, CurTime, hops, prev, CrtTime,Umetrics);
	Entries.push_back(tmp);
	PacketNum++;

	if(bufferRecording)
	{
		/* Keep a record of this action */
		rec2 = new PacketRecord(SGod->getSimTime(), getRTTL(pktID), true, false, false);
		Records.push_back(rec2);
	}

	return true;
}


/* match
 * -----
 * Searches among encoded packets to find one that contains a packet with pktID.
 */
bool PacketBuffer::match(int *pktID,int depth)
{
	list<PacketEntry*>::iterator it;
	/* Check if the encoded packet (with exactly the same contents) exists */
	bool match=false;
	bool found=false;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		match=false;
		if((*it)->Encoded)
		{
			if(depth == (*it)->CodingDepth)
			{
				for(int i=0;i<depth;i++)
				{
					found=false;
					for(int j=0;j<depth;j++)
					{
						if(pktID[i] == (*it)->EncodedPktIDs[j])
						{
							found=true;
							break;
						}
					}
					if(!found)
					{
						match=false;
						break;
					}
					else
					{
						match=true;
					}
				}
			}
		}
		if(match)
		{
			return true;
		}
	}
	return false;
}

/* addEncoded
 * ----------
 * Adds an encoded packet to the buffer.
 */
bool PacketBuffer::addEncoded(int* pktID, int* Dest, int* Src, int depth, double CurTime, int prev,int mimic,double *CrtTime)
{
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	/* Check if the encoded packet (with exactly the same contents) exists */
	if(match(pktID,depth))
	{
		return false;
	}
	/* In case that the buffer is finite, check if it is full */
	double drpCrtTime=0.0;
	PacketRecord *rec1;
	if(isFull())
	{
		drpCrtTime = DropPol->Drop();
		PacketNum--;
		numPacketDrops++;

		if(bufferRecording)
		{
			/* Keep a record of this action */
			if(TTL == 0)
			{
				rec1 = new PacketRecord(SGod->getSimTime(), -1, false, true, false);
				Records.push_back(rec1);
			}
			else
			{
				rec1 = new PacketRecord(SGod->getSimTime(), TTL - (SGod->getSimTime() - drpCrtTime), false, true, false);
				Records.push_back(rec1);
			}
		}
	}
	/* Add the new packet */
	PacketEntry *tmp = new PacketEntry(pktID, Dest,Src,depth,CrtTime,CurTime, prev,mimic);
	Entries.push_back(tmp);
	PacketNum++;
	return true;
}

/* addEncoded
 * ----------
 * Adds an encoded packet to the buffer that consists of packet replicas and each replica
 * has its own utility.
 */
bool PacketBuffer::addEncoded(int* pktID, int* Dest, int* Src, double *Utils,int *Rep, int depth, double CurTime, int prev,int mimic,double *CrtTime)
{
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	/* Check if the encoded packet (with exactly the same contents) exists */
	if(match(pktID,depth))
	{
		return false;
	}
	/* In case that the buffer is finite, check if it is full */
	double drpCrtTime=0.0;
	PacketRecord *rec1;
	if(isFull())
	{
		drpCrtTime = DropPol->Drop();
		PacketNum--;
		numPacketDrops++;

		if(bufferRecording)
		{
			/* Keep a record of this action */
			if(TTL == 0)
			{
				rec1 = new PacketRecord(SGod->getSimTime(), -1, false, true, false);
				Records.push_back(rec1);
			}
			else
			{
				rec1 = new PacketRecord(SGod->getSimTime(), TTL - (SGod->getSimTime() - drpCrtTime), false, true, false);
				Records.push_back(rec1);
			}
		}
	}
	/* Add the new packet */
	PacketEntry *tmp = new PacketEntry(pktID, Dest,Src,Utils,Rep,depth,CrtTime,CurTime, prev,mimic);
	Entries.push_back(tmp);
	PacketNum++;
	return true;
}

/* keepAsEncoded
 * -------------
 * Creates a new encoded packet from packets PID1 and PID2. The two natives packets are removed (from
 * the buffer), while the new encoded packet is inserted to the top.
 */
void PacketBuffer::keepAsEncoded(int PID1, int PID2, double CurTime)
{
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	int *IDs=(int *)malloc(sizeof(int)*2);
	IDs[0]=PID1;
	IDs[1]=PID2;
	int *S=(int *)malloc(sizeof(int)*2);
	int *D=(int *)malloc(sizeof(int)*2);
	double *T=(double *)malloc(sizeof(double)*2);
	double *U=(double *)malloc(sizeof(double)*2);
	int *R=(int *)malloc(sizeof(int)*2);
	
	int both=0;
	int pos=0;
	list<PacketEntry*>::iterator it;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded))
		{
			if((*it)->pktID == PID1)
			{
				pos=0;
				both++;
			}
			else if((*it)->pktID == PID2)
			{
				pos=1;
				both++;
			}
			else
			{
				continue;
			}
			S[pos]=(*it)->Source;
			D[pos]=(*it)->Destination;
			T[pos]=(*it)->CreationTime;
			U[pos]=(*it)->GetMaxUtil();
			R[pos]=(*it)->GetReplicas();
			if(both == 2)
			{
				break;
			}
		}
	}
	if(both != 2)
	{
		printf("Problem: Native packets %d and %d are not stored!Aborting...\n",PID1,PID2);
		exit(1);
	}
	/* remove the two native packets */
	this->removeNativePkt(PID1);
	this->removeNativePkt(PID2);
	/* Add the new packet */
	PacketEntry *tmp = new PacketEntry(IDs,D,S,U,R,2,T,CurTime,NID,-1);
	Entries.push_back(tmp);
	PacketNum++;
	return;
}

/* removeNativePkt
 * ---------------
 * Removes a native packet with pktID from the buffer. 
 */
bool PacketBuffer::removeNativePkt(int pktID)
{
	PacketRecord *rec;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && (*it)->pktID == pktID)
		{
			if(bufferRecording)
			{
				/* Keep a record of this action */
				rec = new PacketRecord(SGod->getSimTime(), getRTTL(pktID), false, false, false);
				Records.push_back(rec);
			}
			delete (*it);
			Entries.erase(it);
			PacketNum--;
			return true;
		}
	}
	
	return false;
}

/* removePkt
 * ---------
 * Removes a packet with pktID from the buffer. If the packet is native then it is discarded from the
 * buffer. On the contrary, in case of an encoded packet the mimic flag is checked. 
 */
bool PacketBuffer::removePkt(int pktID)
{
	PacketRecord *rec;
	list<PacketEntry*>::iterator it= Entries.begin();
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	bool deleted=false;
	while(it != Entries.end())
	{
		deleted=false;
		if(!((*it)->Encoded) && (*it)->pktID == pktID)
		{
			if(bufferRecording)
			{
				/* Keep a record of this action */
				rec = new PacketRecord(SGod->getSimTime(), getRTTL(pktID), false, false, false);
				Records.push_back(rec);
			}
			#ifdef PACKET_BUFFER_DEBUG
			printf("Native!\n");
			#endif
			delete (*it);
			Entries.erase(it++);
			PacketNum--;
			deleted=true;
			//return true;
		}
		else if((*it)->Encoded)
		{
			if((*it)->mimic != -1)
			{
				if((*it)->EncodedPktIDs[(*it)->mimic] == pktID)
				{
					#ifdef PACKET_BUFFER_DEBUG
					printf("Encoded (with %d)!-Full delete.\n",(*it)->EncodedPktIDs[swapPos((*it)->mimic)]);
					#endif
					delete (*it);
					Entries.erase(it++);
					PacketNum--;
					deleted=true;
					//return true;
				}
			}
			else
			{
				for(int i=0;i<(*it)->CodingDepth;i++)
				{
					if((*it)->EncodedPktIDs[i] == pktID)
					{
						#ifdef PACKET_BUFFER_DEBUG
						printf("Encoded!-partial delete.\n");
						#endif
						(*it)->mimic=swapPos(i);
						//return true;
					}
				}
			}
		}
		if(!deleted)
		{
			++it;
		}
	}
	
	return false;
}

/* getDistanceFromBegin
 * --------------------
 * Returns how many packets are inserted after the packet with ID.
 */
int PacketBuffer::getDistanceFromBegin(int ID)
{
	int distance;
	list<PacketEntry*>::iterator it;


	distance = 0;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == ID)
		{
			return distance;
		}
		else
		{
			distance++;
		}
	}
	
	return -1;
}

/* getDistanceFromBegin
 * --------------------
 * Returns how many packets are inserted before the packet with ID.
 */
int PacketBuffer::getDistanceFromEnd(int ID)
{
	int distance;
	list<PacketEntry*>::reverse_iterator rit;


	distance = 0;
	for(rit = Entries.rbegin(); rit != Entries.rend(); ++rit)
	{
		if((*rit)->pktID == ID)
		{
			return distance;
		}
		else
		{
			distance++;
		}
	}
	
	return -1;
}


/* removeEncoded
 * -------------
 * Discards an encoded packet that contains a packet with pktID.
 */
bool PacketBuffer::removeEncoded(int* pktID, int depth)
{
	list<PacketEntry*>::iterator it;
	/* Check if the encoded packet (with exactly the same contents) exists */
	bool match=false;
	bool found=false;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		match=false;
		if((*it)->Encoded)
		{
			if(depth == (*it)->CodingDepth)
			{
				for(int i=0;i<depth;i++)
				{
					found=false;
					for(int j=0;j<depth;j++)
					{
						if(pktID[i] == (*it)->EncodedPktIDs[j])
						{
							found=true;
							break;
						}
					}
					if(!found)
					{
						match=false;
						break;
					}
					else
					{
						match=true;
					}
				}
			}
		}
		if(match)
		{
			delete (*it);
			Entries.erase(it);
			PacketNum--;
			return true;
		}
	}
	return false;
}

/* forwardedPkt
 * ------------
 * Enable the Forwarded flag of a native packet with pktID.
 */
bool PacketBuffer::forwardedPkt(int pktID)
{
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			(*it)->Forwarded = true;
			return true;
		}
	}
	
	return false;
}

/* isForwardedPkt
 * --------------
 * Returns the Forwarded flag of a native packet with pktID.
 */
bool PacketBuffer::isForwardedPkt(int pktID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return((*it)->Forwarded);
		}
	}
	
	return false;
}

/* NativePacketExists
 * ------------------
 * Checks if a native packet with ID exists inside the packet buffer.
 */
bool PacketBuffer::NativePacketExists(int ID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	if(Entries.empty())
	{
		return false;
	}
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && (*it)->pktID == ID)
		{
			return true;
		}
	}
	return false;
}

/* PacketExists
 * ------------
 * Returns if a native packet with ID exists inside the packet buffer. Also, this method checks
 * if the packet with ID is part of an encoded packet that exists inside the buffer.
 */
bool PacketBuffer::PacketExists(int ID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	if(Entries.empty())
	{
		return false;
	}
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && (*it)->pktID == ID)
		{
			return true;
		}
		else if((*it)->Encoded)
		{
			if((*it)->mimic == -1)
			{
				for(int i=0;i<(*it)->CodingDepth;i++)
				{
					if((*it)->EncodedPktIDs[i] == ID)
					{
						return true;
					}
				}
			}
			else
			{
				if((*it)->EncodedPktIDs[(*it)->mimic] == ID)
				{
					return true;
				}
			}
		}
	}
	return false;
}

/* PacketExistsAsExtra
 * -------------------
 * Checks if a packet with ID is part of an encoded packet (that exists inside the packet buffer). If such an 
 * encoded packet found to represent the native one (according to the mimic value) then the method returns true. 
 */
bool PacketBuffer::PacketExistsAsExtra(int ID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	if(Entries.empty())
	{
		return false;
	}
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			if((*it)->mimic != -1)
			{
				for(int i=0;i<(*it)->CodingDepth;i++)
				{
					if(i == (*it)->mimic)
					{
						continue;
					}
					if((*it)->EncodedPktIDs[i] == ID)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

/* UpdatePrevHop
 * -------------
 * Updates the previous hop attribute of the stored native packet with ID.
 */
bool PacketBuffer::UpdatePrevHop(int ID, int prev)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == ID && !((*it)->Encoded))
		{
			(*it)->AddPrevHop(prev);
			return true;
		}
	}
	
	return false;
}

/* isFull
 * ------
 * Checks if the buffer size is exceeded.
 */
bool PacketBuffer::isFull(void)
{
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	if((BufferSize > 0) && (Entries.size() == (unsigned int) BufferSize))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* getFrontPacket
 * --------------
 * Returns the id of the most recent packet received.
 */
int PacketBuffer::getFrontPacket(void)
{
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	return Entries.front()->pktID;
}

/* BufferStores
 * ------------
 * Returns the number of packets currently stored by the packet buffer.
 */
int PacketBuffer::BufferStores(void)
{
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	return (int) this->Entries.size();
}

/* GetHops
 * -------
 * Returns the hops attribute of the stored native packet with pktID.
 */
int PacketBuffer::GetHops(int pktID)
{
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(((*it)->pktID == pktID) && !((*it)->Encoded))
		{
			return (*it)->Hops;
		}
	}

	printf("Error (GetHops): Packet %d not found!\n", pktID);
	exit(EXIT_FAILURE);
}


/* SetHops
 * -------
 * Set the hops attribute of the stored native packet with pktID.
 */
void PacketBuffer::SetHops(int pktID, int updHops)
{
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(((*it)->pktID == pktID) && !((*it)->Encoded))
		{
			(*it)->setHops(updHops);

			return;
		}
	}

	printf("Error (SetHops): Packet %d not found!\n", pktID);
	exit(EXIT_FAILURE);
}


/* GetPrev
 * -------
 * Returns the prevHop attribute of the stored native packet with pktID.
 */
int PacketBuffer::GetPrev(int pktID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return (*it)->prevHop;
		}
	}

	printf("Error (GetPrev): Packet %d not found!\n",pktID);
	exit(EXIT_FAILURE);
}

/* GetPktDestination
 * -----------------
 * Returns the Destination attribute of the stored native packet with pktID. It also 
 * supports encoded packets.
 */
int PacketBuffer::GetPktDestination(int pktID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && (*it)->pktID == pktID)
		{
			return (*it)->Destination;
		}
		else if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == pktID)
				{
					return (*it)->EncodedPktDest[i];
				}
			}
		}
	}

	printf("Error (GetPktDestination): Packet %d not found!\n",pktID);
	exit(EXIT_FAILURE);
}

/* GetPktCreationTime
 * ------------------
 * Returns the CreationTime attribute of the stored native packet with pktID.
 */
double PacketBuffer::GetPktCreationTime(int pktID)
{
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return (*it)->CreationTime;
		}
	}

	printf("Error (GetPktCreationTime): Packet %d not found!\n",pktID);
	exit(EXIT_FAILURE);
}

/* getRTTL
 * -------
 * Returns the remaining ttl value for the stored native packet with pktID using its TimeToLive
 * attribute.
 */
double PacketBuffer::getRTTL(int pktID)
{
	double RTTL;
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			if(TTL == 0)
			{
				RTTL = -1;
			}
			else
			{
				RTTL = TTL - (SGod->getSimTime() - (*it)->CreationTime);
			}
			
			return RTTL;
		}
	}

	printf("Error (getRTTL): Packet %d not found!\n",pktID);
	exit(EXIT_FAILURE);
}

/* getProjNetGrowth
 * ------------
 * Returns the projected net growth in buffer occupancy over some interval
 */
double PacketBuffer::getProjNetGrowth(double interval)
{
	double meanNetGrowth;
	double projNetGrowth;
	unsigned long insertedPackets;
	unsigned long removedPackets;
	list<PacketRecord*>::reverse_iterator rit;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	insertedPackets = 0;
	removedPackets = 0;

	for(rit = Records.rbegin(); rit != Records.rend(); ++rit)
	{
		if(((*rit)->getRegistrationTime() >= (SGod->getSimTime() - interval)) || ((*rit)->getPacketRTTL() == -1))
		{
			if((*rit)->getInsertedPacket())
			{
				insertedPackets++;
			}
			else if(!(*rit)->getDroppedPacket() && !(*rit)->getDeadPacket())
			{
				removedPackets++;
			}
		}
		else if((*rit)->getRegistrationTime() < (SGod->getSimTime() - interval))
		{
			break;
		}
	}

	if(interval > DBL_EPSILON)
	{
		meanNetGrowth = ((double) (insertedPackets - removedPackets)) / interval;
		projNetGrowth = interval * meanNetGrowth;
	}
	else
	{
		meanNetGrowth = 0.0;
		projNetGrowth = 0.0;
	}

	return projNetGrowth;
}

/* getMeanRTTL
 * -----------
 * Returns the mean remaining ttl value for the stored packets in the buffer over some interval.
 */
double PacketBuffer::getMeanRTTL(double interval)
{
	double meanRTTL;
	double sumRTTL;
	unsigned long insertedPackets;
	list<PacketRecord*>::reverse_iterator rit;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	insertedPackets = 0;
	sumRTTL = 0.0;

	for(rit = Records.rbegin(); rit != Records.rend(); ++rit)
	{
		if(((*rit)->getRegistrationTime() >= (SGod->getSimTime() - interval)) || ((*rit)->getPacketRTTL() == -1))
		{
			if((*rit)->getInsertedPacket())
			{
				insertedPackets++;
				sumRTTL += (*rit)->getPacketRTTL();
			}
		}
		else if((*rit)->getRegistrationTime() < (SGod->getSimTime() - interval))
		{
			break;
		}
	}

	if(insertedPackets == 0)
	{
		meanRTTL = 0.0;
	}
	else
	{
		meanRTTL = sumRTTL / insertedPackets;
	}

	return meanRTTL;
}

/* getAllPackets
 * -------------
 * Returns an array that contains all the packet IDs (both native and encoded) that exist 
 * inside the packet buffer.
 */
int *PacketBuffer::getAllPackets(void)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos=0;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && !(this->exists(pkts,(*it)->pktID)))
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = (*it)->pktID;
			pkts[0] = pos;
		}
		else if((*it)->Encoded)
		{
			if((*it)->mimic == -1)
			{
				for(int i=0;i<(*it)->CodingDepth;i++)
				{
					if(!(this->exists(pkts,(*it)->EncodedPktIDs[i])))
					{
						pos++;
						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
						pkts[pos] = (*it)->EncodedPktIDs[i];
						pkts[0] = pos;
					}
				}
			}
			else
			{
				pos++;
				pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
				pkts[pos] = (*it)->EncodedPktIDs[(*it)->mimic];
				pkts[0] = pos;
			}
				
		}
		
	}
	return pkts;
}

/* getPacketsNotDestinedTo
 * -----------------------
 * Returns an array that contains all the packet IDs (both native and encoded) that exist 
 * inside the packet buffer and are destined to node with ID "destination".
 */
int *PacketBuffer::getPacketsNotDestinedTo(int destination)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos=0;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && !(this->exists(pkts,(*it)->pktID)))
		{
			if((*it)->Destination != destination)
			{
				pos++;
				pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
				pkts[pos] = (*it)->pktID;
				pkts[0] = pos;
			}
		}
		//temporarily disabled
// 		else if((*it)->Encoded)
// 		{
// 			if((*it)->mimic == -1)
// 			{
// 				for(int i=0;i<(*it)->CodingDepth;i++)
// 				{
// 					if(!(this->exists(pkts,(*it)->EncodedPktIDs[i])))
// 					{
// 						pos++;
// 						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
// 						pkts[pos] = (*it)->EncodedPktIDs[i];
// 						pkts[0] = pos;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				pos++;
// 				pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
// 				pkts[pos] = (*it)->EncodedPktIDs[(*it)->mimic];
// 				pkts[0] = pos;
// 			}
// 				
// 		}
		
	}
	return pkts;
}


/* getPktIDsandHopsNotDestinedTo
 * -----------------------
 * Returns an array that contains all the packet IDs and their current number
 * of hops (both native and encoded) that exist inside the packet buffer
 * and are destined to node with ID "destination".
 */
struct PktIDandHops *PacketBuffer::getPktIDsandHopsNotDestinedTo(int destination)
{
	int pos;
	struct PktIDandHops *pktData;
	list<PacketEntry*>::iterator it;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	pktData = (struct PktIDandHops *) malloc(sizeof(struct PktIDandHops));
	pktData[0].pktID = 0;
	pktData[0].pktHops = 0;
	pos = 0;

	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && !(this->exists(pktData, (*it)->pktID)))
		{
			if((*it)->Destination != destination)
			{
				pos++;
				pktData = (struct PktIDandHops *) realloc(pktData, (pos + 1) * sizeof(struct PktIDandHops));
				pktData[pos].pktID = (*it)->pktID;
				pktData[pos].pktHops = (*it)->Hops;
				pktData[0].pktID = pos;
				pktData[0].pktHops = pos;
			}
		}
	}

	return pktData;
}


/* getAllNativePackets
 * -------------------
 * Returns an array that contains all the native packet IDs that exist inside the packet buffer.
 */
int *PacketBuffer::getAllNativePackets(void)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos=0;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!((*it)->Encoded) && !(this->exists(pkts,(*it)->pktID)))
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = (*it)->pktID;
			pkts[0] = pos;
		}
	}
	return pkts;
}

/* OrderFIFO
 * ---------
 * Orders the packets given as input according to their distance from the top of the 
 * packet buffer.
 */
int* PacketBuffer::OrderFIFO(int* PIDs)
{
	int *newOrder=(int *)malloc(sizeof(int)*(PIDs[0]+1));
	newOrder[0]=PIDs[0];
	list<PacketEntry*>::iterator it;
	
	int runner=1;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		for(int i=1;i<=PIDs[0];i++)
		{
			if((*it)->pktID == PIDs[i] && !((*it)->Encoded))
			{
				newOrder[runner]=PIDs[i];
				runner++;
				PIDs[i]=-1;
				break;
			}
		}
	}
	//sanity check
	if((runner-1) != newOrder[0])
	{
		printf("Problem: Missing packets from packet buffer..Exiting..\n");
		exit(1);
	}
	return newOrder;
}

/* GetAllPrev
 * ----------
 * Returns the list of all previous hops that owned the packet with pktID.
 */
int *PacketBuffer::GetAllPrev(int pktID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return (*it)->GetAllPrevHops();
		}
	}
	
	printf("Error (GetAllPrev): Packet %d not found!\n",pktID);
	exit(EXIT_FAILURE);
}

/* GetAllFW
 * --------
 * Returns the list of all nodes that have forwarded the packet with pktID.
 */
int *PacketBuffer::GetAllFW(int pktID)
{
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return (*it)->GetAllFWs();
		}
	}
	
	printf("Error (GetAllFW): Packet %d not found!\n",pktID);
	exit(EXIT_FAILURE);
}

/* getAllDestinations
 * ------------------
 * Returns an array with all destinations that the current node carries a packet for.
 */
int *PacketBuffer::getAllDestinations(void)
{
	int i;
	int pos;
	int *Dests;
	bool found;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	Dests = (int *) malloc(sizeof(int));
	Dests[0] = 0;
	pos = 0;
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		found = false;
		if((*it)->Encoded)
		{
			continue;
		}
		for(i = 1; i <= pos; i++)
		{
			if(Dests[i] == (*it)->Destination)
			{
				found = true;
				break;
			}
		}
		
		if(!found)
		{
			pos++;
			Dests = (int *) realloc(Dests, (pos + 1) * sizeof(int));
			Dests[pos] = (*it)->Destination;
			Dests[0] = pos;
		}
	}
	
	return Dests;
}

/* getAllDestExcCrntEnc
 * --------------------
 * Returns an array with all destinations that the current node carries a packet for. The
 * only exception is node with id "crntEnc".
 */
int *PacketBuffer::getAllDestExcCrntEnc(int crntEnc)
{
	int i;
	int pos;
	int *Dests;
	bool found;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	Dests = (int *) malloc(sizeof(int));
	Dests[0] = 0;
	pos = 0;
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		found = false;
		if((*it)->Encoded || (*it)->Destination == crntEnc)
		{
			continue;
		}
		for(i = 1; i <= pos; i++)
		{
			if(Dests[i] == (*it)->Destination)
			{
				found = true;
				break;
			}
		}
		
		if(!found)
		{
			pos++;
			Dests = (int *) realloc(Dests, (pos + 1) * sizeof(int));
			Dests[pos] = (*it)->Destination;
			Dests[0] = pos;
		}
	}
	
	return Dests;
}

/* getPackets
 * ----------
 * Returns an array with packet IDs that are destined to node with id "Destination".
 */
int *PacketBuffer::getPackets(int Destination)
{
	int *Dst;
	int *res;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	Dst = (int *) malloc(2 * sizeof(int));
	Dst[0] = 1;
	Dst[1] = Destination;
	res = getPackets(Dst);
	free(Dst);
	
	return res;
}

/* getPackets
 * ----------
 * Returns an array with packet IDs that are destined to nodes which ids is given as
 * input.
 */
int *PacketBuffer::getPackets(int *Destinations)
{
	int i;
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	
	bool flag=false;
	int PID=0;
	for(i = 1; i <= Destinations[0]; i++)
	{
		#ifdef PACKET_BUFFER_DEBUG
			printf("Packets found for Destination %d:", Destinations[i]);
		#endif
		
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			flag=false;
			PID=0;
			if((*it)->Encoded)
			{
				if((*it)->mimic == -1)
				{
					for(int j=0;j<(*it)->CodingDepth;j++)
					{
						if(Destinations[i] == (*it)->EncodedPktDest[j] && !this->exists(pkts,(*it)->EncodedPktIDs[j]))
						{
							flag=true;
							PID=(*it)->EncodedPktIDs[j];
						}
					}
				}
				else
				{
					if(Destinations[i] == (*it)->EncodedPktDest[(*it)->mimic] && !this->exists(pkts,(*it)->EncodedPktIDs[(*it)->mimic]))
					{
						flag=true;
						PID=(*it)->EncodedPktIDs[(*it)->mimic];
					}
				}
			}
			else
			{
				if(Destinations[i] == (*it)->Destination && !this->exists(pkts,(*it)->pktID))
				{
					flag=true;
					PID=(*it)->pktID;
				}
			}
			
			if(flag)
			{
				#ifdef PACKET_BUFFER_DEBUG
					printf("%d ", (*it)->pktID);
				#endif
				pos++;
				pkts = (int *) realloc(pkts,(pos + 1) * sizeof(int));
				pkts[pos] = PID;
				pkts[0] = pos;
			}
		}
		
		#ifdef PACKET_BUFFER_DEBUG
			printf("\n");
		#endif
	}
	return pkts;
}

/* exists
 * ------
 * Auxiliary method: Checks if packet with PID exists in packet list PktList.
 */
bool PacketBuffer::exists(int *PktList, int PID)
{
	for(int i=1;i<=PktList[0];i++)
	{
		if(PktList[i] == PID)
		{
			return true;
		}
	}
	return false;
}


/* exists
 * ------
 * Auxiliary method: Checks if packet with PID exists in the given list of
 * packet IDs and hops.
 */
bool PacketBuffer::exists(struct PktIDandHops *pktData, int PID)
{
	int i;


	for(i = 1; i <= pktData[0].pktID; i++)
	{
		if(pktData[i].pktID == PID)
		{
			return true;
		}
	}

	return false;
}


/* position
 * --------
 * Auxiliary method: Return the position of packet with PID insided the packet list PktList.
 */
bool PacketBuffer::position(int* PktList, int PID)
{
	for(int i=1;i<=PktList[0];i++)
	{
		if(PktList[i] == PID)
		{
			PktList[i]=-1;
			return i;
		}
	}
	return (-1);
}

/* getPackets
 * ----------
 * Checks all packets destined to a list of destinations given as input (Destinations). Then, it returns
 * an array containing the packet ids that have larger utility value (input:Utils) than the utility
 * value stored for the packet. Note that, no update is made to the stored utility value. 
 */
int *PacketBuffer::getPackets(int *Destinations, double *Utils)
{
	int i;
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	
	for(i = 1; i <= Destinations[0]; i++)
	{
		#ifdef PACKET_BUFFER_DEBUG
			printf("Packets found for Destination %d:", Destinations[i]);
		#endif
		
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			if((*it)->Encoded)
			{
				continue;
			}
			if((Destinations[i] == (*it)->Destination) && (Utils[i] > (*it)->GetMaxUtil()))
			{
				#ifdef PACKET_BUFFER_DEBUG
					printf("%d ", (*it)->pktID);
				#endif
				
				pos++;
				pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
				pkts[pos] = (*it)->pktID;
				pkts[0] = pos;
			}
		}
		
		#ifdef PACKET_BUFFER_DEBUG
			printf("\n");
		#endif
	}
	return pkts;
}

/* getPacketsLowUtil
 * -----------------
 * Returns a subset of packets given as argument (p). This subset corresponds to the packets 
 * that have lower util value (this value is saved by every node for each packet - maxUtil) 
 * than the util value that is given as an argument (Utils). Note that, an update is made if
 * the utility value (given as input) is larger than the stored one.
 */
int *PacketBuffer::getPacketsLowUtil(int *p, double *encUtils, bool *Owned)
{
	int i;
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	for(i = 1; i <= p[0]; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			if(p[i] == (*it)->pktID && !((*it)->Encoded))
			{
				//printf("Util:%f vs My utility level:%f\n",Utils[i],(*it)->GetMaxUtil());
				if(encUtils[i] - (*it)->GetMaxUtil() > DBL_EPSILON)
				{
					(*it)->SetMaxUtil(encUtils[i]);
					if(!(Owned[i]))
					{
						pos++;
						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
						pkts[pos] = (*it)->pktID;
						pkts[0] = pos;
					}
				}
			}
		}
	}
	return pkts;
}

/* getPacketsLowUtilSimBetTS
 * -------------------------
 * Returns an array of packet ids that is a subset of the receivedPkts (given as input). The packets returned
 * have larger SimBetTS utility than the SimBetTS utility stored. Note that, an update is made if the SimBetTS
 * utility value (given as input) is larger than the stored one.
 */
int *PacketBuffer::getPacketsLowUtilSimBetTS(int numReceived, struct PktMultiUtil *receivedPkts)
{
	int i;
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	double perceivedSim=0.0;
	double perceivedBet=0.0;
	double perceivedFreq=0.0;
	double perceivedIntim=0.0;
	double perceivedRec=0.0;
	double perceivedUtil=0.0;
	double encUtil=0.0;
	for(i = 0; i < numReceived; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			if(receivedPkts[i].PID == (*it)->pktID && !((*it)->Encoded))
			{
				perceivedSim=(*it)->GetMaxUtil("Sim");
				perceivedBet=(*it)->GetMaxUtil("Bet");
				perceivedFreq=(*it)->GetMaxUtil("Freq");
				perceivedIntim=(*it)->GetMaxUtil("Intimacy");
				perceivedRec=(*it)->GetMaxUtil("Recency");
				perceivedUtil=CalculateSimBetTSUtility(perceivedSim,perceivedBet,perceivedFreq,perceivedIntim,perceivedRec,receivedPkts[i].Sim,receivedPkts[i].Bet,receivedPkts[i].Frequency,receivedPkts[i].Intimacy,receivedPkts[i].Recency);
				encUtil=CalculateSimBetTSUtility(receivedPkts[i].Sim,receivedPkts[i].Bet,receivedPkts[i].Frequency,receivedPkts[i].Intimacy,receivedPkts[i].Recency,perceivedSim,perceivedBet,perceivedFreq,perceivedIntim,perceivedRec);
				
				if(encUtil - perceivedUtil > DBL_EPSILON)
				{
					(*it)->UpdateMaxUtil("Sim",receivedPkts[i].Sim);
					(*it)->UpdateMaxUtil("Bet",receivedPkts[i].Bet);
					(*it)->UpdateMaxUtil("Freq",receivedPkts[i].Frequency);
					(*it)->UpdateMaxUtil("Intimacy",receivedPkts[i].Intimacy);
					(*it)->UpdateMaxUtil("Recency",receivedPkts[i].Recency);
					if(!(receivedPkts[i].Exists))
					{
						pos++;
						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
						pkts[pos] = (*it)->pktID;
						pkts[0] = pos;
					}
				}
			}
		}
	}
	return pkts;
}

/* getPacketsLowUtilSimBet
 * -----------------------
 * Returns an array of packet ids that is a subset of the receivedPkts (given as input). The packets returned
 * have larger SimBet utility than the SimBet utility stored. Note that, an update is made if the SimBet
 * utility value (given as input) is larger than the stored one.
 */
int *PacketBuffer::getPacketsLowUtilSimBet(int numReceived, PktMultiUtil *receivedPkts)
{
	int i;
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	double perceivedSim=0.0;
	double perceivedBet=0.0;
	double perceivedUtil=0.0;
	double encUtil=0.0;
	for(i = 0; i < numReceived; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			if(receivedPkts[i].PID == (*it)->pktID && !((*it)->Encoded))
			{
				perceivedSim=(*it)->GetMaxUtil("Sim");
				perceivedBet=(*it)->GetMaxUtil("Bet");
				perceivedUtil=CalculateSimBetUtility(perceivedSim,perceivedBet,receivedPkts[i].Sim,receivedPkts[i].Bet);
				encUtil=CalculateSimBetUtility(receivedPkts[i].Sim,receivedPkts[i].Bet,perceivedSim,perceivedBet);
				
				if(encUtil - perceivedUtil > DBL_EPSILON)
				{
					(*it)->UpdateMaxUtil("Sim",receivedPkts[i].Sim);
					(*it)->UpdateMaxUtil("Bet",receivedPkts[i].Bet);
					if(!(receivedPkts[i].Exists))
					{
						pos++;
						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
						pkts[pos] = (*it)->pktID;
						pkts[0] = pos;
					}
				}
			}
		}
	}
	return pkts;
}




/* getPacketsLowUtil
 * ----------------
 * Returns a subset of packets given as an argument (p). This subset
 * corresponds to the packets that have lower util value (this value 
 * is saved by every node for each packet - maxUtil) than the util
 * value that is given as an argument (Utils). If the quota value for a
 * packet is larger than 1 then include packets that have util values larger
 * than node's util value (and not maxUtil).  
 */
// int *PacketBuffer::getPacketsLowUtilQuota(int *p, double* Utils, bool *Owned, double *CurrentUtils)
// {
// 	int i;
// 	int pos;
// 	int *pkts;
// 	list<PacketEntry*>::iterator it;
// 	
// 	
// 	/* Delete the outdated packets because of the TTL value */
// 	DropDeadPackets();
// 	
// 	pkts = (int *) malloc(sizeof(int));
// 	pkts[0] = 0;
// 	pos = 0;
// 	bool better=false;
// 	for(i = 1; i <= p[0]; i++)
// 	{
// 		for(it = Entries.begin(); it != Entries.end(); ++it)
// 		{
// 			better=false;
// 			if(p[i] == (*it)->pktID && !((*it)->Encoded))
// 			{
// 				//printf("Util:%f vs My utility level:%f\n",Utils[i],(*it)->GetMaxUtil());
// 				
// 				//check if the utility of neighbor is better than all the utilities seen so far 
// 				if(Utils[i] - (*it)->GetMaxUtil() > DBL_EPSILON)
// 				{
// 					better=true;
// 					(*it)->SetMaxUtil(Utils[i]);
// 				}
// 				if(!(Owned[i]))
// 				{
// 					if(better==true || ((*it)->Replicas > 1 && Utils[i] - CurrentUtils[(*it)->Destination]) > DBL_EPSILON)
// 					{
// 						pos++;
// 						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
// 						pkts[pos] = (*it)->pktID;
// 						pkts[0] = pos;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	
// 	return pkts;
// }

/* getPacketsLowUtilQuota
 * ----------------------
 * Returns a subset of the packets given as input (p). The packets returned have larger utility
 * value (Utils) than the stored utility. Also, the returned packets should not exists in the Owned
 * array that denotes that the encountered node already holds a packet replica. Note that, an update 
 * is made (regardless the Owned value) if the utility value (given as input) is larger than the stored one.
 */
struct PktsWon *PacketBuffer::getPacketsLowUtilQuota(int *p, double* Utils, bool *Owned, double *CurrentUtils)
{
	int i;
	struct PktsWon *pkts=NULL;
	struct PktsWon *tmp=NULL;
	struct PktsWon *prev=NULL;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	bool better=false;
	for(i = 1; i <= p[0]; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			better=false;
			if(p[i] == (*it)->pktID && !((*it)->Encoded))
			{
// 				printf("ID:%d Encountered util:%f vs My threshold level:%f\n",p[i],Utils[i],(*it)->GetMaxUtil());
				
				if(Utils[i] < 0.0)
				{
					//recover utility
// 					double tmp=Utils[i]*(-1.0);
// 					if(tmp > (*it)->GetMaxUtil())
// 					{
// 						(*it)->SetMaxUtil(tmp);
// 					}
					continue;
				}
				//check if the utility of neighbor is better than all the utilities seen so far 
				if(Utils[i] - (*it)->GetMaxUtil() > DBL_EPSILON)
				{
					better=true;
					(*it)->SetMaxUtil(Utils[i]);
				}
				if(!(Owned[i]))
				{
// 					printf("Encountered node does not have packet %d\n",p[i]);
					if(better==true || (Utils[i] - CurrentUtils[(*it)->Destination] > DBL_EPSILON))
					{
						tmp = (struct PktsWon *)malloc(sizeof(struct PktsWon));
						tmp->PktID=(*it)->pktID;
						tmp->type=0;
						tmp->next=NULL;
						if(prev != NULL)
						{
							prev->next=tmp;
						}
						else
						{
							pkts=tmp;
						}
						prev=tmp;
					}
					if(better==true)
					{
// 						printf("Packet is marked as type 1\n");
						tmp->type=1;
					}
					else if(Utils[i] - CurrentUtils[(*it)->Destination] > DBL_EPSILON)
					{
// 						printf("Packet is marked as type 0\n");
						tmp->type=0;
					}
				}
			}
		}
	}
	
	return pkts;
}

/* getPacketsLowUtilSimBetTS
 * -------------------------
 * Returns an array of packet ids that is a subset of the receivedPkts (given as input). The packets returned
 * have larger SimBetTS utility than the SimBetTS utility stored. Note that, an update is made if the SimBetTS
 * utility value (given as input) is larger than the stored one.
 */
struct PktsWon *PacketBuffer::getPacketsLowUtilSimBetTS(int numReceived,struct PktMultiUtil *receivedPkts,struct SimBetTSmetrics *LocalU)
{
	int i;
	struct PktsWon *pkts=NULL;
	struct PktsWon *tmp=NULL;
	struct PktsWon *prev=NULL;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	bool better=false;
	double perceivedSim=0.0;
	double perceivedBet=0.0;
	double perceivedFreq=0.0;
	double perceivedIntim=0.0;
	double perceivedRec=0.0;
	double perceivedUtil=0.0;
	double encUtil=0.0;
	double localSim=0.0;
	double localBet=0.0;
	double localFreq=0.0;
	double localIntim=0.0;
	double localRec=0.0;
	double localUtil=0.0;
	for(i = 0; i < numReceived; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			better=false;
			if(receivedPkts[i].PID == (*it)->pktID && !((*it)->Encoded))
			{
				
				if(receivedPkts[i].Sim < 0)
				{
					//recover utility
// 					double tmp=Utils[i]*(-1.0);
// 					if(tmp > (*it)->GetMaxUtil())
// 					{
// 						(*it)->SetMaxUtil(tmp);
// 					}
					continue;
				}
				
				perceivedSim=(*it)->GetMaxUtil("Sim");
				perceivedBet=(*it)->GetMaxUtil("Bet");
				perceivedFreq=(*it)->GetMaxUtil("Freq");
				perceivedIntim=(*it)->GetMaxUtil("Intimacy");
				perceivedRec=(*it)->GetMaxUtil("Recency");
				perceivedUtil=CalculateSimBetTSUtility(perceivedSim,perceivedBet,perceivedFreq,perceivedIntim,perceivedRec,receivedPkts[i].Sim,receivedPkts[i].Bet,receivedPkts[i].Frequency,receivedPkts[i].Intimacy,receivedPkts[i].Recency);
				
				encUtil=CalculateSimBetTSUtility(receivedPkts[i].Sim,receivedPkts[i].Bet,receivedPkts[i].Frequency,receivedPkts[i].Intimacy,receivedPkts[i].Recency,perceivedSim,perceivedBet,perceivedFreq,perceivedIntim,perceivedRec);
				
				if(encUtil - perceivedUtil > DBL_EPSILON)
				{
					better=true;
					(*it)->UpdateMaxUtil("Sim",receivedPkts[i].Sim);
					(*it)->UpdateMaxUtil("Bet",receivedPkts[i].Bet);
					(*it)->UpdateMaxUtil("Freq",receivedPkts[i].Frequency);
					(*it)->UpdateMaxUtil("Intimacy",receivedPkts[i].Intimacy);
					(*it)->UpdateMaxUtil("Recency",receivedPkts[i].Recency);
				}
				if(!(receivedPkts[i].Exists))
				{
					localSim=LocalU->Similarity;
					localBet=LocalU->Betweenness;
					localFreq=LocalU->Frequency;
					localIntim=LocalU->Intimacy;
					localRec=LocalU->Recency;
					
					localUtil=CalculateSimBetTSUtility(localSim,localBet,localFreq,localIntim,localRec,receivedPkts[i].Sim,receivedPkts[i].Bet,receivedPkts[i].Frequency,receivedPkts[i].Intimacy,receivedPkts[i].Recency);
					
// 					printf("Encountered node does not have packet %d\n",p[i]);
					if(better==true || (encUtil - localUtil > DBL_EPSILON))
					{
						tmp = (struct PktsWon *)malloc(sizeof(struct PktsWon));
						tmp->PktID=(*it)->pktID;
						tmp->type=0;
						tmp->next=NULL;
						if(prev != NULL)
						{
							prev->next=tmp;
						}
						else
						{
							pkts=tmp;
						}
						prev=tmp;
					}
					if(better==true)
					{
// 						printf("Packet is marked as type 1\n");
						tmp->type=1;
					}
					else if(encUtil - localUtil > DBL_EPSILON)
					{
// 						printf("Packet is marked as type 0\n");
						tmp->type=0;
					}
				}
			}
		}
	}
	return pkts;
}

/* CalculateSimBetTSUtility
 * ------------------------
 * Calculates the SimBetTS utility from its components for the two nodes in contact.
 */
double PacketBuffer::CalculateSimBetTSUtility(double SimA, double BetA, double FreqA, double IntimacyA, double RecencyA,double SimB, double BetB, double FreqB, double IntimacyB, double RecencyB)
{
	double SimUtil=0.0;
	double BetUtil=0.0;
	double TSA=FreqA+IntimacyA+RecencyA;
	double TSB=FreqB+IntimacyB+RecencyB;
	double TSUtil=0.0;
	double finalUtil=0.0;
	
	SimUtil=SimA/(SimA+SimB);
	BetUtil=BetA/(BetA+BetB);
	TSUtil=TSA/(TSA+TSB);
	finalUtil=SimUtil+BetUtil+TSUtil;
	return finalUtil;
}

/* getPacketsLowUtilSimBet
 * -----------------------
 * Returns an array of packet ids that is a subset of the receivedPkts (given as input). The packets returned
 * have larger SimBet utility than the SimBet utility stored. Note that, an update is made if the SimBet
 * utility value (given as input) is larger than the stored one.
 */
PktsWon* PacketBuffer::getPacketsLowUtilSimBet(int numReceived, PktMultiUtil* receivedPkts, SimBetTSmetrics* LocalU)
{
	int i;
	struct PktsWon *pkts=NULL;
	struct PktsWon *tmp=NULL;
	struct PktsWon *prev=NULL;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	bool better=false;
	double perceivedSim=0.0;
	double perceivedBet=0.0;
	double perceivedUtil=0.0;
	double encUtil=0.0;
	double localSim=0.0;
	double localBet=0.0;
	double localUtil=0.0;
	for(i = 0; i < numReceived; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
			better=false;
			if(receivedPkts[i].PID == (*it)->pktID && !((*it)->Encoded))
			{
				
				if(receivedPkts[i].Sim < 0)
				{
					//recover utility
// 					double tmp=Utils[i]*(-1.0);
// 					if(tmp > (*it)->GetMaxUtil())
// 					{
// 						(*it)->SetMaxUtil(tmp);
// 					}
					continue;
				}
				
				perceivedSim=(*it)->GetMaxUtil("Sim");
				perceivedBet=(*it)->GetMaxUtil("Bet");
				perceivedUtil=CalculateSimBetUtility(perceivedSim,perceivedBet,receivedPkts[i].Sim,receivedPkts[i].Bet);
				
				encUtil=CalculateSimBetUtility(receivedPkts[i].Sim,receivedPkts[i].Bet,perceivedSim,perceivedBet);
				
				if(encUtil - perceivedUtil > DBL_EPSILON)
				{
					better=true;
					(*it)->UpdateMaxUtil("Sim",receivedPkts[i].Sim);
					(*it)->UpdateMaxUtil("Bet",receivedPkts[i].Bet);
					(*it)->UpdateMaxUtil("Freq",receivedPkts[i].Frequency);
					(*it)->UpdateMaxUtil("Intimacy",receivedPkts[i].Intimacy);
					(*it)->UpdateMaxUtil("Recency",receivedPkts[i].Recency);
				}
				if(!(receivedPkts[i].Exists))
				{
					localSim=LocalU->Similarity;
					localBet=LocalU->Betweenness;
					localUtil=CalculateSimBetUtility(localSim,localBet,receivedPkts[i].Sim,receivedPkts[i].Bet);
					
// 					printf("Encountered node does not have packet %d\n",p[i]);
					if(better==true || (encUtil - localUtil > DBL_EPSILON))
					{
						tmp = (struct PktsWon *)malloc(sizeof(struct PktsWon));
						tmp->PktID=(*it)->pktID;
						tmp->type=0;
						tmp->next=NULL;
						if(prev != NULL)
						{
							prev->next=tmp;
						}
						else
						{
							pkts=tmp;
						}
						prev=tmp;
					}
					if(better==true)
					{
// 						printf("Packet is marked as type 1\n");
						tmp->type=1;
					}
					else if(encUtil - localUtil > DBL_EPSILON)
					{
// 						printf("Packet is marked as type 0\n");
						tmp->type=0;
					}
				}
			}
		}
	}
	return pkts;
}

/* CalculateSimBetUtility
 * ----------------------
 * Calculates the SimBet utility from its components for the two nodes in contact.
 */
double PacketBuffer::CalculateSimBetUtility(double SimA, double BetA, double SimB, double BetB)
{
	double SimUtil=0.0;
	double BetUtil=0.0;
	double finalUtil=0.0;
	
	SimUtil=SimA/(SimA+SimB);
	BetUtil=BetA/(BetA+BetB);
	finalUtil=(0.5*SimUtil)+(0.5*BetUtil);
	return finalUtil;
}

/* UpdateLowUtil
 * -------------
 * For all packets given as input and their corresponding utility values (p,Utils) an
 * update is made if the given utility value is larger than the stored one.
 */
void PacketBuffer::UpdateLowUtil(int *p, double *Utils)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(int i = 1; i <= p[0]; i++)
	{
		for(it = Entries.begin(); it != Entries.end(); ++it)
		{
				if(!((*it)->Encoded) && p[i] == (*it)->pktID)
				{
					if(Utils[i] > (*it)->GetMaxUtil())
					{
						(*it)->SetMaxUtil(Utils[i]);
					}
				}
				else if((*it)->Encoded)
				{
					if((*it)->mimic == -1)
					{
						for(int j=0;j<(*it)->CodingDepth;j++)
						{
							if((*it)->EncodedPktIDs[j] == p[i] && Utils[i] > (*it)->EncodedMaxUtil[j])
							{
								(*it)->EncodedMaxUtil[j]=Utils[i];
							}
						}
					}
					else
					{
						if((*it)->EncodedPktIDs[(*it)->mimic] == p[i] && Utils[i] > (*it)->EncodedMaxUtil[(*it)->mimic])
						{
							(*it)->EncodedMaxUtil[(*it)->mimic]=Utils[i];
						}
					}
				}
		}
	}
	return;
}

/* DropDeadPackets
 * ---------------
 * All packets that have an expired TTL value are discarded. For the encoded packets the mimic flag
 * changes accordingly. 
 */
void PacketBuffer::DropDeadPackets(void)
{
	PacketRecord *rec;
	list<PacketEntry*>::iterator it;
	
	
	if(TTL == 0)
	{
		return;
	}
	
	bool delflag=false;
	int pos=-1;
	for(it=Entries.begin(); it!=Entries.end(); /* Do nothing */)
	{
		if((*it)->Encoded)
		{
			pos=-1;
			delflag=false;
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((SGod->getSimTime() - (*it)->EncodedCreationTime[i]) > TTL)
				{
					if((*it)->mimic == -1)
					{
						(*it)->mimic=swapPos(i);
					}
					else if((*it)->mimic == i)
					{
						delflag=true;
						pos=i;
						break;
					}
				}
			}
			if(delflag)
			{
				Stat->incPktsDied((*it)->EncodedCreationTime[pos]);
				delete (*it);
				it = Entries.erase(it);
				PacketNum--;
			}
			else
			{
				++it;
			}
		}
		else
		{
			if((SGod->getSimTime() - (*it)->CreationTime) > TTL)
			{
				if(bufferRecording)
				{
					/* Keep a record of this action */
					rec = new PacketRecord(SGod->getSimTime(), 0.0, false, false, true);
					Records.push_back(rec);
				}
				
				Stat->incPktsDied((*it)->CreationTime);
				delete (*it);
				it = Entries.erase(it);
				PacketNum--;
			}
			else
			{
				++it;
			}
		}
	}
}

/* UpdateThresholdDD
 * -----------------
 * Updates the stored destination dependent utility value (threshold) for all packets that are destined 
 * to node with id "Destination". The update is made only if the utility value given as input (Util) is 
 * larger than the stored one.
 */
void PacketBuffer::UpdateThresholdDD(int Destination, double Util)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(Destination == (*it)->Destination)
		{
			//printf("%d: Packet %d(intime:%f) destined to node %d\n",this->NID,(*it)->pktID,(*it)->InsTime,(*it)->Destination);
			if(Util - (*it)->GetMaxUtil() > DBL_EPSILON)
			{
				(*it)->SetMaxUtil(Util);
			}
		}
	}
	return;
}

/* UpdateThresholdDD
 * -----------------
 * Updates the stored destination dependent utility value (threshold) for all packets. The new utility values
 * are given in an array form, where each position denotes the destination node. Note that the update is 
 * made only if the utility value given as input (Utils[]) is larger than the stored one.
 */
void PacketBuffer::UpdateThresholdDD(double *Utils)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(Utils[(*it)->Destination] - (*it)->GetMaxUtil() > DBL_EPSILON)
		{
			(*it)->SetMaxUtil(Utils[(*it)->Destination]);
		}
	}
	return;
}

/* UpdateThresholdDI
 * -----------------
 * Updates the stored destination independent utility value (threshold) for all packets. Note that the update is 
 * made only if the utility value given as input (Util) is larger than the stored one.
 */
void PacketBuffer::UpdateThresholdDI(double Util)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(Util - (*it)->GetMaxUtil() > DBL_EPSILON)
		{
			(*it)->SetMaxUtil(Util);
		}
	}
	return;
}

/* UpdateThresholdSimBetTS
 * -----------------------
 * Updates the stored SimBetTS utility value. Note that the update is made only if the SimBetTS
 * utility value given as input (Util) is larger than the stored one.
 */
void PacketBuffer::UpdateThresholdSimBetTS(SimBetTSmetrics* localMetrics)
{
	double perceivedSim=0.0;
	double perceivedBet=0.0;
	double perceivedFreq=0.0;
	double perceivedIntim=0.0;
	double perceivedRec=0.0;
	double perceivedUtil=0.0;
	double localSim=0.0;
	double localBet=0.0;
	double localFreq=0.0;
	double localIntim=0.0;
	double localRec=0.0;
	double localUtil=0.0;
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		perceivedSim=(*it)->GetMaxUtil("Sim");
		perceivedBet=(*it)->GetMaxUtil("Bet");
		perceivedFreq=(*it)->GetMaxUtil("Freq");
		perceivedIntim=(*it)->GetMaxUtil("Intimacy");
		perceivedRec=(*it)->GetMaxUtil("Recency");
		
		localSim=localMetrics[(*it)->Destination].Similarity;
		localBet=localMetrics[(*it)->Destination].Betweenness;
		localFreq=localMetrics[(*it)->Destination].Frequency;
		localIntim=localMetrics[(*it)->Destination].Intimacy;
		localRec=localMetrics[(*it)->Destination].Recency;
		
		perceivedUtil=CalculateSimBetTSUtility(perceivedSim,perceivedBet,perceivedFreq,perceivedIntim,perceivedRec,localSim,localBet,localFreq,localIntim,localRec);
		localUtil=CalculateSimBetTSUtility(localSim,localBet,localFreq,localIntim,localRec,perceivedSim,perceivedBet,perceivedFreq,perceivedIntim,perceivedRec);
		
		if(localUtil - perceivedUtil > DBL_EPSILON)
		{
			(*it)->UpdateMaxUtil("Sim",localSim);
			(*it)->UpdateMaxUtil("Bet",localBet);
			(*it)->UpdateMaxUtil("Freq",localFreq);
			(*it)->UpdateMaxUtil("Intimacy",localIntim);
			(*it)->UpdateMaxUtil("Recency",localRec);
		}
	}
	return;
}

/* PrintPkts
 * ---------
 * Prints the packets that exist in the packet buffer (both native and encoded).
 */
void PacketBuffer::PrintPkts(void)
{
	list<PacketEntry*>::iterator it;
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if(!(*it)->Encoded)
		{
			printf("(Native)pkt ID:%d\tInserted time:%f\tSource:%d\tDestination:%d\tprev hop:%d\tReplicas:%d\tMaxUtil:%f\n", (*it)->pktID, (*it)->InsTime,(*it)->Source ,(*it)->Destination, (*it)->prevHop, (*it)->Replicas,(*it)->GetMaxUtil());
		}
		else
		{
			printf("(Encoded)pkt IDs:%d-%d mimic:%d\tSource:%d-%d\tDestination:%d-%d\tReplicas:%d-%d\tMaxUtil:%f-%f\n", (*it)->EncodedPktIDs[0], (*it)->EncodedPktIDs[1],(*it)->mimic,(*it)->EncodedPktSource[0],(*it)->EncodedPktSource[1],(*it)->EncodedPktDest[0],(*it)->EncodedPktDest[1],(*it)->EncodedReplicationValue[0],(*it)->EncodedReplicationValue[1],(*it)->EncodedMaxUtil[0],(*it)->EncodedMaxUtil[1]);
		}
	}
	
	return;
}

/* printBufferContents
 * -------------------
 * Prints the packets that exist in the packet buffer (only native).
 */
void PacketBuffer::printBufferContents()
{
	int counter;
	list<PacketEntry*>::iterator it;
	
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	printf("Begin of the buffer with size of %d packets\n", getBufferSize());
	counter = 0;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		counter++;
		printf("#%d: packetID = %d\tSourceNode = %d\tDestinationNode = %d\tPreviousHop = %d\tInsertionTime = %f\tCreationTime = %f\n", counter, (*it)->pktID, (*it)->Source, (*it)->Destination, (*it)->prevHop, (*it)->InsTime, (*it)->CreationTime);
	}
	printf("End of the buffer with residual space of %d packets\n", getBufferSize() - getBufferLength());
	
	return;
}

/* getPacketData
 * -------------
 * Returns a pointer to a PacketEntry object that contains the data of packet with id
 * pktID.
 */
PacketEntry *PacketBuffer::getPacketData(int pktID)
{
	list<PacketEntry*>::iterator it;
	
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == pktID && !((*it)->Encoded))
		{
			return (*it);
		}
	}
	printf("%d - Error (GetPacketData): Packet %d not found!\n",this->NID,pktID);
	exit(EXIT_FAILURE);
}

/* getEncodedDepth
 * ---------------
 * Returns the number of native packets included in the encoded packet with id PID.
 */
int PacketBuffer::getEncodedDepth(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->CodingDepth;
				}
			}
		}
	}
	printf("Error (GetEncodedDepth): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getEncodedIDs
 * -------------
 * Returns the ids of the native packets included in the encoded packet with id PID.
 */
int *PacketBuffer::getEncodedIDs(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedPktIDs;
				}
			}
		}
	}
	printf("Error (getEncodedIDs): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getRepVal
 * ---------
 * Returns the replica value of packet with id PID (supports both native and
 * encoded packets).
 */
int PacketBuffer::getRepVal(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedReplicationValue[i];
				}
			}
		}
		else if((*it)->pktID == PID)
		{
			return (*it)->GetReplicas();
		}
	}
	printf("Error (getRepVal): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getEncRepVal
 * ------------
 * Returns the replica value of a native packet with id PID that lies within
 * an encoded packet. Note that the position inside the encoded packets should be given as input.
 */
int PacketBuffer::getEncRepVal(int PID, int pos)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedReplicationValue[pos];
				}
			}
		}
	}
	printf("Error (getEncRepVal): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* setEncRepVal
 * ------------
 * Sets the replica value of the encoded packet with id PID. Note that the position
 * inside the encoded packets should be given as input.
 */
void PacketBuffer::setEncRepVal(int PID, int pos, int val)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					(*it)->EncodedReplicationValue[pos]=val;
					return;
				}
			}
		}
	}
	printf("Error (setEncRepVal): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getEncRepVal
 * ------------
 * Returns the replica value of a native packet with id PID that lies within
 * an encoded packet.
 */
int PacketBuffer::getEncRepVal(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedReplicationValue[i];
				}
			}
		}
	}
	printf("Error (getEncRepVal): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* setEncRepVal
 * ------------
 * Sets the replica value of the encoded packet with id PID.
 */
void PacketBuffer::setEncRepVal(int PID, int val)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					(*it)->EncodedReplicationValue[i]=val;
					return;
				}
			}
		}
	}
	printf("Error (setEncRepVal): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getEncodedRepVals
 * -----------------
 * Returns the replica value of the encoded packet with id PID itself.
 */
int *PacketBuffer::getEncodedRepVals(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedReplicationValue;
				}
			}
		}
	}
	printf("Error (getEncodedIDs): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getMimic
 * --------
 * Returns the mimic flag used in an encoded packet that contains a native
 * packet with id PID. 
 */
int PacketBuffer::getMimic(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->mimic;
				}
			}
		}
	}
	printf("Error (getSignificance): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getPacketDest
 * -------------
 * Returns the destination node of the packet with id PID (supports both native and
 * encoded packets).
 */
int PacketBuffer::getPacketDest(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == PID && !((*it)->Encoded))
		{
			return (*it)->getDestination();
		}
		else if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedPktDest[i];
				}
			}
		}
	}
	printf("Error (GetPacketDest): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* getPacketSource
 * ---------------
 * Returns the source node of the packet with id PID (supports both native and
 * encoded packets).
 */
int PacketBuffer::getPacketSource(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == PID && !((*it)->Encoded))
		{
			return (*it)->getSource();
		}
		else if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return (*it)->EncodedPktSource[i];
				}
			}
		}
	}
	printf("Error (GetPacketSource): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* isEncoded
 * ---------
 * Returns whether a packet is part of an encoded packet or not.
 */
bool PacketBuffer::isEncoded(int PID)
{
	list<PacketEntry*>::iterator it;
	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->pktID == PID && !((*it)->Encoded))
		{
			return false;
		}
		else if((*it)->Encoded)
		{
			for(int i=0;i<(*it)->CodingDepth;i++)
			{
				if((*it)->EncodedPktIDs[i] == PID)
				{
					return true;
				}
			}
		}
	}
	printf("Error (isEncoded): Packet %d not found!\n",PID);
	exit(EXIT_FAILURE);
}

/* ActivatePackets
 * ---------------
 * Activates packets given as input (IDs[]) using the mimic flag of the corresponding
 * encoded packets. 
 */
void PacketBuffer::ActivatePackets(int *IDs)
{
	list<PacketEntry*>::iterator it;
	int candidate=-1;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			if((*it)->mimic != -1)
			{
				candidate=swapPos((*it)->mimic);
				if(exists(IDs, (*it)->EncodedPktIDs[candidate]))
				{
					(*it)->mimic=-1;
				}
			}
		}
	}
	return;
}

/* ActivatePackets
 * ---------------
 * Activates packets given as input (IDs[]) using the mimic flag of the corresponding
 * encoded packets. Further it updates the stored utility value for the packet (in case
 * that the new value is larger).
 */
void PacketBuffer::ActivatePackets(int *IDs, double *Utils)
{
	list<PacketEntry*>::iterator it;
	int candidate=-1;
	int pos=-1;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			if((*it)->mimic != -1)
			{
				candidate=swapPos((*it)->mimic);
				if((pos=position(IDs, (*it)->EncodedPktIDs[candidate])) != -1)
				{
					(*it)->mimic=-1;
					if(Utils[pos] > (*it)->EncodedMaxUtil[candidate])
					{
						(*it)->EncodedMaxUtil[candidate]=Utils[pos];
					}
				}
			}
		}
	}
	return;
}

/* ActivatePackets
 * ---------------
 * Activates packets given as input (IDs[]) using the mimic flag of the corresponding
 * encoded packets. Further it updates the stored replication value.
 */
void PacketBuffer::ActivatePackets(int *IDs, int *RV)
{
	list<PacketEntry*>::iterator it;
	int candidate=-1;
	int pos=-1;
	for(it = Entries.begin(); it != Entries.end(); ++it)
	{
		if((*it)->Encoded)
		{
			if((*it)->mimic != -1)
			{
				candidate=swapPos((*it)->mimic);
				if((pos=position(IDs, (*it)->EncodedPktIDs[candidate])) != -1)
				{
					(*it)->mimic=-1;
					(*it)->EncodedReplicationValue[candidate]=RV[pos];
				}
			}
		}
	}
	return;
}

/* getAvailSpace
 * -------------
 * Returns the remaining storage space of the packet buffer (in packets).
 */
int PacketBuffer::getAvailSpace(void)
{
	if(BufferSize == 0)
	{
		return -1;
	}

	return (BufferSize - Entries.size());
}

/* getNumPacketDrops
 * -----------------
 * Returns the number of packet drops occurred.
 */
unsigned int PacketBuffer::getNumPacketDrops()
{
	return numPacketDrops;
}

/* getNumPacketDrops
 * -----------------
 * Returns the number of packet drops occurred during a time period given as input.
 */
unsigned int PacketBuffer::getNumPacketDrops(double startTime, double endTime)
{
	unsigned long droppedPackets;
	list<PacketRecord*>::reverse_iterator rit;


	/* Delete the outdated packets because of the TTL value */
	DropDeadPackets();

	droppedPackets = 0;
	for(rit = Records.rbegin(); rit != Records.rend(); ++rit)
	{
		if(((*rit)->getRegistrationTime() >= startTime) && ((*rit)->getRegistrationTime() < endTime))
		{
			if((*rit)->getDroppedPacket())
			{
				droppedPackets++;
			}
		}
		else if((*rit)->getRegistrationTime() < startTime)
		{
			break;	
		}
	}

	return droppedPackets;
}

bool PacketBuffer::isBackgroundTraffic(int pktID)
{
	return Stat->isBackgroundTraffic(this->GetPktCreationTime(pktID));
}

void PacketBuffer::writeString(string outputData)
{
	this->SGod->writeString(outputData);

	return;
}
