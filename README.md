Adyton
======

Adyton is an event-driven network simulator, written in C++, for Opportunistic Networks (a.k.a. Delay-Tolerant Networks) that is capable of processing contact traces.
The Adyton simulator supports a plethora of routing protocols and real-world contact traces, while also providing several congestion control mechanisms and buffer management policies.

Adyton was developed by [Nikolaos Papanikos](http://cse.uoi.gr/~npapanik) and [Dimitrios-Georgios Akestoridis](http://cse.uoi.gr/~dakestor) during their graduate studies in the [Department of Computer Science and Engineering at University of Ioannina, Greece](http://cse.uoi.gr) under the supervision of Prof. [Evangelos Papapetrou](http://cse.uoi.gr/~epap).

> Adyton was a restricted area within Ancient Greek temples, where oracles and priests gave their oracular responses.


Features
----------

**Supported Contact Traces<sup>[1](#note1)</sup>**
* [Milan PMTR](http://crawdad.org/unimi/pmtr/20081201/)
* [MIT Reality Mining](http://crawdad.org/mit/reality/20050701/)
* [Intel](http://crawdad.org/cambridge/haggle/20090529/)
* [Haggle-Cam](http://crawdad.org/cambridge/haggle/20090529/)
* [INFOCOM 2005](http://crawdad.org/cambridge/haggle/20090529/)
* [INFOCOM 2006](http://crawdad.org/cambridge/haggle/20090529/)
* [SASSY](http://crawdad.org/st_andrews/sassy/20110603/)
* [SIGCOMM 2009](http://crawdad.org/thlab/sigcomm2009/20120715/)
* [Cambridge](http://crawdad.org/upmc/content/20061117/)
* [RollerNet](http://crawdad.org/upmc/rollernet/20090202/)
* [Nodobo](http://crawdad.org/strath/nodobo/20110323/)
* [UCSD](http://sysnet.ucsd.edu/wtd/)
* [Cabspotting](http://crawdad.org/epfl/mobility/20090224/)
* [Lyon Primary School](http://www.sociopatterns.org/datasets/primary-school-temporal-network-data/)
* [Stanford High School](http://www.salathegroup.com/guide/school_2010.html)
* [DieselNet](http://crawdad.org/umass/diesel/20080914/)
* [Dartmouth](http://crawdad.org/dartmouth/campus/20090909/)

**Supported Routing Protocols**
* [Direct Delivery](http://dx.doi.org/10.1109/TNET.2007.897962)
* [Epidemic Routing](http://issg.cs.duke.edu/epidemic/epidemic.pdf)
* [PRoPHET](https://datatracker.ietf.org/doc/rfc6693/)
* [SimBet](http://dx.doi.org/10.1145/1288107.1288113)
* [SimBetTS](http://dx.doi.org/10.1109/TMC.2008.161)
* [Bubble Rap](http://dx.doi.org/10.1109/TMC.2010.246)
* [Spray and Wait](http://dx.doi.org/10.1145/1080139.1080143)
* [LSF Spray and Wait](http://dx.doi.org/10.1109/TMC.2008.172)
* [MSF Spray and Wait](http://dx.doi.org/10.1109/TMC.2008.172)
* PRoPHET Spray and Wait
* [LSF Spray and Focus](http://dx.doi.org/10.1109/PERCOMW.2007.108)
* Compare and Replicate
* [Encounter-Based Routing](http://dx.doi.org/10.1109/INFCOM.2009.5061994)
* [Delegation Forwarding](http://dx.doi.org/10.1145/1374618.1374653)
* [Coordinated Delegation Forwarding](http://dx.doi.org/10.1109/WiMOB.2014.6962142)
* Optimal Routing

**Supported Congestion Control Mechanisms<sup>[2](#note2)</sup>**
* Avoid Overflow
* [Autonomous Congestion Control (ACC)](http://dx.doi.org/10.2514/6.2006-5970)
* [FairRoute (FR)](http://dx.doi.org/10.1109/INFCOM.2009.5061993)
* [Buffer Space Advertisements (BSA)](http://dx.doi.org/10.1007/978-3-642-21560-5_32)
* [Context-Aware Congestion Control (CACC)](http://dx.doi.org/10.1109/MCOM.2014.6829943)
* [Congestion Control with Adjustable Fairness (CCAF)](http://dx.doi.org/10.1109/WiMOB.2014.6962204)

**Supported Scheduling Policies<sup>[2](#note2)</sup>**
* [FIFO](http://dx.doi.org/10.1145/1409985.1409994)
* [LIFO](http://dx.doi.org/10.1145/1409985.1409994)
* [GRTRMax](http://dx.doi.org/10.1109/COMSWA.2006.1665196)
* [GRTRSort](http://dx.doi.org/10.1109/COMSWA.2006.1665196)
* HNUV

**Supported Dropping Policies**
* [Drop Front](http://dx.doi.org/10.1145/1409985.1409994)
* [Drop Tail](http://dx.doi.org/10.1145/1409985.1409994)
* [Drop Oldest](http://dx.doi.org/10.1109/ISWC.2001.962117)
* Drop Youngest
* Drop Front - Avoid Source Packets
* Drop Tail - Avoid Source Packets
* Drop Oldest - Avoid Source Packets
* Drop Youngest - Avoid Source Packets

**Supported Deletion Mechanisms<sup>[3](#note3)</sup>**
* [Just TTL](http://dx.doi.org/10.1109/TNET.2005.863461)
* No Duplicates - Delete Replicas
* No Duplicates - Keep Replicating
* [Vaccine](http://dx.doi.org/10.1109/TNET.2005.863461)
* Cataclysm

<a name="note1">1</a>: Note that the original traces are not directly supported by Adyton. Further processing is required for each trace in order to be compatible with Adyton. More instructions about trace processing can be found in the utils/trace-processing directory. For any questions or special requests please contact us.

<a name="note2">2</a>: Currently all congestion control mechanisms and some scheduling policies are supported only by single-copy routing protocols. Their use in multi-copy protocols is still experimental.

<a name="note3">3</a>: Deletion mechanisms are used only in the case of multi-copy routing protocols where multiple packet replicas exist throughout the network.


Installation
------------

**Requirements**

Adyton currently supports only Linux-based systems with a proper C++ compiler.
All the required third-party libraries are included in the ext/ directory.

**Compiling**

To compile Adyton from source code, just execute the following command inside the src/ directory:
```
$ make
```

**Execution**

After compiling Adyton from source code, you can use the executable file to run a simulation with the default settings with the following command:
```
$ ./Adyton
```


Configuration
-------------

**Syntax**

```
$ ./Adyton -TRACE <contact trace> -RT <routing protocol> -CC <congestion control mechanism> -SP <scheduling policy> -DP <dropping policy> -DM <deletion mechanism> -OUTPUT <output type> -TRAFFIC_TYPE <traffic type> -TRAFFIC_LOAD <number of packets> -TTL <time to live> -BUFFER <buffer size> -REP <replication number> -SPLIT <number of pieces> -SEED <seed number> -PROFILE <text file> -RES_DIR <results directory> -TRC_DIR <trace directory> -BATCH <batch mode>
```

**Options**

```<contact trace>``` options:

| Contact Trace              | Arithmetic Value | String Value       |
| -------------              | ---------------- | ------------       |
| Milan PMTR                 | 1                | MILAN              |
| MIT Reality Mining         | 2                | REALITY            |
| Intel                      | 3                | INTEL              |
| Haggle-Cam                 | 4                | HAGGLE-CAM         |
| INFOCOM 2005               | 5                | INFOCOM-2005       |
| INFOCOM 2006               | 6                | INFOCOM-2006       |
| INFOCOM 2006 (Short Range) | 7                | INFOCOM-2006-SR    |
| SASSY                      | 8                | SASSY              |
| SIGCOMM 2009               | 9                | SIGCOMM-2009       |
| Cambridge                  | 10               | CAMBRIDGE          |
| Cambridge Students         | 11               | CAMBRIDGE-STUDENTS |
| RollerNet                  | 12               | ROLLERNET          |
| Nodobo                     | 13               | NODOBO             |
| UCSD                       | 14               | UCSD               |
| UCSD (3 Weeks)             | 15               | UCSD-3-WEEKS       |
| Cabspotting                | 16               | CABS               |
| Cabspotting (2 Days)       | 17               | CABS-2-DAYS        |
| Lyon Primary School        | 18               | LYON               |
| Stanford High School       | 19               | STANFORD           |
| DieselNet                  | 20               | DIESELNET          |
| Dartmouth                  | 21               | DARTMOUTH          |
| Dartmouth (2 Weeks)        | 22               | DARTMOUTH-2-WEEKS  |
| Debugging                  | 23               | DEBUG              |

```<routing protocol>``` options:

| Routing Protocol                  | Arithmetic Value | String Value |
| ----------------                  | ---------------- | ------------ |
| Direct Delivery                   | 1                | DIRECT       |
| Epidemic Routing                  | 2                | EPIDEMIC     |
| PRoPHET                           | 3                | PROPHET      |
| SimBet                            | 4                | SIMBET       |
| SimBetTS                          | 5                | SIMBETTS     |
| Bubble Rap                        | 6                | BUBBLE       |
| Spray and Wait                    | 7                | SNW          |
| LSF Spray and Wait                | 8                | LSF-SNW      |
| MSF Spray and Wait                | 9                | MSF-SNW      |
| PRoPHET Spray and Wait            | 10               | PROPHET-SNW  |
| LSF Spray and Focus               | 11               | LSF-SNF      |
| Compare and Replicate             | 12               | CNR          |
| Encounter-Based Routing           | 13               | EBR          |
| Delegation Forwarding             | 14               | DF           |
| Coordinated Delegation Forwarding | 15               | COORD        |
| Optimal Routing                   | 16               | OPTIMAL      |

```<congestion control mechanism>``` options: 

| Congestion Control Mechanism | Arithmetic Value | String Value   |
| ---------------------------- | ---------------- | ------------   |
| None                         | 1                | NONE           |
| Avoid Overflow               | 2                | AVOID-OVERFLOW |
| ACC                          | 3                | ACC            |
| FairRoute                    | 4                | FR             |
| BSA                          | 5                | BSA            |
| CACC                         | 6                | CACC           |
| CCAF                         | 7                | CCAF           |

```<scheduling policy>``` options:

| Scheduling Policy | Arithmetic Value | String Value |
| ----------------- | ---------------- | ------------ |
| FIFO              | 1                | FIFO         |
| LIFO              | 2                | LIFO         |
| GRTRMax           | 3                | GRTRMAX      |
| GRTRSort          | 4                | GRTRSORT     |
| HNUV              | 5                | HNUV         |

```<dropping policy>``` options:

| Dropping Policy                      | Arithmetic Value | String Value |
| ---------------                      | ---------------- | ------------ |
| Drop Front                           | 1                | DF           |
| Drop Tail                            | 2                | DT           |
| Drop Oldest                          | 3                | DO           |
| Drop Youngest                        | 4                | DY           |
| Drop Front - Avoid Source Packets    | 5                | DF-ASP       |
| Drop Tail - Avoid Source Packets     | 6                | DT-ASP       |
| Drop Oldest - Avoid Source Packets   | 7                | SO-ASP       |
| Drop Youngest - Avoid Source Packets | 8                | DY-ASP       |

```<deletion mechanism>``` options:

| Deletion Mechanism               | Arithmetic Value | String Value  |
| ------------------               | ---------------- | ------------  |
| Just TTL                         | 1                | JUST-TTL      |
| No Duplicates - Delete Replicas  | 2                | NO-DUPS       |
| No Duplicates - Keep Replicating | 3                | KEEP-REP      |
| Vaccine                          | 4                | VACCINE       |
| Cataclysm                        | 5                | CATACLYSM     |

```<output type>``` options:

| Output Type | Arithmetic Value | String Value |
| ----------- | ---------------- | ------------ |
| Type 1      | 1                | T1           |
| Type 2      | 2                | T2           |
| Type 3      | 3                | T3           |
| Type 4      | 4                | T4           |
| Type 5      | 5                | T5           |

```<traffic type>``` options:

| Traffic Type | Arithmetic Value | String Value |
| ------------ | ---------------- | ------------ |
| Uniform      | 1                | UNIFORM      |
| Burst        | 2                | BURST        |
| Sample       | 3                | SAMPLE       |
| Predefined   | 4                | PREDEFINED   |

```<number of packets>``` is the number of packets that will be created.

```<time to live>``` is defined as a timeout value, which specifies when a packet is no longer useful and should be deleted (if <time to live> is equal to 0, the TTL value will be infinite).

```<buffer size>``` is defined as the maximum number of packets that each node can store (if <buffer size> is equal to 0, infinite buffers will be used).

```<replication number>``` is the replicas of each original packet (used only for multi-copy protocols).

```<number of pieces>``` is the number of pieces to split the contact trace file, before uploading to memory.

```<seed number>``` is used by the pseudo-random number generator algorithm.

```<text file>``` is the name of a text file with additional parameter arguments.

```<results directory>``` is the name of the directory that will store the results of the simulation.

```<trace directory>``` is the name of the directory that stores the contact trace files.

```<batch mode>``` options:
* ON = the results will not be printed
* OFF = all the results will be printed


**Default Settings**

* ```<contact trace>``` = MILAN
* ```<routing protocol>``` = DIRECT
* ```<congestion control mechanism>``` = NONE
* ```<scheduling policy>``` = FIFO
* ```<dropping policy>``` = DF
* ```<deletion mechanism>``` = JUST-TTL
* ```<output type>``` = Τ1
* ```<traffic type>``` = UNIFORM
* ```<number of packets>``` = 10000
* ```<time to live>``` = 0
* ```<buffer size>``` = 0
* ```<replication number>``` = 1
* ```<number of pieces>``` = 100
* ```<seed number>``` = time(NULL)
* ```<text file>``` = ""
* ```<results directory>``` = "../res/"
* ```<trace directory>``` = "../trc/"
* ```<batch mode>``` = OFF


How to cite Adyton
------------------

When you write a paper that involves the use of Adyton, we would appreciate it if you can cite our tool using the following entry.

* N. Papanikos, D.-G. Akestoridis, E. Papapetrou, "Adyton: A network simulator for opportunistic networks", 2015. [Online]. Available: https://github.com/npapanik/Adyton.

* BibTeX entry: 
>@misc{Adyton,  
>	author = {Papanikos, Nikolaos and Akestoridis, Dimitrios-Georgios and Papapetrou, Evangelos},  
>	title = {Adyton: A Network Simulator for Opportunistic Networks},  
>	howpublished = {[Online]. Available: https://github.com/npapanik/Adyton},  
>	year = {2015}  
>}


Publications
------------

Adyton has been used in the following publications:

* D.-G. Akestoridis, N. Papanikos, and E. Papapetrou, "Exploiting Social Preferences for Congestion Control in Opportunistic Networks", in _Proceedings of the 10th IEEE International Conference on Wireless and Mobile Computing, Networking and Communications (WiMob)_, 2014, pp. 413-418. DOI: [10.1109/WiMOB.2014.6962204](http://dx.doi.org/10.1109/WiMOB.2014.6962204).
* N. Papanikos, and E. Papapetrou, "Coordinating Replication Decisions in Multi-copy Routing for Opportunistic Networks", in _Proceedings of the 10th IEEE International Conference on Wireless and Mobile Computing, Networking and Communications (WiMob)_, 2014, pp. 8-13. DOI: [10.1109/WiMOB.2014.6962142](http://dx.doi.org/10.1109/WiMOB.2014.6962142).


License
-------

Copyright (C) 2015  Nikolaos Papanikos, Dimitrios-Georgios Akestoridis,
and Evangelos Papapetrou

Adyton is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Adyton is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Adyton.  If not, see <http://www.gnu.org/licenses/>.
