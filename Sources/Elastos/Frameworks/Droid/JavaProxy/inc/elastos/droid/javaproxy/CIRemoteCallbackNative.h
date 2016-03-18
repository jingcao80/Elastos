
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIRemoteCallback;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteCallbackNative)
    , public Object
    , public IIRemoteCallback
    , public IBinder
{
public:
    ~CIRemoteCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI SendResult(
        /* [in] */ IBundle* data);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECALLBACKNATIVE_H__
