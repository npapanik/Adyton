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


#ifndef DROP_OLDEST_H
	#define DROP_OLDEST_H
	#include "DropOldest.h"
#endif


DropOldest::DropOldest(list<PacketEntry*> *B, Statistics *St): DroppingPolicy(B, St)
{
	return;
}


DropOldest::~DropOldest()
{
	return;
}


double DropOldest::Drop()
{
	double oldestTime;
	int drpHops;
	list<PacketEntry*>::iterator it;
	list<PacketEntry*>::iterator drop_cand;


	drop_cand = Buf->begin();
	oldestTime = (*(Buf->begin()))->CreationTime;
	drpHops = (*(Buf->begin()))->Hops;

	for(it = Buf->begin(); it != Buf->end(); ++it)
	{
		if((*it)->CreationTime < oldestTime)
		{
			oldestTime = (*it)->CreationTime;
			drpHops = (*it)->Hops;
			drop_cand = it;
		}
	}

	delete (*drop_cand);
	Buf->erase(drop_cand);

	Stat->incPktsDropped(oldestTime);
	if(drpHops == 0)
	{
		Stat->incSrcPktsDropped(oldestTime);
	}
	else
	{
		Stat->incRelPktsDropped(oldestTime);
	}

	return oldestTime;
}
