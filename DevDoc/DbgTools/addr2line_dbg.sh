#!/bin/bash

cut $1 -d : -f 4 | cut -d ' ' -f 8-10 > tempx8x9

#while read line
#do
#    echo $line
#done < tempx8x9

while read line
do
     #echo $line
    NUM=`echo $line | cut -d ' ' -f 1`
    #echo $NUM
    F=`echo $line | cut -d ' ' -f 2 | cut -d '/' -f 4`
    #echo $F
    #echo addr2line -e ../../Targets/rdk/arm.gnu.android.dbg/bin/dbg_info/$F $NUM
    addr2line -e ../../Out/target/arm.clang.android.elastos.dbg/bin/dbg_info/$F $NUM >> crashinfo
done < tempx8x9

rm tempx8x9

while read line
do
    echo $line
done < crashinfo
