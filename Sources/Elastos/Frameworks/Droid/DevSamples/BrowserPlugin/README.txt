use the below html code to do the test

<object type="application/x-testbrowserplugin" id="sample" height=80 width=250>
    <param name="DrawingModel" value="Bitmap"/>
    <param name="PluginType" value="Audio"/>
</object>

<object type="application/x-testbrowserplugin" id="sample" height=100 width=250>
    <param name="DrawingModel" value="Bitmap"/>
<param name="PluginType" value="Hello"/>

note:
    1. the epk which contain so should install under the /data/app,
    2. according the Android spec, the so should be under a folder which represent
       the cpu target, example: lib/armeabi-v7a/XXX.so
    3. add the env variable in the source file:
        TARGET_CPU_ABI = armeabi-v7a
        LIBRESOURCES += $(XDK_TARGETS)/libsampleplugin.so


