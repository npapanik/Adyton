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


#ifndef CACC_H
	#define CACC_H
	#include "CACC.h"
#endif



CACC::CACC(int NID, PacketBuffer* Bf, Settings *S): CongestionControl(NID, Bf, S)
{
	this->information = NULL;
	this->extraInformation = NULL;
	this->AMTUtils = new AMT(NID, S->getNN());

	return;
}


CACC::~CACC()
{
	delete this->AMTUtils;

	return;
}


void CACC::contactUp(double currTime, int recID)
{
	this->AMTUtils->ContactUp(recID, currTime);

	return;
}


void CACC::contactDown(double currTime, int recID)
{
	this->AMTUtils->ContactDown(recID, currTime);

	return;
}


void CACC::receivedBufferReq(double currentTime, int reqNode)
{
	return;
}


void CACC::addBufferInfo(void* info)
{
	if(!info)
	{
		printf("CACC error: No information added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	this->information = (struct CongestionInformation *) info;

	return;
}


void *CACC::getExtraInfo(double currTime)
{
	struct CACCExtraInformation *extraInfo;


	extraInfo = (struct CACCExtraInformation *) malloc(sizeof(struct CACCExtraInformation));
	extraInfo->recContactNumber = this->AMTUtils->getDstContacts();
	extraInfo->recAverageDuration = this->AMTUtils->getAverageDurations();

	return (void *) extraInfo;
}


void CACC::setExtraInfo(void *extraInfo, double currTime)
{
	if(extraInfo == NULL)
	{
		printf("CACC error: The receiver (%d) did not send the required extra information to the sender (%d) node!\nExiting...\n", information->recID, this->nodeID);
		exit(EXIT_FAILURE);
	}
	else
	{
		this->extraInformation = (struct CACCExtraInformation *) extraInfo;
	}

	return;
}


void CACC::addPacketInfo(struct ImportInformation *imp)
{
	struct CACCPacketInformation newInfo;


	if(this->extraInformation == NULL)
	{
		printf("CACC error: The receiver (%d) did not send the required extra information to the sender (%d) node!\nExiting...\n", information->recID, this->nodeID);
		exit(EXIT_FAILURE);
	}

	newInfo.packetID = imp->pktID;
	newInfo.numContacts = this->extraInformation->recContactNumber[Buff->GetPktDestination(newInfo.packetID)];
	newInfo.normNumContacts = 0.0;
	newInfo.avgDuration = this->extraInformation->recAverageDuration[Buff->GetPktDestination(newInfo.packetID)];
	newInfo.normAvgDuration = 0.0;
	newInfo.gainValue = 0.0;

	pInfo.push_back(newInfo);
	free(imp);

	#ifdef CC_DEBUG
		printf("Added packet information with ID = %d, contact number = %d, and average duration = %f\n", newInfo.packetID, newInfo.numContacts, newInfo.avgDuration);
	#endif

	return;
}


int *CACC::filterPackets(int* pktList)
{
	int i;
	int j;
	int remainingStorage;
	int highestGainPacketID;
	double minNumContacts;
	double maxNumContacts;
	double minAvgDuration;
	double maxAvgDuration;
	double sumNumContacts;
	double sumSquNumContacts;
	double meanNumContacts;
	double diffSumSquNumContacts;
	double sumAvgDuration;
	double sumSquAvgDuration;
	double meanAvgDuration;
	double diffSumSquAvgDuration;
	double jointlySum;
	double contactsStdDev;
	double durationStdDev;
	double corrNumerator;
	double corrDenominator;
	double correlationCoefficient;
	double contactsConflict;
	double durationConflict;
	double contactsWeight;
	double durationWeight;
	double highestGainValue;
	bool first;
	int *newPktList;
	vector<CACCPacketInformation>::iterator it;


	if(!pktList)
	{
		releaseInformation();
		return NULL;
	}

	if(!this->information)
	{
		printf("CACC error: Needed information not added!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	#ifdef CC_DEBUG
		printf("\n>> Received the following packet list: ");
		for(i = 1; i <= pktList[0]; i++)
		{
			printf("%d\t", pktList[i]);
		}
		printf("\n");
	#endif

	remainingStorage = information->recBufSize - information->recBufLength;

	if((remainingStorage > 0) && (pktList[0] > 0))
	{
		/* find the lowest and the highest contact number and average duration */
		first = true;

		for(it = pInfo.begin(); it != pInfo.end(); ++it)
		{
			if(first)
			{
				minNumContacts = (*it).numContacts;
				maxNumContacts = (*it).numContacts;
				minAvgDuration = (*it).avgDuration;
				maxAvgDuration = (*it).avgDuration;
				first = false;
			}
			else
			{
				if(minNumContacts - (*it).numContacts > DBL_EPSILON)
				{
					minNumContacts = (*it).numContacts;
				}
				
				if((*it).numContacts - maxNumContacts > DBL_EPSILON)
				{
					maxNumContacts = (*it).numContacts;
				}

				if(minAvgDuration - (*it).avgDuration > DBL_EPSILON)
				{
					minAvgDuration = (*it).avgDuration;
				}
				
				if((*it).avgDuration - maxAvgDuration > DBL_EPSILON)
				{
					maxAvgDuration = (*it).avgDuration;
				}
			}
		}

		if(first)
		{
			printf("CCAC error: The packet information struct is empty!\nExiting...\n");
			exit(EXIT_FAILURE);
		}

		#ifdef CC_DEBUG
			printf("\nStep 1: minNumContacts = %f, maxNumContacts = %f, minAvgDuration = %f, maxAvgDuration = %f\n", minNumContacts, maxNumContacts, minAvgDuration, maxAvgDuration);
		#endif


		/* normalize contact number and average duration for each packet */
		#ifdef CC_DEBUG
			printf("\nStep 2: List of normalized values:\n");
		#endif

		for(it = pInfo.begin(); it != pInfo.end(); ++it)
		{
			if(minNumContacts == maxNumContacts)
			{
				(*it).normNumContacts = 0.0;
			}
			else
			{
				(*it).normNumContacts = ((*it).numContacts - minNumContacts) / (maxNumContacts - minNumContacts);
			}

			if(minAvgDuration == maxAvgDuration)
			{
				(*it).normAvgDuration = 0.0;
			}
			else
			{
				(*it).normAvgDuration = ((*it).avgDuration - minAvgDuration) / (maxAvgDuration - minAvgDuration);
			}

			#ifdef CC_DEBUG
				printf("Packet ID = %d, normNumContacts = %f, normAvgDuration = %f\n", (*it).packetID, (*it).normNumContacts, (*it).normAvgDuration);
			#endif
		}


		/* calculate the required sums */
		sumNumContacts = 0.0;
		sumSquNumContacts = 0.0;
		sumAvgDuration = 0.0;
		sumSquAvgDuration = 0.0;
		jointlySum = 0.0;

		for(it = pInfo.begin(); it != pInfo.end(); ++it)
		{
			sumNumContacts += (*it).normNumContacts;
			sumSquNumContacts += pow((*it).normNumContacts, 2.0);
			sumAvgDuration += (*it).normAvgDuration;
			sumSquAvgDuration += pow((*it).normAvgDuration, 2.0);
			jointlySum += (*it).normNumContacts * (*it).normAvgDuration;
		}


		/* calculate the standard deviations */
		meanNumContacts = sumNumContacts / pInfo.size();
		meanAvgDuration = sumAvgDuration / pInfo.size();
		diffSumSquNumContacts = 0.0;
		diffSumSquAvgDuration = 0.0;

		for(it = pInfo.begin(); it != pInfo.end(); ++it)
		{
			diffSumSquNumContacts += pow((*it).normNumContacts - meanNumContacts, 2.0);
			diffSumSquAvgDuration += pow((*it).normAvgDuration - meanAvgDuration, 2.0);
		}

		contactsStdDev = sqrt(diffSumSquNumContacts / pInfo.size());
		durationStdDev = sqrt(diffSumSquAvgDuration / pInfo.size());

		#ifdef CC_DEBUG
			printf("\nStep 3: contactsStdDev = %f, durationStdDev = %f\n", contactsStdDev, durationStdDev);
		#endif


		/* calculate the correlation coefficient */
		if((sumNumContacts > DBL_EPSILON) || (sumAvgDuration > DBL_EPSILON))
		{
			corrNumerator = (pInfo.size() * jointlySum) - (sumNumContacts * sumAvgDuration);
			corrDenominator = (sqrt((pInfo.size() * sumSquNumContacts) - pow(sumNumContacts, 2.0))) * (sqrt((pInfo.size() * sumSquAvgDuration) - pow(sumAvgDuration, 2.0)));

			if(corrDenominator > DBL_EPSILON)
			{
				if(corrDenominator - fabs(corrNumerator) > DBL_EPSILON)
				{
					correlationCoefficient = corrNumerator / corrDenominator;
				}
				else
				{
					if(corrNumerator < 0.0)
					{
						correlationCoefficient = -1.0;
					}
					else
					{
						correlationCoefficient = 1.0;
					}
				}
			}
			else
			{
				correlationCoefficient = 0.0;
			}
		}
		else
		{
			correlationCoefficient = 0.0;
		}

		#ifdef CC_DEBUG
			printf("\nStep 4: correlationCoefficient = %f\n", correlationCoefficient);
		#endif


		/* calculate the conflicts */
		contactsConflict = contactsStdDev * (1 - correlationCoefficient);
		durationConflict = durationStdDev * (1 - correlationCoefficient);

		#ifdef CC_DEBUG
			printf("\nStep 5: contactsConflict = %f, durationConflict = %f\n", contactsConflict, durationConflict);
		#endif


		/* calculate the weights */
		if((contactsConflict + durationConflict) > DBL_EPSILON)
		{
			contactsWeight = contactsConflict / (contactsConflict + durationConflict);
			durationWeight = durationConflict / (contactsConflict + durationConflict);
		}
		else
		{
			contactsWeight = 0.0;
			durationWeight = 0.0;
		}

		#ifdef CC_DEBUG
			printf("\nStep 6: contactsWeight = %f, durationWeight = %f\n", contactsWeight, durationWeight);
		#endif


		/* calculate the gain value of each packet */
		#ifdef CC_DEBUG
			printf("\nStep 7: List of gain values:\n");
		#endif

		for(it = pInfo.begin(); it != pInfo.end(); ++it)
		{
			(*it).gainValue = (contactsWeight * (*it).normNumContacts) + (durationWeight * (*it).normAvgDuration);

			#ifdef CC_DEBUG
				printf("Packet ID = %d, gain = %f\n", (*it).packetID, (*it).gainValue);
			#endif
		}


		/* create the new packet list */
		if(pktList[0] <= remainingStorage)
		{
			newPktList = (int *) malloc((pktList[0] + 1) * sizeof(int));
			newPktList[0] = pktList[0];	
		}
		else
		{
			newPktList = (int *) malloc((remainingStorage + 1) * sizeof(int));
			newPktList[0] = remainingStorage;
		}

		#ifdef CC_DEBUG
			printf("\nStep 8: About to choose %d packets\n", newPktList[0]);
		#endif

		for(i = newPktList[0]; i >= 1; i--)
		{
			first = true;
			highestGainValue = 0.0;
			highestGainPacketID = 0;

			for(j = pktList[0]; j >= 1; j--)
			{
				if(pktList[j] != REMOVED_PACKET)
				{
					if(first)
					{
						highestGainValue = getGainValue(pktList[j]);
						highestGainPacketID = pktList[j];
						first = false;
					}
					else
					{
						if(getGainValue(pktList[j]) > highestGainValue)
						{
							highestGainValue = getGainValue(pktList[j]);
							highestGainPacketID = pktList[j];
						}
					}
				}
			}

			if(first)
			{
				break;
			}
			else
			{
				#ifdef CC_DEBUG
					printf("#%d: The packet with ID %d will be forwarded because it satisfies the forwarding condition with gain = %f!\n", (newPktList[0] - i) + 1, highestGainPacketID, highestGainValue);
				#endif

				newPktList[i] = highestGainPacketID;

				for(j = pktList[0]; j >= 1; j--)
				{
					if(pktList[j] == highestGainPacketID)
					{
						pktList[j] = REMOVED_PACKET;
						break;
					}
				}
			}
		}
	}
	else
	{
		newPktList = (int *) malloc(sizeof(int));
		newPktList[0] = 0;

		#ifdef CC_DEBUG
			printf("#0: None packet will be forwarded because there are not enough resources!\n");
		#endif
	}

	free(pktList);
	releaseInformation();

	#ifdef CC_DEBUG
		printf("\n>> Returning the following packet list: ");
		for(i = 1; i <= newPktList[0]; i++)
		{
			printf("%d\t", newPktList[i]);
		}
		printf("\n");
	#endif

	return newPktList;
}


void CACC::releaseInformation()
{
	free(this->information);
	this->information = NULL;
	pInfo.clear();

	if(this->extraInformation != NULL)
	{
		free(this->extraInformation->recContactNumber);
		free(this->extraInformation->recAverageDuration);
		free(this->extraInformation);
		this->extraInformation = NULL;
	}

	return;
}


int CACC::getBufferSize()
{
	return Buff->getBufferSize();
}


int CACC::getBufferLength()
{
	return Buff->getBufferLength();
}


double CACC::getGainValue(int ID)
{
	vector<CACCPacketInformation>::iterator it;


	for(it = pInfo.begin(); it != pInfo.end(); ++it)
	{
		if((*it).packetID == ID)
		{
			return (*it).gainValue;
		}
	}

	printf("CACC error: Could not find the packet with ID %d\nExiting...\n", ID);
	exit(EXIT_FAILURE);
}
