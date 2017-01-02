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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/RelativeSizeSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(RelativeSizeSpan, MetricAffectingSpan, IRelativeSizeSpan, IParcelableSpan, IParcelable)

RelativeSizeSpan::RelativeSizeSpan()
    : mProportion(0)
{}

RelativeSizeSpan::~RelativeSizeSpan()
{}

ECode RelativeSizeSpan::constructor()
{
    return NOERROR;
}

ECode RelativeSizeSpan::constructor(
    /* [in] */ Float proportion)
{
    mProportion = proportion;
    return NOERROR;
}

ECode RelativeSizeSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::RELATIVE_SIZE_SPAN;
    return NOERROR;
}

ECode RelativeSizeSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadFloat(&mProportion);
}

ECode RelativeSizeSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteFloat(mProportion);
}

ECode RelativeSizeSpan::GetSizeChange(
    /* [out] */ Float* change)
{
    VALIDATE_NOT_NULL(change)
    *change = mProportion;
    return NOERROR;
}

ECode RelativeSizeSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textSize;
    p->GetTextSize(&textSize);
    p->SetTextSize(textSize * mProportion);
    return NOERROR;
}

ECode RelativeSizeSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textSize;
    p->GetTextSize(&textSize);
    p->SetTextSize(textSize * mProportion);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastoss
