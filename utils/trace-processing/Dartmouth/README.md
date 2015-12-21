####Introduction

This README is a short description of the steps required to import the Dartmouth dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton.


####Getting the original Dartmouth dataset from CRAWDAD

In order to get the original Dartmouth dataset, you should join the CRAWDAD community.
The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html).
After creating a CRAWDAD account, you will be able to download the Dartmouth dataset [here](http://crawdad.org//download/dartmouth/campus/movement/movement-v1.3.tar.gz).
Save the compressed file to "Adyton/utils/trace-processing/Dartmouth/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Dartmouth/original
$ tar -xvzf movement-v1.3.tar.gz
```
A directory named "movement" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Dartmouth/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Dartmouth/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the Dartmouth contact trace.
