#include "elastos/droid/internal/telephony/CGsmAlphabet.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CGsmAlphabet, Singleton, IGsmAlphabet)

CAR_SINGLETON_IMPL(CGsmAlphabet)

ECode CGsmAlphabet::CharToGsm(
    /* [in] */ Char32 c,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::CharToGsm(c, res);
}

ECode CGsmAlphabet::CharToGsm(
    /* [in] */ Char32 c,
    /* [in] */ Boolean throwException,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::CharToGsm(c, throwException, res);
}

ECode CGsmAlphabet::CharToGsmExtended(
    /* [in] */ Char32 c,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::CharToGsmExtended(c, res);
}

ECode CGsmAlphabet::GsmToChar(
    /* [in] */ Int32 gsmChar,
    /* [out] */ Char32* res)
{
    return GsmAlphabet::GsmToChar(gsmChar, res);
}

ECode CGsmAlphabet::GsmExtendedToChar(
    /* [in] */ Int32 gsmChar,
    /* [out] */ Char32* res)
{
    return GsmAlphabet::GsmExtendedToChar(gsmChar, res);
}

ECode CGsmAlphabet::StringToGsm7BitPackedWithHeader(
    /* [in] */ const String& data,
    /* [in] */ ArrayOf<Byte>* header,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return GsmAlphabet::StringToGsm7BitPackedWithHeader(data, header, res);
}

ECode CGsmAlphabet::StringToGsm7BitPackedWithHeader(
    /* [in] */ const String& data,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return GsmAlphabet::StringToGsm7BitPackedWithHeader(data, header,
            languageTable, languageShiftTable, res);
}

ECode CGsmAlphabet::StringToGsm7BitPacked(
    /* [in] */ const String& data,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return GsmAlphabet::StringToGsm7BitPacked(data, res);
}

ECode CGsmAlphabet::StringToGsm7BitPacked(
    /* [in] */ const String& data,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return GsmAlphabet::StringToGsm7BitPacked(data, languageTable, languageShiftTable, res);
}

ECode CGsmAlphabet::StringToGsm7BitPacked(
    /* [in] */ const String& data,
    /* [in] */ Int32 startingSeptetOffset,
    /* [in] */ Boolean throwException,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return GsmAlphabet::StringToGsm7BitPacked(data, startingSeptetOffset,
            throwException, languageTable, languageShiftTable, res);
}

ECode CGsmAlphabet::Gsm7BitPackedToString(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 lengthSeptets,
    /* [out] */ String* res)
{
    return GsmAlphabet::Gsm7BitPackedToString(pdu, offset, lengthSeptets, res);
}

ECode CGsmAlphabet::Gsm7BitPackedToString(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 lengthSeptets,
    /* [in] */ Int32 numPaddingBits,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 shiftTable,
    /* [out] */ String* res)
{
    return GsmAlphabet::Gsm7BitPackedToString(pdu, offset, lengthSeptets,
            numPaddingBits, languageTable, shiftTable, res);
}

ECode CGsmAlphabet::Gsm8BitUnpackedToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    return GsmAlphabet::Gsm8BitUnpackedToString(data, offset, length, res);
}

ECode CGsmAlphabet::Gsm8BitUnpackedToString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ const String& characterset,
    /* [out] */ String* res)
{
    return GsmAlphabet::Gsm8BitUnpackedToString(data, offset, length, characterset, res);
}

ECode CGsmAlphabet::StringToGsm8BitPacked(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return GsmAlphabet::StringToGsm8BitPacked(s, res);
}

ECode CGsmAlphabet::StringToGsm8BitUnpackedField(
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Byte>* dest,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return GsmAlphabet::StringToGsm8BitUnpackedField(s, dest, offset, length);
}

ECode CGsmAlphabet::CountGsmSeptets(
    /* [in] */ Char32 c,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::CountGsmSeptets(c, res);
}

ECode CGsmAlphabet::CountGsmSeptets(
    /* [in] */ Char32 c,
    /* [in] */ Boolean throwsException,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::CountGsmSeptets(c, throwsException, res);
}

ECode CGsmAlphabet::IsGsmSeptets(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return GsmAlphabet::IsGsmSeptets(c, res);
}

ECode CGsmAlphabet::CountGsmSeptetsUsingTables(
    /* [in] */ ICharSequence* s,
    /* [in] */ Boolean use7bitOnly,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::CountGsmSeptetsUsingTables(s, use7bitOnly, languageTable, languageShiftTable, res);
}

ECode CGsmAlphabet::CountGsmSeptets(
    /* [in] */ ICharSequence* s,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** res)
{
    return GsmAlphabet::CountGsmSeptets(s, use7bitOnly, res);
}

ECode CGsmAlphabet::FindGsmSeptetLimitIndex(
    /* [in] */ const String& s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ Int32 langTable,
    /* [in] */ Int32 langShiftTable,
    /* [out] */ Int32* res)
{
    return GsmAlphabet::FindGsmSeptetLimitIndex(s, start, limit, langTable, langShiftTable, res);
}

ECode CGsmAlphabet::SetEnabledSingleShiftTables(
    /* [in] */ ArrayOf<Int32>* tables)
{
    return GsmAlphabet::SetEnabledSingleShiftTables(tables);
}

ECode CGsmAlphabet::SetEnabledLockingShiftTables(
    /* [in] */ ArrayOf<Int32>* tables)
{
    return GsmAlphabet::SetEnabledLockingShiftTables(tables);
}

ECode CGsmAlphabet::GetEnabledSingleShiftTables(
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    return GsmAlphabet::GetEnabledSingleShiftTables(res);
}

ECode CGsmAlphabet::GetEnabledLockingShiftTables(
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    return GsmAlphabet::GetEnabledLockingShiftTables(res);
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony
