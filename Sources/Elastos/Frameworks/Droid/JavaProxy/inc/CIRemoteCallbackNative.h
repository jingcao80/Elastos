
#ifndef __CIREMOTECALLBACKNATIVE_H__
#define __CIREMOTECALLBACKNATIVE_H__

#include "_CIRemoteCallbackNative.h"
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

#endif // __CIREMOTECALLBACKNATIVE_H__
