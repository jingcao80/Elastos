
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDATAOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDATAOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIPackageDataObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIPackageDataObserverNative)
{
public:
    ~CIPackageDataObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnRemoveCompleted(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean succeeded);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIPACKAGEDATAOBSERVERNATIVE_H__
