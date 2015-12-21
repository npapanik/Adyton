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
#include <math.h>
#include <list>

#define MAX_LINE_SIZE 512

#define MIN_DAY 30
#define MIN_MONTH 1
#define MAX_DAY 3
#define MAX_MONTH 6

using namespace std;


const char *inputPath = "../../original/DieselNetTracesSpring2006/Bus-Bus/";

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


struct PairIDs
{
	unsigned long long int originalNodeID;
	unsigned long long int normalizedNodeID;
};



int main(int argc, char *argv[])
{
	unsigned int currMonth;
	unsigned int currDay;
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int tmpNodeA;
	unsigned long long int tmpNodeB;
	unsigned long long int tmpDuration;
	unsigned long long int tmpMinutes;
	unsigned long long int tmpSeconds;
	unsigned long long int minSimulationTime;
	unsigned long long int elapsedSeconds;
	unsigned long long int elapsedDays;
	unsigned long long int numNodes;
	unsigned long long int normNodeA;
	unsigned long long int normNodeB;
	char line[MAX_LINE_SIZE];
	char expectedValue[MAX_LINE_SIZE];
	char tmpTime[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char outputFilename[MAX_LINE_SIZE];
	char *value;
	bool foundA;
	bool foundB;
	bool firstEntry;
	bool expectedEntry;
	PairIDs tmpPairIDs;
	list<PairIDs> uniqueNodeIDs;
	list<PairIDs>::iterator it;
	FILE *inFile;
	FILE *outFile;


	/* Initializations */
	currDay = MIN_DAY;
	currMonth = MIN_MONTH;
	elapsedDays = 0;
	numNodes = 0;
	firstEntry = true;


	/* Create the output filename */
	sprintf(outputFilename, "../process2/unsortedDieselNet.txt");


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	while((currDay != MAX_DAY) || (currMonth != MAX_MONTH))
	{
		/* Create the input filename */
		if(currDay < 10)
		{
			sprintf(inputFilename, "%s%u0%u2006", inputPath, currMonth, currDay);
		}
		else
		{
			sprintf(inputFilename, "%s%u%u2006", inputPath, currMonth, currDay);
		}

		if((currDay == 4 && currMonth == 2) || (currDay == 5 && currMonth == 2) || (currDay == 11 && currMonth == 2) || (currDay == 12 && currMonth == 2) || (currDay == 18 && currMonth == 2) || (currDay == 19 && currMonth == 2) || (currDay == 20 && currMonth == 2) || (currDay == 24 && currMonth == 2) || (currDay == 26 && currMonth == 2) || (currDay == 4 && currMonth == 3) || (currDay == 5 && currMonth == 3) || (currDay == 12 && currMonth == 3) || (currDay == 18 && currMonth == 3) || (currDay == 19 && currMonth == 3) || (currDay == 20 && currMonth == 3) || (currDay == 21 && currMonth == 3) || (currDay == 22 && currMonth == 3) || (currDay == 23 && currMonth == 3) || (currDay == 24 && currMonth == 3) || (currDay == 25 && currMonth == 3) || (currDay == 26 && currMonth == 3) || (currDay == 1 && currMonth == 4) || (currDay == 8 && currMonth == 4) || (currDay == 9 && currMonth == 4) || (currDay == 15 && currMonth == 4) || (currDay == 16 && currMonth == 4) || (currDay == 22 && currMonth == 4) || (currDay == 23 && currMonth == 4) || (currDay == 29 && currMonth == 4) || (currDay == 30 && currMonth == 4) || (currDay == 6 && currMonth == 5) || (currDay == 7 && currMonth == 5) || (currDay == 12 && currMonth == 5) || (currDay == 13 && currMonth == 5) || (currDay == 14 && currMonth == 5) || (currDay == 20 && currMonth == 5) || (currDay == 21 && currMonth == 5) || (currDay == 22 && currMonth == 5) || (currDay == 27 && currMonth == 5) || (currDay == 29 && currMonth == 5) || (currDay == 30 && currMonth == 5) || (currDay == 31 && currMonth == 5) || (currDay == 1 && currMonth == 6))
		{
			/* There should be no input file for these dates */
			if((inFile = fopen(inputFilename, "r")))
			{
				printf("\nError! I did not expect to find an input file with the following filename: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			/* Open the input file */
			if(!(inFile = fopen(inputFilename, "r")))
			{
				printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}


			/* Read the file line by line */
			lineCounter = 0;
			while(fgets(line, MAX_LINE_SIZE, inFile))
			{
				lineCounter++;

				if(line[0] == '\n')
				{
					continue;
				}

				field = 0;
				value = strtok(line, " ");
				while(value)
				{
					field++;

					if(field == 1)
					{
						strcpy(expectedValue, "Bus");
						expectedEntry = true;
					}
					else if(field == 2)
					{/* BUS ID */
						expectedEntry = false;
						tmpNodeA = strtoull(value, NULL, 10);
					}
					else if(field == 3)
					{
						strcpy(expectedValue, "at");
						expectedEntry = true;
					}
					else if(field == 4)
					{/* NORTH */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 5)
					{/* WEST */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 6)
					{
						strcpy(expectedValue, "on");
						expectedEntry = true;
					}
					else if(field == 7)
					{
						strcpy(expectedValue, "route");
						expectedEntry = true;
					}
					else if(field == 8)
					{/* ROUTE */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 9)
					{
						strcpy(expectedValue, "in");
						expectedEntry = true;
					}
					else if(field == 10)
					{
						strcpy(expectedValue, "contact");
						expectedEntry = true;
					}
					else if(field == 11)
					{
						strcpy(expectedValue, "with");
						expectedEntry = true;
					}
					else if(field == 12)
					{
						strcpy(expectedValue, "bus");
						expectedEntry = true;
					}
					else if(field == 13)
					{/* BUS ID */
						tmpNodeB = strtoull(value, NULL, 10);
						expectedEntry = false;
					}
					else if(field == 14)
					{
						strcpy(expectedValue, "at");
						expectedEntry = true;
					}
					else if(field == 15)
					{/* NORTH */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 16)
					{/* WEST */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 17)
					{
						strcpy(expectedValue, "on");
						expectedEntry = true;
					}
					else if(field == 18)
					{
						strcpy(expectedValue, "route");
						expectedEntry = true;
					}
					else if(field == 19)
					{/* ROUTE */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 20)
					{
						strcpy(expectedValue, "at");
						expectedEntry = true;
					}
					else if(field == 21)
					{
						strcpy(expectedValue, "time");
						expectedEntry = true;
					}
					else if(field == 22)
					{/* MMMM:SS */
						strcpy(tmpTime, value);
						expectedEntry = false;
					}
					else if(field == 23)
					{
						strcpy(expectedValue, "for");
						expectedEntry = true;
					}
					else if(field == 24)
					{/* BYTES */
						/* Do nothing */
						expectedEntry = false;
					}
					else if(field == 25)
					{
						strcpy(expectedValue, "bytes");
						expectedEntry = true;
					}
					else if(field == 26)
					{
						strcpy(expectedValue, "in");
						expectedEntry = true;
					}
					else if(field == 27)
					{/* CONTACT DURATION (convert milliseconds to seconds) */
						if((unsigned long long int) atof(value) > 100000000000)
						{
							tmpDuration = 60;
						}
						else
						{
							tmpDuration = (unsigned long long int) fabs(round(atof(value) / 1000.0));
						}
						expectedEntry = false;
					}
					else if(field == 28)
					{
						strcpy(expectedValue, "ms\n");
						expectedEntry = true;
					}
					else
					{
						printf("\nError at line #%llu of the input file %s\nUnexpected field #%llu: %s\n\n", lineCounter, inputFilename, field, value);
						exit(EXIT_FAILURE);
					}

					if(expectedEntry)
					{
						if(strcmp(value, expectedValue))
						{
							printf("\nError at line #%llu of the input file %s\nExpected to read \"%s\" at field #%llu but read \"%s\" instead\n\n", lineCounter, inputFilename, expectedValue, field, value);
							exit(EXIT_FAILURE);
						}
					}

					value = strtok(NULL, " ");
				}

				field = 0;
				value = strtok(tmpTime, ":");
				while(value)
				{
					field++;

					if(field == 1)
					{/* Elapsed minutes since midnight of the current day */
						tmpMinutes = strtoull(value, NULL, 10);
					}
					else if(field == 2)
					{/* Elapsed seconds since midnight of the current day */
						tmpSeconds = strtoull(value, NULL, 10);
					}
					else
					{
						printf("\nError at line #%llu of the input file %s\nUnexpected attribute #%llu at field #22: %s\n\n", lineCounter, inputFilename, field, value);
						exit(EXIT_FAILURE);
					}

					value = strtok(NULL, ":");
				}


				/* Collect and normalize all node IDs */
				foundA = false;
				foundB = false;
				for(it = uniqueNodeIDs.begin(); it != uniqueNodeIDs.end(); ++it)
				{
					if((*it).originalNodeID == tmpNodeA)
					{
						foundA = true;
						normNodeA = (*it).normalizedNodeID;
					}

					if((*it).originalNodeID == tmpNodeB)
					{
						foundB = true;
						normNodeB = (*it).normalizedNodeID;
					}

					if(foundA && foundB)
					{
						break;
					}
				}

				if(!foundA)
				{
					normNodeA = numNodes;

					tmpPairIDs.originalNodeID = tmpNodeA;
					tmpPairIDs.normalizedNodeID = normNodeA;
					uniqueNodeIDs.push_back(tmpPairIDs);

					numNodes++;
				}

				if(!foundB)
				{
					normNodeB = numNodes;

					tmpPairIDs.originalNodeID = tmpNodeB;
					tmpPairIDs.normalizedNodeID = normNodeB;
					uniqueNodeIDs.push_back(tmpPairIDs);

					numNodes++;
				}


				/* Calculate the elapsed seconds since midnight of the first day */
				elapsedSeconds = (86400 * elapsedDays) + (60 * tmpMinutes) + tmpSeconds;

				if(firstEntry)
				{
					minSimulationTime = elapsedSeconds;
					firstEntry = false;
				}
				else
				{
					if(elapsedSeconds < minSimulationTime)
					{
						printf("Error!\nExpected the first entry to have the earliest entry (%llu), but I later encountered an entry with only %llu elapsed seconds since midnight of the first day\n", minSimulationTime, elapsedSeconds);
						exit(EXIT_FAILURE);
					}
				}

				fprintf(outFile, "%llu\t%llu\t%llu\t%llu\n", normNodeA, normNodeB, (elapsedSeconds - minSimulationTime), ((elapsedSeconds + tmpDuration) - minSimulationTime));
			}


			/* Close the input file */
			if(fclose(inFile))
			{
				printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
				exit(EXIT_FAILURE);
			}
		}


		/* Move to the next day */
		if(currMonth > 12)
		{
			printf("Error!\n The current month (%u) exceeded the maximum value which is 12\n\n", currMonth);
			exit(EXIT_FAILURE);
		}
		else
		{
			if(currDay == numDaysInMonth[currMonth - 1])
			{
				currDay = 1;
				currMonth++;

				if(currMonth == 13)
				{
					printf("Error!\n The current month (%u) exceeded the maximum value which is 12\n\n", currMonth);
					exit(EXIT_FAILURE);
				}
			}
			else if(currDay > numDaysInMonth[currMonth - 1])
			{
				printf("Error!\n The current day (%u) exceeded the maximum value of the current month (%u)\n\n", currDay, currMonth);
				exit(EXIT_FAILURE);
			}
			else
			{
				currDay++;
			}

			elapsedDays++;
		}
	}


	/* Close the output file */
	if(fclose(outFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}

	uniqueNodeIDs.clear();

	return EXIT_SUCCESS;
}
