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
 *  Written by Nikolaos Papanikos.
 */


#ifndef SPM_H
	#define SPM_H
	#include "SPM.h"
#endif

/* Description
 * -----------
 *  Social Pressure Metric
 *  
 */

SPM::SPM(int ID,int NodeSize): Utility(ID,NodeSize)
{
	this->ActiveContacts=(int *)malloc(sizeof(int)*NodeSize);
	this->SPMsums=(double *)malloc(sizeof(double)*NodeSize);
	this->LastContactTime=(double *)malloc(sizeof(double)*NodeSize);
	this->SPMvalues=(double *)malloc(sizeof(double)*NodeSize);
	for(int i=0;i<NodeSize;i++)
	{
		this->ActiveContacts[i]=0;
		this->LastContactTime[i]=0.0;
		this->SPMsums[i]=0.0;
		this->SPMvalues[i]=-1.0;
	}
	
// 	this->N=(int *)malloc(sizeof(int)*NodeSize);
// 	for(int i=0;i<NodeSize;i++)
// 	{
// 		N[i]=0;
// 	}
// 	this->tAstart=(double **)malloc(sizeof(double *)*NodeSize);
// 	this->tAend=(double **)malloc(sizeof(double *)*NodeSize);
// 	this->tBstart=(double **)malloc(sizeof(double *)*NodeSize);
// 	this->tBend=(double **)malloc(sizeof(double *)*NodeSize);
// 	this->RSPMsums=(double **)malloc(sizeof(double *)*NodeSize);
// 	this->RSPMvalues=(double **)malloc(sizeof(double *)*NodeSize);
// 	this->MyRSPMvalues=(double **)malloc(sizeof(double *)*NodeSize);
// 	for(int i=0;i<NodeSize;i++)
// 	{
// 		this->tAstart[i]=(double *)malloc(sizeof(double)*NodeSize);
// 		this->tAend[i]=(double *)malloc(sizeof(double)*NodeSize);
// 		this->tBstart[i]=(double *)malloc(sizeof(double)*NodeSize);
// 		this->tBend[i]=(double *)malloc(sizeof(double)*NodeSize);
// 		this->RSPMsums[i]=(double *)malloc(sizeof(double)*NodeSize);
// 		this->RSPMvalues[i]=(double *)malloc(sizeof(double)*NodeSize);
// 		this->MyRSPMvalues[i]=(double *)malloc(sizeof(double)*NodeSize);
// 	}
// 	//initialize
// 	for(int i=0;i<NodeSize;i++)
// 	{
// 		for(int j=0;j<NodeSize;j++)
// 		{
// 			this->tAstart[i][j]=0.0;
// 			this->tAend[i][j]=0.0;
// 			this->tBstart[i][j]=0.0;
// 			this->tBend[i][j]=0.0;
// 			this->RSPMsums[i][j]=0.0;
// 			this->RSPMvalues[i][j]=0.0;
// 			this->MyRSPMvalues[i][j]=-1.0;
// 		}
// 	}
	return;
}

SPM::~SPM()
{
	free(ActiveContacts);
	free(SPMsums);
	free(LastContactTime);
	free(SPMvalues);
	
// 	for(int i=0;i<NSize;i++)
// 	{
// 		free(tAstart[i]);
// 		free(tAend[i]);
// 		free(tBstart[i]);
// 		free(tBend[i]);
// 		free(RSPMsums[i]);
// 		free(RSPMvalues[i]);
// 		free(MyRSPMvalues[i]);
// 	}
// 	free(tAstart);
// 	free(tAend);
// 	free(tBstart);
// 	free(tBend);
// 	free(RSPMsums);
// 	free(RSPMvalues);
// 	free(MyRSPMvalues);
	
	return;
}


void SPM::Update(int ID,double CurrentTime)
{
	return;
}

void SPM::ContactUp(int ID, double CurrentTime)
{
	//printf("%.1f:Contact between %d and %d is activated\n",CurrentTime,this->NID,ID);
	this->ActiveContacts[ID]++;
	if(this->ActiveContacts[ID] > 1)
	{
		//Case a contact between the two nodes is still up
		//Some traces have artifacts of this type 
		//printf("Problem: %f at node %d contact with %d is already up\n",CurrentTime,this->NID,ID);
		return;
	}
	//calculate SPM
	if(CurrentTime > 0)
	{
		double diff=CurrentTime-this->LastContactTime[ID];
		this->SPMsums[ID]+=(diff*diff);
		this->SPMvalues[ID]=this->SPMsums[ID]/(2.0*CurrentTime);
	}
	//calculate RSPM
// 	for(int i=0;i<this->NSize;i++)
// 	{
// 		if(!N[i] || i == ID)
// 		{
// 			continue;
// 		}
// 		tBend[i][ID]=CurrentTime;
// 		double tA=0.0;
// 		double tB=0.0;
// 		if(tAend[i][ID] > tAstart[i][ID])
// 		{
// 			tB=tBend[i][ID]-tBstart[i][ID];
// 			tA=tAend[i][ID]-tAstart[i][ID];
// 			RSPMsums[i][ID]+=(2.0*tA*tB)+(tA*tA);
// 			RSPMvalues[i][ID]=RSPMsums[i][ID]/(2.0*CurrentTime);
// 			//printf("%d: I help %d to reach %d with RSPM value %f\n",this->NID,i,ID,RSPMvalues[i][ID]);
// 			tAstart[i][ID]=tAend[i][ID];
// 		}
// 	}
	return;
}

void SPM::ContactDown(int ID, double CurrentTime)
{
	this->ActiveContacts[ID]--;
	if(this->ActiveContacts[ID])
	{
		//case of multiple parallel contacts
		return;
	}
	this->LastContactTime[ID]=CurrentTime;
	this->SPMvalues[ID]=this->SPMsums[ID]/(2.0*CurrentTime);
// 	for(int i=0;i<this->NSize;i++)
// 	{
// 		if(i == this->NID)
// 		{
// 			continue;
// 		}
// 		tAend[ID][i]=CurrentTime;
// 		tBstart[ID][i]=CurrentTime;
// 	}
// 	N[ID]=1;
	return;
}

double SPM::get(int ID,double CurrentTime)
{
	double wSPM=0.0;
// 	double wRSPM=0.0;
	
	if(this->SPMvalues[ID] > 0.0)
	{
		wSPM=(1.0/(this->SPMvalues[ID]));
	}
	return wSPM;
	
	/*double tmpWeight=0.0;
	//check all potential relayes
	for(int i=0;i<this->NSize;i++)
	{
		if(this->MyRSPMvalues[i][ID] > 0.0)
		{
			tmpWeight=(1.0/this->MyRSPMvalues[i][ID]);
		}
		else
		{
			continue;
		}
		
		if(wRSPM < tmpWeight)
		{
			wRSPM=tmpWeight;
		}
	}
	//printf("wSPM:%f\t wRSPM:%f ->",wSPM,wRSPM);
	if(wSPM >= wRSPM)
	{
		//printf("SPM wins :)\n");
		return wSPM;
	}
	else
	{
		//printf("RSPM wins :)\n");
	}
	return wRSPM*/;
}

double* SPM::getRSPMfor(int ID)
{
	if(!N[ID])
	{
		//case I don't have node with ID as a contact and as a result I do not have any
		//indirect information for him
		return NULL;
	}
	double *res=(double *)malloc(sizeof(double)*this->NSize);
	for(int i=0;i<this->NSize;i++)
	{
		res[i]=RSPMvalues[ID][i];
	}
	return res;
}

void SPM::updateRSPM(int through,double* values)
{
	if(!values)
	{
		return;
	}
	for(int i=0;i<this->NSize;i++)
	{
		MyRSPMvalues[through][i]=values[i];
	}
	return;
}

double SPM::getActualSPM(int ID, double CurrentTime)
{
	if(SPMvalues[ID] < 0.0)
	{
		return -1.0;
	}
	else
	{
		return SPMvalues[ID];
	}
}
