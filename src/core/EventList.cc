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


#ifndef EVENT_LIST_H
	#define EVENT_LIST_H
	#include "EventList.h"
#endif


/* Constructor: EventList
 * ----------------------
 * Initializing attributes.
 */
EventList::EventList(void)
{
	this->MinTime=0.0;
	this->NumberEvents=0;
	this->Top=NULL;
}

/* GetTop
 * ------
 * Returning the event with smallest time value. 
 * Attributes of MinTime and NumberEvents are properly adjusted.
 */
Event* EventList::GetTop(void )
{
	if(Top == NULL)
	{
		return NULL;
	}
	Event *tmp=Top;
	Top=Top->Next;
	
	this->MinTime=tmp->getEventTime();
	this->NumberEvents--;
	return tmp;
}

/* InsertEvent
 * -----------
 * This method inserts a new event into the ordered list. The insertion keeps the order
 * between the instances of events. 
 */
void EventList::InsertEvent(Event* e)
{
	this->NumberEvents++;
	//case list is empty
	if(Top == NULL)
	{
		Top=e;
		e->Next=NULL;
		return;
	}
	
	Event *prev=NULL;  
	Event *runner=Top;
	while(runner != NULL)
	{
		if(runner->getEventTime() >= e->getEventTime())
		{
			break;
		}
		prev=runner;
		runner=runner->Next;
	}
	
	//case: first place
	if(prev == NULL)
	{
		e->Next=Top;
		Top=e;
		return;
	}
	
	//case: last place
	if(runner == NULL)
	{
		prev->Next=e;
		e->Next=NULL;
		return;
	}
	
	//default case: somewhere inside the list
	prev->Next=e;
	e->Next=runner;
	return;
}

/* PrintList
 * ---------
 * Events are printed (debugging purposes)
 * 
 */
void EventList::PrintList(void )
{
	Event *runner=Top;
	while(runner != NULL)
	{
		runner->printEvent();
		runner=runner->Next;
	}
}