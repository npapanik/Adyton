####Introduction

This README is a short description of the steps required to import the Cabspotting dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton.


####Getting the original Cabspotting dataset from CRAWDAD

In order to get the original Cabspotting dataset, you should join the CRAWDAD community.
The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html).
After creating a CRAWDAD account, you will be able to download the Cabspotting dataset [here](http://crawdad.org//download/epfl/mobility/cabspottingdata.tar.gz).
Save the compressed file to "Adyton/utils/trace-processing/Cabspotting/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Cabspotting/original
$ tar -xvzf cabspottingdata.tar.gz
```
A directory named "cabspottingdata" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Cabspotting/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Cabspotting/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the Cabspotting contact trace.
