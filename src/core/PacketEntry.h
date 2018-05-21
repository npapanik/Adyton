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


#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>


using namespace std;

struct SimBetTSmetrics
{
	double Similarity;
	double Betweenness;
	double Recency;
	double Intimacy;
	double Frequency;
};

class PacketEntry
{
public:
	//Marks Nodes that used as FWers for this packet
	vector<int> FWused;
	//Mark Nodes that are covered (with previous FW)
	vector<int> NodesCovered;
	vector<int> AllprevHop;
	
	//Maximum utility values - This packet was replicated to a node that has these utility values
	struct SimBetTSmetrics *MaxUtils;
	
	//Maximum utility value - This packet was replicated to a node that had this utility value
	double MaxUtil;
	double MaxUtilDestDep;
	double MaxUtilDestInd;
	
	double CreationTime;
	double InsTime;
	int pktID;
	int Hops;
	int Source;
	int Destination;
	int prevHop;
	int Replicas;
	int InitReplicas;
	
	//Used for SC Routing Algorithms
	bool Forwarded;
	
	
	//used for encoded packets
	bool Encoded;
	int CodingDepth;
	int *EncodedPktIDs;
	int *EncodedPktSource;
	int *EncodedPktDest;
	double *EncodedCreationTime;
	double *EncodedMaxUtil;
	int *EncodedReplicationValue;
	int mimic; //-1: means do not mimic - 0:mimic slot 0 component  - 1:mimic slot 1 component
	
	PacketEntry(int ID,int Dest,int Src,int Itime,int hops,int prev,double CrtTime);
	PacketEntry(int *ID,int *Dest,int *Src,int depth,double *CrtTime,int Itime,int prev,int mimic);
	PacketEntry(int *ID,int *Dest,int *Src,double *Utils,int *Rep,int depth,double *CrtTime,int Itime,int prev,int mimic);
	PacketEntry(int ID,int Dest,int Src,int Itime,int hops,int prev,double CrtTime,struct SimBetTSmetrics *Umetrics);
	~PacketEntry();
	void AddForwarder(int ID);
	bool IsOldForwarder(int ID);
	void AddCovered(int *IDs);
	bool IsCovered(int ID);
	int *getCovered(void);
	void AddPrevHop(int prev);
	bool DupPrevHop(int prev);
	int *GetAllPrevHops(void);
	int *GetAllFWs(void);
	void SetReplicas(int R);
	int GetReplicas(void);
	
	void IncReplicas(int R);
	void SetMaxUtilDestDep(double val){this->MaxUtilDestDep=val;};
	double GetMaxUtilDestDep(void){return this->MaxUtilDestDep;};
	void SetMaxUtilDestInd(double val){this->MaxUtilDestInd=val;};
	double GetMaxUtilDestInd(void){return this->MaxUtilDestInd;};

	void SetMaxUtil(double val){this->MaxUtil=val;};
	double GetMaxUtil(void){return this->MaxUtil;};
	
	int getDestination(void);
	int getSource(void);
	int getHops(void){return this->Hops;};
	void setHops(int h){this->Hops=h;};
	
	int getEncodedRepVal(int pos);
	void setEncodedRepVal(int pos,int val);
	
	double GetMaxUtil(string utilityMetric);
	void UpdateMaxUtil(string utilityMetric,double val);
	void freeMem(void);
	
private:
	void init(int ID,int Dest,int Src,int Itime,int hops,int prev,double CrtTime);
};
