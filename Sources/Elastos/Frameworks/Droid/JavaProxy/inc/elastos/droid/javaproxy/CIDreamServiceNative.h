
#ifndef __ELASTOS_DROID_JAVAPROXY_CIDREAMSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIDREAMSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIDreamServiceNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Service::Dreams::IIDreamService;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIDreamServiceNative)
    , public Object
    , public IIDreamService
    , public IBinder
{
public:
    ~CIDreamServiceNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Attach(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Boolean canDoze);

    CARAPI Detach();

    CARAPI WakeUp();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIDREAMSERVICENATIVE_H__
