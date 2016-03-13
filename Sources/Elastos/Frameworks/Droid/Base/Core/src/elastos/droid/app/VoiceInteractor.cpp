
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/VoiceInteractor.h"
#include "elastos/droid/app/CVoiceInteractorCallback.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::App::EIID_IIVoiceInteractorCallback;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String VoiceInteractor::TAG("VoiceInteractor");
const Boolean VoiceInteractor::DEBUG = TRUE;

const Int32 VoiceInteractor::MSG_CONFIRMATION_RESULT = 1;
const Int32 VoiceInteractor::MSG_COMPLETE_VOICE_RESULT = 2;
const Int32 VoiceInteractor::MSG_ABORT_VOICE_RESULT = 3;
const Int32 VoiceInteractor::MSG_COMMAND_RESULT = 4;
const Int32 VoiceInteractor::MSG_CANCEL_RESULT = 5;

//=====================================================================================
// VoiceInteractor::HandlerCallerCallback
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor::HandlerCallerCallback, Object, IHandlerCallerCallback)

VoiceInteractor::HandlerCallerCallback::HandlerCallerCallback(
    /* [in] */ VoiceInteractor* host)
    : mHost(host)
{
}

ECode VoiceInteractor::HandlerCallerCallback::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    AutoPtr<IVoiceInteractorRequest> request;
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    switch (what) {
        case MSG_CONFIRMATION_RESULT:
            request = mHost->PullRequest(IIVoiceInteractorRequest::Probe(args->mArg1), TRUE);
            if (DEBUG) Logger::D("VoiceInteractor", "onConfirmResult: req=%s / %s confirmed %d result=%s",
                TO_CSTR(args->mArg1), TO_CSTR(request),
                arg1, TO_CSTR(args->mArg2));
               if (request != NULL) {
                IVoiceInteractorConfirmationRequest::Probe(request)->OnConfirmationResult(arg1 != 0, IBundle::Probe(args->mArg2));
                request->Clear();
            }
            break;

        case MSG_COMPLETE_VOICE_RESULT:
            request = mHost->PullRequest(IIVoiceInteractorRequest::Probe(args->mArg1), TRUE);
            if (DEBUG) Logger::D("VoiceInteractor", "onCompleteVoice: req=%s / %s result=%s",
                TO_CSTR(args->mArg1), TO_CSTR(request),
                TO_CSTR(args->mArg2));
            if (request != NULL) {
                IVoiceInteractorCompleteVoiceRequest::Probe(request)->OnCompleteResult(IBundle::Probe(args->mArg2));
                request->Clear();
            }
            break;

        case MSG_ABORT_VOICE_RESULT:
            request = mHost->PullRequest(IIVoiceInteractorRequest::Probe(args->mArg1), TRUE);
            if (DEBUG) Logger::D("VoiceInteractor", "onAbortVoice: req=%s / %s result=%s",
                TO_CSTR(args->mArg1), TO_CSTR(request),
                TO_CSTR(args->mArg2));
            if (request != NULL) {
                IVoiceInteractorAbortVoiceRequest::Probe(request)->OnAbortResult(IBundle::Probe(args->mArg2));
                request->Clear();
            }
            break;

        case MSG_COMMAND_RESULT:
            request = mHost->PullRequest(IIVoiceInteractorRequest::Probe(args->mArg1), arg1 != 0);
            if (DEBUG) Logger::D("VoiceInteractor", "onCommandResult: req=%s / %s result=%s",
                TO_CSTR(args->mArg1), TO_CSTR(request),
                TO_CSTR(args->mArg2));
            if (request != NULL) {
                IVoiceInteractorCommandRequest::Probe(request)->OnCommandResult(IBundle::Probe(args->mArg2));
                if (arg1 != 0) {
                    request->Clear();
                }
            }
            break;

        case MSG_CANCEL_RESULT:
            request = mHost->PullRequest(IIVoiceInteractorRequest::Probe(args->mArg1), TRUE);
            if (DEBUG) Logger::D("VoiceInteractor", "onCancelResult: req=%s / %s",
                TO_CSTR(args->mArg1), TO_CSTR(request));
            if (request != NULL) {
                request->OnCancel();
                request->Clear();
            }
            break;
    }
    return NOERROR;
}

//=====================================================================================
// VoiceInteractor::VoiceInteractorCallback
//=====================================================================================
CAR_INTERFACE_IMPL_2(VoiceInteractor::VoiceInteractorCallback, Object, IIVoiceInteractorCallback, IBinder);

VoiceInteractor::VoiceInteractorCallback::VoiceInteractorCallback()
{}

VoiceInteractor::VoiceInteractorCallback::~VoiceInteractorCallback()
{}

CARAPI VoiceInteractor::VoiceInteractorCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

ECode VoiceInteractor::VoiceInteractorCallback::constructor(
    /* [in] */ IObject* handlerCaller)
{
    mHandlerCaller = IHandlerCaller::Probe(handlerCaller);
    return NOERROR;
}

ECode VoiceInteractor::VoiceInteractorCallback::DeliverConfirmationResult(
    /* [in] */ IIVoiceInteractorRequest* request,
    /* [in] */ Boolean confirmed,
    /* [in] */ IBundle* result)
{
    AutoPtr<IMessage> msg;
    mHandlerCaller->ObtainMessageIOO(
        VoiceInteractor::MSG_CONFIRMATION_RESULT,
        confirmed ? 1 : 0, request, result, (IMessage**)&msg);
    return mHandlerCaller->SendMessage(msg);
}

ECode VoiceInteractor::VoiceInteractorCallback::DeliverCompleteVoiceResult(
    /* [in] */ IIVoiceInteractorRequest* request,
    /* [in] */ IBundle* result)
{
    AutoPtr<IMessage> msg;
    mHandlerCaller->ObtainMessageOO(
        VoiceInteractor::MSG_COMPLETE_VOICE_RESULT, request, result, (IMessage**)&msg);
    return mHandlerCaller->SendMessage(msg);
}

ECode VoiceInteractor::VoiceInteractorCallback::DeliverAbortVoiceResult(
    /* [in] */ IIVoiceInteractorRequest* request,
    /* [in] */ IBundle* result)
{
    AutoPtr<IMessage> msg;
    mHandlerCaller->ObtainMessageOO(
        VoiceInteractor::MSG_ABORT_VOICE_RESULT, request, result, (IMessage**)&msg);
    return mHandlerCaller->SendMessage(msg);
}

ECode VoiceInteractor::VoiceInteractorCallback::DeliverCommandResult(
    /* [in] */ IIVoiceInteractorRequest* request,
    /* [in] */ Boolean complete,
    /* [in] */ IBundle* result)
{
    AutoPtr<IMessage> msg;
    mHandlerCaller->ObtainMessageIOO(
        VoiceInteractor::MSG_COMMAND_RESULT, complete ? 1 : 0, request, result, (IMessage**)&msg);
    return mHandlerCaller->SendMessage(msg);
}

ECode VoiceInteractor::VoiceInteractorCallback::DeliverCancel(
    /* [in] */ IIVoiceInteractorRequest* request)
{
    AutoPtr<IMessage> msg;
    mHandlerCaller->ObtainMessageO(
        VoiceInteractor::MSG_CANCEL_RESULT, request, (IMessage**)&msg);
    return mHandlerCaller->SendMessage(msg);
}

//=====================================================================================
// VoiceInteractor::Request
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor::Request, Object, IVoiceInteractorRequest);

VoiceInteractor::Request::Request()
{}

VoiceInteractor::Request::~Request()
{}

ECode VoiceInteractor::Request::Cancel()
{
    ECode ec = mRequestInterface->Cancel();
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W("VoiceInteractor", "Voice interactor has died");
        ec = NOERROR;
    }
    return ec;
}

ECode VoiceInteractor::Request::GetContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    *ctx = mContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode VoiceInteractor::Request::GetActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = mActivity;
    REFCOUNT_ADD(*activity);
    return NOERROR;
}

ECode VoiceInteractor::Request::OnCancel()
{
    return NOERROR;
}

ECode VoiceInteractor::Request::OnAttached(
    /* [in] */ IActivity* activity)
{
    return NOERROR;
}

ECode VoiceInteractor::Request::OnDetached()
{
    return NOERROR;
}

ECode VoiceInteractor::Request::Clear()
{
    mRequestInterface = NULL;
    mContext = NULL;
    mActivity = NULL;
    return NOERROR;
}

//=====================================================================================
// VoiceInteractor::ConfirmationRequest
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor::ConfirmationRequest, VoiceInteractor::Request, IVoiceInteractorConfirmationRequest);

ECode VoiceInteractor::ConfirmationRequest::constructor(
    /* [in] */ ICharSequence* prompt,
    /* [in] */ IBundle* extras)
{
    mPrompt = prompt;
    mExtras = extras;
    return NOERROR;
}

ECode VoiceInteractor::ConfirmationRequest::OnConfirmationResult(
    /* [in] */ Boolean confirmed,
    /* [in] */ IBundle* result)
{
    return NOERROR;
}

ECode VoiceInteractor::ConfirmationRequest::Submit(
    /* [in] */ IIVoiceInteractor* interactor,
    /* [in] */ const String& packageName,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [out] */ IIVoiceInteractorRequest** request)
{
    return interactor->StartConfirmation(packageName, callback, mPrompt, mExtras, request);
}


//=====================================================================================
// VoiceInteractor::CompleteVoiceRequest
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor::CompleteVoiceRequest, VoiceInteractor::Request, IVoiceInteractorCompleteVoiceRequest);

ECode VoiceInteractor::CompleteVoiceRequest::constructor(
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras)
{
    mMessage = message;
    mExtras = extras;
    return NOERROR;
}

ECode VoiceInteractor::CompleteVoiceRequest::OnCompleteResult(
    /* [in] */ IBundle* result)
{
    return NOERROR;
}

ECode VoiceInteractor::CompleteVoiceRequest::Submit(
    /* [in] */ IIVoiceInteractor* interactor,
    /* [in] */ const String& packageName,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [out] */ IIVoiceInteractorRequest** request)
{
    return interactor->StartCompleteVoice(packageName, callback, mMessage, mExtras, request);
}

//=====================================================================================
// VoiceInteractor::AbortVoiceRequest
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor::AbortVoiceRequest, VoiceInteractor::Request, IVoiceInteractorAbortVoiceRequest);

ECode VoiceInteractor::AbortVoiceRequest::constructor(
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras)
{
    mMessage = message;
    mExtras = extras;
    return NOERROR;
}

ECode VoiceInteractor::AbortVoiceRequest::OnAbortResult(
    /* [in] */ IBundle* result)
{
    return NOERROR;
}

ECode VoiceInteractor::AbortVoiceRequest::Submit(
    /* [in] */ IIVoiceInteractor* interactor,
    /* [in] */ const String& packageName,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [out] */ IIVoiceInteractorRequest** request)
{
    return interactor->StartAbortVoice(packageName, callback, mMessage, mExtras, request);
}

//=====================================================================================
// VoiceInteractor::CommandRequest
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor::CommandRequest, VoiceInteractor::Request, IVoiceInteractorCommandRequest);

ECode VoiceInteractor::CommandRequest::constructor(
    /* [in] */ const String& command,
    /* [in] */ IBundle* args)
{
    mCommand = command;
    mArgs = args;
    return NOERROR;
}

ECode VoiceInteractor::CommandRequest::OnCommandResult(
    /* [in] */ IBundle* result)
{
    return NOERROR;
}

ECode VoiceInteractor::CommandRequest::Submit(
    /* [in] */ IIVoiceInteractor* interactor,
    /* [in] */ const String& packageName,
    /* [in] */ IIVoiceInteractorCallback* callback,
    /* [out] */ IIVoiceInteractorRequest** request)
{
    return interactor->StartCommand(packageName, callback, mCommand, mArgs, request);
}

//=====================================================================================
// VoiceInteractor
//=====================================================================================
CAR_INTERFACE_IMPL(VoiceInteractor, Object, IVoiceInteractor)

VoiceInteractor::VoiceInteractor()
{}

VoiceInteractor::~VoiceInteractor()
{}

ECode VoiceInteractor::constructor(
    /* [in] */ IIVoiceInteractor* interactor,
    /* [in] */ IContext* context,
    /* [in] */ IActivity* activity,
    /* [in] */ ILooper* looper)
{
    mInteractor = interactor;
    mContext = context;
    mActivity = activity;

    mHandlerCallerCallback = new HandlerCallerCallback(this);
    CHandlerCaller::New(context, looper, mHandlerCallerCallback, TRUE, TRUE, (IHandlerCaller**)&mHandlerCaller);
    CVoiceInteractorCallback::New(IObject::Probe(mHandlerCaller), (IIVoiceInteractorCallback**)&mCallback);
    return NOERROR;
}

AutoPtr<IVoiceInteractorRequest> VoiceInteractor::PullRequest(
    /* [in] */ IIVoiceInteractorRequest* request,
    /* [in] */ Boolean complete)
{
    AutoPtr<IVoiceInteractorRequest> req;
    synchronized(mActiveRequestsLock) {
        HashMap<AutoPtr<IBinder>, AutoPtr<IVoiceInteractorRequest> >::Iterator it;
        AutoPtr<IBinder> binder = IBinder::Probe(request);
        it = mActiveRequests.Find(binder);
        if (it != mActiveRequests.End()) {
            req = it->mSecond;
            if (req != NULL && complete) {
                mActiveRequests.Erase(it);
            }

        }
    }
    return req;
}

AutoPtr<IArrayList> VoiceInteractor::MakeRequestList()
{
    Int32 N = mActiveRequests.GetSize();
    if (N < 1) {
        return NULL;
    }

    AutoPtr<IArrayList> list;
    CArrayList::New(N, (IArrayList**)&list);

    HashMap<AutoPtr<IBinder>, AutoPtr<IVoiceInteractorRequest> >::Iterator it;
    for (it = mActiveRequests.Begin(); it != mActiveRequests.End(); ++it) {
        list->Add(it->mSecond.Get());
    }

    return list;
}

ECode VoiceInteractor::AttachActivity(
    /* [in] */ IActivity* activity)
{
    if (mActivity.Get() == activity) {
        return NOERROR;
    }

    mContext = IContext::Probe(activity);
    mActivity = activity;
    AutoPtr<IArrayList> reqs = MakeRequestList();
    if (reqs != NULL) {
        Int32 size;
        reqs->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            reqs->Get(i, (IInterface**)&obj);
            IVoiceInteractorRequest* req = IVoiceInteractorRequest::Probe(reqs);
            ((Request*)req)->mContext = mContext;
            ((Request*)req)->mActivity = activity;
            req->OnAttached(activity);
        }
    }
    return NOERROR;
}

ECode VoiceInteractor::DetachActivity()
{
    AutoPtr<IArrayList> reqs = MakeRequestList();
    if (reqs != NULL) {
        Int32 size;
        reqs->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            reqs->Get(i, (IInterface**)&obj);
            IVoiceInteractorRequest* req = IVoiceInteractorRequest::Probe(obj);
            req->OnDetached();
            ((Request*)req)->mActivity = NULL;
            ((Request*)req)->mContext = NULL;
        }
    }
    mContext = NULL;
    mActivity = NULL;
    return NOERROR;
}

ECode VoiceInteractor::SubmitRequest(
    /* [in] */ IVoiceInteractorRequest* vir,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Request* request = (Request*)vir;
    AutoPtr<IIVoiceInteractorRequest> ireq;
    String packageName;
    mContext->GetOpPackageName(&packageName);
    ECode ec = request->Submit(mInteractor, packageName, mCallback, (IIVoiceInteractorRequest**)&ireq);
    FAIL_GOTO(ec, _EXIT_)

    request->mRequestInterface = ireq;
    request->mContext = mContext;
    request->mActivity = mActivity;

    synchronized(mActiveRequestsLock) {
        AutoPtr<IBinder> binder = IBinder::Probe(ireq);
        mActiveRequests[binder] = vir;
    }

_EXIT_:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Remove voice interactor service died");
    }
    else if (!FAILED(ec)) {
        *result = TRUE;
    }
    return ec;
}

ECode VoiceInteractor::SupportsCommands(
    /* [in] */ ArrayOf<String>* commands,
    /* [out, callee] */ ArrayOf<Boolean>** results)
{
    VALIDATE_NOT_NULL(results)
    *results = NULL;

    String packageName;
    mContext->GetOpPackageName(&packageName);
    ECode ec = mInteractor->SupportsCommands(packageName, commands, results);
    if (DEBUG) Logger::D(TAG, "supportsCommands: cmds=%s res=%s",
        Arrays::ToString(commands).string(), Arrays::ToString(*results).string());
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        *results = NULL;
        ec = E_RUNTIME_EXCEPTION;
        Logger::E(TAG, "Voice interactor has died");
    }

    return ec;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
