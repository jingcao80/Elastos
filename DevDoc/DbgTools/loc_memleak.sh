#!/bin/bash

PWDAA=`pwd`
FIRSTFILE=$PWDAA/first.txt
SECONDFILE=$PWDAA/second.txt
DIFFFILE=$PWDAA/diff.txt
LOGCATFILE=$PWDAA/logcat.txt
ADDR2LINE=$PWDAA/addr2line.txt
FORMATTEREXE=$XDK_ROOT/DevDoc/DbgTools/MemDumpFormatter
RESULTFILE=$PWDAA/result.txt

DEBUGFILE_ANDROID=/data/debug/memo_dump.txt
LOGCATFILE_ANDROID=/data/debug/result.txt
DIFFFILE_ANDROID=/data/debug/diff.txt
TARGETPID_ANDROID=
TARGETNAME_ANDROID=BackupManagerDemo

source $XDK_SETUP_PATH/SetEnv.sh arm_android

function pre_begin()
{
    adb shell setprop libc.debug.malloc 1
    adb shell stop
    adb shell start
}

function pull_file()
{
    echo "pull the file , and then rename to $1"
    if [ "X$1" = "Xfirst" ]; then
        adb pull $DEBUGFILE_ANDROID $FIRSTFILE;
    elif [ "X$1" = "Xsecond" ]; then
        adb pull $DEBUGFILE_ANDROID $SECONDFILE;
    else
        echo "unknown command";
        usage;exit;
    fi
}

function wait_input()
{
    while true; do
        read  -p "Please run multiple times the target program, and then start the task(yes or no):" Arg
        case $Arg in
            Y|y|YES|yes)
                break;;
            N|n|NO|no)
                exit;;
            "")
                break;;
        esac
    done
}

function diff_file()
{
    echo "now diff the two files"
    diff $FIRSTFILE $SECONDFILE > $DIFFFILE
    adb push $DIFFFILE /data/debug
}

function target_pid()
{
    while true; do
        read -p "Please input the target program's NAME(such as: SystemUI PinyinIME):" TARGETNAME_ANDROID
        TARGETPID_ANDROID=`adb shell ps | grep -w $TARGETNAME_ANDROID | awk '{print $2}'`
        echo "Your input name is $TARGETNAME_ANDROID PID:$TARGETPID_ANDROID"
        if [ "X$TARGETPID_ANDROID" = "X" ]; then
            echo "The target program's NAME is error, Please input again\n"
        else
            break;
        fi
        done
}

function mem_dummper()
{
    echo "now mem_dummper, wait....."
    #local ELPID
    #if [ "X$1" = "X" ]; then
        #ELPID=`adb shell ps | grep BackupManagerDemo | awk '{print $2}'`
    #else
        #ELPID=`adb shell ps | grep $1 | awk '{print $2}'`

    #fi
    logcat_dump
    adb shell /data/debug/MemoryDumper $TARGETPID_ANDROID
    kill_logcat
    adb pull $LOGCATFILE_ANDROID $LOGCATFILE
}

function memdump_formatter()
{
    echo "now dump formatter -u -n"
    #-u : unique, no repeat
    #-m : number bigger than 1
    #-n : order by number
    $FORMATTEREXE -u -m -n $LOGCATFILE $ADDR2LINE
    #$FORMATTEREXE -u -n $LOGCATFILE $ADDR2LINE
}

function logcat_dump()
{
    adb logcat -c
    adb logcat -f $LOGCATFILE_ANDROID &
}
function kill_logcat()
{
    LOGCATPID=`adb shell ps | grep logcat | awk '{print $2}'`
    adb shell kill -9 $LOGCATPID
}

function find_memleak()
{
    echo "now find the memleak"
    if [ -f $RESULTFILE ]; then
        rm $RESULTFILE
    fi
    while read line
    do
        FIRST=`echo $line | awk '{print $1}'`
        if [ "X$FIRST" != "Xarm-linux-androideabi-addr2line" ]; then
            echo "$line" | tee -a $RESULTFILE
        else
            #cd $XDK_TARGETS/dbg_info
            #$line
            NUM=`echo $line | awk '{print $4}'`
            F=`echo $line | awk '{print $3}'`
            #echo "arm-linux-androideabi-addr2line -e $XDK_TARGETS/dbg_info/$F $NUM " 2>&1 | tee $RESULTFILE
            arm-linux-androideabi-addr2line -e $XDK_TARGETS/dbg_info/$F $NUM 2>&1 | tee -a $RESULTFILE
        fi
    done < $ADDR2LINE
}

function set_env()
{
    if [ -f $FIRSTFILE ]; then
        rm $FIRSTFILE
    fi
    if [ -f $SECONDFILE ]; then
        rm $SECONDFILE
    fi
    if [ -f $DIFFFILE ]; then
        rm $DIFFFILE
    fi
    if [ -f $LOGCATFILE ]; then
        rm $LOGCATFILE
    fi
    if [ -f $ADDR2LINE ]; then
        rm $ADDR2LINE
    fi
    adb shell rm $DEBUGFILE_ANDROID
    adb shell rm $LOGCATFILE_ANDROID
    adb shell rm $DIFFFILE_ANDROID
}

function first_runtarget()
{
    while true; do
        read  -p "Please run the target program, and then start the task(yes or no):" Arg
        case $Arg in
            Y|y|YES|yes)
                break;;
            N|n|NO|no)
                exit;;
            "")
                break;;
        esac
    done
}

function all()
{
    set_env
    first_runtarget
    pull_file "first"
    target_pid
    wait_input
    pull_file "second"
    diff_file
    mem_dummper
    memdump_formatter
    find_memleak
}
function usage()
{
    echo "`basename $0` [-f|-s]";
    echo "  -p      pre the begin"
    echo "  -f      pull the fist time"
    echo "  -s      pull the second time"
    echo "  -exe    run the MemDumpFormatter, when run this command, we have $LOGCATFILE,"
    echo "          and this command is designed to produce $ADDR2LINE"
    echo "  -find   find the mem_leak , when run this command"
    echo "          we have $ADDR2LINE"
    echo "  -all    run this script all"
}

echo "mem-leak test begin"

for moption
do
    case $moption in
        "-p") pre_begin;break
            ;;
        "-f") pull_file "first";break
            ;;
        "-s") pull_file "second";break
            ;;
        "-d") diff_file;break
            ;;
        "-exe") memdump_formatter;break
            ;;

        "-find") find_memleak;break
            ;;

        "-all") all;break
            ;;

        "-h") usage;;
        *)
            if ["X$moption" != "X"]; then
                usage;exit;
            fi
            ;;
    esac
done
