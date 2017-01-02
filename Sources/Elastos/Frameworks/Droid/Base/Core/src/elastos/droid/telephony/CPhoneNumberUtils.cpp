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

#include "elastos/droid/telephony/CPhoneNumberUtils.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CPhoneNumberUtils, Singleton, IPhoneNumberUtils)

CAR_SINGLETON_IMPL(CPhoneNumberUtils)

ECode CPhoneNumberUtils::IsISODigit (
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsISODigit(c, res);
}

ECode CPhoneNumberUtils::Is12Key(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::Is12Key(c, res);
}

ECode CPhoneNumberUtils::IsDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsDialable(c, res);
}

ECode CPhoneNumberUtils::IsReallyDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsReallyDialable(c, res);
}

ECode CPhoneNumberUtils::IsNonSeparator(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsNonSeparator(c, res);
}

ECode CPhoneNumberUtils::IsStartsPostDial(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsStartsPostDial(c, res);
}

ECode CPhoneNumberUtils::GetNumberFromIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IContext* ctx,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::GetNumberFromIntent(intent, ctx, res);
}

ECode CPhoneNumberUtils::ExtractNetworkPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ExtractNetworkPortion(phoneNumber, res);
}

ECode CPhoneNumberUtils::ExtractNetworkPortionAlt(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ExtractNetworkPortionAlt(phoneNumber, res);
}

ECode CPhoneNumberUtils::StripSeparators(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::StripSeparators(phoneNumber, res);
}

ECode CPhoneNumberUtils::ConvertAndStrip(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ConvertAndStrip(phoneNumber, res);
}

ECode CPhoneNumberUtils::ConvertPreDial(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ConvertPreDial(phoneNumber, res);
}

ECode CPhoneNumberUtils::ExtractPostDialPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ExtractPostDialPortion(phoneNumber, res);
}

ECode CPhoneNumberUtils::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::Compare(a, b, res);
}

ECode CPhoneNumberUtils::Compare(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::Compare(ctx, a, b, res);
}

ECode CPhoneNumberUtils::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean useStrictComparation,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::Compare(a, b, useStrictComparation, res);
}

ECode CPhoneNumberUtils::CompareLoosely(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::CompareLoosely(a, b, res);
}

ECode CPhoneNumberUtils::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::CompareStrictly(a, b, res);
}

ECode CPhoneNumberUtils::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean acceptInvalidCCCPrefix,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::CompareStrictly(a, b, acceptInvalidCCCPrefix, res);
}

ECode CPhoneNumberUtils::ToCallerIDMinMatch(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ToCallerIDMinMatch(phoneNumber, res);
}

ECode CPhoneNumberUtils::GetStrippedReversed(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::GetStrippedReversed(phoneNumber, res);
}

ECode CPhoneNumberUtils::StringFromStringAndTOA(
    /* [in] */ const String& s,
    /* [in] */ Int32 TOA,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::StringFromStringAndTOA(s, TOA, res);
}

ECode CPhoneNumberUtils::ToaFromString(
    /* [in] */ const String& s,
    /* [out] */ Int32* res)
{
    return PhoneNumberUtils::ToaFromString(s, res);
}

ECode CPhoneNumberUtils::CalledPartyBCDToString (
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::CalledPartyBCDToString(bytes, offset, length, res);
}

ECode CPhoneNumberUtils::CalledPartyBCDFragmentToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::CalledPartyBCDFragmentToString(bytes, offset, length, res);
}

ECode CPhoneNumberUtils::IsWellFormedSmsAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsWellFormedSmsAddress(address, res);
}

ECode CPhoneNumberUtils::IsGlobalPhoneNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsGlobalPhoneNumber(phoneNumber, res);
}

ECode CPhoneNumberUtils::NetworkPortionToCalledPartyBCD(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return PhoneNumberUtils::NetworkPortionToCalledPartyBCD(s, res);
}

ECode CPhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return PhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(s, res);
}

ECode CPhoneNumberUtils::NumberToCalledPartyBCD(
    /* [in] */ const String& number,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return PhoneNumberUtils::NumberToCalledPartyBCD(number, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& source,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::FormatNumber(source, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& source,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::FormatNumber(source, defaultFormattingType, res);
}

ECode CPhoneNumberUtils::GetFormatTypeForLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Int32* res)
{
    return PhoneNumberUtils::GetFormatTypeForLocale(locale, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ IEditable* text,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ IEditable** result)
{
    return PhoneNumberUtils::FormatNumber(text, defaultFormattingType, result);
}

ECode CPhoneNumberUtils::FormatNanpNumber(
    /* [in] */ IEditable* text,
    /* [out] */ IEditable** result)
{
    return PhoneNumberUtils::FormatNanpNumber(text, result);
}

ECode CPhoneNumberUtils::FormatJapaneseNumber(
    /* [in] */ IEditable* text,
    /* [out] */ IEditable** result)
{
    return PhoneNumberUtils::FormatJapaneseNumber(text, result);
}

ECode CPhoneNumberUtils::FormatNumberToE164(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::FormatNumberToE164(phoneNumber, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::FormatNumber(phoneNumber, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& phoneNumberE164,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::FormatNumber(phoneNumber, phoneNumberE164, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::NormalizeNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::NormalizeNumber(phoneNumber, res);
}

ECode CPhoneNumberUtils::ReplaceUnicodeDigits(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ReplaceUnicodeDigits(number, res);
}

ECode CPhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsEmergencyNumber(number, res);
}

ECode CPhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsEmergencyNumber(subId, number, res);
}

ECode CPhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsPotentialEmergencyNumber(number, res);
}

ECode CPhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsPotentialEmergencyNumber(subId, number, res);
}

ECode CPhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsEmergencyNumber(number, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsEmergencyNumber(subId, number, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsPotentialEmergencyNumber(number, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsPotentialEmergencyNumber(subId, number, defaultCountryIso, res);
}

ECode CPhoneNumberUtils::IsLocalEmergencyNumber(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsLocalEmergencyNumber(ctx, number, res);
}

ECode CPhoneNumberUtils::IsLocalEmergencyNumber(
    /* [in] */ IContext* ctx,
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsLocalEmergencyNumber(ctx, subId, number, res);
}

ECode CPhoneNumberUtils::IsPotentialLocalEmergencyNumber(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsPotentialLocalEmergencyNumber(ctx, number, res);
}

ECode CPhoneNumberUtils::IsPotentialLocalEmergencyNumber(
    /* [in] */ IContext* ctx,
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsPotentialLocalEmergencyNumber(ctx, subId, number, res);
}

ECode CPhoneNumberUtils::IsVoiceMailNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsVoiceMailNumber(number, res);
}

ECode CPhoneNumberUtils::IsVoiceMailNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsVoiceMailNumber(subId, number, res);
}

ECode CPhoneNumberUtils::ConvertKeypadLettersToDigits(
    /* [in] */ const String& input,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::ConvertKeypadLettersToDigits(input, res);
}

ECode CPhoneNumberUtils::CdmaCheckAndProcessPlusCode(
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::CdmaCheckAndProcessPlusCode(dialStr, res);
}

ECode CPhoneNumberUtils::CdmaCheckAndProcessPlusCodeForSms(
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::CdmaCheckAndProcessPlusCodeForSms(dialStr, res);
}

ECode CPhoneNumberUtils::CdmaCheckAndProcessPlusCodeByNumberFormat(
    /* [in] */ const String& dialStr,
    /* [in] */ Int32 currFormat,
    /* [in] */ Int32 defaultFormat,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::CdmaCheckAndProcessPlusCodeByNumberFormat(
    		dialStr, currFormat, defaultFormat, res);
}

ECode CPhoneNumberUtils::IsNanp(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsNanp(dialStr, res);
}

ECode CPhoneNumberUtils::IsUriNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return PhoneNumberUtils::IsUriNumber(number, res);
}

ECode CPhoneNumberUtils::GetUsernameFromUriNumber(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    return PhoneNumberUtils::GetUsernameFromUriNumber(number, res);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos