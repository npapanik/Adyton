/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015-2018  Nikolaos Papanikos,
 *  Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <list>

using namespace std;


class Position
{
private:
	double latitude;
	double longitude;
	unsigned long long int startTime;
	unsigned long long int endTime;

public:
	Position(double lat, double lon, unsigned long long int start, unsigned long long int end)
	{
		latitude = lat;
		longitude = lon;
		startTime = start;
		endTime = end;
	}

	double getLatitude()
	{
		return latitude;
	}

	double getLongitude()
	{
		return longitude;
	}

	unsigned long long int getStartTime()
	{
		return startTime;
	}

	unsigned long long int getEndTime()
	{
		return endTime;
	}
};


class TimeSlot
{
private:
	unsigned long long int startTime;
	unsigned long long int endTime;
	bool connected;

public:
	TimeSlot(unsigned long long int start, unsigned long long int end)
	{
		startTime = start;
		endTime = end;
		connected = false;
	}

	unsigned long long int getStartTime()
	{
		return startTime;
	}

	unsigned long long int getEndTime()
	{
		return endTime;
	}

	bool areConnected()
	{
		return connected;
	}

	void setConnection(bool conn)
	{
		connected = conn;
	}
};


const unsigned int minID = 0;
const unsigned int maxID = 535;

const unsigned int maxLineSize = 64;

const char outputFN[] = "../process5/unsorted_trace.txt";

const double R = 6371000.0;			/* radius of the earth in meters */
const double commRange = 100.0;		/* Wi-Fi range in meters */
const double pi = 3.141592653589793;


double calcDistance(double lat1, double lon1, double lat2, double lon2)
{
	double Dlat;
	double Dlon;
	double a;
	double c;
	double d;


	Dlat = ((lat2 - lat1) * pi) / 180.0;
	Dlon = ((lon2 - lon1) * pi) / 180.0;

	a = pow(sin(Dlat / 2.0), 2.0) + cos((lat1 * pi) / 180.0) * cos((lat2 * pi) / 180.0) * pow(sin(Dlon / 2.0), 2.0);

	c = 2.0 * atan2(sqrt(a), sqrt(1 - a));

	d = R * c;

	return d;
}



int main(int argc, char *argv[])
{
	unsigned int i;
	unsigned int j;
	unsigned int field;
	bool firstPos;
	double currLat;
	double currLon;
	unsigned long long int currUpdateTime;
	double nextLat;
	double nextLon;
	unsigned long long int nextUpdateTime;
	Position *tmpPos;
	list<Position*> nodeA;
	list<Position*> nodeB;
	list<Position*>::iterator itA;
	list<Position*>::iterator itB;
	unsigned long long int slotStart;
	unsigned long long int slotEnd;
	TimeSlot *tmpSlot;
	list<TimeSlot*> slots;
	list<TimeSlot*>::iterator itS;
	bool alreadyConnected;
	unsigned long long int contactUp;
	unsigned long long int contactDown;
	char line[maxLineSize];
	char inputFN[maxLineSize];
	char *word;
	FILE *infile;
	FILE *outfile;

	for(i = minID; i <= maxID; i++)
	{
		//printf("Progress: %.2f%%\r", 100.0 * (((double) i) / ((double) (maxID + 1))));
		fflush(stdout);

		sprintf(inputFN, "taxi-%u.txt", i);

		if(!(infile = fopen(inputFN, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFN);
			exit(EXIT_FAILURE);
		}

		firstPos = true;
		while(fgets(line, maxLineSize, infile))
		{
			if(line[0] == '\n')
			{
				continue;
			}

			field = 0;
			word = strtok(line, "\t");
			while(word)
			{
				field++;

				if(firstPos)
				{
					if(field == 1)
					{
						currLat = atof(word);
					}
					else if(field == 2)
					{
						currLon = atof(word);
					}
					else if(field == 3)
					{
						currUpdateTime = strtoull(word, NULL, 10);

						firstPos = false;
					}
					else
					{
						printf("\nError! Unknown field: %u\n\n", field);
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					if(field == 1)
					{
						nextLat = atof(word);
					}
					else if(field == 2)
					{
						nextLon = atof(word);
					}
					else if(field == 3)
					{
						nextUpdateTime = strtoull(word, NULL, 10);

						tmpPos = new Position(currLat, currLon, currUpdateTime, nextUpdateTime - 1);
						nodeA.push_back(tmpPos);

						currLat = nextLat;
						currLon = nextLon;
						currUpdateTime = nextUpdateTime;
					}
					else
					{
						printf("\nError! Unknown field: %u\n\n", field);
						exit(EXIT_FAILURE);
					}
				}

				word = strtok(NULL, "\t");
			}
		}

		if(fclose(infile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", inputFN);
			exit(EXIT_FAILURE);
		}


		for(j= i + 1; j <= maxID; j++)
		{
			sprintf(inputFN, "taxi-%u.txt", j);

			if(!(infile = fopen(inputFN, "r")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", inputFN);
				exit(EXIT_FAILURE);
			}

			firstPos = true;
			while(fgets(line, maxLineSize, infile))
			{
				if(line[0] == '\n')
				{
					continue;
				}

				field = 0;
				word = strtok(line, "\t");
				while(word)
				{
					field++;

					if(firstPos)
					{
						if(field == 1)
						{
							currLat = atof(word);
						}
						else if(field == 2)
						{
							currLon = atof(word);
						}
						else if(field == 3)
						{
							currUpdateTime = strtoull(word, NULL, 10);

							firstPos = false;
						}
						else
						{
							printf("\nError! Unknown field: %u\n\n", field);
							exit(EXIT_FAILURE);
						}
					}
					else
					{
						if(field == 1)
						{
							nextLat = atof(word);
						}
						else if(field == 2)
						{
							nextLon = atof(word);
						}
						else if(field == 3)
						{
							nextUpdateTime = strtoull(word, NULL, 10);

							tmpPos = new Position(currLat, currLon, currUpdateTime, nextUpdateTime - 1);
							nodeB.push_back(tmpPos);

							currLat = nextLat;
							currLon = nextLon;
							currUpdateTime = nextUpdateTime;
						}
						else
						{
							printf("\nError! Unknown field: %u\n\n", field);
							exit(EXIT_FAILURE);
						}
					}

					word = strtok(NULL, "\t");
				}
			}

			if(fclose(infile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", inputFN);
				exit(EXIT_FAILURE);
			}


			itA = nodeA.begin();
			itB = nodeB.begin();

			if((*itA)->getStartTime() < (*itB)->getStartTime())
			{
				do
				{
					++itA;
				}while((*itA)->getStartTime() < (*itB)->getStartTime() && itA != nodeA.end());
			}
			else if((*itA)->getStartTime() > (*itB)->getStartTime())
			{
				do
				{
					++itB;
				}while((*itA)->getStartTime() > (*itB)->getStartTime() && itB != nodeB.end());
			}

			if(itA != nodeA.end() && itB != nodeB.end())
			{
				if((*itA)->getStartTime() < (*itB)->getStartTime())
				{
					slotStart = (*itA)->getStartTime();
					++itA;
				}
				else
				{
					slotStart = (*itB)->getStartTime();
					++itB;
				}
			}

			while(itA != nodeA.end() && itB != nodeB.end())
			{
				if((*itA)->getStartTime() < (*itB)->getStartTime())
				{
					slotEnd = (*itA)->getStartTime() - 1;

					tmpSlot = new TimeSlot(slotStart, slotEnd);
					slots.push_back(tmpSlot);

					slotStart = (*itA)->getStartTime();
					++itA;
				}
				else
				{
					slotEnd = (*itB)->getStartTime() - 1;

					tmpSlot = new TimeSlot(slotStart, slotEnd);
					slots.push_back(tmpSlot);

					slotStart = (*itB)->getStartTime();
					++itB;
				}
			}

			if(itA == nodeA.end() && !slots.empty())
			{
				while(itB != nodeB.end())
				{
					if((*itB)->getStartTime() < nodeA.back()->getEndTime())
					{
						slotEnd = (*itB)->getStartTime() - 1;

						tmpSlot = new TimeSlot(slotStart, slotEnd);
						slots.push_back(tmpSlot);

						slotStart = (*itB)->getStartTime();
						++itB;
					}
					else
					{
						break;
					}
				}

				slotStart = slots.back()->getEndTime() + 1;
				slotEnd = nodeA.back()->getEndTime();

				tmpSlot = new TimeSlot(slotStart, slotEnd);
				slots.push_back(tmpSlot);
			}
			else if(itB == nodeB.end() && !slots.empty())
			{
				while(itA != nodeA.end())
				{
					if((*itA)->getStartTime() < nodeB.back()->getEndTime())
					{
						slotEnd = (*itA)->getStartTime() - 1;

						tmpSlot = new TimeSlot(slotStart, slotEnd);
						slots.push_back(tmpSlot);

						slotStart = (*itA)->getStartTime();
						++itA;
					}
					else
					{
						break;
					}
				}

				slotStart = slots.back()->getEndTime() + 1;
				slotEnd = nodeB.back()->getEndTime();

				tmpSlot = new TimeSlot(slotStart, slotEnd);
				slots.push_back(tmpSlot);
			}


			itA = nodeA.begin();
			itB = nodeB.begin();
			for(itS=slots.begin(); itS!=slots.end(); ++itS)
			{
				while(itA != nodeA.end() && (*itA)->getEndTime() < (*itS)->getStartTime())
				{
					++itA;
				}

				while(itB != nodeB.end() && (*itB)->getEndTime() < (*itS)->getStartTime())
				{
					++itB;
				}

				if(itA != nodeA.end() && itB != nodeB.end())
				{
					if(calcDistance((*itA)->getLatitude(), (*itA)->getLongitude(), (*itB)->getLatitude(), (*itB)->getLongitude()) <= commRange)
					{
						(*itS)->setConnection(true);
					}
					else
					{
						(*itS)->setConnection(false);
					}
				}
			}

			if(!(outfile = fopen(outputFN, "a")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", outputFN);
				exit(EXIT_FAILURE);
			}

			alreadyConnected = false;
			for(itS=slots.begin(); itS!=slots.end(); ++itS)
			{
				if((*itS)->areConnected() && !alreadyConnected)
				{
					contactUp = (*itS)->getStartTime();
					alreadyConnected = true;
				}
				else if((*itS)->areConnected() && alreadyConnected)
				{
					continue;
				}
				else if(!(*itS)->areConnected() && !alreadyConnected)
				{
					continue;
				}
				else if(!(*itS)->areConnected() && alreadyConnected)
				{
					contactDown = (*itS)->getStartTime()-1;
					alreadyConnected = false;

					fprintf(outfile, "%u\t%u\t%llu\t%llu\n", i, j, contactUp, contactDown);
				}
			}

			if(fclose(outfile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", outputFN);
				exit(EXIT_FAILURE);
			}

			for(itS=slots.begin(); itS!=slots.end(); ++itS)
			{
				delete (*itS);
				(*itS) = NULL;
			}
			slots.clear();

			for(itB=nodeB.begin(); itB!=nodeB.end(); ++itB)
			{
				delete (*itB);
				(*itB) = NULL;
			}
			nodeB.clear();
		}

		for(itA=nodeA.begin(); itA!=nodeA.end(); ++itA)
		{
			delete (*itA);
			(*itA) = NULL;
		}
		nodeA.clear();
	}

	return EXIT_SUCCESS;
}
