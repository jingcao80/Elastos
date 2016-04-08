
#ifndef __ELAUDIOTRACK_H__
#define __ELAUDIOTRACK_H__

#include <media/AudioTrack.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryBase.h>


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

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

class ElAudioTrack
{
public:
    static const char* TAG;
    /** indicates AudioTrack state is stopped */
    static const int PLAYSTATE_STOPPED = 1;  // matches SL_PLAYSTATE_STOPPED
    /** indicates AudioTrack state is paused */
    static const int PLAYSTATE_PAUSED  = 2;  // matches SL_PLAYSTATE_PAUSED
    /** indicates AudioTrack state is playing */
    static const int PLAYSTATE_PLAYING = 3;  // matches SL_PLAYSTATE_PLAYING

    /**
     * Creation mode where audio data is transferred from Java to the native layer
     * only once before the audio starts playing.
     */
    static const int MODE_STATIC = 0;
    /**
     * Creation mode where audio data is streamed from Java to the native layer
     * as the audio is playing.
     */
    static const int MODE_STREAM = 1;

    /**
     * State of an AudioTrack that was not successfully initialized upon creation.
     */
    static const int STATE_UNINITIALIZED = 0;
    /**
     * State of an AudioTrack that is ready to be used.
     */
    static const int STATE_INITIALIZED   = 1;
    /**
     * State of a successfully initialized AudioTrack that uses static data,
     * but that hasn't received that data yet.
     */
    static const int STATE_NO_STATIC_DATA = 2;

    // Error codes:
    // to keep in sync with frameworks/base/core/jni/android_media_AudioTrack.cpp
    /**
     * Denotes a successful operation.
     */
    static const int SUCCESS                               = 0;
    /**
     * Denotes a generic operation failure.
     */
    static const int ERROR                                 = -1;
    /**
     * Denotes a failure due to the use of an invalid value.
     */
    static const int ERROR_BAD_VALUE                       = -2;
    /**
     * Denotes a failure due to the improper use of a method.
     */
    static const int ERROR_INVALID_OPERATION               = -3;

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
    ElAudioTrack(
        /* [in] */ int streamType,
        /* [in] */ int sampleRateInHz,
        /* [in] */ int channelConfig,
        /* [in] */ int audioFormat,
        /* [in] */ int bufferSizeInBytes,
        /* [in] */ int mode,
        /* [in] */ int sessionId = 0);

    ~ElAudioTrack();

    void Release();

    int GetPlayState();

    int GetPlaybackHeadPosition();

    static int GetMinBufferSize(
        /* [in] */ int sampleRateInHz,
        /* [in] */ int channelConfig,
        /* [in] */ int audioFormat);

    int SetStereoVolume(
        /* [in] */ float leftVolume,
        /* [in] */ float rightVolume);

    void Play();

    void Pause();

    void Stop();

    void Flush();

    int Write(void* audioData, int offsetInBytes, int sizeInBytes);

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
    int native_setup(int streamType, int sampleRate, int nbChannels, int audioFormat,
        int buffSizeInBytes, int mode, int* nSession);

    void native_finalize();

    void native_release();

    int native_get_position();

    void native_setVolume(float leftVolume, float rightVolume);

    void native_start();

    void native_pause();

    void native_stop();

    void native_flush();

    int native_write_byte(void* audioData, int offsetInBytes, int sizeInBytes, int format);

    static int native_get_min_buff_size(
        int sampleRateInHz, int channelConfig, int audioFormat);

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
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__ELAUDIOTRACK_H__
