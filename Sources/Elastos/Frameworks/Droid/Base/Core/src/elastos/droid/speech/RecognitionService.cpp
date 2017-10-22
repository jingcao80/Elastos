//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/speech/RecognitionService.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::EIID_IHandler;

namespace Elastos {
namespace Droid {
namespace Speech {

const String RecognitionService::TAG("RecognitionService");
const Boolean RecognitionService::DBG = FALSE;
const Int32 RecognitionService::MSG_START_LISTENING = 1;
const Int32 RecognitionService::MSG_STOP_LISTENING = 2;
const Int32 RecognitionService::MSG_CANCEL = 3;
const Int32 RecognitionService::MSG_RESET = 4;

/******************************RecognitionService::RecognitionServiceCallback*************************/
CAR_INTERFACE_IMPL(RecognitionService::RecognitionServiceCallback::RecognitionServiceCallback, Object, IRecognitionServiceCallback)

RecognitionService::RecognitionServiceCallback::RecognitionServiceCallback()
{}

RecognitionService::RecognitionServiceCallback::~RecognitionServiceCallback()
{}

ECode RecognitionService::RecognitionServiceCallback::constructor()
{
    return NOERROR;
}

RecognitionService::RecognitionServiceCallback::RecognitionServiceCallback(
    /* [in] */ IIRecognitionListener* listener,
    /* [in] */ RecognitionService* rs)
{
    mListener = listener;
    mHost = rs;
}

ECode RecognitionService::RecognitionServiceCallback::BeginningOfSpeech()
{
    if (DBG) {
        Logger::D(TAG, String("beginningOfSpeech\n"));
    }
    mListener->OnBeginningOfSpeech();
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::BufferReceived(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    mListener->OnBufferReceived(buffer);
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::EndOfSpeech()
{
    mListener->OnEndOfSpeech();
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::Error(
    /* [in] */ Int32 error)
{
    Boolean result;
    mHost->mHandler->SendEmptyMessage(RecognitionService::MSG_RESET, &result);
    mListener->OnError(error);
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::PartialResults(
    /* [in] */ IBundle* partialResults)
{
    mListener->OnPartialResults(partialResults);
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::ReadyForSpeech(
    /* [in] */ IBundle* params)
{
    mListener->OnReadyForSpeech(params);
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::Results(
    /* [in] */ IBundle* results)
{
    Boolean result;
    mHost->mHandler->SendEmptyMessage(RecognitionService::MSG_RESET, &result);
    mListener->OnResults(results);
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceCallback::RmsChanged(
    /* [in] */ Float rmsdB)
{
    mListener->OnRmsChanged(rmsdB);
    return NOERROR;
}

/******************************RecognitionService::RecognitionServiceStartListeningArgs*************************/

RecognitionService::RecognitionServiceStartListeningArgs::RecognitionServiceStartListeningArgs(
    /* [in] */ IIntent* intent,
    /* [in] */ IIRecognitionListener* listener)
{
    mIntent = intent;
    mListener = listener;
}

/******************************RecognitionService::RecognitionServiceHandler*************************/
//CAR_INTERFACE_IMPL(RecognitionService::RecognitionServiceHandler, Object, IHandler);

RecognitionService::RecognitionServiceHandler::RecognitionServiceHandler()
{}

RecognitionService::RecognitionServiceHandler::RecognitionServiceHandler(
    /* [in] */ RecognitionService* host)
    : mHost(host)
{}

RecognitionService::RecognitionServiceHandler::~RecognitionServiceHandler()
{}

ECode RecognitionService::RecognitionServiceHandler::constructor()
{
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case RecognitionService::MSG_START_LISTENING: {
            RecognitionServiceStartListeningArgs* args = (RecognitionServiceStartListeningArgs*)IObject::Probe(obj.Get());
            mHost->DispatchStartListening(args->mIntent.Get(), args->mListener.Get());
            break;
        }
        case RecognitionService::MSG_STOP_LISTENING:
            mHost->DispatchStopListening(IIRecognitionListener::Probe(obj));
            break;
        case RecognitionService::MSG_CANCEL:
            mHost->DispatchCancel(IIRecognitionListener::Probe(obj));
            break;
        case RecognitionService::MSG_RESET:
            mHost->DispatchClearCallback();
            break;
    }
    return NOERROR;
}

/******************************RecognitionService::RecognitionServiceBinder*************************/
CAR_INTERFACE_IMPL(RecognitionService::RecognitionServiceBinder, Object, IIRecognitionService)

RecognitionService::RecognitionServiceBinder::RecognitionServiceBinder()
{}

RecognitionService::RecognitionServiceBinder::~RecognitionServiceBinder()
{}

ECode RecognitionService::RecognitionServiceBinder::constructor()
{
    return NOERROR;
}

RecognitionService::RecognitionServiceBinder::RecognitionServiceBinder(
    /* [in] */ RecognitionService* service)
{
    mInternalService = service;
}

ECode RecognitionService::RecognitionServiceBinder::StartListening(
    /* [in] */ IIntent* recognizerIntent,
    /* [in] */ IIRecognitionListener* listener)
{
    if (DBG) {
        //Java:    Log.d(TAG, "startListening called by:" + listener.asBinder());
        Logger::D(TAG, "startListening called by:\n");
    }
    if (mInternalService != NULL && mInternalService->CheckPermissions(listener)) {
        AutoPtr<RecognitionServiceStartListeningArgs> rslArgs =
            new RecognitionServiceStartListeningArgs(recognizerIntent, listener);
        AutoPtr<IMessage> msg;
        mInternalService->mHandler->ObtainMessage(
            RecognitionService::MSG_START_LISTENING, TO_IINTERFACE(rslArgs.Get()), (IMessage**)&msg);
        Boolean result;
        return mInternalService->mHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceBinder::StopListening(
    /* [in] */ IIRecognitionListener* listener)
{
    if (DBG) {
        //Java:    Log.d(TAG, "stopListening called by:" + listener.asBinder());
        Logger::D(TAG, "stopListening called by:\n");

    }
    if (mInternalService != NULL && mInternalService->CheckPermissions(listener)) {
        AutoPtr<IMessage> msg;
        mInternalService->mHandler->ObtainMessage(
            RecognitionService::MSG_STOP_LISTENING, listener, (IMessage**)&msg);
        Boolean result;
        return mInternalService->mHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceBinder::Cancel(
    /* [in] */ IIRecognitionListener* listener)
{
    if (DBG){
        //Java:    Log.d(TAG, "cancel called by:" + listener.asBinder());
        Logger::D(TAG, "cancel called by:\n");
    }
    if (mInternalService != NULL && mInternalService->CheckPermissions(listener)) {
        AutoPtr<IMessage> msg;
        mInternalService->mHandler->ObtainMessage(
            RecognitionService::MSG_CANCEL, listener, (IMessage**)&msg);
        Boolean result;
        return mInternalService->mHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode RecognitionService::RecognitionServiceBinder::ClearReference()
{
    mInternalService = NULL;
    return NOERROR;
}

/******************************RecognitionService*************************/
void RecognitionService::DispatchStartListening(
    /* [in] */ IIntent* intent,
    /* [in] */ IIRecognitionListener* listener)
{
    if (mCurrentCallback == NULL) {
        if (DBG){
            //Java:    Log.d(TAG, "created new mCurrentCallback, listener = " + listener.asBinder());
            Logger::D(TAG, "created new mCurrentCallback, listener = \n");
        }
        mCurrentCallback = new RecognitionServiceCallback(listener, this);
        OnStartListening(intent, mCurrentCallback);
    } else {
        //try {
            listener->OnError(ISpeechRecognizer::ERROR_RECOGNIZER_BUSY);
        //} catch (RemoteException e) {
            //Java:    Log.d(TAG, "onError call from startListening failed");
            Logger::D(TAG, "onError call from startListening failed\n");
        //}
        //Java:    Log.i(TAG, "concurrent startListening received - ignoring this call");
        Logger::I(TAG, "concurrent startListening received - ignoring this call\n");
    }
}

void RecognitionService::DispatchStopListening(
    /* [in] */ IIRecognitionListener* listener)
{
    //try {
        if (mCurrentCallback == NULL) {
            listener->OnError(ISpeechRecognizer::ERROR_CLIENT);
            //Java:    Log.w(TAG, "stopListening called with no preceding startListening - ignoring");
            Logger::W(TAG, "stopListening called with no preceding startListening - ignoring\n");
        } else if ( (mCurrentCallback->mListener).Get() != listener) {
            listener->OnError(ISpeechRecognizer::ERROR_RECOGNIZER_BUSY);
            //Java:    Log.w(TAG, "stopListening called by other caller than startListening - ignoring");
            Logger::W(TAG, "stopListening called by other caller than startListening - ignoring\n");
        } else { // the correct state
            OnStopListening(mCurrentCallback);
        }
    //} catch (RemoteException e) { // occurs if onError fails
        //Java:    Log.d(TAG, "onError call from stopListening failed");
        Logger::D(TAG, "onError call from stopListening failed\n");
    //}
}

void RecognitionService::DispatchCancel(
    /* [in] */ IIRecognitionListener* listener)
{
    if (mCurrentCallback == NULL) {
        if (DBG){
            //Java:    Log.d(TAG, "cancel called with no preceding startListening - ignoring");
            Logger::D(TAG, "cancel called with no preceding startListening - ignoring\n");
        }
    } else if ( (mCurrentCallback->mListener).Get() != listener) {
        //Java:    Log.w(TAG, "cancel called by client who did not call startListening - ignoring");
        Logger::W(TAG, "cancel called by client who did not call startListening - ignoring\n");
    } else { // the correct state
        OnCancel(mCurrentCallback);
        mCurrentCallback = NULL;
        if (DBG){
            //Java:    Log.d(TAG, "canceling - setting mCurrentCallback to null");
            Logger::D(TAG, "canceling - setting mCurrentCallback to null\n");
        }
    }
}

void RecognitionService::DispatchClearCallback()
{
    mCurrentCallback = NULL;
}

Boolean RecognitionService::CheckPermissions(
    /* [in] */ IIRecognitionListener* listener)
{
    if (DBG) {
        //Java:    Log.d(TAG, "checkPermissions");
        Logger::D(TAG, "checkPermissions\n");
    }
    if (/*RecognitionService.this.checkCallingOrSelfPermission(Elastos::Droid::Manifest::permission::RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED*/FALSE) {
        return TRUE;
    }
    //try {
        //Java:    Log.e(TAG, "call for recognition service without RECORD_AUDIO permissions");
        Logger::E(TAG, "call for recognition service without RECORD_AUDIO permissions\n");
        listener->OnError(ISpeechRecognizer::ERROR_INSUFFICIENT_PERMISSIONS);
    //} catch (RemoteException re) {
        //Java:    Log.e(TAG, "sending ERROR_INSUFFICIENT_PERMISSIONS message failed", re);
        Logger::E(TAG, "sending ERROR_INSUFFICIENT_PERMISSIONS message failed\n");
    //}
    return FALSE;
}

ECode RecognitionService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** service)
{
    if (DBG) {
        //Java:    Log.d(TAG, "onBind, intent=" + intent);
        String strIntent;
        intent->ToString(&strIntent);
        Logger::D(TAG, "onBind, intent=%s\n", strIntent.string());
    }
    *service = NULL;
    return NOERROR;
}

ECode RecognitionService::OnDestroy()
{
    if (DBG) {
        //Java:    Log.d(TAG, "onDestroy");
        Logger::D(TAG, "onDestroy\n");
    }
    mCurrentCallback = NULL;
    mBinder->ClearReference();
    Service::OnDestroy();
    return NOERROR;
}

RecognitionService::RecognitionService()
{
    mBinder = new RecognitionServiceBinder();
//    mHandler = new RecognitionServiceHandler();
}

} // namespace Speech
} // namepsace Droid
} // namespace Elastos
