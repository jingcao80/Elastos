
#ifndef __CINPUTMETHODCALLBACKNATIVE_H__
#define __CINPUTMETHODCALLBACKNATIVE_H__

#include "_CInputMethodCallbackNative.h"
#include <jni.h>

using Elastos::Droid::Internal::View::IIInputMethodSession;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodCallbackNative)
{
public:
    ~CInputMethodCallbackNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI FinishedEvent(
        /* [in] */ Int32 seq,
        /* [in] */ Boolean handled);

    CARAPI SessionCreated(
        /* [in] */ IIInputMethodSession* session);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CINPUTMETHODCALLBACKNATIVE_H__
