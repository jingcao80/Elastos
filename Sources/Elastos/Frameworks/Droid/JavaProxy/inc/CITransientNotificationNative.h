
#ifndef __CITRANSIENTNOTIFICATIONNATIVE_H__
#define __CITRANSIENTNOTIFICATIONNATIVE_H__

#include "_CITransientNotificationNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CITransientNotificationNative)
{
public:
    ~CITransientNotificationNative();
    
    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI Show();

    CARAPI Hide();

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

#endif // __CITRANSIENTNOTIFICATIONNATIVE_H__
