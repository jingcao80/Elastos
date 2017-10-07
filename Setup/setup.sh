#!/bin/bash

# create desktop for env
for i in `ls -b ./Shortcuts | grep .desktop$`
do
  cp -f $PWD/Shortcuts/$i ~/Desktop
  sed "s#ELASTOS_ROOT_PATH/Setup#$PWD#g" -i ~/Desktop/$i
done

# create Init for env
for i in `ls -b | grep _Init$`
do
  sed "s#ELASTOS_ROOT_PATH/Setup#$PWD#g" -i $PWD/$i
done
