
#ifndef __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIILocationProviderNative.h"
#include <jni.h>

using Elastos::Droid::Location::IProviderProperties;
using Elastos::Droid::Location::IProviderRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIILocationProviderNative)
{
public:
    ~CIILocationProviderNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI Enable();

    CARAPI Disable();

    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* ws);

    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    CARAPI GetStatus(
        /* [out] */ IBundle** extras,
        /* [out] */ Int32* status);

    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* updateTime);

    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [out] */ IBundle** extras,
        /* [out] */ Boolean* result);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__
