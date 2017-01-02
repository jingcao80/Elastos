//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
