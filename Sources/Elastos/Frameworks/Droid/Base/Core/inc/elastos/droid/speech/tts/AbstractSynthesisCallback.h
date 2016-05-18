#ifndef __ELASTOS_DROID_SPEECH_TTS_AbstractSynthesisCallback_H__
#define __ELASTOS_DROID_SPEECH_TTS_AbstractSynthesisCallback_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "elastos/droid/speech/tts/SynthesisCallback.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Defines additional methods the synthesis callback must implement that
 * are private to the TTS service implementation.
 *
 * All of these class methods (with the exception of {@link #stop()}) can be only called on the
 * synthesis thread, while inside
 * {@link TextToSpeechService#onSynthesizeText} or {@link TextToSpeechService#onSynthesizeTextV2}.
 * {@link #stop()} is the exception, it may be called from multiple threads.
 */

//
//abstract class
//
class AbstractSynthesisCallback
    : public Object
    , public SynthesisCallback
{
public:
    CAR_INTERFACE_DECL()

    AbstractSynthesisCallback();

    virtual ~AbstractSynthesisCallback();

    CARAPI constructor();

    /**
     * Constructor.
     * @param clientIsUsingV2 If true, this callback will be used inside
     *         {@link TextToSpeechService#onSynthesizeTextV2} method.
     */
     CARAPI constructor(
        /* [in] */ Boolean clientIsUsingV2);

    /**
     * Aborts the speech request.
     *
     * Can be called from multiple threads.
     */
    CARAPI Stop();

    /**
     * Get status code for a "stop".
     *
     * V2 Clients will receive special status, V1 clients will receive standard error.
     *
     * This method should only be called on the synthesis thread,
     * while in {@link TextToSpeechService#onSynthesizeText}.
     */
    ECode ErrorCodeOnStop(
        /* [out] */ Int32* ret);

protected:
    /** If true, request comes from V2 TTS interface */
    Boolean mClientIsUsingV2;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_AbstractSynthesisCallback_H__
