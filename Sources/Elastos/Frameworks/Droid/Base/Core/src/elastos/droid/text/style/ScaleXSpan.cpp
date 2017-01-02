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
#include "elastos/droid/text/style/ScaleXSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(ScaleXSpan, MetricAffectingSpan, IScaleXSpan, IParcelableSpan, IParcelable)

ScaleXSpan::ScaleXSpan()
    : mProportion(0)
{}

ScaleXSpan::~ScaleXSpan()
{}

ECode ScaleXSpan::constructor()
{
    return NOERROR;
}

ECode ScaleXSpan::constructor(
    /* [in] */ Float proportion)
{
    mProportion = proportion;
    return NOERROR;
}

ECode ScaleXSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SCALE_X_SPAN;
    return NOERROR;
}

ECode ScaleXSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadFloat(&mProportion);
}

ECode ScaleXSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteFloat(mProportion);
}

ECode ScaleXSpan::GetScaleX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x)
    *x = mProportion;
    return NOERROR;
}

ECode ScaleXSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textScaleX;
    p->GetTextScaleX(&textScaleX);
    p->SetTextScaleX(textScaleX * mProportion);
    return NOERROR;
}

ECode ScaleXSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textScaleX;
    p->GetTextScaleX(&textScaleX);
    p->SetTextScaleX(textScaleX * mProportion);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
