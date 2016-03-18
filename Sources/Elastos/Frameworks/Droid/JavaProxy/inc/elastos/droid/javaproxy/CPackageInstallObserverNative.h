
#ifndef __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CPackageInstallObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::Pm::IIPackageInstallObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CPackageInstallObserverNative)
    , public Object
    , public IIPackageInstallObserver
    , public IBinder
{
public:
    ~CPackageInstallObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI PackageInstalled(
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

#endif // __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVERNATIVE_H__
