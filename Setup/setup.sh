#!/bin/sh

# create desktop for env

for i in `ls -b ./Shortcuts | grep .desktop$`
do
  cp -f $PWD/Shortcuts/$i ~/Desktop
  if [ `echo $i | grep SDK` ]; then
    sed "s#~/ElastosSDK/Setup#$PWD#g" -i ~/Desktop/$i
  else
    sed "s#~/ElastosRDK5_0/Setup#$PWD#g" -i ~/Desktop/$i
  fi
done

# create Init for env
for i in `ls -b | grep _Init$`
do
  if [ `echo $i | grep SDK` ]; then
    sed "s#~/ElastosSDK/Setup#$PWD#g" -i $PWD/$i
  else
    sed "s#~/ElastosRDK5_0/Setup#$PWD#g" -i $PWD/$i
  fi
done
