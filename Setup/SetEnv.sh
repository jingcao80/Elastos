if [ "$1" == '-h' ]; then
    echo Usage
    echo
    echo
    echo
    echo
    echo
    echo
    echo
    echo
    return
else
    ################################################################################
    # Set DevKit path

    if [ "$1" == "sdk" ]; then
        export XDK_BUILD_ENV=sdk
        shift
    else
        export XDK_BUILD_ENV=rdk
    fi

    export XDK_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

    if [[ ! -f $XDK_ROOT/Setup/Config/$1.sh ]]; then
        echo "Error: $XDK_SETUP_PATH/Config/$1.sh is not exist."
        unset XDK_BUILD_ENV XDK_ROOT
        return
    fi
    ################################################################################
    # Clean WINDOWS path
    source $XDK_ROOT/Setup/CleanEnv.sh

    export XDK_SETUP_PATH=$XDK_ROOT/Setup
    export XDK_BUILD_PATH=$XDK_ROOT/Build
    export XDK_SOURCE_PATH=$XDK_ROOT/Sources
    export XDK_TARGETS_PATH=$XDK_ROOT/Targets
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
        if [ -f  $XDK_ROOT/Setup/UserDefined.sh ]; then
            source $XDK_ROOT/Setup/UserDefined.sh
        fi

        if [ -z $2 ]; then
            export XDK_TARGET_HARDWARE=generic
        else
            export XDK_TARGET_HARDWARE=$2
        fi

        if [ ! -f $XDK_SETUP_PATH/Config/$XDK_TARGET_HARDWARE.sh ]; then
            echo
            echo "Error: $XDK_SETUP_PATH/Config/$XDK_TARGET_HARDWARE.sh doesn't exist."
        else
            source $XDK_SETUP_PATH/Config/$XDK_TARGET_HARDWARE.sh

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
            export XDK_USER_OBJ=$XDK_TARGETS_PATH/obj/$XDK_BUILD_ENV
            export XDK_USER_INC=$XDK_USER_OBJ/$XDK_BUILD_KIND/inc
            export XDK_USER_LIB=$XDK_USER_OBJ/$XDK_BUILD_KIND/lib
            export XDK_TARGETS=$XDK_TARGETS_PATH/$XDK_BUILD_ENV/$XDK_BUILD_KIND/bin
            export TARGET_PACK_PATH=$XDK_USER_OBJ/$XDK_BUILD_KIND/package

            if [ "$XDK_BUILD_ENV" == "sdk" ]; then
                export XDK_INC_PATH=$XDK_BUILD_PATH/Prebuilt/RDK/$XDK_BUILD_KIND/inc
                export XDK_LIB_PATH=$XDK_BUILD_PATH/Prebuilt/RDK/$XDK_BUILD_KIND/lib
            fi

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

           if [ "$XDK_BUILD_ENV" == "sdk" ]; then
                export PATH=$PATH:$XDK_LIB_PATH:$XDK_TARGETS
            else
                export PATH=$PATH:$XDK_TARGETS
            fi

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
fi
