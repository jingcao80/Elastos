cd ../../../../../../Setup
. SetEnv.sh arm_android
cd Elastos/Frameworks/Droid/DevSamples/jpk/CarRuntime/
emake rebuild
cd ../JSTextViewDemo
emake rebuild
cd ..

adb shell rm -r /data/temp/node
adb push bin /data/temp/node/bin
adb push Common/js /data/temp/node/Common/js

adb shell mv /data/temp/node/Common/js/TextViewDemo /data/temp/node/JSTextViewDemo
adb shell mv /data/temp/node/bin/libnode.so /system/lib/.

pd @
adb push Elastos.DevSamples.Node.CarRuntime.eco /data/temp/node/bin/.
adb push Elastos.DevSamples.Node.JSTextViewDemo.epk /system/app/.
adb push Elastos.DevSamples.Node.JSTextViewDemo.eco /data/elastos/.
