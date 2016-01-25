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

#define MINUTES_TO_SECONDS 60
#define HOURS_TO_SECONDS 3600
#define DAYS_TO_SECONDS 86400
#define START_TIME 21686400


const char *inputFilename = "../../original/csv/presences.csv";
const unsigned int maxLineSize = 1024;

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
	int i;
	int tmpYear;
	int tmpMonth;
	int tmpDay;
	int tmpHours;
	int tmpMinutes;
	int tmpSeconds;
	unsigned long field;
	unsigned long lineCounter;
	unsigned long currentTime;
	char line[maxLineSize];
	char date[maxLineSize];
	char time[maxLineSize];
	char scanningUserID[maxLineSize];
	char detectedUserID[maxLineSize];
	char outputFilename[maxLineSize];
	char *value;
	bool invalidLine;
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
	while(fgets(line, maxLineSize, inFile))
	{
		invalidLine = false;
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
			{/* Scanning User ID */
				strcpy(scanningUserID, value);
			}
			else if(field == 2)
			{/* Detected User ID */
				strcpy(detectedUserID, value);
			}
			else if(field == 3)
			{/* Name of the device */
				/* Do nothing */
			}
			else if(field == 4)
			{/* Reported class of the device */
				/* Do nothing */
			}
			else if(field == 5)
			{/* Timestamp (format: Wed Sep 15 19:01:01 +0100 2010) */
				strcpy(date, value);
			}
			else
			{
				printf("\nError at line #%lu of the input file!\nUnexpected field #%lu: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, ",");
		}

		field = 0;
		value = strtok(date, " ");
		while(value)
		{
			field++;

			if(field == 1)
			{/* Day */
				if(strcmp(value, "Mon") && strcmp(value, "Tue") && strcmp(value, "Wed") && strcmp(value, "Thu") && strcmp(value, "Fri") && strcmp(value, "Sat") && strcmp(value, "Sun"))
				{
					printf("Unknown day = %s\n", value);
					exit(EXIT_FAILURE);
				}
				else
				{
					/* Do nothing */
				}
			}
			else if(field == 2)
			{/* Month */
				if(strcmp(value, "Jan") && strcmp(value, "Feb") && strcmp(value, "Mar") && strcmp(value, "Apr") && strcmp(value, "May") && strcmp(value, "Jun") && strcmp(value, "Jul") && strcmp(value, "Aug") && strcmp(value, "Sep") && strcmp(value, "Oct") && strcmp(value, "Nov") && strcmp(value, "Dec"))
				{
					printf("Unknown month = %s\n", value);
					exit(EXIT_FAILURE);
				}
				else
				{
					if(!strcmp(value, "Jan"))
					{
						tmpMonth = 1;
					}
					else if(!strcmp(value, "Feb"))
					{
						tmpMonth = 2;
					}
					else if(!strcmp(value, "Mar"))
					{
						tmpMonth = 3;
					}
					else if(!strcmp(value, "Apr"))
					{
						tmpMonth = 4;
					}
					else if(!strcmp(value, "May"))
					{
						tmpMonth = 5;
					}
					else if(!strcmp(value, "Jun"))
					{
						tmpMonth = 6;
					}
					else if(!strcmp(value, "Jul"))
					{
						tmpMonth = 7;
					}
					else if(!strcmp(value, "Aug"))
					{
						tmpMonth = 8;
					}
					else if(!strcmp(value, "Sep"))
					{
						tmpMonth = 9;
					}
					else if(!strcmp(value, "Oct"))
					{
						tmpMonth = 10;
					}
					else if(!strcmp(value, "Nov"))
					{
						tmpMonth = 11;
					}
					else if(!strcmp(value, "Dec"))
					{
						tmpMonth = 12;
					}
				}
			}
			else if(field == 3)
			{/* Day of the month */
				if(atoi(value) <= 0 || atoi(value) > 31)
				{
					printf("Invalid day of the month = %d\n", atoi(value));
					exit(EXIT_FAILURE);
				}
				else
				{
					tmpDay = atoi(value);
				}
			}
			else if(field == 4)
			{/* Time */
				strcpy(time, value);
			}
			else if(field == 5)
			{/* Timezone */
				if(strcmp(value, "+0000") && strcmp(value, "+0100"))
				{
					printf("Invalid timezone = %s\n", value);
					exit(EXIT_FAILURE);
				}
				else
				{
					/* Do nothing */
				}
			}
			else if(field == 6)
			{/* Year */
				if(atoi(value) != 2010 && atoi(value) != 2011)
				{
					//printf("Invalid year = %d\n", atoi(value));
					//exit(EXIT_FAILURE);
					invalidLine = true;
					break;
				}
				else
				{
					tmpYear = atoi(value);
				}
			}
			else
			{
				printf("\nError at line #%lu of the input file!\nUnexpected attribute #%lu at field #5: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, " ");
		}


		if(invalidLine)
		{
			continue;
		}


		field = 0;
		value = strtok(time, ":");
		while(value)
		{
			field++;

			if(field == 1)
			{/* HH */
				if(atoi(value) < 0 || atoi(value) > 23)
				{
					printf("Invalid number of hours = %d\n", atoi(value));
					exit(EXIT_FAILURE);
				}
				else
				{
					tmpHours = atoi(value);
				}
			}
			else if(field == 2)
			{/* MM */
				if(atoi(value) < 0 || atoi(value) > 59)
				{
					printf("Invalid number of minutes = %d\n", atoi(value));
					exit(EXIT_FAILURE);
				}
				else
				{
					tmpMinutes = atoi(value);
				}
			}
			else if(field == 3)
			{/* SS */
				if(atoi(value) < 0 || atoi(value) > 59)
				{
					printf("Invalid number of seconds = %d\n", atoi(value));
					exit(EXIT_FAILURE);
				}
				else
				{
					tmpSeconds = atoi(value);
				}
			}
			else
			{
				printf("\nError at line #%lu of the input file!\nUnexpected attribute #%lu at field #4: %s\n\n", lineCounter, field, value);
				exit(EXIT_FAILURE);
			}

			value = strtok(NULL, ":");
		}


		/* Calculate the actual time in seconds from January 1st 2010 */
		currentTime = 0;
		if(tmpYear == 2011)
		{
			for(i = 1; i <= 12; i++)
			{
				currentTime += DAYS_TO_SECONDS * numDaysInMonth[i - 1];
			}
		}
		for(i = 1; i < tmpMonth; i++)
		{
			currentTime += DAYS_TO_SECONDS * numDaysInMonth[i - 1];
		}
		currentTime += DAYS_TO_SECONDS * (tmpDay - 1);
		currentTime += HOURS_TO_SECONDS * tmpHours;
		currentTime += MINUTES_TO_SECONDS * tmpMinutes;
		currentTime += tmpSeconds;


		/* The experiment started in 2010-09-09 */
		if(currentTime >= START_TIME)
		{
			/* Create the output filename */
			sprintf(outputFilename, "../process2/%s.txt", scanningUserID);


			/* Open the output file */
			if(!(outFile = fopen(outputFilename, "a")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
				exit(EXIT_FAILURE);
			}


			/* Write one entry at a time */
			fprintf(outFile, "%lu\t%s\n", currentTime, detectedUserID);


			/* Close the output file */
			if(fclose(outFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
				exit(EXIT_FAILURE);
			}
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
