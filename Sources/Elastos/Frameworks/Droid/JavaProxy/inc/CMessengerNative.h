
#ifndef __CMESSENGERNATIVE_H__
#define __CMESSENGERNATIVE_H__

#include "_CMessengerNative.h"
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

#endif //__CMESSENGERNATIVE_H__
