
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPACKAGEMOVEOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPACKAGEMOVEOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIPackageMoveObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::Pm::IIPackageMoveObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIPackageMoveObserverNative)
    , public Object
    , public IIPackageMoveObserver
    , public IBinder
{
public:
    ~CIPackageMoveObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI PackageMoved(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 returnCode);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIPACKAGEMOVEOBSERVERNATIVE_H__
