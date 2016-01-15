
#include "CPhoneNumberUtilsHelper.h"
#include "CPhoneNumberUtils.h"

using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Telephony {


ECode CPhoneNumberUtilsHelper::IsISODigit(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsISODigit(c, res);
}

ECode CPhoneNumberUtilsHelper::GetNumberFromIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IContext* context,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::GetNumberFromIntent(intent, context, res);
}

ECode CPhoneNumberUtilsHelper::ExtractNetworkPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ExtractNetworkPortion(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::ExtractNetworkPortionAlt(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ExtractNetworkPortionAlt(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::StripSeparators(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::StripSeparators(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::ConvertAndStrip(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ConvertAndStrip(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::ConvertPreDial(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ConvertPreDial(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::ExtractPostDialPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ExtractPostDialPortion(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::Compare(a, b, res);
}

ECode CPhoneNumberUtilsHelper::Compare(
    /* [in] */ IContext* context,
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::Compare(context, a, b, res);
}

ECode CPhoneNumberUtilsHelper::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean useStrictComparation,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::Compare(a, b, useStrictComparation, res);
}

ECode CPhoneNumberUtilsHelper::CompareLoosely(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::CompareLoosely(a, b, res);
}

ECode CPhoneNumberUtilsHelper::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::CompareStrictly(a, b, res);
}

ECode CPhoneNumberUtilsHelper::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean acceptInvalidCCCPrefix,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::CompareStrictly(a, b, acceptInvalidCCCPrefix, res);
}

ECode CPhoneNumberUtilsHelper::ToCallerIDMinMatch(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ToCallerIDMinMatch(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::GetStrippedReversed(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::GetStrippedReversed(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::StringFromStringAndTOA(
    /* [in] */ const String& s,
    /* [in] */ Int32 TOA,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::StringFromStringAndTOA(s, TOA, res);
}

ECode CPhoneNumberUtilsHelper::ToaFromString(
    /* [in] */ const String& s,
    /* [out] */ Int32* res)
{
    return CPhoneNumberUtils::ToaFromString(s, res);
}

ECode CPhoneNumberUtilsHelper::CalledPartyBCDToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::CalledPartyBCDToString(bytes, offset, length, res);
}

ECode CPhoneNumberUtilsHelper::CalledPartyBCDFragmentToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::CalledPartyBCDFragmentToString(bytes, offset, length, res);
}

ECode CPhoneNumberUtilsHelper::IsWellFormedSmsAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsWellFormedSmsAddress(address, res);
}

ECode CPhoneNumberUtilsHelper::IsGlobalPhoneNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsGlobalPhoneNumber(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::NetworkPortionToCalledPartyBCD(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return CPhoneNumberUtils::NetworkPortionToCalledPartyBCD(s, res);
}

ECode CPhoneNumberUtilsHelper::NetworkPortionToCalledPartyBCDWithLength(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return CPhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(s, res);
}

ECode CPhoneNumberUtilsHelper::NumberToCalledPartyBCD(
    /* [in] */ const String& number,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return CPhoneNumberUtils::NumberToCalledPartyBCD(number, res);
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& source,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::FormatNumber(source, res);
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& source,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::FormatNumber(source, defaultFormattingType, res);
}

ECode CPhoneNumberUtilsHelper::GetFormatTypeForLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Int32* res)
{
    return CPhoneNumberUtils::GetFormatTypeForLocale(locale, res);
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ IEditable* text,
    /* [in] */ Int32 defaultFormattingType)
{
    return CPhoneNumberUtils::FormatNumber(text, defaultFormattingType);
}

ECode CPhoneNumberUtilsHelper::FormatNanpNumber(
    /* [in] */ IEditable* text)
{
    return CPhoneNumberUtils::FormatNanpNumber(text);
}

ECode CPhoneNumberUtilsHelper::FormatJapaneseNumber(
    /* [in] */ IEditable* text)
{
    return CPhoneNumberUtils::FormatJapaneseNumber(text);
}

ECode CPhoneNumberUtilsHelper::FormatNumberToE164(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::FormatNumberToE164(phoneNumber, defaultCountryIso, res);
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::FormatNumber(phoneNumber, defaultCountryIso, res);
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& phoneNumberE164,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::FormatNumber(phoneNumber, phoneNumberE164, defaultCountryIso, res);
}

ECode CPhoneNumberUtilsHelper::NormalizeNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::NormalizeNumber(phoneNumber, res);
}

ECode CPhoneNumberUtilsHelper::ReplaceUnicodeDigits(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ReplaceUnicodeDigits(number, res);
}

ECode CPhoneNumberUtilsHelper::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsEmergencyNumber(number, res);
}

ECode CPhoneNumberUtilsHelper::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsPotentialEmergencyNumber(number, res);
}

ECode CPhoneNumberUtilsHelper::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsEmergencyNumber(number, defaultCountryIso, res);
}

ECode CPhoneNumberUtilsHelper::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsPotentialEmergencyNumber(number, defaultCountryIso, res);
}

ECode CPhoneNumberUtilsHelper::IsLocalEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsLocalEmergencyNumber(number, context, res);
}

ECode CPhoneNumberUtilsHelper::IsPotentialLocalEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsPotentialLocalEmergencyNumber(number, context, res);
}

ECode CPhoneNumberUtilsHelper::IsVoiceMailNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsVoiceMailNumber(number, res);
}

ECode CPhoneNumberUtilsHelper::ConvertKeypadLettersToDigits(
    /* [in] */ const String& input,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::ConvertKeypadLettersToDigits(input, res);
}

ECode CPhoneNumberUtilsHelper::CdmaCheckAndProcessPlusCode(
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::CdmaCheckAndProcessPlusCode(dialStr, res);
}

ECode CPhoneNumberUtilsHelper::CdmaCheckAndProcessPlusCodeByNumberFormat(
    /* [in] */ const String& dialStr,
    /* [in] */ Int32 currFormat,
    /* [in] */ Int32 defaultFormat,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::CdmaCheckAndProcessPlusCodeByNumberFormat(dialStr, currFormat, defaultFormat, res);
}

ECode CPhoneNumberUtilsHelper::IsUriNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return CPhoneNumberUtils::IsUriNumber(number, res);
}

ECode CPhoneNumberUtilsHelper::GetUsernameFromUriNumber(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    return CPhoneNumberUtils::GetUsernameFromUriNumber(number, res);
}

}
}
}

