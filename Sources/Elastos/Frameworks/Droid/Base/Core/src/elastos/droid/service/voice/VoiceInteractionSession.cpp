#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/inputmethodservice/CSoftInputWindow.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/service/voice/CVoiceInteractionSessionCaller.h"
#include "elastos/droid/service/voice/CVoiceInteractionSessionInsets.h"
#include "elastos/droid/service/voice/CVoiceInteractionSessionRequest.h"
#include "elastos/droid/service/voice/VoiceInteractionSession.h"
#include "elastos/droid/app/Instrumentation.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/view/CDispatcherState.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::Instrumentation;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::InputMethodService::EIID_ISoftInputWindowCallback;
using Elastos::Droid::Internal::App::EIID_IIVoiceInteractor;
using Elastos::Droid::Internal::App::EIID_IIVoiceInteractorRequest;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::InputMethodService::CSoftInputWindow;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Service::Voice::CVoiceInteractionSessionCaller;
using Elastos::Droid::Service::Voice::CVoiceInteractionSessionInsets;
using Elastos::Droid::Service::Voice::CVoiceInteractionSessionRequest;
using Elastos::Droid::Service::Voice::IVoiceInteractionSessionInsets;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::CDispatcherState;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IOnComputeInternalInsetsListener;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::R;
using Elastos::Core::CArrayOf;
using Elastos::Core::CBoolean;
using Elastos::Core::CString;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::EIID_IString;
using Elastos::Core::IArrayOf;
using Elastos::Core::IBoolean;
using Elastos::Core::IString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

//=============================================
// VoiceInteractionSession::MInteractor
//=============================================

CAR_INTERFACE_IMPL_2(VoiceInteractionSession::MInteractor, Object, IIVoiceInteractor, IBinder)

VoiceInteractionSession::MInteractor::MInteractor(
    /* [in] */ VoiceInteractionSession* host)
    : mHost(host)
{
}

ECode VoiceInteractionSession::MInteractor::StartConfirmation(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [in] */ ICharSequence* prompt,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IVoiceInteractionSessionRequest> request;
    mHost->NewRequest(callback, (IVoiceInteractionSessionRequest**)&request);
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IVoiceInteractionSessionCaller>  caller;
    CVoiceInteractionSessionCaller::New(callingPackage, uid, (IVoiceInteractionSessionCaller**)&caller);
    AutoPtr<IMessage> message;
    mHost->mHandlerCaller->ObtainMessageOOOO(VoiceInteractionSession::MSG_START_CONFIRMATION,
        caller.Get(), request.Get(), prompt, extras, (IMessage**)&message);
    mHost->mHandlerCaller->SendMessage(message.Get());
    AutoPtr<Request> _request = (Request*)request.Get();
    *result =  _request->mInterface;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode VoiceInteractionSession::MInteractor::StartCompleteVoice(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IVoiceInteractionSessionRequest> request;
    mHost->NewRequest(callback, (IVoiceInteractionSessionRequest**)&request);
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IVoiceInteractionSessionCaller>  caller;
    CVoiceInteractionSessionCaller::New(callingPackage, uid, (IVoiceInteractionSessionCaller**)&caller);
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessageOOOO(VoiceInteractionSession::MSG_START_COMPLETE_VOICE,
        caller.Get(), request.Get(), message, extras, (IMessage**)&msg);
    mHost->mHandlerCaller->SendMessage(msg.Get());
    AutoPtr<Request> _request = (Request*)request.Get();
    *result =  _request->mInterface;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode VoiceInteractionSession::MInteractor::StartAbortVoice(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IVoiceInteractionSessionRequest> request;
    mHost->NewRequest(callback, (IVoiceInteractionSessionRequest**)&request);
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IVoiceInteractionSessionCaller>  caller;
    CVoiceInteractionSessionCaller::New(callingPackage, uid, (IVoiceInteractionSessionCaller**)&caller);
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessageOOOO(VoiceInteractionSession::MSG_START_ABORT_VOICE,
        caller.Get(), request.Get(), message, extras, (IMessage**)&msg);
    mHost->mHandlerCaller->SendMessage(msg.Get());
    AutoPtr<Request> _request = (Request*)request.Get();
    *result =  _request->mInterface;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode VoiceInteractionSession::MInteractor::StartCommand(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IVoiceInteractionSessionRequest> request;
    mHost->NewRequest(callback, (IVoiceInteractionSessionRequest**)&request);
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IVoiceInteractionSessionCaller>  caller;
    CVoiceInteractionSessionCaller::New(callingPackage, uid, (IVoiceInteractionSessionCaller**)&caller);
    AutoPtr<IMessage> message;
    mHost->mHandlerCaller->ObtainMessageOOOO(VoiceInteractionSession::MSG_START_COMMAND,
        IInterface::Probe(caller), IInterface::Probe(request), IInterface::Probe((IString*)&command), IInterface::Probe(extras), (IMessage**)&message);
    mHost->mHandlerCaller->SendMessage(message.Get());
    AutoPtr<Request> _request = (Request*)request.Get();
    *result =  _request->mInterface;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode VoiceInteractionSession::MInteractor::SupportsCommands(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<String>* commands,
    /* [out, callee] */ ArrayOf<Boolean>** results)
{
    VALIDATE_NOT_NULL(results)
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IVoiceInteractionSessionCaller>  caller;
    CVoiceInteractionSessionCaller::New(callingPackage, uid, (IVoiceInteractionSessionCaller**)&caller);

    Int32 length = commands->GetLength();
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IString, length, (IArrayOf**)&array);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IString> str;
        CString::New((*commands)[i], (IString**)&str);
        AutoPtr<IInterface> obj = IInterface::Probe(str);
        array->Set(i, obj.Get());
    }
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessageIOO(VoiceInteractionSession::MSG_SUPPORTS_COMMANDS,
        0, IInterface::Probe(caller), IInterface::Probe(array), (IMessage**)&msg);
    AutoPtr<ISomeArgs> args;
    mHost->mHandlerCaller->SendMessageAndWait(msg.Get(), (ISomeArgs**)&args);
    if (args != NULL) {
        AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
        AutoPtr<IArrayOf> array = IArrayOf::Probe(_args->mArg1);
        Int32 size;
        array->GetLength(&size);
        AutoPtr<ArrayOf<Boolean> > res = ArrayOf<Boolean>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            array->Get(i, (IInterface**)&obj);
            AutoPtr<IBoolean> element = IBoolean::Probe(obj);
            Boolean value = element->GetValue(&value);
            (*res)[i] = value;
        }
        args->Recycle();
        *results = res.Get();
    }
    else {
        *results = ArrayOf<Boolean>::Alloc(commands->GetLength());
    }
    REFCOUNT_ADD(*results)
    return NOERROR;
}

//=============================================
// VoiceInteractionSession::MSession
//=============================================

CAR_INTERFACE_IMPL_2(VoiceInteractionSession::MSession, Object, IIVoiceInteractionSession, IBinder)

VoiceInteractionSession::MSession::MSession(
    /* [in] */ VoiceInteractionSession* host)
    : mHost(host)
{
}

ECode VoiceInteractionSession::MSession::TaskStarted(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessageIO(VoiceInteractionSession::MSG_TASK_STARTED, taskId, intent, (IMessage**)&msg);
    mHost->mHandlerCaller->SendMessage(msg.Get());
    return NOERROR;
}

ECode VoiceInteractionSession::MSession::TaskFinished(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessageIO(VoiceInteractionSession::MSG_TASK_FINISHED, taskId, intent, (IMessage**)&msg);
    mHost->mHandlerCaller->SendMessage(msg.Get());
    return NOERROR;
}

ECode VoiceInteractionSession::MSession::CloseSystemDialogs()
{
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessage(VoiceInteractionSession::MSG_CLOSE_SYSTEM_DIALOGS, (IMessage**)&msg);
    mHost->mHandlerCaller->SendMessage(msg.Get());
    return NOERROR;
}

ECode VoiceInteractionSession::MSession::Destroy()
{
    AutoPtr<IMessage> msg;
    mHost->mHandlerCaller->ObtainMessage(VoiceInteractionSession::MSG_DESTROY, (IMessage**)&msg);
    mHost->mHandlerCaller->SendMessage(msg.Get());
    return NOERROR;
}

//=============================================
// VoiceInteractionSession::Request::MInterface
//=============================================

CAR_INTERFACE_IMPL_2(VoiceInteractionSession::Request::MInterface, Object, IIVoiceInteractorRequest, IBinder)

VoiceInteractionSession::Request::MInterface::MInterface(
    /* [in] */ Request* host)
    : mHost(host)
{
}

ECode VoiceInteractionSession::Request::MInterface::Cancel()
{
    AutoPtr<IVoiceInteractionSession> session;
    mHost->mSession->Resolve(EIID_IVoiceInteractionSession, (IInterface**)&session);
    if (session != NULL) {
        AutoPtr<VoiceInteractionSession> _session = (VoiceInteractionSession*)session.Get();
        AutoPtr<IMessage> msg;
        _session->mHandlerCaller->ObtainMessageO(VoiceInteractionSession::MSG_CANCEL, (IVoiceInteractionSessionRequest*)mHost, (IMessage**)&msg);
        _session->mHandlerCaller->SendMessage(msg.Get());
    }
    return NOERROR;
}

//=============================================
// VoiceInteractionSession::Request
//=============================================

CAR_INTERFACE_IMPL(VoiceInteractionSession::Request, Object, IVoiceInteractionSessionRequest)

VoiceInteractionSession::Request::Request()
{
    AutoPtr<MInterface> mi = new MInterface(this);
    mInterface = mi.Get();
}

ECode VoiceInteractionSession::Request::SendConfirmResult(
    /* [in] */ Boolean confirmed,
    /* [in] */ IBundle* result)
{
    if (VoiceInteractionSession::DEBUG) {
        String str1, str2;
        IObject::Probe(mInterface)->ToString(&str1);
        IObject::Probe(result)->ToString(&str2);
        Logger::D(VoiceInteractionSession::TAG, "sendConfirmResult: req=%s confirmed=%s result=%s", str1.string(),
            confirmed ? "TRUE" : "FALSE" , str2.string());
    }
    FinishRequest();
    ECode ec = mCallback->DeliverConfirmationResult(mInterface, confirmed, result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::Request::SendCompleteVoiceResult(
    /* [in] */ IBundle* result)
{
    if (VoiceInteractionSession::DEBUG) {
        String str1, str2;
        IObject::Probe(mInterface)->ToString(&str1);
        IObject::Probe(result)->ToString(&str2);
        Logger::D(VoiceInteractionSession::TAG, "sendCompleteVoiceResult: req=%s result=%s", str1.string(), str2.string());
    }
    FinishRequest();
    ECode ec = mCallback->DeliverCompleteVoiceResult(mInterface, result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::Request::SendAbortVoiceResult(
    /* [in] */ IBundle* result)
{
    if (VoiceInteractionSession::DEBUG) {
        String str1, str2;
        IObject::Probe(mInterface)->ToString(&str1);
        IObject::Probe(result)->ToString(&str2);
        Logger::D(VoiceInteractionSession::TAG, "sendConfirmResult: req=%s result=%s", str1.string(), str2.string());
    }
    FinishRequest();
    ECode ec = mCallback->DeliverAbortVoiceResult(mInterface, result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::Request::SendCommandResult(
    /* [in] */ Boolean complete,
    /* [in] */ IBundle* result)
{
    if (VoiceInteractionSession::DEBUG) {
        String str1, str2;
        IObject::Probe(mInterface)->ToString(&str1);
        IObject::Probe(result)->ToString(&str2);
        Logger::D(VoiceInteractionSession::TAG, "sendCommandResult: req=%s result=%s", str1.string(), str2.string());
    }
    FinishRequest();
    ECode ec = mCallback->DeliverCommandResult(mInterface, complete, result);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::Request::SendCancelResult()
{
    if (VoiceInteractionSession::DEBUG) {
        String str;
        IObject::Probe(mInterface)->ToString(&str);
        Logger::D(VoiceInteractionSession::TAG, "sendCancelResult: req=%s", str.string());
    }
    FinishRequest();
    ECode ec = mCallback->DeliverCancel(mInterface);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::Request::constructor(
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [in] */ IVoiceInteractionSession* session)
{
    mCallback = callback;
    IWeakReferenceSource::Probe(session)->GetWeakReference((IWeakReference**)&mSession);
    return NOERROR;
}

ECode VoiceInteractionSession::Request::FinishRequest()
{
    AutoPtr<IVoiceInteractionSession> session;
    mSession->Resolve(EIID_IVoiceInteractionSession, (IInterface**)&session);
    if (session == NULL) {
        Logger::E(VoiceInteractionSession::TAG, "VoiceInteractionSession has been destroyed");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IVoiceInteractionSessionRequest> req;
    session->RemoveRequest(IBinder::Probe(mInterface), (IVoiceInteractionSessionRequest**)&req);
    if (req == NULL) {
        String str;
        this->ToString(&str);
        Logger::E(VoiceInteractionSession::TAG, "Request not active: %s", str.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (req != this) {
        String str1, str2;
        IObject::Probe(req)->ToString(&str1);
        this->ToString(&str2);
        Logger::E(VoiceInteractionSession::TAG, "Current active request %s not same as calling request %s", str1.string(), str2.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

//=============================================
// VoiceInteractionSession::Caller
//=============================================

CAR_INTERFACE_IMPL(VoiceInteractionSession::Caller, Object, IVoiceInteractionSessionCaller)

VoiceInteractionSession::Caller::Caller()
{
}

ECode VoiceInteractionSession::Caller::constructor(
    /* [in] */ const String& _packageName,
    /* [in] */ Int32 _uid)
{
    mPackageName = _packageName;
    mUid = _uid;
    return NOERROR;
}

//=============================================
// VoiceInteractionSession::MyCallbacks
//=============================================

CAR_INTERFACE_IMPL_2(VoiceInteractionSession::MyCallbacks, Object, IHandlerCallerCallback, ISoftInputWindowCallback)

VoiceInteractionSession::MyCallbacks::MyCallbacks(
    /* [in] */ VoiceInteractionSession* host)
    : mHost(host)
{
}

ECode VoiceInteractionSession::MyCallbacks::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<ISomeArgs> args;
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case VoiceInteractionSession::MSG_START_CONFIRMATION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = ISomeArgs::Probe(obj);
            AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
            if (VoiceInteractionSession::DEBUG) {
                AutoPtr<IVoiceInteractionSessionRequest> request = IVoiceInteractionSessionRequest::Probe(_args->mArg2);
                AutoPtr<Request> _request = (Request*)request.Get();
                String str1, str2, str3;
                IObject::Probe(_request->mInterface)->ToString(&str1);
                IObject::Probe(_args->mArg3)->ToString(&str2);
                IObject::Probe(_args->mArg4)->ToString(&str3);
                Logger::D(VoiceInteractionSession::TAG, "onConfirm: req=%s prompt=%s extras=%s", str1.string(), str2.string(), str3.string());
            }
            mHost->OnConfirm(IVoiceInteractionSessionCaller::Probe(_args->mArg1),
                IVoiceInteractionSessionRequest::Probe(_args->mArg2),
                ICharSequence::Probe(_args->mArg3),
                IBundle::Probe(_args->mArg4));
            break;
        }
        case VoiceInteractionSession::MSG_START_COMPLETE_VOICE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = ISomeArgs::Probe(obj);
            AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
            if (VoiceInteractionSession::DEBUG) {
                AutoPtr<IVoiceInteractionSessionRequest> request = IVoiceInteractionSessionRequest::Probe(_args->mArg2);
                AutoPtr<Request> _request = (Request*)request.Get();
                String str1, str2, str3;
                IObject::Probe(_request->mInterface)->ToString(&str1);
                IObject::Probe(_args->mArg3)->ToString(&str2);
                IObject::Probe(_args->mArg4)->ToString(&str3);
                Logger::D(VoiceInteractionSession::TAG, "onCompleteVoice: req=%s message=%s extras=%s", str1.string(), str2.string(), str3.string());
            }
            mHost->OnCompleteVoice(IVoiceInteractionSessionCaller::Probe(_args->mArg1),
                IVoiceInteractionSessionRequest::Probe(_args->mArg2),
                ICharSequence::Probe(_args->mArg3),
                IBundle::Probe(_args->mArg4));
            break;
        }
        case VoiceInteractionSession::MSG_START_ABORT_VOICE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = ISomeArgs::Probe(obj);
            AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
            if (VoiceInteractionSession::DEBUG) {
                AutoPtr<IVoiceInteractionSessionRequest> request = IVoiceInteractionSessionRequest::Probe(_args->mArg2);
                AutoPtr<Request> _request = (Request*)request.Get();
                String str1, str2, str3;
                IObject::Probe(_request->mInterface)->ToString(&str1);
                IObject::Probe(_args->mArg3)->ToString(&str2);
                IObject::Probe(_args->mArg4)->ToString(&str3);
                Logger::D(VoiceInteractionSession::TAG, "onAbortVoice: req=%s message=%s extras=%s", str1.string(), str2.string(), str3.string());
            }
            mHost->OnAbortVoice(IVoiceInteractionSessionCaller::Probe(_args->mArg1),
                IVoiceInteractionSessionRequest::Probe(_args->mArg2),
                ICharSequence::Probe(_args->mArg3),
                IBundle::Probe(_args->mArg4));
            break;
        }
        case VoiceInteractionSession::MSG_START_COMMAND: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = ISomeArgs::Probe(obj);
            AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
            if (VoiceInteractionSession::DEBUG) {
                AutoPtr<IVoiceInteractionSessionRequest> request = IVoiceInteractionSessionRequest::Probe(_args->mArg2);
                AutoPtr<Request> _request = (Request*)request.Get();
                String str1, str2, str3;
                IObject::Probe(_request->mInterface)->ToString(&str1);
                IObject::Probe(_args->mArg3)->ToString(&str2);
                IObject::Probe(_args->mArg4)->ToString(&str3);
                Logger::D(VoiceInteractionSession::TAG, "onCommand: req=%s command=%s extras=%s", str1.string(), str2.string(), str3.string());
            }
            String str4;
            ICharSequence::Probe(_args->mArg3)->ToString(&str4);
            mHost->OnCommand(IVoiceInteractionSessionCaller::Probe(_args->mArg1),
                IVoiceInteractionSessionRequest::Probe(_args->mArg2),
                str4, IBundle::Probe(_args->mArg4));
            break;
        }
        case VoiceInteractionSession::MSG_SUPPORTS_COMMANDS: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = ISomeArgs::Probe(obj);
            AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
            if (VoiceInteractionSession::DEBUG) {
                String str;
                IObject::Probe(_args->mArg2)->ToString(&str);
                Logger::D(VoiceInteractionSession::TAG, "onGetSupportedCommands: cmds=%s", str.string());
            }
            AutoPtr<IArrayOf> array = IArrayOf::Probe(_args->mArg2);
            Int32 size;
            array->GetLength(&size);
            AutoPtr<ArrayOf<String> > commands = ArrayOf<String>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                array->Get(i, (IInterface**)&obj);
                AutoPtr<IString> element = IString::Probe(obj);
                String value;
                IObject::Probe(element)->ToString(&value);
                (*commands)[i] = value;
            }
            AutoPtr<ArrayOf<Boolean> > results;
            mHost->OnGetSupportedCommands(IVoiceInteractionSessionCaller::Probe(_args->mArg1), commands.Get(), (ArrayOf<Boolean>**)&results);
            AutoPtr<IArrayOf> arrayoutput;
            CArrayOf::New(EIID_IBoolean, results->GetLength(), (IArrayOf**)&arrayoutput);
            for (Int32 i = 0; i < results->GetLength(); i++) {
                AutoPtr<IBoolean> element;
                CBoolean::New((*results)[i], (IBoolean**)&element);
                arrayoutput->Set(i, element.Get());
            }
            _args->mArg1 = IInterface::Probe(arrayoutput);
            break;
        }
        case VoiceInteractionSession::MSG_CANCEL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            args = ISomeArgs::Probe(obj);
            AutoPtr<SomeArgs> _args = (SomeArgs*)args.Get();
            if (VoiceInteractionSession::DEBUG) {
                AutoPtr<IVoiceInteractionSessionRequest> request = IVoiceInteractionSessionRequest::Probe(_args->mArg2);
                AutoPtr<Request> _request = (Request*)request.Get();
                String str;
                IObject::Probe(_request->mInterface)->ToString(&str);
                Logger::D(VoiceInteractionSession::TAG, "onCancel: req=%s", str.string());
            }
            mHost->OnCancel(IVoiceInteractionSessionRequest::Probe(_args->mArg1));
            break;
        }
        case VoiceInteractionSession::MSG_TASK_STARTED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (VoiceInteractionSession::DEBUG) {
                String str;
                IObject::Probe(obj)->ToString(&str);
                Logger::D(VoiceInteractionSession::TAG, "onTaskStarted: intent=%s taskId=%d", str.string(), arg1);
            }
            mHost->OnTaskStarted(IIntent::Probe(obj), arg1);
            break;
        }
        case VoiceInteractionSession::MSG_TASK_FINISHED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (VoiceInteractionSession::DEBUG) {
                String str;
                IObject::Probe(obj)->ToString(&str);
                Logger::D(VoiceInteractionSession::TAG, "onTaskFinished: intent=%s taskId=%d", str.string(), arg1);
            }
            mHost->OnTaskFinished(IIntent::Probe(obj), arg1);
            break;
        }
        case VoiceInteractionSession::MSG_CLOSE_SYSTEM_DIALOGS: {
            if (VoiceInteractionSession::DEBUG) Logger::D(VoiceInteractionSession::TAG, "onCloseSystemDialogs");
            mHost->OnCloseSystemDialogs();
            break;
        }
        case VoiceInteractionSession::MSG_DESTROY: {
            if (VoiceInteractionSession::DEBUG) Logger::D(VoiceInteractionSession::TAG, "doDestroy");
            mHost->DoDestroy();
            break;
        }
    }
    return NOERROR;
}

ECode VoiceInteractionSession::MyCallbacks::OnBackPressed()
{
    return mHost->OnBackPressed();
}

//=============================================
// VoiceInteractionSession::Insets
//=============================================

CAR_INTERFACE_IMPL(VoiceInteractionSession::Insets, Object, IVoiceInteractionSessionInsets)

VoiceInteractionSession::Insets::Insets()
    : mTouchableInsets(0)
{
    CRect::New((IRect**)&mContentInsets);
    CRegion::New((IRegion**)&mTouchableRegion);
}

ECode VoiceInteractionSession::Insets::constructor()
{
    return NOERROR;
}

//=============================================
// VoiceInteractionSession::MInsetsComputer
//=============================================

CAR_INTERFACE_IMPL(VoiceInteractionSession::MInsetsComputer, Object, IOnComputeInternalInsetsListener)

VoiceInteractionSession::MInsetsComputer::MInsetsComputer(
    /* [in] */ VoiceInteractionSession* host)
    : mHost(host)
{
}

ECode VoiceInteractionSession::MInsetsComputer::OnComputeInternalInsets(
    /* [in] */ IInternalInsetsInfo* info)
{
    mHost->OnComputeInsets(mHost->mTmpInsets);
    AutoPtr<IRect> contentInsets1, contentInsets2;
    info->GetContentInsets((IRect**)&contentInsets1);
    AutoPtr<Insets> _mTmpInsets = (Insets*)(mHost->mTmpInsets).Get();
    contentInsets2 = _mTmpInsets->mContentInsets;
    contentInsets1->Set(contentInsets2.Get());

    AutoPtr<IRect> visibleInsets;
    info->GetVisibleInsets((IRect**)&visibleInsets);
    visibleInsets->Set(contentInsets2.Get());

    AutoPtr<IRegion> touchableRegion1, touchableRegion2;
    info->GetTouchableRegion((IRegion**)&touchableRegion1);
    touchableRegion2 = _mTmpInsets->mTouchableRegion;
    Boolean isSet;
    touchableRegion1->Set(touchableRegion2.Get(), &isSet);

    Int32 touchableInsets = _mTmpInsets->mTouchableInsets;
    info->SetTouchableInsets(touchableInsets);
    return NOERROR;
}

//=============================================
// VoiceInteractionSession
//=============================================

const String VoiceInteractionSession::TAG("VoiceInteractionSession");
const Boolean VoiceInteractionSession::DEBUG = TRUE;

const Int32 VoiceInteractionSession::MSG_START_CONFIRMATION;
const Int32 VoiceInteractionSession::MSG_START_COMPLETE_VOICE;
const Int32 VoiceInteractionSession::MSG_START_ABORT_VOICE;
const Int32 VoiceInteractionSession::MSG_START_COMMAND;
const Int32 VoiceInteractionSession::MSG_SUPPORTS_COMMANDS;
const Int32 VoiceInteractionSession::MSG_CANCEL;

const Int32 VoiceInteractionSession::MSG_TASK_STARTED;
const Int32 VoiceInteractionSession::MSG_TASK_FINISHED;
const Int32 VoiceInteractionSession::MSG_CLOSE_SYSTEM_DIALOGS;
const Int32 VoiceInteractionSession::MSG_DESTROY;

CAR_INTERFACE_IMPL_2(VoiceInteractionSession, Object, IKeyEventCallback, IVoiceInteractionSession)

VoiceInteractionSession::VoiceInteractionSession()
    : mTheme(0)
    , mInitialized(FALSE)
    , mWindowVisible(FALSE)
    , mWindowWasVisible(FALSE)
    , mInShowWindow(FALSE)
{
    CDispatcherState::New((IDispatcherState**)&mDispatcherState);
    CArrayMap::New((IArrayMap**)&mActiveRequests);
    CVoiceInteractionSessionInsets::New((IVoiceInteractionSessionInsets**)&mTmpInsets);
    mTmpLocation = ArrayOf<Int32>::Alloc(2);
    mCallbacks = new MyCallbacks(this);
    AutoPtr<MInsetsComputer> mic = new MInsetsComputer(this);
    mInsetsComputer = mic.Get();
    AutoPtr<MSession> _mSession = new MSession(this);
    mSession = IIVoiceInteractionSession::Probe(_mSession);
    AutoPtr<MInteractor> _mInteractor = new MInteractor(this);
    mInteractor = IIVoiceInteractor::Probe(_mInteractor);
}

ECode VoiceInteractionSession::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IHandler> h;
    CHandler::New((IHandler**)&h);
    return constructor(context, h.Get());
}

ECode VoiceInteractionSession::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
{
    mContext = context;
    AutoPtr<ILooper> l;
    handler->GetLooper((ILooper**)&l);
    CHandlerCaller::New(context, l.Get(), (IHandlerCallerCallback*)mCallbacks.Get(), TRUE, TRUE, (IHandlerCaller**)&mHandlerCaller);
    return NOERROR;
}

ECode VoiceInteractionSession::NewRequest(
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [out] */ IVoiceInteractionSessionRequest** request)
{
    VALIDATE_NOT_NULL(request)
    {    AutoLock syncLock(this);
        AutoPtr<IVoiceInteractionSessionRequest> req;
        CVoiceInteractionSessionRequest::New(callback, this, (IVoiceInteractionSessionRequest**)&req);
        AutoPtr<Request> _req = (Request*)req.Get();
        mActiveRequests->Put(IBinder::Probe(_req->mInterface), req);
        *request = req.Get();
        REFCOUNT_ADD(*request)
    }
    return NOERROR;
}

ECode VoiceInteractionSession::RemoveRequest(
    /* [in] */ IBinder* reqInterface,
    /* [out] */ IVoiceInteractionSessionRequest** request)
{
    VALIDATE_NOT_NULL(request)
    {    AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mActiveRequests->Get(reqInterface, (IInterface**)&obj);
        AutoPtr<IVoiceInteractionSessionRequest> req = IVoiceInteractionSessionRequest::Probe(obj);
        if (req != NULL) {
            mActiveRequests->Remove(req);
        }
        *request = req.Get();
        REFCOUNT_ADD(*request)
    }
    return NOERROR;
}

ECode VoiceInteractionSession::DoCreate(
    /* [in] */ IIVoiceInteractionManagerService* service,
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* args)
{
    mSystemService = service;
    mToken = token;
    return OnCreate(args);
}

ECode VoiceInteractionSession::DoDestroy()
{
    OnDestroy();
    if (mInitialized) {
        AutoPtr<IViewTreeObserver> observer;
        mRootView->GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->RemoveOnComputeInternalInsetsListener(mInsetsComputer.Get());
        if (mWindowAdded) {
            IDialogInterface::Probe(mWindow)->Dismiss();
            mWindowAdded = FALSE;
        }
        mInitialized = FALSE;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::InitViews()
{
    mInitialized = TRUE;
    AutoPtr< ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::VoiceInteractionSession),
        sizeof(R::styleable::VoiceInteractionSession)/sizeof(R::styleable::VoiceInteractionSession[0]));
    mContext->ObtainStyledAttributes(attrIds, (ITypedArray**)&mThemeAttrs);
    mInflater->Inflate(R::layout::voice_interaction_session, NULL, (IView**)&mRootView);
    mRootView->SetSystemUiVisibility(
            IView::SYSTEM_UI_FLAG_LAYOUT_STABLE | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION);
    IDialog::Probe(mWindow)->SetContentView(mRootView);
    AutoPtr<IViewTreeObserver> observer;
    mRootView->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->AddOnComputeInternalInsetsListener(mInsetsComputer.Get());
    AutoPtr<IView> v;
    mRootView->FindViewById(R::id::content, (IView**)&v);
    mContentFrame = IFrameLayout::Probe(v);
    return NOERROR;
}

ECode VoiceInteractionSession::ShowWindow()
{
    if (DEBUG) {
        Logger::V(TAG, "Showing window: mWindowAdded=%s mWindowVisible=",
            mWindowAdded ? "TRUE" : "FALSE",
            mWindowVisible ? "TRUE" : "FALSE");
    }
    if (mInShowWindow) {
        Logger::W(TAG, "Re-entrance in to showWindow");
        return E_NULL_POINTER_EXCEPTION;
    }

    mInShowWindow = TRUE;
    if (!mWindowVisible) {
        mWindowVisible = TRUE;
        if (!mWindowAdded) {
            mWindowAdded = TRUE;
            AutoPtr<IView> v;
            OnCreateContentView((IView**)&v);
            if (v != NULL) {
                SetContentView(v);
            }
        }
        IDialog::Probe(mWindow)->Show();
    }
    mWindowWasVisible = TRUE;
    mInShowWindow = FALSE;
    return NOERROR;
}

ECode VoiceInteractionSession::HideWindow() {
    if (mWindowVisible) {
        IDialog::Probe(mWindow)->Hide();
        mWindowVisible = FALSE;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::SetTheme(
    /* [in] */ Int32 theme) {
    if (mWindow != NULL) {
        Logger::E(TAG, "Must be called before onCreate()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mTheme = theme;
    return NOERROR;
}

ECode VoiceInteractionSession::StartVoiceActivity(
    /* [in] */ IIntent* intent) {
    if (mToken == NULL) {
        Logger::E(TAG, "Can't call before onCreate()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // try {
    Boolean result;
    intent->MigrateExtraStreamToClipData(&result);
    intent->PrepareToLeaveProcess();
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String type;
    intent->ResolveType(resolver, &type);
    Int32 res;
    mSystemService->StartVoiceActivity(mToken, intent, type, &res);
    Instrumentation::CheckStartActivityResult(res, intent);
    // } catch (RemoteException e) {
    // } //TODO
    return NOERROR;
}

ECode VoiceInteractionSession::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater)
    *inflater = mInflater;
    REFCOUNT_ADD(*inflater)
    return NOERROR;
}

ECode VoiceInteractionSession::GetWindow(
    /* [out] */ IDialog** window)
{
    VALIDATE_NOT_NULL(window)
    *window = IDialog::Probe(mWindow);
    REFCOUNT_ADD(*window)
    return NOERROR;
}

ECode VoiceInteractionSession::Finish()
{
    if (mToken == NULL) {
        Logger::E(TAG, "Can't call before onCreate()");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    HideWindow();
    ECode ec = mSystemService->Finish(mToken);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode VoiceInteractionSession::OnCreate(
    /* [in] */ IBundle* args)
{
    mTheme = mTheme != 0 ? mTheme : R::style::Theme_DeviceDefault_VoiceInteractionSession;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    CSoftInputWindow::New(mContext, String("VoiceInteractionSession"),
        mTheme, mCallbacks, this, mDispatcherState,
        IWindowManagerLayoutParams::TYPE_VOICE_INTERACTION,
        IGravity::TOP, TRUE, (ISoftInputWindow**)&mWindow);
    AutoPtr<IWindow> win;
    IDialog::Probe(mWindow)->GetWindow((IWindow**)&win);
    win->AddFlags(IWindowManagerLayoutParams::FLAG_HARDWARE_ACCELERATED);
    InitViews();
    win->SetLayout(IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::WRAP_CONTENT);
    mWindow->SetToken(mToken);
    return NOERROR;
}

ECode VoiceInteractionSession::OnDestroy()
{
    return NOERROR;
}

ECode VoiceInteractionSession::OnCreateContentView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    return NOERROR;
}

ECode VoiceInteractionSession::SetContentView(
    /* [in] */ IView* view)
{
    IViewGroup::Probe(mContentFrame)->RemoveAllViews();
    AutoPtr<IFrameLayoutLayoutParams> params;
    CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::WRAP_CONTENT, (IFrameLayoutLayoutParams**)&params);
    IViewGroup::Probe(mContentFrame)->AddView(view, IViewGroupLayoutParams::Probe(params));
    return NOERROR;
}

ECode VoiceInteractionSession::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode VoiceInteractionSession::OnKeyLongPress(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode VoiceInteractionSession::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode VoiceInteractionSession::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 count,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode VoiceInteractionSession::OnBackPressed()
{
    return Finish();
}

ECode VoiceInteractionSession::OnCloseSystemDialogs()
{
    return Finish();
}

ECode VoiceInteractionSession::OnComputeInsets(
    /* [in] */ IVoiceInteractionSessionInsets* outInsets)
{
    AutoPtr<ArrayOf<Int32> > loc = mTmpLocation;
    AutoPtr<IDialog> dialog;
    GetWindow((IDialog**)&dialog);
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    AutoPtr<IView> decor;
    window->GetDecorView((IView**)&decor);
    decor->GetLocationInWindow(loc.Get());
    AutoPtr<Insets> _outInsets = (Insets*)outInsets;
    _outInsets->mContentInsets->SetTop(0);
    _outInsets->mContentInsets->SetLeft(0);
    _outInsets->mContentInsets->SetRight(0);
    _outInsets->mContentInsets->SetBottom(0);
    _outInsets->mTouchableInsets = IVoiceInteractionSessionInsets::TOUCHABLE_INSETS_FRAME;
    _outInsets->mTouchableRegion->SetEmpty();
    return NOERROR;
}

ECode VoiceInteractionSession::OnTaskStarted(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    return NOERROR;
}

ECode VoiceInteractionSession::OnTaskFinished(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    return Finish();
}

ECode VoiceInteractionSession::OnGetSupportedCommands(
    /* [in] */ IVoiceInteractionSessionCaller* caller,
    /* [in] */ ArrayOf<String>* commands,
    /* [out, callee] */ ArrayOf<Boolean>** results)
{
    VALIDATE_NOT_NULL(results)
    *results = ArrayOf<Boolean>::Alloc(commands->GetLength());
    REFCOUNT_ADD(*results)
    return NOERROR;
}

ECode VoiceInteractionSession::OnCompleteVoice(
    /* [in] */ IVoiceInteractionSessionCaller* caller,
    /* [in] */ IVoiceInteractionSessionRequest* request,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras)
{
    return request->SendCompleteVoiceResult(NULL);
}

ECode VoiceInteractionSession::OnAbortVoice(
    /* [in] */ IVoiceInteractionSessionCaller* caller,
    /* [in] */ IVoiceInteractionSessionRequest* request,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras)
{
    return request->SendAbortVoiceResult(NULL);
}

}//namespace Voice
}//namespace Service
}//namespace Droid
}//namespace Elastos