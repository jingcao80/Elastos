
#ifndef __CIPACKAGEMOVEOBSERVERNATIVE_H__
#define __CIPACKAGEMOVEOBSERVERNATIVE_H__

#include "_CIPackageMoveObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIPackageMoveObserverNative)
{
public:
    ~CIPackageMoveObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI PackageMoved(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 returnCode);

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

#endif // __CIPACKAGEMOVEOBSERVERNATIVE_H__
