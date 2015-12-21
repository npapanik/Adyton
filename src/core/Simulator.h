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
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef NODE_H
	#define NODE_H
	#include "Node.h"
#endif

#ifndef EVENT_H
	#define EVENT_H
	#include "Event.h"
#endif

#ifndef EVENT_LIST_H
	#define EVENT_LIST_H
	#include "EventList.h"
#endif

#ifndef MAC_H
	#define MAC_H
	#include "MAC.h"
#endif

#ifndef PACKETPOOL_H
	#define PACKETPOOL_H
	#include "PacketPool.h"
#endif

#ifndef STATS_H
	#define STATS_H
	#include "Statistics.h"
#endif

#ifndef TRAFFICGEN_H
	#define TRAFFICGEN_H
	#include "TrafficGen.h"
#endif

#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "Settings.h"
#endif

#ifndef GOD_H
	#define GOD_H
	#include "God.h"
#endif

#ifndef RES_H
	#define RES_H
	#include "Results.h"
#endif

/* Simulator
 * --------
 * The basic class of Simulation.
 */
class Simulator
{
private:
	TrafficGen *Traffic;

public:
	ConnectionMap *CMap;
	//The ordered list of events of the Simulation
	EventList *SimList;
	MAC *MAClayer;
	//Current Simulation Time
	double CurrentTime;
	vector<Node*> Nodes;
	PacketPool *Pool;
	vector<Statistics*> Stats;
	Results *Res;
	God *SimGod;
	Settings *Set;
	long int CurrentReadBlock;
	long int ReadBlockSize;
	long int NumberReadBlocks;

	Simulator(Settings *S);
	~Simulator();
	void performSimulation();
	void startSimulation();
	void LoadContacts();
	void LoadCompleteContacts();
	void LoadPartialContacts();
	void LoadTraffic();
	bool Check(Transmission *trans);
};
