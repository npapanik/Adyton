####Introduction

This README is a short tutorial describing how to use the "caravida.sh" script. Initially, the script fetches the original trace file(s) from the web (mainly from [CRAWDAD](http://crawdad.org/)). Then, it processes the downloaded file(s) in order to make them compatible with Adyton: A Network Simulator for Opportunistic Networks.

####Requirements

To successfully use the "import-dataset.sh" script the following tools must be installed.
* gcc compiler
* awk
* bc
* wget
* md5sum
* MySQL (only for the MIT Reality trace)
* dialog (only for -gui option)

Commonly used linux distributions, e.g., Ubuntu, come with all the required tools already installed. To get access in most of the original datasets you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html).

####Processing one or more traces

To process a particular trace file use the following commands in the linux terminal.
```
$ cd "path_to_Adyton"/utils/trace-processing/
$ chmod +x caravida.sh
$ ./caravida.sh <trace_name>
```
To process more than one trace use the same command, but using more options.
```
$ ./caravida.sh <trace_name_1> <trace_name_2> .. <trace_name_N>
```
To process all the supported traces, execute:
```
$ ./caravida.sh -all
```
To clean all temp files created during the processing, execute:
```
$ ./caravida.sh -clean
```

####Running with a gui

Also, a gui version of the tool is available through the command:
```
$ ./caravida.sh -gui
``` 

####Traces supported

Currently, the "caravida.sh" script supports the following list of traces.

---------------------------------------------
| trace/dataset  | Estimated Execution Time |
-----------------|--------------------------|
| REALITY        | 20-25 minutes            |
| INFOCOM05      | 15-30 seconds            |
| INFOCOM06      | 40-50 seconds            |
| SIGCOMM09      | 10-15 minutes            |
| HAGGLE-CAM     | 15-20 seconds            |
| INTEL          | 15-20 seconds            |
| ROLLERNET      | 20-25 seconds            |
| SASSY          | 20-25 seconds            |
| CAMBRIDGE      | 20-25 seconds            |
| MILANO         | 15-20 seconds            |
| CABSPOTTING    | 3.5-4 hours              |
| DARTMOUTH      | 30-35 minutes            |
| DIESELNET      | 20-25 seconds            |
| NODOBO         | 5-10 minutes             |
| LYON           | 40-50 minutes            |
| STANFORD       | 10-15 minutes            |
| UCSD           | 11-12 hours              |
---------------------------------------------

