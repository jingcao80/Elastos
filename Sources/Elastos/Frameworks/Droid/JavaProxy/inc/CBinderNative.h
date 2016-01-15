
#ifndef __CBINDERNATIVE_H__
#define __CBINDERNATIVE_H__

#include "_CBinderNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CBinderNative)
{
public:
    ~CBinderNative();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // JavaProxy
} // Droid
} // Elastos

#endif // __CBINDERNATIVE_H__
