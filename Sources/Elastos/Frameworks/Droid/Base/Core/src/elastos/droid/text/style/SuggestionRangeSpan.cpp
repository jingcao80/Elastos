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

#include "elastos/droid/text/style/SuggestionRangeSpan.h"
#include "elastos/droid/text/CTextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SuggestionRangeSpan, CharacterStyle, ISuggestionRangeSpan, IParcelableSpan, IParcelable)

SuggestionRangeSpan::SuggestionRangeSpan()
    : mBackgroundColor(0) // 0 is a fully transparent black. Has to be set using #setBackgroundColor
{
}

SuggestionRangeSpan::~SuggestionRangeSpan()
{}

ECode SuggestionRangeSpan::constructor()
{
    return NOERROR;
}

ECode SuggestionRangeSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt32(&mBackgroundColor);
}

ECode SuggestionRangeSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt32(mBackgroundColor);
}

ECode SuggestionRangeSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SUGGESTION_RANGE_SPAN;
    return NOERROR;
}

ECode SuggestionRangeSpan::SetBackgroundColor(
    /* [in] */ Int32 backgroundColor)
{
    mBackgroundColor = backgroundColor;
    return NOERROR;
}

ECode SuggestionRangeSpan::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    tp->SetBgColor(mBackgroundColor);
    return NOERROR;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos