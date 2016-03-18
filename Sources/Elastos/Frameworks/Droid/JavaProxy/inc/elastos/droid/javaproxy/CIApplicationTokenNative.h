
#ifndef __ELASTOS_DROID_JAVAPROXY_CIAPPLICATIONTOKENNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIAPPLICATIONTOKENNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIApplicationTokenNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IApplicationToken;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIApplicationTokenNative)
    , public Object
    , public IApplicationToken
    , public IBinder
{
public:
    ~CIApplicationTokenNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI WindowsDrawn();

    CARAPI WindowsVisible();

    CARAPI WindowsGone();

    CARAPI KeyDispatchingTimedOut(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* res);

    CARAPI GetKeyDispatchingTimeout(
        /* [out] */ Int64* timeOut);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIAPPLICATIONTOKENNATIVE_H__
