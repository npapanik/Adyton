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
#include <ctype.h>

#define MAX_LINE_LENGTH 512


const unsigned long minID = 1;
const unsigned long maxStudentID = 36;
const unsigned long maxID = 54;
const char output1[] = "../process2/unsorted_Cambridge.txt";
const char output2[] = "../process2/unsorted_CambridgeStudents.txt";



int main(int argc, char *argv[])
{
	unsigned long field;
	unsigned long nodeA;
	unsigned long nodeB;
	unsigned long contactUp;
	unsigned long contactDown;
	unsigned long lineCounter;
	unsigned long startingTime;
	char line[MAX_LINE_LENGTH];
	char input[MAX_LINE_LENGTH];
	char *word;
	bool firstEntry;
	FILE *inputFile;
	FILE *outputFile1;
	FILE *outputFile2;


	/* Compute the starting time */
	firstEntry = true;
	for(nodeA = minID; nodeA <= maxID; nodeA++)
	{
		if(nodeA <= 36)
		{
			sprintf(input, "../../original/imote-traces-cambridge/SR-10mins-Students/%lu.dat", nodeA);
		}
		else if(nodeA <= 40)
		{
			sprintf(input, "../../original/imote-traces-cambridge/LR-2mins/%lu.dat", nodeA);
		}
		else if(nodeA <= 52)
		{
			sprintf(input, "../../original/imote-traces-cambridge/SR-10mins-FixLocation/%lu.dat", nodeA);
		}
		else
		{
			sprintf(input, "../../original/imote-traces-cambridge/SR-6mins-FixLocation/%lu.dat", nodeA);
		}

		if(!(inputFile = fopen(input, "r")))
		{
			printf("\n[Error]: Couldn't open the file: %s\n\n", input);
			exit(EXIT_FAILURE);
		}

		lineCounter = 0;
		while(fgets(line, MAX_LINE_LENGTH, inputFile))
		{
			lineCounter++;

			if(line[0] == '\n' || line[0] == '#')
			{
				continue;
			}

			field = 0;
			word = strtok(line, " ");

			while(word)
			{
				field++;

				/* Sanity check */
				if(!(isdigit(word[0])))
				{
					printf("\n[Error]: Expected a non-negative integer but read %s in line #%lu\n\n", word, lineCounter);
					exit(EXIT_FAILURE);
				}


				/* Identify the type of the attribute */
				if(field == 1)
				{/* Encountered node ID */
					if(strtoul(word, NULL, 10) < minID || strtoul(word, NULL, 10) > maxID)
					{
						break;
					}
					else
					{
						nodeB = strtoul(word, NULL, 10);
					}
				}
				else if(field == 2)
				{/* Contact start time */
					contactUp = strtoul(word, NULL, 10);

					if(firstEntry)
					{
						startingTime = contactUp;
						firstEntry = false;
					}
					else if(contactUp < startingTime)
					{
						startingTime = contactUp;
					}
				}
				else if(field == 3)
				{/* Contact end time */
					contactDown = strtoul(word, NULL, 10);
				}
				else
				{
					printf("\n[Error]: Unexpected field #%lu in line #%lu\n\n", field, lineCounter);
					exit(EXIT_FAILURE);
				}

				word = strtok(NULL, " ");
			}
		}

		if(fclose(inputFile))
		{
			printf("\n[Error]: Couldn't close the file: %s\n\n", input);
			exit(EXIT_FAILURE);
		}
	}

	/* Open the output files */
	if(!(outputFile1 = fopen(output1, "w")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", output1);
		exit(EXIT_FAILURE);
	}

	if(!(outputFile2 = fopen(output2, "w")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", output2);
		exit(EXIT_FAILURE);
	}


	/* Calculate the corresponding simulation time */
	for(nodeA = minID; nodeA <= maxID; nodeA++)
	{
		if(nodeA <= 36)
		{
			sprintf(input, "../../original/imote-traces-cambridge/SR-10mins-Students/%lu.dat", nodeA);
		}
		else if(nodeA <= 40)
		{
			sprintf(input, "../../original/imote-traces-cambridge/LR-2mins/%lu.dat", nodeA);
		}
		else if(nodeA <= 52)
		{
			sprintf(input, "../../original/imote-traces-cambridge/SR-10mins-FixLocation/%lu.dat", nodeA);
		}
		else
		{
			sprintf(input, "../../original/imote-traces-cambridge/SR-6mins-FixLocation/%lu.dat", nodeA);
		}

		if(!(inputFile = fopen(input, "r")))
		{
			printf("\n[Error]: Couldn't open the file: %s\n\n", input);
			exit(EXIT_FAILURE);
		}

		lineCounter = 0;
		while(fgets(line, MAX_LINE_LENGTH, inputFile))
		{
			lineCounter++;

			if(line[0] == '\n' || line[0] == '#')
			{
				continue;
			}

			field = 0;
			word = strtok(line, " ");

			while(word)
			{
				field++;

				/* Sanity check */
				if(!(isdigit(word[0])))
				{
					printf("\n[Error]: Expected a non-negative integer but read %s in line #%lu\n\n", word, lineCounter);
					exit(EXIT_FAILURE);
				}


				/* Identify the type of the attribute */
				if(field == 1)
				{/* Encountered node ID */
					if(strtoul(word, NULL, 10) < minID || strtoul(word, NULL, 10) > maxID)
					{
						break;
					}
					else
					{
						nodeB = strtoul(word, NULL, 10);
					}
				}
				else if(field == 2)
				{/* Contact start time */
					contactUp = strtoul(word, NULL, 10);
				}
				else if(field == 3)
				{/* Contact end time */
					contactDown = strtoul(word, NULL, 10);

					fprintf(outputFile1, "%lu\t%lu\t%lu\t%lu\n", nodeA, nodeB, (contactUp - startingTime), (contactDown - startingTime));

					if((nodeA >= minID) && (nodeA <= maxStudentID) && (nodeB >= minID) && (nodeB <= maxStudentID))
					{
						fprintf(outputFile2, "%lu\t%lu\t%lu\t%lu\n", nodeA, nodeB, (contactUp - startingTime), (contactDown - startingTime));
					}
				}
				else
				{
					printf("\n[Error]: Unexpected field #%lu in line #%lu\n\n", field, lineCounter);
					exit(EXIT_FAILURE);
				}

				word = strtok(NULL, " ");
			}
		}

		if(fclose(inputFile))
		{
			printf("\n[Error]: Couldn't close the file: %s\n\n", input);
			exit(EXIT_FAILURE);
		}
	}


	/* Close the output files */
	if(fclose(outputFile1))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", output1);
		exit(EXIT_FAILURE);
	}

	if(fclose(outputFile2))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", output2);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
