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

#ifndef __ELASTOS_DROID_SPEECH_TTS_AbstractEventLogger_H__
#define __ELASTOS_DROID_SPEECH_TTS_AbstractEventLogger_H__

/**
 * AbstractEventLogger.java
 */

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "Elastos.Droid.Speech.h"

using Elastos::Droid::Speech::Tts::IAbstractEventLogger;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Base class for storing data about a given speech synthesis request to the
 * event logs. The data that is logged depends on actual implementation. Note
 * that {@link AbstractEventLogger#onAudioDataWritten()} and
 * {@link AbstractEventLogger#onEngineComplete()} must be called from a single
 * thread (usually the audio playback thread}.
 */
class AbstractEventLogger
    : public Object
    , public IAbstractEventLogger
{
public:
    CAR_INTERFACE_DECL()

    AbstractEventLogger();

    virtual ~AbstractEventLogger();

    CARAPI constructor(
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& serviceApp);

    /**
     * Notifies the logger that this request has been selected from
     * the processing queue for processing. Engine latency / total time
     * is measured from this baseline.
     */
    CARAPI OnRequestProcessingStart();

    /**
     * Notifies the logger that a chunk of data has been received from
     * the engine. Might be called multiple times.
     */
    CARAPI OnEngineDataReceived();

    /**
     * Notifies the logger that the engine has finished processing data.
     * Will be called exactly once.
     */
    CARAPI OnEngineComplete();

    /**
     * Notifies the logger that audio playback has started for some section
     * of the synthesis. This is normally some amount of time after the engine
     * has synthesized data and varies depending on utterances and
     * other audio currently in the queue.
     */
    CARAPI OnAudioDataWritten();

    /**
     * Notifies the logger that the current synthesis has completed.
     * All available data is not logged.
     */
    CARAPI OnCompleted(
        /* [in] */  Int32 statusCode);

    // protected abstract void
    CARAPI LogFailure(
        /* [in] */  Int32 statusCode);

    // protected abstract void
    CARAPI LogSuccess(
        /* [in] */ Int64 audioLatency,
        /* [in] */ Int64 engineLatency,
        /* [in] */ Int64 engineTotal);

private:
    String mServiceApp;
    Int32 mCallerUid;
    Int32 mCallerPid;
    Int64 mReceivedTime;
    Int64 mPlaybackStartTime;

    volatile Int64 mRequestProcessingStartTime;
    volatile Int64 mEngineStartTime;
    volatile Int64 mEngineCompleteTime;

    Boolean mLogWritten;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_AbstractEventLogger_H__
