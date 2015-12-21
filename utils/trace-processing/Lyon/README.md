####Introduction

This README is a short description of the steps required to import the Lyon dataset (downloaded from [the SocioPatterns project](http://www.sociopatterns.org/)) in Adyton.


####Getting the original Lyon dataset from the SocioPatterns project

The Lyon dataset is freely available for download [here](http://www.sociopatterns.org/wp-content/uploads/2015/09/primaryschool.csv.gz).
Save the compressed file to "Adyton/utils/trace-processing/Lyon/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Lyon/original
$ gunzip -c primaryschool.csv.gz > primaryschool.csv
```
A file named "primaryschool.csv" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Lyon/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Lyon/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the Lyon contact trace.
