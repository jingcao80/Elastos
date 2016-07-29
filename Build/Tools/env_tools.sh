#!/bin/bash

get_android_serialno ()
{
    if [ -f  /tmp/adb_devices_list.txt ]; then
        rm -f  /tmp/adb_devices_list.txt
    fi
    adb devices | sed -n 's/\(.*\)\tdevice$/\1/p'> /tmp/adb_devices_list.txt
    if [ -s  /tmp/adb_devices_list.txt ]; then
        unset AVD_LIST ANDROID_AVD
        while read -u3 LINE;
        do
            if [ ! -z "$LINE" ]; then
                if [ -z "$AVD_LIST" ]; then
                    export AVD_LIST=($LINE)
                else
                    export AVD_LIST=($AVD_LIST $LINE)
                fi
            fi
        done 3< /tmp/adb_devices_list.txt
        if [ -z "$ANDROID_AVD" ]; then
            if [ "{AVD_LIST[@]}" != "emulator-5554" ]; then
                if [ "${#AVD_LIST[@]}" == "1" ]; then
                    export ANDROID_AVD=$AVD_LIST
                else
                    if [ -z "$AVD_ID" ]; then
                        select AVD_ID in ${AVD_LIST[@]} EXIT
                        do
                            break
                        done
                    fi
                    if [ -z "$AVD_ID" ]; then
                        echo "The select number is wrong!"
                    elif [ ! "$AVD_ID" == "EXIT" ]; then
                        export ANDROID_AVD=$AVD_ID
                    fi
                    unset AVD_ID
                 fi
            else
                export ANDROID_AVD=emulator-5554
            fi
        else
            AVD_NUMBER=${ANDROID_AVD/emulator-/}
            if [ ! "${#AVD_NUMBER}" == "4" ]; then
                echo "The Android Emulator serial number is wrong, please check!"
                unset ANDROID_AVD AVD_NUMBER
            else
                if [ ! `sed -e "/^\`echo $ANDROID_AVD\`/=" -n  /tmp/adb_devices_list.txt` ]; then
                    echo "The Android Emulator serial number is wrong, please check!"
                    unset ANDROID_AVD AVD_NUMBER
                fi
            fi
        fi
    else
        echo "Android Emulator not start!"
        if [ -n "$ANDROID_AVD" ]; then
            unset ANDROID_AVD
        fi
    fi
}

change_version ()
{
    if [ "$1" == "dbg" ]; then
        export XDK_VERSION=dbg
    fi
#    if [ "$1" == "opt" ]; then
#        export XDK_VERSION=opt
#    fi
#    if [ "$1" == "pre" ]; then
#        export XDK_VERSION=pre
#    fi
    if [ "$1" == "rls" ]; then
        export XDK_VERSION=rls
    fi
    reset_env
}

reset_env ()
{
   export PATH=$XDK_GENERAL_PATH

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
   export XDK_USER_OBJ=$XDK_TARGETS_PATH/obj/$XDK_BUILD_ENV
   export XDK_USER_INC=$XDK_USER_OBJ/$XDK_BUILD_KIND/inc
   export XDK_USER_LIB=$XDK_USER_OBJ/$XDK_BUILD_KIND/lib
   export XDK_TARGETS=$XDK_TARGETS_PATH/$XDK_BUILD_ENV/$XDK_BUILD_KIND/bin
   export TARGET_PACK_PATH=$XDK_USER_OBJ/$XDK_BUILD_KIND/package

   export PATH=$PATH:$XDK_TARGETS:

#   if [ -d $XDK_DEFAULT_PATH ]; then
#      cd $XDK_DEFAULT_PATH
#   else
#      cd /d $XDK_SOURCE_PATH
#      export XDK_DEFAULT_PATH=$XDK_SOURCE_PATH
#   fi
   #set title
   if [ -z "$XDK_TARGET_PRODUCT" ]; then
       local TITLE=$XDK_TARGET_PLATFORM-$XDK_TARGET_CPU.$XDK_COMPILER-$XDK_VERSION
   else
       local TITLE=$XDK_TARGET_PLATFORM-$XDK_TARGET_CPU.$XDK_COMPILER-$XDK_TARGET_PRODUCT-$XDK_VERSION
   fi
   export PS1="\[\e]0;$TITLE \w\a\]${debian_chroot:+($debian_chroot)}\w\$"
}

makedir ()
{
    local RMD="adb -s $ANDROID_AVD shell mkdir"
    $RMD /data/data/com.elastos.runtime 1>/dev/null 2>/dev/null
    $RMD /data/data/com.elastos.runtime/elastos 1>/dev/null 2>/dev/null
    $RMD /data/data/com.elastos.runtime/app 1>/dev/null 2>/dev/null
}

set_for_android ()
{
    DEST_PATH=/data/data/com.elastos.runtime/elastos
    WAIT_DEV="adb -s $ANDROID_AVD wait-for-device"
    RCP="adb -s $ANDROID_AVD push"
    RCX="adb -s $ANDROID_AVD shell chmod 777"
    RCD="adb -s $ANDROID_AVD shell chmod -R 777"
}

start_copy ()
{

    # adb push do not support wildcards
    for i in $1; do
        echo "Copying $i to $DEST..."
        $RCP $i "$DEST_HOST$DEST"
    done
    if [ ! "$RCX" == "" ]; then
        TARGET_FILE=$1
        TARGET_FILE_NAME=${TARGET_FILE##*/}
        if [ -f "$TARGET_FILE" ]; then
            if [ "${TARGET_FILE_NAME##*.}" == "$TARGET_FILE_NAME" ]; then
                $RCX "$DEST/$TARGET_FILE_NAME"
            elif [ ! "${TARGET_FILE_NAME##*.}" == "eco" ]; then
                if [ ! "${TARGET_FILE_NAME##*.}" == "cls" ]; then
                    $RCX "$DEST/$TARGET_FILE_NAME"
                fi
            fi
        elif [ -d "$TARGET_FILE" ]; then
            $RCD "$DEST"
        fi
    fi
}

function elrsh ()
{
    if [ "$XDK_TARGET_PLATFORM-$XDK_TARGET_CPU" == "android-arm" ]; then
        if [ "${1:0:9}" == "emulator-" ]; then
            ANDROID_AVD="$1"
        fi
        get_android_serialno
        if [ -n "$ANDROID_AVD" ]; then
            (gnome-terminal -t "elrsh" -e "$EMULATOR_PATH/platform-tools/adb -s $ANDROID_AVD shell" &)
            unset ANDROID_AVD
        fi
    else
        echo ERROR: Do not support this environment.
    fi
}

function elcopy ()
{
    case "$1" in
    "-h")
        echo "Usage: %~n0 Source [DestFolder]"
        echo "Copy local Elastos files to remote $XDK_TARGET_PLATFORM host"
        echo
        echo  " Source       PC file[s] or folder to be copied"
        echo  " DestFolder   Optional remote folder (Default: $DEST_PATH)"
        echo
        echo "Copy files:"
        echo   "%~n0 MPlayer.dll         Copy one file to $DestFolder"
        echo   "%~n0 *.mp3 Jukebox       Copy more files to $DestFolder/Jukebox"
        echo
        echo "Copy folder:"
        echo "  %~n0 Wallpapers          Copy folder to $DestFolder"
        echo "  %~n0 Wallpapers Gallery  Copy folder to $DestFolder/Gallery"
        ;;
    "")
        echo "ERROR: \$1 is not exist."
        ;;
    *)
        if [ -z "$ANDROID_AVD" ]; then
            if [ "${1:0:9}" == "emulator-" ]; then
                export ANDROID_AVD="$1"
                shift
            fi
            get_android_serialno
        fi
        if [ -n "$ANDROID_AVD" ]; then
            $WAIT_DEV
            if [ -z $MAKE_DIR ]; then
                makedir
            fi
            if [ -z "$SET_FOR_ANDROID" ]; then
                set_for_android
            fi
            local DEST=$2
            if [ -z "$DEST" ]; then
                DEST=$DEST_PATH
            else
                if [ ! "${DEST:0:1}" == "/" ]; then
                    DEST=$DEST_PATH/$DEST
                fi
            fi
            start_copy $1
        fi
        if [ -z "$SET_FOR_ANDROID" ]; then
            unset DEST_PATH WAIT_DEV RCP RCX
        fi
        if [ -z "$ANDROID_AVD_TAG" ]; then
            unset ANDROID_AVD
        fi
        ;;
    esac
}

function eldrop ()
{
    if [ "$1" == "-h" ]; then
        echo "Usage:"
        echo "Deploy Elastos Runtime to remote $XDK_TARGET_PLATFORM host"
    elif [ "$1" == "java" ]; then
        get_android_serialno
        if [ -n "$ANDROID_AVD" ]; then
            ANDROID_AVD_TAG=1
            makedir
            MAKE_DIR=1
            set_for_android
            SET_FOR_ANDROID=1
            while read -u3 LINE;
            do
                if [ "`expr index "$LINE" ";"`" == "0" ]; then
                    TARGET=${LINE% *}
                    TARGET_DIR=${LINE#* }
                    elcopy $XDK_BUILD_PATH/Prebuilt/JavaFramework/$TARGET $TARGET_DIR
                fi
            done 3<$XDK_TOOLS/eldrop_java.list
            unset ANDROID_AVD_TAG MAKE_DIR SET_FOR_ANDROID ANDROID_AVD
        fi
    elif [ "$1" == "r1s" ]; then
        get_android_serialno
        if [ -n "$ANDROID_AVD" ]; then
            ANDROID_AVD_TAG=1
            makedir
            MAKE_DIR=1
            set_for_android
            SET_FOR_ANDROID=1
            while read -u3 LINE;
            do
                if [ "`expr index "$LINE" ";"`" == "0" ]; then
                    TARGET=${LINE% *}
                    TARGET_DIR=${LINE#* }
                    elcopy $XDK_BUILD_PATH/Prebuilt/JavaFramwork/r1s/$TARGET $TARGET_DIR
                fi
            done 3<$XDK_TOOLS/eldrop_r1s.list
            unset ANDROID_AVD_TAG MAKE_DIR SET_FOR_ANDROID ANDROID_AVD
        fi
    elif [ "$1" == "epk" ]; then
        get_android_serialno
        if [ -n "$ANDROID_AVD" ]; then
            ANDROID_AVD_TAG=1
            makedir
            MAKE_DIR=1
            set_for_android
            SET_FOR_ANDROID=1
            while read -u3 LINE;
            do
                if [ "`expr index "$LINE" ";"`" == "0" ]; then
                    TARGET=${LINE% *}
                    TARGET_DIR=${LINE#* }
                    elcopy $TARGET_PACK_PATH/$TARGET $TARGET_DIR/$TARGET
                fi
            done 3<$XDK_TOOLS/eldrop_epk.list
            unset ANDROID_AVD_TAG MAKE_DIR SET_FOR_ANDROID ANDROID_AVD
        fi
    elif [ "$1" == "apps" ]; then
        get_android_serialno
        if [ -n "$ANDROID_AVD" ]; then
            ANDROID_AVD_TAG=1
            makedir
            MAKE_DIR=1
            set_for_android
            SET_FOR_ANDROID=1
            while read -u3 LINE;
            do
                if [ "`expr index "$LINE" ";"`" == "0" ]; then
                    TARGET=${LINE% *}
                    if [ "$TARGET_TYPE" == "dir" ]; then
                        elcopy $XDK_TARGETS/$TARGET $TARGET
                     else
                        elcopy $XDK_TARGETS/$TARGET
                    fi
                fi
            done 3<$XDK_TOOLS/eldrop_apps.list
            unset ANDROID_AVD_TAG MAKE_DIR SET_FOR_ANDROID ANDROID_AVD
        fi
    elif [ -z "$1" ]; then
        get_android_serialno
        if [ -n "$ANDROID_AVD" ]; then
            ANDROID_AVD_TAG=1
            makedir
            MAKE_DIR=1
            set_for_android
            SET_FOR_ANDROID=1
            while read -u3 TARGET TARGET_TYPE;
            do
                if [ "`expr index "$TARGET" ";"`" == "0" ]; then
                    if [ "$XDK_BUILD_ENV" == "rdk" ]; then
                        if [ "$TARGET_TYPE" == "dir" ]; then
                            elcopy $XDK_TARGETS/$TARGET $TARGET
                        else
                            if [ "${TARGET:0-4:4}" == ".eco" ]; then
                                elcopy $XDK_TARGETS/$TARGET /system/lib
                            elif [ "$TARGET" == "elservicemanager" ]; then
                                elcopy $XDK_TARGETS/elservicemanager /system/bin
                            elif [ "$TARGET" == "ElApp_process" ]; then
                                elcopy $XDK_TARGETS/ElApp_process /system/bin
                            elif [ "$TARGET" == "pm" ]; then
                                elcopy $XDK_TARGETS/pm /system/bin
                            else
                                elcopy $XDK_TARGETS/$TARGET
                            fi
                        fi
                    elif [ "$XDK_BUILD_ENV" == "sdk" ]; then
                        if [ "$TARGET_TYPE" == "dir" ]; then
                            elcopy $XDK_LIB_PATH/$TARGET $TARGET
                        else
                            if [ "${TARGET:0-4:4}" == ".eco" ]; then
                                elcopy $XDK_LIB_PATH/$TARGET /system/lib
                            else
                                elcopy $XDK_LIB_PATH/$TARGET
                            fi
                        fi
                    fi
                fi
            done 3<$XDK_TOOLS/eldrop.list
            unset ANDROID_AVD_TAG MAKE_DIR SET_FOR_ANDROID ANDROID_AVD
        fi
    else
        echo "Error: The parameter is wrong, please check!"
    fi
}

function pd ()
{
    case "$1" in
        "-h")
            echo "usage: pd DIR     (same as PUSHD DIR)"
            echo "       pd         (same as POPD)"
            echo "       pd m       (PUSHD to mirror directory)"
#            echo "       pd p       (PUSHD to android package directory or msvc win32 app directory)"
            echo "       pd @       (PUSHD to target directory)"
            echo "       pd inc     (PUSHD to compiled INC directory)"
            echo "       pd lib     (PUSHD to compiled LIB directory)"
            echo "       pd p       (PUSHD to compiled EPK directory)"
#            echo "       pd cls     (PUSHD to compiled CLS directory)"
        ;;
        "")
            popd
        ;;
        "@")
            pushd $XDK_TARGETS 1>/dev/null
        ;;
        "inc" | "cls" | "lib")
            pushd $XDK_USER_OBJ/$XDK_BUILD_KIND/$1 1>/dev/null
        ;;
        "p")
            pushd $XDK_USER_OBJ/$XDK_BUILD_KIND/package 1>/dev/null
        ;;
        "m")
            local PROJECT_DIR=$PWD
            local XDK_EMAKE_DIR=${PWD/$XDK_SOURCE_PATH/}
            pushd $XDK_USER_OBJ/$XDK_BUILD_KIND/mirror$XDK_EMAKE_DIR 1>/dev/null
        ;;
#        "p")
#            PROJECT_DIR=$PWD
#            temp_dir=`dirname $PROJECT_DIR`
#            XDK_EMAKE_DIR=${PROJECT_DIR/$temp_dir/}
#            if [ "$XDK_TARGET_PLATFORM" = "win32" ]; then
#                if [ -d $XDK_TARGETS$XDK_EMAKE_DIR ]; then
#                    pushd $XDK_TARGETS$XDK_EMAKE_DIR
#                else
#                    pushd $XDK_TARGETS
#                fi
#            elif [ "$XDK_TARGET_PLATFORM" = "openkode" ]; then
#                if [ -d $XDK_USER_OBJ/$XDK_BUILD_KIND/package$XDK_EMAKE_DIR ]; then
#                    pushd $XDK_USER_OBJ/$XDK_BUILD_KIND/package$XDK_EMAKE_DIR
#                else
#                    pushd $XDK_USER_OBJ/$XDK_BUILD_KIND/package
#                fi
#            fi
#        ;;
        *)
            pushd $1 1>/dev/null
        ;;
    esac
}

function chv ()
{
    case "$1" in
        "-h")
            echo "Version:"
            echo "  dbg   "
#            echo "  opt   "
#            echo "  pre   "
            echo "  rls   "
        ;;
        "dbg" | "opt" | "pre" | "rls")
            change_version $1
        ;;
        *)
            echo "Error: No version matched."
        ;;
    esac
}

emu_arm_android_linux ()
{
    if [ "$1" == "-h" ]; then
        echo "Usage: %~n0 [/w] [/s]"
        echo "ARM Emulator for Android"
        echo
        echo "  -h   Show this help"
        echo "  -c   Clean "/data" and "/sdcard" directory"
        echo "  -d   Delete previous session\'s pid file"
        echo "  -w   Wait for its terminate"
    else
        local ANDROID_SH=android
        local QEMU_EXE=$EMULATOR_PATH/tools/emulator
        local QEMU_NAME=ElastosOS421
        local QEMU_DIR=$HOME/.android/avd/$QEMU_NAME.avd
        local QEMU_LOCK=$QEMU_DIR/userdata-qemu.img.lock
        local QEMU_SD=$QEMU_DIR/sdcard.img

        for I in $*; do
            if [ "$I" == "-c" ]; then
                local CLEAN_STORAGE=1
            fi
            if [ "$I" == "-d" ]; then
                rm -f "$QEMU_LOCK" 2>/dev/null
                return
            fi
            if [ "$I" == "-tst" ]; then
                local TEST_FLAG=1
            fi
        done

        if [ ! `android list avd | sed -n "s/^\s*Name: $QEMU_NAME$/1/p"` ]; then
            echo no|$ANDROID_SH create avd -f -c 1024M -t android-21 -n $QEMU_NAME -s WXGA800 1>/dev/null 2>/dev/null
        fi

        if [ -f "$QEMU_LOCK" ]; then
            echo "ERROR: only one session is allowed."
        else
            if [ ! -f "$QEMU_SD" ]; then
                echo "Creating 128MB SD Card image...1"
                mksdcard 128M "$QEMU_SD"
            fi
            if [ ! "$CLEAN_STORAGE" == "" ]; then
                echo "Creating 128MB SD Card image...2"
                mksdcard 128M "$QEMU_SD"
            fi
                local QEMU_FLAG="-avd $QEMU_NAME"
                local QEMU_FLAG="$QEMU_FLAG -memory 1024"
                local QEMU_FLAG="$QEMU_FLAG -show-kernel"
#                local QEMU_FLAG="$QEMU_FLAG -scale 0.8"
                local QEMU_FLAG="$QEMU_FLAG -skin 1200x720 "
                local QEMU_FLAG="$QEMU_FLAG -dynamic-skin"
                if [ "$TEST_FLAG" == "" ]; then
                    local QEMU_FLAG="$QEMU_FLAG -partition-size 1024"
                else
                    local QEMU_FLAG="$QEMU_FLAG -partition-size 2000"
                fi
                if [ ! "$CLEAN_STORAGE" == "" ]; then
                    rm -f "$QEMU_DIR/userdata-qemu.img"
                fi
                (gnome-terminal -t "emu" -e "$QEMU_EXE $QEMU_FLAG" &)
                adb wait-for-device
                adb forward tcp:1234 tcp:1234
        fi
    fi
}

function emu ()
{
    if [ "$XDK_TARGET_PLATFORM-$XDK_TARGET_CPU" == "android-arm" ]; then
        emu_arm_android_linux $*
    else
        echo "ERROR: Do not support Elastos Device Emulator in this environment."
    fi
}

function emake ()
{
    if [ "$XDK_TOOLS" == "" ]; then
        echo "Error# Environment variables XDK_TOOLS not export."
    elif [ "$XDK_TARGET_CPU" == "" ]; then
        echo "Error# Environment variables XDK_TARGET_CPU not export."
    elif [ "$XDK_COMPILER" == "" ]; then
        echo "Error# Environment variables XDK_COMPILER not export."
    else
        # Get the current build path
        local PROJECT_DIR=$PWD

        # Initialize the checking flag
        local XDK_CHECK_ERROR=

        # Skip directory checking result while clobbering or cleaning
        if [ "$1" == "clobber" ]; then
            local XDK_CHECK_ERROR=
        fi

        #Set XDK_EMAKE_DIR
        if [ ! "$XDK_EMAKE_DIR" == "" ]; then
            unset XDK_EMAKE_DIR
        fi
        if [ ! "$XDK_SOURCE_PATH" == "$PWD" ]; then
            export XDK_EMAKE_DIR=/${PWD/$XDK_SOURCE_PATH\//}
        fi

        # Judge whether continue the building process or not
        if [ ! $XDK_CHECK_ERROR == "" ]; then
                unset XDK_EMAKE_DIR
                echo "Error: Can\'t build here. Please go to the project relative directory."
        else
            # Set verbose mode if -v is specified
            if [ "$1" == "/v" ]; then
                export BUILD_VERBOSE=1
                shift
            elif [ "$1" == "-v" ]; then
                export BUILD_VERBOSE=1
                shift
            fi
            # Set test coverage mode if -c is specified
            if [ "$1" == "/c" ]; then
                export TEST_COVERAGE=1
                shift
            elif [ "$1" == "-c" ]; then
                export TEST_COVERAGE=1
                shift
            fi

            if [ "$BUILD_VERBOSE" == "" ]; then
                export XDK_MAKE='make -s'
            else
                export XDK_MAKE='make'
            fi

            export XDK_MAKEFILE=$XDK_BUILD_PATH/Makefiles/makefile_$XDK_BUILD_ENV.mk

            export LD_LIBRARY_PATH=$XDK_BUILD_PATH/Tools

            if [ ! "${1%.car}" == "$1" ]; then
                carc -i -c ${1%.car}.cls $1
                if [ "$?" == "0" ]; then
                    if [ "$XDK_PACKAGE" == "sdk" ]; then
                       if [ -z "$INTERNAL_SDK" ]; then
                         lube -C ${1%.car}.cls -s -T foreground
                       else
                         lube -C ${1%.car}.cls -T foreground
                       fi
                    else
                        lube -C ${1%.car}.cls -T foreground
                    fi
                fi
                if [ -f ${1%.car}.cls ]; then
                    rm -f ${1%.car}.cls
                fi
            elif [ "$1" == "c2j" ]; then
                if [ ! "${2%.car}" == "$2" ]; then
                    carc -i -c ${2%.car}.cls $2
                    if [ ! "$?" == "0" ]; then
                        return
                    fi
                    lube -B $XDK_TOOLS/CAR2Javalube.lbo -C ${2%.car}.cls -T CAR2Java
                    if [ -f ${2%.car}.cls ]; then
                        rm -f ${2%.car}.cls
                    fi
                fi
            else
                if [ "$CLOSE_EMAKE_TIMES" == "1" ]; then
                    $XDK_MAKE -f$XDK_MAKEFILE $1 $2 $3 $4 $5 $6
                else
                    local START_TIME=`date +%s`
                    $XDK_MAKE -f$XDK_MAKEFILE $1 $2 $3 $4 $5 $6
                    local ELAPSED_TIME=$(( `date +%s`-$START_TIME ))
                    local HOURS=`echo $ELAPSED_TIME/3600 | bc`
                    local MINUTES=`echo $ELAPSED_TIME/60%60 | bc`
                    local SECONDS=`echo $ELAPSED_TIME%60 | bc`
                    echo "Build finished, elapsed time: $HOURS Hours, $MINUTES Minutes, $SECONDS Seconds."
                fi
                unset XDK_MAKE XDK_MAKEFILE LD_LIBRARY_PATH XDK_EMAKE_DIR BUILD_VERBOSE TEST_COVERAGE
            fi
        fi
    fi
}

alias dbg='chv dbg'
alias rls='chv rls'
alias cls='clear'
alias rm='rm -i'
alias gdb='gdb.sh $*'
alias setup='pushd $XDK_SETUP_PATH/$1 1>/dev/null'
alias tools='pushd $XDK_TOOLS/$1 1>/dev/null'
alias mk='pushd $XDK_TOOLS/../Makefiles 1>/dev/null'
alias src='pushd $XDK_SOURCE_PATH/$1 1>/dev/null'
alias inc='pd inc 1>/dev/null'
alias lib='pd lib 1>/dev/null'

if [ -f "$XDK_TOOLS/alias_usr.sh" ]; then
    source $XDK_TOOLS/alias_usr.sh
fi
