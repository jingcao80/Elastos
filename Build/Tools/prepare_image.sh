if [ "$1" == "-h" ]; then
    echo "Usage:"
    echo "Copy Elastos before android making pack system.img"
    exit 0
fi

TARGET_PATH=$XDK_ROOT/../android/out/target/product/sugar-evb

if [ "$1" ]; then
    if [[ -f "$1/out/target/product/sugar-evb/system.img" ]]; then
        TARGET_PATH=$1/out/target/product/sugar-evb
    else
        echo "Make sure the path are correct!"
        exit -1
    fi
fi
TARGET_ELASTOS_PATH=$TARGET_PATH/data/data/com.elastos.runtime/elastos
if [ ! -d "$TARGET_ELASTOS_PATH" ]; then mkdir -p $TARGET_ELASTOS_PATH; fi
if [ ! -d "$TARGET_PATH/data/app" ]; then mkdir -p $TARGET_PATH/data/app; fi
if [ ! -d "$TARGET_PATH/system/app" ]; then mkdir -p $TARGET_PATH/system/app; fi
while read -u3 TARGET TARGET_DIR;
do
    if [ "`expr index "$TARGET" ";"`" == "0" ]; then
         cp -f $XDK_BUILD_PATH/Prebuilt/JavaFramwork/$TARGET $TARGET_PATH/$TARGET_DIR
    fi
done 3<$XDK_TOOLS/eldrop_java.list
while read -u3 TARGET TARGET_DIR;
do
    if [ "`expr index "$TARGET" ";"`" == "0" ]; then
        cp -f $XDK_TARGETS/$TARGET $TARGET_PATH/$TARGET_DIR
    fi
done 3<$XDK_TOOLS/eldrop_epk.list
while read -u3 TARGET TARGET_TYPE;
do
    if [ "`expr index "$TARGET" ";"`" == "0" ]; then
        if [ "$TARGET_TYPE" == "dir" ]; then
            cp -rf $XDK_TARGETS/$TARGET $TARGET_ELASTOS_PATH
        else
            if [ "${TARGET:0-4:4}" == ".eco" ]; then
                cp -f $XDK_TARGETS/$TARGET $TARGET_PATH/system/lib
            elif [ "$TARGET" == "ServiceManager" ]; then
                cp -f $XDK_TARGETS/ServiceManager $TARGET_PATH/system/bin
            elif [ "$TARGET" == "ElApp_process" ]; then
                cp -f $XDK_TARGETS/ElApp_process $TARGET_PATH/system/bin
            else
                cp -f $XDK_TARGETS/$TARGET $TARGET_ELASTOS_PATH
            fi
        fi
   fi
done 3<$XDK_TOOLS/eldrop.list
if [ -d "$TARGET_PATH/data/app" ]; then 
    rm -rf $TARGET_PATH/data/app 
    mkdir -p $TARGET_PATH/data/app 
fi
