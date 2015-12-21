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

#define MAX_LINE_SIZE 512

using namespace std;


const char *inputFilename = "normalizedLyon.txt";
const char *outputFilename = "../process3/unsortedLyon.txt";

const unsigned long long int minID = 0;
const unsigned long long int maxID = 241;



int main(int argc, char *argv[])
{
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int pairsCounter;
	unsigned long long int tmpNodeA;
	unsigned long long int tmpNodeB;
	unsigned long long int tmpContactUp;
	unsigned long long int tmpContactDown;
	unsigned long long int currNodeA;
	unsigned long long int currNodeB;
	unsigned long long int currContactUp;
	unsigned long long int currContactDown;
	char line[MAX_LINE_SIZE];
	char *value;
	bool firstContact;
	list<unsigned long long int> originalNodeIDs;
	list<unsigned long long int>::iterator it;
	FILE *inFile;
	FILE *outFile;


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Combine the concurrent contacts */
	pairsCounter = 0;
	for(currNodeA = minID; currNodeA <= maxID; currNodeA++)
	{
		for(currNodeB = minID; currNodeB <= maxID; currNodeB++)
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


			/* Read the file line by line */
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
				value = strtok(line, "\t");
				while(value)
				{
					field++;

					if(field == 1)
					{/* Node A */
						tmpNodeA = strtoull(value, NULL, 10);
					}
					else if(field == 2)
					{/* Node B */
						tmpNodeB = strtoull(value, NULL, 10);
					}
					else if(field == 3)
					{/* Contact Up */
						tmpContactUp = strtoull(value, NULL, 10);
					}
					else if(field == 4)
					{/* Contact Down */
						tmpContactDown = strtoull(value, NULL, 10);
					}
					else
					{
						printf("\nError at line #%llu of the input file!\nUnexpected field #%llu: %s\n\n", lineCounter, field, value);
						exit(EXIT_FAILURE);
					}

					value = strtok(NULL, "\t");
				}


				/* Examine each pair of nodes independently */
				if((tmpNodeA == currNodeA) && (tmpNodeB == currNodeB))
				{
					if(firstContact)
					{
						currContactUp = tmpContactUp;
						currContactDown = tmpContactDown;
						firstContact = false;
					}
					else
					{
						if(currContactDown == tmpContactUp)
						{
							currContactDown = tmpContactDown;
						}
						else
						{
							fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", currNodeA, currNodeB, currContactUp, currContactDown);
							currContactUp = tmpContactUp;
							currContactDown = tmpContactDown;
						}
					}
				}
			}

			if(!firstContact)
			{
				fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", currNodeA, currNodeB, currContactUp, currContactDown);
			}


			/* Close the input file */
			if(fclose(inFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}

			//printf("Progress: %.2f%%\r", 100.0 * (((double) pairsCounter) / ((double) ((maxID + 1) * (maxID + 1)))));
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
