#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/media/_AudioErrors.h"
#include "elastos/droid/media/CAudioAttributes.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/CAudioFormat.h"
#include "elastos/droid/media/CAudioFormatBuilder.h"
#include "elastos/droid/media/CAudioTrack.h"
#include "elastos/droid/media/CAudioTimestamp.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"

#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>
#include <system/audio.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::IO::IBuffer;
using Elastos::IO::INioUtils;
using Elastos::IO::CNioUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CAudioTrack::TAG("CAudioTrack");

const Float CAudioTrack::GAIN_MIN = 0.0f;
const Float CAudioTrack::GAIN_MAX = 1.0f;

const Int32 CAudioTrack::SAMPLE_RATE_HZ_MIN = 4000;
const Int32 CAudioTrack::SAMPLE_RATE_HZ_MAX = 96000;
const Int32 CAudioTrack::CHANNEL_COUNT_MAX = 8;

const Int32 CAudioTrack::ERROR_NATIVESETUP_AUDIOSYSTEM         = -16;
const Int32 CAudioTrack::ERROR_NATIVESETUP_INVALIDCHANNELMASK  = -17;
const Int32 CAudioTrack::ERROR_NATIVESETUP_INVALIDFORMAT       = -18;
const Int32 CAudioTrack::ERROR_NATIVESETUP_INVALIDSTREAMTYPE   = -19;
const Int32 CAudioTrack::ERROR_NATIVESETUP_NATIVEINITFAILED    = -20;

const Int32 CAudioTrack::NATIVE_EVENT_MARKER  = 3;
const Int32 CAudioTrack::NATIVE_EVENT_NEW_POS = 4;

const Int32 CAudioTrack::SUPPORTED_OUT_CHANNELS =
    IAudioFormat::CHANNEL_OUT_FRONT_LEFT |
    IAudioFormat::CHANNEL_OUT_FRONT_RIGHT |
    IAudioFormat::CHANNEL_OUT_FRONT_CENTER |
    IAudioFormat::CHANNEL_OUT_LOW_FREQUENCY |
    IAudioFormat::CHANNEL_OUT_BACK_LEFT |
    IAudioFormat::CHANNEL_OUT_BACK_RIGHT |
    IAudioFormat::CHANNEL_OUT_BACK_CENTER |
    IAudioFormat::CHANNEL_OUT_SIDE_LEFT |
    IAudioFormat::CHANNEL_OUT_SIDE_RIGHT;

//===================================================================================
//              CAudioTrack::EventHandler
//===================================================================================

ECode CAudioTrack::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mAudioTrack == NULL) {
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
    case CAudioTrack::NATIVE_EVENT_MARKER:
        if (mListener != NULL) {
            mListener->OnMarkerReached(
                IAudioTrack::Probe(mAudioTrack));
        }
        break;
    case CAudioTrack::NATIVE_EVENT_NEW_POS:
        if (mListener != NULL) {
            mListener->OnPeriodicNotification(
                IAudioTrack::Probe(mAudioTrack));
        }
        break;
    default:
        Logger::E(CAudioTrack::TAG, "Unknown event type: %d", what);
        break;
    }
    return NOERROR;
}

//===================================================================================
//              CAudioTrack::NativeEventHandlerDelegate
//===================================================================================

CAudioTrack::NativeEventHandlerDelegate::NativeEventHandlerDelegate(
    /* [in] */ CAudioTrack* track,
    /* [in] */ IAudioTrackOnPlaybackPositionUpdateListener* listener,
    /* [in] */ IHandler* handler)
    : mAudioTrack(track)
{
    // find the looper for our new event handler
    AutoPtr<ILooper> looper;
    if (handler != NULL) {
        handler->GetLooper((ILooper**)&looper);
    }
    else {
        // no given handler, use the looper the AudioTrack was created in
        looper = mAudioTrack->mInitializationLooper;
    }

    // construct the event handler with this looper
    if (looper != NULL) {
        // implement the event handler delegate
        mHandler = new EventHandler(looper, mAudioTrack, listener);
    }
    else {
        mHandler = NULL;
    }
}

AutoPtr<IHandler> CAudioTrack::NativeEventHandlerDelegate::GetHandler()
{
    return mHandler;
}

//===================================================================================
//              CAudioTrack
//===================================================================================

CAR_INTERFACE_IMPL(CAudioTrack, Object, IAudioTrack)

CAR_OBJECT_IMPL(CAudioTrack)

CAudioTrack::CAudioTrack()
    : mState(STATE_UNINITIALIZED)
    , mPlayState(PLAYSTATE_STOPPED)
    , mNativeBufferSizeInBytes(0)
    , mNativeBufferSizeInFrames(0)
    , mSampleRate(22050)
    , mChannelCount(1)
    , mChannels(IAudioFormat::CHANNEL_OUT_MONO)
    , mStreamType(IAudioManager::STREAM_MUSIC)
    , mDataLoadMode(MODE_STREAM)
    , mChannelConfiguration(IAudioFormat::CHANNEL_OUT_MONO)
    , mAudioFormat(IAudioFormat::ENCODING_PCM_16BIT)
    , mSessionId(IAudioSystem::AUDIO_SESSION_ALLOCATE)
    , mNativeTrack(0)
    , mNativeData(0)
{}

CAudioTrack::~CAudioTrack()
{
    NativeFinalize();
}

ECode CAudioTrack::constructor(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 bufferSizeInBytes,
    /* [in] */ Int32 mode)
{
    return constructor(streamType, sampleRateInHz, channelConfig,
        audioFormat, bufferSizeInBytes, mode, IAudioSystem::AUDIO_SESSION_ALLOCATE);
}

ECode CAudioTrack::constructor(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 bufferSizeInBytes,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 sessionId)
{
    // mState already == STATE_UNINITIALIZED
    AutoPtr<IAudioAttributesBuilder> aaBuilder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&aaBuilder);
    aaBuilder->SetLegacyStreamType(streamType);
    AutoPtr<IAudioAttributes> aa;
    aaBuilder->Build((IAudioAttributes**)&aa);

    AutoPtr<IAudioFormatBuilder> afBuilder;
    CAudioFormatBuilder::New((IAudioFormatBuilder**)&afBuilder);
    afBuilder->SetChannelMask(channelConfig);
    afBuilder->SetEncoding(audioFormat);
    afBuilder->SetSampleRate(sampleRateInHz);
    AutoPtr<IAudioFormat> af;
    afBuilder->Build((IAudioFormat**)&af);

    return constructor(aa, af, bufferSizeInBytes, mode, sessionId);
}

ECode CAudioTrack::constructor(
    /* [in] */ IAudioAttributes* attributes,
    /* [in] */ IAudioFormat* format,
    /* [in] */ Int32 bufferSizeInBytes,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 sessionId)
{
    // mState already == STATE_UNINITIALIZED

    if (attributes == NULL) {
        // throw new IllegalArgumentException("Illegal NULL AudioAttributes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (format == NULL) {
        // throw new IllegalArgumentException("Illegal NULL AudioFormat");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // remember which looper is associated with the AudioTrack instantiation
    AutoPtr<ILooper> looper;
    if ((looper = Looper::GetMyLooper()) == NULL) {
        looper = Looper::GetMainLooper();
    }

    Int32 mask;
    format->GetPropertySetMask(&mask);
    Int32 rate = 0;
    if ((mask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_SAMPLE_RATE) != 0)
    {
        format->GetSampleRate(&rate);
    } else {
         AudioSystem::GetPrimaryOutputSamplingRate(&rate);
        if (rate <= 0) {
            rate = 44100;
        }
    }
    Int32 channelMask = IAudioFormat::CHANNEL_OUT_FRONT_LEFT | IAudioFormat::CHANNEL_OUT_FRONT_RIGHT;
    if ((mask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_CHANNEL_MASK) != 0)
    {
        format->GetChannelMask(&channelMask);
    }
    Int32 encoding = IAudioFormat::ENCODING_DEFAULT;
    if ((mask & IAudioFormat::AUDIO_FORMAT_HAS_PROPERTY_ENCODING) != 0) {
        format->GetEncoding(&encoding);
    }

    FAIL_RETURN(AudioParamCheck(rate, channelMask, encoding, mode));
    mStreamType = IAudioSystem::STREAM_DEFAULT;

    FAIL_RETURN(AudioBuffSizeCheck(bufferSizeInBytes));

    mInitializationLooper = looper;
    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::APP_OPS_SERVICE);
    assert(b != NULL);
    mAppOps = IIAppOpsService::Probe(b);

    AutoPtr<IAudioAttributesBuilder> aaBuilder;
    CAudioAttributesBuilder::New(attributes, (IAudioAttributesBuilder**)&aaBuilder);
    aaBuilder->Build((IAudioAttributes**)&mAttributes);

    if (sessionId < 0) {
        Logger::E(TAG, "Invalid audio session ID: %d", sessionId);
        //throw (new IllegalArgumentException("Invalid audio session ID: "+sessionId));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 session[] = { sessionId };
    // native initialization
    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    Int32 initResult = NativeSetup(weakHost, mAttributes, mSampleRate, mChannels,
            mAudioFormat, mNativeBufferSizeInBytes, mDataLoadMode, session);
    if (initResult != IAudioTrack::SUCCESS) {
        Logger::E(TAG, "Error code %d  when initializing AudioTrack.", initResult);
        //loge("Error code "+initResult+" when initializing AudioTrack.");
        return NOERROR; // with mState == STATE_UNINITIALIZED
    }

    mSessionId = session[0];

    if (mDataLoadMode == MODE_STATIC) {
        mState = STATE_NO_STATIC_DATA;
    }
    else {
        mState = STATE_INITIALIZED;
    }
    return NOERROR;
}

ECode CAudioTrack::AudioParamCheck(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 mode)
{
    //--------------
    // sample rate, note these values are subject to change
    if (sampleRateInHz < SAMPLE_RATE_HZ_MIN || sampleRateInHz > SAMPLE_RATE_HZ_MAX) {
        // throw new IllegalArgumentException(sampleRateInHz
        //         + "Hz is not a supported sample rate.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSampleRate = sampleRateInHz;

    //--------------
    // sample rate, note these values are subject to change
    if ( (sampleRateInHz < 4000) || (sampleRateInHz > 48000) ) {
        Logger::E(TAG, "%d Hz is not a supported sample rate.", sampleRateInHz);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        mSampleRate = sampleRateInHz;
    }

    //--------------
    // channel config
    mChannelConfiguration = channelConfig;

    switch (channelConfig) {
    case IAudioFormat::CHANNEL_OUT_DEFAULT: //AudioFormat.CHANNEL_CONFIGURATION_DEFAULT
    case IAudioFormat::CHANNEL_OUT_MONO:
    case IAudioFormat::CHANNEL_CONFIGURATION_MONO:
        mChannelCount = 1;
        mChannels = IAudioFormat::CHANNEL_OUT_MONO;
        break;
    case IAudioFormat::CHANNEL_OUT_STEREO:
    case IAudioFormat::CHANNEL_CONFIGURATION_STEREO:
        mChannelCount = 2;
        mChannels = IAudioFormat::CHANNEL_OUT_STEREO;
        break;
    default:
        if (!IsMultichannelConfigSupported(channelConfig)) {
            // input channel configuration features unsupported channels
            //throw(new IllegalArgumentException("Unsupported channel configuration."));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mChannels = channelConfig;
        mChannelCount = Elastos::Core::Math::BitCount(channelConfig);
    }

    //--------------
    // audio format
    if (audioFormat == IAudioFormat::ENCODING_DEFAULT) {
        audioFormat = IAudioFormat::ENCODING_PCM_16BIT;
    }

    Boolean b;
    if (CAudioFormat::IsValidEncoding(audioFormat, &b), !b) {
        // throw new IllegalArgumentException("Unsupported audio encoding.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAudioFormat = audioFormat;

    //--------------
    // audio load mode
    CAudioFormat::IsEncodingLinearPcm(mAudioFormat, &b);
    if (((mode != MODE_STREAM) && (mode != MODE_STATIC)) ||
            ((mode != MODE_STREAM) && !b)) {
        // throw new IllegalArgumentException("Invalid mode.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDataLoadMode = mode;
    return NOERROR;
}

Boolean CAudioTrack::IsMultichannelConfigSupported(
    /* [in] */ Int32 channelConfig)
{
    // check for unsupported channels
    if ((channelConfig & SUPPORTED_OUT_CHANNELS) != channelConfig) {
        Logger::E(TAG, "Channel configuration features unsupported channels");
        return FALSE;
    }
    Int32 channelCount = Elastos::Core::Math::BitCount(channelConfig);
    if (channelCount > CHANNEL_COUNT_MAX) {
        Logger::E(TAG, "Channel configuration contains too many channels %d > %d",
                channelCount, CHANNEL_COUNT_MAX);
        return FALSE;
    }

    // check for unsupported multichannel combinations:
    // - FL/FR must be present
    // - L/R channels must be paired (e.g. no single L channel)
    const Int32 frontPair = IAudioFormat::CHANNEL_OUT_FRONT_LEFT
        | IAudioFormat::CHANNEL_OUT_FRONT_RIGHT;
    if ((channelConfig & frontPair) != frontPair) {
        Logger::E(TAG, "Front channels must be present in multichannel configurations");
        return FALSE;
    }
    const Int32 backPair = IAudioFormat::CHANNEL_OUT_BACK_LEFT
        | IAudioFormat::CHANNEL_OUT_BACK_RIGHT;
    if ((channelConfig & backPair) != 0) {
        if ((channelConfig & backPair) != backPair) {
            Logger::E(TAG, "Rear channels can't be used independently");
            return FALSE;
        }
    }
    Int32 sidePair =
            IAudioFormat::CHANNEL_OUT_SIDE_LEFT | IAudioFormat::CHANNEL_OUT_SIDE_RIGHT;
    if ((channelConfig & sidePair) != 0
            && (channelConfig & sidePair) != sidePair) {
        Logger::E(TAG, "Side channels can't be used independently");
        return FALSE;
    }
    return TRUE;
}

ECode CAudioTrack::AudioBuffSizeCheck(
    /* [in] */ Int32 audioBufferSize)
{
    // NB: this section is only valid with PCM data.
    //     To update when supporting compressed formats
    Int32 frameSizeInBytes;
    Boolean b;
    if (CAudioFormat::IsEncodingLinearPcm(mAudioFormat, &b), b) {
        Int32 val;
        CAudioFormat::GetBytesPerSample(mAudioFormat, &val);
        frameSizeInBytes = mChannelCount * val;
    }
    else {
        frameSizeInBytes = 1;
    }
    if ((audioBufferSize % frameSizeInBytes != 0) || (audioBufferSize < 1)) {
        Logger::E(TAG, "Invalid audio buffer size.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mNativeBufferSizeInBytes = audioBufferSize;
    mNativeBufferSizeInFrames = audioBufferSize / frameSizeInBytes;
    return NOERROR;
}

ECode CAudioTrack::ReleaseResources()
{
    // even though native_release() stops the native AudioTrack, we need to stop
    // AudioTrack subclasses too.
    // try {
    Stop();
    // } catch(IllegalStateException ise) {
    //     // don't raise an exception, we're releasing the resources.
    // }

    NativeRelease();
    mState = STATE_UNINITIALIZED;
    return NOERROR;
}

//--------------------------------------------------------------------------
// Getters
//--------------------

ECode CAudioTrack::GetMinVolume(
    /* [out] */ Float* minVolume)
{
    VALIDATE_NOT_NULL(minVolume);
    *minVolume = GAIN_MIN;
    return NOERROR;
}

ECode CAudioTrack::GetMaxVolume(
    /* [out] */ Float* maxVolume)
{
    VALIDATE_NOT_NULL(maxVolume);
    *maxVolume = GAIN_MAX;
    return NOERROR;
}

ECode CAudioTrack::GetSampleRate(
    /* [out] */ Int32* rate)
{
    VALIDATE_NOT_NULL(rate);
    *rate = mSampleRate;
    return NOERROR;
}

ECode CAudioTrack::GetPlaybackRate(
    /* [out] */ Int32* rate)
{
    assert(mNativeTrack != 0);
    VALIDATE_NOT_NULL(rate);
    *rate = NativeGetPlaybackRate();
    return NOERROR;
}

ECode CAudioTrack::GetAudioFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);
    *format = mAudioFormat;
    return NOERROR;
}

ECode CAudioTrack::GetStreamType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mStreamType;
    return NOERROR;
}

ECode CAudioTrack::GetChannelConfiguration(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChannelConfiguration;
    return NOERROR;
}

ECode CAudioTrack::GetChannelCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mChannelCount;
    return NOERROR;
}

ECode CAudioTrack::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CAudioTrack::GetPlayState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    {    AutoLock syncLock(mPlayStateLock);
        *state = mPlayState;
    }
    return NOERROR;
}

Int32 CAudioTrack::GetNativeFrameCount()
{
    assert(mNativeTrack != 0);
    return NativeGetNativeFrameCount();
}

ECode CAudioTrack::GetNotificationMarkerPosition(
    /* [out] */ Int32* pos)
{
    assert(mNativeTrack != 0);
    VALIDATE_NOT_NULL(pos);
    *pos = NativeGetMarkerPos();
    return NOERROR;
}

ECode CAudioTrack::GetPositionNotificationPeriod(
    /* [out] */ Int32* period)
{
    assert(mNativeTrack != 0);
    VALIDATE_NOT_NULL(period);
    *period = NativeGetPosUpdatePeriod();
    return NOERROR;
}

ECode CAudioTrack::GetPlaybackHeadPosition(
    /* [out] */ Int32* position)
{
    assert(mNativeTrack != 0);
    VALIDATE_NOT_NULL(position);
    *position = NativeGetPosition();
    return NOERROR;
}

ECode CAudioTrack::GetLatency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NativeGetLatency();
    return NOERROR;
}

ECode CAudioTrack::GetNativeOutputSampleRate(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* rate)
{
    VALIDATE_NOT_NULL(rate);
    *rate = NativeGetOutputSampleRate(streamType);
    return NOERROR;
}

ECode CAudioTrack::GetMinBufferSize(
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    Int32 channelCount = 0;
    switch(channelConfig) {
    case IAudioFormat::CHANNEL_OUT_MONO:
    case IAudioFormat::CHANNEL_CONFIGURATION_MONO:
        channelCount = 1;
        break;
    case IAudioFormat::CHANNEL_OUT_STEREO:
    case IAudioFormat::CHANNEL_CONFIGURATION_STEREO:
        channelCount = 2;
        break;
    default:
        if ((channelConfig & SUPPORTED_OUT_CHANNELS) != channelConfig) {
        // input channel configuration features unsupported channels
        Logger::E(TAG, "getMinBufferSize(): Invalid channel configuration.");
        *size = IAudioTrack::ERROR_BAD_VALUE;
        return NOERROR;
        }
        else {
            channelCount = Elastos::Core::Math::BitCount(channelConfig);
        }
     }

    Boolean b;
    if (CAudioFormat::IsValidEncoding(audioFormat, &b), !b) {
        Logger::E(TAG, "getMinBufferSize(): Invalid audio format.");
        *size = IAudioTrack::ERROR_BAD_VALUE;
        return NOERROR;
    }

    if ( (sampleRateInHz < SAMPLE_RATE_HZ_MIN) || (sampleRateInHz > SAMPLE_RATE_HZ_MAX) ) {
        Logger::E(TAG, "getMinBufferSize(): %d Hz is not a supported sample rate.", sampleRateInHz);
        *size = IAudioTrack::ERROR_BAD_VALUE;
        return NOERROR;
    }

    Int32 bufferSize = NativeGetMinBuffSize(sampleRateInHz, channelCount, audioFormat);
    if (bufferSize <= 0) {
        Logger::E(TAG, "getMinBufferSize(): error querying hardware");
        *size = IAudioTrack::ERROR;
        return NOERROR;
    }
    else {
        *size = bufferSize;
    }
    return NOERROR;
}

ECode CAudioTrack::GetAudioSessionId(
    /* [out] */ Int32 * id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSessionId;
    return NOERROR;
}

ECode CAudioTrack::GetTimestamp(
    /* [in] */ IAudioTimestamp* timestamp,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (timestamp == NULL) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // It's unfortunate, but we have to either create garbage every time or use synchronized
    AutoPtr<ArrayOf<Int64> > longArray = ArrayOf<Int64>::Alloc(2);
    Int32 ret = NativeGetTimestamp(longArray);
    if (ret != IAudioTrack::SUCCESS) {
        *result = FALSE;
        return NOERROR;
    }
    ((CAudioTimestamp*)timestamp)->mFramePosition = (*longArray)[0];
    ((CAudioTimestamp*)timestamp)->mNanoTime = (*longArray)[1];
    *result = TRUE;
    return NOERROR;
}

//--------------------------------------------------------------------------
// Initialization / configuration
//--------------------
ECode CAudioTrack::SetPlaybackPositionUpdateListener(
    /* [in] */ IAudioTrackOnPlaybackPositionUpdateListener *listener)
{
    return SetPlaybackPositionUpdateListener(listener, NULL);
}

ECode CAudioTrack::SetPlaybackPositionUpdateListener(
    /* [in] */ IAudioTrackOnPlaybackPositionUpdateListener *listener,
    /* [in] */ IHandler *handler)
{
    if (listener != NULL) {
        mEventHandlerDelegate = new NativeEventHandlerDelegate(this, listener, handler);
    }
    else {
        mEventHandlerDelegate = NULL;
    }
    return NOERROR;
}

ECode CAudioTrack::ClampGainOrLevel(
    /* [in] */ Float gainOrLevel,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (Elastos::Core::Math::IsNaN(gainOrLevel)) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (gainOrLevel < GAIN_MIN) {
        gainOrLevel = GAIN_MIN;
    } else if (gainOrLevel > GAIN_MAX) {
        gainOrLevel = GAIN_MAX;
    }
    *result = gainOrLevel;
    return NOERROR;
}

ECode CAudioTrack::SetStereoVolume(
    /* [in] */ Float leftGain,
    /* [in] */ Float rightGain,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (IsRestricted()) {
        *result = IAudioTrack::SUCCESS;
        return NOERROR;
    }
    if (mState == STATE_UNINITIALIZED) {
        *result = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    ClampGainOrLevel(leftGain, &leftGain);
    ClampGainOrLevel(rightGain, &rightGain);

    NativeSetVolume(leftGain, rightGain);

    *result = IAudioTrack::SUCCESS;
    return NOERROR;
}

ECode CAudioTrack::SetVolume(
    /* [in] */ Float gain,
    /* [out] */ Int32* result)
{
    return SetStereoVolume(gain, gain, result);
}

ECode CAudioTrack::SetPlaybackRate(
    /* [in] */ Int32 sampleRateInHz,
    /* [out] */ Int32* rate)
{
    VALIDATE_NOT_NULL(rate);
    if (mState != STATE_INITIALIZED) {
        *rate = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    if (sampleRateInHz <= 0) {
        *rate = IAudioTrack::ERROR_BAD_VALUE;
        return NOERROR;
    }
    *rate = NativeSetPlaybackRate(sampleRateInHz);
    return NOERROR;
}

ECode CAudioTrack::SetNotificationMarkerPosition(
    /* [in] */ Int32 markerInFrames,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState == STATE_UNINITIALIZED) {
        *result = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    *result = NativeSetMarkerPos(markerInFrames);
    return NOERROR;
}

ECode CAudioTrack::SetPositionNotificationPeriod(
    /* [in] */ Int32 periodInFrames,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState == STATE_UNINITIALIZED) {
        *result = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    *result = NativeSetPosUpdatePeriod(periodInFrames);
    return NOERROR;
}

ECode CAudioTrack::SetPlaybackHeadPosition(
    /* [in] */ Int32 positionInFrames,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 state;
    GetPlayState(&state);
    if (mDataLoadMode == MODE_STREAM || mState != STATE_INITIALIZED ||
            state == PLAYSTATE_PLAYING) {
        *result = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    if (!(0 <= positionInFrames && positionInFrames <= mNativeBufferSizeInFrames)) {
        *result = IAudioTrack::ERROR_BAD_VALUE;
        return NOERROR;
    }
    *result = NativeSetPosition(positionInFrames);
    return NOERROR;
}

ECode CAudioTrack::SetLoopPoints(
    /* [in] */ Int32 startInFrames,
    /* [in] */ Int32 endInFrames,
    /* [in] */ Int32 loopCount,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 state;
    GetPlayState(&state);
    if (mDataLoadMode == MODE_STREAM || mState != STATE_INITIALIZED ||
            state == PLAYSTATE_PLAYING) {
        *result = ERROR_INVALID_OPERATION;
    }
    if (loopCount == 0) {
        ;   // explicitly allowed as an exception to the loop region range check
    } else if (!(0 <= startInFrames && startInFrames < mNativeBufferSizeInFrames &&
            startInFrames < endInFrames && endInFrames <= mNativeBufferSizeInFrames)) {
        *result = ERROR_BAD_VALUE;
    }
    *result = NativeSetLoop(startInFrames, endInFrames, loopCount);
    return NOERROR;
}

/**
 * Sets the initialization state of the instance. To be used in an AudioTrack subclass
 * constructor to set a subclass-specific post-initialization state.
 * @param state the state of the AudioTrack instance
 */
void CAudioTrack::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
}

//---------------------------------------------------------
// Transport control methods
//--------------------
ECode CAudioTrack::Play()
{
    if (mState != STATE_INITIALIZED) {
        Logger::E(TAG, "play() called on uninitialized AudioTrack.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (IsRestricted()) {
        Int32 result;
        SetVolume(0, &result);
    }
    {    AutoLock syncLock(mPlayStateLock);
        NativeStart();
        mPlayState = PLAYSTATE_PLAYING;
    }
    return NOERROR;
}

Boolean CAudioTrack::IsRestricted()
{
    // try {
    Int32 usage;
    CAudioAttributes::UsageForLegacyStreamType(mStreamType, &usage);
    String str = CActivityThread::GetCurrentPackageName();
    Int32 mode;
    mAppOps->CheckAudioOperation(IAppOpsManager::OP_PLAY_AUDIO, usage,
            Process::MyUid(), str, &mode);
    return mode != IAppOpsManager::MODE_ALLOWED;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CAudioTrack::Stop()
{
    if (mState != STATE_INITIALIZED) {
        Logger::E(TAG, "stop() called on uninitialized AudioTrack.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // stop playing
    {    AutoLock syncLock(mPlayStateLock);
        NativeStop();
        mPlayState = PLAYSTATE_STOPPED;
    }
    return NOERROR;
}

ECode CAudioTrack::Pause()
{
    if (mState != STATE_INITIALIZED) {
        Logger::E(TAG, "pause() called on uninitialized AudioTrack.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //logd("pause()");

    // pause playback
    {    AutoLock syncLock(mPlayStateLock);
        NativePause();
        mPlayState = PLAYSTATE_PAUSED;
    }
    return NOERROR;
}

//---------------------------------------------------------
// Audio data supply
//--------------------
ECode CAudioTrack::Flush()
{
    if (mState == STATE_INITIALIZED) {
         // flush the data in native layer
         NativeFlush();
     }
    return NOERROR;
}

ECode CAudioTrack::Write(
    /* [in] */ ArrayOf<Byte>* audioData,
    /* [in] */ Int32 offsetInBytes,
    /* [in] */ Int32 sizeInBytes,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mState == STATE_UNINITIALIZED || mAudioFormat == IAudioFormat::ENCODING_PCM_FLOAT) {
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ( (audioData == NULL) || (offsetInBytes < 0 ) || (sizeInBytes < 0)
            || (offsetInBytes + sizeInBytes < 0)    // detect integer overflow
            || (offsetInBytes + sizeInBytes > audioData->GetLength())) {
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    Int32 ret = NativeWriteByte(audioData, offsetInBytes, sizeInBytes, mAudioFormat,
            TRUE /*isBlocking*/);

    if ((mDataLoadMode == MODE_STATIC)
            && (mState == STATE_NO_STATIC_DATA)
            && (ret > 0)) {
        // benign race with respect to other APIs that read mState
        mState = STATE_INITIALIZED;
    }

    *result = ret;
    return NOERROR;
}

ECode CAudioTrack::Write(
    /* [in] */ ArrayOf<Int16>* audioData,
    /* [in] */ Int32 offsetInShorts,
    /* [in] */ Int32 sizeInShorts,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState == STATE_UNINITIALIZED || mAudioFormat == IAudioFormat::ENCODING_PCM_FLOAT) {
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ( (audioData == NULL) || (offsetInShorts < 0 ) || (sizeInShorts < 0)
            || (offsetInShorts + sizeInShorts < 0)  // detect integer overflow
            || (offsetInShorts + sizeInShorts > audioData->GetLength())) {
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    Int32 ret = NativeWriteInt16(audioData, offsetInShorts, sizeInShorts, mAudioFormat);

    if ((mDataLoadMode == MODE_STATIC)
            && (mState == STATE_NO_STATIC_DATA)
            && (ret > 0)) {
        // benign race with respect to other APIs that read mState
        mState = STATE_INITIALIZED;
    }

    *result = ret;
    return NOERROR;
}

ECode CAudioTrack::Write(
    /* [in] */ ArrayOf<Float>* audioData,
    /* [in] */ Int32 offsetInFloats,
    /* [in] */ Int32 sizeInFloats,
    /* [in] */ Int32 writeMode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (mState == STATE_UNINITIALIZED) {
        Logger::E(TAG, "AudioTrack.write() called in invalid state STATE_UNINITIALIZED");
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if (mAudioFormat != IAudioFormat::ENCODING_PCM_FLOAT) {
        Logger::E(TAG, "AudioTrack.write(float[] ...) requires format ENCODING_PCM_FLOAT");
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ((writeMode != WRITE_BLOCKING) && (writeMode != WRITE_NON_BLOCKING)) {
        Logger::E(TAG, "AudioTrack.write() called with invalid blocking mode");
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    if ( (audioData == NULL) || (offsetInFloats < 0 ) || (sizeInFloats < 0)
            || (offsetInFloats + sizeInFloats < 0)  // detect integer overflow
            || (offsetInFloats + sizeInFloats > audioData->GetLength())) {
        Logger::E(TAG, "AudioTrack.write() called with invalid array, offset, or size");
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    Int32 ret = NativeWriteFloat(audioData, offsetInFloats, sizeInFloats, mAudioFormat,
            writeMode == WRITE_BLOCKING);

    if ((mDataLoadMode == MODE_STATIC)
            && (mState == STATE_NO_STATIC_DATA)
            && (ret > 0)) {
        // benign race with respect to other APIs that read mState
        mState = STATE_INITIALIZED;
    }

    *result = ret;
    return NOERROR;
}

ECode CAudioTrack::Write(
    /* [in] */ IByteBuffer* audioData,
    /* [in] */ Int32 sizeInBytes,
    /* [in] */ Int32 writeMode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (mState == STATE_UNINITIALIZED) {
        Logger::E(TAG, "AudioTrack.write() called in invalid state STATE_UNINITIALIZED");
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if ((writeMode != WRITE_BLOCKING) && (writeMode != WRITE_NON_BLOCKING)) {
        Logger::E(TAG, "AudioTrack.write() called with invalid blocking mode");
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    Int32 remaining;
    IBuffer::Probe(audioData)->GetRemaining(&remaining);
    if ( (audioData == NULL) || (sizeInBytes < 0) || (sizeInBytes > remaining)) {
        Logger::E(TAG, "AudioTrack.write() called with invalid size (%d) value", sizeInBytes);
        *result = ERROR_BAD_VALUE;
        return NOERROR;
    }

    Boolean b;
    IBuffer::Probe(audioData)->IsDirect(&b);
    Int32 position;
    IBuffer::Probe(audioData)->GetPosition(&position);
    Int32 ret = 0;
    if (b) {
        ret = NativeWriteNativeBytes(audioData,
                position, sizeInBytes, mAudioFormat,
                writeMode == WRITE_BLOCKING);
    }
    else {
        AutoPtr<INioUtils> helper;
        CNioUtils::AcquireSingleton((INioUtils**)&helper);
        AutoPtr<ArrayOf<Byte> > array;
        helper->GetUnsafeArray(audioData, (ArrayOf<Byte>**)&array);
        Int32 val;
        helper->GetUnsafeArrayOffset(audioData, &val);
        ret = NativeWriteByte(array,
                val + position,
                sizeInBytes, mAudioFormat,
                writeMode == WRITE_BLOCKING);
    }

    if ((mDataLoadMode == MODE_STATIC)
            && (mState == STATE_NO_STATIC_DATA)
            && (ret > 0)) {
        // benign race with respect to other APIs that read mState
        mState = STATE_INITIALIZED;
    }

    if (ret > 0) {
        IBuffer::Probe(audioData)->SetPosition(position + ret);
    }

    *result = ret;
    return NOERROR;
}

ECode CAudioTrack::ReloadStaticData(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mDataLoadMode == MODE_STREAM || mState != STATE_INITIALIZED) {
        *result = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    *result = NativeReload();
    return NOERROR;
}

//--------------------------------------------------------------------------
// Audio effects management
//--------------------
ECode CAudioTrack::AttachAuxEffect(
    /* [in] */ Int32 effectId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState == STATE_UNINITIALIZED) {
        *result = IAudioTrack::ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    *result = NativeAttachAuxEffect(effectId);
    return NOERROR;
}

ECode CAudioTrack::SetAuxEffectSendLevel(
    /* [in] */ Float level,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (IsRestricted()) {
        *result = SUCCESS;
        return NOERROR;
    }
    if (mState == STATE_UNINITIALIZED) {
        *result = ERROR_INVALID_OPERATION;
        return NOERROR;
    }
    Float val;
    ClampGainOrLevel(level, &val);
    Int32 err = NativeSetAuxEffectSendLevel(val);
    *result = err == 0 ? SUCCESS : ERROR;
    return NOERROR;
}

ECode CAudioTrack::PostEventFromNative(
    /* [in] */ IInterface* ref,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    //logd("Event posted from the native side: event="+ what + " args="+ arg1+" "+arg2);
    AutoPtr<IWeakReference> wr = IWeakReference::Probe(ref);
    AutoPtr<IAudioTrack> iat;
    wr->Resolve(EIID_IAudioTrack, (IInterface**)&iat);
    if (iat == NULL) {
        return NOERROR;
    }

    AutoPtr<CAudioTrack> track = (CAudioTrack*)iat.Get();

    AutoPtr<NativeEventHandlerDelegate> delegate = track->mEventHandlerDelegate;
    if (delegate != NULL) {
        AutoPtr<IHandler> handler = delegate->GetHandler();
        if (handler != NULL) {
            AutoPtr<IMessage> m;
            handler->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&m);
            Boolean b;
            handler->SendMessage(m, &b);
        }
    }
    return NOERROR;
}

// ----------------------------------------------------------------------------
#define JNI_TRUE    1

#define DEFAULT_OUTPUT_SAMPLE_RATE   44100

#define AUDIOTRACK_ERROR_SETUP_AUDIOSYSTEM         -16
#define AUDIOTRACK_ERROR_SETUP_INVALIDCHANNELMASK  -17
#define AUDIOTRACK_ERROR_SETUP_INVALIDFORMAT       -18
#define AUDIOTRACK_ERROR_SETUP_INVALIDSTREAMTYPE   -19
#define AUDIOTRACK_ERROR_SETUP_NATIVEINITFAILED    -20

// static Mutex sLock;
// static SortedVector <audiotrack_callback_cookie *> sAudioTrackCallBackCookies;

struct audiotrack_callback_cookie
{
    AutoPtr<IWeakReference> mAudioTrackRef;
};

class AudioTrackJniStorage
{
public:
    AudioTrackJniStorage()
    {
        mCallbackData.mAudioTrackRef = NULL;
        mStreamType = AUDIO_STREAM_DEFAULT;
    }

    ~AudioTrackJniStorage()
    {
        mMemBase.clear();
        mMemHeap.clear();
    }

    CARAPI_(Boolean) AllocSharedMem(
        /* [in] */ Int32 sizeInBytes)
    {
        mMemHeap = new android::MemoryHeapBase(sizeInBytes, 0, "AudioTrack Heap Base");
        if (mMemHeap->getHeapID() < 0) {
            return FALSE;
        }
        mMemBase = new android::MemoryBase(mMemHeap, 0, sizeInBytes);
        return TRUE;
    }

public:
    android::sp<android::MemoryHeapBase> mMemHeap;
    android::sp<android::MemoryBase> mMemBase;
    struct audiotrack_callback_cookie mCallbackData;
    audio_stream_type_t mStreamType;
};

static void audioCallback(int event, void* user, void *info)
{
    if (event == android::AudioTrack::EVENT_MORE_DATA) {
        // set size to 0 to signal we're not using the callback to write more data
        android::AudioTrack::Buffer* pBuff = (android::AudioTrack::Buffer*)info;
        pBuff->size = 0;

    }
    else if (event == android::AudioTrack::EVENT_MARKER) {
        audiotrack_callback_cookie* callbackInfo = (audiotrack_callback_cookie *)user;
        if (user) {
            CAudioTrack::PostEventFromNative(
                callbackInfo->mAudioTrackRef, event, 0, 0, NULL);
        }
    }
    else if (event == android::AudioTrack::EVENT_NEW_POS) {
        audiotrack_callback_cookie* callbackInfo = (audiotrack_callback_cookie *)user;
        if (user) {
            CAudioTrack::PostEventFromNative(
                callbackInfo->mAudioTrackRef, event, 0, 0, NULL);
        }
    }
}

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

template<bool readOnly>
class ScopedBytes {
public:
    ScopedBytes(IInterface* object)
    : mObject(object), mByteArray(NULL), mPtr(NULL)
    {
        if (mObject == NULL) {
            // jniThrowNullPointerException(mEnv, NULL);
            return;
        }
        else if (IByteBuffer::Probe(mObject)) {
            IByteBuffer::Probe(mObject)->GetArray((ArrayOf<Byte>**)&mByteArray);
            mPtr = mByteArray->GetPayload();
        }
        else {
            Handle64 addr;
            IBuffer::Probe(mObject)->GetEffectiveDirectAddress(&addr);
            mPtr = (Byte*)addr;
        }
    }

    ~ScopedBytes() {
    }

private:
    AutoPtr<IInterface> mObject;
    AutoPtr<ArrayOf<Byte> > mByteArray;

protected:
    Byte* mPtr;

private:
    // Disallow copy and assignment.
    ScopedBytes(const ScopedBytes&);
    void operator=(const ScopedBytes&);
};

class ScopedBytesRO : public ScopedBytes<true> {
public:
    ScopedBytesRO(IInterface* object) : ScopedBytes<true>(object) {}
    Byte* get() const {
        return mPtr;
    }
};

Int32 CAudioTrack::NativeSetup(
    /* [in] */ IWeakReference* weak_this,
    /* [in] */ IAudioAttributes* attributes,
    /* [in] */ Int32 sampleRateInHertz,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 buffSizeInBytes,
    /* [in] */ Int32 memoryMode,
    /* [in] */ Int32* jSession)
{
    ALOGV("sampleRate=%d, audioFormat(from Java)=%d, channel mask=%x, buffSize=%d",
        sampleRateInHertz, audioFormat, channelMask, buffSizeInBytes);

    if (attributes == 0) {
        ALOGE("Error creating AudioTrack: invalid audio attributes");
        return IAudioSystem::ERROR;
    }

    // Java channel masks don't map directly to the native definition, but it's a simple shift
    // to skip the two deprecated channel configurations "default" and "mono".
    audio_channel_mask_t nativeChannelMask = ((uint32_t)channelMask) >> 2;

    if (!audio_is_output_channel(nativeChannelMask)) {
        ALOGE("Error creating AudioTrack: invalid channel mask %#x.", channelMask);
        return AUDIOTRACK_ERROR_SETUP_INVALIDCHANNELMASK;
    }

    uint32_t channelCount = audio_channel_count_from_out_mask(nativeChannelMask);

    // check the format.
    // This function was called from Java, so we compare the format against the Java constants
    audio_format_t format = audioFormatToNative(audioFormat);
    if (format == AUDIO_FORMAT_INVALID) {
        ALOGE("Error creating AudioTrack: unsupported audio format %d.", audioFormat);
        return AUDIOTRACK_ERROR_SETUP_INVALIDFORMAT;
    }

    // for the moment 8bitPCM in MODE_STATIC is not supported natively in the AudioTrack C++ class
    // so we declare everything as 16bitPCM, the 8->16bit conversion for MODE_STATIC will be handled
    // in android_media_AudioTrack_native_write_byte()
    if ((format == AUDIO_FORMAT_PCM_8_BIT)
        && (memoryMode == MODE_STATIC)) {
        ALOGV("android_media_AudioTrack_setup(): requesting MODE_STATIC for 8bit \
            buff size of %dbytes, switching to 16bit, buff size of %dbytes",
            buffSizeInBytes, 2*buffSizeInBytes);
        format = AUDIO_FORMAT_PCM_16_BIT;
        // we will need twice the memory to store the data
        buffSizeInBytes *= 2;
    }

    // compute the frame count
    size_t frameCount;
    if (audio_is_linear_pcm(format)) {
        const size_t bytesPerSample = audio_bytes_per_sample(format);
        frameCount = buffSizeInBytes / (channelCount * bytesPerSample);
    }
    else {
        frameCount = buffSizeInBytes;
    }

    // jclass clazz = env->GetObjectClass(thiz);
    // if (clazz == NULL) {
    //     ALOGE("Can't find %s when setting up callback.", kClassPathName);
    //     return (jint) AUDIOTRACK_ERROR_SETUP_NATIVEINITFAILED;
    // }

    if (jSession == NULL) {
        ALOGE("Error creating AudioTrack: invalid session ID pointer");
        return IAudioSystem::ERROR;
    }

    Int32 sessionId = jSession[0];

    // create the native AudioTrack object
    android::sp<android::AudioTrack> lpTrack = new android::AudioTrack();

    audio_attributes_t *paa = NULL;
    // read the AudioAttributes values
    paa = (audio_attributes_t *) calloc(1, sizeof(audio_attributes_t));

    AutoPtr<CAudioAttributes> attr = (CAudioAttributes*)attributes;
    const char* tags = attr->mFormattedTags.string();
    // copying array size -1, char array for tags was calloc'd, no need to NULL-terminate it
    strncpy(paa->tags, tags, AUDIO_ATTRIBUTES_TAGS_MAX_SIZE - 1);
    paa->usage = (audio_usage_t) attr->mUsage;
    paa->content_type = (audio_content_type_t) attr->mContentType;
    paa->flags = attr->mFlags;

    ALOGV("AudioTrack_setup for usage=%d content=%d flags=0x%#x tags=%s",
            paa->usage, paa->content_type, paa->flags, paa->tags);

    // initialize the callback information:
    // this data will be passed with every AudioTrack callback
    AudioTrackJniStorage* lpJniStorage = new AudioTrackJniStorage();
    // we use a weak reference so the AudioTrack object can be garbage collected.
    lpJniStorage->mCallbackData.mAudioTrackRef = weak_this;

    // initialize the native AudioTrack object
    android::status_t status = android::NO_ERROR;
    switch (memoryMode) {
    case MODE_STREAM:

        status = lpTrack->set(
                AUDIO_STREAM_DEFAULT,// stream type, but more info conveyed in paa (last argument)
                sampleRateInHertz,
                format,// word length, PCM
                nativeChannelMask,
                frameCount,
                AUDIO_OUTPUT_FLAG_NONE,
                audioCallback, &(lpJniStorage->mCallbackData),//callback, callback data (user)
                0,// notificationFrames == 0 since not using EVENT_MORE_DATA to feed the AudioTrack
                0,// shared mem
                true,// thread can call Java
                sessionId,// audio session ID
                android::AudioTrack::TRANSFER_SYNC,
                NULL,                         // default offloadInfo
                -1, -1,                       // default uid, pid values
                paa);
        break;

    case MODE_STATIC:
        // AudioTrack is using shared memory

        if (!lpJniStorage->AllocSharedMem(buffSizeInBytes)) {
            ALOGE("Error creating AudioTrack in static mode: error creating mem heap base");
            goto native_init_failure;
        }

        status = lpTrack->set(
                AUDIO_STREAM_DEFAULT,// stream type, but more info conveyed in paa (last argument)
                sampleRateInHertz,
                format,// word length, PCM
                nativeChannelMask,
                frameCount,
                AUDIO_OUTPUT_FLAG_NONE,
                audioCallback, &(lpJniStorage->mCallbackData),//callback, callback data (user));
                0,// notificationFrames == 0 since not using EVENT_MORE_DATA to feed the AudioTrack
                lpJniStorage->mMemBase,// shared mem
                true,// thread can call Java
                sessionId,// audio session ID
                android::AudioTrack::TRANSFER_SHARED,
                NULL,                         // default offloadInfo
                -1, -1,                       // default uid, pid values
                paa);
        break;

    default:
        ALOGE("Unknown mode %d", memoryMode);
        goto native_init_failure;
    }

    if (status != android::NO_ERROR) {
        ALOGE("Error %d initializing AudioTrack", status);
        goto native_init_failure;
    }

    // read the audio session ID back from AudioTrack in case we create a new session
    jSession[0] = lpTrack->getSessionId();

    // {   // scope for the lock
    //     Mutex::Autolock l(sLock);
    //     sAudioTrackCallBackCookies.add(&lpJniStorage->mCallbackData);
    // }
    // save our newly created C++ AudioTrack in the "nativeTrackInJavaObj" field
    // of the Java object (in mNativeTrackInJavaObj)
    mNativeTrack = (Int64) lpTrack.get();

    // save the JNI resources so we can free them later
    //ALOGV("storing lpJniStorage: %x\n", (long)lpJniStorage);
    mNativeData = (Int64)lpJniStorage;

    // since we had audio attributes, the stream type was derived from them during the
    // creation of the native AudioTrack: push the same value to the Java object
    mStreamType = (Int32)lpTrack->streamType();

    // audio attributes were copied in AudioTrack creation
    free(paa);
    paa = NULL;

    return IAudioSystem::SUCCESS;

    // failures:
native_init_failure:
    if (paa != NULL) {
        free(paa);
    }
    delete lpJniStorage;

    mNativeData = 0;

    return AUDIOTRACK_ERROR_SETUP_NATIVEINITFAILED;
}

void CAudioTrack::NativeFinalize()
{
    //ALOGV("android_media_AudioTrack_finalize jobject: %x\n", (int)thiz);
    NativeRelease();
}

void CAudioTrack::NativeRelease()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return;
    }

    //ALOGV("deleting lpTrack: %x\n", (int)lpTrack);
    lpTrack->stop();
//TODO:
    // delete lpTrack;

    // delete the JNI data
    AudioTrackJniStorage* pJniStorage = (AudioTrackJniStorage *)mNativeData;
    // reset the native resources in the Java object so any attempt to access
    // them after a call to release fails.

    if (pJniStorage) {
        // Mutex::Autolock l(sLock);
        // audiotrack_callback_cookie *lpCookie = &pJniStorage->mCallbackData;
        //ALOGV("deleting pJniStorage: %x\n", (int)pJniStorage);
        // sAudioTrackCallBackCookies.remove(lpCookie);
        delete pJniStorage;
    }

    mNativeTrack = 0;
    mNativeData = 0;
}

void CAudioTrack::NativeStart()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    assert(lpTrack != NULL);

    lpTrack->start();
}

void CAudioTrack::NativeStop()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    assert(lpTrack != NULL);

    lpTrack->stop();
}

void CAudioTrack::NativePause()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    assert(lpTrack != NULL);

    lpTrack->pause();
}

void CAudioTrack::NativeFlush()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    assert(lpTrack != NULL);

    lpTrack->flush();
}

static Int32 WriteToTrack(
    /* [in] */ android::AudioTrack* pTrack,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Byte* data,
    /* [in] */ Int32 offsetInBytes,
    /* [in] */ Int32 sizeInBytes,
    /* [in] */ Boolean blocking = true)
{
    // give the data to the native AudioTrack object (the data starts at the offset)
    ssize_t written = 0;
    // regular write() or copy the data to the AudioTrack's shared memory?
    if (pTrack->sharedBuffer() == 0) {
        written = pTrack->write(data + offsetInBytes, sizeInBytes, blocking);
        // for compatibility with earlier behavior of write(), return 0 in this case
        if (written == (ssize_t) android::WOULD_BLOCK) {
            written = 0;
        }
    }
    else {
        const audio_format_t format = audioFormatToNative(audioFormat);
        switch (format) {

            default:
            case AUDIO_FORMAT_PCM_FLOAT:
            case AUDIO_FORMAT_PCM_16_BIT: {
                // writing to shared memory, check for capacity
                if ((size_t)sizeInBytes > pTrack->sharedBuffer()->size()) {
                    sizeInBytes = pTrack->sharedBuffer()->size();
                }
                memcpy(pTrack->sharedBuffer()->pointer(), data + offsetInBytes, sizeInBytes);
                written = sizeInBytes;
                break;
            }
            case IAudioFormat::ENCODING_PCM_8BIT: {
                // data contains 8bit data we need to expand to 16bit before copying
                // to the shared memory
                // writing to shared memory, check for capacity,
                // note that input data will occupy 2X the input space due to 8 to 16bit conversion
                if (((size_t)sizeInBytes) * 2 > pTrack->sharedBuffer()->size()) {
                    sizeInBytes = pTrack->sharedBuffer()->size() / 2;
                }
                Int32 count = sizeInBytes;
                int16_t *dst = (int16_t *)pTrack->sharedBuffer()->pointer();
                const uint8_t *src = (const uint8_t *)(data + offsetInBytes);

                // memcpy_to_i16_from_u8(dst, src, count)
                dst += count;
                src += count;
                while (count--) {
                    *--dst = (int16_t)(*--src - 0x80) << 8;
                }
                // even though we wrote 2*sizeInBytes, we only report sizeInBytes as written to hide
                // the 8bit mixer restriction from the user of this function
                written = sizeInBytes;
                break;
            }
        }
    }
    return written;
}

Int32 CAudioTrack::NativeWriteByte(
    /* [in] */ ArrayOf<Byte>* audioData,
    /* [in] */ Int32 offsetInBytes,
    /* [in] */ Int32 sizeInBytes,
    /* [in] */ Int32 javaAudioFormat,
    /* [in] */ Boolean isBlocking)
{
    Byte* cAudioData = NULL;
    android::AudioTrack *lpTrack = NULL;

    //LOGV("android_media_AudioTrack_native_write(offset=%d, sizeInBytes=%d) called",offsetInBytes, sizeInBytes);
    lpTrack = (android::AudioTrack *)mNativeTrack;
    assert(lpTrack != NULL);

    // get the pointer for the audio data from the java array
    if (audioData) {
         cAudioData = audioData->GetPayload();
        if (cAudioData == NULL) {
            //LOGE("Error retrieving source of audio data to play, can't play");
            return 0; // out of memory or no data to load
        }
    }
    else {
        //LOGE("NULL java array of audio data to play, can't play");
        return 0;
    }

    Int32 written = WriteToTrack(lpTrack, javaAudioFormat,
            cAudioData, offsetInBytes, sizeInBytes, isBlocking == JNI_TRUE /*blocking*/);

    //LOGV("write wrote %d (tried %d) bytes in the native AudioTrack with offset %d",
    //     (int)written, (int)(sizeInBytes), (int)offsetInBytes);
    return written;
}

Int32 CAudioTrack::NativeWriteInt16(
    /* [in] */ ArrayOf<Int16>* audioData,
    /* [in] */ Int32 offsetInShorts,
    /* [in] */ Int32 sizeInShorts,
    /* [in] */ Int32 javaAudioFormat)
{
    //ALOGV("android_media_AudioTrack_write_short(offset=%d, sizeInShorts=%d) called",
    //    offsetInShorts, sizeInShorts);
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioTrack pointer for write()");
        return 0;
    }

    // get the pointer for the audio data from the java array
    // NOTE: We may use GetPrimitiveArrayCritical() when the JNI implementation changes in such
    // a way that it becomes much more efficient. When doing so, we will have to prevent the
    // AudioSystem callback to be called while in critical section (in case of media server
    // process crash for instance)
    Int16* cAudioData = NULL;
    if (audioData) {
        cAudioData = audioData->GetPayload();
        if (cAudioData == NULL) {
            ALOGE("Error retrieving source of audio data to play, can't play");
            return 0; // out of memory or no data to load
        }
    }
    else {
        ALOGE("NULL java array of audio data to play, can't play");
        return 0;
    }
    Int32 written = WriteToTrack(lpTrack, javaAudioFormat, (Byte *)cAudioData,
                                offsetInShorts * sizeof(short), sizeInShorts * sizeof(short),
            true /*blocking write, legacy behavior*/);

    if (written > 0) {
        written /= sizeof(short);
    }
    //ALOGV("write wrote %d (tried %d) shorts in the native AudioTrack with offset %d",
    //     (int)written, (int)(sizeInShorts), (int)offsetInShorts);

    return written;
}

Int32 CAudioTrack::NativeWriteFloat(
    /* [in] */ ArrayOf<Float>* audioData,
    /* [in] */ Int32 offsetInFloats,
    /* [in] */ Int32 sizeInFloats,
    /* [in] */ Int32 format,
    /* [in] */ Boolean isWriteBlocking)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioTrack pointer for write()");
        return 0;
    }

    Float* cAudioData = NULL;
    if (audioData) {
        cAudioData = audioData->GetPayload();
        if (cAudioData == NULL) {
            ALOGE("Error retrieving source of audio data to play, can't play");
            return 0; // out of memory or no data to load
        }
    }
    else {
        ALOGE("NULL java array of audio data to play, can't play");
        return 0;
    }
    Int32 written = WriteToTrack(lpTrack, format, (Byte *)cAudioData,
                                offsetInFloats * sizeof(float), sizeInFloats * sizeof(float),
                                isWriteBlocking == JNI_TRUE /* blocking */);

    if (written > 0) {
        written /= sizeof(float);
    }

    return written;
}

Int32 CAudioTrack::NativeWriteNativeBytes(
    /* [in] */ IByteBuffer* audioData,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 sizeInBytes,
    /* [in] */ Int32 format,
    /* [in] */ Boolean isWriteBlocking)
{
    //ALOGV("android_media_AudioTrack_write_native_bytes(offset=%d, sizeInBytes=%d) called",
    //    offsetInBytes, sizeInBytes);
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //         "Unable to retrieve AudioTrack pointer for write()");
        return 0;
    }

    ScopedBytesRO bytes(audioData);
    if (bytes.get() == NULL) {
        // ALOGE("Error retrieving source of audio data to play, can't play");
        return IAudioSystem::BAD_VALUE;
    }

    Int32 written = WriteToTrack(lpTrack, format, bytes.get(), byteOffset,
            sizeInBytes, isWriteBlocking == JNI_TRUE /* blocking */);

    return written;
}

Int32 CAudioTrack::NativeGetNativeFrameCount()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return lpTrack->frameCount();
}

void CAudioTrack::NativeSetVolume(
    /* [in] */ Float left,
    /* [in] */ Float right)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return;
    }

    lpTrack->setVolume(left, right);
}

Int32 CAudioTrack::NativeSetPlaybackRate(
    /* [in] */ Int32 sampleRateInHz)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->setSampleRate(sampleRateInHz));
}

Int32 CAudioTrack::NativeGetPlaybackRate()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return (Int32)lpTrack->getSampleRate();
}

Int32 CAudioTrack::NativeSetMarkerPos(
    /* [in] */ Int32 markerPos)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->setMarkerPosition(markerPos));
}

Int32 CAudioTrack::NativeGetMarkerPos()
{
    uint32_t position = 0;

    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    lpTrack->getMarkerPosition(&position);
    return (Int32)position;
}

Int32 CAudioTrack::NativeSetPosUpdatePeriod(
    /* [in] */ Int32 periodInFrames)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->setPositionUpdatePeriod(periodInFrames));
}

Int32 CAudioTrack::NativeGetPosUpdatePeriod()
{
    uint32_t period = 0;

    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    lpTrack->getPositionUpdatePeriod(&period);
    return (Int32)period;
}

Int32 CAudioTrack::NativeSetPosition(
    /* [in] */ Int32 position)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->setPosition(position));
}

Int32 CAudioTrack::NativeGetPosition()
{
    uint32_t position = 0;

    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    lpTrack->getPosition(&position);
    return (Int32)position;
}

Int32 CAudioTrack::NativeGetLatency()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;

    if (lpTrack == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //     "Unable to retrieve AudioTrack pointer for latency()");
        return IAudioSystem::ERROR;
    }
    return (Int32)lpTrack->latency();
}

Int32 CAudioTrack::NativeGetTimestamp(
    /* [in] */ ArrayOf<Int64>* jTimestamp)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;

    if (lpTrack == NULL) {
        // ALOGE("Unable to retrieve AudioTrack pointer for getTimestamp()");
        return IAudioSystem::ERROR;
    }
    android::AudioTimestamp timestamp;
    android::status_t status = lpTrack->getTimestamp(timestamp);

    if (status == android::OK) {
        Int64* nTimestamp = jTimestamp->GetPayload();
        if (nTimestamp == NULL) {
            ALOGE("Unable to get array for getTimestamp()");
            return IAudioSystem::ERROR;
        }
        nTimestamp[0] = (Int64) timestamp.mPosition;
        nTimestamp[1] = (Int64) ((timestamp.mTime.tv_sec * 1000000000LL) + timestamp.mTime.tv_nsec);
    }
    return NativeToElastosStatus(status);
}

Int32 CAudioTrack::NativeSetLoop(
    /* [in] */ Int32 loopStart,
    /* [in] */ Int32 loopEnd,
    /* [in] */ Int32 loopCount)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->setLoop(loopStart, loopEnd, loopCount));
}

Int32 CAudioTrack::NativeReload()
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->reload());
}

Int32 CAudioTrack::NativeGetOutputSampleRate(
   /* [in] */ Int32 javaStreamType)
{
    uint32_t afSamplingRate;
    // convert the stream type from Java to native value
    // FIXME: code duplication with android_media_AudioTrack_native_setup()
    audio_stream_type_t nativeStreamType;
    switch (javaStreamType) {
    case AUDIO_STREAM_VOICE_CALL:
    case AUDIO_STREAM_SYSTEM:
    case AUDIO_STREAM_RING:
    case AUDIO_STREAM_MUSIC:
    case AUDIO_STREAM_ALARM:
    case AUDIO_STREAM_NOTIFICATION:
    case AUDIO_STREAM_BLUETOOTH_SCO:
    case AUDIO_STREAM_DTMF:
#ifdef QCOM_HARDWARE
    case AUDIO_STREAM_INCALL_MUSIC:
#endif
        nativeStreamType = (audio_stream_type_t) javaStreamType;
        break;
    default:
        nativeStreamType = AUDIO_STREAM_DEFAULT;
        break;
    }

    android::status_t status = android::AudioSystem::getOutputSamplingRate(&afSamplingRate, nativeStreamType);
    if (status != android::NO_ERROR) {
        ALOGE("Error %d in AudioSystem::getOutputSamplingRate() for stream type %d "
              "in AudioTrack JNI", status, nativeStreamType);
        return DEFAULT_OUTPUT_SAMPLE_RATE;
    }
    else {
        return afSamplingRate;
    }
}

// returns the minimum required size for the successful creation of a streaming AudioTrack
// returns -1 if there was an error querying the hardware.
Int32 CAudioTrack::NativeGetMinBuffSize(
    /* [in] */ Int32 sampleRateInHertz,
    /* [in] */ Int32 channelCount,
    /* [in] */ Int32 audioFormat)
{
    size_t frameCount;
    const android::status_t status = android::AudioTrack::getMinFrameCount(
            &frameCount, AUDIO_STREAM_DEFAULT, sampleRateInHertz);
    if (status != android::NO_ERROR) {
        ALOGE("AudioTrack::getMinFrameCount() for sample rate %d failed with status %d",
                sampleRateInHertz, status);
        return -1;
    }
    const audio_format_t format = audioFormatToNative(audioFormat);
    if (audio_is_linear_pcm(format)) {
        const size_t bytesPerSample = audio_bytes_per_sample(format);
        return frameCount * channelCount * bytesPerSample;
    }
    else {
        return frameCount;
    }
}

Int32 CAudioTrack::NativeSetAuxEffectSendLevel(
    /* [in] */ Float level)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return -1;
    }

    android::status_t status = lpTrack->setAuxEffectSendLevel(level);
    if (status != android::NO_ERROR) {
        ALOGE("AudioTrack::setAuxEffectSendLevel() for level %g failed with status %d",
                level, status);
    }
    return (Int32) status;
}

Int32 CAudioTrack::NativeAttachAuxEffect(
    /* [in] */ Int32 effectId)
{
    android::AudioTrack* lpTrack = (android::AudioTrack *)mNativeTrack;
    if (lpTrack == NULL) {
        return IAudioSystem::ERROR;
    }

    return NativeToElastosStatus(lpTrack->attachAuxEffect(effectId));
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
