
#ifndef __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CPACKAGEINSTALLOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CPackageInstallObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CPackageInstallObserverNative)
{
public:
    ~CPackageInstallObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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
