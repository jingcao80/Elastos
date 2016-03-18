
#ifndef __ELASTOS_DROID_JAVAPROXY_CBINDERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CBINDERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CBinderNative.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CBinderNative)
    , public Object
    , public IBinder
{
public:
    ~CBinderNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // JavaProxy
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CBINDERNATIVE_H__
