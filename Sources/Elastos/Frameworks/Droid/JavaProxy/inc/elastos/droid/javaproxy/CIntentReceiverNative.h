
#ifndef __ELASTOS_DROID_JAVAPROXY_CINTENTRECEIVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINTENTRECEIVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIntentReceiverNative.h"
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIntentReceiverNative)
{
public:
    ~CIntentReceiverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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
