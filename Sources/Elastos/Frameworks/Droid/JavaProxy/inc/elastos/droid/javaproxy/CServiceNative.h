
#ifndef __ELASTOS_DROID_JAVAPROXY_CSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CServiceNative.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CServiceNative)
    , public Object
    , public IParcelable
    , public IBinder
{
public:
    ~CServiceNative();

    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    CARAPI GetRemoteInstance(
        /* [in] */ JNIEnv* env,
        /* [out] */ Handle64* obj);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
    android::sp<android::IBinder> mibinder;
};

} // JavaProxy
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CSERVICENATIVE_H__
