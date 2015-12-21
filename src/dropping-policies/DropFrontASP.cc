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


#ifndef DROP_FRONT_ASP_H
	#define DROP_FRONT_ASP_H
	#include "DropFrontASP.h"
#endif


DropFrontASP::DropFrontASP(list<PacketEntry*> *B, Statistics *St): DroppingPolicy(B, St)
{
	return;
}


DropFrontASP::~DropFrontASP()
{
	return;
}


double DropFrontASP::Drop()
{
	bool found;
	double drpCrtTime;
	int drpHops;
	list<PacketEntry*>::iterator it;


	found = false;
	drpCrtTime = 0.0;
	drpHops = 0;

	for(it = Buf->begin(); it != Buf->end(); ++it)
	{
		if((*it)->Hops > 0)
		{
			drpCrtTime = (*it)->CreationTime;
			drpHops = (*it)->Hops;
			delete (*it);
			Buf->erase(it);
			found = true;
			break;
		}
	}

	if(!found)
	{
		drpCrtTime = Buf->front()->CreationTime;
		drpHops = Buf->front()->Hops;
		delete Buf->front();
		Buf->pop_front();
	}

	Stat->incPktsDropped(drpCrtTime);
	if(drpHops == 0)
	{
		Stat->incSrcPktsDropped(drpCrtTime);
	}
	else
	{
		Stat->incRelPktsDropped(drpCrtTime);
	}

	return drpCrtTime;
}

