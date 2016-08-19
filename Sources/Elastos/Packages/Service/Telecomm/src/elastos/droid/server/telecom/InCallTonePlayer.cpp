
#include "elastos/droid/server/telecom/InCallTonePlayer.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallAudioManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/Ringer.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/droid/os/Looper.h>

using Elastos::Droid::Media::CToneGenerator;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// InCallTonePlayer::Factory
//=============================================================================
ECode InCallTonePlayer::Factory::constructor(
    /* [in] */ CallAudioManager* callAudioManager)
{
    IWeakReferenceSource* source = IWeakReferenceSource::Probe(TO_IINTERFACE(callAudioManager));
    source->GetWeakReference((IWeakReference**)&mCallAudioManager);
    return NOERROR;
}

ECode InCallTonePlayer::Factory::CreatePlayer(
    /* [in] */ Int32 tone,
    /* [out] */ InCallTonePlayer** result)
{
    AutoPtr<IInterface> ws;
    mCallAudioManager->Resolve(EIID_IInterface, (IInterface**)&ws);
    if (ws != NULL) {
        *result = new InCallTonePlayer(tone, (CallAudioManager*) IObject::Probe(ws));
        REFCOUNT_ADD(*result)
    } else {
        Log::E("InCallTonePlayer", "mCallAudioManager is NULL");
        *result = NULL;
        assert(0);
    }
    return NOERROR;
}

//=============================================================================
// InCallTonePlayer::SubRunnable
//=============================================================================
CAR_INTERFACE_IMPL(InCallTonePlayer::SubRunnable, Object, IRunnable)

InCallTonePlayer::SubRunnable::SubRunnable(
    /* [in] */ InCallTonePlayer* host)
    : mHost(host)
{}

ECode InCallTonePlayer::SubRunnable::Run()
{
    if (sTonesPlaying == 0) {
        Log::Wtf("InCallTonePlayer", "Over-releasing focus for tone player.");
    } else if (--sTonesPlaying == 0) {
        AutoPtr<IInterface> ws;
        mHost->mCallAudioManager->Resolve(EIID_IInterface, (IInterface**)&ws);
        if (ws != NULL) {
            ((CallAudioManager*) IObject::Probe(ws))->SetIsTonePlaying(FALSE);
        } else {
            Log::E("InCallTonePlayer", "mCallAudioManager is NULL");
            assert(0);
        }
    }
    return NOERROR;
}

//=============================================================================
// InCallTonePlayer
//=============================================================================
const Int32 InCallTonePlayer::TONE_INVALID = 0;
const Int32 InCallTonePlayer::TONE_BUSY = 1;
const Int32 InCallTonePlayer::TONE_CALL_ENDED = 2;
const Int32 InCallTonePlayer::TONE_OTA_CALL_ENDED = 3;
const Int32 InCallTonePlayer::TONE_CALL_WAITING = 4;
const Int32 InCallTonePlayer::TONE_CDMA_DROP = 5;
const Int32 InCallTonePlayer::TONE_CONGESTION = 6;
const Int32 InCallTonePlayer::TONE_INTERCEPT = 7;
const Int32 InCallTonePlayer::TONE_OUT_OF_SERVICE = 8;
const Int32 InCallTonePlayer::TONE_REDIAL = 9;
const Int32 InCallTonePlayer::TONE_REORDER = 10;
const Int32 InCallTonePlayer::TONE_RING_BACK = 11;
const Int32 InCallTonePlayer::TONE_UNOBTAINABLE_NUMBER = 12;
const Int32 InCallTonePlayer::TONE_VOICE_PRIVACY = 13;
const Int32 InCallTonePlayer::TONE_HOLD_RECALL = 14;
const Int32 InCallTonePlayer::TONE_SUPERVISORY_CH = 15;
const Int32 InCallTonePlayer::RELATIVE_VOLUME_EMERGENCY = 100;
const Int32 InCallTonePlayer::RELATIVE_VOLUME_HIPRI = 80;
const Int32 InCallTonePlayer::RELATIVE_VOLUME_LOPRI = 50;
const Int32 InCallTonePlayer::TIMEOUT_BUFFER_MILLIS = 20;
const Int32 InCallTonePlayer::STATE_OFF = 0;
const Int32 InCallTonePlayer::STATE_ON = 1;
const Int32 InCallTonePlayer::STATE_STOPPED = 2;
Int32 InCallTonePlayer::sTonesPlaying = 0;

InCallTonePlayer::InCallTonePlayer(
    /* [in] */ Int32 toneId,
    /* [in] */ CallAudioManager* callAudioManager)
    : mToneId(toneId)
{
    Thread::constructor();
    mState = STATE_OFF;

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(TO_IINTERFACE(callAudioManager));
    source->GetWeakReference((IWeakReference**)&mCallAudioManager);

    CHandler::New(Looper::GetMainLooper(), (IHandler**)&mMainThreadHandler);
}

ECode InCallTonePlayer::Run()
{
    AutoPtr<IToneGenerator> toneGenerator;
    // try {
    ECode ec;
    do {
        Log::D("InCallTonePlayer", "run(toneId = %d)", mToneId);
        Int32 toneType;  // Passed to ToneGenerator.startTone.
        Int32 toneVolume;  // Passed to the ToneGenerator constructor.
        Int32 toneLengthMillis;
        switch (mToneId) {
            case TONE_BUSY:
                // TODO: CDMA-specific tones
                toneType = IToneGenerator::TONE_SUP_BUSY;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 4000;
                break;
            case TONE_CALL_ENDED:
                toneType = IToneGenerator::TONE_PROP_PROMPT;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 200;
                break;
            case TONE_OTA_CALL_ENDED:
                // TODO: fill in
                Log::E("InCallTonePlayer", "OTA Call ended NYI.");
                return E_ILLEGAL_STATE_EXCEPTION;
            case TONE_CALL_WAITING:
                toneType = IToneGenerator::TONE_SUP_CALL_WAITING;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = Elastos::Core::Math::INT32_MAX_VALUE - TIMEOUT_BUFFER_MILLIS;
                break;
            case TONE_CDMA_DROP:
                toneType = IToneGenerator::TONE_CDMA_CALLDROP_LITE;
                toneVolume = RELATIVE_VOLUME_LOPRI;
                toneLengthMillis = 375;
                break;
            case TONE_CONGESTION:
                toneType = IToneGenerator::TONE_SUP_CONGESTION;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 4000;
                break;
            case TONE_INTERCEPT:
                toneType = IToneGenerator::TONE_CDMA_ABBR_INTERCEPT;
                toneVolume = RELATIVE_VOLUME_LOPRI;
                toneLengthMillis = 500;
                break;
            case TONE_OUT_OF_SERVICE:
                toneType = IToneGenerator::TONE_CDMA_CALLDROP_LITE;
                toneVolume = RELATIVE_VOLUME_LOPRI;
                toneLengthMillis = 375;
                break;
            case TONE_REDIAL:
                toneType = IToneGenerator::TONE_CDMA_ALERT_AUTOREDIAL_LITE;
                toneVolume = RELATIVE_VOLUME_LOPRI;
                toneLengthMillis = 5000;
                break;
            case TONE_REORDER:
                toneType = IToneGenerator::TONE_CDMA_REORDER;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 4000;
                break;
            case TONE_RING_BACK:
                toneType = IToneGenerator::TONE_SUP_RINGTONE;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = Elastos::Core::Math::INT32_MAX_VALUE - TIMEOUT_BUFFER_MILLIS;
                break;
            case TONE_UNOBTAINABLE_NUMBER:
                toneType = IToneGenerator::TONE_SUP_ERROR;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                toneLengthMillis = 4000;
                break;
            case TONE_HOLD_RECALL:
                toneType = IToneGenerator::TONE_HOLD_RECALL;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                // Call hold recall tone is stopped by stopTone() method
                toneLengthMillis = Elastos::Core::Math::INT32_MAX_VALUE - TIMEOUT_BUFFER_MILLIS;
                break;
            case TONE_SUPERVISORY_CH:
                toneType = IToneGenerator::TONE_SUPERVISORY_CH;
                toneVolume = RELATIVE_VOLUME_HIPRI;
                // Supervisory call held tone is stopped by stopTone() method
                toneLengthMillis = Elastos::Core::Math::INT32_MAX_VALUE - TIMEOUT_BUFFER_MILLIS;
                break;
            case TONE_VOICE_PRIVACY:
                // TODO: fill in.
                Log::E("InCallTonePlayer", "Voice privacy tone NYI.");
                return E_ILLEGAL_STATE_EXCEPTION;
            default:
                Log::E("InCallTonePlayer", "Bad toneId: %d", mToneId);
                return E_ILLEGAL_STATE_EXCEPTION;
        }
        Int32 stream = IAudioManager::STREAM_VOICE_CALL;
        Boolean isBluetoothAudioOn;
        AutoPtr<IInterface> ws;
        mCallAudioManager->Resolve(EIID_IInterface, (IInterface**)&ws);
        if (ws != NULL) {
            ((CallAudioManager*) IObject::Probe(ws))->IsBluetoothAudioOn(&isBluetoothAudioOn);
        } else {
            Log::E("InCallTonePlayer", "mCallAudioManager is NULL");
            assert(0);
        }

        if (isBluetoothAudioOn) {
            stream = IAudioManager::STREAM_BLUETOOTH_SCO;
        }
        if (toneType == IToneGenerator::TONE_SUPERVISORY_CH) {
            stream = IAudioManager::STREAM_INCALL_MUSIC;
        }
        // If the ToneGenerator creation fails, just continue without it. It is a local audio
        // signal, and is not as important.
        // try {
        Log::V("InCallTonePlayer", "Creating generator");
        ec = CToneGenerator::New(stream, toneVolume, (IToneGenerator**)&toneGenerator);
        // } catch (RuntimeException e) {
        if (FAILED(ec)) {
            if ((ECode)E_RUNTIME_EXCEPTION == ec) {
                Log::W("InCallTonePlayer", "Failed to create ToneGenerator.%d", ec);
                return NOERROR;
            }
            else {
                break;
            }
        }
        // }
        // TODO: Certain CDMA tones need to check the ringer-volume state before
        // playing. See CallNotifier.InCallTonePlayer.
        // TODO: Some tones play through the end of a call so we need to inform
        // CallAudioManager that we want focus the same way that Ringer does.
        {
            AutoLock lock(this);
            if (mState != STATE_STOPPED) {
                mState = STATE_ON;
                Boolean b;
                toneGenerator->StartTone(toneType, &b);
                // try {
                Log::V("InCallTonePlayer", "Starting tone %d...waiting for %d ms.", mToneId,
                        toneLengthMillis + TIMEOUT_BUFFER_MILLIS);
                ec = Wait(toneLengthMillis + TIMEOUT_BUFFER_MILLIS);
                // } catch (InterruptedException e) {
                if (FAILED(ec)) {
                    if ((ECode) E_INTERRUPTED_EXCEPTION == ec) {
                        Log::W("InCallTonePlayer", "wait interrupted %d", ec);
                    }
                    else {
                        break;
                    }
                }
                // }
            }
        }
        mState = STATE_OFF;
    } while(FALSE);
    // } finally {
    if (toneGenerator != NULL) {
        toneGenerator->ReleaseResources();
    }
    CleanUpTonePlayer();
    return ec;
    // }
}

ECode InCallTonePlayer::StartTone()
{
    ThreadUtil::CheckOnMainThread();
    sTonesPlaying++;
    if (sTonesPlaying == 1) {
        AutoPtr<IInterface> ws;
        mCallAudioManager->Resolve(EIID_IInterface, (IInterface**)&ws);
        if (ws != NULL) {
            ((CallAudioManager*) IObject::Probe(ws))->SetIsTonePlaying(TRUE);
        } else {
            Log::E("InCallTonePlayer", "mCallAudioManager is NULL");
            assert(0);
        }
    }
    Start();
    return NOERROR;
}

ECode InCallTonePlayer::StopTone()
{
    {
        AutoLock lock(this);
        if (mState == STATE_ON) {
            Log::D("InCallTonePlayer", "Stopping the tone %d.", mToneId);
            Notify();
        }
        mState = STATE_STOPPED;
    }
    return NOERROR;
}

ECode InCallTonePlayer::CleanUpTonePlayer()
{
    // Release focus on the main thread.
    Boolean b;
    return mMainThreadHandler->Post(new SubRunnable(this), &b);
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
