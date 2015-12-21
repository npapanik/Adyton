####Introduction

This README is a short description of the steps required to import the Haggle-Cam dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original Haggle-Cam dataset from CRAWDAD

In order to get the original Haggle-Cam dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the Haggle-Cam dataset [here](http://crawdad.org//download/cambridge/haggle/imote-trace2.tar.gz). Save the compressed file to "Adyton/utils/trace-processing/Haggle-Cam/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Haggle-Cam/original
$ tar -xvzf imote-trace2.tar.gz
```
A directory named "Exp2" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Haggle-Cam/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Haggle-Cam/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Haggle-Cam/processing
$ cp HaggleCam.txt path/to/Adyton/trc/.
$ cp HaggleCam-presence.txt path/to/Adyton/trc/.
```
