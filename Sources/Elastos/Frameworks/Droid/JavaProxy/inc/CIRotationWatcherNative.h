
#ifndef __CIROTATIONWATCHERNATIVE_H__
#define __CIROTATIONWATCHERNATIVE_H__

#include "_CIRotationWatcherNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRotationWatcherNative)
{
public:
    ~CIRotationWatcherNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

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

#endif // __CIROTATIONWATCHERNATIVE_H__
