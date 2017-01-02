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

#include "elastos/droid/text/style/SuperscriptSpan.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Text::CTextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SuperscriptSpan, MetricAffectingSpan, ISuperscriptSpan, IParcelableSpan, IParcelable)

SuperscriptSpan::SuperscriptSpan()
{
}

SuperscriptSpan::~SuperscriptSpan()
{
}

ECode SuperscriptSpan::constructor()
{
    return NOERROR;
}

ECode SuperscriptSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SUPERSCRIPT_SPAN;
    return NOERROR;
}

ECode SuperscriptSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode SuperscriptSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode SuperscriptSpan::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    CTextPaint* p = (CTextPaint*)tp;
    Float fAscent;
    Int32 baselineShift;
    p->GetBaselineShift(&baselineShift);
    p->Ascent(&fAscent);
    p->SetBaselineShift(baselineShift + (Int32)(fAscent / 2));
    return NOERROR;
}

ECode SuperscriptSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    CTextPaint* p = (CTextPaint*)tp;
    Float fAscent;
    Int32 baselineShift;
    p->GetBaselineShift(&baselineShift);
    p->Ascent(&fAscent);
    p->SetBaselineShift(baselineShift + (Int32)(fAscent / 2));
    return NOERROR;}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos