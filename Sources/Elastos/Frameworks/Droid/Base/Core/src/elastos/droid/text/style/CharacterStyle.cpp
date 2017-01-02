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

#include "elastos/droid/text/style/CharacterStyle.h"
#include "elastos/droid/text/style/MetricAffectingSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

//===========================================================
// CharacterStyle
//===========================================================
CAR_INTERFACE_IMPL(CharacterStyle, Object, ICharacterStyle)

CharacterStyle::CharacterStyle()
{
}

CharacterStyle::~CharacterStyle()
{
}

AutoPtr<ICharacterStyle> CharacterStyle::Wrap(
    /* [in] */ ICharacterStyle* cs)
{
    AutoPtr<IMetricAffectingSpan> mas = IMetricAffectingSpan::Probe(cs);
    if (mas != NULL) {
        AutoPtr<ICharacterStyle> ret = new MetricAffectingSpanPassthrough(mas);
        return ret;
    }
    else {
        AutoPtr<ICharacterStyle> ret = new CharacterStylePassthrough(cs);
        return ret;
    }
}

ECode CharacterStyle::GetUnderlying(
    /* [out] */ ICharacterStyle** style)
{
    VALIDATE_NOT_NULL(style)
    *style = this;
    REFCOUNT_ADD(*style);
    return NOERROR;
}

//===========================================================
// CharacterStylePassthrough
//===========================================================
CharacterStylePassthrough::CharacterStylePassthrough(
    /* [in] */ ICharacterStyle* cs)
{
    mStyle = cs;
}

ECode CharacterStylePassthrough::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    return mStyle->UpdateDrawState(tp);
}

ECode CharacterStylePassthrough::GetUnderlying(
    /* [out] */ ICharacterStyle** result)
{
    return mStyle->GetUnderlying(result);
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos