
#include "CNativeConverterHelper.h"
#include "NativeConverter.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CNativeConverterHelper)

CAR_INTERFACE_IMPL(CNativeConverterHelper, Singleton, INativeConverterHelper)

ECode CNativeConverterHelper::Decode(
    /* [in] */ Int64 converterHandle,
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inEnd,
    /* [in] */ ArrayOf<Char32>* output,
    /* [in] */ Int32 outEnd,
    /* [in] */ ArrayOf<Int32>* data,
    /* [in] */ Boolean flush,
    /* [out] */ Int32* result)
{
    return NativeConverter::Decode(converterHandle, input, inEnd, output, outEnd, data, flush, result);
}

ECode CNativeConverterHelper::Encode(
    /* [in] */ Int64 converterHandle,
    /* [in] */ ArrayOf<Char32>* input,
    /* [in] */ Int32 inEnd,
    /* [in] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outEnd,
    /* [in] */ ArrayOf<Int32>* data,
    /* [in] */ Boolean flush,
    /* [out] */ Int32* result)
{
    return NativeConverter::Encode(converterHandle, input, inEnd, output, outEnd, data, flush, result);
}

ECode CNativeConverterHelper::OpenConverter(
    /* [in] */ const String& charsetName,
    /* [out] */ Int64* value)
{
    return NativeConverter::OpenConverter(charsetName, value);
}

ECode CNativeConverterHelper::CloseConverter(
    /* [in] */ Int64 converterHandle)
{
    return NativeConverter::CloseConverter(converterHandle);
}

ECode CNativeConverterHelper::ResetByteToChar(
    /* [in] */ Int64 converterHandle)
{
    return NativeConverter::ResetByteToChar(converterHandle);
}

ECode CNativeConverterHelper::ResetCharToByte(
    /* [in] */ Int64 converterHandle)
{
    return NativeConverter::ResetCharToByte(converterHandle);
}

ECode CNativeConverterHelper::GetSubstitutionBytes(
    /* [in] */ Int64 converterHandle,
    /* [out, callee] */ ArrayOf<Byte>** outarray)
{
    return NativeConverter::GetSubstitutionBytes(converterHandle, outarray);
}

ECode CNativeConverterHelper::GetMaxBytesPerChar(
    /* [in] */ Int64 converterHandle,
    /* [out] */ Int32* value)
{
    return NativeConverter::GetMaxBytesPerChar(converterHandle, value);
}

ECode CNativeConverterHelper::GetMinBytesPerChar(
    /* [in] */ Int64 converterHandle,
    /* [out] */ Int32* value)
{
    return NativeConverter::GetMinBytesPerChar(converterHandle, value);
}

ECode CNativeConverterHelper::GetAveBytesPerChar(
    /* [in] */ Int64 converterHandle,
    /* [out] */ Float* value)
{
    return NativeConverter::GetAveBytesPerChar(converterHandle, value);
}

ECode CNativeConverterHelper::GetAveCharsPerByte(
    /* [in] */ Int64 converterHandle,
    /* [out] */ Float* value)
{
    return NativeConverter::GetAveCharsPerByte(converterHandle, value);
}

ECode CNativeConverterHelper::Contains(
    /* [in] */ const String& converterName1,
    /* [in] */ const String& converterName2,
    /* [out] */ Boolean* value)
{
    return NativeConverter::Contains(converterName1, converterName2, value);
}

ECode CNativeConverterHelper::GetAvailableCharsetNames(
    /* [out, callee] */ ArrayOf<String> ** outarray)
{
    return NativeConverter::GetAvailableCharsetNames(outarray);
}

ECode CNativeConverterHelper::CharsetForName(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** outset)
{
    return NativeConverter::CharsetForName(charsetName, outset);
}

ECode CNativeConverterHelper::SetCallbackDecode(
    /* [in] */ Int64 converterHandle,
    /* [in] */ ICharsetDecoder* decoder)
{
    return NativeConverter::SetCallbackDecode(converterHandle, decoder);
}

ECode CNativeConverterHelper::SetCallbackEncode(
    /* [in] */ Int64 converterHandle,
    /* [in] */ ICharsetEncoder* encoder)
{
    return NativeConverter::SetCallbackEncode(converterHandle, encoder);
}

} // ICU
} // Libcore
