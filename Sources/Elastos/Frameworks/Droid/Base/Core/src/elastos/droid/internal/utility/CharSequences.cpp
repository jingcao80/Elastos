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

#include "elastos/droid/internal/utility/CharSequences.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CharSequences::AsciiBytesCharSequence1, Object, ICharSequence)

CharSequences::AsciiBytesCharSequence1::AsciiBytesCharSequence1(
    /* [in] */ ArrayOf<Byte>* bytes)
    : mBytes(bytes)
{}

ECode CharSequences::AsciiBytesCharSequence1::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c)
    *c = (Char32)(*mBytes)[index];
    return NOERROR;
}

ECode CharSequences::AsciiBytesCharSequence1::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mBytes->GetLength();
    return NOERROR;
}

ECode CharSequences::AsciiBytesCharSequence1::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs)
    return ForAsciiBytes(mBytes, start, end, cs);
}

ECode CharSequences::AsciiBytesCharSequence1::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(*mBytes);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CharSequences::AsciiBytesCharSequence2, Object, ICharSequence)

CharSequences::AsciiBytesCharSequence2::AsciiBytesCharSequence2(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
    : mBytes(bytes)
    , mStart(start)
    , mEnd(end)
{}

ECode CharSequences::AsciiBytesCharSequence2::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c)
    *c = (Char32)(*mBytes)[index + mStart];
    return NOERROR;
}

ECode CharSequences::AsciiBytesCharSequence2::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    *length = mEnd - mStart;
    return NOERROR;
}

ECode CharSequences::AsciiBytesCharSequence2::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs)
    start -= mStart;
    end -= mStart;
    FAIL_RETURN(Validate(start, end, mEnd - mStart));
    return ForAsciiBytes(mBytes, start, end, cs);
}

ECode CharSequences::AsciiBytesCharSequence2::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(*mBytes, mStart, mEnd - mStart);
    return NOERROR;
}

AutoPtr<ICharSequence> CharSequences::ForAsciiBytes(
     /* [in] */ ArrayOf<Byte>* bytes)
{
    return new AsciiBytesCharSequence1(bytes);
}

ECode CharSequences::ForAsciiBytes(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs)
    FAIL_RETURN(Validate(start, end, bytes->GetLength()));
    *cs = new AsciiBytesCharSequence2(bytes, start, end);
    REFCOUNT_ADD(*cs)
    return NOERROR;
}

ECode CharSequences::Validate(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 length)
{
    if (start < 0) return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    if (end < 0) return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    if (end > length) return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    if (start > end) return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    return NOERROR;
}

Boolean CharSequences::Equals(
     /* [in] */ ICharSequence* a,
     /* [in] */ ICharSequence* b)
{
    if (a == b) {
        return TRUE;
    }
    if (a == NULL || b == NULL) {
        return FALSE;
    }
    Int32 al, bl;
    a->GetLength(&al);
    b->GetLength(&bl);
    if (al != bl) {
        return FALSE;
    }

    Char32 ac, bc;
    for (Int32 i = 0; i < al; i++) {
        a->GetCharAt(i, &ac);
        b->GetCharAt(i, &bc);
        if (ac != bc) {
            return FALSE;
        }
    }
    return TRUE;
}

Int32 CharSequences::CompareToIgnoreCase(
     /* [in] */ ICharSequence* a,
     /* [in] */ ICharSequence* b)
{
    // Code adapted from String#compareTo
    Int32 al, bl;
    a->GetLength(&al);
    b->GetLength(&bl);
    Int32 aPos = 0, bPos = 0, result;
    Int32 end = (al < bl) ? al : bl;

    Char32 ac, bc;
    while (aPos < end) {
        a->GetCharAt(aPos++, &ac);
        b->GetCharAt(bPos++, &bc);
        if ((result = Character::ToLowerCase(ac) - Character::ToLowerCase(bc)) != 0) {
            return result;
        }
    }
    return al - bl;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
