
#ifndef __CPACKAGEINSTALLOBSERVERNATIVE_H__
#define __CPACKAGEINSTALLOBSERVERNATIVE_H__

#include "_CPackageInstallObserverNative.h"
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

#endif // __CPACKAGEINSTALLOBSERVERNATIVE_H__
