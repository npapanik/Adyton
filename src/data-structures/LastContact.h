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


#include <stdio.h>
#include <stdlib.h>

#ifndef UTILITY_H
	#define UTILITY_H
	#include "Utility.h"
#endif

class LastContact:public Utility
{
protected:
	double LastTimeSeen;
public:
	LastContact(int ID,int NodeSize);
	~LastContact();
	virtual void Update(int ID,double CurrentTime);
	virtual double get(int ID,double CurrentTime);
	virtual void ContactDown(int ID,double CurrentTime);
private:
	bool Reset(double CurrentTime);
};
