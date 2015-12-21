####Introduction

This README is a short description of the steps required to import the DieselNet dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton.


####Getting the original DieselNet dataset from CRAWDAD

In order to get the original DieselNet dataset, you should join the CRAWDAD community.
The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html).
After creating a CRAWDAD account, you will be able to download the DieselNet dataset [here](http://crawdad.org//download/umass/diesel/DieselNetTracesSpring2006.tar.gz).
Save the compressed file to "Adyton/utils/trace-processing/DieselNet/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/DieselNet/original
$ tar -xvzf DieselNetTracesSpring2006.tar.gz
```
A directory named "DieselNetTracesSpring2006" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/DieselNet/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/DieselNet/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the DieselNet contact trace.
