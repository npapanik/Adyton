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


#ifndef CONNECTIONMAP_H
	#define CONNECTIONMAP_H
	#include "ConnectionMap.h"
#endif

/* Constructor:ConnectionMap
 * -------------------------
 * A new instance of connection is created for each network node.
 */
ConnectionMap::ConnectionMap(int Nodes)
{
	this->NetworkNodes=Nodes;
	for(int i=0;i<Nodes;i++)
	{
		Connection *tmp=new Connection(Nodes,i);
		Map.push_back(tmp);
	}
	return;
}

ConnectionMap::~ConnectionMap()
{
	for(int i=0;i<NetworkNodes;i++)
	{
		delete Map[i];
	}
	return;
}



/* SetConnection
 * -------------
 * Node with "NodeID" is marked to have a new connection with node having
 * id "Position".
 */
void ConnectionMap::SetConnection(int NodeID, int Position)
{
	if(NodeID == Position)
	{
		return;
	}
	Connection *tmp=Map[NodeID];
	if(tmp->N[Position] != 1)
	{
		tmp->N[Position]=1;
		tmp->NumberOfNodes++;
	}
	
	return;
}

/* UnSetConnection
 * ---------------
 * Node with "NodeID" is now disconnected with node having id "Position".
 */
void ConnectionMap::UnSetConnection(int NodeID, int Position)
{
	Connection *tmp=Map[NodeID];
	if(tmp->N[Position] != 0)
	{
		tmp->N[Position]=0;
		tmp->NumberOfNodes--;
	}
	return;
}

/* NumberOfN
 * ---------
 * This method returns the number of connections that node with id "NodeID" has.
 */
int ConnectionMap::NumberOfN(int NodeID)
{
	Connection *tmp=Map[NodeID];
	return tmp->NumberOfNodes;
}

/* GetN
 * ----
 * This method returns an array that contains all one hop connections. 
 */
int *ConnectionMap::GetN(int NodeID)
{
	Connection *tmp=Map[NodeID];
	
	int pos=0;
	int *Nmatrix=(int *)malloc((tmp->NumberOfNodes)*(sizeof(int)));
	for(int i=0;i<(this->NetworkNodes);i++)
	{
		if(tmp->N[i] == 1)
		{
			Nmatrix[pos]=i;
			pos++;
		}
	}
	return Nmatrix;
}

/* PrintConnections
 * ----------------
 * For Debugging purporses: Node IDs of one hop connections of node having
 * id "NodeID" is printed. 
 */
void ConnectionMap::PrintConnections(int NodeID)
{
	int *N=this->GetN(NodeID);
	
	printf("%d:",NodeID);
	for(int i=0;i<this->NumberOfN(NodeID);i++)
	{
		printf("%d\t",N[i]);
	}
	printf("\n");
	return;
}

/* AreConnected
 * ------------
 * If there is a connection between NodeA & NodeB then this method returns
 * true. If not this method return false. Note that this method responds based
 * on information that NodeA has (NodeB's information is not checked).
 */
bool ConnectionMap::AreConnected(int NodeA,int NodeB)
{
	Connection *tmp=Map[NodeA];
	if(tmp->N[NodeB] == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
