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


const unsigned int minID = 0;
const unsigned int maxID = 535;

const unsigned int maxLineSize = 64;



int main(int argc, char *argv[])
{
	unsigned int i;
	unsigned int field;
	unsigned long long int minTime;
	bool firstTime;
	char line[maxLineSize];
	char inputFN[maxLineSize];
	char outputFN[maxLineSize];
	char *word;
	FILE *infile;
	FILE *outfile;


	firstTime = true;
	for(i = minID; i <= maxID; i++)
	{
		sprintf(inputFN, "taxi-%u.txt", i);

		if(!(infile = fopen(inputFN, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFN);
			exit(EXIT_FAILURE);
		}

		while(fgets(line, maxLineSize, infile))
		{
			if(line[0] == '\n')
			{
				continue;
			}

			field = 0;
			word = strtok(line, " ");
			while(word)
			{
				field++;

				if(field == 4)
				{
					if(firstTime)
					{
						minTime = strtoull(word, NULL, 10);
						firstTime = false;
					}
					else if(strtoull(word, NULL, 10) < minTime)
					{
						minTime = strtoull(word, NULL, 10);
					}
				}

				word = strtok(NULL, " ");
			}
		}

		if(fclose(infile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", inputFN);
			exit(EXIT_FAILURE);
		}
	}

	for(i = minID; i <= maxID; i++)
	{
		sprintf(inputFN, "taxi-%u.txt", i);
		sprintf(outputFN, "../process3/taxi-%u.txt", i);

		if(!(infile = fopen(inputFN, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFN);
			exit(EXIT_FAILURE);
		}

		if(!(outfile = fopen(outputFN, "w")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", outputFN);
			exit(EXIT_FAILURE);
		}

		while(fgets(line, maxLineSize, infile))
		{
			if(line[0] == '\n')
			{
				continue;
			}

			field = 0;
			word = strtok(line, " ");
			while(word)
			{
				field++;

				if(field == 1 || field == 2)
				{
					fprintf(outfile, "%s\t", word);
				}
				else if(field == 4)
				{
					fprintf(outfile, "%llu\n", strtoull(word, NULL, 10) - minTime);
				}

				word = strtok(NULL, " ");
			}
		}

		if(fclose(infile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", inputFN);
			exit(EXIT_FAILURE);
		}

		if(fclose(outfile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", outputFN);
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
