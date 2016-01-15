#!/bin/bash
uuiddata=`uuidgen`
echo "// {"$uuiddata"}"

data1=`echo $uuiddata | awk -F '-' '{print $1}'`
data2=`echo $uuiddata | awk -F '-' '{print $2}'`
data3=`echo $uuiddata | awk -F '-' '{print $3}'`
datamid1=`echo $uuiddata | awk -F '-' '{print $4}'`
data4=`echo $datamid1 | cut -c 1-2`
data5=`echo $datamid1 | cut -c 3-4`
datamid2=`echo $uuiddata | awk -F '-' '{print $5}'`
data6=`echo $datamid2 | cut -c 1-2`
data7=`echo $datamid2 | cut -c 3-4`
data8=`echo $datamid2 | cut -c 5-6`
data9=`echo $datamid2 | cut -c 7-8`
data10=`echo $datamid2 | cut -c 9-10`
data11=`echo $datamid2 | cut -c 11-12`

printf "extern \"C\" const InterfaceID EIID_XXXXXX =
        { 0x%s, 0x%s, 0x%s, { 0x%s, 0x%s, 0x%s, 0x%s, 0x%s, 0x%s, 0x%s, 0x%s } };\n" $data1 $data2 $data3 $data4 $data5 $data6 $data7 $data8 $data9 $data10 $data11
