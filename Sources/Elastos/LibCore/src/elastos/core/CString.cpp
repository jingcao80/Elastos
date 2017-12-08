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

#include "CString.h"
#include "StringBuilder.h"
#include <cutils/log.h>

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_3(CString, Object, ICharSequence, IString, IComparable)

CAR_OBJECT_IMPL(CString)

ECode CString::constructor(
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        // ALOGW("CString: null string when create ICharSequence.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mString = str;
    return NOERROR;
}

ECode CString::GetLength(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mString.GetLength();
    return NOERROR;
}

ECode CString::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);

    if (index < 0 || index >= mString.GetLength()) {
        *c = '\0';
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *c = mString.GetChar(index);

    return NOERROR;
}

ECode CString::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);

    if (start < 0 || start >= end) {
        *csq = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String subStr = mString.Substring(start, end);
    CString::New(subStr, csq);
    return NOERROR;
}

ECode CString::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mString;
    return NOERROR;
}

ECode CString::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(obj)

    if (ICharSequence::Probe(obj) == NULL) {
        return NOERROR;
    }

    String str = Object::ToString(obj);
    *result = mString.Equals(str);
    return NOERROR;
}

ECode CString::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    // BKDR Hash Function
    UInt32 seed = 31; // 31 131 1313 13131 131313 etc..
    UInt32 hash = 0;

    const char *string = mString.string();
    for ( ; *string; ++string) {
        hash = hash * seed + (*string);
    }
    *hashCode = (hash & 0x7FFFFFFF);
    return NOERROR;
}

ECode CString::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(another);

    AutoPtr<ICharSequence> res = ICharSequence::Probe(another);
    if (res == NULL) {
        return E_CLASS_CAST_EXCEPTION;
    }

    String str;
    res->ToString(&str);
    *result = mString.Compare(str);
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
