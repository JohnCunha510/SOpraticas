#!/bin/sh
if [ $# -eq 1 ]
then
if [ '$1'=='-l' ]
then
ls -p $pwd
fi
else
ls -1 $pwd
fi
