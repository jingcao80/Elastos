
#ifndef __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CInputMethodServiceNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Internal::View::IIInputSessionCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputBinding;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CInputMethodServiceNative)
    , public Object
    , public IIInputMethod
    , public IBinder
{
public:
    ~CInputMethodServiceNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI AttachToken(
        /* [in] */ IBinder* token);

    CARAPI BindInput(
        /* [in] */ IInputBinding* binding);

    CARAPI UnbindInput();

    CARAPI StartInput(
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute);

    CARAPI RestartInput(
        /* [in] */ IIInputContext* inputContext,
        /* [in] */ IEditorInfo* attribute);

    CARAPI CreateSession(
        /* [in] */ IInputChannel* channel,
        /* [in] */ IIInputSessionCallback* inputSessionCalllback);

    CARAPI SetSessionEnabled(
        /* [in] */ IIInputMethodSession* session,
        /* [in] */ Boolean enabled);

    CARAPI RevokeSession(
        /* [in] */ IIInputMethodSession* session);

    CARAPI ShowSoftInput(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    CARAPI HideSoftInput(
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    CARAPI ChangeInputMethodSubtype(
        /* [in] */ IInputMethodSubtype* subtype);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CINPUTMETHODSERVICENATIVE_H__
