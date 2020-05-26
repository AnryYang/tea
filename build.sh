#########################################################################
# File Name: build.sh
# Author: anryyang
# mail: anryyang@gmail.com
# Created Time: Fri 29 Sep 2017 08:30:44 PM
#########################################################################
#!/bin/bash

cmake .
make clean all -j16
