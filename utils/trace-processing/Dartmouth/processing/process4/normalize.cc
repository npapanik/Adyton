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

#define MAX_LINE_SIZE 512


using namespace std;


struct PairIDs
{
	unsigned long long int originalNodeID;
	unsigned long long int normalizedNodeID;
};



int main(int argc, char *argv[])
{
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int tmpNodeA;
	unsigned long long int tmpNodeB;
	unsigned long long int tmpContactUp;
	unsigned long long int tmpContactDown;
	unsigned long long int numActiveNodes;
	unsigned long long int normNodeA;
	unsigned long long int normNodeB;
	char line[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char outputFilename[MAX_LINE_SIZE];
	char *value;
	bool foundA;
	bool foundB;
	PairIDs tmpPairIDs;
	list<PairIDs> uniqueNodeIDs;
	list<PairIDs>::iterator it;
	FILE *inFile;
	FILE *outFile;


	/* Create the output filename */
	sprintf(outputFilename, "../process5/unsortedDartmouth.txt");


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Create the input filename */
	sprintf(inputFilename, "tmpDartmouth.txt");


	/* Open the input file */
	if(!(inFile = fopen(inputFilename, "r")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Read the file line by line */
	numActiveNodes = 0;
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
			{
				tmpNodeA = strtoull(value, NULL, 10);
			}
			else if(field == 2)
			{
				tmpNodeB = strtoull(value, NULL, 10);
			}
			else if(field == 3)
			{
				tmpContactUp = strtoull(value, NULL, 10);
			}
			else if(field == 4)
			{
				tmpContactDown = strtoull(value, NULL, 10);
			}
			else
			{
				printf("\nError at line #%llu of the input file %s\nUnexpected field #%llu: %s\n\n", lineCounter, inputFilename, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}


		/* Normalize the IDs of the active nodes */
		foundA = false;
		foundB = false;
		for(it = uniqueNodeIDs.begin(); it != uniqueNodeIDs.end(); ++it)
		{
			if((*it).originalNodeID == tmpNodeA)
			{
				foundA = true;
				normNodeA = (*it).normalizedNodeID;
			}

			if((*it).originalNodeID == tmpNodeB)
			{
				foundB = true;
				normNodeB = (*it).normalizedNodeID;
			}

			if(foundA && foundB)
			{
				break;
			}
		}

		if(!foundA)
		{
			normNodeA = numActiveNodes;

			tmpPairIDs.originalNodeID = tmpNodeA;
			tmpPairIDs.normalizedNodeID = normNodeA;
			uniqueNodeIDs.push_back(tmpPairIDs);

			numActiveNodes++;
		}

		if(!foundB)
		{
			normNodeB = numActiveNodes;

			tmpPairIDs.originalNodeID = tmpNodeB;
			tmpPairIDs.normalizedNodeID = normNodeB;
			uniqueNodeIDs.push_back(tmpPairIDs);

			numActiveNodes++;
		}

		fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", normNodeA, normNodeB, tmpContactUp, tmpContactDown);
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

	uniqueNodeIDs.clear();

	return EXIT_SUCCESS;
}
