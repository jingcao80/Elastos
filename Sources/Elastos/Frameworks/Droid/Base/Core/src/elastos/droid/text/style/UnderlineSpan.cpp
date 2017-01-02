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

#include "elastos/droid/text/style/UnderlineSpan.h"
#include "elastos/droid/text/CTextPaint.h"

using Elastos::Droid::Text::CTextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_4(UnderlineSpan, CharacterStyle, IUnderlineSpan, IUpdateAppearance, IParcelableSpan, IParcelable)

UnderlineSpan::UnderlineSpan()
{
}

UnderlineSpan::~UnderlineSpan()
{
}

ECode UnderlineSpan::constructor()
{
    return NOERROR;
}

ECode UnderlineSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::UNDERLINE_SPAN;
    return NOERROR;
}

ECode UnderlineSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode UnderlineSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode UnderlineSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    CTextPaint* p = (CTextPaint*)ds;
    p->SetUnderlineText(TRUE);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos