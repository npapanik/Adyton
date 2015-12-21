####Introduction

This README is a short description of the steps required to import the Stanford dataset (downloaded from [the Salathe Group](http://www.salathegroup.com/)) in Adyton.


####Getting the original Stanford dataset from the Salathe Group

The Stanford dataset is freely available for download [here](http://www.salathegroup.com/data/school_2010/moteFiles.zip).
Save the compressed file to "Adyton/utils/trace-processing/Stanford/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Stanford/original
$ unzip moteFiles.zip
```
A directory named "moteFiles" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Stanford/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Stanford/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the Stanford contact trace.
