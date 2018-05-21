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

#define MIN_NODE_ID 1
#define MAX_NODE_ID 789
#define GRANULARITY 20

#define MAX_LINE_SIZE 1024



int main(int argc, char *argv[])
{
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int pairsCounter;
	unsigned long long int currNodeA;
	unsigned long long int currNodeB;
	unsigned long long int tmpTime;
	unsigned long long int tmpEncNode;
	unsigned long long int currContactStart;
	unsigned long long int latestBeacon;
	char line[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char outputFilename[MAX_LINE_SIZE];
	char *value;
	bool firstContact;
	FILE *inFile;
	FILE *outFile;



	/* Create the output filename */
	sprintf(outputFilename, "../process2/unsortedStanford.txt");


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Read the mote file of each node */
	pairsCounter = 0;
	for(currNodeA = MIN_NODE_ID; currNodeA <= MAX_NODE_ID; currNodeA++)
	{
		/* Create the input filename */
		sprintf(inputFilename, "../../original/moteFiles/node-%llu", currNodeA);


		/* Calculate the contacts between each pair of nodes */
		for(currNodeB = MIN_NODE_ID; currNodeB <= MAX_NODE_ID; currNodeB++)
		{
			pairsCounter++;

			if(currNodeA == currNodeB)
			{
				continue;
			}


			/* Open the input file */
			if(!(inFile = fopen(inputFilename, "r")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}


			/* Read the input file line by line */
			lineCounter = 0;
			firstContact = true;
			while(fgets(line, MAX_LINE_SIZE, inFile))
			{
				lineCounter++;

				if(line[0] == '\n')
				{
					continue;
				}

				field = 0;
				value = strtok(line, " ");
				while(value)
				{
					field++;

					if(field == 1)
					{/* Sender's ID */
						tmpEncNode = strtoull(value, NULL, 10);
					}
					else if(field == 2)
					{/* Sender's sequence number */
						/* Do nothing */
					}
					else if(field == 3)
					{/* Signal strength */
						/* Do nothing */
					}
					else if(field == 4)
					{/* Local sequence number */
						/* Do nothing */
					}
					else if(field == 5)
					{/* Global timer */
						tmpTime = strtoull(value, NULL, 10);
					}
					else
					{
						printf("\nError at line #%llu of the input file!\nUnexpected field #%llu: %s\n\n", lineCounter, field, value);
						exit(EXIT_FAILURE);
					}

					value = strtok(NULL, " ");
				}

				if(tmpEncNode == currNodeB)
				{
					if(firstContact)
					{
						currContactStart = tmpTime;
						latestBeacon = tmpTime;
						firstContact = false;
					}
					else if(tmpTime - latestBeacon < 2 * GRANULARITY)
					{
						latestBeacon = tmpTime;
					}
					else
					{
						/* Write the previous encounter */
						fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", currNodeA, currNodeB, currContactStart, latestBeacon + GRANULARITY);


						/* Update the attributes */
						currContactStart = tmpTime;
						latestBeacon = tmpTime;
					}
				}
			}


			/* Write the last encounter */
			if(!firstContact)
			{
				fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", currNodeA, currNodeB, currContactStart, latestBeacon + GRANULARITY);
			}


			/* Close the input file */
			if(fclose(inFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}

			//printf("Progress: %.2f%%\r", 100.0 * (((double) pairsCounter) / ((double) (MAX_NODE_ID * MAX_NODE_ID))));
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
