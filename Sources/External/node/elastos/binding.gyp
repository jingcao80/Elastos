{
    "targets": [
        {
            "target_name": "elastos",
            #'type': 'executable',
            #'type': 'none',
            #'type': '<(library)',
            "library_dirs": [
                "/home/whm/kt_work/Elastos/ElastosRDK5/Elastos5/Build/Prebuilt/Linux/usr/lib",
            ],
            "libraries": [
                "/home/whm/kt_work/Elastos/ElastosRDK5/Elastos5/Targets/obj/rdk/arm.gnu.android.dbg/lib/Elastos.Runtime.lib",
                "/home/whm/kt_work/Elastos/ElastosRDK5/Elastos5/Targets/rdk/arm.gnu.android.dbg/bin/Elastos.Runtime.eco",
                "/home/whm/kt_work/Elastos/ElastosRDK5/Elastos5/Targets/rdk/arm.gnu.android.dbg/bin/Elastos.CoreLibrary.eco",
                "/home/whm/kt_work/Elastos/ElastosRDK5/Elastos5/Targets/obj/rdk/arm.gnu.android.dbg/lib/crt0.lib",

                "-licuuc",
                "-licui18n",
                "-llog",
                "-lnode",
            ],
            "include_dirs": [
                "JavaScriptCore",
                "JavaScriptCore/icu",
                "JavaScriptCore/wtf",
                "JavaScriptCore/wtf/text",
                "JavaScriptCore/wtf/unicode",
                "JavaScriptCore/wtf/unicode/icu",
                "../../../Elastos/Runtime/Core/inc",
                "../../../Elastos/Runtime/Library/inc",
                "../../../Elastos/Runtime/Library/inc/car",
                "../../../Elastos/Runtime/Library/inc/clsmodule",
                "../../../Elastos/Runtime/Library/inc/driver",
                "../../../Elastos/Runtime/Library/inc/elasys",
                "../../../Elastos/Runtime/Library/inc/eltypes",
                "../../../Elastos/LibCore/inc/common",
                "../../../Elastos/LibCore/inc/elastos/utility",

                "../../../Elastos/Frameworks/Droid/Base/Core/inc/elastos/droid",
                "../../../../Targets/obj/rdk/arm.gnu.android.dbg/inc",
                "../../../../Targets/obj/rdk/arm.gnu.android.dbg/mirror/Elastos/Frameworks/Droid/Base/Core/src/elastos/droid",

                "../../../../Targets/obj/rdk/arm.gnu.android.dbg/mirror/Elastos/LibCore/src/corelibrary",

                "bridge",
                "bridge/car",
                "bridge/car/v8",
                "bindings_node/v8",
                "plugins",
                "addon",

                "platform",
                "platform/elastos",
                "platform/graphics",
                "platform/text",

                "utils",
                "include",
            ],
            "sources": [
                "addon/elastos_root.cpp",

                "bindings_node/v8/V8Utilities.cpp",
                "bindings_node/v8/NPV8Object.cpp",
                "bindings_node/v8/V8NPObject.cpp",
                "bindings_node/v8/V8NPUtils.cpp",
                "bindings_node/v8/V8Helpers.cpp",
                "bindings_node/v8/V8Proxy.cpp",
                "bindings_node/v8/npruntime.cpp",
                "bindings_node/v8/npapi.cpp",

                "bridge/car/libcore/Elastos.CoreLibrary.cpp",

                "bridge/car/CarUtility.cpp",
                "bridge/car/CobjectWrapper.cpp",
                "bridge/car/CarCallbackProxy.cpp",
                "bridge/car/v8/CarClassV8.cpp",
                "bridge/car/v8/CarFieldV8.cpp",
                "bridge/car/v8/CarInstanceV8.cpp",
                "bridge/car/v8/CarMethodV8.cpp",
                "bridge/car/v8/CarNPObjectV8.cpp",
                "bridge/car/v8/CarUtilityPrivate.cpp",

                "JavaScriptCore/wtf/Assertions.cpp",
                "JavaScriptCore/wtf/CurrentTime.cpp",
                "JavaScriptCore/wtf/dtoa.cpp",
                "JavaScriptCore/wtf/DecimalNumber.cpp",
                "JavaScriptCore/wtf/FastMalloc.cpp",
                "JavaScriptCore/wtf/HashTable.cpp",
                "JavaScriptCore/wtf/StackBounds.cpp",
                "JavaScriptCore/wtf/TCSystemAlloc.cpp",
                "JavaScriptCore/wtf/Threading.cpp",
                "JavaScriptCore/wtf/ThreadingNone.cpp",
                "JavaScriptCore/wtf/WTFThreadData.cpp",
                "JavaScriptCore/wtf/text/AtomicString.cpp",
                "JavaScriptCore/wtf/text/CString.cpp",
                "JavaScriptCore/wtf/text/StringBuilder.cpp",
                "JavaScriptCore/wtf/text/StringImpl.cpp",
                "JavaScriptCore/wtf/text/StringStatics.cpp",
                "JavaScriptCore/wtf/text/WTFString.cpp",
                "JavaScriptCore/wtf/unicode/icu/CollatorICU.cpp",
                "JavaScriptCore/wtf/unicode/CollatorDefault.cpp",
                "JavaScriptCore/wtf/unicode/UTF8.cpp",
            ]
        }
    ]

}
