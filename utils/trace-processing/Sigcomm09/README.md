####Introduction

This README is a short description of the steps required to import the Sigcomm 2009 dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original Sigcomm 2009 dataset from CRAWDAD

In order to get the original Sigcomm 2009 dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the Sigcomm 2009 dataset [here](http://crawdad.org//download/thlab/sigcomm2009/sigcomm2009.tar.gz). Save the compressed file to "Adyton/utils/trace-processing/Sigcomm09/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Sigcomm09/original
$ tar -xvzf sigcomm2009.tar.gz
```
A directory named "Exp3" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Sigcomm09/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Sigcomm09/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Sigcomm09/processing
$ cp Sigcomm09.txt path/to/Adyton/trc/.
$ cp Sigcomm09-presence.txt path/to/Adyton/trc/.
```
