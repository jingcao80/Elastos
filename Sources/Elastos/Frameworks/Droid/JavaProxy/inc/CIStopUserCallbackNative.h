
#ifndef __CISTOPUSERCALLBACKNATIVE_H__
#define __CISTOPUSERCALLBACKNATIVE_H__

#include "_CIStopUserCallbackNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIStopUserCallbackNative)
{
public:
    ~CIStopUserCallbackNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI UserStopped(
        /* [in] */ Int32 userId);

    CARAPI UserStopAborted(
        /* [in] */ Int32 userId);

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

#endif // __CISTOPUSERCALLBACKNATIVE_H__
