####Introduction

This README is a short description of the steps required to import the Infocom 2006 dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original Infocom 2006 dataset from CRAWDAD

In order to get the original Infocom 2006 dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the Infocom 2006 dataset [here](http://crawdad.org//download/cambridge/haggle/Exp6.tar.gz). Save the compressed file to "Adyton/utils/trace-processing/Infocom06/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Infocom06/original
$ tar -xvzf Exp6.tar.gz
```
A directory named "Exp3" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Infocom06/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Infocom06/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script may require a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Infocom06/processing
$ cp Infocom06.txt path/to/Adyton/trc/.
$ cp Infocom06-presence.txt path/to/Adyton/trc/.
$ cp Infocom06-SR.txt path/to/Adyton/trc/.
$ cp Infocom06-SR-presence.txt path/to/Adyton/trc/.
```

Note that the output file "Infocom06.txt" contains the contacts occured between all participating nodes, while the output file "Infocom06-SR.txt" contains the contacts occured between short range devices (nodes with IDs from 1 to 17 are excluded).
