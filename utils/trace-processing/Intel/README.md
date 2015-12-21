####Introduction

This README is a short description of the steps required to import the Intel dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original Intel dataset from CRAWDAD

In order to get the original Intel dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the Intel dataset [here](http://crawdad.org//download/cambridge/haggle/imote-trace1.tar.gz). Save the compressed file to "Adyton/utils/trace-processing/Intel/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Intel/original
$ tar -xvzf imote-trace1.tar.gz
```
A directory named "Exp1" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Intel/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Intel/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Intel/processing
$ cp Intel.txt path/to/Adyton/trc/.
$ cp Intel-presence.txt path/to/Adyton/trc/.
```
