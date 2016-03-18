
#ifndef __ELASTOS_DROID_JAVAPROXY_CIIntentSenderNative_H__
#define __ELASTOS_DROID_JAVAPROXY_CIIntentSenderNative_H__

#include "_Elastos_Droid_JavaProxy_CIIntentSenderNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IIIntentSender;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIIntentSenderNative)
    , public Object
    , public IIIntentSender
    , public IBinder
{
public:
    ~CIIntentSenderNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Send(
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* finishedReceiver,
        /* [in] */ const String& requiredPermission,
        /* [out] */ Int32* result);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIIntentSenderNative_H__
