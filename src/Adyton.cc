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


#ifndef SIMULATOR_H
	#define SIMULATOR_H
	#include "core/Simulator.h"
#endif

#ifndef USERINTERACTION_H
	#define USERINTERACTION_H
	#include "core/UserInteraction.h"
#endif



int main(int argc, char *argv[])
{
	UserInteraction *UI;
	Simulator *Sim;


	/* Construct the UserInteraction amd Simulator objects */
	UI = new UserInteraction();
	Sim = new Simulator(UI->getSettings(argc, argv));


	/* Perform the simulation */
	Sim->performSimulation();


	/* Destruct the UserInteraction and Simulator objects */
	delete Sim;
	delete UI;

	return EXIT_SUCCESS;
}
