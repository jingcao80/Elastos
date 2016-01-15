#ifndef __ELASTOS_DROID_SPEECH_TTS_BlockingAudioTrack_H__
#define __ELASTOS_DROID_SPEECH_TTS_BlockingAudioTrack_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "elastos/droid/speech/tts/AudioOutputParams.h"
#include "Elastos.Droid.Speech.h"

//
// WAITING for IAudioTrack
//
using Elastos::Droid::Media::IAudioTrack;
//#define IAudioTrack void *

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Exposes parts of the {@link AudioTrack} API by delegating calls to an
 * underlying {@link AudioTrack}. Additionally, provides methods like
 * {@link #waitAndRelease()} that will block until all audiotrack
 * data has been flushed to the mixer, and is estimated to have completed
 * playback.
 */
class BlockingAudioTrack
    : public Object
    , public IBlockingAudioTrack
{
public:
    CAR_INTERFACE_DECL();

    BlockingAudioTrack();

    virtual ~BlockingAudioTrack();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ AudioOutputParams *audioParams,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 channelCount);

    CARAPI Init(
        /* [out] */ Boolean* ret);

    CARAPI Stop();

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int32* ret);

    CARAPI WaitAndRelease();

public:
    static CARAPI_(Int32) GetChannelConfig(
        /* [in] */ Int32 channelCount);

    CARAPI_(Int64) GetAudioLengthMs(
        /* [in] */ Int32 numBytes);

private:
    static CARAPI_(Int32) WriteToAudioTrack(
        /* [in] */ IAudioTrack* audioTrack,
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI_(AutoPtr<IAudioTrack>) CreateStreamingAudioTrack();

    CARAPI_(void) BlockUntilDone(
        /* [in] */ IAudioTrack* audioTrack);

    CARAPI_(void) BlockUntilEstimatedCompletion();

    CARAPI_(void) BlockUntilCompletion(
        /* [in] */ IAudioTrack* audioTrack);

    static CARAPI_(void) SetupVolume(
        /* [in] */ IAudioTrack* audioTrack,
        /* [in] */ Float volume,
        /* [in] */ Float pan);

    static CARAPI_(Int64) Clip(
        /* [in] */ Int64 value,
        /* [in] */ Int64 min,
        /* [in] */ Int64 max);

    static CARAPI_(Float) Clip(
        /* [in] */ Float value,
        /* [in] */ Float min,
        /* [in] */ Float max);

private:
    static const String TAG;        // = "TTS.BlockingAudioTrack";
    static const Boolean DBG;       // = FALSE;

    /**
     * The minimum increment of time to wait for an AudioTrack to finish
     * playing.
     */
    static const Int64 MIN_SLEEP_TIME_MS;

    /**
     * The maximum increment of time to sleep while waiting for an AudioTrack
     * to finish playing.
     */
    static const Int64 MAX_SLEEP_TIME_MS;

    /**
     * The maximum amount of time to wait for an audio track to make progress while
     * it remains in PLAYSTATE_PLAYING. This should never happen in normal usage, but
     * could happen in exceptional circumstances like a media_server crash.
     */
    static const Int64 MAX_PROGRESS_WAIT_MS;

    /**
     * Minimum size of the buffer of the underlying {@link android.media.AudioTrack}
     * we create.
     */
    static const Int32 MIN_AUDIO_BUFFER_SIZE;

    AudioOutputParams* mAudioParams;
    Int32 mSampleRateInHz;
    Int32 mAudioFormat;
    Int32 mChannelCount;
    Float mVolume;
    Float mPan;

    Int32 mBytesPerFrame;
    /**
     * A "short utterance" is one that uses less bytes than the audio
     * track buffer size (mAudioBufferSize). In this case, we need to call
     * {@link AudioTrack#stop()} to send pending buffers to the mixer, and slightly
     * different logic is required to wait for the track to finish.
     *
     * Not volatile, accessed only from the audio playback thread.
     */
    Boolean mIsShortUtterance;
    /**
     * Will be valid after a call to {@link #init()}.
     */
    Int32 mAudioBufferSize;
    Int32 mBytesWritten;

    // Need to be seen by stop() which can be called from another thread. mAudioTrack will be
    // set to null only after waitAndRelease().
    Object mAudioTrackLock;
    AutoPtr<IAudioTrack> mAudioTrack;
    /*volatile*/ Boolean mStopped;
    Int32 mSessionId;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_BlockingAudioTrack_H__
