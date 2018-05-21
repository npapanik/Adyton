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

#define MINUTES_TO_SECONDS 60
#define HOURS_TO_SECONDS 3600
#define DAYS_TO_SECONDS 86400


const char *inputFilename = "../../original/wtd_data_release/wtd.csv";
const unsigned int maxLineSize = 128;

const unsigned int numDaysInMonth[12] =
{
	31,	/* January */
	28,	/* February */
	31,	/* March */
	30,	/* April */
	31,	/* May */
	30,	/* June */
	31,	/* July */
	31,	/* August */
	30,	/* September */
	31,	/* October */
	30,	/* November */
	31	/* December */
};



int main(int argc, char *argv[])
{
	unsigned long i;
	unsigned long field;
	unsigned long lineCounter;
	unsigned long tmpUserID;
	unsigned long tmpAccessPointID;
	unsigned long tmpMonth;
	unsigned long tmpDay;
	unsigned long tmpHour;
	unsigned long tmpMinute;
	unsigned long tmpSecond;
	unsigned long currentTime;
	unsigned long startingTime;
	unsigned long simulationTime;
	char line[maxLineSize];
	char date[maxLineSize];
	char time[maxLineSize];
	char outputFilename[maxLineSize];
	char *value;
	bool firstEntry;
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
	while(fgets(line, maxLineSize, inFile))
	{
		lineCounter++;

		if(line[0] == '\n' || lineCounter == 1)
		{
			continue;
		}

		field = 0;
		value = strtok(line, ",");
		while(value)
		{
			field++;

			if(field == 1)
			{/* User ID */
				tmpUserID = strtoul(value, NULL, 10);
			}
			else if(field == 2)
			{/* Month-Day */
				strcpy(date, value);
			}
			else if(field == 3)
			{/* HH:MM:SS */
				strcpy(time, value);
			}
			else if(field == 4)
			{/* Access point ID */
				tmpAccessPointID = strtoul(value, NULL, 10);
			}
			else if(field == 5)
			{/* Signal strength */
				/* Do nothing */
			}
			else if(field == 6)
			{/* AC power (1) or battery (0) */
				/* Do nothing */
			}
			else if(field == 7)
			{/* Associated with the access point (1) or not (0) */
				/* Do nothing */
			}
			else
			{
				printf("\nError at line #%lu of the input file!\nUnexpected field #%lu: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, ",");
		}

		field = 0;
		value = strtok(date, "-");
		while(value)
		{
			field++;

			if(field == 1)
			{/* Month */
				tmpMonth = strtoul(value, NULL, 10);
			}
			else if(field == 2)
			{/* Day */
				tmpDay = strtoul(value, NULL, 10);
			}
			else
			{
				printf("\nError at line #%lu of the input file!\nUnexpected attribute #%lu at field #2: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, "-");
		}

		field = 0;
		value = strtok(time, ":");
		while(value)
		{
			field++;

			if(field == 1)
			{/* HH */
				tmpHour = strtoul(value, NULL, 10);
			}
			else if(field == 2)
			{/* MM */
				tmpMinute = strtoul(value, NULL, 10);
			}
			else if(field == 3)
			{/* SS */
				tmpSecond = strtoul(value, NULL, 10);
			}
			else
			{
				printf("\nError at line #%lu of the input file!\nUnexpected attribute #%lu at field #3: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, ":");
		}


		/* Calculate the actual time in seconds from January 1st */
		currentTime = 0;
		currentTime += tmpSecond;
		currentTime += MINUTES_TO_SECONDS * tmpMinute;
		currentTime += HOURS_TO_SECONDS * tmpHour;
		for(i = 1; i < tmpMonth; i++)
		{
			currentTime += DAYS_TO_SECONDS * numDaysInMonth[i];
		}
		currentTime += DAYS_TO_SECONDS * tmpDay;


		/* Set the start of the simulation time */
		if(firstEntry)
		{
			startingTime = currentTime;
			firstEntry = false;
		}


		/* Calculate the corresponding simulation time */
		if(currentTime >= startingTime)
		{
			simulationTime = currentTime - startingTime;
		}
		else
		{
			printf("\nError! Time is going backwards!\n\n");
			exit(EXIT_FAILURE);
		}


		/* Create the output filename */
		sprintf(outputFilename, "../process2/user_%lu.txt", tmpUserID);


		/* Open the output file */
		if(!(outFile = fopen(outputFilename, "a")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
			exit(EXIT_FAILURE);
		}


		/* Write one entry at a time */
		fprintf(outFile, "%lu\t%lu\n", simulationTime, tmpAccessPointID);


		/* Close the output file */
		if(fclose(outFile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
			exit(EXIT_FAILURE);
		}
	}


	/* Close the input file */
	if(fclose(inFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
