####Introduction

This README is a short description of the steps required to import the PMTR-Milan dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original PMTR-Milan dataset from CRAWDAD

In order to get the original PMTR-Milan dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the PMTR-Milan dataset [here](http://crawdad.org//download/unimi/pmtr/pmtr.txt.gz). Save the compressed file to "Adyton/utils/trace-processing/PMTR-Milan/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/PMTR-Milan/original
$ gzip -d pmtr.txt.gz
```
The downloaded file is in tab-separated format.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/PMTR-Milan/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/PMTR-Milan/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/PMTR-Milan/processing
$ cp Milano.txt path/to/Adyton/trc/.
$ cp Milano-presence.txt path/to/Adyton/trc/.
```
