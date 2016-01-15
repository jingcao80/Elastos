
#ifndef __CIUSERSWITCHOBSERVERNATIVE_H__
#define __CIUSERSWITCHOBSERVERNATIVE_H__

#include "_CIUserSwitchObserverNative.h"
#include <jni.h>

using Elastos::Droid::Os::IRemoteCallback;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIUserSwitchObserverNative)
{
public:
    ~CIUserSwitchObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnUserSwitching(
        /* [in] */ Int32 newUserId,
        /* [in] */ IRemoteCallback* reply);

    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 newUserId);

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

#endif // __CIUSERSWITCHOBSERVERNATIVE_H__
