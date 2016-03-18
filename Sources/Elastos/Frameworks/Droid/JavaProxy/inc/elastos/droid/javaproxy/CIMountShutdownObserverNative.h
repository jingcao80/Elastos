
#ifndef __ELASTOS_DROID_JAVAPROXY_CIMOUNTSHUTDOWNOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIMOUNTSHUTDOWNOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIMountShutdownObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Storage::IIMountShutdownObserver;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIMountShutdownObserverNative)
    , public Object
    , public IIMountShutdownObserver
    , public IBinder
{
public:
    ~CIMountShutdownObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnShutDownComplete(
        /* [in] */ Int32 statusCode);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIMOUNTSHUTDOWNOBSERVERNATIVE_H__
