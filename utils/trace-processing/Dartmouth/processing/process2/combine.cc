/*
 *  Adyton: A Network Simulator for Opportunistic Networks
 *  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
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
#include <string>
#include <list>

#define DAYS_TO_SECONDS 86400
#define MAX_LINE_SIZE 512

using namespace std;


const unsigned long long int minNodeID = 0;
const unsigned long long int maxNodeID = 13887;

const unsigned long long int minSimulationTime = 1067731200;
const unsigned long long int maxNumDays = 119;

const unsigned long long int minDaysPresent = 85;



int main(int argc, char *argv[])
{
	unsigned long long int i;
	unsigned long long int currNodeID;
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int prevTime;
	unsigned long long int currTime;
	unsigned long long int currDay;
	unsigned long long int numDaysPresent;
	char line[MAX_LINE_SIZE];
	char prevAP[MAX_LINE_SIZE];
	char currAP[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char outputFilename[MAX_LINE_SIZE];
	char *value;
	bool found;
	bool disconnectedNode;
	bool firstEntry;
	bool *daysPresent;
	list<string> uniqueAPs;
	list<string>::iterator itAP;
	FILE *inFile;
	FILE *outFile;


	/* Check for which days each node was present in the network */
	if((daysPresent = (bool *) malloc(maxNumDays * sizeof(bool))) == NULL)
	{
		printf("\n[Error]: Unable to allocate memory for the Boolean array\n\n");
		exit(EXIT_FAILURE);
	}


	/* Combine the contacts of nodes associated with the same access point */
	for(currNodeID = minNodeID; currNodeID <= maxNodeID; currNodeID++)
	{
		/* Initializations */
		numDaysPresent = 0;
		for(i = 0; i < maxNumDays; i++)
		{
			daysPresent[i] = false;
		}


		/* Create the input filename */
		sprintf(inputFilename, "node-%llu.txt", currNodeID);


		/* Open the input file */
		if(!(inFile = fopen(inputFilename, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Read the file line by line */
		lineCounter = 0;
		while(fgets(line, MAX_LINE_SIZE, inFile))
		{
			lineCounter++;

			if(line[0] == '\n')
			{
				continue;
			}

			field = 0;
			value = strtok(line, "\t");
			while(value)
			{
				field++;

				if(field == 1)
				{/* Unix timestamp */
					currTime = strtoull(value, NULL, 10);
				}
				else if(field == 2)
				{/* Associated Access Point */
					strcpy(currAP, value);

					for(i = 0; i < MAX_LINE_SIZE; i++)
					{
						if(currAP[i] == '\n')
						{
							currAP[i] = '\0';
							break;
						}
					}
				}
				else
				{
					printf("\nError at line #%llu of the input file %s!\nUnexpected field #%llu: %s\n\n", lineCounter, inputFilename, field, value);
					exit(EXIT_FAILURE);
				}

				value = strtok(NULL, "\t");
			}


			/* Count the number of days that each node was present in the network */
			if(strcmp(currAP, "OFF") && currTime >= minSimulationTime && currTime < (minSimulationTime + (maxNumDays * DAYS_TO_SECONDS)))
			{
				currDay = (currTime - minSimulationTime) / DAYS_TO_SECONDS;

				if(currDay >= maxNumDays)
				{
					printf("\nError at line #%llu of the input file %s!\nThe unix timestamp (%llu) unexpectedly exceeds the maximum number of days (%llu)\n\n", lineCounter, inputFilename, currTime, maxNumDays);
					exit(EXIT_FAILURE);
				}
				else
				{
					if(!daysPresent[currDay])
					{
						daysPresent[currDay] = true;
						numDaysPresent++;
					}
				}
			}
		}


		/* Close the input file */
		if(fclose(inFile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Consider contacts only from nodes that are present in the network for a minimum number of days */
		if(numDaysPresent >= minDaysPresent)
		{
			/* Open the input file */
			if(!(inFile = fopen(inputFilename, "r")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}


			/* Read the file line by line */
			disconnectedNode = true;
			firstEntry = true;
			lineCounter = 0;
			while(fgets(line, MAX_LINE_SIZE, inFile))
			{
				lineCounter++;

				if(line[0] == '\n')
				{
					continue;
				}

				field = 0;
				value = strtok(line, "\t");
				while(value)
				{
					field++;

					if(field == 1)
					{/* Unix timestamp */
						currTime = strtoull(value, NULL, 10);
					}
					else if(field == 2)
					{/* Associated Access Point */
						strcpy(currAP, value);

						for(i = 0; i < MAX_LINE_SIZE; i++)
						{
							if(currAP[i] == '\n')
							{
								currAP[i] = '\0';
								break;
							}
						}
					}
					else
					{
						printf("\nError at line #%llu of the input file %s!\nUnexpected field #%llu: %s\n\n", lineCounter, inputFilename, field, value);
						exit(EXIT_FAILURE);
					}

					value = strtok(NULL, "\t");
				}


				/* Sanity check */
				if(!firstEntry)
				{
					if(prevTime > currTime)
					{
						printf("\nError at line #%llu of the input file %s!\nTime is going backwards (previous unix timestamp = %llu, current unix timestamp = %llu)\n\n", lineCounter, inputFilename, prevTime, currTime);
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					firstEntry = false;
				}


				/* Calculate the contacts of the node with each access point */
				if(!disconnectedNode)
				{
					if(prevTime >= minSimulationTime && currTime < (minSimulationTime + (maxNumDays * DAYS_TO_SECONDS)))
					{
						/* Create the output filename */
						sprintf(outputFilename, "../process3/%s.txt", prevAP);


						/* Open the output file */
						if(!(outFile = fopen(outputFilename, "a")))
						{
							printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
							exit(EXIT_FAILURE);
						}


						/* Normalize simulation time */
						fprintf(outFile, "%llu\t%llu\t%llu\n", currNodeID, (prevTime - minSimulationTime), (currTime - minSimulationTime));


						/* Close the output file */
						if(fclose(outFile))
						{
							printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
							exit(EXIT_FAILURE);
						}


						/* Collect the access point names */
						found = false;
						for(itAP = uniqueAPs.begin(); itAP != uniqueAPs.end(); ++itAP)
						{
							if(!strcmp((*itAP).c_str(), prevAP))
							{
								found = true;
								break;
							}
						}

						if(!found)
						{
							uniqueAPs.push_back(prevAP);
						}
					}
				}

				prevTime = currTime;
				strcpy(prevAP, currAP);

				if(!strcmp(currAP, "OFF"))
				{
					disconnectedNode = true;
				}
				else
				{
					disconnectedNode = false;
				}
			}


			/* Close the input file */
			if(fclose(inFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}
		}

		//printf("Progress: %.2f%%\r", 100.0 * (((double) currNodeID) / ((double) (maxNodeID + 1))));
		fflush(stdout);
	}


	/* Create the output filename */
	sprintf(outputFilename, "../process3/uniqueAPs.txt");


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Provide a list of all the access point names */
	for(itAP = uniqueAPs.begin(); itAP != uniqueAPs.end(); ++itAP)
	{
		fprintf(outFile, "%s\n", (*itAP).c_str());
	}


	/* Close the output file */
	if(fclose(outFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
