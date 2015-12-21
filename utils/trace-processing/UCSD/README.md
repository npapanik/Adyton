####Introduction

This README is a short description of the steps required to import the UCSD dataset (downloaded from [the Wireless Topology Discovery (WTD) project](http://sysnet.ucsd.edu/wtd/)) in Adyton.


####Getting the original UCSD dataset from the Wireless Topology Discovery (WTD) project

The UCSD dataset is freely available for download [here](http://sysnet.ucsd.edu/wtd/data_download/wtd_data_release.tgz).
Save the compressed file to "Adyton/utils/trace-processing/UCSD/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/UCSD/original
$ tar -xvzf wtd_data_release.tgz
```
A directory named "wtd_data_release" will be created.
Inside this directory, you will find another compressed file named "wtd.csv.gz".
To extract its contents use the following command in the linux terminal.
```
$ cd wtd_data_release
$ gunzip -c wtd.csv.gz > wtd.csv
```


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/UCSD/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/UCSD/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the UCSD contact trace.
