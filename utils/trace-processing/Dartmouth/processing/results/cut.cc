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

#define MAX_LINE_LENGTH 512


const char input[] = "Dartmouth.txt";
const char output[] = "Dartmouth2weeks.txt";

const unsigned long long int minValidTime = 8467200;
const unsigned long long int maxValidTime = 9676800;



int main(int argc, char *argv[])
{
	unsigned long long int nodeA;
	unsigned long long int nodeB;
	unsigned long long int contactUp;
	unsigned long long int contactDown;
	unsigned long long int field;
	char line[MAX_LINE_LENGTH];
	char *word;
	FILE *inFile;
	FILE *outFile;


	if(!(inFile = fopen(input, "r")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}

	if(!(outFile = fopen(output, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", output);
		exit(EXIT_FAILURE);
	}

	while(fgets(line, MAX_LINE_LENGTH, inFile))
	{
		if(line[0] == '\n' || line[0] == '#')
		{
			continue;
		}

		field = 0;
		word = strtok(line, "\t");
		while(word)
		{
			field++;

			if(field == 1)
			{
				nodeA = strtoull(word, NULL, 10);
			}
			else if(field == 2)
			{
				nodeB = strtoull(word, NULL, 10);
			}
			else if(field == 3)
			{
				contactUp = strtoull(word, NULL, 10);
			}
			else if(field == 4)
			{
				contactDown = strtoull(word, NULL, 10);

				if(contactUp >= minValidTime && contactDown <= maxValidTime)
				{
					fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", nodeA, nodeB, (contactUp - minValidTime), (contactDown - minValidTime));
				}
			}
			else
			{
				printf("\nError! Unknown field: %llu\n\n", field);
				exit(EXIT_FAILURE);
			}

			word = strtok(NULL, "\t");
		}
	}

	if(fclose(inFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", input);
		exit(EXIT_FAILURE);
	}

	if(fclose(outFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", output);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
