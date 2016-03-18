
#ifndef __ELASTOS_DROID_JAVAPROXY_CACCOUNTMANAGERRESPONSENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CACCOUNTMANAGERRESPONSENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CAccountManagerResponseNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CAccountManagerResponseNative)
    , public Object
    , public IAccountManagerResponse
    , public IBinder
{
public:
    ~CAccountManagerResponseNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnResult(
        /* [in] */ IBundle* value);

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CACCOUNTMANAGERRESPONSENATIVE_H__
