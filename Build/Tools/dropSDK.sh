#!/bin/bash

#set ELASTOS SDK PATH and mkdir
if [ -n "$1" ]; then
    ELASTOS_SDK_PATH=$1
else
    ELASTOS_SDK_PATH=${XDK_ROOT%/*}
fi

ELASTOS_SDK=$ELASTOS_SDK_PATH/Elastos.SDK

if [ -d "$ELASTOS_SDK" ]; then
    rm -rf $ELASTOS_SDK
fi

mkdir -p $ELASTOS_SDK
mkdir -p $ELASTOS_SDK/Sources
mkdir -p $ELASTOS_SDK/Targets

#start copy ENV and Tools stripe from Elastos RDK to SDK
TARGET_DIR="Build Setup"

for target in $TARGET_DIR; do
  cp -rf $XDK_ROOT/$target $ELASTOS_SDK
done

mv $ELASTOS_SDK/Build/Makefiles/makefile_rdk.mk $ELASTOS_SDK/Build/Makefiles/makefile_sdk.mk
mv $ELASTOS_SDK/Build/Makefiles/makedefs_gnu_rdk.mk $ELASTOS_SDK/Build/Makefiles/makedefs_gnu_sdk.mk
mv $ELASTOS_SDK/Build/Makefiles/makerules_gnu_rdk.mk $ELASTOS_SDK/Build/Makefiles/makerules_gnu_sdk.mk
mv $ELASTOS_SDK/Setup/Arm_AndroidForElastosSDK_Init_ $ELASTOS_SDK/Setup/Arm_AndroidForElastosSDK_Init
mv $ELASTOS_SDK/Setup/Shortcuts/Arm_AndroidForElastosSDK.desktop_ $ELASTOS_SDK/Setup/Shortcuts/Arm_AndroidForElastosSDK.desktop


#start copy inc lib and eco files from Elastos RDK  to SDK
mkdir -p $ELASTOS_SDK/Build/Prebuilt/RDK/$XDK_BUILD_KIND

cp -rf $XDK_USER_INC $ELASTOS_SDK/Build/Prebuilt/RDK/$XDK_BUILD_KIND
cp -rf $XDK_USER_LIB $ELASTOS_SDK/Build/Prebuilt/RDK/$XDK_BUILD_KIND

while read -u3 TARGET TARGET_TYPE;
do
    if [ "`expr index "$TARGET" ";"`" == "0" ]; then
        if [ ! "$TARGET_TYPE" == "dir" ]; then
            cp -f $XDK_TARGETS/$TARGET $ELASTOS_SDK/Build/Prebuilt/RDK/$XDK_BUILD_KIND/lib
        else
            if [ ! "$TARGET" == "Snowfox" ] && [ ! "$TARGET" == "XBMC" ]; then 
                cp -rf $XDK_TARGETS/$TARGET $ELASTOS_SDK/Build/Prebuilt/RDK/$XDK_BUILD_KIND/lib/$TARGET
            fi
        fi
    fi
done 3<$XDK_TOOLS/eldrop.list
cp -f $XDK_TARGETS/ElastosCore.cls  $ELASTOS_SDK/Build/Prebuilt/RDK/$XDK_BUILD_KIND/lib

#Clean the unused files an dirs in SDK
while read -u3 TARGET TARGET_DIR;
do
    if [ "`expr index "$TARGET" ";"`" == "0" ]; then
        rm -f $ELASTOS_SDK/$TARGET_DIR/$TARGET
    fi
done 3<$XDK_TOOLS/clean4SDK.list

#Copy samples to SDK and modify Sources and other files to let it build at SDK env
TARGET_SAMPLES="ImageViewDemo TextViewDemo Gallery MusicPlayer"
for target in $TARGET_SAMPLES; do
  cp -rf $XDK_ROOT/Sources/Elastos/Frameworks/Droid/DevSamples/$target $ELASTOS_SDK/Sources
  sed "s#/../../sources.inc#/../sources.inc#g" -i $ELASTOS_SDK/Sources/$target/sources
  sed "s#XDK_USER_LIB#XDK_LIB_PATH#g" -i $ELASTOS_SDK/Sources/$target/sources
  sed "s#XDK_TARGETS#XDK_LIB_PATH#g" -i $ELASTOS_SDK/Sources/$target/sources
done
cp -f $XDK_ROOT/Sources/Elastos/Frameworks/Droid/sources.inc $ELASTOS_SDK/Sources

#sed "s#XDK_TARGETS#XDK_LIB_PATH#g" -i $ELASTOS_SDK/Build/Tools/eldrop.sh
#sed "/ dir/d" -i $ELASTOS_SDK/Build/Tools/eldrop.list
sed "/Snowfox dir/d" -i $ELASTOS_SDK/Build/Tools/eldrop.list
sed "/XBMC dir/d" -i $ELASTOS_SDK/Build/Tools/eldrop.list

echo Drop SDK Finish.
