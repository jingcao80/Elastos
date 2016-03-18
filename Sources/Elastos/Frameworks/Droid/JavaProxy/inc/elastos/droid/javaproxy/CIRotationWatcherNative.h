
#ifndef __ELASTOS_DROID_JAVAPROXY_CIROTATIONWATCHERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIROTATIONWATCHERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIRotationWatcherNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IRotationWatcher;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRotationWatcherNative)
    , public Object
    , public IRotationWatcher
    , public IBinder
{
public:
    ~CIRotationWatcherNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIROTATIONWATCHERNATIVE_H__
