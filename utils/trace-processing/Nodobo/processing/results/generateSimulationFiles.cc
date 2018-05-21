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



int main(int argc, char *argv[])
{
	unsigned long long int i;
	unsigned long long int field;
	unsigned long long int nodeA;
	unsigned long long int nodeB;
	unsigned long long int contactUp;
	unsigned long long int contactDown;
	unsigned long long int lineCounter;
	unsigned long long int largestID;
	unsigned long long int numContacts;
	unsigned long long int simDuration;
	unsigned long long int numActiveNodes;
	unsigned long long int numInactiveNodes;
	unsigned long long int warningSameNode;
	unsigned long long int warningZeroDuration;
	unsigned long long int warningTimeBackwards;
	unsigned long long int *firstAppearance;
	unsigned long long int *lastAppearance;
	char line[MAX_LINE_LENGTH];
	char input[MAX_LINE_LENGTH];
	char dataSet[MAX_LINE_LENGTH];
	char output[MAX_LINE_LENGTH];
	char *value;
	bool *active;
	FILE *filePointer;


	/* Sanity check */
	if(argc != 3)
	{
		printf("\n[Usage]: %s <input filename> <Name of the data set>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	/* Get the input arguments */
	strcpy(input, argv[1]);
	strcpy(dataSet, argv[2]);


	/* Compute some basic characteristics */
	if(!(filePointer = fopen(input, "r")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}

	largestID = 0;
	numContacts = 0;
	simDuration = 0;
	lineCounter = 0;
	warningSameNode = 0;
	warningZeroDuration = 0;
	warningTimeBackwards = 0;
	while(fgets(line, MAX_LINE_LENGTH, filePointer))
	{
		lineCounter++;

		if(line[0] == '\n' || line[0] == '#')
		{
			continue;
		}

		numContacts++;

		field = 0;
		value = strtok(line, "\t");
		while(value)
		{
			field++;

			/* Sanity check */
			if(!(isdigit(value[0])))
			{
				printf("\n[Error]: Expected a non-negative integer but read %s in line #%llu\n\n", value, lineCounter);
				exit(EXIT_FAILURE);
			}


			/* Identify the type of the attribute */
			if(field == 1)
			{/* Node A ID */
				nodeA = strtoull(value, NULL, 10);

				if(nodeA > largestID)
				{
					largestID = nodeA;
				}
			}
			else if(field == 2)
			{/* Node B ID */
				nodeB = strtoull(value, NULL, 10);

				if(nodeB > largestID)
				{
					largestID = nodeB;
				}

				if(nodeA == nodeB)
				{
					//printf("[Warning]: Contact between two nodes with the same ID (%llu, %llu) in line #%llu\n", nodeA, nodeB, lineCounter);
					warningSameNode++;
				}
			}
			else if(field == 3)
			{/* Contact start time */
				contactUp = strtoull(value, NULL, 10);
			}
			else if(field == 4)
			{/* Contact end time */
				contactDown = strtoull(value, NULL, 10);

				if(contactDown < contactUp)
				{
					//printf("[Warning]: Time is going backwards in line #%llu\n", lineCounter);
					warningTimeBackwards++;
				}
				else if(contactDown == contactUp)
				{
					//printf("[Warning]: Contact of zero duration in line #%llu\n", lineCounter);
					warningZeroDuration++;
				}

				if(contactDown > simDuration)
				{
					simDuration = contactDown;
				}
			}
			else
			{
				printf("\n[Error]: Unexpected field #%llu in line #%llu\n\n", field, lineCounter);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}
	}

	if(fclose(filePointer))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}


	/* Allocate memory for further analysis */
	if((active = (bool *) malloc((largestID + 1) * sizeof(bool))) == NULL)
	{
		printf("\n[Error]: Unable to allocate memory for the Boolean array of active nodes\n\n");
		exit(EXIT_FAILURE);
	}

	if((firstAppearance = (unsigned long long int *) malloc((largestID + 1) * sizeof(unsigned long long int))) == NULL)
	{
		printf("\n[Error]: Unable to allocate memory for the array of first appearances\n\n");
		exit(EXIT_FAILURE);
	}

	if((lastAppearance = (unsigned long long int *) malloc((largestID + 1) * sizeof(unsigned long long int))) == NULL)
	{
		printf("\n[Error]: Unable to allocate memory for the array of last appearances\n\n");
		exit(EXIT_FAILURE);
	}


	/* Distinguish active and inactive nodes */
	if(!(filePointer = fopen(input, "r")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}

	for(i = 0; i <= largestID; i++)
	{
		active[i] = false;
		firstAppearance[i] = simDuration;
		lastAppearance[i] = 0;
	}

	lineCounter = 0;
	while(fgets(line, MAX_LINE_LENGTH, filePointer))
	{
		lineCounter++;

		if(line[0] == '\n' || line[0] == '#')
		{
			continue;
		}

		field = 0;
		value = strtok(line, "\t");
		while(value)
		{
			field++;

			/* Sanity check */
			if(!(isdigit(value[0])))
			{
				printf("\n[Error]: Expected a non-negative integer but read %s in line #%llu\n\n", value, lineCounter);
				exit(EXIT_FAILURE);
			}


			/* Identify the type of the attribute */
			if(field == 1)
			{/* Node A ID */
				nodeA = strtoull(value, NULL, 10);
				active[nodeA] = true;
			}
			else if(field == 2)
			{/* Node B ID */
				nodeB = strtoull(value, NULL, 10);
				active[nodeB] = true;
			}
			else if(field == 3)
			{/* Contact start time */
				contactUp = strtoull(value, NULL, 10);

				if(contactUp < firstAppearance[nodeA])
				{
					firstAppearance[nodeA] = contactUp;
				}

				if(contactUp < firstAppearance[nodeB])
				{
					firstAppearance[nodeB] = contactUp;
				}
			}
			else if(field == 4)
			{/* Contact end time */
				contactDown = strtoull(value, NULL, 10);

				if(contactDown > lastAppearance[nodeA])
				{
					lastAppearance[nodeA] = contactDown;
				}

				if(contactDown > lastAppearance[nodeB])
				{
					lastAppearance[nodeB] = contactDown;
				}
			}
			else
			{
				printf("\n[Error]: Unexpected field #%llu in line #%llu\n\n", field, lineCounter);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "\t");
		}
	}

	if(fclose(filePointer))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}


	/* Calculate the number of active and inactive nodes */
	numActiveNodes = 0;
	numInactiveNodes = 0;
	for(i = 0; i <= largestID; i++)
	{
		if(active[i])
		{
			numActiveNodes++;
		}
		else
		{
			numInactiveNodes++;
		}
	}


	/* Write the characteristics of the data set */
	sprintf(output, "%s-characteristics.txt", dataSet);

	if(!(filePointer = fopen(output, "w")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", output);
		exit(EXIT_FAILURE);
	}

	fprintf(filePointer, "Largest Node ID:\t%llu\n", largestID);
	fprintf(filePointer, "Number of Active Nodes:\t%llu\n", numActiveNodes);
	fprintf(filePointer, "Number of Inactive Nodes:\t%llu\n", numInactiveNodes);
	fprintf(filePointer, "Total Duration:\t%llu\n", simDuration);
	fprintf(filePointer, "Number of Contacts:\t%llu\n", numContacts);
	fprintf(filePointer, "Number of Lines:\t%llu\n", lineCounter);

	if(fclose(filePointer))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", output);
		exit(EXIT_FAILURE);
	}


	/* Write the first and the last appearance of each node */
	sprintf(output, "%s-presence.txt", dataSet);

	if(!(filePointer = fopen(output, "w")))
	{
		printf("\n[Error]: Couldn't open the file: %s\n\n", output);
		exit(EXIT_FAILURE);
	}

	for(i = 0; i <= largestID; i++)
	{
		if(active[i])
		{
			fprintf(filePointer, "%llu\t%llu\t%llu\n", i, firstAppearance[i], lastAppearance[i]);
		}
		else
		{
			fprintf(filePointer, "%llu\t-\t-\n", i);
		}
	}

	if(fclose(filePointer))
	{
		printf("\n[Error]: Couldn't close the file: %s\n\n", output);
		exit(EXIT_FAILURE);
	}


	/* Print a summary of the warnings */
	if(warningSameNode > 0)
	{
		printf("[Warning]: In %llu contacts, the encountered nodes have the same ID!\n", warningSameNode);
	}

	if(warningZeroDuration > 0)
	{
		printf("[Warning]: In %llu contacts, the contact duration is equal to zero!\n", warningZeroDuration);
	}

	if(warningTimeBackwards > 0)
	{
		printf("[Warning]: In %llu contacts, the time is going backwards!\n", warningTimeBackwards);
	}

	free(active);
	free(firstAppearance);
	free(lastAppearance);

	return EXIT_SUCCESS;
}
