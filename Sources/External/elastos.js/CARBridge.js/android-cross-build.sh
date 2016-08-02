#!/bin/bash

if [ -z "$2" ]; then
    ARCH=arm
else
    ARCH="$2"
fi

CC_VER="4.8"
case $ARCH in
    arm)
        DEST_CPU="$ARCH"
        SUFFIX="$ARCH-linux-androideabi"
        TOOLCHAIN_NAME="$SUFFIX"
        ;;
    x86)
        DEST_CPU="ia32"
        SUFFIX="i686-linux-android"
        TOOLCHAIN_NAME="$ARCH"
        ;;
    x86_64)
        DEST_CPU="ia32"
        SUFFIX="$ARCH-linux-android"
        TOOLCHAIN_NAME="$ARCH"
        ;;
    *)
        echo "Unsupported architecture provided: $ARCH"
        exit 1
        ;;
esac

export TOOLCHAIN=$PWD/android-toolchain
mkdir -p $TOOLCHAIN
if [ -n "$1" ]; then
    $1/build/tools/make-standalone-toolchain.sh \
        --toolchain=$TOOLCHAIN_NAME-$CC_VER \
        --arch=$ARCH \
        --install-dir=$TOOLCHAIN \
        --platform=android-9
fi

export PATH=$TOOLCHAIN/bin:$PATH
export AR=$TOOLCHAIN/bin/$SUFFIX-ar
export CC=$TOOLCHAIN/bin/$SUFFIX-gcc
export CXX=$TOOLCHAIN/bin/$SUFFIX-g++
export LINK=$TOOLCHAIN/bin/$SUFFIX-g++

GYP_DEFINES="target_arch=$ARCH"
GYP_DEFINES+=" v8_target_arch=$ARCH"
GYP_DEFINES+=" android_target_arch=$ARCH"
GYP_DEFINES+=" host_os=linux OS=android"
export GYP_DEFINES

node-gyp \
    clean \
    configure --arch=$ARCH --dest-cpu=$DEST_CPU --dest-os=android --without-snapshot \
    --nodedir=/home/kevin/workspace/Kortide/node.js.native \
    build

