#  Adyton: A Network Simulator for Opportunistic Networks
#  Copyright (C) 2015, 2016  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
#  and Evangelos Papapetrou
#
#  This file is part of Adyton.
#
#  Adyton is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Adyton is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Adyton.  If not, see <http://www.gnu.org/licenses/>.
#
#  Written by Nikolaos Papanikos.


BEGIN{flag=0;unknown=0;distance=0;polling=140;}
{
if(flag==1)
{
	distance++;
	# If the end time of a contact with dev[i] is found on previous iteration
	# set found[i] as true to reduce the number of searches
	for(i=1;i<=unknown;i++)
	{
		if(end[i] == 0)
		{
			found[i]=0;
		}
		else
		{
			found[i]=1;
		}
	}
	# search all the device IDs of the current row
	# if a device id that we are looking for is found set
	# found[i] to 1
	for(i=1;i<=unknown;i++)
	{
		if(found[i] == 1)
		{
			continue;
		}
		for(j=2;j<=NF;j++)
		{
			if($j == id[i] && $1<=(start+(distance*polling)))
			{
				found[i]=1;
				break;
			}
		}
	}
	# if found[i] and end[i] are both 0, then the device ID id[i] does not
	# exist in the current row but exists in all previous rows. In other words,
	# this is the first polling interval that the device is not seen. So we have
	# to mark the end of contact for the device id[i] 
	for(i=1;i<=unknown;i++)
	{
		if(found[i] == 0 && end[i] == 0)
		{
			if($1<=(start+(distance*polling)))
			{
				end[i]=$1;	
			}
			else
			{
				end[i]=start+(distance*polling);
			}
		}		
	}
	
}
else
{
	flag=1;
	unknown=NF-1;
	start=$1;
	#fill an array with the device IDs of first line
	for(i=1;i<=unknown;i++)
	{
		id[i]=$(i+1);
		end[i]=0;
		found[i]=0;
	}
}
HighestTime=$1;
}
END{
	for(i=1;i<=unknown;i++)
	{
		if(!(end[i] == 0))
		{
			printf("%d\t%d\t%d\t%d\n",NID,id[i],start,end[i]);
		}
		else
		{
			#case that contact is scanned in the end of the discovery file
			# there is not enough data to find the end time of the contact
			# then, just stop the contact to the next scanning interval of 
			# the last event (of the device) we know of
			printf("%d\t%d\t%d\t%d\n",NID,id[i],start,HighestTime+120);
		}
	}
}
