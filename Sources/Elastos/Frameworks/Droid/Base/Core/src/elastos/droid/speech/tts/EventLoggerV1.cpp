#include "elastos/droid/speech/tts/EventLoggerV1.h"
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

ECode EventLoggerV1::constructor(
    /* [in] */ ISynthesisRequest* request,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& serviceApp)
{
    AbstractEventLogger::constructor(callerUid, callerPid, serviceApp);
    mRequest = request;

    return NOERROR;
}

Int32 EventLoggerV1::GetUtteranceLength()
{
    String utterance;
    mRequest->GetText(&utterance);
    return utterance.IsNull() ? 0 : utterance.GetLength();
}

String EventLoggerV1::GetLocaleString()
{
    //Java:    StringBuilder sb = new StringBuilder(mRequest.getLanguage());
    String rLanguage;
    StringBuilder sb;
    sb.Append((mRequest->GetLanguage(&rLanguage), rLanguage));

    String rCountry;
    AutoPtr<ICharSequence> cs;
    CString::New((mRequest->GetCountry(&rCountry), rCountry), (ICharSequence**)&cs);

    if (!TextUtils::IsEmpty(cs)) {
        sb.AppendChar((Char32)"-");
        sb.Append(rCountry);

        String rVariant;
        mRequest->GetVariant(&rVariant);
        cs = NULL;
        CString::New(rVariant, (ICharSequence**)&cs);
        if (!TextUtils::IsEmpty(cs)) {
            sb.AppendChar((Char32)"-");
            sb.Append(rVariant);
        }
    }
    return sb.ToString();
}

ECode EventLoggerV1::LogFailure(
    /* [in] */  Int32 statusCode)
{
//TODO
#if 0
    // We don't report stopped syntheses because their overall
    // total time spent will be inaccurate (will not correlate with
    // the length of the utterance).
    if (statusCode != ITextToSpeech::STOPPED) {
        EventLogTags.writeTtsSpeakFailure(mServiceApp, mCallerUid, mCallerPid,
                getUtteranceLength(), getLocaleString(),
                mRequest.getSpeechRate(), mRequest.getPitch());
    }
#endif
    return NOERROR;
}

ECode EventLoggerV1::LogSuccess(
    /* [in] */ Int64 audioLatency,
    /* [in] */ Int64 engineLatency,
    /* [in] */ Int64 engineTotal)
{
//TODO
#if 0
    EventLogTags.writeTtsSpeakSuccess(mServiceApp, mCallerUid, mCallerPid,
            getUtteranceLength(), getLocaleString(),
            mRequest.getSpeechRate(), mRequest.getPitch(),
            engineLatency, engineTotal, audioLatency);
#endif
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
