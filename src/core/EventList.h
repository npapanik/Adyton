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

#ifndef EVENT_H
	#define EVENT_H
	#include "Event.h"
#endif


/* EventList
 * ---------
 * EventList is an ordered list of events. The order is based on time property of events (increasing order).
 * The method InsertEvent is responsible of keeping events in order. 
 */

class EventList
{
private:
	double MinTime;
	int NumberEvents;
	class Event *Top;
public:
	EventList(void);
	void InsertEvent(class Event *e);
	class Event *GetTop(void);
	void PrintList(void);
};
