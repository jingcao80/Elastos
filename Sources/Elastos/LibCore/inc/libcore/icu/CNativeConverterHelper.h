
#ifndef __LIBCORE_ICU_CNATIVECONVERTERHELPER_H__
#define __LIBCORE_ICU_CNATIVECONVERTERHELPER_H__

#include "Singleton.h"
#include "_Libcore_ICU_CNativeConverterHelper.h"

using Elastos::Core::Singleton;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetDecoder;
using Elastos::IO::Charset::ICharsetEncoder;

namespace Libcore {
namespace ICU {

CarClass(CNativeConverterHelper)
    , public Singleton
    , public INativeConverterHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Decode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inEnd,
        /* [in] */ ArrayOf<Char32>* output,
        /* [in] */ Int32 outEnd,
        /* [in] */ ArrayOf<Int32>* data,
        /* [in] */ Boolean flush,
        /* [out] */ Int32* result);

    CARAPI Encode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ArrayOf<Char32>* input,
        /* [in] */ Int32 inEnd,
        /* [in] */ ArrayOf<Byte>* output,
        /* [in] */ Int32 outEnd,
        /* [in] */ ArrayOf<Int32>* data,
        /* [in] */ Boolean flush,
        /* [out] */ Int32* result);

    CARAPI OpenConverter(
        /* [in] */ const String& charsetName,
        /* [out] */ Int64* value);

    CARAPI CloseConverter(
        /* [in] */ Int64 converterHandle);

    CARAPI ResetByteToChar(
        /* [in] */ Int64 converterHandle);

    CARAPI ResetCharToByte(
        /* [in] */ Int64 converterHandle);

    CARAPI GetSubstitutionBytes(
        /* [in] */ Int64 converterHandle,
        /* [out, callee] */ ArrayOf<Byte> ** outarray);

    CARAPI GetMaxBytesPerChar(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Int32* value);

    CARAPI GetMinBytesPerChar(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Int32* value);

    CARAPI GetAveBytesPerChar(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Float* value);

    CARAPI GetAveCharsPerByte(
        /* [in] */ Int64 converterHandle,
        /* [out] */ Float* value);

    CARAPI Contains(
        /* [in] */ const String& converterName1,
        /* [in] */ const String& converterName2,
        /* [out] */ Boolean* value);

    CARAPI GetAvailableCharsetNames(
        /* [out, callee] */ ArrayOf<String> ** outarray);

    CARAPI CharsetForName(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** outset);

    CARAPI SetCallbackDecode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ICharsetDecoder* decoder);

    CARAPI SetCallbackEncode(
        /* [in] */ Int64 converterHandle,
        /* [in] */ ICharsetEncoder* encoder);

};

}    // ICU
}    // Libcore

#endif // __LIBCORE_ICU_CNATIVECONVERTERHELPER_H__