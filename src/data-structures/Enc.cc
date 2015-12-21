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


#ifndef ENC_H
	#define ENC_H
	#include "Enc.h"
#endif

/* Description
 * -----------
 *  Encounters
 *  This utility calculates the number of contacts of a node.
 *  It is destination independent, so each node calculates this value
 *  for itself.
 */

Enc::Enc(int ID,int NodeSize,double a,double W): Utility(ID,NodeSize)
{
	this->alpha=a;
	this->Window=W;
	this->LastUpdated=0.0;
	this->EV=0.0;
	this->CWC=0.0;
	this->stage=0;
	this->DecayEnabled=true;
	return;
}

Enc::Enc(int ID,int NodeSize): Utility(ID,NodeSize)
{
	this->alpha=0.0;
	this->Window=0.0;
	this->LastUpdated=0.0;
	this->EV=0.0;
	this->CWC=0.0;
	this->stage=0;
	this->DecayEnabled=false;
	return;
}

Enc::~Enc()
{
	return;
}


void Enc::Update(int ID,double CurrentTime)
{
	return;
}

void Enc::ContactUp(int ID, double CurrentTime)
{
	if(Reset(CurrentTime) && this->DecayEnabled)
	{
		this->EV=(this->alpha*this->CWC)+((1-this->alpha)*this->EV);
		this->CWC=0.0;
		this->stage++;
	}
	this->CWC++;
}


double Enc::get(int ID,double CurrentTime)
{
	if(!(this->stage) || !(this->DecayEnabled))
	{
		return this->CWC;
	}
	else
	{
		return this->EV;
	}
}

bool Enc::Reset(double CurrentTime)
{
	if((CurrentTime - LastUpdated) > this->Window)
	{
		return true;
	}
	else
	{
		return false;
	}
}
