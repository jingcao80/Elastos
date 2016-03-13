
#ifndef __ELASTOS_DROID_JAVAPROXY_CLOCALACTIVITYRECORDNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CLOCALACTIVITYRECORDNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CLocalActivityRecordNative.h"
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

#endif // __ELASTOS_DROID_JAVAPROXY_CLOCALACTIVITYRECORDNATIVE_H__
