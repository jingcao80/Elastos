
#ifndef __CIONPRIMARYCLIPCHANGEDLISTENERNATIVE_H__
#define __CIONPRIMARYCLIPCHANGEDLISTENERNATIVE_H__

#include "_CIOnPrimaryClipChangedListenerNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIOnPrimaryClipChangedListenerNative)
{
public:
    ~CIOnPrimaryClipChangedListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI DispatchPrimaryClipChanged();

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CIONPRIMARYCLIPCHANGEDLISTENERNATIVE_H__
