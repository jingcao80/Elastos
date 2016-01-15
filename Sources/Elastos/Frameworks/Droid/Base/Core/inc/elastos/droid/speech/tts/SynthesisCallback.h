#ifndef __ELASTOS_DROID_SPEECH_TTS_SynthesisCallback_H__
#define __ELASTOS_DROID_SPEECH_TTS_SynthesisCallback_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Speech.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * A callback to return speech data synthesized by a text to speech engine.
 *
 * The engine can provide streaming audio by calling
 * {@link #start}, then {@link #audioAvailable} until all audio has been provided, then finally
 * {@link #done}.
 *
 * {@link #error} can be called at any stage in the synthesis process to
 * indicate that an error has occurred, but if the call is made after a call
 * to {@link #done}, it might be discarded.
 *
 * {@link #done} must be called at the end of synthesis, regardless of errors.
 *
 * All methods can be only called on the synthesis thread.
 */

//
//public interface
//
class SynthesisCallback
    : public ISynthesisCallback
{
public:
    /**
     * @return the maximum number of bytes that the TTS engine can pass in a single call of
     *         {@link #audioAvailable}. Calls to {@link #audioAvailable} with data lengths
     *         larger than this value will not succeed.
     */
    CARAPI GetMaxBufferSize(
        /* [out] */ Int32* ret) = 0;

    /**
     * The service should call this when it starts to synthesize audio for this
     * request.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * @param sampleRateInHz Sample rate in HZ of the generated audio.
     * @param audioFormat Audio format of the generated audio. Must be one of
     *         the ENCODING_ constants defined in {@link android.media.AudioFormat}.
     * @param channelCount The number of channels. Must be {@code 1} or {@code 2}.
     * @return {@link TextToSpeech#SUCCESS}, {@link TextToSpeech#ERROR} or
     *          {@link TextToSpeech#STOPPED}.
     */
    CARAPI Start(
        /* [in] */ Int32 sampleRateInHz,
        /* [in] */ Int32 audioFormat,
        /* [in] */ Int32 channelCount,
        /* [out] */ Int32* ret) = 0;

    /**
     * The service should call this method when synthesized audio is ready for consumption.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * @param buffer The generated audio data. This method will not hold on to {@code buffer},
     *         so the caller is free to modify it after this method returns.
     * @param offset The offset into {@code buffer} where the audio data starts.
     * @param length The number of bytes of audio data in {@code buffer}. This must be
     *         less than or equal to the return value of {@link #getMaxBufferSize}.
     * @return {@link TextToSpeech#SUCCESS}, {@link TextToSpeech#ERROR} or
     *          {@link TextToSpeech#STOPPED}.
     */
    CARAPI AudioAvailable(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* ret) = 0;

    /**
     * The service should call this method when all the synthesized audio for a request has
     * been passed to {@link #audioAvailable}.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * This method has to be called if {@link #start} and/or {@link #error} was called.
     *
     * @return {@link TextToSpeech#SUCCESS}, {@link TextToSpeech#ERROR} or
     *          {@link TextToSpeech#STOPPED}.
     */
    CARAPI Done(
        /* [out] */ Int32* ret) = 0;

    /**
     * The service should call this method if the speech synthesis fails.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     */
    CARAPI Error() = 0;

    /**
     * The service should call this method if the speech synthesis fails.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * @param errorCode Error code to pass to the client. One of the ERROR_ values from
     *      {@link TextToSpeech}
     */
    CARAPI Error(
        /* [in] */ Int32 errorCode) = 0;

    /**
     * Check if {@link #start} was called or not.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * Useful for checking if a fallback from network request is possible.
     */
    CARAPI HasStarted(
        /* [out] */ Boolean* started) = 0;

    /**
     * Check if {@link #done} was called or not.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     *
     * Useful for checking if a fallback from network request is possible.
     */
    CARAPI HasFinished(
        /* [out] */ Boolean* finished) = 0;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_SynthesisCallback_H__
