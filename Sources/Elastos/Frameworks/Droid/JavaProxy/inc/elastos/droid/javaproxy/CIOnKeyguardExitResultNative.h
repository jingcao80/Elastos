
#ifndef __ELASTOS_DROID_JAVAPROXY_CIONKEYGUARDEXITRESULTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIONKEYGUARDEXITRESULTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIOnKeyguardExitResultNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIOnKeyguardExitResultNative)
{
public:
    ~CIOnKeyguardExitResultNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnKeyguardExitResult(
        /* [in] */ Boolean success);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIONKEYGUARDEXITRESULTNATIVE_H__
