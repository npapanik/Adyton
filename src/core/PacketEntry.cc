/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015-2018  Nikolaos Papanikos,
 *  Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou
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

#ifndef PACKET_ENTRY_H
	#define PACKET_ENTRY_H
	#include "PacketEntry.h"
#endif


/* init
 * ----
 * Initialization of the packet entry attributes. 
 */
void PacketEntry::init(int ID, int Dest,int Src, int Itime, int hops, int prev, double CrtTime)
{
	this->pktID=ID;
	this->CreationTime=CrtTime;
	this->InsTime=Itime;
	this->Hops=hops;
	this->Source=Src;
	this->Destination=Dest;
	this->prevHop=prev;
	this->FWused.clear();
	this->NodesCovered.clear();
	this->AllprevHop.clear();
	this->Replicas=1;
	this->MaxUtil=0.0;
	this->Forwarded=false;
	this->MaxUtilDestDep=0.0;
	this->MaxUtilDestInd=0.0;
	this->Encoded=false;
	this->CodingDepth=0;
	this->EncodedPktIDs=NULL;
	this->EncodedPktDest=NULL;
	this->EncodedCreationTime=NULL;
	this->EncodedMaxUtil=NULL;
	this->EncodedReplicationValue=NULL;
	this->EncodedPktSource=NULL;
	this->mimic=-1;
	this->MaxUtils=NULL;
	return;
}

/* Constructor:PacketEntry
 * -----------------------
 * Creates a packet entry object for a native packet.
 */
PacketEntry::PacketEntry(int ID,int Dest,int Src,int Itime,int hops,int prev, double CrtTime)
{
	this->init(ID,Dest,Src,Itime,hops,prev, CrtTime);
	return;
}

/* Constructor:PacketEntry
 * -----------------------
 * Creates a packet entry object for an encoded packet.
 */
PacketEntry::PacketEntry(int *ID,int *Dest,int *Src,int depth,double *CrtTime,int Itime,int prev,int mimic)
{
	this->init(-1,-1,-1,Itime,0,prev,-1.0);
	this->Encoded=true;
	this->CodingDepth=depth;
	this->EncodedPktIDs=ID;
	this->EncodedPktSource=Src;
	this->EncodedPktDest=Dest;
	this->EncodedCreationTime=CrtTime;
	this->mimic=mimic;
	return;
}

/* Constructor:PacketEntry
 * -----------------------
 * Creates a packet entry object for an encoded packet, which consists of multiple native
 * packets with their corresponding utility values and replica values.
 */
PacketEntry::PacketEntry(int *ID,int *Dest,int *Src,double *Utils,int *Rep,int depth,double *CrtTime,int Itime,int prev,int mimic)
{
	this->init(-1,-1,-1,Itime,0,prev,-1.0);
	this->Encoded=true;
	this->CodingDepth=depth;
	this->EncodedPktIDs=ID;
	this->EncodedPktSource=Src;
	this->EncodedPktDest=Dest;
	this->EncodedCreationTime=CrtTime;
	this->mimic=mimic;
	this->EncodedMaxUtil=Utils;
	this->EncodedReplicationValue=Rep;
	return;
}

/* Constructor:PacketEntry
 * -----------------------
 * Creates a packet entry object for a native packet and its corresponding utility metrics used
 * by SimBetTS.
 */
PacketEntry::PacketEntry(int ID, int Dest, int Src, int Itime, int hops, int prev, double CrtTime, struct SimBetTSmetrics *Umetrics)
{
	this->init(ID,Dest,Src,Itime,hops,prev, CrtTime);
	this->MaxUtils=Umetrics;
	return;
}

/* Destructor:PacketEntry
 * ----------------------
 * Frees all data structures used.
 */
PacketEntry::~PacketEntry()
{
	FWused.clear();
	NodesCovered.clear();
	AllprevHop.clear();
	
	free(EncodedPktIDs);
	free(EncodedPktSource);
	free(EncodedPktDest);
	free(EncodedCreationTime);
	free(EncodedMaxUtil);
	free(EncodedReplicationValue);
	free(MaxUtils);
	return;
}

/* freeMem
 * -------
 * Frees all data structures used.
 */
void PacketEntry::freeMem(void)
{
	free(EncodedPktIDs);
	free(EncodedPktDest);
	free(EncodedCreationTime);
	free(EncodedMaxUtil);
	free(MaxUtils);
	return;
}

/* getDestination
 * --------------
 * Returns the destination attribute of the packet entry.
 */
int PacketEntry::getDestination(void)
{
	if(!this->Encoded)
	{
		return this->Destination;
	}
	return this->EncodedPktDest[0];
}

/* getDestination
 * --------------
 * Returns the source attribute of the packet entry.
 */
int PacketEntry::getSource(void)
{
	if(!this->Encoded)
	{
		return this->Source;
	}
	return this->EncodedPktSource[0];
}

/* SetReplicas
 * -----------
 * Sets the replica attribute of the packet entry according to the input given.
 */
void PacketEntry::SetReplicas(int R)
{
	this->Replicas=R;
	return;
}

/* GetReplicas
 * -----------
 * Returns the replica attribute of the packet entry.
 */
int PacketEntry::GetReplicas(void)
{
	return this->Replicas;
}

/* IncReplicas
 * -----------
 * Increments the replica attribute of the packet entry by R.
 */
void PacketEntry::IncReplicas(int R)
{
	this->Replicas+=R;
	return;
}

/* AddPrevHop
 * ----------
 * Adds "prev" to the list of previous hop nodes for the packet entry.
 */
void PacketEntry::AddPrevHop(int prev)
{
	if(!DupPrevHop(prev))
	{
		this->AllprevHop.push_back(prev);
	}
	return;
}

/* AddPrevHop
 * ----------
 * Checks if "prev" already exists in the list of previous hop nodes. 
 */
bool PacketEntry::DupPrevHop(int prev)
{
	for(unsigned int i=0;i<(this->AllprevHop.size());i++)
	{
		if(this->AllprevHop[i] == prev)
		{
			return true;
		}
	}
	return false;
}

/* GetAllPrevHops
 * --------------
 * Returns an array that contains a list of previous hop nodes for the
 * packet entry.
 */
int *PacketEntry::GetAllPrevHops(void )
{
	int *res=(int *)malloc(sizeof(int)*(this->AllprevHop.size())+1);
	res[0]=this->AllprevHop.size();
	int pos=1;
	for(unsigned int i=0;i<AllprevHop.size();i++)
	{
		res[pos]=this->AllprevHop[i];
		pos++;
	}
	return res;
}

/* AddForwarder
 * ------------
 * Marks node with ID as a forwarder of the packet (that corresponds to
 * the packet entry).
 */
void PacketEntry::AddForwarder(int ID)
{
	if(!IsOldForwarder(ID))
	{
		this->FWused.push_back(ID);
	}
	return;
}

/* IsOldForwarder
 * --------------
 * Checks if "ID" already exists in the list of packet forwarders.
 */
bool PacketEntry::IsOldForwarder(int ID)
{
	for(unsigned int i=0;i<(this->FWused.size());i++)
	{
		if(this->FWused[i] == ID)
		{
			return true;
		}
	}
	return false;
}

/* GetAllFWs
 * ---------
 * Returns an array that contains a list of nodes that have forwarded the packet
 * that corresponds to the packet entry.
 */
int *PacketEntry::GetAllFWs(void )
{
	int *res=(int *)malloc(sizeof(int)*(this->FWused.size())+1);
	res[0]=this->FWused.size();
	int pos=1;
	for(unsigned int i=0;i<FWused.size();i++)
	{
		res[pos]=this->FWused[i];
		pos++;
	}
	return res;
}

/* AddCovered
 * ----------
 * Marks a list of node ids (given as input - IDs) as covered for the current packet
 * entry. A node is consider as covered when it has received a packet replica or exists
 * in the ego network of a node that own a packet replica (as direct contact).
 */
void PacketEntry::AddCovered(int *IDs)
{
	for(int i=1;i<=IDs[0];i++)
	{
		if(!IsCovered(IDs[i]))
		{
			this->NodesCovered.push_back(IDs[i]);
		}
	}
	return;
}

/* getCovered
 * ----------
 * Returns an array of node ids that are marked as covered for the current
 * packet entry. 
 */
int *PacketEntry::getCovered(void )
{
	int *res=(int *)malloc(sizeof(int)*(this->NodesCovered.size()+1));
	res[0]=this->NodesCovered.size();
	int pos=1;
	for(unsigned int i=0;i<(this->NodesCovered.size());i++)
	{
		res[pos]=this->NodesCovered[i];
		pos++;
	}
	return res;
}

/* IsCovered
 * ---------
 * Returns if the node with ID is marked as covered for the current packet entry.
 */
bool PacketEntry::IsCovered(int ID)
{
	for(unsigned int i=0;i<(this->NodesCovered.size());i++)
	{
		if(this->NodesCovered[i] == ID)
		{
			return true;
		}
	}
	return false;
}

/* getEncodedRepVal
 * ----------------
 * Returns the replication value for a native packet included inside the packet entry of
 * an encoded packet. The input value defines the position of the native among other native
 * packets inside the encoded packet.  
 */
int PacketEntry::getEncodedRepVal(int pos)
{
	return this->EncodedReplicationValue[pos];
}

/* setEncodedRepVal
 * ----------------
 * Sets the replication  for a native packet included inside the packet entry of
 * an encoded packet.
 */
void PacketEntry::setEncodedRepVal(int pos, int val)
{
	this->EncodedReplicationValue[pos]=val;
}

/* GetMaxUtil
 * ----------
 * Returns the maximum utility value (threshold) of the current packet entry. Multiple utility metrics 
 * are supported. For that reason the type of the utility metric is given as input.
 */
double PacketEntry::GetMaxUtil(string utilityMetric)
{
	if(!MaxUtils)
	{
		printf("PacketEntry problem: Packet entry is not properly initialized\nAborting..\n");
		exit(1);
	}
	if(utilityMetric == "Sim")
	{
		return MaxUtils->Similarity;
	}
	else if(utilityMetric == "Bet")
	{
		return MaxUtils->Betweenness;
	}
	else if(utilityMetric == "Freq")
	{
		return MaxUtils->Frequency;
	}
	else if(utilityMetric == "Intimacy")
	{
		return MaxUtils->Intimacy;
	}
	else if(utilityMetric == "Recency")
	{
		return MaxUtils->Recency;
	}
	else
	{
		printf("PacketEntry problem: Cannot find the threshold value of the give utility metric\nAborting..\n");
		exit(1);
	}
}

/* UpdateMaxUtil
 * -------------
 * Updates the maximum utility value (threshold) of the current packet entry.
 */
void PacketEntry::UpdateMaxUtil(string utilityMetric, double val)
{
	if(!MaxUtils)
	{
		printf("PacketEntry problem: Packet entry is not properly initialized\nAborting..\n");
		exit(1);
	}
	if(utilityMetric == "Sim")
	{
		MaxUtils->Similarity=val;
	}
	else if(utilityMetric == "Bet")
	{
		MaxUtils->Betweenness=val;
	}
	else if(utilityMetric == "Freq")
	{
		MaxUtils->Frequency=val;
	}
	else if(utilityMetric == "Intimacy")
	{
		MaxUtils->Intimacy=val;
	}
	else if(utilityMetric == "Recency")
	{
		MaxUtils->Recency=val;
	}
	else
	{
		printf("PacketEntry problem: Cannot find the threshold value of the give utility metric\nAborting..\n");
		exit(1);
	}
	return;
}
