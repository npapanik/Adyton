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
#include <list>

#define MAX_LINE_SIZE 512

using namespace std;


const char *inputFilename = "../../original/primaryschool.csv";
const char *outputFilename = "../process2/normalizedLyon.txt";



int main(int argc, char *argv[])
{
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int tmpContactInterval;
	unsigned long long int tmpNodeA;
	unsigned long long int tmpNodeB;
	unsigned long long int minSimulationTime;
	unsigned long long int normNodeA;
	unsigned long long int normNodeB;
	char line[MAX_LINE_SIZE];
	char *value;
	bool firstEntry;
	bool foundA;
	bool foundB;
	list<unsigned long long int> originalNodeIDs;
	list<unsigned long long int>::iterator it;
	FILE *inFile;
	FILE *outFile;


	/* Open the input file */
	if(!(inFile = fopen(inputFilename, "r")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Read the file line by line */
	lineCounter = 0;
	firstEntry = true;
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
			{/* Contact interval [t - 20, t] in seconds */
				tmpContactInterval = strtoull(value, NULL, 10);
			}
			else if(field == 2)
			{/* Node A */
				tmpNodeA = strtoull(value, NULL, 10);
			}
			else if(field == 3)
			{/* Node B */
				tmpNodeB = strtoull(value, NULL, 10);
			}
			else if(field == 4)
			{/* Class of node A */
				/* Do nothing */
			}
			else if(field == 5)
			{/* Class of node B */
				/* Do nothing */
			}
			else
			{
				printf("\nError at line #%llu of the input file!\nUnexpected field #%llu: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}


		/* Calculate the minimum simulation time */
		if(firstEntry)
		{
			minSimulationTime = tmpContactInterval - 20;
			firstEntry = false;
		}
		else
		{
			if(tmpContactInterval - 20 < minSimulationTime)
			{
				minSimulationTime = tmpContactInterval - 20;
			}
		}


		/* Collect all node IDs */
		foundA = false;
		foundB = false;
		for(it = originalNodeIDs.begin(); it != originalNodeIDs.end(); ++it)
		{
			if((*it) == tmpNodeA)
			{
				foundA = true;
			}

			if((*it) == tmpNodeB)
			{
				foundB = true;
			}

			if(foundA && foundB)
			{
				break;
			}
		}

		if(!foundA)
		{
			originalNodeIDs.push_back(tmpNodeA);
		}

		if(!foundB)
		{
			originalNodeIDs.push_back(tmpNodeB);
		}
	}


	/* Close the input file */
	if(fclose(inFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


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
			{/* Contact interval [t - 20, t] in seconds */
				tmpContactInterval = strtoull(value, NULL, 10);
			}
			else if(field == 2)
			{/* Node A */
				tmpNodeA = strtoull(value, NULL, 10);

				normNodeA = 0;
				for(it = originalNodeIDs.begin(); it != originalNodeIDs.end(); ++it)
				{
					if((*it) == tmpNodeA)
					{
						break;
					}
					else
					{
						normNodeA++;
					}
				}
			}
			else if(field == 3)
			{/* Node B */
				tmpNodeB = strtoull(value, NULL, 10);

				normNodeB = 0;
				for(it = originalNodeIDs.begin(); it != originalNodeIDs.end(); ++it)
				{
					if((*it) == tmpNodeB)
					{
						break;
					}
					else
					{
						normNodeB++;
					}
				}
			}
			else if(field == 4)
			{/* Class of node A */
				/* Do nothing */
			}
			else if(field == 5)
			{/* Class of node B */
				/* Do nothing */
			}
			else
			{
				printf("\nError at line #%llu of the input file!\nUnexpected field #%llu: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}

		fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", normNodeA, normNodeB, ((tmpContactInterval - 20) - minSimulationTime), (tmpContactInterval - minSimulationTime));
	}


	/* Close the input file */
	if(fclose(inFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Close the output file */
	if(fclose(outFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}

	originalNodeIDs.clear();

	return EXIT_SUCCESS;
}
