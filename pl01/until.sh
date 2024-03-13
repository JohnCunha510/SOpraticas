#!/bin/sh
until [ $# -le 0 ]
do
echo $1
shift
done
