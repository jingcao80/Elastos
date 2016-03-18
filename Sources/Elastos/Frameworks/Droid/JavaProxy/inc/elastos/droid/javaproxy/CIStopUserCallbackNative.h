
#ifndef __ELASTOS_DROID_JAVAPROXY_CISTOPUSERCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CISTOPUSERCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIStopUserCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IStopUserCallback;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIStopUserCallbackNative)
    , public Object
    , public IStopUserCallback
    , public IBinder
{
public:
    ~CIStopUserCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI UserStopped(
        /* [in] */ Int32 userId);

    CARAPI UserStopAborted(
        /* [in] */ Int32 userId);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CISTOPUSERCALLBACKNATIVE_H__
