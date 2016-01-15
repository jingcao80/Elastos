#include "elastos/droid/media/CWebVttParserHelper.h"
#include "elastos/droid/media/WebVttParserHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CWebVttParserHelper)

CAR_INTERFACE_IMPL(CWebVttParserHelper, Singleton, IWebVttParserHelper)

ECode CWebVttParserHelper::ParseFloatPercentage(
    /* [in] */ const String& s,
    /* [out] */ Float* result)
{
    return WebVttParserHelper::ParseFloatPercentage(s, result);
}

ECode CWebVttParserHelper::ParseIntPercentage(
    /* [in] */ const String& s,
    /* [out] */ Int32* result)
{
    return WebVttParserHelper::ParseIntPercentage(s, result);
}

ECode CWebVttParserHelper::ParseTimestampMs(
    /* [in] */ const String& s,
    /* [out] */ Int64* result)
{
    return WebVttParserHelper::ParseTimestampMs(s, result);
}

ECode CWebVttParserHelper::TimeToString(
    /* [in] */ Int64 timeMs,
    /* [out] */ String* result)
{
    return WebVttParserHelper::TimeToString(timeMs, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos