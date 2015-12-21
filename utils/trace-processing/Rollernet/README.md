####Introduction

This README is a short description of the steps required to import the Rollernet dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original Rollerner dataset from CRAWDAD

In order to get the original Rollernet dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the Rollernet dataset [here](http://crawdad.org/download/upmc/rollernet/imote-traces-RollerNet.tar.gz). Save the compressed file to "Adyton/utils/trace-processing/Rollernet/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Rollernet/original
$ tar -xvzf imote-traces-RollerNet.tar.gz
```
A directory named "imote-traces-RollerNet" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Rollernet/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Rollernet/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Rollernet/processing
$ cp Rollernet.txt path/to/Adyton/trc/.
$ cp Rollernet-presence.txt path/to/Adyton/trc/.
```
