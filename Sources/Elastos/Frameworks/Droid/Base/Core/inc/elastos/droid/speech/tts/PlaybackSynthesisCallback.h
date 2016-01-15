#ifndef __ELASTOS_DROID_SPEECH_TTS_PlaybackSynthesisCallback_H__
#define __ELASTOS_DROID_SPEECH_TTS_PlaybackSynthesisCallback_H__

#include "elastos/droid/speech/tts/AbstractSynthesisCallback.h"
#include "elastos/droid/speech/tts/SynthesisPlaybackQueueItem.h"
#include "elastos/droid/speech/tts/EventLoggerV1.h"
#include "elastos/droid/speech/tts/AudioPlaybackHandler.h"
#include "elastos/droid/speech/tts/AudioOutputParams.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Speech synthesis request that plays the audio as it is received.
 */
class PlaybackSynthesisCallback
    : public AbstractSynthesisCallback
{
public:
    PlaybackSynthesisCallback(
        /* [in] */ AudioOutputParams* audioParams,
        /* [in] */ AudioPlaybackHandler* audioTrackHandler,
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ EventLoggerV1* logger,
        /* [in] */ Boolean clientIsUsingV2);

    //@Override
    CARAPI Stop();

    //@Override
    CARAPI GetMaxBufferSize(
        /* [out] */ Int32* ret);

    CARAPI HasStarted(
        /* [out] */ Boolean* ret);

    CARAPI HasFinished(
        /* [out] */ Boolean* ret);

    //@Override
    CARAPI Start(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 channelCount,
        /* [out] */ Int32* ret);

    //@Override
    CARAPI AudioAvailable(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* ret);

    //@Override
    CARAPI Done(
        /* [out] */ Int32* ret);

    //@Override
    CARAPI Error();

    //@Override
    CARAPI Error(
        /* [in] */ Int32 errorCode);

private:
    static const String TAG;            // = "PlaybackSynthesisRequest";
    static const Boolean DBG;           // = FALSE;

    static const Int32 MIN_AUDIO_BUFFER_SIZE;       // = 8192;

    AudioOutputParams* mAudioParams;

    /**
     * Guards {@link #mAudioTrackHandler}, {@link #mItem} and {@link #mStopped}.
     */
    Object mStateLock;

    // Handler associated with a thread that plays back audio requests.
    AutoPtr<AudioPlaybackHandler> mAudioTrackHandler;
    // A request "token", which will be non null after start() has been called.
    AutoPtr<SynthesisPlaybackQueueItem> mItem;
    /** Status code of synthesis */
    Int32 mStatusCode;

    volatile Boolean mDone;

    AutoPtr<IUtteranceProgressDispatcher> mDispatcher;
    AutoPtr<IInterface> mCallerIdentity;
    AutoPtr<EventLoggerV1> mLogger;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_PlaybackSynthesisCallback_H__
