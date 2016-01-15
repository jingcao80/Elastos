
#ifndef __CACCOUNTMANAGERRESPONSENATIVE_H__
#define __CACCOUNTMANAGERRESPONSENATIVE_H__

#include "_CAccountManagerResponseNative.h"
#include <jni.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CAccountManagerResponseNative)
{
public:
    ~CAccountManagerResponseNative();
    
    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnResult(
        /* [in] */ IBundle* value);

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CACCOUNTMANAGERRESPONSENATIVE_H__
