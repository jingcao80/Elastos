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

#include "CNativeBreakIteratorHelper.h"
#include "NativeBreakIterator.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CNativeBreakIteratorHelper)

CAR_INTERFACE_IMPL(CNativeBreakIteratorHelper, Singleton, INativeBreakIteratorHelper)

ECode CNativeBreakIteratorHelper::GetCharacterInstance(
    /* [in] */ ILocale * where,
    /* [out] */ INativeBreakIterator ** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<INativeBreakIterator> nbi = NativeBreakIterator::GetCharacterInstance(where);
    *outiter = nbi;
    REFCOUNT_ADD(*outiter);
    return NOERROR;
}

ECode CNativeBreakIteratorHelper::GetLineInstance(
    /* [in] */ ILocale * where,
    /* [out] */ INativeBreakIterator ** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<INativeBreakIterator> nbi = NativeBreakIterator::GetLineInstance(where);
    *outiter = nbi;
    REFCOUNT_ADD(*outiter);
    return NOERROR;
}

ECode CNativeBreakIteratorHelper::GetSentenceInstance(
    /* [in] */ ILocale * where,
    /* [out] */ INativeBreakIterator ** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<INativeBreakIterator> nbi = NativeBreakIterator::GetSentenceInstance(where);
    *outiter = nbi;
    REFCOUNT_ADD(*outiter);
    return NOERROR;
}

ECode CNativeBreakIteratorHelper::GetWordInstance(
    /* [in] */ ILocale * where,
    /* [out] */ INativeBreakIterator ** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    AutoPtr<INativeBreakIterator> nbi = NativeBreakIterator::GetWordInstance(where);
    *outiter = nbi;
    REFCOUNT_ADD(*outiter);
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
