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


#include <stdlib.h>
#include <stdio.h>
#include <vector>

#ifndef CONNECTION_H
	#define CONNECTION_H
	#include "Connection.h"
#endif

/* ConnectionMap
 * -------------
 * This class contains the connections of all nodes of the network.
 * Note that ConnectionMap stores real connections between nodes
 * during Simulation.
 */
class ConnectionMap
{
public:
	int NetworkNodes;
	vector<Connection*> Map;
	ConnectionMap(int Nodes);
	~ConnectionMap();
	void SetConnection(int NodeID,int Position);
	void UnSetConnection(int NodeID,int Position);
	int NumberOfN(int NodeID);
	int *GetN(int NodeID);
	int *GetReachableNodes(int NID);
	void PrintConnections(int NodeID);
	bool AreConnected(int NodeA,int NodeB);
};
