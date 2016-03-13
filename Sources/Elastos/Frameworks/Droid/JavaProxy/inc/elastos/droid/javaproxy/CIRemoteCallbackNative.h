
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteCallbackNative.h"
#include <jni.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteCallbackNative)
{
public:
    ~CIRemoteCallbackNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI SendResult(
        /* [in] */ IBundle* data);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECALLBACKNATIVE_H__
