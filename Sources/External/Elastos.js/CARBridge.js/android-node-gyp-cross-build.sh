#!/bin/bash

if [ -z "$2" ]; then
    ARCH=arm
else
    ARCH="$2"
fi

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

export TOOLCHAIN=$1/android-toolchain

export PATH=$TOOLCHAIN/bin:$PATH

export AR=$TOOLCHAIN/bin/$SUFFIX-ar
export CC=$TOOLCHAIN/bin/$SUFFIX-gcc
export CXX=$TOOLCHAIN/bin/$SUFFIX-g++
export LINK=$TOOLCHAIN/bin/$SUFFIX-g++

node-gyp \
    clean \
    configure --arch=$ARCH --dest-cpu=$DEST_CPU --dest-os=android --without-snapshot \
    --nodedir=/home/kevin/workspace/Kortide/node.js.native \
    build

