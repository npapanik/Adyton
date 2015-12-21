####Introduction

This README is a short description of the steps required to import the Nodobo dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton.


####Getting the original Nodobo dataset from CRAWDAD

In order to get the original Nodobo dataset, you should join the CRAWDAD community.
The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html).
After creating a CRAWDAD account, you will be able to download the Nodobo dataset [here](http://crawdad.org//download/strath/nodobo/nodobo-csv.zip).
Save the compressed file to "Adyton/utils/trace-processing/Nodobo/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Nodobo/original
$ unzip nodobo-csv.zip
```
A directory named "csv" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Nodobo/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Nodobo/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the Nodobo contact trace.
