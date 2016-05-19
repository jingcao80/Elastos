
#ifndef __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVER2NATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVER2NATIVE_H__

#include "_Elastos_Droid_JavaProxy_CPackageInstallObserver2Native.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver2;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CPackageInstallObserver2Native)
    , public Object
    , public IIPackageInstallObserver2
    , public IBinder
{
public:
    ~CPackageInstallObserver2Native();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnUserActionRequired(
        /* [in] */ IIntent* intent);

    CARAPI OnPackageInstalled(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg,
        /* [in] */ IBundle* extras);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVER2NATIVE_H__
