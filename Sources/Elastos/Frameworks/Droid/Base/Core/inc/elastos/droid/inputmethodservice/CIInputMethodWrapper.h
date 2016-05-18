
#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CIINPUTMETHODWRAPPER_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CIINPUTMETHODWRAPPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_InputMethodService_CIInputMethodWrapper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::Os::IHandlerCaller;
using Elastos::Droid::Internal::Os::IHandlerCallerCallback;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IInputMethodCallback;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::Internal::View::IIInputMethod;
using Elastos::Droid::Internal::View::IIInputSessionCallback;
using Elastos::Droid::View::InputMethod::IInputBinding;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::IInputMethodSubtype;
using Elastos::Droid::View::InputMethod::IInputMethodSessionCallback;
using Elastos::Droid::View::InputMethod::IInputMethod;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

/**
 * Implements the internal IInputMethod interface to convert incoming calls
 * on to it back to calls on the public InputMethod interface, scheduling
 * them on the main thread of the process.
 */
CarClass(CIInputMethodWrapper)
    , public Object
    , public IIInputMethod
    , public IBinder
    , public IHandlerCallerCallback
{
private:
    // static class Notifier {
    //     Boolean notified;
    // };

    // NOTE: we should have a cache of these.
    class InputMethodSessionCallbackWrapper
        : public Object
        , public IInputMethodSessionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InputMethodSessionCallbackWrapper(
            /* [in] */ IContext* context,
            /* [in] */ IInputChannel* channel,
            /* [in] */ IIInputSessionCallback* cb);

        CARAPI SessionCreated(
            /* [in] */ IInputMethodSession* session);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IInputChannel> mChannel;
        AutoPtr<IIInputSessionCallback> mCb;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IAbstractInputMethodService* context,
        /* [in] */ IInputMethod* inputMethod);

    CARAPI_(AutoPtr<IInputMethod>) GetInternalInputMethod();

    CARAPI ExecuteMessage(
        /* [in] */ IMessage* msg);

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
        /* [in] */ IIInputSessionCallback* callback);

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
    static const Boolean DEBUG;

    static const Int32 DO_DUMP;
    static const Int32 DO_ATTACH_TOKEN;
    static const Int32 DO_SET_INPUT_CONTEXT;
    static const Int32 DO_UNSET_INPUT_CONTEXT;
    static const Int32 DO_START_INPUT;
    static const Int32 DO_RESTART_INPUT;
    static const Int32 DO_CREATE_SESSION;
    static const Int32 DO_SET_SESSION_ENABLED;
    static const Int32 DO_REVOKE_SESSION;
    static const Int32 DO_SHOW_SOFT_INPUT;
    static const Int32 DO_HIDE_SOFT_INPUT;
    static const Int32 DO_CHANGE_INPUTMETHOD_SUBTYPE;

    AutoPtr<IWeakReference> mTarget;//final WeakReference<AbstractInputMethodService> ;
    AutoPtr<IContext> mContext;
    AutoPtr<IHandlerCaller> mCaller;
    AutoPtr<IWeakReference> mInputMethod; // WeakReference<InputMethod>
    Int32 mTargetSdkVersion;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ELASTOS_DROID_INPUTMEHTODSERVICE_CIINPUTMETHODWRAPPER_H__
