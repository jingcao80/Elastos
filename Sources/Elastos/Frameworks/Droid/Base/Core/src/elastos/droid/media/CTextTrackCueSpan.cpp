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