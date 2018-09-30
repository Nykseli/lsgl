#!/bin/bash

PID=`ps axuf|awk '/[n]yan/{print $2}'`

while(true)
do
    timedatectl | awk 'NR==1 {printf "%s %s\n", $4, $5}'
    pmap -d $PID | tail -1
    sleep 1
    echo
done
