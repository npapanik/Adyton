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


#ifndef CONNECTION_H
	#define CONNECTION_H
	#include "Connection.h"
#endif


/* Constructor:Connection
 * ----------------------
 * Allocation of memory that will be used to store one hop neighborhood.
 */
Connection::Connection(int Nodes,int MyID)
{
	this->N=(int*)malloc(Nodes*sizeof(int));
	for(int i=0;i<Nodes;i++)
	{
		N[i]=0;
	}
	this->NumberOfNodes=0;
	this->ID=MyID;
	return;
}

Connection::~Connection()
{
	free(N);
	return;
}


