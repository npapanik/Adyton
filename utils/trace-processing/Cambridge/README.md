####Introduction

This README is a short description of the steps required to import the Cambridge dataset (downloaded from [CRAWDAD](http://crawdad.org/)) in Adyton.


####Getting the original Cambridge dataset from CRAWDAD

In order to get the original Cambridge dataset, you should join the CRAWDAD community.
The CRAWDAD registration is free and can be completed through the [CRAWDAD registration page](http://crawdad.org/joinup.html).
After creating a CRAWDAD account, you will be able to download the Cambridge dataset [here](http://crawdad.org//download/upmc/content/imote-traces-cambridge.tar.gz).
Save the compressed file to "Adyton/utils/trace-processing/Cambridge/original" directory.
To extract the contents of the compressed file use the following command in the linux terminal.
```
$ cd path/to/Adyton/utils/trace-processing/Cambridge/original
$ tar -xvzf imote-traces-cambridge.tar.gz
```
A directory named "imote-traces-cambridge" will be created.


####Processing the extracted contact trace

Inside "Adyton/utils/trace-processing/Cambridge/processing" directory execute the "process.sh" script.
```
$ cd path/to/Adyton/utils/trace-processing/Cambridge/processing
$ chmod +x process.sh
$ ./process.sh
```
This will create the required files for the Cambridge and Cambridge Students contact traces.
