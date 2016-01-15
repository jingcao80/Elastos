
#ifndef __CIOBBACTIONLISTENERNATIVE_H__
#define __CIOBBACTIONLISTENERNATIVE_H__

#include "_CIObbActionListenerNative.h"
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

#endif // __CIOBBACTIONLISTENERNATIVE_H__
