
#include "ElAudioTrack.h"
#include <stdio.h>
#include <utils/Log.h>
#include <assert.h>

ElAudioTrack::AudioTrackStorage::~AudioTrackStorage()
{
    mMemBase.clear();
    mMemHeap.clear();
}

bool ElAudioTrack::AudioTrackStorage::AllocSharedMem(
    /* [in] */ int sizeInBytes)
{
    mMemHeap = new android::MemoryHeapBase(sizeInBytes, 0, "AudioTrack Heap Base");
    if (mMemHeap->getHeapID() < 0) {
        return false;
    }
    mMemBase = new android::MemoryBase(mMemHeap, 0, sizeInBytes);
    return true;
}

ElAudioTrack::ElAudioTrack(
    /* [in] */ int streamType,
    /* [in] */ int sampleRateInHz,
    /* [in] */ int channelConfig,
    /* [in] */ int audioFormat,
    /* [in] */ int bufferSizeInBytes,
    /* [in] */ int mode,
    /* [in] */ int sessionId)
    : mState(STATE_UNINITIALIZED)
    , mPlayState(PLAYSTATE_STOPPED)
    , mNativeBufferSizeInBytes(0)
    , mSampleRate(22050)
    , mChannelCount(1)
    , mChannels(AudioFormat::CHANNEL_OUT_MONO)
    , mStreamType(AudioManager::STREAM_MUSIC)
    , mDataLoadMode(MODE_STREAM)
    , mChannelConfiguration(AudioFormat::CHANNEL_OUT_MONO)
    , mAudioFormat(AudioFormat::ENCODING_PCM_16BIT)
    , mSessionId(0)
    , mNativeAudioTrack(NULL)
    , mStorage(NULL)
{
    mState = STATE_UNINITIALIZED;

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
    if (initResult != SUCCESS) {
        //ALOGE("Error code "+initResult+" when initializing AudioTrack.");
        assert(0);
    }

    mSessionId = session[0];

    if (mDataLoadMode == MODE_STATIC) {
        mState = STATE_NO_STATIC_DATA;
    }
    else {
        mState = STATE_INITIALIZED;
    }
}

ElAudioTrack::~ElAudioTrack()
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
void ElAudioTrack::AudioParamCheck(
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
    if ( (mode != MODE_STREAM) && (mode != MODE_STATIC) ) {
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
void ElAudioTrack::AudioBuffSizeCheck(
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

void ElAudioTrack::Release()
{
    // even though native_release() stops the native AudioTrack, we need to stop
    // AudioTrack subclasses too.
    Stop();

    native_release();
    mState = STATE_UNINITIALIZED;
}

/**
 * Returns the playback state of the AudioTrack instance.
 * @see #PLAYSTATE_STOPPED
 * @see #PLAYSTATE_PAUSED
 * @see #PLAYSTATE_PLAYING
 */
int ElAudioTrack::GetPlayState()
{
    AutoLock lock(mPlayStateLock);
    return mPlayState;
}

 /**
 * Returns the playback head position expressed in frames
 */
int ElAudioTrack::GetPlaybackHeadPosition()
{
    return native_get_position();
}

/**
 * Returns the minimum buffer size required for the successful creation of an AudioTrack
 * object to be created in the {@link #MODE_STREAM} mode. Note that this size doesn't
 * guarantee a smooth playback under load, and higher values should be chosen according to
 * the expected frequency at which the buffer will be refilled with additional data to play.
 * @param sampleRateInHz the sample rate expressed in Hertz.
 * @param channelConfig describes the configuration of the audio channels.
 *   See {@link AudioFormat#CHANNEL_OUT_MONO} and
 *   {@link AudioFormat#CHANNEL_OUT_STEREO}
 * @param audioFormat the format in which the audio data is represented.
 *   See {@link AudioFormat#ENCODING_PCM_16BIT} and
 *   {@link AudioFormat#ENCODING_PCM_8BIT}
 * @return {@link #ERROR_BAD_VALUE} if an invalid parameter was passed,
 *   or {@link #ERROR} if the implementation was unable to query the hardware for its output
 *     properties,
 *   or the minimum buffer size expressed in bytes.
 */
int ElAudioTrack::GetMinBufferSize(
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
        return ERROR_BAD_VALUE;
    }

    if ((audioFormat != AudioFormat::ENCODING_PCM_16BIT)
        && (audioFormat != AudioFormat::ENCODING_PCM_8BIT)) {
        ALOGE("getMinBufferSize(): Invalid audio format.");
        return ERROR_BAD_VALUE;
    }

    if ( (sampleRateInHz < 4000) || (sampleRateInHz > 48000) ) {
        ALOGE("getMinBufferSize(): %d Hz is not a supported sample rate.", sampleRateInHz);
        return ERROR_BAD_VALUE;
    }

    int size = native_get_min_buff_size(sampleRateInHz, channelCount, audioFormat);
    if ((size == -1) || (size == 0)) {
        ALOGE("getMinBufferSize(): error querying hardware");
        return ERROR;
    }
    else {
        return size;
    }
}

/**
 * Sets the specified left/right output volume values on the AudioTrack. Values are clamped
 * to the ({@link #VOLUME_MIN}, {@link #VOLUME_MAX}) interval if outside this range.
 * @param leftVolume output attenuation for the left channel. A value of 0.0f is silence,
 *      a value of 1.0f is no attenuation.
 * @param rightVolume output attenuation for the right channel
 * @return error code or success, see {@link #SUCCESS},
 *    {@link #ERROR_INVALID_OPERATION}
 */
int ElAudioTrack::SetStereoVolume(
    /* [in] */ float leftVolume,
    /* [in] */ float rightVolume)
{
    if (mState != STATE_INITIALIZED) {
        return ERROR_INVALID_OPERATION;
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

    return SUCCESS;
}

/**
 * Starts playing an AudioTrack.
 *
 * @throws IllegalStateException
 */
void ElAudioTrack::Play()
{
    if (mState != STATE_INITIALIZED) {
        //throw(new IllegalStateException("play() called on uninitialized AudioTrack."));
        assert(0);
        return;
    }

    AutoLock lock(mPlayStateLock);
    native_start();
    mPlayState = PLAYSTATE_PLAYING;
}

/**
 * Stops playing the audio data.
 *
 * @throws IllegalStateException
 */
void ElAudioTrack::Stop()
{
    if (mState != STATE_INITIALIZED) {
        //throw(new IllegalStateException("stop() called on uninitialized AudioTrack."));
        assert(0);
        return;
    }

    // stop playing
    AutoLock lock(mPlayStateLock);
    native_stop();
    mPlayState = PLAYSTATE_STOPPED;
}

//---------------------------------------------------------
// Audio data supply
//--------------------

/**
 * Flushes the audio data currently queued for playback. Any data that has
 * not been played back will be discarded.
 */
void ElAudioTrack::Flush()
{
    if (mState == STATE_INITIALIZED) {
        // flush the data in native layer
        native_flush();
    }
}

int ElAudioTrack::Write(void* audioData, int offsetInBytes, int sizeInBytes)
{
    if ((mDataLoadMode == MODE_STATIC)
        && (mState == STATE_NO_STATIC_DATA)
        && (sizeInBytes > 0)) {
        mState = STATE_INITIALIZED;
    }

    if (mState != STATE_INITIALIZED) {
        return ERROR_INVALID_OPERATION;
    }

    if ( (audioData == NULL) || (offsetInBytes < 0 ) || (sizeInBytes < 0)
        /*|| (offsetInBytes + sizeInBytes > audioData.length)*/) {
        return ERROR_BAD_VALUE;
    }

    return native_write_byte(audioData, offsetInBytes, sizeInBytes, mAudioFormat);
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
static void audioCallback(int event, void* user, void *info)
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
            //((ElAudioTrack*)user->PostEventFromNative(event, 0, 0, NULL);
        }
    }
}

int ElAudioTrack::native_setup(int streamType, int sampleRate, int channelMask, int audioFormat,
        int buffSizeInBytes, int mode, int* nSession)
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
        && (mode == MODE_STATIC)) {
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
    if (mode == MODE_STREAM) {
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
    else if (mode == MODE_STATIC) {
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

void ElAudioTrack::native_finalize()
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

void ElAudioTrack::native_release()
{
    native_finalize();
}

int ElAudioTrack::native_get_position()
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

void ElAudioTrack::native_setVolume(float leftVolume, float rightVolume)
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->setVolume(leftVolume, rightVolume);
}

void ElAudioTrack::native_start()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->start();
}

void ElAudioTrack::native_stop()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->stop();
}

void ElAudioTrack::native_flush()
{
    assert(mNativeAudioTrack);
    mNativeAudioTrack->flush();
}

int ElAudioTrack::native_write_byte(void* audioData, int offsetInBytes, int sizeInBytes, int format)
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

int ElAudioTrack::native_get_min_buff_size(
    int sampleRateInHz, int channelConfig, int audioFormat)
{
    int frameCount = 0;
    if (android::AudioTrack::getMinFrameCount(&frameCount, AUDIO_STREAM_DEFAULT,
        sampleRateInHz) != android::NO_ERROR) {
        return -1;
    }

    return frameCount * channelConfig * (audioFormat == AudioFormat::ENCODING_PCM_16BIT ? 2 : 1);
}

