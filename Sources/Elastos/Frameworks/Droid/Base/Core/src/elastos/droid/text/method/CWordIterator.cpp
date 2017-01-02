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

#include "elastos/droid/text/method/CWordIterator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

ECode CWordIterator::SetCharSequence(
    /* [in] */ ICharSequence* charSequence,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    WordIterator::SetCharSequence(charSequence, start, end);
    return NOERROR;
}

ECode CWordIterator::GetBeginning(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::GetBeginning(offset);
    return NOERROR;
}

ECode CWordIterator::GetEnd(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::GetEnd(offset);
    return NOERROR;
}

ECode CWordIterator::Preceding(
    /* [in] */ Int32 position,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::Preceding(position);
    return NOERROR;
}

ECode CWordIterator::Following(
    /* [in] */ Int32 position,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret=WordIterator::Following(position);
    return NOERROR;
}

ECode CWordIterator::constructor()
{
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> locale;
    localeHelp->GetDefault((ILocale**)&locale);

    WordIterator::Init(locale);
    return NOERROR;
}

ECode CWordIterator::constructor(
    /* [in] */ ILocale* locale)
{
    WordIterator::Init(locale);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

