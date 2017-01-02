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
#include "elastos/droid/text/style/QuoteSpan.h"

using Elastos::Droid::Graphics::PaintStyle;
using Elastos::Droid::Graphics::PaintStyle_FILL;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

const Int32 QuoteSpan::STRIPE_WIDTH = 2;
const Int32 QuoteSpan::GAP_WIDTH = 2;

CAR_INTERFACE_IMPL_5(QuoteSpan, Object, IQuoteSpan, ILeadingMarginSpan, IParcelableSpan, IParcelable, IParagraphStyle)

QuoteSpan::QuoteSpan()
    : mColor(0xff0000ff)
{}

QuoteSpan::~QuoteSpan()
{}

ECode QuoteSpan::constructor()
{
    return NOERROR;
}

ECode QuoteSpan::constructor(
    /* [in] */ Int32 color)
{
    mColor = color;
    return NOERROR;
}

ECode QuoteSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::QUOTE_SPAN;
    return NOERROR;
}
ECode QuoteSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt32(&mColor);
}

ECode QuoteSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt32(mColor);
}

ECode QuoteSpan::GetColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mColor;
    return NOERROR;
}

ECode QuoteSpan::GetLeadingMargin(
    /* [in] */ Boolean first,
    /* [out] */ Int32* margin)
{
    VALIDATE_NOT_NULL(margin)
    *margin = STRIPE_WIDTH + GAP_WIDTH;
    return NOERROR;
}

ECode QuoteSpan::DrawLeadingMargin(
    /* [in] */ ICanvas* c,
    /* [in] */ IPaint* p,
    /* [in] */ Int32 x,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 top,
    /* [in] */ Int32 baseline,
    /* [in] */ Int32 bottom,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean first,
    /* [in] */ ILayout* layout)
{
    PaintStyle style;
    p->GetStyle(&style);
    Int32 color;
    p->GetColor(&color);

    p->SetStyle(PaintStyle_FILL);
    p->SetColor(mColor);

    c->DrawRect(x, top, x + dir * STRIPE_WIDTH, bottom, p);

    p->SetStyle(style);
    p->SetColor(color);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
