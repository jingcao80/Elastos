
#include "CAudioTrack.h"
#include <stdio.h>
#include <utils/Log.h>
#include <assert.h>

int GetMinBufferSize(
    /* [in] */ int sampleRateInHz,
    /* [in] */ int channelConfig,
    /* [in] */ int audioFormat)
{
    int channelCount = 0;
    switch(channelConfig) {
    case AudioFormat::CHANNEL_OUT_MONO:
    case AudioFormat::CHANNEL_CONFIGURATION_MONO:
        channelCount = 1;
        break;
    case AudioFormat::CHANNEL_OUT_STEREO:
    case AudioFormat::CHANNEL_CONFIGURATION_STEREO:
        channelCount = 2;
        break;
    default:
        ALOGE("getMinBufferSize(): Invalid channel configuration.");
        return AudioTrack_ERROR_BAD_VALUE;
    }

    if ((audioFormat != AudioFormat::ENCODING_PCM_16BIT)
        && (audioFormat != AudioFormat::ENCODING_PCM_8BIT)) {
        ALOGE("getMinBufferSize(): Invalid audio format.");
        return AudioTrack_ERROR_BAD_VALUE;
    }

    if ( (sampleRateInHz < 4000) || (sampleRateInHz > 48000) ) {
        ALOGE("getMinBufferSize(): %d Hz is not a supported sample rate.",
             sampleRateInHz);
        return AudioTrack_ERROR_BAD_VALUE;
    }

    int size = CAudioTrack::native_get_min_buff_size(
                    sampleRateInHz, channelCount, audioFormat);
    if ((size == -1) || (size == 0)) {
        ALOGE("getMinBufferSize(): error querying hardware");
        return AudioTrack_ERROR;
    }
    else {
        return size;
    }
}

CAudioTrack::AudioTrackStorage::~AudioTrackStorage()
{
    mMemBase.clear();
    mMemHeap.clear();
}

bool CAudioTrack::AudioTrackStorage::AllocSharedMem(
    /* [in] */ int sizeInBytes)
{
    mMemHeap = new android::MemoryHeapBase(sizeInBytes, 0, "AudioTrack Heap Base");
    if (mMemHeap->getHeapID() < 0) {
        return false;
    }
    mMemBase = new android::MemoryBase(mMemHeap, 0, sizeInBytes);
    return true;
}

CAudioTrack::CAudioTrack()
    : mState(AudioTrack_STATE_UNINITIALIZED)
    , mPlayState(AudioTrack_PLAYSTATE_STOPPED)
    , mNativeBufferSizeInBytes(0)
    , mSampleRate(22050)
    , mChannelCount(1)
    , mChannels(AudioFormat::CHANNEL_OUT_MONO)
    , mStreamType(AudioManager::STREAM_MUSIC)
    , mDataLoadMode(AudioTrack_MODE_STREAM)
    , mChannelConfiguration(AudioFormat::CHANNEL_OUT_MONO)
    , mAudioFormat(AudioFormat::ENCODING_PCM_16BIT)
    , mSessionId(0)
    , mNativeAudioTrack(NULL)
    , mStorage(NULL)
{}

CAudioTrack::~CAudioTrack()
{
    delete mNativeAudioTrack;
    delete mStorage;
}

  // Convenience method for the constructor's parameter checks.
// This is where constructor IllegalArgumentException-s are thrown
// postconditions:
//    mStreamType is valid
//    mChannelCount is valid
//    mChannels is valid
//    mAudioFormat is valid
//    mSampleRate is valid
//    mDataLoadMode is valid
void CAudioTrack::AudioParamCheck(
    /* [in] */ int streamType,
    /* [in] */ int sampleRateInHz,
    /* [in] */ int channelConfig,
    /* [in] */ int audioFormat,
    /* [in] */ int mode)
{
    //--------------
    // stream type
    if( (streamType != AudioManager::STREAM_ALARM) && (streamType != AudioManager::STREAM_MUSIC)
       && (streamType != AudioManager::STREAM_RING) && (streamType != AudioManager::STREAM_SYSTEM)
       && (streamType != AudioManager::STREAM_VOICE_CALL)
       && (streamType != AudioManager::STREAM_NOTIFICATION)
       && (streamType != AudioManager::STREAM_BLUETOOTH_SCO)
       && (streamType != AudioManager::STREAM_DTMF)) {
       //throw (new IllegalArgumentException("Invalid stream type."));
        assert(0);
    }
    else {
        mStreamType = streamType;
    }

    //--------------
    // sample rate
    if ( (sampleRateInHz < 4000) || (sampleRateInHz > 48000) ) {
        // throw (new IllegalArgumentException(sampleRateInHz
        //         + "Hz is not a supported sample rate."));
        assert(0);
    }
    else {
        mSampleRate = sampleRateInHz;
    }

    //--------------
    // channel config
    mChannelConfiguration = channelConfig;

    switch (channelConfig) {
    case AudioFormat::CHANNEL_OUT_DEFAULT: //AudioFormat::CHANNEL_CONFIGURATION_DEFAULT
    case AudioFormat::CHANNEL_OUT_MONO:
    case AudioFormat::CHANNEL_CONFIGURATION_MONO:
        mChannelCount = 1;
        mChannels = AudioFormat::CHANNEL_OUT_MONO;
        break;
    case AudioFormat::CHANNEL_OUT_STEREO:
    case AudioFormat::CHANNEL_CONFIGURATION_STEREO:
        mChannelCount = 2;
        mChannels = AudioFormat::CHANNEL_OUT_STEREO;
        break;
    default:
        mChannelCount = 0;
        mChannels = AudioFormat::CHANNEL_INVALID;
        mChannelConfiguration = AudioFormat::CHANNEL_CONFIGURATION_INVALID;
        //throw(new IllegalArgumentException("Unsupported channel configuration."));
        assert(0);
    }

    //--------------
    // audio format
    switch (audioFormat) {
    case AudioFormat::ENCODING_DEFAULT:
        mAudioFormat = AudioFormat::ENCODING_PCM_16BIT;
        break;
    case AudioFormat::ENCODING_PCM_16BIT:
    case AudioFormat::ENCODING_PCM_8BIT:
        mAudioFormat = audioFormat;
        break;
    default:
        mAudioFormat = AudioFormat::ENCODING_INVALID;
        assert(0);
        // throw(new IllegalArgumentException("Unsupported sample encoding."
        //     + " Should be ENCODING_PCM_8BIT or ENCODING_PCM_16BIT."));
    }

    //--------------
    // audio load mode
    if ( (mode != AudioTrack_MODE_STREAM)
        && (mode != AudioTrack_MODE_STATIC) ) {
        //throw(new IllegalArgumentException("Invalid mode."));
        assert(0);
    }
    else {
        mDataLoadMode = mode;
    }
}


// Convenience method for the contructor's audio buffer size check.
// preconditions:
//    mChannelCount is valid
//    mAudioFormat is valid
// postcondition:
//    mNativeBufferSizeInBytes is valid (multiple of frame size, positive)
void CAudioTrack::AudioBuffSizeCheck(
    /* [in] */ int audioBufferSize)
{
    // NB: this section is only valid with PCM data.
    //     To update when supporting compressed formats
    int frameSizeInBytes = mChannelCount
            * (mAudioFormat == AudioFormat::ENCODING_PCM_8BIT ? 1 : 2);
    if ((audioBufferSize % frameSizeInBytes != 0) || (audioBufferSize < 1)) {
        //throw (new IllegalArgumentException("Invalid audio buffer size."));
        assert(0);
    }

    mNativeBufferSizeInBytes = audioBufferSize;
}

ECode CAudioTrack::Destroy()
{
    // even though native_release() stops the native AudioTrack, we need to stop
    // AudioTrack subclasses too.
    Stop();

    native_release();
    mState = AudioTrack_STATE_UNINITIALIZED;

    return NOERROR;
}

ECode CAudioTrack::GetPlayState(
    /* [out] */ Int32 * pState)
{
    AutoLock lock(mPlayStateLock);
    *pState = mPlayState;

    return NOERROR;
}

ECode CAudioTrack::GetPlaybackHeadPosition(
    /* [out] */ Int32 * pPosition)
{
    *pPosition = native_get_position();

    return NOERROR;
}

ECode CAudioTrack::SetStereoVolume(
    /* [in] */ Float leftVolume,
    /* [in] */ Float rightVolume,
    /* [out] */ Int32 * pResult)
{
    if (mState != AudioTrack_STATE_INITIALIZED) {
        *pResult = AudioTrack_ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    // clamp the volumes
    if (leftVolume < VOLUME_MIN) {
        leftVolume = VOLUME_MIN;
    }
    if (leftVolume > VOLUME_MAX) {
        leftVolume = VOLUME_MAX;
    }
    if (rightVolume < VOLUME_MIN) {
        rightVolume = VOLUME_MIN;
    }
    if (rightVolume > VOLUME_MAX) {
        rightVolume = VOLUME_MAX;
    }

    native_setVolume(leftVolume, rightVolume);

    *pResult = AudioTrack_SUCCESS;

    return NOERROR;
}

ECode CAudioTrack::Play()
{
    if (mState != AudioTrack_STATE_INITIALIZED) {
        //throw(new IllegalStateException("play() called on uninitialized AudioTrack."));
        assert(0);
        return NOERROR;
    }

    AutoLock lock(mPlayStateLock);
    native_start();
    mPlayState = AudioTrack_PLAYSTATE_PLAYING;

    return NOERROR;
}

ECode CAudioTrack::Pause()
{
    if (mState != AudioTrack_STATE_INITIALIZED) {
        //throw(new IllegalStateException("pause() called on uninitialized AudioTrack."));
        assert(0);
        return NOERROR;
    }

    // pause playback
    AutoLock lock(mPlayStateLock);
    native_pause();
    mPlayState = AudioTrack_PLAYSTATE_PAUSED;

    return NOERROR;
}

ECode CAudioTrack::Stop()
{
    if (mState != AudioTrack_STATE_INITIALIZED) {
        //throw(new IllegalStateException("stop() called on uninitialized AudioTrack."));
        assert(0);
        return NOERROR;
    }

    // stop playing
    AutoLock lock(mPlayStateLock);
    native_stop();
    mPlayState = AudioTrack_PLAYSTATE_STOPPED;

    return NOERROR;
}

ECode CAudioTrack::Flush()
{
    if (mState == AudioTrack_STATE_INITIALIZED) {
        // flush the data in native layer
        native_flush();
    }
    return NOERROR;
}

ECode CAudioTrack::Write(
    /* [in] */ const MemoryBuf & audioData,
    /* [out] */ Int32 * pWritten)
{
    if ((mDataLoadMode == AudioTrack_MODE_STATIC)
        && (mState == AudioTrack_STATE_NO_STATIC_DATA)
        && (audioData.GetUsed() > 0)) {
        mState = AudioTrack_STATE_INITIALIZED;
    }

    if (mState != AudioTrack_STATE_INITIALIZED) {
        *pWritten = AudioTrack_ERROR_INVALID_OPERATION;
        return NOERROR;
    }

    if (audioData.IsNullOrEmpty()) {
        *pWritten = AudioTrack_ERROR_BAD_VALUE;
        return NOERROR;
    }

    *pWritten = native_write_byte(audioData.GetPayload(),
                     0, audioData.GetUsed(), mAudioFormat);

    return NOERROR;
}

ECode CAudioTrack::Init(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 sampleRateInHz,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat,
    /* [in] */ Int32 bufferSizeInBytes,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 sessionId)
{
    mState = AudioTrack_STATE_UNINITIALIZED;

    // remember which looper is associated with the AudioTrack instanciation
    // if ((mInitializationLooper = Looper.myLooper()) == null) {
    //     mInitializationLooper = Looper.getMainLooper();
    // }

    AudioParamCheck(streamType, sampleRateInHz, channelConfig, audioFormat, mode);

    AudioBuffSizeCheck(bufferSizeInBytes);

    if (sessionId < 0) {
        //throw (new IllegalArgumentException("Invalid audio session ID: "+sessionId));
        assert(0);
    }

    int session[1];
    session[0] = sessionId;
    // native initialization
    int initResult = native_setup(
        mStreamType, mSampleRate, mChannels, mAudioFormat,
        mNativeBufferSizeInBytes, mDataLoadMode, session);
    if (initResult != AudioTrack_SUCCESS) {
        //ALOGE("Error code "+initResult+" when initializing AudioTrack.");
        assert(0);
    }

    mSessionId = session[0];

    if (mDataLoadMode == AudioTrack_MODE_STATIC) {
        mState = AudioTrack_STATE_NO_STATIC_DATA;
    }
    else {
        mState = AudioTrack_STATE_INITIALIZED;
    }
    return NOERROR;
}

#define DEFAULT_OUTPUT_SAMPLE_RATE   44100

#define AUDIOTRACK_SUCCESS                         0
#define AUDIOTRACK_ERROR                           -1
#define AUDIOTRACK_ERROR_BAD_VALUE                 -2
#define AUDIOTRACK_ERROR_INVALID_OPERATION         -3
#define AUDIOTRACK_ERROR_SETUP_AUDIOSYSTEM         -16
#define AUDIOTRACK_ERROR_SETUP_INVALIDCHANNELMASK -17
#define AUDIOTRACK_ERROR_SETUP_INVALIDFORMAT       -18
#define AUDIOTRACK_ERROR_SETUP_INVALIDSTREAMTYPE   -19
#define AUDIOTRACK_ERROR_SETUP_NATIVEINITFAILED    -20

// ----------------------------------------------------------------------------
static void audioCallback(
    /* [in] */ int event,
    /* [in] */ void* user,
    /* [in] */ void *info)
{
    if (event == android::AudioTrack::EVENT_MORE_DATA) {
        // set size to 0 to signal we're not using the callback to write more data
        android::AudioTrack::Buffer* pBuff = (android::AudioTrack::Buffer*)info;
        pBuff->size = 0;

    }
    else if (event == android::AudioTrack::EVENT_MARKER
        || event == android::AudioTrack::EVENT_NEW_POS) {
        if (user) {
            ALOGV("audioCallback event = %d", event);
            //((CAudioTrack*)user->PostEventFromNative(event, 0, 0, NULL);
        }
    }
}

int CAudioTrack::native_setup(
    /* [in] */ int streamType,
    /* [in] */ int sampleRate,
    /* [in] */ int channelMask,
    /* [in] */ int audioFormat,
    /* [in] */ int buffSizeInBytes,
    /* [in] */ int mode,
    /* [in] */ int* nSession)
{
    ALOGV("sampleRate=%d, audioFormat =%d, channel mask=%x, buffSize=%d",
        sampleRate, audioFormat, channelMask, buffSizeInBytes);

    int afSampleRate;
    int afFrameCount;

    if (android::AudioSystem::getOutputFrameCount(&afFrameCount, streamType) != android::NO_ERROR) {
        ALOGE("Error creating AudioTrack: Could not get AudioSystem frame count.");
        return AUDIOTRACK_ERROR_SETUP_AUDIOSYSTEM;
    }
    if (android::AudioSystem::getOutputSamplingRate(&afSampleRate, streamType) != android::NO_ERROR) {
        ALOGE("Error creating AudioTrack: Could not get AudioSystem sampling rate.");
        return AUDIOTRACK_ERROR_SETUP_AUDIOSYSTEM;
    }

    // Java channel masks don't map directly to the native definition, but it's a simple shift
    // to skip the two deprecated channel configurations "default" and "mono".
    uint32_t nativeChannelMask = ((uint32_t)channelMask) >> 2;

    if (!audio_is_output_channel(nativeChannelMask)) {
        ALOGE("Error creating AudioTrack: invalid channel mask.");
        return AUDIOTRACK_ERROR_SETUP_INVALIDCHANNELMASK;
    }

    int nbChannels = popcount(nativeChannelMask);

    // check the stream type
    audio_stream_type_t atStreamType;
    switch (streamType) {
    case AUDIO_STREAM_VOICE_CALL:
    case AUDIO_STREAM_SYSTEM:
    case AUDIO_STREAM_RING:
    case AUDIO_STREAM_MUSIC:
    case AUDIO_STREAM_ALARM:
    case AUDIO_STREAM_NOTIFICATION:
    case AUDIO_STREAM_BLUETOOTH_SCO:
    case AUDIO_STREAM_DTMF:
        atStreamType = (audio_stream_type_t) streamType;
        break;
    default:
        ALOGE("Error creating AudioTrack: unknown stream type.");
        return AUDIOTRACK_ERROR_SETUP_INVALIDSTREAMTYPE;
    }

    // check the format.
    // This function was called from Java, so we compare the format against the Java constants
    if ((audioFormat != AudioFormat::ENCODING_PCM_16BIT) &&
        (audioFormat != AudioFormat::ENCODING_PCM_8BIT)) {
        ALOGE("Error creating AudioTrack: unsupported audio format.");
        return AUDIOTRACK_ERROR_SETUP_INVALIDFORMAT;
    }

    // for the moment 8bitPCM in MODE_STATIC is not supported natively in the AudioTrack C++ class
    // so we declare everything as 16bitPCM, the 8->16bit conversion for MODE_STATIC will be handled
    // in android_media_AudioTrack_native_write()
    if ((audioFormat == AudioFormat::ENCODING_PCM_8BIT)
        && (mode == AudioTrack_MODE_STATIC)) {
        ALOGV("android_media_AudioTrack_native_setup(): requesting MODE_STATIC for 8bit \
            buff size of %dbytes, switching to 16bit, buff size of %dbytes",
            buffSizeInBytes, 2*buffSizeInBytes);
        audioFormat = AudioFormat::ENCODING_PCM_16BIT;
        // we will need twice the memory to store the data
        buffSizeInBytes *= 2;
    }

    // compute the frame count
    int bytesPerSample = audioFormat == AudioFormat::ENCODING_PCM_16BIT ? 2 : 1;
    audio_format_t format = audioFormat == AudioFormat::ENCODING_PCM_16BIT ?
            AUDIO_FORMAT_PCM_16_BIT : AUDIO_FORMAT_PCM_8_BIT;
    int frameCount = buffSizeInBytes / (nbChannels * bytesPerSample);

    AudioTrackStorage* lpStorage = new AudioTrackStorage();

    if (nSession == NULL) {
        ALOGE("Error creating AudioTrack: invalid session ID pointer");
        return AUDIOTRACK_ERROR;
    }

    int sessionId = nSession[0];

    // create the native AudioTrack object
    android::AudioTrack* lpTrack = new android::AudioTrack();
    if (lpTrack == NULL) {
        ALOGE("Error creating uninitialized AudioTrack");
        goto native_track_failure;
    }

    // initialize the native AudioTrack object
    if (mode == AudioTrack_MODE_STREAM) {
        lpTrack->set(
            atStreamType,// stream type
            sampleRate,
            format,// word length, PCM
            nativeChannelMask,
            frameCount,
            AUDIO_OUTPUT_FLAG_NONE,// flags
            audioCallback, (void*)this,//callback, callback data (user)
            0,// notificationFrames == 0 since not using EVENT_MORE_DATA to feed the AudioTrack
            0,// shared mem
            true,// thread can call Java
            sessionId);// audio session ID

    }
    else if (mode == AudioTrack_MODE_STATIC) {
        // AudioTrack is using shared memory

        if (!lpStorage->AllocSharedMem(buffSizeInBytes)) {
            ALOGE("Error creating AudioTrack in static mode: error creating mem heap base");
            goto native_init_failure;
        }

        lpTrack->set(
            atStreamType,// stream type
            sampleRate,
            format,// word length, PCM
            nativeChannelMask,
            frameCount,
            AUDIO_OUTPUT_FLAG_NONE,// flags
            audioCallback, (void*)this,//callback, callback data (user));
            0,// notificationFrames == 0 since not using EVENT_MORE_DATA to feed the AudioTrack
            lpStorage->mMemBase,// shared mem
            true,// thread can call Java
            sessionId);// audio session ID
    }

    if (lpTrack->initCheck() != android::NO_ERROR) {
        ALOGE("Error initializing AudioTrack");
        goto native_init_failure;
    }

    // read the audio session ID back from AudioTrack in case we create a new session
    nSession[0] = lpTrack->getSessionId();

    mNativeAudioTrack = lpTrack;
    mStorage = lpStorage;

    return AUDIOTRACK_SUCCESS;

    // failures:
native_init_failure:
    delete lpTrack;
    mNativeAudioTrack = NULL;

native_track_failure:
    delete lpStorage;
    mStorage = NULL;
    return AUDIOTRACK_ERROR_SETUP_NATIVEINITFAILED;
}

void CAudioTrack::native_finalize()
{
    if (mNativeAudioTrack) {
        //ALOGV("deleting lpTrack: %x\n", (int)lpTrack);
        mNativeAudioTrack->stop();
        delete mNativeAudioTrack;
        mNativeAudioTrack = NULL;
    }

    if (mStorage) {
        delete mStorage;
        mStorage = NULL;
    }
}

void CAudioTrack::native_release()
{
    native_finalize();
}

int CAudioTrack::native_get_position()
{
    uint32_t position = 0;

    if (mNativeAudioTrack) {
        mNativeAudioTrack->getPosition(&position);
        return position;
    }
    else {
        assert(0);
        return AUDIOTRACK_ERROR;
    }
}

void CAudioTrack::native_setVolume(float leftVolume, float rightVolume)
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->setVolume(leftVolume, rightVolume);
}

void CAudioTrack::native_start()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->start();
}

void CAudioTrack::native_pause()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->pause();
}


void CAudioTrack::native_stop()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->stop();
}


void CAudioTrack::native_flush()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->flush();
}

int CAudioTrack::native_write_byte(
    /* [in] */ void* audioData,
    /* [in] */ int offsetInBytes,
    /* [in] */ int sizeInBytes,
    /* [in] */ int format)
{
    assert(mNativeAudioTrack);
    if (audioData == NULL) {
        ALOGE("NULL array of audio data to play, can't play");
        return 0; // out of memory or no data to load
    }

    //ALOGV("android_media_AudioTrack_native_write(offset=%d, sizeInBytes=%d) called",
    //    offsetInBytes, sizeInBytes);

    // give the data to the native AudioTrack object (the data starts at the offset)
    ssize_t written = 0;
    // regular write() or copy the data to the AudioTrack's shared memory?
    if (mNativeAudioTrack->sharedBuffer() == 0) {
        written = mNativeAudioTrack->write(audioData + offsetInBytes, sizeInBytes);
    }
    else {
        if (format == AudioFormat::ENCODING_PCM_16BIT) {
            // writing to shared memory, check for capacity
            if ((size_t)sizeInBytes > mNativeAudioTrack->sharedBuffer()->size()) {
                sizeInBytes = mNativeAudioTrack->sharedBuffer()->size();
            }
            memcpy(mNativeAudioTrack->sharedBuffer()->pointer(), audioData + offsetInBytes, sizeInBytes);
            written = sizeInBytes;
        }
        else if (format == AudioFormat::ENCODING_PCM_8BIT) {
            // data contains 8bit data we need to expand to 16bit before copying
            // to the shared memory
            // writing to shared memory, check for capacity,
            // note that input data will occupy 2X the input space due to 8 to 16bit conversion
            if (((size_t)sizeInBytes)*2 > mNativeAudioTrack->sharedBuffer()->size()) {
                sizeInBytes = mNativeAudioTrack->sharedBuffer()->size() / 2;
            }
            int count = sizeInBytes;
            int16_t *dst = (int16_t *)mNativeAudioTrack->sharedBuffer()->pointer();
            const int8_t *src = (const int8_t *)(audioData + offsetInBytes);
            while(count--) {
                *dst++ = (int16_t)(*src++^0x80) << 8;
            }
            // even though we wrote 2*sizeInBytes, we only report sizeInBytes as written to hide
            // the 8bit mixer restriction from the user of this function
            written = sizeInBytes;
        }
    }

    //ALOGV("write wrote %d (tried %d) bytes in the native AudioTrack with offset %d",
    //     (int)written, (int)(sizeInBytes), (int)offsetInBytes);
    return written;
}

int CAudioTrack::native_get_min_buff_size(
    /* [in] */ int sampleRateInHz,
    /* [in] */ int channelConfig,
    /* [in] */ int audioFormat)
{
    int frameCount = 0;
    if (android::AudioTrack::getMinFrameCount(&frameCount, AUDIO_STREAM_DEFAULT,
        sampleRateInHz) != android::NO_ERROR) {
        return -1;
    }

    return frameCount * channelConfig * (audioFormat == AudioFormat::ENCODING_PCM_16BIT ? 2 : 1);
}

