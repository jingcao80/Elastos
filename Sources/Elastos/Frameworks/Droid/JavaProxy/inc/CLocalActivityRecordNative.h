
#ifndef __CLOCALACTIVITYRECORDNATIVE_H__
#define __CLOCALACTIVITYRECORDNATIVE_H__

#include "_CLocalActivityRecordNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CLocalActivityRecordNative)
{
public:
    ~CLocalActivityRecordNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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

#endif // __CLOCALACTIVITYRECORDNATIVE_H__
