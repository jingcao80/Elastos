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

#include "CStringTokenizer.h"
#include "CString.h"
#include "StringUtils.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL_2(CStringTokenizer, Object, IStringTokenizer, IEnumeration)

CAR_OBJECT_IMPL(CStringTokenizer)

CStringTokenizer::CStringTokenizer()
    : mReturnDelimiters(FALSE)
    , mPosition(0)
{
}

ECode CStringTokenizer::constructor(
    /* [in] */ const String& string)
{
    return this->constructor(string, String(" \t\n\r\f"), FALSE);
}

ECode CStringTokenizer::constructor(
    /* [in] */ const String& string,
    /* [in] */ const String& delimiters)
{
    return this->constructor(string, delimiters, FALSE);
}

ECode CStringTokenizer::constructor(
    /* [in] */ const String& string,
    /* [in] */ const String& delimiters,
    /* [in] */ Boolean returnDelimiters)
{
    if (string.IsNull()) {
        // throw new NullPointerException("string == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mString = string;
    mDelimiters = delimiters;
    mReturnDelimiters = returnDelimiters;
    mPosition = 0;
    return NOERROR;
}

ECode CStringTokenizer::CountTokens(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 count = 0;
    Boolean inToken = FALSE;
    AutoPtr<ArrayOf<Char32> > char32Array = mString.GetChars();
    for (Int32 i = mPosition, length = mString.GetLength(); i < length; i++) {
        if (mDelimiters.IndexOf((*char32Array)[i], 0) >= 0) {
            if (mReturnDelimiters)
                count++;
            if (inToken) {
                count++;
                inToken = FALSE;
            }
        }
        else {
            inToken = TRUE;
        }
    }
    if (inToken)
        count++;
    *value = count;
    return NOERROR;
}

ECode CStringTokenizer::HasMoreElements(
    /* [out] */ Boolean * value)
{
    return HasMoreTokens(value);
}

ECode CStringTokenizer::HasMoreTokens(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (mDelimiters == NULL) {
        // throw new NullPointerException("mDelimiters == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 length = mString.GetLength();
    if (mPosition < length) {
        if (mReturnDelimiters){
            *value = TRUE; // there is at least one character and even if
            return NOERROR;
        }
        // it is a delimiter it is a token

        // otherwise find a character which is not a delimiter
        AutoPtr<ArrayOf<Char32> > char32Array = mString.GetChars();
        for (Int32 i = mPosition; i < length; i++)
            if (mDelimiters.IndexOf((*char32Array)[i], 0) == -1) {
                *value = TRUE;
                return NOERROR;
            }
    }
    *value = FALSE;
    return NOERROR;
}

ECode CStringTokenizer::GetNextElement(
    /* [out] */ IInterface ** inter)
{
    VALIDATE_NOT_NULL(inter)

    String str;
    FAIL_RETURN(GetNextToken(&str));
    AutoPtr<ICharSequence> sq;
    CString::New(str, (ICharSequence**)&sq);
    *inter = sq;
    REFCOUNT_ADD(*inter)
    return NOERROR;
}

ECode CStringTokenizer::GetNextToken(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    if (mDelimiters == NULL) {
        // throw new NullPointerException("mDelimiters == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 i = mPosition;
    Int32 length = mString.GetLength();

    if (i < length) {
        AutoPtr<ArrayOf<Char32> > char32Array = mString.GetChars();

        if (mReturnDelimiters) {
            if (mDelimiters.IndexOf((*char32Array)[mPosition], 0) >= 0) {
                String outstr("");
                outstr.Append((*char32Array)[mPosition++]);
                *str = outstr;
                return NOERROR;
            }
            for (mPosition++; mPosition < length; mPosition++) {
                if (mDelimiters.IndexOf((*char32Array)[mPosition], 0) >= 0) {
                    *str = mString.Substring(i, mPosition);
                    return NOERROR;
                }
            }
            *str = mString.Substring(i);
            return NOERROR;
        }

        while (i < length && mDelimiters.IndexOf((*char32Array)[i], 0) >= 0) {
            i++;
        }
        mPosition = i;
        if (i < length) {
            for (mPosition++; mPosition < length; mPosition++) {
                if (mDelimiters.IndexOf((*char32Array)[mPosition], 0) >= 0) {
                    *str = mString.Substring(i, mPosition);
                    return NOERROR;
                }
            }
            *str = mString.Substring(i);
            return NOERROR;
        }
    }
    // throw new NoSuchElementException();
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode CStringTokenizer::GetNextToken(
    /* [in] */ const String& delims,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    mDelimiters = delims;
    return GetNextToken(str);
}

} // namespace Utility
} // namespace Elastos
