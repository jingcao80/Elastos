#!/bin/bash

if [ "$1" == '-h' ]; then
    echo "Usage: SetEnv.sh {elastos_android_arm | devtools_linux_x86}"
    echo ""
    echo "  elastos_android_arm"
    echo "                 setup elastos development environment."
    echo "  devtools_linux_x86"
    echo "                 setup elastos devtools development environment."
    return
else
    ################################################################################
    # Set DevKit path

    IFS='_'; ARGS=($1); unset IFS;
    TARGET=${ARGS[0]}
    PLATFORM=${ARGS[1]}
    HARDWARE=${ARGS[2]}

    if [ "$TARGET" == "elastos" ]; then
        export XDK_BUILD_ENV=target
    elif [ "$TARGET" == "devtools" ]; then
        export XDK_BUILD_ENV=host
    else
        echo "Unknow build target '$TARGET'."
        return
    fi

    if [ "$PLATFORM" != "android" ] && [ "$PLATFORM" != "linux" ]; then
        echo "Unknow build platform '$PLATFORM'."
        return
    fi

    if [ "$HARDWARE" != "arm" ] && [ "$HARDWARE" != "x86" ]; then
        echo "Unknow build target hardware '$HARDWARE'."
        return
    fi

    export XDK_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

    if [[ ! -f $XDK_ROOT/Setup/Config/$1.sh ]]; then
        echo "Error: $XDK_SETUP_PATH/Config/$1.sh is not exist."
        unset XDK_BUILD_ENV XDK_TARGET_PLATFORM XDK_TARGET_CPU XDK_ROOT
        return
    fi
    ################################################################################
    # Clean WINDOWS path
    source $XDK_ROOT/Setup/CleanEnv.sh

    export XDK_SETUP_PATH=$XDK_ROOT/Setup
    export XDK_BUILD_PATH=$XDK_ROOT/Build
    export XDK_SOURCE_PATH=$XDK_ROOT/Sources
    export XDK_TARGETS_PATH=$XDK_ROOT/Out
    export XDK_TEMP=$XDK_TARGETS_PATH/Temp

    ################################################################################
    # Set user defined environment variables if the user config file exist
    source $XDK_SETUP_PATH/Config/$1.sh

    # Set command and build-tool lookup path
    export XDK_TOOLS=$XDK_BUILD_PATH/Tools
    #export XDK_COMMANDES=$XDK_ROOT/Commandes
    export PATH=.:$XDK_TOOLS:$PATH:
    #export PATH=.:$XDK_TOOLS:$XDK_COMMANDES:

    # Set user defined environment variables if the user config file exists
    if [ -f $XDK_SETUP_PATH/UserDefined.sh ]; then
        source $XDK_SETUP_PATH/UserDefined.sh
    fi

    if [ -f $XDK_SETUP_PATH/Config/$XDK_TARGET_HARDWARE.sh ]; then
        source $XDK_SETUP_PATH/Config/$XDK_TARGET_HARDWARE.sh
    fi

    source $XDK_TOOLS/env_tools.sh

    if [ -z "$XDK_VERSION" ]; then
        export XDK_VERSION=dbg
    fi

    unset DEBUG_INFO

    if [ ! "$XDK_VERSION" == "rls" ]; then
        export DEBUG_INFO=1
    fi

    # Set target object environment variables
    if [ -z "$XDK_TARGET_PRODUCT" ]; then
        export XDK_BUILD_KIND=$XDK_TARGET_CPU.$XDK_COMPILER.$XDK_TARGET_PLATFORM.$XDK_VERSION
    else
        export XDK_BUILD_KIND=$XDK_TARGET_CPU.$XDK_COMPILER.$XDK_TARGET_PLATFORM.$XDK_TARGET_PRODUCT.$XDK_VERSION
    fi
    export XDK_USER_OBJ=$XDK_TARGETS_PATH/$XDK_BUILD_ENV/$XDK_BUILD_KIND/obj
    export XDK_USER_INC=$XDK_USER_OBJ/inc
    export XDK_USER_LIB=$XDK_USER_OBJ/lib
    export XDK_TARGETS=$XDK_TARGETS_PATH/$XDK_BUILD_ENV/$XDK_BUILD_KIND/bin
    export TARGET_PACK_PATH=$XDK_TARGETS/packages

    # Finally add Ubuntu System to the path, so we could use system tools
    PATH_LIST=${PATH//\:/ }
    for line in $PATH_LIST; do
        if [ ! `echo $line | grep \/android\/` ]; then
            if [ -z "$PATH_FILTEROUT_ANDROID" ]; then
               PATH_FILTEROUT_ANDROID="$line"
            else
               PATH_FILTEROUT_ANDROID="$PATH_FILTEROUT_ANDROID:$line"
            fi
        fi
    done

    export PATH=$PATH_FILTEROUT_ANDROID
    export XDK_GENERAL_PATH=$PATH_FILTEROUT_ANDROID

    unset PATH_LIST
    unset PATH_FILTEROUT_ANDROID

    export PATH=$PATH:$XDK_TARGETS

    cd $XDK_SOURCE_PATH

    #set title
    if [ -z "$XDK_TARGET_PRODUCT" ]; then
        TITLE=$XDK_TARGET_PLATFORM-$XDK_TARGET_CPU.$XDK_COMPILER-$XDK_VERSION
    else
        TITLE=$XDK_TARGET_PLATFORM-$XDK_TARGET_CPU.$XDK_COMPILER-$XDK_TARGET_PRODUCT-$XDK_VERSION
    fi
    export PS1='\[\e]0;$TITLE \w\a\]${debian_chroot:+($debian_chroot)}\w\$ '
    #clear
fi
