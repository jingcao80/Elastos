
#ifndef __ELASTOS_DROID_JAVAPROXY_CMESSENGERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CMESSENGERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CMessengerNative.h"
#include <jni.h>

using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CMessengerNative)
{
public:
    ~CMessengerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI Send(
        /* [in] */ IMessage* message);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_JAVAPROXY_CMESSENGERNATIVE_H__
