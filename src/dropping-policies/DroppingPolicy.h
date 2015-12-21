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
 *  Written by Dimitrios-Georgios Akestoridis.
 */


#include <stdlib.h>
#include <stdio.h>
#include <list>

#ifndef PACKET_ENTRY_H
	#define PACKET_ENTRY_H
	#include "../core/PacketEntry.h"
#endif

#ifndef SETTINGS_H
	#define SETTINGS_H
	#include "../core/Settings.h"
#endif

#ifndef STATS_H
	#define STATS_H
	#include "../core/Statistics.h"
#endif


using namespace std;

class DroppingPolicy
{
public:
	list<PacketEntry*> *Buf;
	Statistics *Stat;
	DroppingPolicy(list<PacketEntry*> *B, Statistics *St);
	virtual ~DroppingPolicy();
	virtual double Drop() = 0;
};
