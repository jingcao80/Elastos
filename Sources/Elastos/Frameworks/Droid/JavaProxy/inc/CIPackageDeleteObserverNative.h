
#ifndef __CIPACKAGEDELETEOBSERVERNATIVE_H__
#define __CIPACKAGEDELETEOBSERVERNATIVE_H__

#include "_CIPackageDeleteObserverNative.h"
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

#endif // __CIPACKAGEDELETEOBSERVERNATIVE_H__
