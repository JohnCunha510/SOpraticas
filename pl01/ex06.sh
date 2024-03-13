#!/bin/sh

horas="$(date +%H):$(date +%M):$(date +%S)"
echo "Time: $horas"
ts=$((($(date +%H) * 3600) + ($(date +%M) * 60) + $(date +%S)))
tsn=$((86400 - ts))
echo "Only $tsn seconds until midnight \n"
tsnp=$tsn

while true
do
horas="$(date +%H):$(date +%M):$(date +%S)"
ts=$((($(date +%H) * 3600) + ($(date +%M) * 60) + $(date +%S)))
tsn=$((86400 - ts))
if [ $tsn -ne $tsnp ]
then
echo "Time: $horas"
echo "Only $tsn seconds until midnight \n"
tsnp=$tsn
fi
done
