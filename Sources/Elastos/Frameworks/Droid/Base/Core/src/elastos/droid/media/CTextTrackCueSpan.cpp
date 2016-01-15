#include "elastos/droid/media/CTextTrackCueSpan.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_OBJECT_IMPL(CTextTrackCueSpan)

CAR_INTERFACE_IMPL(CTextTrackCueSpan, Object, ITextTrackCueSpan)

CTextTrackCueSpan::CTextTrackCueSpan()
    : mTimestampMs(0)
    , mEnabled(FALSE)
{}

CTextTrackCueSpan::~CTextTrackCueSpan()
{}

ECode CTextTrackCueSpan::constructor(
    /* [in] */ const String& text,
    /* [in] */ Int64 timestamp)
{
    mTimestampMs = timestamp;
    mText = text;
    mEnabled = (mTimestampMs < 0);
    return NOERROR;
}

ECode CTextTrackCueSpan::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (ITextTrackCueSpan::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ITextTrackCueSpan> span = ITextTrackCueSpan::Probe(o);
    Int64 mts;
    span->GetTimeStampMs(&mts);
    String text;
    span->GetText(&text);
    *result = (mTimestampMs == mts &&
            mText.Equals(text));
    return NOERROR;
}

ECode CTextTrackCueSpan::GetTimeStampMs(
    /* [out] */ Int64* timestampMs)
{
    VALIDATE_NOT_NULL(timestampMs);
    *timestampMs = mTimestampMs;
    return NOERROR;
}

ECode CTextTrackCueSpan::SetTimeStampMs(
    /* [in] */ Int64 timestampMs)
{
    mTimestampMs = timestampMs;
    return NOERROR;
}

ECode CTextTrackCueSpan::GetText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mText;
    return NOERROR;
}

ECode CTextTrackCueSpan::SetText(
    /* [in] */ const String& result)
{
    mText = result;
    return NOERROR;
}

ECode CTextTrackCueSpan::GetEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEnabled;
    return NOERROR;
}

ECode CTextTrackCueSpan::SetEnabled(
    /* [in] */ Boolean result)
{
    mEnabled = result;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos