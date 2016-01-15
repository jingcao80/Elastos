
#ifndef __CIAPPLICATIONTOKENNATIVE_H__
#define __CIAPPLICATIONTOKENNATIVE_H__

#include "_CIApplicationTokenNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIApplicationTokenNative)
{
public:
    ~CIApplicationTokenNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI WindowsDrawn();

    CARAPI WindowsVisible();

    CARAPI WindowsGone();

    CARAPI KeyDispatchingTimedOut(
        /* [out] */ Boolean* res);

    CARAPI GetKeyDispatchingTimeout(
        /* [out] */ Int64* timeOut);

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

#endif // __CIAPPLICATIONTOKENNATIVE_H__
