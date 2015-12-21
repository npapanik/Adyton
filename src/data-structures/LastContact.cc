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


#ifndef LAST_CONTACT_H
	#define LAST_CONTACT_H
	#include "LastContact.h"
#endif

LastContact::LastContact(int ID,int NodeSize): Utility(ID,NodeSize)
{
	this->LastTimeSeen=-1.0;
	return;
}

LastContact::~LastContact()
{
	return;
}

void LastContact::Update(int ID,double CurrentTime)
{
	return;
}

void LastContact::ContactDown(int ID, double CurrentTime)
{
	if(ID < 0 || ID > this->NSize)
	{
		printf("LST utility:Node %d does not exist!\nAborting..\n",ID);
		exit(1);
	}
	this->LastTimeSeen=CurrentTime;
}

double LastContact::get(int ID,double CurrentTime)
{
	if(this->LastTimeSeen < 0)
	{
			return 0.0;
	}
	double diff=CurrentTime-this->LastTimeSeen;
	double tmp=1.0/(1.0+diff);
	
	if(!(tmp >= 0.0 && tmp <=1.0))
	{
		printf("[%d->%d]\n",this->NID,ID);
		printf("Current Time:%f, Last Time Seen:%f\n",CurrentTime,this->LastTimeSeen);
		printf("Value of LTS utility is %f\n",tmp);
		printf("LTS Utility value is not between 0.0 and 1.0\nAborting..\n");
		exit(1);
	}
	return tmp;
}
