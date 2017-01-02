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

#include <coredef.h>
#include "Elastos.CoreLibrary.Core.h"
#include "NativeNormalizer.h"
#include "ElStringByteSink.h"
#include <unicode/normlzr.h>

namespace Libcore {
namespace ICU {

ECode NativeNormalizer::IsNormalized(
    /* [in] */ ICharSequence* src,
    /* [in] */ Form form,
    /* [out] */ Boolean* isNormalized)
{
    VALIDATE_NOT_NULL(isNormalized);
    String s;
    src->ToString(&s);
    return IsNormalizedImpl(s, ToUNormalizationMode(form),isNormalized);
}

ECode NativeNormalizer::Normalize(
    /* [in] */ ICharSequence* src,
    /* [in] */ Form form,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String s;
    src->ToString(&s);
    return NormalizeImpl(s, ToUNormalizationMode(form),result);
}


Int32 NativeNormalizer::ToUNormalizationMode(
    /* [in] */ Form form)
{
    // Translates Java enum constants to ICU int constants.
    // See UNormalizationMode in "unicode/unorm.h". Stable API since ICU 2.0.
    switch (form) {
        case Form_NFC: return 4;
        case Form_NFD: return 2;
        case Form_NFKC: return 5;
        case Form_NFKD: return 3;
    }
    assert(0);
    return -1;
}

extern ECode maybeThrowIcuException(const char*, UErrorCode errorCode);

ECode NativeNormalizer::NormalizeImpl(
    /* [in] */ const String& src,
    /* [in] */ NormalizerForm form,
    /* [out] */ String* result)
{
    UNormalizationMode mode = static_cast<UNormalizationMode>(form);
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString dst;
    Normalizer::normalize(UnicodeString::fromUTF8(src.string()), mode, 0, dst, status);
    FAIL_RETURN(maybeThrowIcuException("Normalizer::normalize", status));
    if (dst.isBogus()) {
        *result = NULL;
    }
    else {
        String s("");
        ElStringByteSink sink(&s);
        dst.toUTF8(sink);
        *result = s;
    }
    return NOERROR;
}

ECode NativeNormalizer::IsNormalizedImpl(
    /* [in] */ const String& src,
    /* [in] */ NormalizerForm form,
    /* [out] */ Boolean* isNormalized)
{
    UNormalizationMode mode = static_cast<UNormalizationMode>(form);
    UErrorCode status = U_ZERO_ERROR;
    UBool result = Normalizer::isNormalized(UnicodeString::fromUTF8(src.string()), mode, status);
    FAIL_RETURN(maybeThrowIcuException("Normalizer::isNormalized", status));
    *isNormalized = result;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
