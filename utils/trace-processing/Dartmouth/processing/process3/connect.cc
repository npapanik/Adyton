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
#include <string>
#include <list>

#define MAX_LINE_SIZE 512

using namespace std;



struct IntervalPoint
{
	unsigned long long int nodeID;
	unsigned long long int eventTime;
	bool isContactUp;

	bool operator < (const IntervalPoint& p2) const
	{
		if(eventTime < p2.eventTime)
		{
			return true;
		}
		else if(eventTime == p2.eventTime)
		{
			if(isContactUp)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
};



int main(int argc, char *argv[])
{
	unsigned long long int i;
	unsigned long long int countAPs;
	unsigned long long int countPairs;
	unsigned long long int field;
	unsigned long long int lineCounter;
	unsigned long long int tmpNode;
	unsigned long long int tmpContactUp;
	unsigned long long int tmpContactDown;
	unsigned long long int counterA;
	unsigned long long int counterB;
	unsigned long long int numNodes;
	unsigned long long int numAPs;
	unsigned long long int minContactUpA;
	unsigned long long int maxContactDownA;
	unsigned long long int minContactUpB;
	unsigned long long int maxContactDownB;
	char line[MAX_LINE_SIZE];
	char inputFilename[MAX_LINE_SIZE];
	char outputFilename[MAX_LINE_SIZE];
	char *value;
	bool found;
	bool firstIntervalA;
	bool firstIntervalB;
	IntervalPoint tmpIntervalPoint;
	list<IntervalPoint> intervalPairs;
	list<IntervalPoint>::iterator it;
	list<unsigned long long int> uniqueNodeIDs;
	list<unsigned long long int>::iterator itA;
	list<unsigned long long int>::iterator itB;
	list<string> uniqueAPs;
	list<string>::iterator itAP;
	FILE *inFile;
	FILE *outFile;


	/* Create the input filename */
	sprintf(inputFilename, "./uniqueAPs.txt");


	/* Open the input file */
	if(!(inFile = fopen(inputFilename, "r")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Read the file line by line */
	numAPs = 0;
	lineCounter = 0;
	while(fgets(line, MAX_LINE_SIZE, inFile))
	{
		lineCounter++;

		if(line[0] == '\n')
		{
			continue;
		}

		for(i = 0; i < MAX_LINE_SIZE; i++)
		{
			if(line[i] == '\n')
			{
				line[i] = '\0';
				break;
			}
		}


		/* Collect the names of all the access points */
		found = false;
		for(itAP = uniqueAPs.begin(); itAP != uniqueAPs.end(); ++itAP)
		{
			if(!strcmp((*itAP).c_str(), line))
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			uniqueAPs.push_back(line);
			numAPs++;
		}
	}


	/* Close the input file */
	if(fclose(inFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
		exit(EXIT_FAILURE);
	}


	/* Create the output filename */
	sprintf(outputFilename, "../process4/tmpDartmouth.txt");


	/* Open the output file */
	if(!(outFile = fopen(outputFilename, "w")))
	{
		printf("\nError! Couldn't open the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}


	/* Process the contacts of the nodes with each access point */
	countAPs = 0;
	for(itAP = uniqueAPs.begin(); itAP != uniqueAPs.end(); ++itAP)
	{
		/* Create the input filename */
		sprintf(inputFilename, "./%s.txt", (*itAP).c_str());


		/* Open the input file */
		if(!(inFile = fopen(inputFilename, "r")))
		{
			printf("\nError! Couldn't open the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Read the file line by line */
		numNodes = 0;
		lineCounter = 0;
		while(fgets(line, MAX_LINE_SIZE, inFile))
		{
			lineCounter++;

			if(line[0] == '\n')
			{
				continue;
			}

			field = 0;
			value = strtok(line, "\t");
			while(value)
			{
				field++;

				if(field == 1)
				{/* Node ID */
					tmpNode = strtoull(value, NULL, 10);
				}
				else if(field == 2)
				{/* Contact Up */
					tmpContactUp = strtoull(value, NULL, 10);
				}
				else if(field == 3)
				{/* Contact Down */
					tmpContactDown = strtoull(value, NULL, 10);
				}
				else
				{
					printf("\nError at line #%llu of the input file!\nUnexpected field #%llu: %s\n\n", lineCounter, field, value);
					exit(EXIT_FAILURE);
				}

				value = strtok(NULL, "\t");
			}


			/* Collect all node IDs */
			found = false;
			for(itA = uniqueNodeIDs.begin(); itA != uniqueNodeIDs.end(); ++itA)
			{
				if((*itA) == tmpNode)
				{
					found = true;
					break;
				}
			}

			if(!found)
			{
				uniqueNodeIDs.push_back(tmpNode);
				numNodes++;
			}
		}


		/* Close the input file */
		if(fclose(inFile))
		{
			printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
			exit(EXIT_FAILURE);
		}


		/* Find all the overlapping intervals for each pair of nodes */
		countPairs = 0;
		if(numNodes > 1)
		{
			for(itA = uniqueNodeIDs.begin(); itA != uniqueNodeIDs.end(); ++itA)
			{
				for(itB = itA; itB != uniqueNodeIDs.end(); ++itB)
				{
					if((*itA) == (*itB))
					{
						continue;
					}

					countPairs++;
					firstIntervalA = true;
					firstIntervalB = true;


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
						value = strtok(line, "\t");
						while(value)
						{
							field++;

							if(field == 1)
							{/* Node ID */
								tmpNode = strtoull(value, NULL, 10);
							}
							else if(field == 2)
							{/* Contact Up */
								tmpContactUp = strtoull(value, NULL, 10);
							}
							else if(field == 3)
							{/* Contact Down */
								tmpContactDown = strtoull(value, NULL, 10);
							}
							else
							{
								printf("\nError at line #%llu of the input file!\nUnexpected field #%llu: %s\n\n", lineCounter, field, value);
								exit(EXIT_FAILURE);
							}

							value = strtok(NULL, "\t");
						}


						/* Collect all the interval start-points and end-points */
						if(tmpNode == (*itA) || tmpNode == (*itB))
						{
							tmpIntervalPoint.nodeID = tmpNode;
							tmpIntervalPoint.eventTime = tmpContactUp;
							tmpIntervalPoint.isContactUp = true;
							intervalPairs.push_back(tmpIntervalPoint);

							tmpIntervalPoint.nodeID = tmpNode;
							tmpIntervalPoint.eventTime = tmpContactDown;
							tmpIntervalPoint.isContactUp = false;
							intervalPairs.push_back(tmpIntervalPoint);

							if(tmpNode == (*itA))
							{
								if(firstIntervalA)
								{
									minContactUpA = tmpContactUp;
									maxContactDownA = tmpContactDown;
									firstIntervalA = false;
								}
								else
								{
									if(tmpContactUp < minContactUpA)
									{
										minContactUpA = tmpContactUp;
									}

									if(tmpContactDown > maxContactDownA)
									{
										maxContactDownA = tmpContactDown;
									}
								}
							}
							else
							{
								if(firstIntervalB)
								{
									minContactUpB = tmpContactUp;
									maxContactDownB = tmpContactDown;
									firstIntervalB = false;
								}
								else
								{
									if(tmpContactUp < minContactUpB)
									{
										minContactUpB = tmpContactUp;
									}

									if(tmpContactDown > maxContactDownB)
									{
										maxContactDownB = tmpContactDown;
									}
								}
							}
						}
					}


					/* Close the input file */
					if(fclose(inFile))
					{
						printf("\nError! Couldn't close the file: %s\n\n", inputFilename);
						exit(EXIT_FAILURE);
					}


					if((maxContactDownA < minContactUpB) || (maxContactDownB < minContactUpA))
					{
						/* These nodes were never in contact */
					}
					else
					{
						/* Sort the intervals points of the two nodes */
						intervalPairs.sort();


						/* Calculate all the overlapping intervals of the two nodes */
						counterA = 0;
						counterB = 0;
						for(it = intervalPairs.begin(); it != intervalPairs.end(); ++it)
						{
							if((*it).isContactUp)
							{
								if((*it).nodeID == (*itA))
								{
									counterA++;

									if((counterA == 1) && (counterB > 0))
									{
										fprintf(outFile, "%llu\t%llu\t%llu\t", (*itA), (*itB), (*it).eventTime);
									}
								}
								else if((*it).nodeID == (*itB))
								{
									counterB++;

									if((counterB == 1) && (counterA > 0))
									{
										fprintf(outFile, "%llu\t%llu\t%llu\t", (*itA), (*itB), (*it).eventTime);
									}
								}
								else
								{
									printf("Error! Expected an interval point from node %llu or node %llu, but read an interval point from node %llu\n\n", (*itA), (*itB), (*it).nodeID);
									exit(EXIT_FAILURE);
								}
							}
							else
							{
								if((*it).nodeID == (*itA))
								{
									if(counterA == 0)
									{
										printf("Error! Read the end-point of an interval at simulation time %llu from node %llu before reading a start-point at the %s AP\n\n", (*it).eventTime, (*it).nodeID, (*itAP).c_str());
										exit(EXIT_FAILURE);
									}
									else
									{
										counterA--;
									}

									if((counterA == 0) && (counterB > 0))
									{
										fprintf(outFile, "%llu\n", (*it).eventTime);
									}
								}
								else if((*it).nodeID == (*itB))
								{
									if(counterB == 0)
									{
										printf("Error! Read the end-point of an interval at simulation time %llu from node %llu before reading a start-point at the %s AP\n\n", (*it).eventTime, (*it).nodeID, (*itAP).c_str());
										exit(EXIT_FAILURE);
									}
									else
									{
										counterB--;
									}

									if((counterB == 0) && (counterA > 0))
									{
										fprintf(outFile, "%llu\n", (*it).eventTime);
									}
								}
								else
								{
									printf("Error! Expected an interval point from node %llu or node %llu, but read an interval point from node %llu\n\n", (*itA), (*itB), (*it).nodeID);
									exit(EXIT_FAILURE);
								}
							}
						}

						if((counterA > 0) || (counterB > 0))
						{
							printf("Error! The counters for the overlapping intervals between node %llu (%llu) and node %llu (%llu) at the %s AP are not equal to 0 after processing all of their intervals\n\n", (*itA), counterA, (*itB), counterB, (*itAP).c_str());
							exit(EXIT_FAILURE);
						}
					}

					intervalPairs.clear();

					//printf("Progress: %.2f%%\r", 100.0 * ((((double) countAPs) / ((double) numAPs)) + ((1.0 / ((double) numAPs)) * (((double) countPairs) / (((double) (numNodes * (numNodes - 1))) / 2.0)))));
					fflush(stdout);
				}
			}
		}

		countAPs++;
		uniqueNodeIDs.clear();
	}


	/* Close the output file */
	if(fclose(outFile))
	{
		printf("\nError! Couldn't close the file: %s\n\n", outputFilename);
		exit(EXIT_FAILURE);
	}

	uniqueAPs.clear();

	return EXIT_SUCCESS;
}
