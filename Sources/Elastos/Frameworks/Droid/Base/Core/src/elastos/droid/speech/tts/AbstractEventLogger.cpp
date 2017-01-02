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

#include "elastos/droid/speech/tts/AbstractEventLogger.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CAR_INTERFACE_IMPL(AbstractEventLogger, Object, IAbstractEventLogger);

AbstractEventLogger::AbstractEventLogger()
{}

AbstractEventLogger::~AbstractEventLogger()
{}

ECode AbstractEventLogger::constructor(
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& serviceApp)
{
    mCallerUid = callerUid;
    mCallerPid = callerPid;
    mServiceApp = serviceApp;
    mReceivedTime = SystemClock::GetElapsedRealtime();

    mPlaybackStartTime = -1;
    mRequestProcessingStartTime = -1;
    mEngineStartTime = -1;
    mEngineCompleteTime = -1;

    mLogWritten = FALSE;

    return NOERROR;
}

ECode AbstractEventLogger::OnRequestProcessingStart()
{
    mRequestProcessingStartTime = SystemClock::GetElapsedRealtime();

    return NOERROR;
}

ECode AbstractEventLogger::OnEngineDataReceived()
{
    if (mEngineStartTime == -1) {
        mEngineStartTime = SystemClock::GetElapsedRealtime();
    }

    return NOERROR;
}

ECode AbstractEventLogger::OnEngineComplete()
{
    mEngineCompleteTime = SystemClock::GetElapsedRealtime();

    return NOERROR;
}

ECode AbstractEventLogger::OnAudioDataWritten()
{
    // For now, keep track of only the first chunk of audio
    // that was played.
    if (mPlaybackStartTime == -1) {
        mPlaybackStartTime = SystemClock::GetElapsedRealtime();
    }

    return NOERROR;
}

ECode AbstractEventLogger::OnCompleted(
    /* [in] */ Int32 statusCode)
{
    if (mLogWritten) {
        return NOERROR;
    } else {
        mLogWritten = TRUE;
    }

    Int64 UNUSED(completionTime) = SystemClock::GetElapsedRealtime();

    // We don't report latency for stopped syntheses because their overall
    // total time spent will be inaccurate (will not correlate with
    // the length of the utterance).

    // onAudioDataWritten() should normally always be called, and hence mPlaybackStartTime
    // should be set, if an error does not occur.
    if (statusCode != ITextToSpeech::TTS_SUCCESS
            || mPlaybackStartTime == -1 || mEngineCompleteTime == -1) {
        LogFailure(statusCode);
        return NOERROR;
    }

    Int64 audioLatency = mPlaybackStartTime - mReceivedTime;
    Int64 engineLatency = mEngineStartTime - mRequestProcessingStartTime;
    Int64 engineTotal = mEngineCompleteTime - mRequestProcessingStartTime;

    LogSuccess(audioLatency, engineLatency, engineTotal);

    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
