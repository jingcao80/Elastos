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

#include "elastos/droid/text/style/SubscriptSpan.h"
#include "elastos/droid/text/CTextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SubscriptSpan, MetricAffectingSpan, ISubscriptSpan, IParcelableSpan, IParcelable)

SubscriptSpan::SubscriptSpan()
{
}

SubscriptSpan::~SubscriptSpan()
{
}

ECode SubscriptSpan::constructor()
{
    return NOERROR;
}

ECode SubscriptSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SUBSCRIPT_SPAN;
    return NOERROR;
}

ECode SubscriptSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode SubscriptSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode SubscriptSpan::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    Float fAscent;
    Int32 baselineShift;
    tp->GetBaselineShift(&baselineShift);
    IPaint::Probe(tp)->Ascent(&fAscent);
    tp->SetBaselineShift(baselineShift-(Int32) (fAscent / 2));
    return NOERROR;
}

ECode SubscriptSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* tp)
{
    Float fAscent;
    Int32 baselineShift;
    tp->GetBaselineShift(&baselineShift);
    IPaint::Probe(tp)->Ascent(&fAscent);
    tp->SetBaselineShift(baselineShift-(Int32) (fAscent / 2));
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos