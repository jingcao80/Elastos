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

#include "elastos/droid/internal/utility/CCharSequences.h"
#include "elastos/droid/internal/utility/CharSequences.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CCharSequences, Singleton, ICharSequences)

CAR_SINGLETON_IMPL(CCharSequences)

ECode CCharSequences::ForAsciiBytes(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs)
    AutoPtr<ICharSequence> temp = CharSequences::ForAsciiBytes(bytes);
    *cs = temp;
    REFCOUNT_ADD(*cs)
    return NOERROR;
}

ECode CCharSequences::ForAsciiBytes(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** cs)
{
    return CharSequences::ForAsciiBytes(bytes, start, end, cs);
}

ECode CCharSequences::Equals(
    /* [in] */ ICharSequence* a,
    /* [in] */ ICharSequence* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CharSequences::Equals(a, b);
    return NOERROR;
}

ECode CCharSequences::CompareToIgnoreCase(
    /* [in] */ ICharSequence* me,
    /* [in] */ ICharSequence* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = CharSequences::CompareToIgnoreCase(me, another);
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
