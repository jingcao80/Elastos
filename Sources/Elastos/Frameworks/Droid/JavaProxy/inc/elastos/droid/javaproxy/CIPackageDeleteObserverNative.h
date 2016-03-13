
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDELETEOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDELETEOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIPackageDeleteObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIPackageDeleteObserverNative)
{
public:
    ~CIPackageDeleteObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI PackageDeleted(
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

#endif // __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDELETEOBSERVERNATIVE_H__
