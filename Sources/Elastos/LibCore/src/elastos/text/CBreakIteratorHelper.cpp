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

#include "CBreakIteratorHelper.h"
#include "BreakIterator.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CBreakIteratorHelper, Singleton, IBreakIteratorHelper)

CAR_SINGLETON_IMPL(CBreakIteratorHelper)

ECode CBreakIteratorHelper::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return BreakIterator::GetAvailableLocales(locales);
}

ECode CBreakIteratorHelper::GetCharacterInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetCharacterInstance(instance);
}

ECode CBreakIteratorHelper::GetCharacterInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetCharacterInstance(where,instance);
}

ECode CBreakIteratorHelper::GetLineInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetLineInstance(instance);
}

ECode CBreakIteratorHelper::GetLineInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetLineInstance(where,instance);
}

ECode CBreakIteratorHelper::GetSentenceInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetSentenceInstance(instance);
}

ECode CBreakIteratorHelper::GetSentenceInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetSentenceInstance(where,instance);
}

ECode CBreakIteratorHelper::GetWordInstance(
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetWordInstance(instance);
}

ECode CBreakIteratorHelper::GetWordInstance(
    /* [in] */ ILocale* where,
    /* [out] */ IBreakIterator** instance)
{
    return BreakIterator::GetWordInstance(where,instance);
}

} // namespace Text
} // namespace Elastos