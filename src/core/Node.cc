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
 *  Written by Nikolaos Papanikos.
 */


#ifndef NODE_H
	#define NODE_H
	#include "Node.h"
#endif

#ifndef GOD_H
	#define GOD_H
	#include "God.h"
#endif

class God;

//#define NODE_DEBUG


/* Constructor:Node
 * ----------------
 * Initialization according to user input. More specifically, the packet buffer
 * and the routing protocol of each node are initialized.
 */
Node::Node(int NodeID, PacketPool *pktPool, MAC *mlayer, Statistics *St, Settings *S, God *G)
{
	int randomRange;
	int BufferSize;
	string profileAttribute;


	this->SimSet = S;
	this->ID = NodeID;
	this->Pool = pktPool;
	this->macLayer = mlayer;
	this->Stat = St;
	this->Gd = G;

	if((profileAttribute = this->SimSet->GetProfileAttribute("UniformBuffer")) != "none")
	{
		if((randomRange = atoi(profileAttribute.c_str())) < 0)
		{
			printf("Error! Read a negative range of values for the uniform buffer distribution (%d)\nExiting...\n", randomRange);
			exit(EXIT_FAILURE);
		}

		BufferSize = this->SimSet->getBuffer() + (rand() % (randomRange + 1));
	}
	else
	{
		BufferSize = this->SimSet->getBuffer();
	}

	this->Buffer = new PacketBuffer(this->ID, this->Stat, this->Gd, BufferSize, this->SimSet->getDroppingPolicy(), this->SimSet->getTTL());

	switch(SimSet->getRT())
	{
		case DIRECT_RT:
		{
			this->RLogic = new Direct(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case EPIDEMIC_RT:
		{
			this->RLogic = new Epidemic(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case PROPHET_RT:
		{
			this->RLogic = new Prophet(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case SIMBET_RT:
		{
			this->RLogic = new SimBet(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case SIMBETTS_RT:
		{
			this->RLogic = new SimBetTS(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case BUBBLERAP_RT:
		{
			this->RLogic = new BubbleRap(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case SW_RT:
		{
			this->RLogic = new SprayWait(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case LSFSW_RT:
		{
			this->RLogic = new LSFSpray(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case MSFSW_RT:
		{
			this->RLogic = new MSFSpray(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case PROPHETSW_RT:
		{
			this->RLogic = new ProphetSpray(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case LSFSF_RT:
		{
			this->RLogic = new LSFSprayFocus(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case CNF_RT:
		{
			/* Compare and Forward is the single-copy version of Compare and Replicate */
			this->RLogic = new CnR(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case CNR_RT:
		{
			this->RLogic = new CnR(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case EBR_RT:
		{
			this->RLogic = new EBR(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case DELEGATION_RT:
		{
			this->RLogic = new Delegation(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		case COORD_RT:
		{
			//this->RLogic = new COORDE(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			printf("\n[Error]: The COORD routing protocol is temporarily not available\n\n");
			exit(EXIT_FAILURE);
			break;
		}
		case OPTIMAL_RT:
		{
			this->RLogic = new Optimal(this->Pool, this->macLayer, this->Buffer, this->ID, this->Stat, this->SimSet, this->Gd);
			break;
		}
		default:
		{
			printf("\n[Error]: (Node::Node) Unknown routing protocol identifier (%d)\n\n", SimSet->getRT());
			exit(EXIT_FAILURE);
		}
	}

	return;
}


Node::~Node()
{
	delete Buffer;
	delete RLogic;

	return;
}


/* ConUpdate
 * ---------
 * This method is called when the node connects/disconnects (see status)
 * with other nodes of the network.
 * 
 * Note: Here a data structure should handle the connections and
 * disconnections. This data structure must have limited resources
 * and probably utilize timers or windows (in order to be realistic).
 */
void Node::ConUpdate(double CTime, int NodeID, bool status, bool history)
{
	if(status)
	{
		if(!history)
		{
			RLogic->NewContact(CTime, NodeID);
		}
		else
		{
			RLogic->Contact(CTime, NodeID);
		}
	}
	else
	{
		RLogic->ContactRemoved(CTime,NodeID);
	}

	return;
}


/* recvFromApp
 * -----------
 * This method is called when a new packet is received from the application
 * layer. The packet has not been created yet. At this point, a new packet
 * must be created with the proper attributes.
 */
void Node::recvFromApp(double CurTime, int Dest)
{
	Packet *tmp;
	Header *h;


	/* Create a new data packet and initialize its header properly */
	tmp = new DataPacket(CurTime, 0);
	h = new SimpleHeader(this->ID, Dest, this->ID);
	h->SetHops(0);
	h->SetRep(SimSet->getReplicas());
	tmp->setHeader(h);
	Pool->AddOriginalPacket(tmp);


	/* Send the new data packet to the network layer */
	RLogic->recv(CurTime,tmp->getID());

	#ifdef NODE_DEBUG
		printf("%f: Node %d generated a new data packet with ID %d\n", CurTime, this->ID, tmp->getID());
	#endif
	
	return;
}


/* recv
 * ----
 * This method is called when a new packet is received from the network
 * layer.
 */
void Node::recv(int pktID, double CurTime)
{
	#ifdef NODE_DEBUG
		printf("%f: Node %d received packet with ID %d\n", CurTime, this->ID, pktID);
	#endif

	RLogic->recv(CurTime,pktID);

	return;
}


/* PrintBuffer
 * -----------
 * Prints information about packets inside the buffer.
 */
void Node::PrintBuffer(void)
{
	printf("Node %d - Buffer Contents(%d)\n", this->ID, Buffer->getPacketNum());
	printf("-----------------------------\n");
	Buffer->PrintPkts();
	printf("-----------------------------\n");
}
