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

#include "elastos/droid/text/style/StyleSpan.h"
#include "elastos/droid/graphics/CTypeface.h"

using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::CTypeface;
using Elastos::Droid::Text::EIID_IParcelableSpan;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(StyleSpan, MetricAffectingSpan, IStyleSpan, IParcelableSpan, IParcelable)

StyleSpan::StyleSpan()
    : mStyle(0)
{}

StyleSpan::~StyleSpan()
{}

ECode StyleSpan::constructor()
{
    return NOERROR;
}

ECode StyleSpan::constructor(
    /* [in] */ Int32 style)
{
    mStyle = style;
    return NOERROR;
}

ECode StyleSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::STYLE_SPAN;
    return NOERROR;
}

ECode StyleSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt32(&mStyle);
}

ECode StyleSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt32(mStyle);
}

ECode StyleSpan::GetStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style)
    *style = mStyle;
    return NOERROR;
}

ECode StyleSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    return Apply(IPaint::Probe(ds), mStyle);
}

ECode StyleSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* paint)
{
    return Apply(IPaint::Probe(paint), mStyle);
}

ECode StyleSpan::Apply(
    /* [in] */ IPaint* paint,
    /* [in] */ Int32 style)
{
    assert(paint != NULL);
    Int32 oldStyle;

    AutoPtr<ITypeface> old;
    paint->GetTypeface( (ITypeface**)&old );
    if (old == NULL) {
        oldStyle = 0;
    } else {
        old->GetStyle(&oldStyle);
    }

    Int32 want = oldStyle | style;

    AutoPtr<ITypeface> tf;
    if (old == NULL) {
        CTypeface::DefaultFromStyle(want, (ITypeface**)&tf);
    } else {
        CTypeface::Create(old, want, (ITypeface**)&tf);
    }

    Int32 s;
    tf->GetStyle(&s);

    Int32 fake = want & ~s;

    if ((fake & ITypeface::BOLD) != 0) {
        paint->SetFakeBoldText(TRUE);
    }

    if ((fake & ITypeface::ITALIC) != 0) {
        paint->SetTextSkewX(-0.25f);
    }

    paint->SetTypeface(tf);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos