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

#include "CharsetICU.h"
#include "CharsetEncoderICU.h"
#include "CharsetDecoderICU.h"
#include "NativeConverter.h"

using Libcore::ICU::NativeConverter;

namespace Elastos {
namespace IO {
namespace Charset {

CharsetICU::CharsetICU()
{}

ECode CharsetICU::constructor(
    /* [in] */ const String& canonicalName,
    /* [in] */ const String& icuCanonName,
    /* [in] */ ArrayOf<String>* aliases)
{
    mIcuCanonicalName = icuCanonName;
    return Charset::constructor(canonicalName, aliases);
}

ECode CharsetICU::NewEncoder(
    /* [out] */ ICharsetEncoder** charsetEncoder)
{
    VALIDATE_NOT_NULL(charsetEncoder);
    *charsetEncoder = NULL;

    AutoPtr<CharsetEncoderICU> encoderICU;
    FAIL_RETURN(CharsetEncoderICU::NewInstance(this, mIcuCanonicalName, (CharsetEncoderICU**)&encoderICU));
    *charsetEncoder = ICharsetEncoder::Probe(encoderICU.Get());
    REFCOUNT_ADD(*charsetEncoder);
    return NOERROR;
}

ECode CharsetICU::NewDecoder(
    /* [out] */ ICharsetDecoder** charsetDecoder)
{
    VALIDATE_NOT_NULL(charsetDecoder);
    *charsetDecoder = NULL;

    AutoPtr<CharsetDecoderICU> decoderICU;
    FAIL_RETURN(CharsetDecoderICU::NewInstance(this, mIcuCanonicalName, (CharsetDecoderICU**)&decoderICU));
    *charsetDecoder = ICharsetDecoder::Probe(decoderICU.Get());
    REFCOUNT_ADD(*charsetDecoder);
    return NOERROR;
}

ECode CharsetICU::Contains(
    /* [in] */ ICharset* charset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (charset == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    else if (this == charset) {
        *result = TRUE;
        return NOERROR;
    }

    String temp;
    charset->GetName(&temp);
    return NativeConverter::Contains(mIcuCanonicalName, temp, result);
}

ECode CharsetICU::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mCanonicalName;
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
