
#ifndef __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIILOCATIONPROVIDERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIILocationProviderNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::Location::IILocationProvider;
using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Droid::Internal::Location::IProviderRequest;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIILocationProviderNative)
    , public Object
    , public IILocationProvider
    , public IBinder
{
public:
    ~CIILocationProviderNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

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
        /* [in] */ IBundle* inExtras,
        /* [out] */ IBundle** outExtras,
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
