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
 *  Written by Nikolaos Papanikos and Dimitrios-Georgios Akestoridis.
 */


#ifndef RES_H
	#define RES_H
	#include "Results.h"
#endif


/* Constructor:Results
 * -------------------
 * Initializing attributes.
 */
Results::Results(vector<Statistics*> *St, vector<Node*> *N, Settings* Setng, God *SGod, TrafficGen *TrfGen)
{
	Stats = St;
	Nodes = N;
	Set = Setng;
	SimGod = SGod;
	Traffic = TrfGen;
	LastUpdateDR = 0.0;
	LastUpdateFW = 0.0;
	LastUpdateDrops = 0.0;
	LastUpdateBO = 0.0;
	LastValDR = 0.0;
	LastValFW = 0.0;
	LastValDrops = 0.0;
	LastValBO = 0.0;
	LastTimeUpdated = 0.0;
	FPTimeDR = NULL;
	FPTimeFW = NULL;
	FPTimeDrops = NULL;
	FPTimeBO = NULL;
	CommitDR = false;
	CommitFW = false;
	CommitDrops = false;
	CommitBO = false;
	this->simID = 0;

	return;
}


/* writeSimulationSettings
 * -----------------------
 * Creates a new directory inside the res/ directory (if it does not already exist)
 * to write the simulation settings and other simulation results.
 */
void Results::writeSimulationSettings()
{
	char command[256];
	char settingsFilename[256];
	char numLines[256];
	FILE *setFile;


	/* If the results directory does not exists, create it */
	if(access(Set->getResultsDirectory().c_str(), F_OK) != 0)
	{
		printf("Creating directory \"%s\"!\n\n", Set->getResultsDirectory().c_str());

		sprintf(command, "mkdir -p %s", Set->getResultsDirectory().c_str());

		if(system(command) < 0)
		{
			printf("\n[Error]: Failed to execute the command \"%s\"\n\n", command);
			exit(EXIT_FAILURE);
		}
	}


	/* If the is no file with previous simulation settings, create one */
	sprintf(settingsFilename, "%sSimulationSettings.txt", Set->getResultsDirectory().c_str());

	if(access(settingsFilename, F_OK) != 0)
	{
		if((setFile = fopen(settingsFilename, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", settingsFilename);
			exit(EXIT_FAILURE);
		}

		fprintf(setFile, "#1.SID\t2.TRACE\t3.RT\t4.CC\t5.SP\t6.DP\t7.DM\t8.OUTPUT\t9.TRAFFIC_TYPE\t10.TRAFFIC_LOAD\t11.TTL\t12.BUFFER\t13.REP\t14.SPLIT\t15.SEED\t16.PROFILE\t17.RES_DIR\t18.TRC_DIR\t19.BATCH\n");

		if(fclose(setFile) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", settingsFilename);
			exit(EXIT_FAILURE);
		}
	}


	/* Get a unique simulation identifier */
	sprintf(command, "wc -l < %s", settingsFilename);

	if((setFile = popen(command, "r")) == NULL)
	{
		printf("\n[Error]: Failed to execute the command \"%s\"\n\n", command);
		exit(EXIT_FAILURE);
	}

	if(fgets(numLines, 256, setFile) != NULL)
	{
		this->simID = strtoull(numLines, NULL, 10);
	}
	else
	{
		printf("\n[Error]: Failed to read the output of the command \"%s\"\n\n", command);
		exit(EXIT_FAILURE);
	}

	if(pclose(setFile) == -1)
	{
		printf("\n[Error]: An error was detected while executing the command \"%s\"\n\n", command);
		exit(EXIT_FAILURE);
	}


	/* Write the simulation settings */
	if((setFile = fopen(settingsFilename, "a")) == NULL)
	{
		printf("\n[Error]: Could not open the file \"%s\"\n\n", settingsFilename);
		exit(EXIT_FAILURE);
	}

	fprintf(setFile, "%llu\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%d\t%d\t%ld\t%d\t%s\t%s\t%s\t%d\n", this->simID, Set->getContactTrace(), Set->getRT(), Set->getCongestionControl(), Set->getSchedulingPolicy(), Set->getDroppingPolicy(), Set->getDM(), Set->getOUT(), Set->getTrafficType(), Set->getTrafficLoad(), Set->getTTL(), Set->getBuffer(), Set->getReplicas(), Set->getSplit(), Set->getSeed(), Set->getProfileName().c_str(), Set->getResultsDirectory().c_str(), Set->getTraceDirectory().c_str(), Set->getBatchmode());

	if(fclose(setFile) == EOF)
	{
		printf("\n[Error]: Could not close the file \"%s\"\n\n", settingsFilename);
		exit(EXIT_FAILURE);
	}

	return;
}


/* writeSimulationResults
 * ----------------------
 * 
 */
void Results::writeSimulationResults()
{
	if(Set->getOUT() == T1_OUT)
	{
		this->writeBasicResults();
		this->writeDetailedResults();
	}
	else if(Set->getOUT() == T2_OUT)
	{
		this->writeBasicResults();
		this->writeDetailedResults();

		this->writePacketAnalysis();
	}
	else if(Set->getOUT() == T3_OUT)
	{
		this->writeBasicResults();
		this->writeDetailedResults();

		this->writePacketAnalysis();

		this->writeFairnessIndices();
	}
	else if(Set->getOUT() == T4_OUT)
	{
		this->writeBasicResults();
		this->writeDetailedResults();

		this->writePacketAnalysis();

		this->writeFairnessIndices();

		this->writeDistributions();
	}
	else if(Set->getOUT() == T5_OUT)
	{
		this->writeBasicResults();
		this->writeDetailedResults();

		this->writePacketAnalysis();

		this->writeFairnessIndices();

		this->writeDistributions();
	}
	else
	{
		printf("\nError! Unknown output type: %d\n", Set->getOUT());
		printf("Writing only basic results...\n\n");

		this->writeBasicResults();
	}

	return;
}


/* writeBasicResults
 * -----------------
 * 
 */
void Results::writeBasicResults()
{
	int i=0;
	double DRatio=0.0;
	double sumDelay=0.0;
	double avgDelay=0.0;
	double avgHops=0.0;
	unsigned long TotalDelivered=0;
	unsigned long TotalForw=0;
	unsigned long Dups=0;
	unsigned long ReplicasCreated=0;
	unsigned long TotalPktsDropped=0;
	unsigned long TotalPktsDied=0;
	unsigned long sumHops=0;
	char basicFilename[256];
	FILE *basicResFile=NULL;

	sprintf(basicFilename, "%sBasicResults.txt", Set->getResultsDirectory().c_str());
	if(access(basicFilename, F_OK) != 0)
	{
		if((basicResFile = fopen(basicFilename, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", basicFilename);
			exit(EXIT_FAILURE);
		}

		fprintf(basicResFile, "#%-8s  %-18s  %-18s  %-18s  %-10s  %-14s  %-14s  %-14s  %-14s  %-14s\n","SID","Delivery Ratio","Transmissions","Delay","Hops(avg)","Generated","Delivered","DUPS","DROPS","TIMEOUTS");

		if(fclose(basicResFile) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", basicFilename);
			exit(EXIT_FAILURE);
		}
	}
	for(i = 0; i < Set->getNN(); i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}

		TotalDelivered += (*Stats)[i]->getTotalDelivered();
		TotalForw += (*Stats)[i]->getTransmissions();
		Dups += (*Stats)[i]->getDuplicates();
		sumDelay += (*Stats)[i]->getDelay();
		sumHops += (*Stats)[i]->getHops();
		ReplicasCreated += (*Stats)[i]->getRepCreated();
		TotalPktsDropped += (*Stats)[i]->getPktsDropped();
		TotalPktsDied += (*Stats)[i]->getPktsDied();
	}
	if(Set->getRT() == OPTIMAL_RT)
	{
		/* Optimal Routing would avoid any redundant transmission */
		TotalForw = sumHops;
		Dups = 0;
	}
	DRatio = ((double) TotalDelivered) / ((double) Traffic->getTotalStatPkts());
	avgDelay = sumDelay / ((double) TotalDelivered);
	avgHops = ((double) sumHops) / ((double) TotalDelivered);
	if((basicResFile = fopen(basicFilename, "a")) == NULL)
	{
		printf("\n[Error]: Could not open the file \"%s\"\n\n", basicFilename);
		exit(EXIT_FAILURE);
	}
	fprintf(basicResFile, "%-9llu  %-18.4f  %-18lu  %-18.2f  %-10.2f  %-14d  %-14lu  %-14lu  %-14lu  %-14lu\n", this->simID, DRatio, TotalForw, avgDelay, avgHops,  Traffic->getTotalStatPkts(), TotalDelivered, Dups, TotalPktsDropped, TotalPktsDied);
	if(fclose(basicResFile) == EOF)
	{
		printf("\n[Error]: Could not close the file \"%s\"\n\n", basicFilename);
		exit(EXIT_FAILURE);
	}

	return;
}

/* writeDetailedResults
 * --------------------
 * 
 */
void Results::writeDetailedResults()
{
	int i;
	double DRatio;
	double sumDelay;
	double avgDelay;
	double avgHops;
	double avgRep;
	double overheadRatio;
	double deliveryCost;
	double redundancyRatio;
	double wastageIndex;
	double dropRatio;
	unsigned long TotalDelivered;
	unsigned long TotalForw;
	unsigned long Dups;
	unsigned long ReplicasCreated;
	unsigned long TotalPktsDropped;
	unsigned long TotalPktsDied;
	unsigned long sumHops;
	char detailedFilename[256];
	FILE *detailedResFile;


	sprintf(detailedFilename, "%sDetailedResults.txt", Set->getResultsDirectory().c_str());

	if(access(detailedFilename, F_OK) != 0)
	{
		if((detailedResFile = fopen(detailedFilename, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", detailedFilename);
			exit(EXIT_FAILURE);
		}

		fprintf(detailedResFile, "#1.SID\t2.Delivery Ratio\t3.Overhead Ratio\t4.Delivery Cost\t5.Average Number of Hops\t6.Redundancy Ratio\t7.Wastage Index\t8.Average Delay (seconds)\t9.Drop Ratio\t10.Average Replication\t11.Total Number of Forwards\t12.Total Number of Packet Drops\t13.Total Number of Packet Timeouts\t14.Total Number of Duplicates\t15.Total Number of Packets in the Traffic Sample\n");

		if(fclose(detailedResFile) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", detailedFilename);
			exit(EXIT_FAILURE);
		}
	}

	TotalDelivered = 0;
	DRatio = 0.0;
	sumDelay = 0.0;
	avgDelay = 0.0;
	avgHops = 0.0;
	avgRep = 0.0;
	overheadRatio = 0.0;
	deliveryCost = 0.0;
	redundancyRatio = 0.0;
	wastageIndex = 0.0;
	dropRatio = 0.0;
	TotalForw = 0;
	Dups = 0;
	ReplicasCreated = 0;
	TotalPktsDropped = 0;
	TotalPktsDied = 0;
	sumHops = 0;

	for(i = 0; i < Set->getNN(); i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}

		TotalDelivered += (*Stats)[i]->getTotalDelivered();
		TotalForw += (*Stats)[i]->getTransmissions();
		Dups += (*Stats)[i]->getDuplicates();
		sumDelay += (*Stats)[i]->getDelay();
		sumHops += (*Stats)[i]->getHops();
		ReplicasCreated += (*Stats)[i]->getRepCreated();
		TotalPktsDropped += (*Stats)[i]->getPktsDropped();
		TotalPktsDied += (*Stats)[i]->getPktsDied();
	}

	if(Set->getRT() == OPTIMAL_RT)
	{
		/* Optimal Routing would avoid any redundant transmission */
		TotalForw = sumHops;
		Dups = 0;
	}

	DRatio = ((double) TotalDelivered) / ((double) Traffic->getTotalStatPkts());
	avgDelay = sumDelay / ((double) TotalDelivered);
	avgHops = ((double) sumHops) / ((double) TotalDelivered);
	avgRep = ((double) ReplicasCreated + (double) Traffic->getTotalStatPkts()) / ((double) Traffic->getTotalStatPkts());
	overheadRatio = ((double) TotalForw) / ((double) Traffic->getTotalStatPkts());
	deliveryCost = ((double) TotalForw) / ((double) TotalDelivered);
	redundancyRatio = ((double) (TotalForw - sumHops)) / ((double) TotalDelivered);
	wastageIndex = ((double) (TotalForw - sumHops)) / ((double) TotalForw);
	dropRatio = ((double) TotalPktsDropped) / ((double) Traffic->getTotalStatPkts());

	if((detailedResFile = fopen(detailedFilename, "a")) == NULL)
	{
		printf("\n[Error]: Could not open the file \"%s\"\n\n", detailedFilename);
		exit(EXIT_FAILURE);
	}

	fprintf(detailedResFile, "%llu\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%d\n", this->simID, DRatio, overheadRatio, deliveryCost, avgHops, redundancyRatio, wastageIndex, avgDelay, dropRatio, avgRep, TotalForw, TotalPktsDropped, TotalPktsDied, Dups, Traffic->getTotalStatPkts());

	if(fclose(detailedResFile) == EOF)
	{
		printf("\n[Error]: Could not close the file \"%s\"\n\n", detailedFilename);
		exit(EXIT_FAILURE);
	}

	return;
}


void Results::writePacketAnalysis()
{
	char pktFilename[512];


	sprintf(pktFilename, "%sPacketAnalysis_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
	SimGod->writePktStats(pktFilename);

	return;
}


/* writeFairnessIndices
 * --------------------
 * 
 */
void Results::writeFairnessIndices()
{
	int i;
	unsigned long sumRelays;
	unsigned long sumSquRelays;
	unsigned long sumForwards;
	unsigned long sumSquForwards;
	unsigned long sumDrops;
	unsigned long sumSquDrops;
	unsigned long sumHandovers;
	unsigned long sumSquHandovers;
	double fairnessRelays;
	double fairnessForwards;
	double fairnessDrops;
	double fairnessHandovers;
	char fairnessFilename[256];
	FILE *fairnessIndicesFile;


	sprintf(fairnessFilename, "%sFairnessIndices.txt", Set->getResultsDirectory().c_str());

	if(access(fairnessFilename, F_OK) != 0)
	{
		if((fairnessIndicesFile = fopen(fairnessFilename, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", fairnessFilename);
			exit(EXIT_FAILURE);
		}

		fprintf(fairnessIndicesFile, "#1.SID\t2.Fairness Index based on the Number of Times used as a Relay Node\t3.Fairness Index based on the Number of Forwards\t4.Fairness Index based on the Number of Packet Drops\t5.Fairness Index based on the Number of Handovers\n");

		if(fclose(fairnessIndicesFile) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", fairnessFilename);
			exit(EXIT_FAILURE);
		}
	}

	sumRelays = 0;
	sumSquRelays = 0;
	sumForwards = 0;
	sumSquForwards = 0;
	sumDrops = 0;
	sumSquDrops = 0;
	sumHandovers = 0;
	sumSquHandovers = 0;
	fairnessRelays = 0.0;
	fairnessForwards = 0.0;
	fairnessDrops = 0.0;
	fairnessHandovers = 0.0;

	for(i = 0; i < Set->getNN(); i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}

		sumRelays += (*Stats)[i]->getTimesAsRelayNode();
		sumSquRelays += pow((*Stats)[i]->getTimesAsRelayNode(), 2.0);
		sumForwards += (*Stats)[i]->getTransmissions();
		sumSquForwards += pow((*Stats)[i]->getTransmissions(), 2.0);
		sumDrops += (*Stats)[i]->getPktsDropped();
		sumSquDrops += pow((*Stats)[i]->getPktsDropped(), 2.0);
		sumHandovers += (*Stats)[i]->getHandovers();
		sumSquHandovers += pow((*Stats)[i]->getHandovers(), 2.0);
	}

	if(sumRelays > 0)
	{
		fairnessRelays = pow(sumRelays, 2.0) / (Set->getActiveNodes() * sumSquRelays);
	}
	else
	{
		fairnessRelays = 1.0;
	}

	if(sumForwards > 0)
	{
		fairnessForwards = pow(sumForwards, 2.0) / (Set->getActiveNodes() * sumSquForwards);
	}
	else
	{
		fairnessForwards = 1.0;
	}

	if(sumDrops > 0)
	{
		fairnessDrops = pow(sumDrops, 2.0) / (Set->getActiveNodes() * sumSquDrops);
	}
	else
	{
		fairnessDrops = 1.0;
	}

	if(sumHandovers > 0)
	{
		fairnessHandovers = pow(sumHandovers, 2.0) / (Set->getActiveNodes() * sumSquHandovers);
	}
	else
	{
		fairnessHandovers = 1.0;
	}

	if((fairnessIndicesFile = fopen(fairnessFilename, "a")) == NULL)
	{
		printf("\n[Error]: Could not open the file \"%s\"\n\n", fairnessFilename);
		exit(EXIT_FAILURE);
	}

	fprintf(fairnessIndicesFile, "%llu\t%f\t%f\t%f\t%f\n", this->simID, fairnessRelays, fairnessForwards, fairnessDrops, fairnessHandovers);

	if(fclose(fairnessIndicesFile) == EOF)
	{
		printf("\n[Error]: Could not close the file \"%s\"\n\n", fairnessFilename);
		exit(EXIT_FAILURE);
	}

	return;
}


/* writeDistributions
 * ------------------
 * 
 */
void Results::writeDistributions()
{
	int i;
	int nodeID;
	unsigned int tmpValue;
	char distrFilename[512];
	FILE *distributionFile;


	for(i = 0; i < 6; i++)
	{
		if(i == 0)
		{
			sprintf(distrFilename, "%sDistributionForwards_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		}
		else if(i == 1)
		{
			sprintf(distrFilename, "%sDistributionRelays_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		}
		else if(i == 2)
		{
			sprintf(distrFilename, "%sDistributionHandovers_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		}
		else if(i == 3)
		{
			sprintf(distrFilename, "%sDistributionDrops_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		}
		else if(i == 4)
		{
			sprintf(distrFilename, "%sDistributionSrcDrops_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		}
		else if(i == 5)
		{
			sprintf(distrFilename, "%sDistributionRelDrops_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		}
		else
		{
			printf("\n[Error]: Unknown distribution identifier (%d)\n\n", i);
			exit(EXIT_FAILURE);
		}

		if((distributionFile = fopen(distrFilename, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", distrFilename);
			exit(EXIT_FAILURE);
		}

		if(i == 0)
		{
			fprintf(distributionFile, "#1.Node ID\t2.Number of Forwards\n");
		}
		else if(i == 1)
		{
			fprintf(distributionFile, "#1.Node ID\t2.Number of Times used as a Relay Node\n");
		}
		else if(i == 2)
		{
			fprintf(distributionFile, "#1.Node ID\t2.Number of Handovers\n");
		}
		else if(i == 3)
		{
			fprintf(distributionFile, "#1.Node ID\t2.Number of Packet Drops\n");
		}
		else if(i == 4)
		{
			fprintf(distributionFile, "#1.Node ID\t2.Number of Source Packet Drops\n");
		}
		else if(i == 5)
		{
			fprintf(distributionFile, "#1.Node ID\t2.Number of Relay Packet Drops\n");
		}
		else
		{
			printf("\n[Error]: Unknown distribution identifier (%d)\n\n", i);
			exit(EXIT_FAILURE);
		}

		for(nodeID = 0; nodeID < Set->getNN(); nodeID++)
		{
			if(!Traffic->isInactive(nodeID))
			{
				if(i == 0)
				{
					tmpValue = (unsigned int) (*Stats)[nodeID]->getTransmissions();
				}
				else if(i == 1)
				{
					tmpValue = (*Stats)[nodeID]->getTimesAsRelayNode();
				}
				else if(i == 2)
				{
					tmpValue = (*Stats)[nodeID]->getHandovers();
				}
				else if(i == 3)
				{
					tmpValue = (*Stats)[nodeID]->getPktsDropped();
				}
				else if(i == 4)
				{
					tmpValue = (*Stats)[nodeID]->getSrcPktsDropped();
				}
				else if(i == 5)
				{
					tmpValue = (*Stats)[nodeID]->getRelPktsDropped();
				}
				else
				{
					printf("\n[Error]: Unknown distribution identifier (%d)\n\n", i);
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				tmpValue = 0.0;
			}

			fprintf(distributionFile, "%d\t%u\n", nodeID, tmpValue);
		}

		if(fclose(distributionFile) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", distrFilename);
			exit(EXIT_FAILURE);
		}
	}

	return;
}


/* printBasicRes
 * -------------
 * 
 */
void Results::printBasicRes(bool writeToFile)
{
	int i;
	double DRatio;
	double sumDelay;
	double avgDelay;
	double avgHops;
	double avgRep;
	double overheadRatio;
	double deliveryCost;
	double redundancyRatio;
	double wastageIndex;
	double dropRatio;
	unsigned long TotalDelivered;
	unsigned long TotalForw;
	unsigned long Dups;
	unsigned long ReplicasCreated;
	unsigned long TotalPktsDropped;
	unsigned long TotalPktsDied;
	unsigned long sumHops;


	TotalDelivered = 0;
	DRatio = 0.0;
	sumDelay = 0.0;
	avgDelay = 0.0;
	avgHops = 0.0;
	avgRep = 0.0;
	overheadRatio = 0.0;
	deliveryCost = 0.0;
	redundancyRatio = 0.0;
	wastageIndex = 0.0;
	dropRatio = 0.0;
	TotalForw = 0;
	Dups = 0;
	ReplicasCreated = 0;
	TotalPktsDropped = 0;
	TotalPktsDied = 0;
	sumHops = 0;

	for(i = 0; i < Set->getNN(); i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}

		TotalDelivered += (*Stats)[i]->getTotalDelivered();
		TotalForw += (*Stats)[i]->getTransmissions();
		Dups += (*Stats)[i]->getDuplicates();
		sumDelay += (*Stats)[i]->getDelay();
		sumHops += (*Stats)[i]->getHops();
		ReplicasCreated += (*Stats)[i]->getRepCreated();
		TotalPktsDropped += (*Stats)[i]->getPktsDropped();
		TotalPktsDied += (*Stats)[i]->getPktsDied();
	}

	if(Set->getRT() == OPTIMAL_RT)
	{
		/* Optimal Routing would avoid any redundant transmission */
		TotalForw = sumHops;
		Dups = 0;
	}

	DRatio = ((double) TotalDelivered) / ((double) Traffic->getTotalStatPkts());
	avgDelay = sumDelay / ((double) TotalDelivered);
	avgHops = ((double) sumHops) / ((double) TotalDelivered);
	avgRep = ((double) ReplicasCreated + (double) Traffic->getTotalStatPkts()) / ((double) Traffic->getTotalStatPkts());
	overheadRatio = ((double) TotalForw) / ((double) Traffic->getTotalStatPkts());
	deliveryCost = ((double) TotalForw) / ((double) TotalDelivered);
	redundancyRatio = ((double) (TotalForw - sumHops)) / ((double) TotalDelivered);
	wastageIndex = ((double) (TotalForw - sumHops)) / ((double) TotalForw);
	dropRatio = ((double) TotalPktsDropped) / ((double) Traffic->getTotalStatPkts());

	if(!writeToFile)
	{
		printf("\n");
		printf("Delivery Ratio: %f\n", DRatio);
		printf("Overhead Ratio: %f\n", overheadRatio);
		printf("Delivery Cost: %f\n", deliveryCost);
		printf("Average Number of Hops: %f\n", avgHops);
		printf("Redundancy Ratio: %f\n", redundancyRatio);
		printf("Wastage Index: %f\n", wastageIndex);
		printf("Average Delay: %f seconds, %f minutes, %f hours, %f days\n", avgDelay, avgDelay / 60.0, avgDelay / 3600.0, avgDelay / 86400.0);
		printf("Drop Ratio: %f\n", dropRatio);
		printf("Average Replication: %f\n", avgRep);
		printf("Total Number of Forwards: %lu\n", TotalForw);
		printf("Total Number of Packet Drops: %lu\n", TotalPktsDropped);
		printf("Total Number of Packet Timeouts: %lu\n", TotalPktsDied);
		printf("Total Number of Duplicates: %lu\n", Dups);
	}
	else
	{
		//GUIrandom=Set->getGUItmpFilename();
		FILE *GUIres=NULL;
		char GUIrandom[150]="/tmp/AdytonGUIrandom.txt";
		if ((GUIres=fopen(GUIrandom, "w")) == NULL)
		{
			printf("Error creating file %s!!!\n",GUIrandom);
			exit(1);
		}

		fprintf(GUIres,"Delivery Ratio: %f\n", DRatio);
		fprintf(GUIres,"Overhead Ratio: %f\n", overheadRatio);
		fprintf(GUIres,"Delivery Cost: %f\n", deliveryCost);
		fprintf(GUIres,"Average Number of Hops: %f\n", avgHops);
		fprintf(GUIres,"Redundancy Ratio: %f\n", redundancyRatio);
		fprintf(GUIres,"Wastage Index: %f\n", wastageIndex);
		fprintf(GUIres,"Average Delay: %f seconds, %f minutes, %f hours, %f days\n", avgDelay, avgDelay / 60.0, avgDelay / 3600.0, avgDelay / 86400.0);
		fprintf(GUIres,"Drop Ratio: %f\n", dropRatio);
		fprintf(GUIres,"Average Replication: %f\n", avgRep);
		fprintf(GUIres,"Total Number of Forwards: %lu\n", TotalForw);
		fprintf(GUIres,"Total Number of Packet Drops: %lu\n", TotalPktsDropped);
		fprintf(GUIres,"Total Number of Packet Timeouts: %lu\n", TotalPktsDied);
		fprintf(GUIres,"Total Number of Duplicates: %lu\n", Dups);
		
		if(fclose(GUIres)==EOF)
		{
			printf("ERROR: could not close file: %s", GUIrandom);
			exit(1);
		}
	}

	return;
}


/* printSynopsis
 * -------------
 * 
 */
void Results::printSynopsis()
{
	int i;
	unsigned long TotalDelivered;
	double DRatio;
	double sumDelay;
	double avgDelay;
	double avgHops;
	double avgRep;
	double overheadRatio;
	double deliveryCost;
	double redundancyRatio;
	double wastageIndex;
	double dropRatio;
	unsigned long TotalForw;
	unsigned long Dups;
	unsigned long ReplicasCreated;
	unsigned long TotalPktsDropped;
	unsigned long TotalPktsDied;
	unsigned long sumHops;


	TotalDelivered = 0;
	DRatio = 0.0;
	sumDelay = 0.0;
	avgDelay = 0.0;
	avgHops = 0.0;
	avgRep = 0.0;
	overheadRatio = 0.0;
	deliveryCost = 0.0;
	redundancyRatio = 0.0;
	wastageIndex = 0.0;
	dropRatio = 0.0;
	TotalForw = 0;
	Dups = 0;
	ReplicasCreated = 0;
	TotalPktsDropped = 0;
	TotalPktsDied = 0;
	sumHops = 0;

	for(i = 0; i < Set->getNN(); i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}

		TotalDelivered += (*Stats)[i]->getTotalDelivered();
		TotalForw += (*Stats)[i]->getTransmissions();
		Dups += (*Stats)[i]->getDuplicates();
		sumDelay += (*Stats)[i]->getDelay();
		sumHops += (*Stats)[i]->getHops();
		ReplicasCreated += (*Stats)[i]->getRepCreated();
		TotalPktsDropped += (*Stats)[i]->getPktsDropped();
		TotalPktsDied += (*Stats)[i]->getPktsDied();
	}

	if(Set->getRT() == OPTIMAL_RT)
	{
		/* Optimal Routing would avoid any redundant transmission */
		TotalForw = sumHops;
		Dups = 0;
	}

	DRatio = ((double) TotalDelivered) / ((double) Traffic->getTotalStatPkts());
	avgDelay = sumDelay / ((double) TotalDelivered);
	avgHops = ((double) sumHops) / ((double) TotalDelivered);
	avgRep = ((double) ReplicasCreated + (double) Traffic->getTotalStatPkts()) / ((double) Traffic->getTotalStatPkts());
	overheadRatio = ((double) TotalForw) / ((double) Traffic->getTotalStatPkts());
	deliveryCost = ((double) TotalForw) / ((double) TotalDelivered);
	redundancyRatio = ((double) (TotalForw - sumHops)) / ((double) TotalDelivered);
	wastageIndex = ((double) (TotalForw - sumHops)) / ((double) TotalForw);
	dropRatio = ((double) TotalPktsDropped) / ((double) Traffic->getTotalStatPkts());

	printf("%d\t%s\t%s\t%d\t%d\t%f\t%f\t%f\t%lu\t%lu\t%lu\t%d\t%d\t%f\t%lu\t%d\t%f\t%f\t%f\t%f\t%d\t%f\t%f\n", Set->getSeed(), Set->getTraceName()->c_str(), Set->getRTname()->c_str(), Set->getReplicas(), Set->getBuffer(), DRatio, avgDelay, avgHops, TotalForw, Dups, TotalPktsDropped, Set->getTrafficType(), Set->getDroppingPolicy(), Set->getTTL(), TotalPktsDied, Set->getTrafficLoad(), deliveryCost, overheadRatio, dropRatio, avgRep, Traffic->getTotalStatPkts(), redundancyRatio, wastageIndex);

	return;
}


/* printStatsPerNode
 * -----------------
 * 
 */
void Results::printStatsPerNode()
{
	//Print per node Statistics
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		//Nodes[i]->PrintBuffer();
		printf("Node %d\n",i);
		printf("-------\n");
		(*Stats)[i]->PrintResults();
	}
	return;
}


/* openFiles
 * ---------
 * 
 */
void Results::openFiles()
{
	sprintf(NameTimeDR, "%sTemporalDeliveryRatio_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
	if((FPTimeDR = fopen(NameTimeDR, "w")) == NULL)
	{
		printf("\n[Error]: Could not open the file \"%s\"\n\n", NameTimeDR);
		exit(EXIT_FAILURE);
	}
	fprintf(FPTimeDR, "#1.Fraction of Simulation Time\t2.Delivery Ratio\n");

	sprintf(NameTimeFW,"%sTemporalForwards_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
	if((FPTimeFW = fopen(NameTimeFW, "w")) == NULL)
	{
		printf("\n[Error]: Could not open the file \"%s\"\n\n", NameTimeFW);
		exit(EXIT_FAILURE);
	}
	fprintf(FPTimeFW, "#1.Fraction of Simulation Time\t2.Number of Forwards\n");

	if(Set->getBuffer() > 0)
	{
		sprintf(NameTimeDrops,"%sTemporalPacketDrops_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		if((FPTimeDrops = fopen(NameTimeDrops, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", NameTimeDrops);
			exit(EXIT_FAILURE);
		}
		fprintf(FPTimeDrops, "#1.Fraction of Simulation Time\t2.Number of Packet Drops\n");

		sprintf(NameTimeBO,"%sTemporalBufferOccupancy_SID%llu.txt", Set->getResultsDirectory().c_str(), this->simID);
		if((FPTimeBO = fopen(NameTimeBO, "w")) == NULL)
		{
			printf("\n[Error]: Could not open the file \"%s\"\n\n", NameTimeBO);
			exit(EXIT_FAILURE);
		}
		fprintf(FPTimeBO, "#1.Fraction of Simulation Time\t2.Buffer Occupancy\n");
	}

	return;
}


/* closeFiles
 * ----------
 * 
 */
void Results::closeFiles()
{
	if(fclose(FPTimeDR) == EOF)
	{
		printf("\n[Error]: Could not close the file \"%s\"\n\n", NameTimeDR);
		exit(EXIT_FAILURE);
	}

	if(fclose(FPTimeFW) == EOF)
	{
		printf("\n[Error]: Could not close the file \"%s\"\n\n", NameTimeFW);
		exit(EXIT_FAILURE);
	}

	if(Set->getBuffer() > 0)
	{
		if(fclose(FPTimeDrops) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", NameTimeDrops);
			exit(EXIT_FAILURE);
		}

		if(fclose(FPTimeBO) == EOF)
		{
			printf("\n[Error]: Could not close the file \"%s\"\n\n", NameTimeBO);
			exit(EXIT_FAILURE);
		}
	}

	return;
}


/* initAll
 * ----
 * 
 */
void Results::initAll()
{
	openFiles();
	initDR();
	initFW();

	if(Set->getBuffer() > 0)
	{
		initDrops();
		initBO();
	}

	return;
}


/* initDR
 * ------
 * 
 */
void Results::initDR()
{
	unsigned long TotalDelivered=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalDelivered+=(*Stats)[i]->getTotalDelivered();
	}
	LastUpdateDR = 0.0;
	LastValDR = (double) TotalDelivered / (double) Traffic->getTotalStatPkts();
	fprintf(FPTimeDR,"%f\t%f\n", LastUpdateDR, LastValDR);
	return;
}


/* initFW
 * ------
 * 
 */
void Results::initFW()
{
	unsigned long TotalForwarded=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalForwarded+=(*Stats)[i]->getTransmissions();
	}
	LastUpdateFW = 0.0;
	LastValFW = (double) TotalForwarded;
	fprintf(FPTimeFW,"%f\t%f\n", LastUpdateFW, LastValFW);
	return;
}


/* initDrops
 * ---------
 * 
 */
void Results::initDrops()
{
	unsigned long TotalDropped=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalDropped+=(*Stats)[i]->getPktsDropped();
	}
	LastUpdateDrops = 0.0;
	LastValDrops = (double) TotalDropped;
	fprintf(FPTimeDrops,"%f\t%f\n", LastUpdateDrops, LastValDrops);
	return;
}


/* initBO
 * ------
 * 
 */
void Results::initBO()
{
	unsigned long sumBufSize=0;
	unsigned long sumBufLength=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		sumBufSize+=(*Nodes)[i]->Buffer->getBufferSize();
		sumBufLength+=(*Nodes)[i]->Buffer->BufferStores();
	}
	LastUpdateBO = 0.0;
	LastValBO = (double) sumBufLength / (double) sumBufSize;
	fprintf(FPTimeBO,"%f\t%f\n", LastUpdateBO, LastValBO);
	return;
}


/* updateAll
 * ---------
 * 
 */
void Results::updateAll(double currTime)
{
	updateDR(currTime);
	updateFW(currTime);

	if(Set->getBuffer() > 0)
	{
		updateDrops(currTime);
		updateBO(currTime);
	}

	updateTime(currTime);

	return;
}


/* updateDR
 * --------
 * 
 */
void Results::updateDR(double currTime)
{
	if(LastTimeUpdated == currTime)
	{
		return;
	}
	unsigned long TotalDelivered=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalDelivered+=(*Stats)[i]->getTotalDelivered();
	}
	double currTimeDR = LastTimeUpdated/Set->getTraceDuration();
	double currValueDR = (double) TotalDelivered / (double) Traffic->getTotalStatPkts();
	if(currValueDR == LastValDR)
	{
		LastUpdateDR = currTimeDR;
		CommitDR = true;
	}
	else
	{
		if(CommitDR)
		{
			fprintf(FPTimeDR,"%f\t%f\n", LastUpdateDR, LastValDR);
		}
		fprintf(FPTimeDR,"%f\t%f\n", currTimeDR, currValueDR);
		LastUpdateDR = currTimeDR;
		LastValDR = currValueDR;
		CommitDR = false;
	}
	return;
}


/* updateFW
 * --------
 * 
 */
void Results::updateFW(double currTime)
{
	if(LastTimeUpdated == currTime)
	{
		return;
	}
	unsigned long TotalForwarded=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalForwarded+=(*Stats)[i]->getTransmissions();
	}
	double currTimeFW = LastTimeUpdated/Set->getTraceDuration();
	double currValueFW = (double) TotalForwarded;
	if(currValueFW == LastValFW)
	{
		LastUpdateFW = currTimeFW;
		CommitFW = true;
	}
	else
	{
		if(CommitFW)
		{
			fprintf(FPTimeFW,"%f\t%f\n", LastUpdateFW, LastValFW);
		}
		fprintf(FPTimeFW,"%f\t%f\n", currTimeFW, currValueFW);
		LastUpdateFW = currTimeFW;
		LastValFW = currValueFW;
		CommitFW = false;
	}
	return;
}


/* updateDrops
 * -----------
 * 
 */
void Results::updateDrops(double currTime)
{
	if(LastTimeUpdated == currTime)
	{
		return;
	}
	unsigned long TotalDropped=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalDropped+=(*Stats)[i]->getPktsDropped();
	}
	double currTimeDrops = LastTimeUpdated/Set->getTraceDuration();
	double currValueDrops = (double) TotalDropped;
	if(currValueDrops == LastValDrops)
	{
		LastUpdateDrops = currTimeDrops;
		CommitDrops = true;
	}
	else
	{
		if(CommitDrops)
		{
			fprintf(FPTimeDrops,"%f\t%f\n", LastUpdateDrops, LastValDrops);
		}
		fprintf(FPTimeDrops,"%f\t%f\n", currTimeDrops, currValueDrops);
		LastUpdateDrops = currTimeDrops;
		LastValDrops = currValueDrops;
		CommitDrops = false;
	}
	return;
}


/* updateBO
 * --------
 * 
 */
void Results::updateBO(double currTime)
{
	if(LastTimeUpdated == currTime)
	{
		return;
	}
	unsigned long sumBufSize=0;
	unsigned long sumBufLength=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		sumBufSize+=(*Nodes)[i]->Buffer->getBufferSize();
		sumBufLength+=(*Nodes)[i]->Buffer->BufferStores();
	}
	double currTimeBO = LastTimeUpdated/Set->getTraceDuration();
	double currValueBO = (double) sumBufLength / (double) sumBufSize;
	if(currValueBO == LastValBO)
	{
		LastUpdateBO = currTimeBO;
		CommitBO = true;
	}
	else
	{
		if(CommitBO)
		{
			fprintf(FPTimeBO,"%f\t%f\n", LastUpdateBO, LastValBO);
		}
		fprintf(FPTimeBO,"%f\t%f\n", currTimeBO, currValueBO);
		LastUpdateBO = currTimeBO;
		LastValBO = currValueBO;
		CommitBO = false;
	}
	return;
}


/* updateTime
 * ----------
 * 
 */
void Results::updateTime(double currTime)
{
	LastTimeUpdated = currTime;

	return;
}


/* finalizeAll
 * --------
 * 
 */
void Results::finalizeAll()
{
	finalizeDR();
	finalizeFW();

	if(Set->getBuffer() > 0)
	{
		finalizeDrops();
		finalizeBO();
	}

	closeFiles();

	return;
}


/* finalizeDR
 * ----------
 * 
 */
void Results::finalizeDR()
{
	unsigned long TotalDelivered=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalDelivered+=(*Stats)[i]->getTotalDelivered();
	}
	double currTimeDR = 1.0;
	double currValueDR = (double) TotalDelivered / (double) Traffic->getTotalStatPkts();
	if(CommitDR && !(currValueDR == LastValDR))
	{
		fprintf(FPTimeDR,"%f\t%f\n", LastUpdateDR, LastValDR);
	}
	fprintf(FPTimeDR,"%f\t%f\n", currTimeDR, currValueDR);
	return;
}


/* finalizeFW
 * ----------
 * 
 */
void Results::finalizeFW()
{
	unsigned long TotalForwarded=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalForwarded+=(*Stats)[i]->getTransmissions();
	}
	double currTimeFW = 1.0;
	double currValueFW = (double) TotalForwarded;
	if(CommitFW && !(currValueFW == LastValFW))
	{
		fprintf(FPTimeFW,"%f\t%f\n", LastUpdateFW, LastValFW);
	}
	fprintf(FPTimeFW,"%f\t%f\n", currTimeFW, currValueFW);
	return;
}


/* finalizeDrops
 * -------------
 * 
 */
void Results::finalizeDrops()
{
	unsigned long TotalDropped=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		TotalDropped+=(*Stats)[i]->getPktsDropped();
	}
	double currTimeDrops = 1.0;
	double currValueDrops = (double) TotalDropped;
	if(CommitDrops && !(currValueDrops == LastValDrops))
	{
		fprintf(FPTimeDrops,"%f\t%f\n", LastUpdateDrops, LastValDrops);
	}
	fprintf(FPTimeDrops,"%f\t%f\n", currTimeDrops, currValueDrops);
	return;
}


/* finalizeBO
 * ----------
 * 
 */
void Results::finalizeBO()
{
	unsigned long sumBufSize=0;
	unsigned long sumBufLength=0;
	for(int i=0;i<Set->getNN();i++)
	{
		if(Traffic->isInactive(i))
		{
			continue;
		}
		
		sumBufSize+=(*Nodes)[i]->Buffer->getBufferSize();
		sumBufLength+=(*Nodes)[i]->Buffer->BufferStores();
	}
	double currTimeBO = 1.0;
	double currValueBO = (double) sumBufLength / (double) sumBufSize;
	if(CommitBO && !(currValueBO == LastValBO))
	{
		fprintf(FPTimeBO,"%f\t%f\n", LastUpdateBO, LastValBO);
	}
	fprintf(FPTimeBO,"%f\t%f\n", currTimeBO, currValueBO);
	return;
}
