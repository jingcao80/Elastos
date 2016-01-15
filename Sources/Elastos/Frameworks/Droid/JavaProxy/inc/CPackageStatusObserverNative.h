
#ifndef __CPACKAGESTATSOBSERVERNATIVE_H__
#define __CPACKAGESTATSOBSERVERNATIVE_H__

#include "_CPackageStatusObserverNative.h"
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

#endif // __CPACKAGESTATSOBSERVERNATIVE_H__
