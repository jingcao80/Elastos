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
