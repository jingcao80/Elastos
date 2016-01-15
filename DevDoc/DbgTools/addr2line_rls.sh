#!/bin/bash

cut $1 -d : -f 2 | cut -d ' ' -f 9-11 > tempx8x9

while read line
do
    echo $line

done < tempx8x9


while read line
do
     #echo $line
    NUM=`echo $line | cut -d ' ' -f 1`
     #echo $NUM
    F=`echo $line | cut -d ' ' -f 2 | cut -d '/' -f 4`
     #echo $F
    arm-linux-androideabi-addr2line -e ../../Targets/rdk/arm.gnu.android.rls/bin/dbg_info/$F $NUM

done < tempx8x9

rm tempx8x9
