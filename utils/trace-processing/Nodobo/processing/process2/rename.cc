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


const unsigned int maxLineSize = 1024;

const char *userID[NUMBER_OF_NODES] =
{
	"1A:60:76:BD:21:38",
	"2B:4E:E5:5B:72:03",
	"3D:D9:5B:DE:E0:D7",
	"04:4F:C5:ED:A8:05",
	"6C:7C:4E:D0:70:9C",
	"9A:7E:81:52:9B:58",
	"13:2A:CD:CD:A9:CC",
	"22:7E:E8:12:DF:40",
	"23:93:EE:CE:3D:F4",
	"32:48:6D:7D:4C:43",
	"39:17:15:39:4C:D9",
	"44:09:79:A3:BC:33",
	"50:4B:5C:AB:4A:AD",
	"59:CF:7B:7A:90:F3",
	"72:99:26:A7:11:5D",
	"75:C1:B0:ED:D4:D4",
	"76:5D:A5:9E:CB:96",
	"76:9D:81:F3:F3:75",
	"A5:77:5C:C5:8E:BB",
	"A7:CC:BA:E2:D0:66",
	"B3:3D:F2:C5:23:53",
	"C8:51:2A:05:66:B0",
	"C9:75:B4:AB:C0:11",
	"D3:53:38:8C:B1:1A",
	"D3:FA:22:5A:FD:5F",
	"E1:7A:37:18:91:17",
	"FE:47:2E:3F:D2:AC"
};



int main(int argc, char *argv[])
{
	int i;
	int j;
	int newUserID;
	unsigned long field;
	unsigned long lineCounter;
	unsigned long tmpTime;
	unsigned long minTime;
	char line[maxLineSize];
	char detectedUserID[maxLineSize];
	char inputFilename[maxLineSize];
	char outputFilename[maxLineSize];
	char *value;
	bool first;
	FILE *inFile;
	FILE *outFile;


	first = true;
	for(i = 0; i < NUMBER_OF_NODES; i++)
	{
		/* Create the input filename */
		sprintf(inputFilename, "%s.txt", userID[i]);


		/* Open the input file */
		if(!(inFile = fopen(inputFilename, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Create the output filename */
		sprintf(outputFilename, "../process3/contacts_%d.txt", i + 1);


		/* Open the output file */
		if(!(outFile = fopen(outputFilename, "w")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
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
				{/* Time */
					tmpTime = strtoul(value, NULL, 10);
				}
				else if(field == 2)
				{/* Detected User ID */
					strcpy(detectedUserID, value);
				}
				else
				{
					printf("\nError at line #%lu of the input file!\nUnexpected field #%lu: %s\n\n", lineCounter, field, value);
					exit(EXIT_FAILURE);
				}

				value = strtok(NULL, "\t");
			}


			/* Rename the user IDs */
			detectedUserID[17] = '\0';
			newUserID = 0;
			for(j = 0; j < NUMBER_OF_NODES; j++)
			{
				if(!strcmp(userID[j], detectedUserID))
				{
					if(i == j)
					{
						printf("Warning! User %s is in contact with himself (ignoring such contacts)!\n", userID[i]);
						break;
					}
					else
					{
						newUserID = j + 1;
						break;
					}
				}
			}


			/* Keep only the contacts between the known users */
			if(newUserID > 0)
			{
				fprintf(outFile, "%lu\t%d\n", tmpTime, newUserID);

				if(first)
				{
					minTime = tmpTime;
					first = false;
				}
				else if(tmpTime < minTime)
				{
					minTime = tmpTime;
				}
			}
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

	printf("Minimum Time = %lu\n", minTime);

	return EXIT_SUCCESS;
}
