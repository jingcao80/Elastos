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

#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/media/_AudioErrors.h"
#include "elastos/droid/media/CAudioAttributes.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/CAudioFormat.h"
#include "elastos/droid/media/CAudioFormatBuilder.h"
#include "elastos/droid/media/CAudioRecord.h"
#include "elastos/droid/media/CMediaRecorder.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <binder/AppOpsManager.h>
#include <binder/IPCThreadState.h>
#include <media/AudioRecord.h>
#include <system/audio.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioFormat;
using Elastos::Droid::Media::CAudioFormatBuilder;
using Elastos::Droid::Media::CMediaRecorder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

#define AUDIORECORD_SUCCESS                          0
#define AUDIORECORD_ERROR                           -1
#define AUDIORECORD_ERROR_BAD_VALUE                 -2
#define AUDIORECORD_ERROR_INVALID_OPERATION         -3
#define AUDIORECORD_ERROR_SETUP_ZEROFRAMECOUNT      -16
#define AUDIORECORD_ERROR_SETUP_INVALIDCHANNELMASK  -17
#define AUDIORECORD_ERROR_SETUP_INVALIDFORMAT       -18
#define AUDIORECORD_ERROR_SETUP_INVALIDSOURCE       -19
#define AUDIORECORD_ERROR_SETUP_NATIVEINITFAILED    -20

static inline audio_format_t audioFormatToNative(int audioFormat)
{
    switch (audioFormat) {
    case IAudioFormat::ENCODING_PCM_16BIT:
        return AUDIO_FORMAT_PCM_16_BIT;
    case IAudioFormat::ENCODING_PCM_8BIT:
        return AUDIO_FORMAT_PCM_8_BIT;
    case IAudioFormat::ENCODING_PCM_FLOAT:
        return AUDIO_FORMAT_PCM_FLOAT;
    case IAudioFormat::ENCODING_AC3:
        return AUDIO_FORMAT_AC3;
    case IAudioFormat::ENCODING_E_AC3:
        return AUDIO_FORMAT_E_AC3;
    case IAudioFormat::ENCODING_DEFAULT:
        return AUDIO_FORMAT_DEFAULT;
    default:
        return AUDIO_FORMAT_INVALID;
    }
}

// static Mutex sLock;
// static SortedVector <audiorecord_callback_cookie *> sAudioRecordCallBackCookies;

const String CAudioRecord::TAG("CAudioRecord");

const Int32 CAudioRecord::NATIVE_EVENT_MARKER = 2;
const Int32 CAudioRecord::NATIVE_EVENT_NEW_POS = 3;

//------------------------------------
//    CAudioRecord::NativeEventHandler
//------------------------------------

ECode CAudioRecord::NativeEventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<IAudioRecordOnRecordPositionUpdateListener> listener;

    Object& lock = mOwner->mPositionListenerLock;
    {    AutoLock syncLock(lock);
        listener = mOwner->mPositionListener;
    }

    switch(what) {
        case NATIVE_EVENT_MARKER: {
            if (listener != NULL) {
                listener->OnMarkerReached(IAudioRecord::Probe(mOwner));
            }
            break;
        }
        case NATIVE_EVENT_NEW_POS: {
            if (listener != NULL) {
                listener->OnPeriodicNotification(IAudioRecord::Probe(mOwner));
            }
            break;
        }
        default: {
            Logger::E(TAG, "[ android.media.AudioRecord.NativeEventHandler ] " \
                "Unknown event type: %d", what);
            break;
        }
    }
    return NOERROR;
}

//------------------------------------
//    CAudioRecord
//------------------------------------

CAR_INTERFACE_IMPL(CAudioRecord, Object, IAudioRecord)

CAR_OBJECT_IMPL(CAudioRecord)

CAudioRecord::CAudioRecord()
    : mNativeRecorderInJavaObj(0)
    , mNativeCallbackCookie(0)
    , mSampleRate(0)
    , mChannelCount(0)
    , mChannelMask(0)
    , mAudioFormat(0)
    , mRecordSource(0)
    , mState(STATE_UNINITIALIZED)
    , mRecordingState(RECORDSTATE_STOPPED)
    , mNativeBufferSizeInBytes(0)
    , mSessionId(IAudioSystem::AUDIO_SESSION_ALLOCATE)
    , mIsSubmixFullVolume(FALSE)
{
    CBinder::New((IBinder**)&mICallBack);
}

CAudioRecord::~CAudioRecord()
{
    Finalize();
}

void CAudioRecord::Finalize()
{
    // will cause stop() to be called, and if appropriate, will handle fixed volume recording
    ReleaseResources();
}

ECode CAudioRecord::constructor(
    /* [in] */ Int32 audioSource,
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 bufferSizeInBytes)
{
    AutoPtr<IAudioAttributesBuilder> aaBuilder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aaBuilder);
    aaBuilder->SetInternalCapturePreset(audioSource);
    AutoPtr<IAudioAttributes> aa;
    aaBuilder->Build((IAudioAttributes**)&aa);

    AutoPtr<IAudioFormatBuilder> afBuilder;
    CAudioFormatBuilder::New((IAudioFormatBuilder**)&afBuilder);
    Int32 val;
    GetChannelMaskFromLegacyConfig(channelConfig,
            TRUE/*allow legacy configurations*/, &val);
    afBuilder->SetChannelMask(val);
    afBuilder->SetEncoding(audioFormat);
    afBuilder->SetSampleRate(sampleRateInHz);
    AutoPtr<IAudioFormat> af;
    afBuilder->Build((IAudioFormat**)&af);
    return constructor(aa, af, bufferSizeInBytes, IAudioManager::AUDIO_SESSION_ID_GENERATE);
}

ECode CAudioRecord::constructor(
    /* [in] */ IAudioAttributes* attributes,
    /* [in] */ IAudioFormat* format,
    /* [in] */ Int32 bufferSizeInBytes,
    /* [in] */ Int32 sessionId)
{
    mRecordingState = RECORDSTATE_STOPPED;

    if (attributes == NULL) {
        // throw new IllegalArgumentException("Illegal NULL AudioAttributes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (format == NULL) {
        // throw new IllegalArgumentException("Illegal NULL AudioFormat");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // remember which looper is associated with the AudioRecord instanciation
    if ((mInitializationLooper = Looper::GetMyLooper()) == NULL) {
        mInitializationLooper = Looper::GetMainLooper();
    }

    mAudioAttributes = attributes;

    // is this AudioRecord using REMOTE_SUBMIX at full volume?
    Int32 val;
    mAudioAttributes->GetCapturePreset(&val);
    if (val == IMediaRecorderAudioSource::REMOTE_SUBMIX) {
        AutoPtr<ISet> tags;
        mAudioAttributes->GetTags((ISet**)&tags);
        AutoPtr<IIterator> tagsIter;
        tags->GetIterator((IIterator**)&tagsIter);
        Boolean b;
        while (tagsIter->HasNext(&b), b) {
            AutoPtr<IInterface> obj;
            tagsIter->GetNext((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            if(str.EqualsIgnoreCase(SUBMIX_FIXED_VOLUME)) {
                mIsSubmixFullVolume = TRUE;
                Logger::V(TAG, "Will record from REMOTE_SUBMIX at full fixed volume");
                break;
            }
        }
    }

    Int32 mask;
    format->GetPropertySetMask(&mask);
    Int32 rate = 0;
    if ((mask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_SAMPLE_RATE) != 0)
    {
        format->GetSampleRate(&rate);
    }
    else {
        AudioSystem::GetPrimaryOutputSamplingRate(&rate);
        if (rate <= 0) {
            rate = 44100;
        }
    }

    Int32 encoding = IAudioFormat::ENCODING_DEFAULT;
    if ((mask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_ENCODING) != 0)
    {
        format->GetEncoding(&encoding);
    }

    Int32 preset;
    attributes->GetCapturePreset(&preset);
    AudioParamCheck(preset, rate, encoding);

    Int32 cMask;
    format->GetChannelMask(&cMask);
    CAudioFormat::ChannelCountFromInChannelMask(cMask, &mChannelCount);
    GetChannelMaskFromLegacyConfig(cMask, FALSE, &mChannelMask);

    FAIL_RETURN(AudioBuffSizeCheck(bufferSizeInBytes));

    // native initialization
    AutoPtr< ArrayOf<Int32> > session = ArrayOf<Int32>::Alloc(1);
    (*session)[0] = sessionId;

    //TODO: update native initialization when information about hardware init failure
    //      due to capture device already open is available.
    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);

    Int32 initResult = NativeSetup(weakHost, mAudioAttributes, mSampleRate,
        mChannelMask, mAudioFormat, mNativeBufferSizeInBytes, session);

    if (initResult != SUCCESS) {
        Loge(String("Error code ")
            + StringUtils::ToString(initResult)
            + String(" when initializing native AudioRecord object."));
        return NOERROR; // with mState == STATE_UNINITIALIZED
    }

    mSessionId = (*session)[0];
    mState = STATE_INITIALIZED;
    return NOERROR;
}

//---------------------------------------------------------
// Transport control methods
//--------------------
ECode CAudioRecord::StartRecording() // throws IllegalStateException
{
    Boolean isAudioRecordAllowed = FALSE;
    if (IsAudioRecordAllowed(&isAudioRecordAllowed), !isAudioRecordAllowed) {
        Logger::E(TAG, "User permission denied!");
        return NOERROR;
    }
    if (mState != STATE_INITIALIZED) {
        // throw new IllegalStateException("startRecording() called on an "
        //         + "uninitialized AudioRecord.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // start recording
    {    AutoLock syncLock(mRecordingStateLock);
        Int32 result;
        NativeStart(IMediaSyncEvent::SYNC_EVENT_NONE, 0, &result);
        if (result == SUCCESS) {
            HandleFullVolumeRec(TRUE);
            mRecordingState = RECORDSTATE_RECORDING;
        }
    }
    return NOERROR;
}

ECode CAudioRecord::StartRecording( // throws IllegalStateException
    /* [in] */ IMediaSyncEvent* syncEvent)
{
    Boolean isAudioRecordAllowed = FALSE;
    if (IsAudioRecordAllowed(&isAudioRecordAllowed), !isAudioRecordAllowed) {
        Logger::E(TAG, "User permission denied!");
        return NOERROR;
    }
    if (mState != STATE_INITIALIZED) {
        /*throw(new IllegalStateException("startRecording() called on an "
                +"uninitialized AudioRecord."));*/
        Logger::E(TAG, "startRecording() called on an uninitialized AudioRecord.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // start recording
    Int32 tempValue1;
    Int32 tempValue2;
    syncEvent->GetType(&tempValue1);
    syncEvent->GetAudioSessionId(&tempValue2);

    {    AutoLock syncLock(mRecordingStateLock);
        Int32 result;
        NativeStart(tempValue1, tempValue2, &result);
        if (result == SUCCESS) {
            HandleFullVolumeRec(TRUE);
            mRecordingState = RECORDSTATE_RECORDING;
        }
    }
    return NOERROR;
}

ECode CAudioRecord::Stop() // throws IllegalStateException
{
    if (mState != STATE_INITIALIZED) {
        //throw(new IllegalStateException("stop() called on an uninitialized AudioRecord."));
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // stop recording
    {    AutoLock syncLock(mRecordingStateLock);
        HandleFullVolumeRec(FALSE);

        NativeStop();
        mRecordingState = RECORDSTATE_STOPPED;
    }
    return NOERROR;
}

//---------------------------------------------------------
// Audio data supply
//--------------------
ECode CAudioRecord::Read(
    /* [in] */ ArrayOf<Byte>* audioData,
    /* [in] */ Int32 offsetInBytes,
    /* [in] */ Int32 sizeInBytes,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mState != STATE_INITIALIZED) {
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ((audioData == NULL) || (offsetInBytes < 0 ) || (sizeInBytes < 0)
         || (offsetInBytes + sizeInBytes < 0)  // detect integer overflow
         || (offsetInBytes + sizeInBytes > audioData->GetLength())) {
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    *result = Native_read_in_byte_array(audioData, offsetInBytes, sizeInBytes);
    return NOERROR;
}

ECode CAudioRecord::Read(
    /* [in] */ ArrayOf<Int16>* audioData,
    /* [in] */ Int32 offsetInShorts,
    /* [in] */ Int32 sizeInShorts,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mState != STATE_INITIALIZED) {
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ( (audioData == NULL) || (offsetInShorts < 0 ) || (sizeInShorts < 0)
         || (offsetInShorts + sizeInShorts < 0)  // detect integer overflow
         || (offsetInShorts + sizeInShorts > audioData->GetLength())) {
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    *result = Native_read_in_short_array(audioData, offsetInShorts, sizeInShorts);
    return NOERROR;
}

ECode CAudioRecord::Read(
    /* [in] */ IByteBuffer* audioBuffer,
    /* [in] */ Int32 sizeInBytes,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mState != STATE_INITIALIZED) {
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ( (audioBuffer == NULL) || (sizeInBytes < 0) ) {
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    *result = Native_read_in_direct_buffer(audioBuffer, sizeInBytes);
    return NOERROR;
}

//--------------------------------------------------------------------------
// Initialization / configuration
//--------------------
ECode CAudioRecord::SetRecordPositionUpdateListener(
    /* [in] */ IAudioRecordOnRecordPositionUpdateListener* listener)
{
    return SetRecordPositionUpdateListener(listener, NULL);
}

ECode CAudioRecord::SetRecordPositionUpdateListener(
    /* [in] */ IAudioRecordOnRecordPositionUpdateListener* listener,
    /* [in] */ IHandler* handler)
{
    {    AutoLock syncLock(mPositionListenerLock);
        mPositionListener = listener;

        if (listener != NULL) {
            if (handler != NULL) {
                AutoPtr<ILooper> looper;
                handler->GetLooper((ILooper**)&looper);
                mEventHandler = new NativeEventHandler(looper, this);
            }
            else {
                // no given handler, use the looper the AudioRecord was created in
                mEventHandler = new NativeEventHandler(mInitializationLooper, this);
            }
        }
        else {
            mEventHandler = NULL;
        }
    }
    return NOERROR;
}

ECode CAudioRecord::SetNotificationMarkerPosition(
    /* [in] */ Int32 markerInFrames,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState == IAudioRecord::STATE_UNINITIALIZED) {
        *result = IAudioRecord::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    return Native_set_marker_pos(markerInFrames, result);
}

ECode CAudioRecord::SetPositionNotificationPeriod(
    /* [in] */ Int32 periodInFrames,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState == IAudioRecord::STATE_UNINITIALIZED) {
        *result = IAudioRecord::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    return Native_set_pos_update_period(periodInFrames, result);
}

ECode CAudioRecord::AudioParamCheck(
    /* [in] */ Int32 audioSource,
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 audioFormat)
{
    //--------------
    // audio source
    Int32 tempValue;
    if ((audioSource < IMediaRecorderAudioSource::DEFAULT) ||
         ((audioSource > (CMediaRecorder::GetAudioSourceMax(&tempValue), tempValue)) &&
         (audioSource != IMediaRecorderAudioSource::HOTWORD))) {
        //throw (new IllegalArgumentException("Invalid audio source."));
        Logger::E(TAG, "Invalid audio source.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRecordSource = audioSource;

    //--------------
    // sample rate
    if ( (sampleRateInHz < 4000) || (sampleRateInHz > 48000) ) {
        /*throw (new IllegalArgumentException(sampleRateInHz
                + "Hz is not a supported sample rate."));*/
        Logger::E(TAG, "Hz is not a supported sample rate.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSampleRate = sampleRateInHz;

    //--------------
    // audio format
    switch (audioFormat) {
        case IAudioFormat::ENCODING_DEFAULT: {
            mAudioFormat = IAudioFormat::ENCODING_PCM_16BIT;
            break;
        }
        case IAudioFormat::ENCODING_PCM_16BIT:
        case IAudioFormat::ENCODING_PCM_8BIT:
        case IAudioFormat::ENCODING_AMRNB:
        case IAudioFormat::ENCODING_AMRWB:
        case IAudioFormat::ENCODING_EVRC:
        case IAudioFormat::ENCODING_EVRCB:
        case IAudioFormat::ENCODING_EVRCWB:
        case IAudioFormat::ENCODING_EVRCNW: {
            mAudioFormat = audioFormat;
            break;
        }
        default:
        {
            /*throw (new IllegalArgumentException("Unsupported sample encoding."
                    + " Should be ENCODING_PCM_8BIT or ENCODING_PCM_16BIT."));*/
            Logger::E(TAG, "Unsupported sample encoding. Should be ENCODING_PCM_8BIT or ENCODING_PCM_16BIT.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CAudioRecord::AudioBuffSizeCheck(
    /* [in] */ Int32 audioBufferSize)
{
    // NB: this section is only valid with PCM data.
    // To update when supporting compressed formats
    Int32 val;
    CAudioFormat::GetBytesPerSample(mAudioFormat, &val);
    Int32 frameSizeInBytes = mChannelCount * val;
    //overwrite frameSizeInBytes for compress voip
    if ((mRecordSource == IMediaRecorderAudioSource::VOICE_COMMUNICATION) &&
        (mAudioFormat != IAudioFormat::ENCODING_PCM_16BIT)) {
        frameSizeInBytes = mChannelCount * 1;
    }
    if ((audioBufferSize % frameSizeInBytes != 0) || (audioBufferSize < 1)) {
        Logger::E(TAG, "Invalid audio buffer size.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNativeBufferSizeInBytes = audioBufferSize;
    return NOERROR;
}

ECode CAudioRecord::ReleaseResources()
{
    // try {
    Stop();
    // } catch(IllegalStateException ise) {
    //     don't raise an exception, we're releasing the resources.
    // }
    NativeRelease();
    mState = STATE_UNINITIALIZED;
    return NOERROR;
}

ECode CAudioRecord::GetSampleRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSampleRate;
    return NOERROR;
}

ECode CAudioRecord::GetAudioSource(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRecordSource;
    return NOERROR;
}

ECode CAudioRecord::GetAudioFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioFormat;
    return NOERROR;
}

ECode CAudioRecord::GetChannelConfiguration(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannelMask;
    return NOERROR;
}

ECode CAudioRecord::GetChannelCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannelCount;
    return NOERROR;
}

ECode CAudioRecord::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode CAudioRecord::GetRecordingState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(mRecordingStateLock);
        *result = mRecordingState;
    }
    return NOERROR;
}

ECode CAudioRecord::GetNotificationMarkerPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return Native_get_marker_pos(result);
}

ECode CAudioRecord::GetPositionNotificationPeriod(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return Native_get_pos_update_period(result);
}

ECode CAudioRecord::GetMinBufferSize(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 channelCount = 0;
    switch(channelConfig) {
        case IAudioFormat::CHANNEL_IN_DEFAULT: // AudioFormat.CHANNEL_CONFIGURATION_DEFAULT
        case IAudioFormat::CHANNEL_IN_MONO:
        case IAudioFormat::CHANNEL_CONFIGURATION_MONO:
        {
            channelCount = 1;
            break;
        }
        case IAudioFormat::CHANNEL_IN_STEREO:
        case IAudioFormat::CHANNEL_CONFIGURATION_STEREO:
        case (IAudioFormat::CHANNEL_IN_FRONT | IAudioFormat::CHANNEL_IN_BACK):
        {
            channelCount = 2;
            break;
        }
        case IAudioFormat::CHANNEL_IN_5POINT1:
            channelCount = 6;
            break;
        case IAudioFormat::CHANNEL_INVALID:
        default:
        {
            Loge(String("getMinBufferSize(): Invalid channel configuration."));
            *result = IAudioRecord::ERROR_BAD_VALUE;
            return NOERROR;
        }
    }

    // PCM_8BIT is not supported at the moment
    if (audioFormat != IAudioFormat::ENCODING_PCM_16BIT
        && audioFormat != IAudioFormat::ENCODING_AMRNB
        && audioFormat != IAudioFormat::ENCODING_AMRWB
        && audioFormat != IAudioFormat::ENCODING_EVRC
        && audioFormat != IAudioFormat::ENCODING_EVRCB
        && audioFormat != IAudioFormat::ENCODING_EVRCWB
        && audioFormat != IAudioFormat::ENCODING_EVRCNW) {
        Loge(String("getMinBufferSize(): Invalid audio format."));
        *result = IAudioRecord::ERROR_BAD_VALUE;
        return NOERROR;
    }

    Int32 size = Native_get_min_buff_size(sampleRateInHz, channelCount, audioFormat);
    if (size == 0) {
        *result = IAudioRecord::ERROR_BAD_VALUE;
        return NOERROR;
    }
    else if (size == -1) {
        *result = IAudioRecord::ERROR;
        return NOERROR;
    }
    else {
        *result = size;
        return NOERROR;
    }
    return NOERROR;
}

ECode CAudioRecord::GetAudioSessionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSessionId;
    return NOERROR;
}

void CAudioRecord::HandleFullVolumeRec(
    /* [in] */ Boolean starting)
{
    if (!mIsSubmixFullVolume) {
        return;
    }

    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::AUDIO_SERVICE);
    assert(b != NULL);
    AutoPtr<IIAudioService> ias = IIAudioService::Probe(b);
    // try {
    ias->ForceRemoteSubmixFullVolume(starting, mICallBack);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error talking to AudioService when handling full submix volume", e);
    // }
}

ECode CAudioRecord::GetChannelMaskFromLegacyConfig(
    /* [in] */ Int32 inChannelConfig,
    /* [in] */ Boolean allowLegacyConfig,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 mask;
    switch (inChannelConfig) {
    case IAudioFormat::CHANNEL_IN_DEFAULT: // IAudioFormat::CHANNEL_CONFIGURATION_DEFAULT
    case IAudioFormat::CHANNEL_IN_MONO:
    case IAudioFormat::CHANNEL_CONFIGURATION_MONO:
        mask = IAudioFormat::CHANNEL_IN_MONO;
        break;
    case IAudioFormat::CHANNEL_IN_STEREO:
    case IAudioFormat::CHANNEL_CONFIGURATION_STEREO:
        mask = IAudioFormat::CHANNEL_IN_STEREO;
        break;
    case (IAudioFormat::CHANNEL_IN_FRONT | IAudioFormat::CHANNEL_IN_BACK):
        mask = inChannelConfig;
        break;
    default:
        // throw new IllegalArgumentException("Unsupported channel configuration.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!allowLegacyConfig && ((inChannelConfig == IAudioFormat::CHANNEL_CONFIGURATION_MONO)
            || (inChannelConfig == IAudioFormat::CHANNEL_CONFIGURATION_STEREO))) {
        // only happens with the constructor that uses AudioAttributes and AudioFormat
        // throw new IllegalArgumentException("Unsupported deprecated configuration.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = mask;
    return NOERROR;
}

//---------------------------------------------------------
// Java methods called from the native side
//--------------------
void CAudioRecord::PostEventFromNative(
    /* [in] */ IInterface* audiorecord_ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    StringBuilder sb("Event posted from the native side: event=");
    sb += what;
    sb += " args=";
    sb += arg1;
    sb += " ";
    sb += arg2;
    Logd(sb.ToString());

    AutoPtr<IWeakReference> wr = IWeakReference::Probe(audiorecord_ref);
    AutoPtr<IAudioRecord> recorder;
    wr->Resolve(EIID_IAudioRecord, (IInterface**)&recorder);
    if (recorder == NULL) {
        return;
    }

    CAudioRecord* car = (CAudioRecord*)recorder.Get();
    if (car->mEventHandler != NULL) {
        AutoPtr<IMessage> m;
        car->mEventHandler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&m);
        Boolean result;
        car->mEventHandler->SendMessage(m, &result);
    }
}

//-------------------------------------------------
//    Native Method : android_media_AudioRecord.cpp
//-------------------------------------------------

struct audiorecord_callback_cookie
{
    AutoPtr<IWeakReference> mAudioRecordRef;
    Boolean                 mBusy;
};

Int32 translateRecorderErrorCode(Int32 code)
{
    switch(code) {
    case android::NO_ERROR:
        return AUDIORECORD_SUCCESS;
    case android::BAD_VALUE:
        return AUDIORECORD_ERROR_BAD_VALUE;
    case android::INVALID_OPERATION:
        return AUDIORECORD_ERROR_INVALID_OPERATION;
    default:
        return AUDIORECORD_ERROR;
    }
}

void CAudioRecord::recorderCallback(Int32 event, void* user, void *info)
{
    if (event == android::AudioRecord::EVENT_MORE_DATA) {
        // set size to 0 to signal we're not using the callback to read more data
        android::AudioRecord::Buffer* pBuff = (android::AudioRecord::Buffer*)info;
        pBuff->size = 0;

    } else if (event == android::AudioRecord::EVENT_MARKER) {
        audiorecord_callback_cookie *callbackInfo = (audiorecord_callback_cookie *)user;
        if (user) {
            PostEventFromNative(callbackInfo->mAudioRecordRef, event, 0,0, NULL);
        }

    } else if (event == android::AudioRecord::EVENT_NEW_POS) {
        audiorecord_callback_cookie *callbackInfo = (audiorecord_callback_cookie *)user;
        if (user) {
            PostEventFromNative(callbackInfo->mAudioRecordRef, event, 0,0, NULL);
        }
    }
}

Int32 CAudioRecord::NativeSetup(
    /* [in] */ IWeakReference* audiorecord_this,
    /* [in] */ IAudioAttributes* attributes,
    /* [in] */ Int32 sampleRateInHertz,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 buffSizeInBytes,
    /* [in] */ ArrayOf<Int32>* session)
{
    //ALOGV(">> Entering android_media_AudioRecord_setup");
    //ALOGV("sampleRate=%d, audioFormat=%d, channel mask=%x, buffSizeInBytes=%d",
    //     sampleRateInHertz, audioFormat, channelMask, buffSizeInBytes);

    if (attributes == 0) {
        ALOGE("Error creating AudioRecord: invalid audio attributes");
        return IAudioSystem::ERROR;
    }

    if (!audio_is_input_channel(channelMask)) {
        ALOGE("Error creating AudioRecord: channel mask %#x is not valid.", channelMask);
        return AUDIORECORD_ERROR_SETUP_INVALIDCHANNELMASK;
    }
    uint32_t channelCount = audio_channel_count_from_in_mask(channelMask);

    // compare the format against the Java constants
    audio_format_t format = audioFormatToNative(audioFormat);
    if (format == AUDIO_FORMAT_INVALID) {
        ALOGE("Error creating AudioRecord: unsupported audio format %d.", audioFormat);
        return AUDIORECORD_ERROR_SETUP_INVALIDFORMAT;
    }

    size_t bytesPerSample = audio_bytes_per_sample(format);

    if (buffSizeInBytes == 0) {
         ALOGE("Error creating AudioRecord: frameCount is 0.");
        return AUDIORECORD_ERROR_SETUP_ZEROFRAMECOUNT;
    }

    if (session == NULL) {
        ALOGE("Error creating AudioRecord: invalid session ID pointer");
        return IAudioSystem::ERROR;
    }

    Int32 sessionId = (*session)[0];

    // create an uninitialized AudioRecord object
    android::sp<android::AudioRecord> lpRecorder = new android::AudioRecord();

    AutoPtr<CAudioAttributes> attr = (CAudioAttributes*)attributes;
    audio_attributes_t *paa = NULL;
    // read the AudioAttributes values
    paa = (audio_attributes_t *) calloc(1, sizeof(audio_attributes_t));
    const char* tags = attr->mFormattedTags.string();
    // copying array size -1, char array for tags was calloc'd, no need to NULL-terminate it
    strncpy(paa->tags, tags, AUDIO_ATTRIBUTES_TAGS_MAX_SIZE - 1);

    paa->source = (audio_source_t) attr->mSource;
    paa->flags = (audio_flags_mask_t)attr->mFlags;

    //overwrite bytesPerSample for compress VOIP use cases
    if ((paa->source == AUDIO_SOURCE_VOICE_COMMUNICATION) &&
        (format != AUDIO_FORMAT_PCM_16_BIT)) {
        bytesPerSample = sizeof(uint8_t);
    }

    size_t frameSize = channelCount * bytesPerSample;
    size_t frameCount = buffSizeInBytes / frameSize;

    ALOGV("AudioRecord_setup for source=%d tags=%s flags=%08x", paa->source, paa->tags, paa->flags);

    audio_input_flags_t flags = AUDIO_INPUT_FLAG_NONE;
    if (paa->flags & AUDIO_FLAG_HW_HOTWORD) {
        flags = AUDIO_INPUT_FLAG_HW_HOTWORD;
    }
    // create the callback information:
    // this data will be passed with every AudioRecord callback
    audiorecord_callback_cookie *lpCallbackData = new audiorecord_callback_cookie;
    // we use a weak reference so the AudioRecord object can be garbage collected.
    lpCallbackData->mAudioRecordRef = audiorecord_this;
    lpCallbackData->mBusy = false;

    const android::status_t status = lpRecorder->set(paa->source,
        sampleRateInHertz,
        format,        // word length, PCM
        channelMask,
        frameCount,
        recorderCallback,// callback_t
        lpCallbackData,// void* user
        0,             // notificationFrames,
        true,          // threadCanCallJava
        sessionId,
        android::AudioRecord::TRANSFER_DEFAULT,
        flags);

    if (status != android::NO_ERROR) {
        ALOGE("Error creating AudioRecord instance: initialization check failed with status %d.",
                status);
        goto native_init_failure;
    }

    // read the audio session ID back from AudioRecord in case a new session was created during set()
    (*session)[0] = lpRecorder->getSessionId();

    // {   // scope for the lock
    //     Mutex::Autolock l(sLock);
    //     sAudioRecordCallBackCookies.add(lpCallbackData);
    // }
    // save our newly created C++ AudioRecord in the "nativeRecorderInJavaObj" field
    // of the Java object
    mNativeRecorderInJavaObj = (Int64)lpRecorder.get();

    // save our newly created callback information in the "nativeCallbackCookie" field
    // of the Java object (in mNativeCallbackCookie) so we can free the memory in finalize()
    mNativeCallbackCookie = (Int64)lpCallbackData;

    return IAudioSystem::SUCCESS;

    // failure:
native_init_failure:
    delete lpCallbackData;
    mNativeCallbackCookie = 0;

    return AUDIORECORD_ERROR_SETUP_NATIVEINITFAILED;
}

void CAudioRecord::NativeFinalize()
{
    NativeRelease();
}

void CAudioRecord::NativeRelease()
{
    // serialize access. Ugly, but functional.
    android::AudioRecord* lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;

    audiorecord_callback_cookie* lpCookie = (audiorecord_callback_cookie *)mNativeCallbackCookie;

    // reset the native resources in the Java object so any attempt to access
    // them after a call to release fails.
    mNativeRecorderInJavaObj = 0;
    mNativeCallbackCookie = 0;

    // delete the AudioRecord object
    if (lpRecorder) {
        ALOGV("About to delete lpRecorder: %p\n", lpRecorder);
        lpRecorder->stop();
// TODO:
        // delete lpRecorder;
    }

    // delete the callback information
    if (lpCookie) {
        ALOGV("deleting lpCookie: %p\n", lpCookie);
        delete lpCookie;
    }
}

ECode CAudioRecord::NativeStart(
    /* [in] */ Int32 syncEvent,
    /* [in] */ Int32 sessionId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    if (lpRecorder == NULL) {
        //jniThrowException(env, "java/lang/IllegalStateException", NULL);
        *result = IAudioSystem::ERROR;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = NativeToElastosStatus(
            lpRecorder->start((android::AudioSystem::sync_event_t)syncEvent, sessionId));
    return NOERROR;
}

ECode CAudioRecord::NativeStop()
{
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    if (lpRecorder == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;;
    }

    lpRecorder->stop();
    //LOGV("Called lpRecorder->stop()");
    return NOERROR;
}

Int32 CAudioRecord::Native_read_in_byte_array(
    /* [in] */ ArrayOf<Byte>* audioData,
    /* [in] */ Int32 offsetInBytes,
    /* [in] */ Int32 sizeInBytes)
{
    Byte* recordBuff = NULL;
    android::AudioRecord *lpRecorder = NULL;

    // get the audio recorder from which we'll read new audio samples
    lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    if (lpRecorder == NULL) {
        //LOGE("Unable to retrieve AudioRecord object, can't record");
        return 0;
    }

    if (!audioData) {
        //LOGE("Invalid Java array to store recorded audio, can't record");
        return 0;
    }

    // get the pointer to where we'll record the audio
    recordBuff = audioData->GetPayload();

    if (recordBuff == NULL) {
        //LOGE("Error retrieving destination for recorded audio data, can't record");
        return 0;
    }

    // read the new audio data from the native AudioRecord object
    ssize_t recorderBuffSize = lpRecorder->frameCount() * lpRecorder->frameSize();
    ssize_t readSize = lpRecorder->read(recordBuff + offsetInBytes,
                            sizeInBytes > (Int32)recorderBuffSize ?
                            (Int32)recorderBuffSize : sizeInBytes );

    if (readSize < 0) {
        readSize = IAudioSystem::INVALID_OPERATION;
    }

    return (Int32)readSize;
}

Int32 CAudioRecord::Native_read_in_short_array(
    /* [in] */ ArrayOf<Int16>* audioData,
    /* [in] */ Int32 offsetInShorts,
    /* [in] */ Int32 sizeInShorts)
{
    Int16* recordBuff = NULL;
    // get the audio recorder from which we'll read new audio samples
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    if (lpRecorder == NULL) {
        ALOGE("Unable to retrieve AudioRecord object, can't record");
        return 0;
    }

    if (!audioData) {
        ALOGE("Invalid Java array to store recorded audio, can't record");
        return 0;
    }

    // get the pointer to where we'll record the audio
    // NOTE: We may use GetPrimitiveArrayCritical() when the JNI implementation changes in such
    // a way that it becomes much more efficient. When doing so, we will have to prevent the
    // AudioSystem callback to be called while in critical section (in case of media server
    // process crash for instance)
    recordBuff = audioData->GetPayload();

    if (recordBuff == NULL) {
        ALOGE("Error retrieving destination for recorded audio data, can't record");
        return 0;
    }

    // read the new audio data from the native AudioRecord object
    const size_t recorderBuffSize = lpRecorder->frameCount()*lpRecorder->frameSize();
    const size_t sizeInBytes = sizeInShorts * sizeof(short);
    ssize_t readSize = lpRecorder->read(recordBuff + offsetInShorts * sizeof(short),
                                        sizeInBytes > recorderBuffSize ?
                                            recorderBuffSize : sizeInBytes);

    if (readSize < 0) {
        readSize = IAudioSystem::INVALID_OPERATION;
    }
    else {
        readSize /= sizeof(short);
    }
    return (Int32)readSize;
}

Int32 CAudioRecord::Native_read_in_direct_buffer(
    /* [in] */ IInterface* buffer,
    /* [in] */ Int32 sizeInBytes)
{
    // get the audio recorder from which we'll read new audio samples
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    if(lpRecorder == NULL) {
        return 0;
    }

    // direct buffer and direct access supported?
    // long capacity = env->GetDirectBufferCapacity(buffer);
    AutoPtr<IByteBuffer> byteBuf = IByteBuffer::Probe(buffer);
    AutoPtr<ArrayOf<Byte> > bytes;
    byteBuf->GetArray((ArrayOf<Byte>**)&bytes);
    Int64 capacity = bytes->GetLength();
    if(capacity == -1) {
        // buffer direct access is not supported
        ALOGE("Buffer direct access is not supported, can't record");
        return 0;
    }
    //LOGV("capacity = %lld", capacity);
    // jbyte* nativeFromJavaBuf = (jbyte*) env->GetDirectBufferAddress(buffer);
    void* nativeFromJavaBuf = bytes->GetPayload();
    if(nativeFromJavaBuf == NULL) {
        ALOGE("Buffer direct access is not supported, can't record");
        return 0;
    }

    // read new data from the recorder
    ssize_t readSize = lpRecorder->read(nativeFromJavaBuf,
                                   capacity < sizeInBytes ? capacity : sizeInBytes);

    if (readSize < 0) {
        readSize = IAudioSystem::INVALID_OPERATION;
    }
    return (Int32)readSize;
}

ECode CAudioRecord::Native_set_marker_pos(
    /* [in] */ Int32 marker,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;

    if (lpRecorder == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioRecord pointer for setMarkerPosition()");
        *result = IAudioSystem::ERROR;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = NativeToElastosStatus(
            lpRecorder->setMarkerPosition(marker));
    return NOERROR;
}

ECode CAudioRecord::Native_get_marker_pos(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    uint32_t markerPos = 0;

    if (lpRecorder == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioRecord pointer for getMarkerPosition()");
        *result = IAudioSystem::ERROR;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    lpRecorder->getMarkerPosition(&markerPos);
    *result = (Int32)markerPos;
    return NOERROR;
}

ECode CAudioRecord::Native_set_pos_update_period(
    /* [in] */ Int32 updatePeriod,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;

    if (lpRecorder == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioRecord pointer for setPositionUpdatePeriod()");
        *result = IAudioSystem::ERROR;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = NativeToElastosStatus(
            lpRecorder->setPositionUpdatePeriod(updatePeriod));
    return NOERROR;
}

ECode CAudioRecord::Native_get_pos_update_period(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    android::AudioRecord *lpRecorder = (android::AudioRecord *)mNativeRecorderInJavaObj;
    uint32_t period = 0;

    if (lpRecorder == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioRecord pointer for getPositionUpdatePeriod()");
        *result = IAudioSystem::ERROR;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    lpRecorder->getPositionUpdatePeriod(&period);
    *result = (Int32)period;
    return NOERROR;
}

/*static*/
Int32 CAudioRecord::Native_get_min_buff_size(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelCount,
    /* [in] */ Int32 audioFormat)
{
    ALOGV(">> android_media_AudioRecord_get_min_buff_size(%d, %d, %d)", sampleRateInHz, channelCount, audioFormat);

    size_t frameCount = 0;
    audio_format_t format = audioFormatToNative(audioFormat);
    audio_channel_mask_t val = audio_channel_in_mask_from_count(channelCount);
    android::status_t result = android::AudioRecord::getMinFrameCount(&frameCount,
            sampleRateInHz,
            format,
            val);

    if (result == android::BAD_VALUE) {
        return 0;
    }
    if (result != android::NO_ERROR) {
        return -1;
    }
    return frameCount * channelCount * audio_bytes_per_sample(format);
}

ECode CAudioRecord::Native_check_permission(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IAppOpsManager> appOpsManager;

    // Get UID here for permission checking
    Int32 clientUid = android::IPCThreadState::self()->getCallingUid();
    return appOpsManager->NoteOp(IAppOpsManager::OP_RECORD_AUDIO, clientUid, packageName, result);
}

//-------------------------------------------------

/*static*/
void CAudioRecord::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

/*static*/
void CAudioRecord::Loge(
    /* [in] */ const String& msg)
{
    Logger::E(TAG, msg);
}

ECode CAudioRecord::IsAudioRecordAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String packageName = CActivityThread::GetCurrentPackageName();
    Int32 checkPermission = 0;
    Native_check_permission(packageName, &checkPermission);
    *result = (checkPermission == IAppOpsManager::MODE_ALLOWED);
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
