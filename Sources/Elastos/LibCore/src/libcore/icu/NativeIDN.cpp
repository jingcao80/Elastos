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

#include "coredef.h"
#include "Elastos.CoreLibrary.h"
#include "NativeIDN.h"
#include "ElStringByteSink.h"
#include <unicode/uidna.h>
#include <unicode/unistr.h>

namespace Libcore {
namespace ICU {

String NativeIDN::ToASCII(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags)
{
    String result;
    Convert(s, flags, TRUE, &result);
    return result;
}

String NativeIDN::ToUnicode(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags)
{
    //try {
    String result;
    if (FAILED(Convert(s, flags, FALSE, &result))) {
        return s;
    }
    return result;
    // } catch (IllegalArgumentException ex) {
    //     // The RI documentation explicitly states that this method can't fail.
    //     // ICU4C disagrees, as does the RI in practice.
    //     // The RI just returns the input string if it can't
    //     return s;
    // }
}

ECode NativeIDN::Convert(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean toAscii,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (s.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
//        throw new NullPointerException("s == null");
    }
    return ConvertImpl(s, flags, toAscii, result);
}

static bool isLabelSeparator(const UChar ch) {
    switch (ch) {
    case 0x3002: // ideographic full stop
    case 0xff0e: // fullwidth full stop
    case 0xff61: // halfwidth ideographic full stop
        return true;
    default:
        return false;
    }
}

ECode NativeIDN::ConvertImpl(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags,
    /* [in] */ Boolean toAscii,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (s.IsNull()) {
        *result = NULL;
        return NOERROR;
    }
    UChar dst[256];
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString unicodeS = UnicodeString::fromUTF8(s.string());
    size_t resultLength = toAscii
        ? uidna_IDNToASCII(unicodeS.getBuffer(), unicodeS.length(), &dst[0], sizeof(dst), flags, NULL, &status)
        : uidna_IDNToUnicode(unicodeS.getBuffer(), unicodeS.length(), &dst[0], sizeof(dst), flags, NULL, &status);
    if (U_FAILURE(status)) {
        *result = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        jniThrowException(env, "java/lang/IllegalArgumentException", u_errorName(status));
    }
    if (!toAscii) {
        // ICU only translates separators to ASCII for toASCII.
        // Java expects the translation for toUnicode too.
        // We may as well do this here, while the string is still mutable.
        for (size_t i = 0; i < resultLength; ++i) {
            if (isLabelSeparator(dst[i])) {
               dst[i] = '.';
            }
        }
    }
    UnicodeString us(&dst[0], resultLength);
    String str("");
    ElStringByteSink sink(&str);
    us.toUTF8(sink);
    *result = s;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
