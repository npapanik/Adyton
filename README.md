Adyton
======

Adyton is an event-driven network simulator, written in C++, for Opportunistic Networks (a.k.a. Delay-Tolerant Networks) that is capable of processing contact traces. The Adyton simulator includes a plethora of routing protocols and real-world contact traces. It also provides several congestion control mechanisms and buffer management policies.

Adyton was developed by [Nikolaos Papanikos](http://cse.uoi.gr/~npapanik) and [Dimitrios-Georgios Akestoridis](http://cse.uoi.gr/~dakestor) during their graduate studies in the [Department of Computer Science and Engineering at University of Ioannina, Greece](http://cse.uoi.gr) under the supervision of Prof. [Evangelos Papapetrou](http://cse.uoi.gr/~epap).

For more information about installing and configuring Adyton check our [wiki](https://github.com/npapanik/Adyton/wiki).

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
* Compare and Forward
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

<a name="note1">1</a>: Due to copyright restrictions, most contact traces cannot be directly included in Adyton. Instead, Adyton provides automated tools for processing and downloading them from the corresponding online sources. Further details about these tools can be found in the utils/trace-processing directory. For any questions or special requests please contact us.

<a name="note2">2</a>: Currently all congestion control mechanisms and some scheduling policies are supported only by single-copy routing protocols. Their use in multi-copy protocols is still experimental.

<a name="note3">3</a>: Deletion mechanisms are used only in the case of multi-copy routing protocols where multiple packet replicas exist throughout the network.


How to cite Adyton
------------------

When you write a paper that involves the use of Adyton, we would appreciate it if you can cite our tool using the following entry.

* N. Papanikos, D.-G. Akestoridis, and E. Papapetrou, "Adyton: A network simulator for opportunistic networks," [Online]. Available: <https://github.com/npapanik/Adyton>, 2018.

```
@misc{Adyton,
    author = {Papanikos, Nikolaos and Akestoridis, Dimitrios-Georgios and Papapetrou, Evangelos},
    title = {Adyton: A Network Simulator for Opportunistic Networks},
    howpublished = {[Online]. Available: \url{https://github.com/npapanik/Adyton}},
    year = {2018}
}
```


Publications
------------

Adyton has been used by its developers in the following publications:

* D.-G. Akestoridis, N. Papanikos, and E. Papapetrou, "Exploiting social preferences for congestion control in opportunistic networks," in _Proceedings of the 10th IEEE International Conference on Wireless and Mobile Computing, Networking and Communications (WiMob)_, 2014, pp. 413-418. DOI: [10.1109/WiMOB.2014.6962204](http://dx.doi.org/10.1109/WiMOB.2014.6962204).
* N. Papanikos and E. Papapetrou, "Coordinating replication decisions in multi-copy routing for opportunistic networks," in _Proceedings of the 10th IEEE International Conference on Wireless and Mobile Computing, Networking and Communications (WiMob)_, 2014, pp. 8-13. DOI: [10.1109/WiMOB.2014.6962142](http://dx.doi.org/10.1109/WiMOB.2014.6962142).

Other researchers have used Adyton in the following publications:

* J. Dede, A. Förster, E. Hernández-Orallo, J. Herrera-Tapia, K. Kuladinithi, V. Kuppusamy, P. Manzoni, A. bin Muslim, A. Udugama, and Z. Vatandas, "Simulating opportunistic networks: Survey and future directions," _IEEE Communications Surveys & Tutorials_, 2017. DOI: [10.1109/COMST.2017.2782182](http://dx.doi.org/10.1109/COMST.2017.2782182).
* Y. Li and R. Bartos, "Interaction based routing algorithm for opportunistic mobile social networks," in _Proceedings of the 14th IEEE Annual Consumer Communications & Networking Conference (CCNC)_, 2017, pp. 492-497. DOI: [10.1109/CCNC.2017.7983157](http://dx.doi.org/10.1109/CCNC.2017.7983157).


License
-------

Copyright (C) 2015-2018  Nikolaos Papanikos,
Dimitrios-Georgios Akestoridis, and Evangelos Papapetrou

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
