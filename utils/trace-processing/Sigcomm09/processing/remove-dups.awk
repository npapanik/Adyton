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


BEGIN{flag=0;}
{
if(flag==1)
{
	if(!($1 == A && $2 == B && $3>= start && $4<=end))
	{
		printf("%d\t%d\t%d\t%d\n",$1,$2,$3,$4);
	}
}
else
{
	flag=1;
	A=$1;
	B=$2;
	start=$3;
	end=$4;
}
}
