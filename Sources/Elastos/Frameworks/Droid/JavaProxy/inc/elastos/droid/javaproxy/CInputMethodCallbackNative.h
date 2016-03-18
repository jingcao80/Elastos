
#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCALLBACKNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCALLBACKNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputMethodCallbackNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IInputMethodCallback;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodCallbackNative)
    , public Object
    , public IInputMethodCallback
    , public IBinder
{
public:
    ~CInputMethodCallbackNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI FinishedEvent(
        /* [in] */ Int32 seq,
        /* [in] */ Boolean handled);

    CARAPI SessionCreated(
        /* [in] */ IIInputMethodSession* session);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCALLBACKNATIVE_H__
