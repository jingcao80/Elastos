cmd_Release/obj.target/elastos/bindings_node/v8/NPV8Object.o := arm-linux-androideabi-g++ '-DNODE_GYP_MODULE_NAME=elastos' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DBUILDING_NODE_EXTENSION' -I/home/whm/.node-gyp/0.12.7/src -I/home/whm/.node-gyp/0.12.7/deps/uv/include -I/home/whm/.node-gyp/0.12.7/deps/v8/include -I../JavaScriptCore -I../JavaScriptCore/icu -I../JavaScriptCore/wtf -I../JavaScriptCore/wtf/text -I../JavaScriptCore/wtf/unicode -I../JavaScriptCore/wtf/unicode/icu -I../../../../Elastos/Runtime/Core/inc -I../../../../Elastos/Runtime/Library/inc -I../../../../Elastos/Runtime/Library/inc/car -I../../../../Elastos/Runtime/Library/inc/clsmodule -I../../../../Elastos/Runtime/Library/inc/driver -I../../../../Elastos/Runtime/Library/inc/elasys -I../../../../Elastos/Runtime/Library/inc/eltypes -I../../../../Elastos/LibCore/inc/common -I../../../../Elastos/LibCore/inc/elastos/utility -I../../../../Elastos/Frameworks/Droid/Base/Core/inc/elastos/droid -I../../../../../Targets/obj/rdk/arm.gnu.android.dbg/inc -I../../../../../Targets/obj/rdk/arm.gnu.android.dbg/mirror/Elastos/Frameworks/Droid/Base/Core/src/elastos/droid -I../bridge -I../bridge/car -I../bridge/car/v8 -I../bindings_node/v8 -I../plugins -I../addon -I../platform -I../platform/elastos -I../platform/graphics -I../platform/text -I../utils -I../include  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -O3 -ffunction-sections -fdata-sections -fno-tree-vrp -fno-omit-frame-pointer -fno-rtti -fno-exceptions -MMD -MF ./Release/.deps/Release/obj.target/elastos/bindings_node/v8/NPV8Object.o.d.raw  -c -o Release/obj.target/elastos/bindings_node/v8/NPV8Object.o ../bindings_node/v8/NPV8Object.cpp
Release/obj.target/elastos/bindings_node/v8/NPV8Object.o: \
 ../bindings_node/v8/NPV8Object.cpp ../JavaScriptCore/config.h \
 ../utils/ElNode.h ../JavaScriptCore/wtf/Platform.h \
 ../JavaScriptCore/wtf/FastMalloc.h ../JavaScriptCore/wtf/Platform.h \
 ../JavaScriptCore/wtf/PossiblyNull.h ../JavaScriptCore/wtf/Assertions.h \
 ../JavaScriptCore/wtf/DisallowCType.h ../bindings_node/v8/NPV8Object.h \
 ../bridge/npruntime.h ../bridge/npapi.h ../bridge/nptypes.h \
 ../bindings_node/v8/WrapperTypeInfo.h \
 /home/whm/.node-gyp/0.12.7/deps/v8/include/v8.h \
 /home/whm/.node-gyp/0.12.7/deps/v8/include/v8stdint.h \
 /home/whm/.node-gyp/0.12.7/deps/v8/include/v8config.h \
 ../platform/elastos/PlatformBridge.h ../platform/graphics/FloatRect.h \
 ../platform/graphics/FloatPoint.h ../platform/graphics/FloatSize.h \
 ../platform/graphics/IntSize.h ../JavaScriptCore/wtf/MathExtras.h \
 ../platform/graphics/IntPoint.h ../platform/KURL.h \
 ../platform/text/PlatformString.h ../JavaScriptCore/wtf/text/WTFString.h \
 ../JavaScriptCore/wtf/text/StringImpl.h \
 ../JavaScriptCore/wtf/ASCIICType.h ../JavaScriptCore/wtf/Assertions.h \
 ../JavaScriptCore/wtf/CrossThreadRefCounted.h \
 ../JavaScriptCore/wtf/PassRefPtr.h ../JavaScriptCore/wtf/AlwaysInline.h \
 ../JavaScriptCore/wtf/NullPtr.h ../JavaScriptCore/wtf/RefCounted.h \
 ../JavaScriptCore/wtf/FastAllocBase.h ../JavaScriptCore/wtf/FastMalloc.h \
 ../JavaScriptCore/wtf/TypeTraits.h ../JavaScriptCore/wtf/Noncopyable.h \
 ../JavaScriptCore/wtf/Threading.h ../JavaScriptCore/wtf/Atomics.h \
 ../JavaScriptCore/wtf/Locker.h ../JavaScriptCore/wtf/Noncopyable.h \
 ../JavaScriptCore/wtf/MainThread.h \
 ../JavaScriptCore/wtf/ThreadSafeRefCounted.h \
 ../JavaScriptCore/wtf/DynamicAnnotations.h \
 ../JavaScriptCore/wtf/ThreadingPrimitives.h \
 ../JavaScriptCore/wtf/FastAllocBase.h ../JavaScriptCore/wtf/Forward.h \
 ../JavaScriptCore/wtf/OwnFastMallocPtr.h \
 ../JavaScriptCore/wtf/StdLibExtras.h \
 ../JavaScriptCore/wtf/StringHasher.h \
 ../JavaScriptCore/wtf/unicode/Unicode.h \
 ../JavaScriptCore/wtf/unicode/icu/UnicodeIcu.h \
 ../JavaScriptCore/icu/unicode/uchar.h \
 ../JavaScriptCore/icu/unicode/utypes.h \
 ../JavaScriptCore/icu/unicode/umachine.h \
 ../JavaScriptCore/icu/unicode/platform.h \
 ../JavaScriptCore/icu/unicode/urename.h \
 ../JavaScriptCore/icu/unicode/utf.h ../JavaScriptCore/icu/unicode/utf8.h \
 ../JavaScriptCore/icu/unicode/utf16.h \
 ../JavaScriptCore/icu/unicode/utf_old.h \
 ../JavaScriptCore/icu/unicode/uversion.h \
 ../JavaScriptCore/icu/unicode/uconfig.h \
 ../JavaScriptCore/icu/unicode/ustring.h \
 ../JavaScriptCore/icu/unicode/putil.h \
 ../JavaScriptCore/icu/unicode/uiter.h ../JavaScriptCore/wtf/Vector.h \
 ../JavaScriptCore/wtf/NotFound.h ../JavaScriptCore/wtf/StdLibExtras.h \
 ../JavaScriptCore/wtf/ValueCheck.h ../JavaScriptCore/wtf/VectorTraits.h \
 ../JavaScriptCore/wtf/OwnPtr.h ../JavaScriptCore/wtf/OwnPtrCommon.h \
 ../JavaScriptCore/wtf/RefPtr.h ../JavaScriptCore/wtf/Alignment.h \
 ../JavaScriptCore/wtf/text/StringImplBase.h ../platform/URLString.h \
 ../JavaScriptCore/wtf/HashMap.h ../JavaScriptCore/wtf/HashTable.h \
 ../JavaScriptCore/wtf/HashTraits.h ../JavaScriptCore/wtf/HashFunctions.h \
 ../JavaScriptCore/wtf/Threading.h ../JavaScriptCore/wtf/HashIterators.h \
 ../JavaScriptCore/wtf/RefPtrHashMap.h ../bridge/npapi.h \
 ../bindings_node/v8/node_DOMWindow.h ../JavaScriptCore/wtf/OwnArrayPtr.h \
 ../JavaScriptCore/wtf/PassOwnArrayPtr.h ../bindings_node/v8/V8Helpers.h \
 ../bindings_node/v8/npruntime_internal.h ../plugins/npfunctions.h \
 ../bindings_node/v8/V8NPUtils.h ../bindings_node/v8/V8Proxy.h \
 ../bindings_node/v8/V8Utilities.h ../bindings_node/v8/npruntime_impl.h \
 ../bindings_node/v8/npruntime_priv.h \
 ../JavaScriptCore/wtf/StringExtras.h ../include/utils/Log.h \
 ../include/cutils/log.h ../include/cutils/uio.h ../include/cutils/logd.h
../bindings_node/v8/NPV8Object.cpp:
../JavaScriptCore/config.h:
../utils/ElNode.h:
../JavaScriptCore/wtf/Platform.h:
../JavaScriptCore/wtf/FastMalloc.h:
../JavaScriptCore/wtf/Platform.h:
../JavaScriptCore/wtf/PossiblyNull.h:
../JavaScriptCore/wtf/Assertions.h:
../JavaScriptCore/wtf/DisallowCType.h:
../bindings_node/v8/NPV8Object.h:
../bridge/npruntime.h:
../bridge/npapi.h:
../bridge/nptypes.h:
../bindings_node/v8/WrapperTypeInfo.h:
/home/whm/.node-gyp/0.12.7/deps/v8/include/v8.h:
/home/whm/.node-gyp/0.12.7/deps/v8/include/v8stdint.h:
/home/whm/.node-gyp/0.12.7/deps/v8/include/v8config.h:
../platform/elastos/PlatformBridge.h:
../platform/graphics/FloatRect.h:
../platform/graphics/FloatPoint.h:
../platform/graphics/FloatSize.h:
../platform/graphics/IntSize.h:
../JavaScriptCore/wtf/MathExtras.h:
../platform/graphics/IntPoint.h:
../platform/KURL.h:
../platform/text/PlatformString.h:
../JavaScriptCore/wtf/text/WTFString.h:
../JavaScriptCore/wtf/text/StringImpl.h:
../JavaScriptCore/wtf/ASCIICType.h:
../JavaScriptCore/wtf/Assertions.h:
../JavaScriptCore/wtf/CrossThreadRefCounted.h:
../JavaScriptCore/wtf/PassRefPtr.h:
../JavaScriptCore/wtf/AlwaysInline.h:
../JavaScriptCore/wtf/NullPtr.h:
../JavaScriptCore/wtf/RefCounted.h:
../JavaScriptCore/wtf/FastAllocBase.h:
../JavaScriptCore/wtf/FastMalloc.h:
../JavaScriptCore/wtf/TypeTraits.h:
../JavaScriptCore/wtf/Noncopyable.h:
../JavaScriptCore/wtf/Threading.h:
../JavaScriptCore/wtf/Atomics.h:
../JavaScriptCore/wtf/Locker.h:
../JavaScriptCore/wtf/Noncopyable.h:
../JavaScriptCore/wtf/MainThread.h:
../JavaScriptCore/wtf/ThreadSafeRefCounted.h:
../JavaScriptCore/wtf/DynamicAnnotations.h:
../JavaScriptCore/wtf/ThreadingPrimitives.h:
../JavaScriptCore/wtf/FastAllocBase.h:
../JavaScriptCore/wtf/Forward.h:
../JavaScriptCore/wtf/OwnFastMallocPtr.h:
../JavaScriptCore/wtf/StdLibExtras.h:
../JavaScriptCore/wtf/StringHasher.h:
../JavaScriptCore/wtf/unicode/Unicode.h:
../JavaScriptCore/wtf/unicode/icu/UnicodeIcu.h:
../JavaScriptCore/icu/unicode/uchar.h:
../JavaScriptCore/icu/unicode/utypes.h:
../JavaScriptCore/icu/unicode/umachine.h:
../JavaScriptCore/icu/unicode/platform.h:
../JavaScriptCore/icu/unicode/urename.h:
../JavaScriptCore/icu/unicode/utf.h:
../JavaScriptCore/icu/unicode/utf8.h:
../JavaScriptCore/icu/unicode/utf16.h:
../JavaScriptCore/icu/unicode/utf_old.h:
../JavaScriptCore/icu/unicode/uversion.h:
../JavaScriptCore/icu/unicode/uconfig.h:
../JavaScriptCore/icu/unicode/ustring.h:
../JavaScriptCore/icu/unicode/putil.h:
../JavaScriptCore/icu/unicode/uiter.h:
../JavaScriptCore/wtf/Vector.h:
../JavaScriptCore/wtf/NotFound.h:
../JavaScriptCore/wtf/StdLibExtras.h:
../JavaScriptCore/wtf/ValueCheck.h:
../JavaScriptCore/wtf/VectorTraits.h:
../JavaScriptCore/wtf/OwnPtr.h:
../JavaScriptCore/wtf/OwnPtrCommon.h:
../JavaScriptCore/wtf/RefPtr.h:
../JavaScriptCore/wtf/Alignment.h:
../JavaScriptCore/wtf/text/StringImplBase.h:
../platform/URLString.h:
../JavaScriptCore/wtf/HashMap.h:
../JavaScriptCore/wtf/HashTable.h:
../JavaScriptCore/wtf/HashTraits.h:
../JavaScriptCore/wtf/HashFunctions.h:
../JavaScriptCore/wtf/Threading.h:
../JavaScriptCore/wtf/HashIterators.h:
../JavaScriptCore/wtf/RefPtrHashMap.h:
../bridge/npapi.h:
../bindings_node/v8/node_DOMWindow.h:
../JavaScriptCore/wtf/OwnArrayPtr.h:
../JavaScriptCore/wtf/PassOwnArrayPtr.h:
../bindings_node/v8/V8Helpers.h:
../bindings_node/v8/npruntime_internal.h:
../plugins/npfunctions.h:
../bindings_node/v8/V8NPUtils.h:
../bindings_node/v8/V8Proxy.h:
../bindings_node/v8/V8Utilities.h:
../bindings_node/v8/npruntime_impl.h:
../bindings_node/v8/npruntime_priv.h:
../JavaScriptCore/wtf/StringExtras.h:
../include/utils/Log.h:
../include/cutils/log.h:
../include/cutils/uio.h:
../include/cutils/logd.h:
