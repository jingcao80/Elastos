#!/bin/bash

TARGET_DIR=$XDK_TARGETS/data/data/com.elastos.runtime/elastos

if [ -d "$TARGET_DIR" ]; then rm -rf $XDK_TARGETS/data; fi
if [ -f "$XDK_TARGETS/userdata.img" ]; then rm -f $XDK_TARGETS/userdata.img; fi
if [ -f "$XDK_TARGETS/userdata.img.tar" ]; then rm -f $XDK_TARGETS/userdata.img.tar; fi
mkdir -p $TARGET_DIR

echo "Start to copy source files to target dir"
while read -u3 LINE;
do
    if [ "`expr index "$LINE" ";"`" == "0" ]; then
        TARGET=${LINE% *}
        TARGET_TYPE=${LINE#* }
        if [ "$TARGET_TYPE" == "dir" ]; then
            if [ -d $XDK_TARGETS/$TARGET ]; then
                cp -r $XDK_TARGETS/$TARGET $TARGET_DIR
            else
                NOT_EXIST=$TARGET
            fi
         else
            if [ -f $XDK_TARGETS/$TARGET ]; then
                cp $XDK_TARGETS/$TARGET $TARGET_DIR
            else
                NOT_EXIST=$TARGET
            fi
        fi
    fi
    if [ "$NOT_EXIST" ]; then break;fi
done 3<$XDK_TOOLS/eldrop.list

if [ "$NOT_EXIST" ]; then
    rm -rf $XDK_TARGETS/data
    echo "Sorry, $NOT_EXIST is not exist, Please check!"
else
    echo "Start to make userdata.img and tar"
    $XDK_TOOLS/mkyaffs2image $XDK_TARGETS/data $XDK_TARGETS/userdata.img
    pushd $XDK_TARGETS
    tar cvf userdata.img.tar userdata.img
    popd
    echo "finish."
fi
