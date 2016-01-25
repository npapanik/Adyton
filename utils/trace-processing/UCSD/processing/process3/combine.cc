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


const unsigned int maxLineSize = 128;
const unsigned int minID = 1;
const unsigned int maxID = 275;



int main(int argc, char *argv[])
{
	unsigned long i;
	unsigned long field;
	unsigned long lineCounter;
	unsigned long prevSimulationTime;
	unsigned long currSimulationTime;
	unsigned long tmpAccessPointID;
	char line[maxLineSize];
	char inputFilename[maxLineSize];
	char outputFilename[maxLineSize];
	char *value;
	bool firstEntry;
	FILE *inFile;
	FILE *outFile;


	for(i = minID; i <= maxID; i++)
	{
		/* Create the input filename */
		sprintf(inputFilename, "sorted_%lu.txt", i);


		/* Create the output filename */
		sprintf(outputFilename, "../process4/node_%lu.txt", i);


		/* Open the input file */
		if(!(inFile = fopen(inputFilename, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Open the output file */
		if(!(outFile = fopen(outputFilename, "w")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
			exit(EXIT_FAILURE);
		}


		/* Read the file line by line */
		lineCounter = 0;
		firstEntry = true;
		while(fgets(line, maxLineSize, inFile))
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
				{/* Simulation time */
					currSimulationTime = strtoul(value, NULL, 10);
				}
				else if(field == 2)
				{/* Access point ID */
					tmpAccessPointID = strtoul(value, NULL, 10);
				}
				else
				{
					printf("\nError at line #%lu of the input file!\nUnexpected field #%lu: %s\n\n", lineCounter, field, value);
					exit(EXIT_FAILURE);
				}

				value = strtok(NULL, "\t");
			}

			if(firstEntry)
			{
				fprintf(outFile, "%lu\t%lu", currSimulationTime, tmpAccessPointID);
				firstEntry = false;
			}
			else
			{
				if(currSimulationTime == prevSimulationTime)
				{
					fprintf(outFile, ",%lu", tmpAccessPointID);
				}
				else if(currSimulationTime > prevSimulationTime)
				{
					fprintf(outFile, "\n%lu\t%lu", currSimulationTime, tmpAccessPointID);
				}
				else
				{
					printf("\nError! Time is going backwards!\n\n");
					exit(EXIT_FAILURE);
				}
			}

			prevSimulationTime = currSimulationTime;
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
	}

	return EXIT_SUCCESS;
}
