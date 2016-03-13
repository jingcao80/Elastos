
#ifndef __ELASTOS_DROID_JAVAPROXY_CIOBBACTIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIOBBACTIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIObbActionListenerNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIObbActionListenerNative)
{
public:
    ~CIObbActionListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnObbResult(
        /* [in] */ const String& filename,
        /* [in] */ Int32 nonce,
        /* [in] */ Int32 status);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIOBBACTIONLISTENERNATIVE_H__
