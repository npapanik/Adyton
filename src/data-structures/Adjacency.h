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
#include <limits.h>
#include <bitset>
#include <vector>
#include <boost/dynamic_bitset.hpp>

using namespace boost;
using namespace std;
using std::vector;

class ContactAggregation;
class GrowingTimeWindow;
class MFaggregation;


class DrawCandidate
{
public:
	int ID;
	int CoverSize;
	bool IsOldFW;
	int Degree;
	int position;
	DrawCandidate(int NodeID,int Cover,bool Old,int Deg,int pos);
	~DrawCandidate(void){return;};
};

class Adjacency
{
	friend class ContactAggregation;
	
protected:
	vector< dynamic_bitset<> > Nodes;
	
	int NSize;
	int NodeID;
	int *Similarities;
	int *DisSimilarities;
	double Bet;
	double Effi;
	
	int *numEncounters;
	int totalNumEncounters;
	double *timeConnected;
	double totalTimeConnected;
	double *lastTimeSeen;
	double lastUpdate;
	int *activeContacts;
	
	ContactAggregation *CA;
	
public:
	Adjacency(int ID, int NodeSize, int AggregationType, double argument);
	Adjacency(int ID, int NodeSize);
	virtual ~Adjacency();
	void init(int ID, int NodeSize, int AggregationType, double argument);
	//Basic methods
	void PrintAdjacency();
	void SetConnection(int Node1, int Node2, double ConTime);
	int *GetMyContacts();
	//Methods that associate with similarity and betweenness metrics
	int CalculateSimilarity(int toDestination);
	int CalculateDisSimilarity(int toDestination);
	void UpdateBetweenness(void);
	void UpdateEfficiency(void);
	void UpdateSimilarity(int Dest);
	void UpdateDisSimilarity(int Dest);
	void UpdateAll();
	void Update();
	double getBet(void){return this->Bet;};
	double getSim(int toDestination){return this->Similarities[toDestination];};
	double getDisSim(int toDestination){return this->DisSimilarities[toDestination];};
	double getEffi(void){return this->Effi;};
	double getCopEffi(int nodeA, int nodeB);
	double *getAllSim(void);
	void PrintSimilarities(void);
	int getEgoNetworkSize(void);
	int getSimilarityRank(int destID);
	//Dominant Pruning Related methods (used by SDP)
	int *GetForwardingSet(int prev,int Dest);
	int *GetForwardingSet(int prev,int *Excluding);
	int *GetForwardingSet(int *prev,int *Excluding);
	int *GetForwardingSet(int *prev,int *Excluding,int *FWs);
	int *GetForwardingSet(int prev,int *Excluding,int *FWs);
	int *GetForwardingSet(int *oldFWs,int prev);
	int *GetForwardingSet2(int *oldFWs,int prev);
	int *GetForwardingSet3(int *oldFWs,int *prev);
	int *GetForwardingSet4(int *oldFWs,int *prev);
	int *GetForwardingSet5(int *oldFWs,int *prev,double Ctime);
	int *GetForwardingSetRecent(int *oldFWs,int *prev,double Ctime);
	int *GetForwardingSetPriority(int *oldFWs,int *prev,int NodeinContact);
	int *GetForwardingSetNew(int *oldFWs,int *prev);
	bool IsDirectContact(int ID);
	int *IsIndirectContact(int ID);
	int GetNumDirect(void);
	int *GetIndirectContacts(int throughID);
	int *GetIndirectContactsOnlyThroughID(int throughID);
	bool AreNodeContactsCovered(int ID, int *NL);
	int HowManyCovered(int throughID);
	int *GetBestFriends(int *Candidates);
	void PrintDraw(vector<DrawCandidate*> DCan);
	bool NotOldExists(vector<DrawCandidate*> DCan);
	int GetHighestDegree(vector<DrawCandidate*> DCan);
	int GetMostRecent(vector<DrawCandidate*> DCan,double Ctime);
	int GetBetterTieStrength(vector<DrawCandidate*> DCan,double Ctime);
	int GetMostFrequent(vector<DrawCandidate*> DCan);
	bool OldExists(vector<DrawCandidate*> DCan);
	bool Is2HopCovered(int ID,int *FWers,int *prevs);
	/* Methods that extend the contact graph to hold weights
	and calculate Frequency, Intimacy and Tie Strength metrics
	(used by SimBetTS) */
	void ContactStart(int ID, double CurrentTime);
	void ContactEnd(int ID, double CurrentTime);
	void updateFrequency(int ID);
	void updateIntimacy(double CurrentTime);
	void updateRecency(int ID, double CurrentTime);
	double getFreq(int ID);
	double getNormalizedFrequency(int ID);
	double getIntimacy(int ID);
	double getNormalizedIntimacy(int ID);
	double getRecency(int ID, double CurrentTime);
	double CalculateTieStrength(int ID, double CurrentTime);
	double *CalculateAllTieStrengths(double CurrentTime);
	double NormalizedTieStrength(int ID, double CurrentTime);
	//Method that calculates utility of the LSF algorithm
	double GetLSFUtility(int ID,double CurrentTime);
	
};

//Change name to --> AdjacencyTimeWindow
class AdjacencyWindow:public Adjacency
{
protected:
	double **ContactTime;
	double WindowSize;
public:
	AdjacencyWindow(int ID,int NodeSize,double Window);
	~AdjacencyWindow();
	void SetConnection(int Node1,int Node2,double CurrentTime);
	void UpdateAll(double CurrentTime);
	void UpdateWindow(double CTime);
};

class AdjacencyWeights:public Adjacency
{
protected:
	double **IntimacyWeight;
	double **FreqWeight;
	
public:
	AdjacencyWeights(int ID,int NodeSize);
	~AdjacencyWeights();
	
	void UpdateFreqWeight(int Neighbor,int Contact, double value);
	double GetFreqWeight(int Neighbor,int Contact);
	
	void UpdateIntWeight(int Neighbor,int Contact, double value);
	double GetIntWeight(int Neighbor,int Contact);
	
	double GetWeight(int Neighbor,int Contact);
	void PrintWeights(void);
	
	int *GetForwardingSetRecent(int *oldFWs,int *prev,double Ctime);
	bool Is2HopCoveredv2(int ID,int *FWers,int *prevs);
};


class ContactAggregation
{
protected:
	vector< dynamic_bitset<> > *CG;
public:
	ContactAggregation(vector< dynamic_bitset<> > *p2Nodes);
	virtual ~ContactAggregation();
	virtual void Update(void)=0;
	virtual void Connect(int Node1,int Node2,double ConTime)=0;
};


class GrowingTimeWindow:public ContactAggregation
{	
public:
	GrowingTimeWindow(vector< dynamic_bitset<> > *p2Nodes);
	~GrowingTimeWindow();
	virtual void Update();
	virtual void Connect(int Node1, int Node2, double ConTime);
};


class SlidingWindow:public ContactAggregation
{
protected:
	double **ContactTime;
	double WindowSize;
	int NN;
	double Oldest;
public:
	SlidingWindow(vector< dynamic_bitset<> > *p2Nodes,int NodeSize,double window);
	~SlidingWindow();
	virtual void Update(void);
	virtual void Connect(int Node1,int Node2,double ConTime);
private:
	void ReCreateContactGraph(double ConTime);
};


class MFaggregation:public ContactAggregation
{
protected:
	double **MostFrequent;
	double TargetDensity;
	double CurrentDensity;
	int ActiveEdges;
	int AllEdges;
	double FreqLimit;
	int NN;
	int NumEdgesLimit;
public:
	MFaggregation(int NodeSize,vector< dynamic_bitset<> > *p2Nodes,double Density);
	~MFaggregation();
	virtual void Update(void);
	virtual void Connect(int Node1,int Node2,double ConTime);
private:
	void ReCreateContactGraph(void);
};


class MRaggregation:public ContactAggregation
{
protected:
	double **LastSeen;
	double TargetDensity;
	double CurrentDensity;
	int ActiveEdges;
	int AllEdges;
	double RecencyLimit;
	int NN;
	int NumEdgesLimit;
public:
	MRaggregation(int NodeSize,vector< dynamic_bitset<> > *p2Nodes,double Density);
	~MRaggregation();
	virtual void Update(void);
	virtual void Connect(int Node1,int Node2,double ConTime);
private:
	void ReCreateContactGraph(double ConTime);
};

