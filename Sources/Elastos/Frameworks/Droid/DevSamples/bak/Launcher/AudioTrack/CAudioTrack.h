
#ifndef __CAUDIOTRACK_H__
#define __CAUDIOTRACK_H__

#include "_CAudioTrack.h"
#include <media/AudioTrack.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryBase.h>
#include "AudioTrackHelper.h"


class AudioManager
{
public:
    /** The audio stream for phone calls */
    static const int STREAM_VOICE_CALL = 0;//AudioSystem.STREAM_VOICE_CALL;
    /** The audio stream for system sounds */
    static const int STREAM_SYSTEM = 1;//AudioSystem.STREAM_SYSTEM;
    /** The audio stream for the phone ring */
    static const int STREAM_RING = 2;//AudioSystem.STREAM_RING;
    /** The audio stream for music playback */
    static const int STREAM_MUSIC = 3;//AudioSystem.STREAM_MUSIC;
    /** The audio stream for alarms */
    static const int STREAM_ALARM = 4;//AudioSystem.STREAM_ALARM;
    /** The audio stream for notifications */
    static const int STREAM_NOTIFICATION = 5;//AudioSystem.STREAM_NOTIFICATION;
    /** @hide The audio stream for phone calls when connected to bluetooth */
    static const int STREAM_BLUETOOTH_SCO = 6;//AudioSystem.STREAM_BLUETOOTH_SCO;
    /** @hide The audio stream for enforced system sounds in certain countries (e.g camera in Japan) */
    static const int STREAM_SYSTEM_ENFORCED = 7;//AudioSystem.STREAM_SYSTEM_ENFORCED;
    /** The audio stream for DTMF Tones */
    static const int STREAM_DTMF = 8;//AudioSystem.STREAM_DTMF;
    /** @hide The audio stream for text to speech (TTS) */
    static const int STREAM_TTS = 9;//AudioSystem.STREAM_TTS;
};

class AudioFormat
{
public:
    //---------------------------------------------------------
    // Constants
    //--------------------
    /** Invalid audio data format */
    static const int ENCODING_INVALID = 0;
    /** Default audio data format */
    static const int ENCODING_DEFAULT = 1;
    /** Audio data format: PCM 16 bit per sample. Guaranteed to be supported by devices. */
    static const int ENCODING_PCM_16BIT = 2; // accessed by native code
    /** Audio data format: PCM 8 bit per sample. Not guaranteed to be supported by devices. */
    static const int ENCODING_PCM_8BIT = 3;  // accessed by native code

    /** Invalid audio channel configuration */
    /** @deprecated use CHANNEL_INVALID instead  */
    static const int CHANNEL_CONFIGURATION_INVALID   = 0;
    /** Default audio channel configuration */
    /** @deprecated use CHANNEL_OUT_DEFAULT or CHANNEL_IN_DEFAULT instead  */
    static const int CHANNEL_CONFIGURATION_DEFAULT   = 1;
    /** Mono audio configuration */
    /** @deprecated use CHANNEL_OUT_MONO or CHANNEL_IN_MONO instead  */
    static const int CHANNEL_CONFIGURATION_MONO      = 2;
    /** Stereo (2 channel) audio configuration */
    /** @deprecated use CHANNEL_OUT_STEREO or CHANNEL_IN_STEREO instead  */
    static const int CHANNEL_CONFIGURATION_STEREO    = 3;

    /** Invalid audio channel mask */
    static const int CHANNEL_INVALID = 0;
    /** Default audio channel mask */
    static const int CHANNEL_OUT_DEFAULT = 1;

    // Channel mask definitions below are translated to the native values defined in
    //  in /system/core/include/system/audio.h in the JNI code of AudioTrack
    static const int CHANNEL_OUT_FRONT_LEFT = 0x4;
    static const int CHANNEL_OUT_FRONT_RIGHT = 0x8;
    static const int CHANNEL_OUT_FRONT_CENTER = 0x10;
    static const int CHANNEL_OUT_LOW_FREQUENCY = 0x20;
    static const int CHANNEL_OUT_BACK_LEFT = 0x40;
    static const int CHANNEL_OUT_BACK_RIGHT = 0x80;
    static const int CHANNEL_OUT_FRONT_LEFT_OF_CENTER = 0x100;
    static const int CHANNEL_OUT_FRONT_RIGHT_OF_CENTER = 0x200;
    static const int CHANNEL_OUT_BACK_CENTER = 0x400;
    /** @hide */
    static const int CHANNEL_OUT_SIDE_LEFT =         0x800;
    /** @hide */
    static const int CHANNEL_OUT_SIDE_RIGHT =       0x1000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_CENTER =       0x2000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_FRONT_LEFT =   0x4000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_FRONT_CENTER = 0x8000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_FRONT_RIGHT = 0x10000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_BACK_LEFT =   0x20000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_BACK_CENTER = 0x40000;
    /** @hide */
    static const int CHANNEL_OUT_TOP_BACK_RIGHT =  0x80000;

    static const int CHANNEL_OUT_MONO = CHANNEL_OUT_FRONT_LEFT;
    static const int CHANNEL_OUT_STEREO = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT);
    static const int CHANNEL_OUT_QUAD = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT |
            CHANNEL_OUT_BACK_LEFT | CHANNEL_OUT_BACK_RIGHT);
    static const int CHANNEL_OUT_SURROUND = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT |
            CHANNEL_OUT_FRONT_CENTER | CHANNEL_OUT_BACK_CENTER);
    static const int CHANNEL_OUT_5POINT1 = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT |
            CHANNEL_OUT_FRONT_CENTER | CHANNEL_OUT_LOW_FREQUENCY | CHANNEL_OUT_BACK_LEFT | CHANNEL_OUT_BACK_RIGHT);
    static const int CHANNEL_OUT_7POINT1 = (CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_RIGHT |
            CHANNEL_OUT_FRONT_CENTER | CHANNEL_OUT_LOW_FREQUENCY | CHANNEL_OUT_BACK_LEFT | CHANNEL_OUT_BACK_RIGHT |
            CHANNEL_OUT_FRONT_LEFT_OF_CENTER | CHANNEL_OUT_FRONT_RIGHT_OF_CENTER);
    /** @hide */
    static const int CHANNEL_OUT_7POINT1_SURROUND = (
            CHANNEL_OUT_FRONT_LEFT | CHANNEL_OUT_FRONT_CENTER | CHANNEL_OUT_FRONT_RIGHT |
            CHANNEL_OUT_SIDE_LEFT | CHANNEL_OUT_SIDE_RIGHT |
            CHANNEL_OUT_BACK_LEFT | CHANNEL_OUT_BACK_RIGHT |
            CHANNEL_OUT_LOW_FREQUENCY);

    static const int CHANNEL_IN_DEFAULT = 1;
    static const int CHANNEL_IN_LEFT = 0x4;
    static const int CHANNEL_IN_RIGHT = 0x8;
    static const int CHANNEL_IN_FRONT = 0x10;
    static const int CHANNEL_IN_BACK = 0x20;
    static const int CHANNEL_IN_LEFT_PROCESSED = 0x40;
    static const int CHANNEL_IN_RIGHT_PROCESSED = 0x80;
    static const int CHANNEL_IN_FRONT_PROCESSED = 0x100;
    static const int CHANNEL_IN_BACK_PROCESSED = 0x200;
    static const int CHANNEL_IN_PRESSURE = 0x400;
    static const int CHANNEL_IN_X_AXIS = 0x800;
    static const int CHANNEL_IN_Y_AXIS = 0x1000;
    static const int CHANNEL_IN_Z_AXIS = 0x2000;
    static const int CHANNEL_IN_VOICE_UPLINK = 0x4000;
    static const int CHANNEL_IN_VOICE_DNLINK = 0x8000;
    static const int CHANNEL_IN_MONO = CHANNEL_IN_FRONT;
    static const int CHANNEL_IN_STEREO = (CHANNEL_IN_LEFT | CHANNEL_IN_RIGHT);
};

CarClass(CAudioTrack)
{
private:
    /** Minimum value for a channel volume */
    static const float VOLUME_MIN = 0.0f;
    /** Maximum value for a channel volume */
    static const float VOLUME_MAX = 1.0f;

    static const int ERROR_NATIVESETUP_AUDIOSYSTEM         = -16;
    static const int ERROR_NATIVESETUP_INVALIDCHANNELMASK  = -17;
    static const int ERROR_NATIVESETUP_INVALIDFORMAT       = -18;
    static const int ERROR_NATIVESETUP_INVALIDSTREAMTYPE   = -19;
    static const int ERROR_NATIVESETUP_NATIVEINITFAILED    = -20;

    // Events:
    // to keep in sync with frameworks/base/include/media/AudioTrack.h
    /**
     * Event id denotes when playback head has reached a previously set marker.
     */
    static const int NATIVE_EVENT_MARKER  = 3;
    /**
     * Event id denotes when previously set update period has elapsed during playback.
     */
    static const int NATIVE_EVENT_NEW_POS = 4;

public:
    CAudioTrack();

    ~CAudioTrack();

    CARAPI Destroy();

    CARAPI GetPlayState(
        /* [out] */ Int32 * pState);

    CARAPI GetPlaybackHeadPosition(
        /* [out] */ Int32 * pPosition);

    CARAPI SetStereoVolume(
        /* [in] */ Float leftVolume,
        /* [in] */ Float rightVolume,
        /* [out] */ Int32 * pResult);

    CARAPI Play();

    CARAPI Pause();

    CARAPI Stop();

    CARAPI Flush();

    CARAPI Write(
        /* [in] */ const MemoryBuf & audioData,
        /* [out] */ Int32 * pWritten);

    CARAPI Init(
        /* [in] */ Int32 streamType,
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 bufferSizeInBytes,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 sessionId);

private:
    void AudioParamCheck(
        /* [in] */ int streamType,
        /* [in] */ int sampleRateInHz,
        /* [in] */ int channelConfig,
        /* [in] */ int audioFormat,
        /* [in] */ int mode);

    void AudioBuffSizeCheck(
        /* [in] */ int audioBufferSize);

private:
    int native_setup(
        /* [in] */ int streamType,
        /* [in] */ int sampleRate,
        /* [in] */ int nbChannels,
        /* [in] */ int audioFormat,
        /* [in] */ int buffSizeInBytes,
        /* [in] */ int mode,
        /* [in] */ int* nSession);

    void native_finalize();

    void native_release();

    int native_get_position();

    void native_setVolume(
        /* [in] */ float leftVolume,
        /* [in] */ float rightVolume);

    void native_start();

    void native_pause();

    void native_stop();

    void native_flush();

    int native_write_byte(
        /* [in] */ void* audioData,
        /* [in] */ int offsetInBytes,
        /* [in] */ int sizeInBytes,
        /* [in] */ int format);

    static int native_get_min_buff_size(
        /* [in] */ int sampleRateInHz,
        /* [in] */ int channelConfig,
        /* [in] */ int audioFormat);

private:
    /**
     * Indicates the state of the AudioTrack instance.
     */
    int mState;

    /**
     * Indicates the play state of the AudioTrack instance.
     */
    int mPlayState;

    /**
     * Lock to make sure mPlayState updates are reflecting the actual state of the object.
     */
    Object mPlayStateLock;

    int mNativeBufferSizeInBytes;

    // /**
    //  * Looper associated with the thread that creates the AudioTrack instance.
    //  */
    // private Looper mInitializationLooper = null;
    /**
     * The audio data sampling rate in Hz.
     */
    int mSampleRate;
    /**
     * The number of audio output channels (1 is mono, 2 is stereo).
     */
    int mChannelCount;
    /**
     * The audio channel mask.
     */
    int mChannels;

    /**
     * The type of the audio stream to play. See
     *   {@link AudioManager#STREAM_VOICE_CALL}, {@link AudioManager#STREAM_SYSTEM},
     *   {@link AudioManager#STREAM_RING}, {@link AudioManager#STREAM_MUSIC} and
     *   {@link AudioManager#STREAM_ALARM}
     */
    int mStreamType;
    /**
     * The way audio is consumed by the hardware, streaming or static.
     */
    int mDataLoadMode;
    /**
     * The current audio channel configuration.
     */
    int mChannelConfiguration;
    /**
     * The encoding of the audio samples.
     * @see AudioFormat#ENCODING_PCM_8BIT
     * @see AudioFormat#ENCODING_PCM_16BIT
     */
    int mAudioFormat;
    /**
     * Audio session ID
     */
    int mSessionId;

    android::AudioTrack* mNativeAudioTrack;

private:
    class AudioTrackStorage
    {
    public:
        ~AudioTrackStorage();

        bool AllocSharedMem(
            /* [in] */ int sizeInBytes);
    public:
        android::sp<android::MemoryHeapBase> mMemHeap;
        android::sp<android::MemoryBase> mMemBase;
    };
    AudioTrackStorage* mStorage;

    friend int GetMinBufferSize(
        /* [in] */ int sampleRateInHz,
        /* [in] */ int channelConfig,
        /* [in] */ int audioFormat);
};

#endif // __CAUDIOTRACK_H__
