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

#define NUMBER_OF_NODES 27


const unsigned int maxLineSize = 1024;
const char *outputFilename = "../process5/Nodobo_unsorted.txt";



int main(int argc, char *argv[])
{
	int i;
	int encounteredUserID;
	unsigned long field;
	unsigned long lineCounter;
	unsigned long contactUp;
	unsigned long contactDown;
	char line[maxLineSize];
	char inputFilename[maxLineSize];
	char *value;
	FILE *inFile;
	FILE *outFile;


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Select one file at a time */
	for(i = 1; i <= NUMBER_OF_NODES; i++)
	{
		/* Create the input filename */
		sprintf(inputFilename, "encounters_%d.txt", i);


		/* Open the input file */
		if(!(inFile = fopen(inputFilename, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Read the input file line by line */
		lineCounter = 0;
		while(fgets(line, maxLineSize, inFile))
		{
			lineCounter++;

			field = 0;
			value = strtok(line, "\t");
			while(value)
			{
				field++;

				if(field == 1)
				{/* Encountered User ID */
					encounteredUserID = atoi(value);
				}				
				else if(field == 2)
				{/* Contact-Up Time */
					contactUp = strtoul(value, NULL, 10);
				}
				else if(field == 3)
				{/* Contact-Down Time */
					contactDown = strtoul(value, NULL, 10);
				}
				else
				{
					printf("\nError at line #%lu of the input file!\nUnexpected field #%lu: %s\n\n", lineCounter, field, value);
					exit(EXIT_FAILURE);
				}

				value = strtok(NULL, "\t");
			}

			/* Write the encounter */
			fprintf(outFile, "%d\t%d\t%lu\t%lu\n", i, encounteredUserID, contactUp, contactDown);
		}


		/* Close the input file */
		if(fclose(inFile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
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
