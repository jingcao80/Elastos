
#ifndef __ELASTOS_DROID_JAVAPROXY_CPACKAGESTATSOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CPACKAGESTATSOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CPackageStatusObserverNative.h"
#include <jni.h>

using Elastos::Droid::Content::Pm::IPackageStats;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CPackageStatusObserverNative)
{
public:
    ~CPackageStatusObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnGetStatsCompleted(
        /* [in] */ IPackageStats* pStats,
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

#endif // __ELASTOS_DROID_JAVAPROXY_CPACKAGESTATSOBSERVERNATIVE_H__
