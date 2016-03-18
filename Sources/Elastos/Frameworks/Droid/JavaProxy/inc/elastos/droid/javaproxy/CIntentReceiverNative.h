
#ifndef __ELASTOS_DROID_JAVAPROXY_CINTENTRECEIVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINTENTRECEIVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIntentReceiverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIntentReceiverNative)
    , public Object
    , public IIntentReceiver
    , public IBinder
{
public:
    ~CIntentReceiverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI PerformReceive(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CINTENTRECEIVERNATIVE_H__
