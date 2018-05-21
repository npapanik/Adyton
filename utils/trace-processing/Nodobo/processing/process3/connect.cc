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

#define NUMBER_OF_NODES 27
#define GRANULARITY 60
#define MINIMUM_TIME 21750710


const unsigned int maxLineSize = 1024;



int main(int argc, char *argv[])
{
	int i;
	int j;
	int encounteredUserID;
	int prevContactStart;
	int prevTime;
	unsigned long field;
	unsigned long lineCounter;
	unsigned long tmpTime;
	char line[maxLineSize];
	char inputFilename[maxLineSize];
	char outputFilename[maxLineSize];
	char *value;
	bool first;
	FILE *inFile;
	FILE *outFile;


	for(i = 1; i <= NUMBER_OF_NODES; i++)
	{
		for(j = 1; j <= NUMBER_OF_NODES; j++)
		{
			/* Create the input filename */
			sprintf(inputFilename, "contacts_%d.txt", i);


			/* Open the input file */
			if(!(inFile = fopen(inputFilename, "r")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}


			/* Create the output filename */
			sprintf(outputFilename, "../process4/encounters_%d.txt", i);


			/* Open the output file */
			if(!(outFile = fopen(outputFilename, "a")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
				exit(EXIT_FAILURE);
			}


			/* Read the input file line by line */
			lineCounter = 0;
			first = true;
			while(fgets(line, maxLineSize, inFile))
			{
				lineCounter++;

				field = 0;
				value = strtok(line, "\t");
				while(value)
				{
					field++;

					if(field == 1)
					{/* Time */
						tmpTime = strtoul(value, NULL, 10);
					}
					else if(field == 2)
					{/* Encountered User ID */
						encounteredUserID = atoi(value);
					}
					else
					{
						printf("\nError at line #%lu of the input file!\nUnexpected field #%lu: %s\n\n", lineCounter, field, value);
						exit(EXIT_FAILURE);
					}

					value = strtok(NULL, "\t");
				}


				/* Sanity check */
				if(tmpTime - prevTime < 0)
				{
					printf("Error! Time is going backwards!\n");
					exit(EXIT_FAILURE);
				}


				if(encounteredUserID == j)
				{
					if(first)
					{
						prevContactStart = tmpTime;
						prevTime = tmpTime;
						first = false;
					}
					else if(tmpTime - prevTime < 2 * GRANULARITY)
					{
						prevTime = tmpTime;
					}
					else
					{
						/* Write the previous encounter */
						fprintf(outFile, "%d\t%d\t%d\n", j, prevContactStart - MINIMUM_TIME, (prevTime - MINIMUM_TIME) + GRANULARITY);


						/* Update the attributes */
						prevContactStart = tmpTime;
						prevTime = tmpTime;
					}
				}
			}


			/* Write the last encounter */
			if(!first)
			{
				fprintf(outFile, "%d\t%d\t%d\n", j, prevContactStart - MINIMUM_TIME, (prevTime - MINIMUM_TIME) + GRANULARITY);
			}


			/* Close the output file */
			if(fclose(outFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
				exit(EXIT_FAILURE);
			}


			/* Close the input file */
			if(fclose(inFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}
		}
	}
	

	return EXIT_SUCCESS;
}
