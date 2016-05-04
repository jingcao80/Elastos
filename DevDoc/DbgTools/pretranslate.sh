#!/bin/bash
# pretranslate.sh
# pre-translate android java file to Elastos C++ file.
# usages:
# chmod u+x pretranslate.sh
# ./pretranslate.sh srcFilename targetFilename
# or ./pretranslate.sh targetDir

pre_translate () {
    target_file=$1

    # process import
    #
    # import 命名空间替换
    sed -i "s@import java.@import Elastos.@" $target_file
    sed -i "s@import android.@import Elastos.Droid.@" $target_file
    sed -i "s@import com.android.@import Elastos.Droid.@" $target_file
    sed -i "s@import com.android.internal.@import Elastos.Droid.Internal.@" $target_file
    sed -i "s@import org.xmlpull.v1.@import Org.Xmlpull.V1.@" $target_file
    sed -i "s@import libcore.io.@using Libcore.IO.@" $target_file
    sed -i '/^import /s/\.io\./\.IO\./g' $target_file
    sed -i '/^import /s/\.util\./\.Utility\./g' $target_file

    # import 开头的行替换.为::
    sed -i '/^import /s/\./::/g' $target_file
    # import 开头的行替换最后一个::为::I
    sed -i '/^import /s/\(.*\)::\(.*\)/\1::I\2/' $target_file
    # import 开头的行替换::小写为::大写
    sed -i '/^import /s@\(::\)\([a-zA-Z]\w*\)@\1\u\2@g' $target_file
    # import 开头的行替换import 为using
    sed -i 's/^import /using /g' $target_file
     # 删除using 开头Exception;结尾的行
    sed -i '/^using.*Exception;$/d' $target_file

    # process types
    #
    sed -i 's/\<byte\>/Byte/g' $target_file
    sed -i 's/\<boolean\>/Boolean/g' $target_file
    sed -i 's/\<char\>/Char32/g' $target_file
    sed -i 's/\<int\>/Int32/g' $target_file
    sed -i 's/\<long\>/Int64/g' $target_file
    sed -i 's/\<float\>/Float/g' $target_file
    sed -i 's/\<double\>/Double/g' $target_file
    sed -i 's/\<true\>/TRUE/g' $target_file
    sed -i 's/\<false\>/FALSE/g' $target_file
    sed -i 's/\<null\>/NULL/g' $target_file

    # process static final
    #
    sed -i "s@static final@static const@" $target_file
    sed -i "s@final static@static const@" $target_file

    # process comments
    #
    sed -i "s:    @Override:    //@Override:g" $target_file
    sed -i "s:    @Deprecated:    //@Deprecated:g" $target_file

    # Log.d or Slog.d => Logger::D( or Slogger::D(
    #
    sed -i "s@Log.d(@Logger::D(@g" $target_file
    sed -i "s@Log.i(@Logger::I(@g" $target_file
    sed -i "s@Log.e(@Logger::E(@g" $target_file
    sed -i "s@Log.w(@Logger::W(@g" $target_file
    sed -i "s@Log.v(@Logger::V(@g" $target_file
    sed -i "s@Slog.d(@Slogger::D(@g" $target_file
    sed -i "s@Slog.i(@Slogger::I(@g" $target_file
    sed -i "s@Slog.e(@Slogger::E(@g" $target_file
    sed -i "s@Slog.w(@Slogger::W(@g" $target_file
    sed -i "s@Slog.v(@Slogger::V(@g" $target_file

    # public void => CARAPI
    #
    sed -i "s:public void:CARAPI:g" $target_file

    # public static const class => public class
    #
    sed -i "s:public static const class:public class:g" $target_file
    sed -i "s:public final class:public class:g" $target_file

    # String toString() { => CARAPI ToString(/* [out] */ String* str) {
    #
    sed -i "s:String toString() {:CARAPI ToString(\n        /* [out] */ String* str)\n    {:g" $target_file
    sed -i "s:public CARAPI:CARAPI:g" $target_file

    # a.fun(  => a->Fun(
    #
    sed -i 's:\([a-zA-Z]\w*\)\(\.\)\([a-zA-Z]\)\(\w*\)\((\):\1->\u\3\4\5:g' $target_file

    # a().fun(  => a()->Fun(
    #
    sed -i 's:\([a-zA-Z]\w*()\)\(\.\)\([a-zA-Z]\)\(\w*\)\((\):\1->\u\3\4\5:g' $target_file

    # fun(  => Fun(
    #
    sed -i 's:\([a-zA-Z]\w*\s*\)\((\):\u\1\2:g' $target_file

    # synchronized ( => synchronized(
    #
    sed -i "s:synchronized (:synchronized(:g" $target_file

    # android.Manifest.permission. => Manifest::permission::
    #
    sed -i "s@android.Manifest.permission.@Manifest::permission::@g" $target_file
    sed -i "s@Manifest.permission.@Manifest::permission::@g" $target_file

    sed -i "s@R.string.@R::string::@g" $target_file
    sed -i "s@R.color.@R::Color::@g" $target_file
    sed -i "s@com.android.R@R@g" $target_file
    sed -i "s@com.android.internal.R@R@g" $target_file

    sed -i "s@Intent.ACTION_@IIntent::ACTION_@g" $target_file
    sed -i "s@Intent.FLAG_@IIntent::FLAG_@g" $target_file

    sed -i "s@Settings.Global.@Settings::Global::@g" $target_file
}

process_dir () {
    path=$1
    echo "> process dir $path"
    postfix=".java"
    declare -i postfixLen=${#postfix}

    for file in $path/*; do
        if [ -d $file ]; then
            #echo "  > process sub dir $file"
            process_dir $file
        elif [ -f "$file" ]; then
            filename=$(basename "$file")
            len=${#filename}
            if [ $len -ge $postfixLen ]; then
                start=$len-$postfixLen
                sub=${filename:$start:$len}
                if [ $sub == $postfix ]; then
                    echo "  > prcess file $filename"
                    pre_translate $file
                    tgt=$(echo $file | sed 's/\(.*\)java/\1cpp/')
                    echo "  > result $tgt"
                    mv $file $tgt
                fi
            fi
        fi
    done
}

echo "pre-translate start..."

count=$#

if [ $count -eq 1 ]; then
    path=$1
    if [ -f "$path" ]; then
        echo " > prcess file $path"
        pre_translate $path
    elif [ -d "$path" ]; then
        echo " > process root dir $path"
        process_dir $path
    else
        echo "file does not exist. $path"
    fi
elif [ $count -eq 2 ]; then
    src_file=$1
    target_file=$2
    cp $src_file $target_file
    echo " > copy $src_file"
    echo " > process $target_file"
    pre_translate $target_file
fi

echo "pre-translate done."