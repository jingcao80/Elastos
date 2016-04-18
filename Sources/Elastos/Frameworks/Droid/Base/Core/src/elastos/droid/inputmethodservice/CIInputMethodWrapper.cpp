
#include "elastos/droid/inputmethodservice/CIInputMethodWrapper.h"
#include "elastos/droid/inputmethodservice/CIInputMethodSessionWrapper.h"
#include "elastos/droid/internal/view/CInputConnectionWrapper.h"
#include "elastos/droid/view/inputmethod/CInputBinding.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include <elastos/utility/logging/Logger.h>
#include <stdio.h>
#include <unistd.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::InputMethodService::CIInputMethodSessionWrapper;
using Elastos::Droid::Internal::View::CInputConnectionWrapper;
using Elastos::Droid::Internal::View::EIID_IIInputMethod;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::View::IIInputMethodSession;
using Elastos::Droid::View::InputMethod::IInputMethod;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::CInputBinding;
using Elastos::Droid::View::InputMethod::EIID_IInputMethodSessionCallback;
using Elastos::Droid::View::InputMethod::EIID_IInputMethod;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

const String CIInputMethodWrapper::TAG("CIInputMethodWrapper");
const Boolean CIInputMethodWrapper::DEBUG = FALSE;
const Int32 CIInputMethodWrapper::DO_DUMP = 1;
const Int32 CIInputMethodWrapper::DO_ATTACH_TOKEN = 10;
const Int32 CIInputMethodWrapper::DO_SET_INPUT_CONTEXT = 20;
const Int32 CIInputMethodWrapper::DO_UNSET_INPUT_CONTEXT = 30;
const Int32 CIInputMethodWrapper::DO_START_INPUT = 32;
const Int32 CIInputMethodWrapper::DO_RESTART_INPUT = 34;
const Int32 CIInputMethodWrapper::DO_CREATE_SESSION = 40;
const Int32 CIInputMethodWrapper::DO_SET_SESSION_ENABLED = 45;
const Int32 CIInputMethodWrapper::DO_REVOKE_SESSION = 50;
const Int32 CIInputMethodWrapper::DO_SHOW_SOFT_INPUT = 60;
const Int32 CIInputMethodWrapper::DO_HIDE_SOFT_INPUT = 70;
const Int32 CIInputMethodWrapper::DO_CHANGE_INPUTMETHOD_SUBTYPE = 80;

CAR_INTERFACE_IMPL(CIInputMethodWrapper::InputMethodSessionCallbackWrapper, Object, IInputMethodSessionCallback)
CIInputMethodWrapper::InputMethodSessionCallbackWrapper::InputMethodSessionCallbackWrapper(
    /* [in] */ IContext* context,
    /* [in] */ IInputChannel* channel,
    /* [in] */ IIInputSessionCallback* cb)
    : mContext(context)
    , mChannel(channel)
    , mCb(cb)
{}

ECode CIInputMethodWrapper::InputMethodSessionCallbackWrapper::SessionCreated(
    /* [in] */ IInputMethodSession* session)
{
    // try {
    if (session != NULL) {
        AutoPtr<IIInputMethodSession> wrap;
        CIInputMethodSessionWrapper::New(mContext, session, mChannel, (IIInputMethodSession**)&wrap);
        return mCb->SessionCreated(wrap);
    }
    else {
        if (mChannel != NULL) {
            mChannel->Dispose();
        }
        return mCb->SessionCreated(NULL);
    }
    // } catch (RemoteException e) {
    // }
}

CAR_OBJECT_IMPL(CIInputMethodWrapper);
CAR_INTERFACE_IMPL_3(CIInputMethodWrapper, Object, IIInputMethod, IBinder, IHandlerCallerCallback);
ECode CIInputMethodWrapper::constructor(
    /* [in] */ IAbstractInputMethodService* context,
    /* [in] */ IInputMethod* inputMethod)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(context);
    if (wrs) wrs->GetWeakReference((IWeakReference**)&mTarget);
    wrs = IWeakReferenceSource::Probe(inputMethod);
    if (wrs) wrs->GetWeakReference((IWeakReference**)&mInputMethod);

    IContext::Probe(context)->GetApplicationContext((IContext**)&mContext);
    CHandlerCaller::New(mContext, NULL, this,
        TRUE /*asyncHandler*/, FALSE, (IHandlerCaller**)&mCaller);
    AutoPtr<IApplicationInfo> appInfo;
    IContext::Probe(context)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    assert(appInfo != NULL);
    appInfo->GetTargetSdkVersion(&mTargetSdkVersion);
    return NOERROR;
}

AutoPtr<IInputMethod> CIInputMethodWrapper::GetInternalInputMethod()
{
    AutoPtr<IInputMethod> im;
    if (mInputMethod) {
        mInputMethod->Resolve(EIID_IInputMethod, (IInterface**)&im);
    }

    return im;
}

ECode CIInputMethodWrapper::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<IInputMethod> inputMethod = GetInternalInputMethod();
    // Need a valid reference to the inputMethod for everything except a dump.
    if (inputMethod == NULL && what != DO_DUMP) {
        Logger::W(TAG, "Input method reference was NULL, ignoring message: %d", what);
        return  NOERROR;
    }

    switch (what) {
        case DO_DUMP: {
            AutoPtr<IAbstractInputMethodService> target;
            mTarget->Resolve(EIID_IAbstractInputMethodService, (IInterface**)&target);
            if (target == NULL) {
                return NOERROR;
            }
            SomeArgs* args = (SomeArgs*)IObject::Probe(obj);

            // try {
            //     target.dump((FileDescriptor)args->mArg1,
            //             (PrintWriter)args->mArg2, (String[])args->mArg3);
            // } catch (RuntimeException e) {
            //     ((PrintWriter)args->mArg2).println("Exception: " + e);
            // }
            // synchronized(args->mArg4) {
            //     ((CountDownLatch)args->mArg4).countDown();
            // }
            args->Recycle();
            return NOERROR;
        }

        case DO_ATTACH_TOKEN: {
            inputMethod->AttachToken(IBinder::Probe(obj));
            return NOERROR;
        }
        case DO_SET_INPUT_CONTEXT: {
            inputMethod->BindInput(IInputBinding::Probe(obj));
            return NOERROR;
        }
        case DO_UNSET_INPUT_CONTEXT:
            inputMethod->UnbindInput();
            return NOERROR;
        case DO_START_INPUT: {
            SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            IIInputContext* inputContext = IIInputContext::Probe(args->mArg1);
            AutoPtr<IInputConnection> ic;
            if (inputContext != NULL)
                CInputConnectionWrapper::New(inputContext, (IInputConnection**)&ic);
            IEditorInfo* info = IEditorInfo::Probe(args->mArg2);
            info->MakeCompatible(mTargetSdkVersion);
            inputMethod->StartInput(ic, info);
            args->Recycle();
            return NOERROR;
        }
        case DO_RESTART_INPUT: {
            SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            IIInputContext* inputContext = IIInputContext::Probe(args->mArg1);
            AutoPtr<IInputConnection> ic;
            if (inputContext != NULL)
                CInputConnectionWrapper::New(inputContext, (IInputConnection**)&ic);
            IEditorInfo* info = IEditorInfo::Probe(args->mArg2);
            info->MakeCompatible(mTargetSdkVersion);
            inputMethod->RestartInput(ic, info);
            args->Recycle();
            return NOERROR;
        }
        case DO_CREATE_SESSION: {
            SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            AutoPtr<IInputMethodSessionCallback> cb = new InputMethodSessionCallbackWrapper(
                    mContext, IInputChannel::Probe(args->mArg1), IIInputSessionCallback::Probe(args->mArg2));
            inputMethod->CreateSession(cb);
            args->Recycle();
            return NOERROR;
        }
        case DO_SET_SESSION_ENABLED:
            inputMethod->SetSessionEnabled(IInputMethodSession::Probe(obj), arg1 != 0);
            return NOERROR;
        case DO_REVOKE_SESSION:
            inputMethod->RevokeSession(IInputMethodSession::Probe(obj));
            return NOERROR;
        case DO_SHOW_SOFT_INPUT:
            inputMethod->ShowSoftInput(arg1, IResultReceiver::Probe(obj));
            return NOERROR;
        case DO_HIDE_SOFT_INPUT:
            inputMethod->HideSoftInput(arg1, IResultReceiver::Probe(obj));
            return NOERROR;
        case DO_CHANGE_INPUTMETHOD_SUBTYPE:
            inputMethod->ChangeInputMethodSubtype(IInputMethodSubtype::Probe(obj));
            return NOERROR;
    }

    Logger::W(TAG, "Unhandled message code: %d", what);
    return NOERROR;
}

ECode CIInputMethodWrapper::AttachToken(
    /* [in] */ IBinder* token)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_ATTACH_TOKEN, token, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::BindInput(
    /* [in] */ IInputBinding* binding)
{
    AutoPtr<IBinder> binder;
    FAIL_RETURN(binding->GetConnectionToken((IBinder**)&binder));
    AutoPtr<IInputConnection> ic;
    CInputConnectionWrapper::New(IIInputContext::Probe(binder), (IInputConnection**)&ic);
    AutoPtr<IInputBinding> nu;
    FAIL_RETURN(CInputBinding::New(ic, binding, (IInputBinding**)&nu));

    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_SET_INPUT_CONTEXT, nu, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::UnbindInput()
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(DO_UNSET_INPUT_CONTEXT, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::StartInput(
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_START_INPUT, inputContext, attribute, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::RestartInput(
    /* [in] */ IIInputContext* inputContext,
    /* [in] */ IEditorInfo* attribute)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_RESTART_INPUT, inputContext, attribute, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::CreateSession(
    /* [in] */ IInputChannel* channel,
    /* [in] */ IIInputSessionCallback* callback)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_CREATE_SESSION, channel, callback, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::SetSessionEnabled(
    /* [in] */ IIInputMethodSession* session,
    /* [in] */ Boolean enabled)
{
    // try {
    AutoPtr<IInputMethodSession> ls = ((CIInputMethodSessionWrapper*)
            session)->GetInternalInputMethodSession();

    if (ls == NULL) {
        Logger::W(TAG, "Session is already finished: 0x%8x", session);
        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageIO(DO_SET_SESSION_ENABLED, enabled ? 1 : 0, ls, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::RevokeSession(
    /* [in] */ IIInputMethodSession* session)
{
    // try {
    AutoPtr<IInputMethodSession> ls = ((CIInputMethodSessionWrapper*)
            session)->GetInternalInputMethodSession();

    if (ls == NULL) {
        Logger::W(TAG, "Session is already finished: 0x%8x", session);
        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_REVOKE_SESSION, ls, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::ShowSoftInput(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageIO(DO_SHOW_SOFT_INPUT, flags, resultReceiver, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::HideSoftInput(
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageIO(DO_HIDE_SOFT_INPUT, flags, resultReceiver, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::ChangeInputMethodSubtype(
    /* [in] */ IInputMethodSubtype* subtype)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_CHANGE_INPUTMETHOD_SUBTYPE, subtype, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodWrapper::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("CIInputMethodWrapper");
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
