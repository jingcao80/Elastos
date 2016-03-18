
#ifndef __ELASTOS_DROID_JAVAPROXY_CIONKEYGUARDEXITRESULTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIONKEYGUARDEXITRESULTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIOnKeyguardExitResultNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IOnKeyguardExitResult;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIOnKeyguardExitResultNative)
    , public Object
    , public IOnKeyguardExitResult
    , public IBinder
{
public:
    ~CIOnKeyguardExitResultNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnKeyguardExitResult(
        /* [in] */ Boolean success);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIONKEYGUARDEXITRESULTNATIVE_H__
