#ifndef __ELASTOS_DROID_SPEECH_TTS_EventLoggerV1_H__
#define __ELASTOS_DROID_SPEECH_TTS_EventLoggerV1_H__

/**
 * AbstractEventLogger.java
 * EventLoggerV1.java
 */

#include "elastos/droid/speech/tts/AbstractEventLogger.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Writes data about a given speech synthesis request for V1 API to the event
 * logs. The data that is logged includes the calling app, length of the
 * utterance, speech rate / pitch, the latency, and overall time taken.
 */
class EventLoggerV1
    : public AbstractEventLogger
{
public:
    ECode constructor(
        /* [in] */ ISynthesisRequest* request,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& serviceApp);

    // protected abstract void
    ECode LogFailure(
        /* [in] */  Int32 statusCode);

    // protected abstract void
    ECode LogSuccess(
        /* [in] */ Int64 audioLatency,
        /* [in] */ Int64 engineLatency,
        /* [in] */ Int64 engineTotal);

private:
    /**
     * @return the length of the utterance for the given synthesis, 0
     *          if the utterance was {@code null}.
     */
    CARAPI_(Int32) GetUtteranceLength();

    /**
     * Returns a formatted locale string from the synthesis params of the
     * form lang-country-variant.
     */
    CARAPI_(String) GetLocaleString();

private:
    AutoPtr<ISynthesisRequest> mRequest;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_EventLoggerV1_H__
