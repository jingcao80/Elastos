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

#include "CIDN.h"
#include <unicode/ucat.h>
#include <unicode/uidna.h>

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CIDN, Singleton, IIDN)

CAR_SINGLETON_IMPL(CIDN)

ECode CIDN::ToASCII(
    /* [in] */ const String& input,
    /* [in] */ Int32 flags,
    /* [out] */ String* result)
{
    return Convert(input, flags, TRUE, result);
}

ECode CIDN::ToASCII(
    /* [in] */ const String& input,
    /* [out] */ String* result)
{
    return ToASCII(input, 0, result);
}

ECode CIDN::ToUnicode(
    /* [in] */ const String& input,
    /* [in] */ Int32 flags,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    // try {
    ECode ec = Convert(input, flags, FALSE, result);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        *result = input;
        return NOERROR;
    }
    return ec;
    // } catch (IllegalArgumentException ex) {
        // The RI documentation explicitly states that this method can't fail.
        // ICU4C disagrees, as does the RI in practice.
        // The RI just returns the input string if it can't
        // return s;
    // }
}

ECode CIDN::ToUnicode(
    /* [in] */ const String& input,
    /* [out] */ String* result)
{
    return ToUnicode(input, 0, result);
}

ECode CIDN::Convert(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean toAscii,
    /* [out] */ String* result)
{
    if (s.IsNull()) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    return NativeConvert(s, flags, toAscii, result);
}

static Boolean IsLabelSeparator(const UChar ch)
{
    switch (ch) {
    case 0x3002: // ideographic full stop
    case 0xff0e: // fullwidth full stop
    case 0xff61: // halfwidth ideographic full stop
        return TRUE;
    default:
        return FALSE;
    }
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

ECode CIDN::NativeConvert(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean toAscii,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    UnicodeString sus = UnicodeString::fromUTF8(s.string());
    const UChar* src = sus.getBuffer();
    const size_t srcLength = sus.length();
    UChar dst[256];
    UErrorCode status = U_ZERO_ERROR;
    size_t resultLength = toAscii
            ? uidna_IDNToASCII(src, srcLength, &dst[0], sizeof(dst), flags, NULL, &status)
            : uidna_IDNToUnicode(src, srcLength, &dst[0], sizeof(dst), flags, NULL, &status);
    if (U_FAILURE(status)) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", u_errorName(status));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!toAscii) {
        // ICU only translates separators to ASCII for toASCII.
        // Java expects the translation for toUnicode too.
        // We may as well do this here, while the string is still mutable.
        for (size_t i = 0; i < resultLength; ++i) {
            if (IsLabelSeparator(dst[i])) {
                dst[i] = '.';
            }
        }
    }

    AutoPtr< ArrayOf<Char32> > outarr = ArrayOf<Char32>::Alloc(resultLength);
    for (size_t i = 0; i < resultLength; ++i) {
        (*outarr)[i] = dst[i];
    }
    *result = String(*outarr);
    return NOERROR;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

} // namespace Net
} // namespace Elastos
