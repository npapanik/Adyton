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


#ifndef CODING_H
	#define CODING_H
	#include "Coding.h"
#endif


Coding::Coding(int NodeID,PacketBuffer* Buf,God *gd)
{
	this->NID=NodeID;
	this->NodeBuf=Buf;
	this->SimGod=gd;
}

/* getMyPackets
 * ------------
 * Returns a subset of the packets given as an argument (Selected). This subset
 * corresponds to the packets that originate from the same node and this node is me.
 */
int *Coding::getMyPackets(int *Selected)
{
	int total=0;
	int *MyPkts=(int *)malloc(sizeof(int));
	MyPkts[0]=0;
	for(int i=1;i<=Selected[0];i++)
	{
		if(NodeBuf->getPacketSource(Selected[i]) == NID && !(NodeBuf->isEncoded(Selected[i])))
		{
			total++;
			MyPkts=(int*)realloc(MyPkts,(sizeof(int)*total)+1);
			MyPkts[total]=Selected[i];
			MyPkts[0]=total;
		}
	}
	return MyPkts;
}

/* getAlreadyEncoded
 * -----------------
 * Returns a subset of the packets given as an argument (Selected). This subset
 * corresponds to the packets that are encoded.
 */
int *Coding::getAlreadyEncoded(int *Selected)
{
	int total=0;
	int *AlreadyEncoded=(int *)malloc(sizeof(int));
	AlreadyEncoded[0]=0;
	for(int i=1;i<=Selected[0];i++)
	{
		if(NodeBuf->isEncoded(Selected[i]))
		{
			total++;
			AlreadyEncoded=(int*)realloc(AlreadyEncoded,(sizeof(int)*total)+1);
			AlreadyEncoded[total]=Selected[i];
			AlreadyEncoded[0]=total;
		}
	}
	return AlreadyEncoded;
}

int *Coding::cleanPackets(int *Selected)
{
	int total=0;
	int *Cleaned=(int *)malloc(sizeof(int));
	Cleaned[0]=0;
	for(int i=1;i<=Selected[0];i++)
	{
		if(NodeBuf->getPacketSource(Selected[i]) == this->NID || NodeBuf->isEncoded(Selected[i]))
		{
			//Do not include encoded packets & packets originate from current node
			continue;
		}
		total++;
		Cleaned=(int*)realloc(Cleaned,(sizeof(int)*total)+1);
		Cleaned[total]=Selected[i];
		Cleaned[0]=total;
	}
	//free(Selected);
	return Cleaned;
}

/* checkOpportunity
 * ----------------
 * Returns true if the stored packets PID1 and PID2 can compose an encoded packet. Otherwise, the false
 * statement is returned. Two packets can be encoded when the source of the first packet is the same as
 * the destination of the second packet and vice versa.
 */
bool Coding::checkOpportunity(int PID1,int PID2)
{
	//printf("%d:Checking coding opportunity between %d(S:%d D:%d) and %d(S:%d D:%d)\n",this->NodeID,PID1,Buf->getPacketSource(PID1),Buf->getPacketDest(PID1),PID2,Buf->getPacketSource(PID2),Buf->getPacketDest(PID2));
	if(NodeBuf->getPacketSource(PID1) == NodeBuf->getPacketDest(PID2) && NodeBuf->getPacketSource(PID2) == NodeBuf->getPacketDest(PID1))
	{
		return true;
	}
	return false;
}

/* deleteOpportunities
 * -------------------
 * Deletes the list that is composed of opportunities given as argument.
 */
void Coding::deleteOpportunities(Opportunities *Opps)
{
	Opportunities *runner=Opps;
	Opportunities *previous=NULL;
	while(runner)
	{
		previous=runner;
		runner=runner->next;
		free(previous);
	}
	return;
}

/* searchCodingOpportunities
 * -------------------------
 * Searches for coding opportunities among packets ("Candidates") that are given as input. 
 * The maximum coding depth for now is set at 2. Actually the codings returned by this method
 * are known as codings of type A.
 */
Opportunities *Coding::searchCodingOpportunities(int *Candidates)
{
	Opportunities *Opps=NULL;
	Opportunities *tmp=NULL;
	Opportunities *previous=NULL;
	bool found=false;
	for(int i=1;i<=Candidates[0];i++)
	{
		if(Candidates[i] == -1)
		{
			continue;
		}
		found=false;
		for(int j=(i+1);j<=Candidates[0];j++)
		{
			if(Candidates[j] == -1)
			{
				continue;
			}
			found=checkOpportunity(Candidates[i],Candidates[j]);
			if(found)
			{
				SimGod->increaseEncodingsA();
				//printf("%d: Found a coding opportunity between %d and %d!\n",this->NodeID,Candidates[i],Candidates[j]);
				tmp=(Opportunities *)malloc(sizeof(Opportunities));
				tmp->PID1=Candidates[i];
				tmp->PID2=Candidates[j];
				tmp->next=NULL;
				if(previous)
				{
					previous->next=tmp;
				}
				if(!Opps)
				{
					Opps=tmp;
				}
				previous=tmp;
				Candidates[i]=-1;
				Candidates[j]=-1;
				break;
			}
		}
	}
	return Opps;
}

/* searchCodingOpportunities
 * -------------------------
 * Searches for coding opportunities among packets ("Candidates" + "NoPriority") that are given as input. 
 * The maximum coding depth for now is set at 2. An opportunity consists of one packet coming from "Candidates"
 * list and a second packet coming from "NoPriority" packet list. Actually the codings returned by this method
 * are known as codings of type B.
 */
Opportunities *Coding::forceCoding(int *Candidates, int *NoPriority)
{
	Opportunities *Opps=NULL;
	Opportunities *tmp=NULL;
	Opportunities *previous=NULL;
	bool found=false;
	for(int i=1;i<=Candidates[0];i++)
	{
		if(Candidates[i] == -1)
		{
			continue;
		}
		found=false;
		for(int j=1;j<=NoPriority[0];j++)
		{
			if(NoPriority[j] == -1)
			{
				continue;
			}
			found=checkOpportunity(Candidates[i],NoPriority[j]);
			if(found)
			{
				SimGod->increaseEncodingsB();
				//printf("%d: Forced a coding opportunity between %d and %d!\n",this->NodeID,Candidates[i],NoPriority[j]);
				tmp=(Opportunities *)malloc(sizeof(Opportunities));
				tmp->PID1=Candidates[i];
				tmp->PID2=NoPriority[j];
				tmp->next=NULL;
				if(previous)
				{
					previous->next=tmp;
				}
				if(!Opps)
				{
					Opps=tmp;
				}
				previous=tmp;
				Candidates[i]=-1;
				NoPriority[j]=-1;
				break;
			}
		}
	}
	return Opps;
}

/* printOpportunities
 * ------------------
 * Prints the list that is composed of opportunities given as argument.
 */
void Coding::printOpportunities(Opportunities* Opps)
{
	Opportunities *runner=Opps;
	//Packet *p=NULL;
	if(!runner)
	{
		printf("-no combinations-\n");
		return;
	}
	int cnt=0;
	while(runner)
	{
		cnt++;
		runner=runner->next;
	}
	printf("(%d combinations): ",cnt);
	runner=Opps;
	while(runner)
	{
		printf("%d-%d ",runner->PID1,runner->PID2);
		//check existence
// 		if((p=pktPool->GetPacket(runner->PID1)))
// 		{
// 			printf("%d->exists!\n",runner->PID1);
// 		}
// 		if((p=pktPool->GetPacket(runner->PID2)))
// 		{
// 			printf("%d->exists!\n",runner->PID2);
// 		}
		runner=runner->next;
	}
	printf("\n");
	return;
}



int* Coding::getRejected(int *Req, int *Chosen,int *owned)
{
	int total=0;
	int *Rejected=(int *)malloc(sizeof(int));
	Rejected[0]=0;
	bool found=true;
	for(int i=1;i<=Req[0];i++)
	{
		found=true;
		if(!(NodeBuf->isEncoded(Req[i])) && NodeBuf->getPacketSource(Req[i]) != NID)
		{
			found=false;
			for(int j=1;j<=Chosen[0];j++)
			{
				if(Req[i] == Chosen[j])
				{
					found=true;
					break;
				}
			}
			for(int j=1;j<=owned[0];j++)
			{
				if(Req[i] == owned[j])
				{
					found=true;
					break;
				}
			}
		}
		if(!found)
		{
			total++;
			Rejected=(int*)realloc(Rejected,(sizeof(int)*total)+1);
			Rejected[total]=Req[i];
			Rejected[0]=total;
		}
	}
	return Rejected;
}


/* removePID
 * ---------
 * Removes the packet with "ID" from the packet list "Selected" and returns the true
 * statement. Otherwise, the false statement is returned.
 */
bool Coding::removePID(int *Selected,int ID)
{
	for(int i=1;i<=Selected[0];i++)
	{
		if(Selected[i] == ID)
		{
			Selected[i]=-1;
			return true;
			break;
		}
	}
	return false;
}

/* excludePairs
 * ------------
 * The contents of the "Selected" list are packets that are stored as parts of encoded packets.
 * This method searches for encoded packets composed of two packets that exist in the "Selected"
 * list. If such a pair found, then one of them is eliminated from the "Selected" list. Finally,
 * this method returns a "status" list that denotes the significance of the packet (in the same 
 * position at the "Selected" list).
 */
int *Coding::excludePairs(int *Selected)
{
	int *status=(int *)malloc(sizeof(int)*(Selected[0]+1));
	status[0]=Selected[0];
	for(int i=1;i<=status[0];i++)
	{
		status[i]=0;
	}
	bool ret=false;
	int *PIDs=NULL;
	for(int i=1;i<=Selected[0];i++)
	{
		if(Selected[i] == -1)
		{
			continue;
		}
		PIDs=NodeBuf->getEncodedIDs(Selected[i]);
		if(Selected[i] == PIDs[0])
		{
			status[i]=0;
			ret=removePID(Selected,PIDs[1]);
		}
		else
		{
			status[i]=1;
			ret=removePID(Selected,PIDs[0]);
		}
		if(ret)
		{
			status[i]=-1;
		}
	}
	return status;
}

/* getPktsBetterFW
 * ----------------
 * Returns a subset of the packets given as an argument (req). This subset
 * corresponds to the packets that have lower util value (this value 
 * is saved by every node for each packet - maxUtil) than the util
 * value that is given as an argument (Utils). Furthermore, the subset
 * of packets returned should not be owned by the other node (argument owned)
 * or include encoded packets.
 * 
 * Requirements
 * 1. MyUtil < Util of node in contact
 * 2. Not owned
 * 3. Not encoded
 * 4. I'm not the originator
 */
int *Coding::getPktsBetterFW(int *req, double *Utils, bool *owned)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(owned[i] || NodeBuf->getPacketSource(req[i]) == this->NID || NodeBuf->isEncoded(req[i]))
		{
			continue;
		}
		for(it = NodeBuf->Entries.begin(); it != NodeBuf->Entries.end(); ++it)
		{
			if(!((*it)->Encoded) && req[i] == (*it)->pktID)
			{
				if(Utils[i] > (*it)->GetMaxUtil())
				{
					pos++;
					pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
					pkts[pos] = (*it)->pktID;
					pkts[0] = pos;
				}
			}
		}
	}
	return pkts;
}


/* getPktsNotBetterFW
 * ------------------
 * Returns a subset of the packets given as an argument (req). This subset
 * corresponds to the packets that have higher util value (this value 
 * is saved by every node for each packet - maxUtil) than the util
 * value that is given as an argument (Utils). Furthermore, the subset
 * of packets returned should not be owned by the other node (argument owned)
 * or include encoded packets.
 * Requirements:
 * 1. MyUtil >= Util of node in contact
 * 2. Not owned
 * 3. Not encoded
 * 4. I'm not the originator
 */
int *Coding::getPktsNotBetterFW(int *req, double *Utils, bool *owned)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(owned[i] || NodeBuf->getPacketSource(req[i]) == this->NID || NodeBuf->isEncoded(req[i]))
		{
			continue;
		}
		for(it = NodeBuf->Entries.begin(); it != NodeBuf->Entries.end(); ++it)
		{
			if(!((*it)->Encoded) && req[i] == (*it)->pktID)
			{
				if(Utils[i] <= (*it)->GetMaxUtil())
				{
					pos++;
					pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
					pkts[pos] = (*it)->pktID;
					pkts[0] = pos;
				}
			}
		}
	}
	return pkts;
}

/* getPktsForActivation
 * --------------------
 * Returns a subset of the packets given as an argument (req). This subset
 * corresponds to the local packets that have lower util value (this value 
 * is saved by every node for each packet - maxUtil) than the util
 * value that is given as an argument (Utils). Furthermore, the subset
 * of packets returned is owned by the other node (argument owned) and is
 * stated as extra (by the other node).
 */
int *Coding::getPktsForActivation(int *req, double *Utils, bool *owned, int *isExtra)
{
	int pos;
	int *pkts;
	bool found=false;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(!owned[i] || !isExtra[i])
		{
			continue;
		}
		for(it = NodeBuf->Entries.begin(); it != NodeBuf->Entries.end(); ++it)
		{
			found=false;
			if(!((*it)->Encoded) && req[i] == (*it)->pktID)
			{
				if(Utils[i] > (*it)->GetMaxUtil())
				{
						pos++;
						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
						pkts[pos] = (*it)->pktID;
						pkts[0] = pos;
				}
			}
			else if((*it)->Encoded)
			{
				if((*it)->mimic == -1)
				{
					for(int j=0;j<(*it)->CodingDepth;j++)
					{
						if((*it)->EncodedPktIDs[j] == req[i] && Utils[i] > (*it)->EncodedMaxUtil[j])
						{
							found=true;
							break;
						}
					}
				}
				else
				{
					if((*it)->EncodedPktIDs[(*it)->mimic] == req[i] && Utils[i] > (*it)->EncodedMaxUtil[(*it)->mimic])
					{
						found=true;
					}
				}
			}
			
			if(found)
			{
				pos++;
				pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
				pkts[pos] = req[i];
				pkts[0] = pos;
				break;
			}
			
		}
	}
	return pkts;
}


int *Coding::getMyPackets(int *req, double *Utils, bool *owned)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(owned[i])
		{
			continue;
		}
		for(it = NodeBuf->Entries.begin(); it != NodeBuf->Entries.end(); ++it)
		{
				if(!((*it)->Encoded) && req[i] == (*it)->pktID && (*it)->getSource() == NID)
				{
					if(Utils[i] > (*it)->GetMaxUtil())
					{
						pos++;
						pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
						pkts[pos] = (*it)->pktID;
						pkts[0] = pos;
					}
				}
		}
	}
	return pkts;
}

int *Coding::getAlreadyEncoded(int *req, double *Utils, bool *owned)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(owned[i])
		{
			continue;
		}
		for(it = NodeBuf->Entries.begin(); it != NodeBuf->Entries.end(); ++it)
		{
				if((*it)->Encoded)
				{
					for(int j=0;j<(*it)->CodingDepth;j++)
					{
						if((*it)->EncodedPktIDs[j] == req[i])
						{
							if(Utils[i] > (*it)->EncodedMaxUtil[j])
							{
								pos++;
								pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
								pkts[pos] = (*it)->EncodedPktIDs[j];
								pkts[0] = pos;
							}
						}
					}
				}
		}
	}
	return pkts;
}


int *Coding::getPktsBetterFW(int *req, bool *isExtra)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	int R=0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(isExtra[i] || NodeBuf->getPacketSource(req[i]) == this->NID || NodeBuf->isEncoded(req[i]))
		{
			continue;
		}
		R=NodeBuf->getPacketData(req[i])->GetReplicas();
		if(R != 1)
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = req[i];
			pkts[0] = pos;
		}
	}
	return pkts;
}

int *Coding::getPktsNotBetterFW(int *req, bool *isExtra)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	int R=0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(isExtra[i] || NodeBuf->getPacketSource(req[i]) == this->NID || NodeBuf->isEncoded(req[i]))
		{
			continue;
		}
		R=NodeBuf->getPacketData(req[i])->GetReplicas();
		if(R == 1)
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = req[i];
			pkts[0] = pos;
		}
	}
	return pkts;
}


int *Coding::getMyPackets(int *req, bool *isExtra)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	int R=0;
	for(int i = 1; i <= req[0]; i++)
	{
		R=NodeBuf->getRepVal(req[i]);
		if(NodeBuf->getPacketSource(req[i]) == this->NID && !NodeBuf->isEncoded(req[i]) && R!= 1 && !isExtra[i])
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = req[i];
			pkts[0] = pos;
		}
	}
	return pkts;
}


int *Coding::getAlreadyEncoded(int *req, bool *isExtra)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	int R=0;
	for(int i = 1; i <= req[0]; i++)
	{
		R=NodeBuf->getRepVal(req[i]);
		if(NodeBuf->isEncoded(req[i]) && R!= 1 && !isExtra[i])
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = req[i];
			pkts[0] = pos;
		}
	}
	return pkts;
}

int *Coding::getPktsForActivation(int *req, bool *isExtra)
{
	int pos;
	int *pkts;
	list<PacketEntry*>::iterator it;
	pkts = (int *) malloc(sizeof(int));
	pkts[0] = 0;
	pos = 0;
	int R=0;
	for(int i = 1; i <= req[0]; i++)
	{
		if(!isExtra[i])
		{
			continue;
		}
		R=NodeBuf->getRepVal(req[i]);
		if(R != 1)
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = req[i];
			pkts[0] = pos;
		}
	}
	return pkts;
}


int *Coding::getCodingCandidates(void)
{
	list<PacketEntry*>::iterator it;
	int pos=0;
	int *pkts=(int *) malloc(sizeof(int));
	pkts[0] = 0;
	for(it = NodeBuf->Entries.begin(); it != NodeBuf->Entries.end(); ++it)
	{
		if(!(*it)->Encoded)
		{
			pos++;
			pkts = (int *) realloc(pkts, (pos + 1) * sizeof(int));
			pkts[pos] = (*it)->pktID;
			pkts[0] = pos;
		}
	}
	return pkts;
}


bool Coding::checkFriendlyOpportunity(int PID1, int PID2)
{
	int S1=NodeBuf->getPacketSource(PID1);
	int D1=NodeBuf->getPacketDest(PID1);
	int S2=NodeBuf->getPacketSource(PID2);
	int D2=NodeBuf->getPacketDest(PID2);
	
	
	
	if(S1 != D2)
	{
		return false;
	}
	//printf("Checking packet A:%d->%d with packet B:%d->%d\n",S1,D1,S2,D2);
	int *friends=SimGod->getFriends(D1);
	for(int i=0;i<50;i++)
	{
		if(friends[i] == S2)
		{
			return true;
		}
	}
	return false;
}

Opportunities *Coding::searchFriendlyCodingOpportunities(int *Candidates)
{
	Opportunities *Opps=NULL;
	Opportunities *tmp=NULL;
	Opportunities *previous=NULL;
	bool found=false;
	for(int i=1;i<=Candidates[0];i++)
	{
		if(Candidates[i] == -1)
		{
			continue;
		}
		found=false;
		for(int j=(i+1);j<=Candidates[0];j++)
		{
			if(Candidates[j] == -1)
			{
				continue;
			}
			found=checkFriendlyOpportunity(Candidates[i],Candidates[j]);
			if(found)
			{
				//SimGod->increaseEncodingsA();
				//printf("%d: Found a coding opportunity between %d and %d!\n",this->NodeID,Candidates[i],Candidates[j]);
				tmp=(Opportunities *)malloc(sizeof(Opportunities));
				tmp->PID1=Candidates[i];
				tmp->PID2=Candidates[j];
				tmp->next=NULL;
				if(previous)
				{
					previous->next=tmp;
				}
				if(!Opps)
				{
					Opps=tmp;
				}
				previous=tmp;
				Candidates[i]=-1;
				Candidates[j]=-1;
				break;
			}
		}
		if(!found)
		{
			
		}
	}
	return Opps;
}

Opportunities *Coding::forceFriendlyCoding(int *Candidates, int *NoPriority)
{
	Opportunities *Opps=NULL;
	Opportunities *tmp=NULL;
	Opportunities *previous=NULL;
	bool found=false;
	for(int i=1;i<=Candidates[0];i++)
	{
		if(Candidates[i] == -1)
		{
			continue;
		}
		found=false;
		for(int j=1;j<=NoPriority[0];j++)
		{
			if(NoPriority[j] == -1)
			{
				continue;
			}
			found=checkFriendlyOpportunity(Candidates[i],NoPriority[j]);
			if(found)
			{
				//SimGod->increaseEncodingsB();
				//printf("%d: Forced a coding opportunity between %d and %d!\n",this->NodeID,Candidates[i],NoPriority[j]);
				tmp=(Opportunities *)malloc(sizeof(Opportunities));
				tmp->PID1=Candidates[i];
				tmp->PID2=NoPriority[j];
				tmp->next=NULL;
				if(previous)
				{
					previous->next=tmp;
				}
				if(!Opps)
				{
					Opps=tmp;
				}
				previous=tmp;
				Candidates[i]=-1;
				NoPriority[j]=-1;
				printf("Combination of %d and %d\n",tmp->PID1,tmp->PID2);
				break;
			}
		}
	}
	return Opps;
}
