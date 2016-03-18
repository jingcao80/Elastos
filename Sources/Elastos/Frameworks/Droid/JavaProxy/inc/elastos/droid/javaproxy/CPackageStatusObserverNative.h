
#ifndef __ELASTOS_DROID_JAVAPROXY_CPACKAGESTATSOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CPACKAGESTATSOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CPackageStatusObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Content::Pm::IIPackageStatsObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CPackageStatusObserverNative)
    , public Object
    , public IIPackageStatsObserver
    , public IBinder
{
public:
    ~CPackageStatusObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

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
