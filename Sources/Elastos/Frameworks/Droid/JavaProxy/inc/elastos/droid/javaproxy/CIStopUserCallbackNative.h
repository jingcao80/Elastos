
#ifndef __ELASTOS_DROID_JAVAPROXY_CISTOPUSERCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CISTOPUSERCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIStopUserCallbackNative.h"
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

#endif // __ELASTOS_DROID_JAVAPROXY_CISTOPUSERCALLBACKNATIVE_H__
