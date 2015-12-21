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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>

#define MAX_SIMULATION_TIME 6739218
#define GRANULARITY 20
#define MAX_LINE_SIZE 128

using namespace std;


const unsigned long minID = 1;
const unsigned long maxID = 275;

unsigned long leftNode;
unsigned long rightNode;
unsigned long simulationTime;
char leftTimeSlots[MAX_SIMULATION_TIME + 1][MAX_LINE_SIZE];
char rightTimeSlots[MAX_SIMULATION_TIME + 1][MAX_LINE_SIZE];
FILE *leftFile;
FILE *rightFile;
FILE *outFile;


void createLeftTimeSlots();
void createRightTimeSlots();
bool commonAccessPoint();



int main(int argc, char *argv[])
{
	unsigned long contactUp;
	unsigned long contactDown;
	unsigned long pairsCounter;
	char outputFilename[MAX_LINE_SIZE];
	bool alreadyConnected;


	/* Create the output filename */
	sprintf(outputFilename, "../process5/unsortedContacts.txt");


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Follow this process for each pair of nodes */
	pairsCounter = 0;
	for(leftNode = minID; leftNode < maxID; leftNode++)
	{
		createLeftTimeSlots();

		for(rightNode = leftNode + 1; rightNode <= maxID; rightNode++)
		{
			createRightTimeSlots();
			alreadyConnected = false;
			pairsCounter++;

			for(simulationTime = 0; simulationTime <= MAX_SIMULATION_TIME; simulationTime++)
			{
				if(commonAccessPoint())
				{
					if(alreadyConnected)
					{
						/* Update contact duration */
						contactDown = simulationTime;
					}
					else
					{
						/* New contact */
						contactUp = simulationTime;
						contactDown = simulationTime;
						alreadyConnected = true;
					}
				}
				else
				{
					if(alreadyConnected)
					{
						/* Contact ended, write total contact duration */
						fprintf(outFile, "%lu\t%lu\t%lu\t%lu\n", leftNode, rightNode, contactUp, contactDown);
						alreadyConnected = false;
					}
					else
					{
						/* Do nothing */
					}
				}
			}

			if(alreadyConnected)
			{
				/* Contact ended, write total contact duration */
				fprintf(outFile, "%lu\t%lu\t%lu\t%lu\n", leftNode, rightNode, contactUp, contactDown);
				alreadyConnected = false;
			}

			//printf("Progress: %.2f%%\r", 100.0 * (((double) pairsCounter) / (((double) (maxID * (maxID - 1))) / 2.0)));
		}
	}


	/* Close the output file */
	if(fclose(outFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}



void createLeftTimeSlots()
{
	unsigned long i;
	unsigned long field;
	unsigned long tmpSimulationTime;
	char tmpAccessPoints[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char line[MAX_LINE_SIZE];
	char *value;


	/* Initialize the time slots */
	for(i = 0; i <= MAX_SIMULATION_TIME; i++)
	{
		strcpy(leftTimeSlots[i], "\0");
	}


	/* Create the input filename */
	sprintf(inputFilename, "node_%lu.txt", leftNode);


	/* Open the input file */
	if(!(leftFile = fopen(inputFilename, "r")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Read the file line by line */
	while(fgets(line, MAX_LINE_SIZE, leftFile))
	{
		if(line[0] == '\n')
		{
			continue;
		}

		field = 0;
		tmpSimulationTime = 0;
		value = strtok(line, "\t");
		while(value)
		{
			field++;

			if(field == 1)
			{/* Simulation time */
				tmpSimulationTime = strtoul(value, NULL, 10);
			}
			else if(field == 2)
			{/* List of access point IDs */
				strcpy(tmpAccessPoints, value);
			}
			else
			{
				printf("\nError at the input file node_%lu.txt!\nUnexpected field #%lu: %s\n\n", leftNode, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}

		if(field == 2)
		{
			for(i = tmpSimulationTime; i <= tmpSimulationTime + GRANULARITY; i++)
			{
				strcpy(leftTimeSlots[i], tmpAccessPoints);
			}
		}
		else
		{
			printf("\nError at the input file node_%lu.txt!\nExpected to read 2 fields, but read %lu fields\n\n", leftNode, field);
			exit(EXIT_FAILURE);
		}
	}


	/* Close the input file */
	if(fclose(leftFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}
}


void createRightTimeSlots()
{
	unsigned long i;
	unsigned long field;
	unsigned long tmpSimulationTime;
	char tmpAccessPoints[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char line[MAX_LINE_SIZE];
	char *value;


	/* Initialize the time slots */
	for(i = 0; i <= MAX_SIMULATION_TIME; i++)
	{
		strcpy(rightTimeSlots[i], "\0");
	}


	/* Create the input filename */
	sprintf(inputFilename, "node_%lu.txt", rightNode);


	/* Open the input file */
	if(!(rightFile = fopen(inputFilename, "r")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Read the file line by line */
	while(fgets(line, MAX_LINE_SIZE, rightFile))
	{
		if(line[0] == '\n')
		{
			continue;
		}

		field = 0;
		tmpSimulationTime = 0;
		value = strtok(line, "\t");
		while(value)
		{
			field++;

			if(field == 1)
			{/* Simulation time */
				tmpSimulationTime = strtoul(value, NULL, 10);
			}
			else if(field == 2)
			{/* List of access point IDs */
				strcpy(tmpAccessPoints, value);
			}
			else
			{
				printf("\nError at the input file node_%lu.txt!\nUnexpected field #%lu: %s\n\n", rightNode, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}

		if(field == 2)
		{
			for(i = tmpSimulationTime; i <= tmpSimulationTime + GRANULARITY; i++)
			{
				strcpy(rightTimeSlots[i], tmpAccessPoints);
			}
		}
		else
		{
			printf("\nError at the input file node_%lu.txt!\nExpected to read 2 fields, but read %lu fields\n\n", rightNode, field);
			exit(EXIT_FAILURE);
		}
	}


	/* Close the input file */
	if(fclose(rightFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}
}


bool commonAccessPoint()
{
	char listOfAccessPoints[MAX_LINE_SIZE];
	char *value;
	list<unsigned long> leftList;
	list<unsigned long> rightList;
	list<unsigned long>::iterator leftIterator;
	list<unsigned long>::iterator rightIterator;


	if(!strcmp(leftTimeSlots[simulationTime], "\0") || (!strcmp(rightTimeSlots[simulationTime], "\0")))
	{
		return false;
	}
	else
	{
		/* Create the list of the left node */
		strcpy(listOfAccessPoints, leftTimeSlots[simulationTime]);
		value = strtok(listOfAccessPoints, ",");

		while(value)
		{
			leftList.push_back(strtoul(value, NULL, 10));
			value = strtok(NULL, ",");
		}


		/* Create the list of the right node */
		strcpy(listOfAccessPoints, rightTimeSlots[simulationTime]);
		value = strtok(listOfAccessPoints, ",");

		while(value)
		{
			rightList.push_back(strtoul(value, NULL, 10));
			value = strtok(NULL, ",");
		}


		/* Search for at least a common access point */
		for(leftIterator = leftList.begin(); leftIterator != leftList.end(); ++leftIterator)
		{
			for(rightIterator = rightList.begin(); rightIterator != rightList.end(); ++rightIterator)
			{
				if(*leftIterator == *rightIterator)
				{
					return true;
				}
			}
		}

		return false;
	}
}
