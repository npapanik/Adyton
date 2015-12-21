####Introduction

This README is a short description of the steps required to import the MIT Reality dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton: A Network Simulator for Opportunistic Networks.


####Getting the original MIT Reality dataset from CRAWDAD

In order to get the original MIT Reality dataset you should join the CRAWDAD community. The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html). After, creating a CRAWDAD account you will be able to download the MIT Reality dataset [here](http://crawdad.org//download/mit/reality/realitymining.tar.gz). Save the compressed file to "Adyton/utils/trace-processing/Reality/original" directory. To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Reality/original
$ tar -xvzf realitymining.tar.gz
```
The downloaded file is in sql format. For that reason the mysql-server package is going to be used to extract the needed contents that represent the node encounters in a tab-separated text file.


####Installing mysql-server (ubuntu)

First, download and install the mysql-server package.
```
$ sudo apt-get install mysql-server
```
Then, restart mysql service.
```
$ sudo service mysql restart
```


####Extracting the node encounters from the sql file

Inside "Adyton/utils/trace-processing/Reality/original" directory execute the command below to run mysql-server as root user.
```
$ cd path/to/Adyton/utils/trace-processing/Reality/original
$ mysql -u root -p
```
Type the root password defined in the mysql-server installation. Then, create a new database called DATABASE REALITY_TRACE.
```
$mysql> CREATE DATABASE REALITY_TRACE;
```
Executing the command below will show all mysql databases.
```
$mysql> SHOW DATABASES;
```
Select the REALITY_TRACE database.
```
$mysql> USE REALITY_TRACE;
```
Load the sql file downloaded from the CRAWDAD archive.
``` 
$mysql> SOURCE ./dump-anonymized.sql
```
Run the sql query below to get the device encounters into "/tmp/reality.txt".
```
$mysql> SELECT * FROM `devicespan`
     -> INTO OUTFILE '/tmp/reality.txt'
     -> FIELDS TERMINATED BY '\t'
     -> LINES TERMINATED BY '\n';
```
Exit mysql-server.
```
$mysql> exit;
```
Move the file containing the extracted device encounters to the "Adyton/utils/trace-processing/Reality/original" directory.
```
$ cp /tmp/reality.txt path/to/Adyton/utils/trace-processing/Reality/original/.
```


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Reality/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Reality/processing
$ chmod +x process.sh
$ ./process.sh
```
Note that the above script requires a considerable amount of time to be completed. Please be patient :smiley:

The final step is to copy the processed output files to the "Adyton/trc" directory.
```
$ cd path/to/Adyton/utils/trace-processing/Reality/processing
$ cp Reality.txt path/to/Adyton/trc/.
$ cp Reality-presence.txt path/to/Adyton/trc/.
```
