
#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCLIENTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCLIENTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputMethodClientNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::View::IInputBindResult;
using Elastos::Droid::Internal::View::IInputMethodClient;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodClientNative)
    , public Object
    , public IInputMethodClient
    , public IBinder
{
public:
    ~CInputMethodClientNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetUsingInputMethod(
        /* [in] */ Boolean state);

    CARAPI OnBindMethod(
        /* [in] */ IInputBindResult* res);

    CARAPI OnUnbindMethod(
        /* [in] */ Int32 sequence);

    CARAPI SetActive(
        /* [in] */ Boolean active);

    CARAPI SetUserActionNotificationSequenceNumber(
        /* [in] */ Int32 sequenceNumber);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODCLIENTNATIVE_H__
