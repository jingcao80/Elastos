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

#include "elastos/droid/text/style/SpellCheckSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SpellCheckSpan, Object, ISpellCheckSpan, IParcelableSpan, IParcelable)

SpellCheckSpan::SpellCheckSpan()
    : mSpellCheckInProgress(FALSE)
{
}

SpellCheckSpan::~SpellCheckSpan()
{
}

ECode SpellCheckSpan::constructor()
{
    return NOERROR;
}

ECode SpellCheckSpan::SetSpellCheckInProgress(
    /* [in] */ Boolean inProgress)
{
    mSpellCheckInProgress = inProgress;
    return NOERROR;
}

ECode SpellCheckSpan::IsSpellCheckInProgress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSpellCheckInProgress;
    return NOERROR;
}

ECode SpellCheckSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadBoolean(&mSpellCheckInProgress);
}

ECode SpellCheckSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteBoolean(mSpellCheckInProgress);
}

ECode SpellCheckSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SPELL_CHECK_SPAN;
    return NOERROR;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos