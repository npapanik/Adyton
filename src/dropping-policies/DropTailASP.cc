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


#ifndef DROP_TAIL_ASP_H
	#define DROP_TAIL_ASP_H
	#include "DropTailASP.h"
#endif

DropTailASP::DropTailASP(list<PacketEntry*> *B, Statistics *St): DroppingPolicy(B, St)
{
	return;
}


DropTailASP::~DropTailASP()
{
	return;
}


double DropTailASP::Drop()
{
	bool found;
	double drpCrtTime;
	int drpHops;
	list<PacketEntry*>::iterator it;
	list<PacketEntry*>::iterator drop_cand;


	found = false;
	drpCrtTime = 0.0;
	drpHops = 0;

	for(it = Buf->begin(); it != Buf->end(); ++it)
	{
		if((*it)->Hops > 0)
		{
			found = true;
			drpCrtTime = (*it)->CreationTime;
			drpHops = (*it)->Hops;
			drop_cand = it;
		}
	}

	if(found)
	{
		delete (*drop_cand);
		Buf->erase(drop_cand);
	}
	else
	{
		drpCrtTime = Buf->back()->CreationTime;
		drpHops = Buf->back()->Hops;

		delete Buf->back();
		Buf->pop_back();
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
