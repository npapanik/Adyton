####Introduction

This README is a short description of the steps required to import the Sassy dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original Sassy dataset from CRAWDAD

In order to get the original Sassy dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the Sassy dataset [here](http://crawdad.org//download/st_andrews/sassy/dsn.csv.gz). Save the compressed file to "Adyton/utils/trace-processing/Sassy/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Sassy/original
$ gzip -d dsn.csv.gz
```
The downloaded file is in tab-separated format.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Sassy/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Sassy/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Sassy/processing
$ cp Sassy.txt path/to/Adyton/trc/.
$ cp Sassy-presence.txt path/to/Adyton/trc/.
```
