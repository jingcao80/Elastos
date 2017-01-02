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

#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/service/voice/CAlwaysOnHotwordDetectorEventPayload.h"
#include "elastos/droid/service/voice/CAlwaysOnHotwordDetectorSoundTriggerListener.h"
#include "elastos/droid/service/voice/AlwaysOnHotwordDetector.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphraseRecognitionExtra.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerRecognitionConfig.h"
#include "elastos/droid/hardware/soundtrigger/KeyphraseMetadata.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerRecognitionEvent.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerKeyphraseRecognitionExtra;
using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerRecognitionConfig;
using Elastos::Droid::Hardware::Soundtrigger::EIID_IIRecognitionStatusCallback;
using Elastos::Droid::Hardware::Soundtrigger::ISoundTriggerConfidenceLevel;
using Elastos::Droid::Hardware::Soundtrigger::ISoundTriggerModuleProperties;
using Elastos::Droid::Hardware::Soundtrigger::ISoundTriggerRecognitionConfig;
using Elastos::Droid::Hardware::Soundtrigger::KeyphraseMetadata;
using Elastos::Droid::Hardware::Soundtrigger::SoundTriggerRecognitionEvent;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Service::Voice::CAlwaysOnHotwordDetectorEventPayload;
using Elastos::Droid::Service::Voice::CAlwaysOnHotwordDetectorSoundTriggerListener;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

//================================
// AlwaysOnHotwordDetector::EventPayload
//================================

CAR_INTERFACE_IMPL(AlwaysOnHotwordDetector::EventPayload, Object, IAlwaysOnHotwordDetectorEventPayload)

AlwaysOnHotwordDetector::EventPayload::EventPayload()
    : mTriggerAvailable(FALSE)
    , mCaptureAvailable(FALSE)
    , mCaptureSession(0)
{
}

ECode AlwaysOnHotwordDetector::EventPayload::constructor(
    /* [in] */ Boolean triggerAvailable,
    /* [in] */ Boolean captureAvailable,
    /* [in] */ IAudioFormat* audioFormat,
    /* [in] */ Int32 captureSession,
    /* [in] */ ArrayOf<Byte>* data)
{
    mTriggerAvailable = triggerAvailable;
    mCaptureAvailable = captureAvailable;
    mCaptureSession = captureSession;
    mAudioFormat = audioFormat;
    mData = data;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::EventPayload::GetCaptureAudioFormat(
    /* [out] */ IAudioFormat** af)
{
    VALIDATE_NOT_NULL(af)
    *af = mAudioFormat.Get();
    REFCOUNT_ADD(*af)
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::EventPayload::GetTriggerAudio(
    /* [out, callee] */ ArrayOf<Byte>** ta)
{
    VALIDATE_NOT_NULL(ta)
    if (mTriggerAvailable) {
        *ta = mData.Get();
        REFCOUNT_ADD(*ta)
    }
    else {
        *ta = NULL;
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::EventPayload::GetCaptureSession(
    /* [out] */ IInteger32** cs)
{
    VALIDATE_NOT_NULL(cs)
    if (mCaptureAvailable) {
        CInteger32::New(mCaptureSession, cs);
    }
    else {
        *cs = NULL;
    }
    return NOERROR;
}

//================================
// AlwaysOnHotwordDetector::Callback
//================================

CAR_INTERFACE_IMPL(AlwaysOnHotwordDetector::Callback, Object, IAlwaysOnHotwordDetectorCallback)

AlwaysOnHotwordDetector::Callback::Callback()
{
}

//================================
// AlwaysOnHotwordDetector::SoundTriggerListener
//================================

CAR_INTERFACE_IMPL_2(AlwaysOnHotwordDetector::SoundTriggerListener, Object, IIRecognitionStatusCallback, IBinder)

AlwaysOnHotwordDetector::SoundTriggerListener::SoundTriggerListener()
{
}

ECode AlwaysOnHotwordDetector::SoundTriggerListener::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::SoundTriggerListener::OnDetected(
    /* [in] */ ISoundTriggerKeyphraseRecognitionEvent* event)
{
    if (AlwaysOnHotwordDetector::DBG) {
        String str;
        IObject::Probe(event)->ToString(&str);
        Slogger::D(AlwaysOnHotwordDetector::TAG, "onDetected(%s)", str.string());
    } else {
        Slogger::I(TAG, "onDetected");
    }
    AutoPtr<SoundTriggerRecognitionEvent> strevent = (SoundTriggerRecognitionEvent*)event;
    AutoPtr<IAlwaysOnHotwordDetectorEventPayload> eventPayload;
    CAlwaysOnHotwordDetectorEventPayload::New(strevent->mTriggerInData,
        strevent->mCaptureAvailable, strevent->mCaptureFormat, strevent->mCaptureSession,
        strevent->mData, (IAlwaysOnHotwordDetectorEventPayload**)&eventPayload);
    AutoPtr<IMessage> msg = CMessage::Obtain(mHandler, AlwaysOnHotwordDetector::MSG_HOTWORD_DETECTED, eventPayload.Get());
    msg->SendToTarget();
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::SoundTriggerListener::OnError(
    /* [in] */ Int32 status)
{
    Slogger::I(AlwaysOnHotwordDetector::TAG, "onError: %d", status);
    Boolean result;
    return mHandler->SendEmptyMessage(AlwaysOnHotwordDetector::MSG_DETECTION_ERROR, &result);
}

ECode AlwaysOnHotwordDetector::SoundTriggerListener::OnRecognitionPaused()
{
    Slogger::I(TAG, "onRecognitionPaused");
    Boolean result;
    return mHandler->SendEmptyMessage(AlwaysOnHotwordDetector::MSG_DETECTION_PAUSE, &result);
}

ECode AlwaysOnHotwordDetector::SoundTriggerListener::OnRecognitionResumed()
{
    Slogger::I(TAG, "onRecognitionResumed");
    Boolean result;
    return mHandler->SendEmptyMessage(AlwaysOnHotwordDetector::MSG_DETECTION_RESUME, &result);
}

//================================
// AlwaysOnHotwordDetector::MyHandler
//================================

AlwaysOnHotwordDetector::MyHandler::MyHandler(
    /* [in] */ AlwaysOnHotwordDetector* host)
    : mHost(host)
{
}

ECode AlwaysOnHotwordDetector::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    {    AutoLock syncLock(this);
        if (mHost->mAvailability == AlwaysOnHotwordDetector::STATE_INVALID) {
            Int32 what;
            msg->GetWhat(&what);
            Slogger::W(TAG, "Received message: %d for an invalid detector", what);
            return E_NULL_POINTER_EXCEPTION;
        }
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case AlwaysOnHotwordDetector::MSG_AVAILABILITY_CHANGED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->mExternalCallback->OnAvailabilityChanged(arg1);
            break;
        }
        case AlwaysOnHotwordDetector::MSG_HOTWORD_DETECTED:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->mExternalCallback->OnDetected(IAlwaysOnHotwordDetectorEventPayload::Probe(obj.Get()));
            break;
        }
        case AlwaysOnHotwordDetector::MSG_DETECTION_ERROR: {
            mHost->mExternalCallback->OnError();
            break;
        }
        case AlwaysOnHotwordDetector::MSG_DETECTION_PAUSE:
            mHost->mExternalCallback->OnRecognitionPaused();
            break;
        case AlwaysOnHotwordDetector::MSG_DETECTION_RESUME:
            mHost->mExternalCallback->OnRecognitionResumed();
            break;
        default:
            Handler::HandleMessage(msg);
    }
    return NOERROR;
}

//================================
// AlwaysOnHotwordDetector::RefreshAvailabiltyTask
//================================

AlwaysOnHotwordDetector::RefreshAvailabiltyTask::RefreshAvailabiltyTask(
    /* [in] */ AlwaysOnHotwordDetector* host)
    : mHost(host)
{
}

ECode AlwaysOnHotwordDetector::RefreshAvailabiltyTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 availability;
    InternalGetInitialAvailability(&availability);
    Boolean enrolled = FALSE;
    // Fetch the sound model if the availability is one of the supported ones.
    if (availability == AlwaysOnHotwordDetector::STATE_NOT_READY
            || availability == IAlwaysOnHotwordDetector::STATE_KEYPHRASE_UNENROLLED
            || availability == IAlwaysOnHotwordDetector::STATE_KEYPHRASE_ENROLLED) {
        AutoPtr<KeyphraseMetadata> km = (KeyphraseMetadata*)((mHost->mKeyphraseMetadata).Get());
        InternalGetIsEnrolled(km->mId, (mHost->mLocale).Get(), &enrolled);
        if (!enrolled) {
            availability = IAlwaysOnHotwordDetector::STATE_KEYPHRASE_UNENROLLED;
        }
        else {
            availability = IAlwaysOnHotwordDetector::STATE_KEYPHRASE_ENROLLED;
        }
    }

    {    AutoLock syncLock(this);
        if (AlwaysOnHotwordDetector::DBG) {
            Slogger::D(AlwaysOnHotwordDetector::TAG, "Hotword availability changed from %d -> %d", mHost->mAvailability, availability);
        }
        mHost->mAvailability = availability;
        mHost->NotifyStateChangedLocked();
    }
    *result = NULL;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::RefreshAvailabiltyTask::InternalGetInitialAvailability(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        // This detector has already been invalidated.
        if (mHost->mAvailability == AlwaysOnHotwordDetector::STATE_INVALID) {
            *result = AlwaysOnHotwordDetector::STATE_INVALID;
            return NOERROR;
        }
    }

    AutoPtr<ISoundTriggerModuleProperties> dspModuleProperties = NULL;

    ECode ec = mHost->mModelManagementService->GetDspModuleProperties(mHost->mVoiceInteractionService, (ISoundTriggerModuleProperties**)&dspModuleProperties);
    if (FAILED(ec)) {
        Slogger::W(AlwaysOnHotwordDetector::TAG, "RemoteException in getDspProperties!");
        return E_REMOTE_EXCEPTION;
    }

    // No DSP available
    if (dspModuleProperties == NULL) {
        return IAlwaysOnHotwordDetector::STATE_HARDWARE_UNAVAILABLE;
    }
    // No enrollment application supports this keyphrase/locale
    if (mHost->mKeyphraseMetadata == NULL) {
        return IAlwaysOnHotwordDetector::STATE_KEYPHRASE_UNSUPPORTED;
    }
    *result = AlwaysOnHotwordDetector::STATE_NOT_READY;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::RefreshAvailabiltyTask::InternalGetIsEnrolled(
    /* [in] */ Int32 keyphraseId,
    /* [in] */ ILocale* locale,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String tag;
    locale->ToLanguageTag(&tag);
    ECode ec = mHost->mModelManagementService->IsEnrolledForKeyphrase(
        mHost->mVoiceInteractionService, keyphraseId, tag, result);
    if (FAILED(ec)) {
        Slogger::W(AlwaysOnHotwordDetector::TAG, "RemoteException in listRegisteredKeyphraseSoundModels!");
        return E_REMOTE_EXCEPTION;
    }
    *result = FALSE;
    return NOERROR;
}

//================================
// AlwaysOnHotwordDetector
//================================

const Int32 AlwaysOnHotwordDetector::STATE_INVALID;

const Int32 AlwaysOnHotwordDetector::STATE_NOT_READY;

const String AlwaysOnHotwordDetector::TAG("AlwaysOnHotwordDetector");
const Boolean AlwaysOnHotwordDetector::DBG = FALSE;

const Int32 AlwaysOnHotwordDetector::STATUS_ERROR;
const Int32 AlwaysOnHotwordDetector::STATUS_OK;

const Int32 AlwaysOnHotwordDetector::MSG_AVAILABILITY_CHANGED;
const Int32 AlwaysOnHotwordDetector::MSG_HOTWORD_DETECTED;
const Int32 AlwaysOnHotwordDetector::MSG_DETECTION_ERROR;
const Int32 AlwaysOnHotwordDetector::MSG_DETECTION_PAUSE;
const Int32 AlwaysOnHotwordDetector::MSG_DETECTION_RESUME;

CAR_INTERFACE_IMPL(AlwaysOnHotwordDetector, Object, IAlwaysOnHotwordDetector)

AlwaysOnHotwordDetector::AlwaysOnHotwordDetector()
    : mAvailability(STATE_NOT_READY)
{
}

ECode AlwaysOnHotwordDetector::constructor(
    /* [in] */ const String& text,
    /* [in] */ ILocale* locale,
    /* [in] */ IAlwaysOnHotwordDetectorCallback* callback,
    /* [in] */ IKeyphraseEnrollmentInfo* keyphraseEnrollmentInfo,
    /* [in] */ IIVoiceInteractionService* voiceInteractionService,
    /* [in] */ IIVoiceInteractionManagerService* modelManagementService)
{
    mText = text;
    mLocale = locale;
    mKeyphraseEnrollmentInfo = keyphraseEnrollmentInfo;
    mKeyphraseEnrollmentInfo->GetKeyphraseMetadata(text, locale, (IKeyphraseMetadata**)&mKeyphraseMetadata);
    mExternalCallback = (Callback*)callback;
    AutoPtr<MyHandler> mh = new MyHandler(this);
    mh->constructor();

    mHandler = (IHandler*)(mh.Get());
    AutoPtr<IIRecognitionStatusCallback> icb;
    CAlwaysOnHotwordDetectorSoundTriggerListener::New(mHandler, (IIRecognitionStatusCallback**)&icb);
    mInternalCallback = (SoundTriggerListener*)icb.Get();
    mVoiceInteractionService = voiceInteractionService;
    mModelManagementService = modelManagementService;
    AutoPtr<RefreshAvailabiltyTask> task = new RefreshAvailabiltyTask(this);
    assert(0);
    // task->Execute();
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::GetSupportedRecognitionModes(
    /* [out] */ Int32* srm)
{
    VALIDATE_NOT_NULL(srm)
    if (DBG) Slogger::D(TAG, "getSupportedRecognitionModes()");
    {    AutoLock syncLock(mLock);
        return GetSupportedRecognitionModesLocked(srm);
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::GetSupportedRecognitionModesLocked(
    /* [out] */ Int32* srml)
{
    VALIDATE_NOT_NULL(srml)
    if (mAvailability == STATE_INVALID) {
        Slogger::E(TAG, "getSupportedRecognitionModes called on an invalid detector");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // This method only makes sense if we can actually support a recognition.
    if (mAvailability != IAlwaysOnHotwordDetector::STATE_KEYPHRASE_ENROLLED
            && mAvailability != IAlwaysOnHotwordDetector::STATE_KEYPHRASE_UNENROLLED) {
        Slogger::E(TAG, "Getting supported recognition modes for the keyphrase is not supported");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<KeyphraseMetadata> _mKeyphraseMetadata = (KeyphraseMetadata*)(mKeyphraseMetadata.Get());
    *srml = _mKeyphraseMetadata->mRecognitionModeFlags;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::StartRecognition(/*@RecognitionFlags*/
/* [in] */ Int32 recognitionFlags,
/* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Slogger::D(TAG, "startRecognition(%d)", recognitionFlags);
    {    AutoLock syncLock(mLock);
        if (mAvailability == STATE_INVALID) {
            Slogger::E(TAG, "startRecognition called on an invalid detector");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        // Check if we can start/stop a recognition.
        if (mAvailability != IAlwaysOnHotwordDetector::STATE_KEYPHRASE_ENROLLED) {
            Slogger::E(TAG, "Recognition for the given keyphrase is not supported");
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        Int32 srl;
        StartRecognitionLocked(recognitionFlags, &srl);
        *result = srl == STATUS_OK;
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::StopRecognition(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Slogger::D(TAG, "stopRecognition()");
    {    AutoLock syncLock(mLock);
        if (mAvailability == STATE_INVALID) {
            Slogger::E(TAG, "stopRecognition called on an invalid detector");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        // Check if we can start/stop a recognition.
        if (mAvailability != IAlwaysOnHotwordDetector::STATE_KEYPHRASE_ENROLLED) {
            Slogger::E(TAG, "Recognition for the given keyphrase is not supported");
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        Int32 srl;
        StopRecognitionLocked(&srl);
        *result = srl == STATUS_OK;
        return NOERROR;
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::CreateEnrollIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    if (DBG) Slogger::D(TAG, "createEnrollIntent");
    {    AutoLock syncLock(mLock);
        return GetManageIntentLocked(IAlwaysOnHotwordDetector::MANAGE_ACTION_ENROLL, intent);
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::CreateUnEnrollIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    if (DBG) Slogger::D(TAG, "createUnEnrollIntent");
    {    AutoLock syncLock(mLock);
        return GetManageIntentLocked(IAlwaysOnHotwordDetector::MANAGE_ACTION_UN_ENROLL, intent);
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::CreateReEnrollIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    if (DBG) Slogger::D(TAG, "createReEnrollIntent");
    {    AutoLock syncLock(mLock);
        return GetManageIntentLocked(IAlwaysOnHotwordDetector::MANAGE_ACTION_RE_ENROLL, intent);
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::GetManageIntentLocked(
    /* [in] */ Int32 action,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    if (mAvailability == STATE_INVALID) {
        Slogger::E(TAG, "getManageIntent called on an invalid detector");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // This method only makes sense if we can actually support a recognition.
    if (mAvailability != IAlwaysOnHotwordDetector::STATE_KEYPHRASE_ENROLLED
            && mAvailability != IAlwaysOnHotwordDetector::STATE_KEYPHRASE_UNENROLLED) {
        Slogger::E(TAG, "Managing the given keyphrase is not supported");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    return mKeyphraseEnrollmentInfo->GetManageKeyphraseIntent(action, mText, mLocale, intent);
}

ECode AlwaysOnHotwordDetector::Invalidate()
{
    {    AutoLock syncLock(mLock);
        mAvailability = STATE_INVALID;
        NotifyStateChangedLocked();
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::OnSoundModelsChanged()
{
    {    AutoLock syncLock(mLock);
        if (mAvailability == STATE_INVALID
                || mAvailability == IAlwaysOnHotwordDetector::STATE_HARDWARE_UNAVAILABLE
                || mAvailability == IAlwaysOnHotwordDetector::STATE_KEYPHRASE_UNSUPPORTED) {
            Slogger::W(TAG, "Received onSoundModelsChanged for an unsupported keyphrase/config");
            return E_NULL_POINTER_EXCEPTION;
        }

        // Stop the recognition before proceeding.
        // This is done because we want to stop the recognition on an older model if it changed
        // or was deleted.
        // The availability change callback should ensure that the client starts recognition
        // again if needed.
        Int32 srl;
        StopRecognitionLocked(&srl);

        // Execute a refresh availability task - which should then notify of a change.
        AutoPtr<RefreshAvailabiltyTask> task = new RefreshAvailabiltyTask(this);
        assert(0);
        // task->Execute();
    }
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::StartRecognitionLocked(
    /* [in] */ Int32 recognitionFlags,
    /* [out] */ Int32* srl)
{
    VALIDATE_NOT_NULL(srl)
    AutoPtr<ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*> > recognitionExtra =
        ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>::Alloc(1);
    AutoPtr<ArrayOf<ISoundTriggerConfidenceLevel*> > confidenceLevel =
        ArrayOf<ISoundTriggerConfidenceLevel*>::Alloc(0);
    // TODO: Do we need to do something about the confidence level here?
    AutoPtr<KeyphraseMetadata> _mKeyphraseMetadata = (KeyphraseMetadata*)mKeyphraseMetadata.Get();
    CSoundTriggerKeyphraseRecognitionExtra::New(_mKeyphraseMetadata->mId,
        _mKeyphraseMetadata->mRecognitionModeFlags, 0, confidenceLevel.Get(),
        (ISoundTriggerKeyphraseRecognitionExtra**)&((*recognitionExtra)[0]));

    Boolean captureTriggerAudio =
            (recognitionFlags & IAlwaysOnHotwordDetector::RECOGNITION_FLAG_CAPTURE_TRIGGER_AUDIO) != 0;
    Boolean allowMultipleTriggers =
            (recognitionFlags & IAlwaysOnHotwordDetector::RECOGNITION_FLAG_ALLOW_MULTIPLE_TRIGGERS) != 0;
    Int32 code = STATUS_ERROR;

    String tag;
    mLocale->ToLanguageTag(&tag);
    AutoPtr<ISoundTriggerRecognitionConfig> rc;
    CSoundTriggerRecognitionConfig::New(captureTriggerAudio, allowMultipleTriggers,
        recognitionExtra, NULL /* additional data */, (ISoundTriggerRecognitionConfig**)&rc);
    ECode ec = mModelManagementService->StartRecognition(mVoiceInteractionService,
                _mKeyphraseMetadata->mId, tag, mInternalCallback, rc, &code);
    if (FAILED(ec)) {
        Slogger::W(TAG, "RemoteException in startRecognition!");
        return E_REMOTE_EXCEPTION;
    }
    if (code != STATUS_OK) {
        Slogger::W(TAG, "startRecognition() failed with error code %d", code);
    }
    *srl = code;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::StopRecognitionLocked(
    /* [out] */ Int32* srl)
{
    VALIDATE_NOT_NULL(srl)
    Int32 code = STATUS_ERROR;
    AutoPtr<KeyphraseMetadata> _mKeyphraseMetadata = (KeyphraseMetadata*)mKeyphraseMetadata.Get();

    ECode ec = mModelManagementService->StopRecognition(
        mVoiceInteractionService, _mKeyphraseMetadata->mId, mInternalCallback, &code);
    if (FAILED(ec)) {
        Slogger::W(TAG, "stopRecognition() failed with error code %d", code);
        return E_REMOTE_EXCEPTION;
    }

    if (code != STATUS_OK) {
        Slogger::W(TAG, "stopRecognition() failed with error code %d", code);
    }
    *srl = code;
    return NOERROR;
}

ECode AlwaysOnHotwordDetector::NotifyStateChangedLocked()
{
    AutoPtr<IMessage> message = CMessage::Obtain(mHandler, MSG_AVAILABILITY_CHANGED);
    message->SetArg1(mAvailability);
    message->SendToTarget();
    return NOERROR;
}

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos
