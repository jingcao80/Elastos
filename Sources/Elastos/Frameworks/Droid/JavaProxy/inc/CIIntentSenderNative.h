
#ifndef __CIIntentSenderNative_H__
#define __CIIntentSenderNative_H__

#include "_CIIntentSenderNative.h"
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIIntentSenderNative)
{
public:
    ~CIIntentSenderNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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

#endif // __CIIntentSenderNative_H__
