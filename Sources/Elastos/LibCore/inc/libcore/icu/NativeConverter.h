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

#ifndef __LIBCORE_ICU_NATIVECONVERTER_H__
#define __LIBCORE_ICU_NATIVECONVERTER_H__

#include "Object.h"

using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetDecoder;
using Elastos::IO::Charset::ICharsetEncoder;
using Elastos::IO::Charset::ICodingErrorAction;

namespace Libcore {
namespace ICU {

class NativeConverter
{
private:
    NativeConverter(){}

public:
    static CARAPI Decode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inEnd,
        /* [in] */ ArrayOf<Char32>* output,
        /* [in] */ Int32 outEnd,
        /* [in] */ ArrayOf<Int32>* data,
        /* [in] */ Boolean flush,
        /* [out] */ Int32* result);

    static CARAPI Encode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ArrayOf<Char32>* input,
        /* [in] */ Int32 inEnd,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outEnd,
        /* [in] */ ArrayOf<Int32>* data,
        /* [in] */ Boolean flush,
        /* [out] */ Int32* result);

    static CARAPI OpenConverter(
        /* [in] */ const String& charsetName,
        /* [out] */ Int64* value);

    static CARAPI CloseConverter(
        /* [in] */ Int64 converterHandle);

    static CARAPI ResetByteToChar(
        /* [in] */ Int64 converterHandle);

    static CARAPI ResetCharToByte(
        /* [in] */ Int64 converterHandle);

    static CARAPI GetSubstitutionBytes(
        /* [in] */ Int64 converterHandle,
        /* [out, callee] */ ArrayOf<Byte> ** outarray);

    static CARAPI GetMaxBytesPerChar(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Int32* value);

    static CARAPI GetMinBytesPerChar(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Int32* value);

    static CARAPI GetAveBytesPerChar(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Float* value);

    static CARAPI GetAveCharsPerByte(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Float* value);

    static CARAPI Contains(
        /* [in] */ const String& converterName1,
        /* [in] */ const String& converterName2,
        /* [out] */ Boolean* value);

    static CARAPI GetAvailableCharsetNames(
        /* [out, callee] */ ArrayOf<String> ** outarray);

    static CARAPI CharsetForName(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** outset );

    static CARAPI SetCallbackDecode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ICharsetDecoder* decoder);

    static CARAPI SetCallbackEncode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ICharsetEncoder* encoder);

private:
    static CARAPI TranslateCodingErrorAction(
        /* [in] */ ICodingErrorAction* action,
        /* [out] */ Int32* rev);

    static CARAPI SetCallbackDecode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ Int32 onMalformedInput,
        /* [in] */ Int32 onUnmappableInput,
        /* [in] */ const String& subChars);

    static CARAPI SetCallbackEncode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ Int32 onMalformedInput,
        /* [in] */ Int32 onUnmappableInput,
        /* [in] */ ArrayOf<Byte>* subBytes);
};

}    // ICU
}    // Libcore

#endif // __LIBCORE_ICU_NATIVECONVERTER_H__