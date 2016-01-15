
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILSHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILSHELPER_H__

#include "_Elastos_Droid_Telephony_CPhoneNumberUtilsHelper.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Utility::ILocale;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneNumberUtilsHelper)
{
public:

    CARAPI IsISODigit(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI  Is12Key(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI  IsDialable(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI  IsReallyDialable(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI  IsNonSeparator(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI  IsStartsPostDial(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI GetNumberFromIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IContext* context,
        /* [out] */ String* res);

    CARAPI ExtractNetworkPortion(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI ExtractNetworkPortionAlt(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI StripSeparators(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI ConvertAndStrip(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI ConvertPreDial(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI ExtractPostDialPortion(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI Compare(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    CARAPI Compare(
        /* [in] */ IContext* context,
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    CARAPI Compare(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [in] */ Boolean useStrictComparation,
        /* [out] */ Boolean* res);

    CARAPI CompareLoosely(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    CARAPI CompareStrictly(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    CARAPI CompareStrictly(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [in] */ Boolean acceptInvalidCCCPrefix,
        /* [out] */ Boolean* res);

    CARAPI ToCallerIDMinMatch(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI GetStrippedReversed(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI StringFromStringAndTOA(
        /* [in] */ const String& s,
        /* [in] */ Int32 TOA,
        /* [out] */ String* res);

    CARAPI ToaFromString(
        /* [in] */ const String& s,
        /* [out] */ Int32* res);

    CARAPI CalledPartyBCDToString(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* res);

    CARAPI CalledPartyBCDFragmentToString(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* res);

    CARAPI IsWellFormedSmsAddress(
        /* [in] */ const String& address,
        /* [out] */ Boolean* res);

    CARAPI IsGlobalPhoneNumber(
        /* [in] */ const String& phoneNumber,
        /* [out] */ Boolean* res);

    CARAPI NetworkPortionToCalledPartyBCD(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI NetworkPortionToCalledPartyBCDWithLength(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI NumberToCalledPartyBCD(
        /* [in] */ const String& number,
        /* [out, callee] */ ArrayOf<Byte>** res);

    CARAPI FormatNumber(
        /* [in] */ const String& source,
        /* [in] */ Int32 defaultFormattingType,
        /* [out] */ String* res);

    CARAPI GetFormatTypeForLocale(
        /* [in] */ Elastos::Utility::ILocale* locale,
        /* [out] */ Int32* res);

    CARAPI FormatNumber(
        /* [in] */ IEditable* text,
        /* [in] */ Int32 defaultFormattingType);

    CARAPI FormatNanpNumber(
        /* [in] */ IEditable* text);

    CARAPI FormatJapaneseNumber(
        /* [in] */ IEditable* text);

    CARAPI FormatNumberToE164(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ String* res);

    CARAPI FormatNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ String* res);

    CARAPI FormatNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& phoneNumberE164,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ String* res);

    CARAPI NormalizeNumber(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    CARAPI ReplaceUnicodeDigits(
        /* [in] */ const String& number,
        /* [out] */ String* res);

    CARAPI IsEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    CARAPI IsLocalEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialLocalEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* res);

    CARAPI IsVoiceMailNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI ConvertKeypadLettersToDigits(
        /* [in] */ const String& input,
        /* [out] */ String* res);

    CARAPI CdmaCheckAndProcessPlusCode(
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    CARAPI CdmaCheckAndProcessPlusCodeByNumberFormat(
        /* [in] */ const String& dialStr,
        /* [in] */ Int32 currFormat,
        /* [in] */ Int32 defaultFormat,
        /* [out] */ String* res);

    CARAPI IsUriNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI GetUsernameFromUriNumber(
        /* [in] */ const String& number,
        /* [out] */ String* res);

private:

    class CountryCallingCodeAndNewIndex
    : public ElRefBase
    {
        public:
            Int32 mCountryCallingCode;
            Int32 mNewIndex;
            CountryCallingCodeAndNewIndex(
                /* [in] */ Int32 countryCode,
                /* [in] */ Int32 newIndex);
    };

    static CARAPI IsPause(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI IsToneWait(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI IsSeparator(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* res);

    static CARAPI IsDialable(
        /* [in] */ const String& address,
        /* [out] */ Boolean* res);

    static CARAPI IsNonSeparator(
        /* [in] */ const String& address,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumberInternal(
        /* [in] */ const String& number,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumberInternal(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsLocalEmergencyNumberInternal(
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsTwoToNine(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI IsNanp(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* res);

    static CARAPI IsOneNanp(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* res);

    static CARAPI MatchIntlPrefix(
        /* [in] */ const String& a,
        /* [in] */ Int32 len,
        /* [out] */ Boolean* res);

    static CARAPI MatchIntlPrefixAndCC(
        /* [in] */ const String& a,
        /* [in] */ Int32 len,
        /* [out] */ Boolean* res);

    static CARAPI MatchTrunkPrefix(
        /* [in] */ const String& a,
        /* [in] */ Int32 len,
        /* [out] */ Boolean* res);

    static CARAPI IsCountryCallingCode(
        /* [in] */ Int32 countryCallingCodeCandidate,
        /* [out] */ Boolean* res);

    static CARAPI CheckPrefixIsIgnorable(
        /* [in] */ const String& str,
        /* [in] */ Int32 forwardIndex,
        /* [in] */ Int32 backwardIndex,
        /* [out] */ Boolean* res);

    static CARAPI AppendPwCharBackToOrigDialStr(
        /* [in] */ Int32 dialableIndex,
        /* [in] */ const String& origStr,
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    static CARAPI GetDefaultIdp(
        /* [out] */ String* res);

    static CARAPI InternalGetStrippedReversed(
        /* [in] */ const String& np,
        /* [in] */ Int32 numDigits,
        /* [out] */ String* res);

    static CARAPI ProcessPlusCodeWithinNanp(
        /* [in] */ const String& networkDialStr,
        /* [out] */ String* res);

    static CARAPI CharToBCD(
        /* [in] */ Char32 c,
        /* [out] */ Int32* res);

    static CARAPI FindDialableIndexFromPostDialStr(
        /* [in] */ const String& postDialStr,
        /* [out] */ Int32* res);

    static CARAPI GetFormatTypeFromCountryCode(
        /* [in] */ const String& country,
        /* [out] */ Int32* res);

    static CARAPI TryGetISODigit(
        /* [in] */ Char32 ch,
        /* [out] */ Int32* res);

    static CARAPI TryGetTrunkPrefixOmittedIndex(
        /* [in] */ const String& str,
        /* [in] */ Int32 currentIndex,
        /* [out] */ Int32* res);

    static CARAPI InternalCalledPartyBCDFragmentToString(
        /* [in] */ StringBuilder* sb,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI Log(
        /* [in] */ const String& msg);

    static CARAPI RemoveDashes(
        /* [in] */ IEditable* text);

    static CARAPI NumberToCalledPartyBCDHelper(
        /* [in] */ const String& number,
        /* [in] */ Boolean includeLength,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI BcdToChar(
        /* [in] */ Byte b,
        /* [out] */ Char32* res);

    static CARAPI TryGetCountryCallingCodeAndNewIndex(
        /* [in] */ const String& str,
        /* [in] */ Boolean acceptThailandCase,
        /* [out] */ CountryCallingCodeAndNewIndex* cccani);

};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILSHELPER_H__
