
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILS_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILS_H__

#include "Elastos.Droid.Core_server.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Text::IEditable;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Telephony {

class CPhoneNumberUtils
{
public:

    static CARAPI IsISODigit(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI  Is12Key(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI  IsDialable(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI  IsReallyDialable(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI  IsNonSeparator(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI  IsStartsPostDial(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI GetNumberFromIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IContext* context,
        /* [out] */ String* res);

    static CARAPI ExtractNetworkPortion(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI ExtractNetworkPortionAlt(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI StripSeparators(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI ConvertAndStrip(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI ConvertPreDial(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI ExtractPostDialPortion(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI Compare(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    static CARAPI Compare(
        /* [in] */ IContext* context,
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    static CARAPI Compare(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [in] */ Boolean useStrictComparation,
        /* [out] */ Boolean* res);

    static CARAPI CompareLoosely(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    static CARAPI CompareStrictly(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* res);

    static CARAPI CompareStrictly(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [in] */ Boolean acceptInvalidCCCPrefix,
        /* [out] */ Boolean* res);

    static CARAPI ToCallerIDMinMatch(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI GetStrippedReversed(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI StringFromStringAndTOA(
        /* [in] */ const String& s,
        /* [in] */ Int32 TOA,
        /* [out] */ String* res);

    static CARAPI ToaFromString(
        /* [in] */ const String& s,
        /* [out] */ Int32* res);

    static CARAPI CalledPartyBCDToString(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* res);

    static CARAPI CalledPartyBCDFragmentToString(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* res);

    static CARAPI IsWellFormedSmsAddress(
        /* [in] */ const String& address,
        /* [out] */ Boolean* res);

    static CARAPI IsGlobalPhoneNumber(
        /* [in] */ const String& phoneNumber,
        /* [out] */ Boolean* res);

    static CARAPI NetworkPortionToCalledPartyBCD(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI NetworkPortionToCalledPartyBCDWithLength(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI NumberToCalledPartyBCD(
        /* [in] */ const String& number,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI FormatNumber(
        /* [in] */ const String& source,
        /* [out] */ String* res);

    static CARAPI FormatNumber(
        /* [in] */ const String& source,
        /* [in] */ Int32 defaultFormattingType,
        /* [out] */ String* res);

    static CARAPI GetFormatTypeForLocale(
        /* [in] */ Elastos::Utility::ILocale* locale,
        /* [out] */ Int32* res);

    static CARAPI FormatNumber(
        /* [in] */ IEditable* text,
        /* [in] */ Int32 defaultFormattingType);

    static CARAPI FormatNanpNumber(
        /* [in] */ IEditable* text);

    static CARAPI FormatJapaneseNumber(
        /* [in] */ IEditable* text);

    static CARAPI FormatNumberToE164(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ String* res);

    static CARAPI FormatNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ String* res);

    static CARAPI FormatNumber(
        /* [in] */ const String& phoneNumber,
        /* [in] */ const String& phoneNumberE164,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ String* res);

    static CARAPI NormalizeNumber(
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* res);

    static CARAPI ReplaceUnicodeDigits(
        /* [in] */ const String& number,
        /* [out] */ String* res);

    static CARAPI IsEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    static CARAPI IsLocalEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialLocalEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* res);

    static CARAPI IsVoiceMailNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI ConvertKeypadLettersToDigits(
        /* [in] */ const String& input,
        /* [out] */ String* res);

    static CARAPI CdmaCheckAndProcessPlusCode(
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    static CARAPI CdmaCheckAndProcessPlusCodeByNumberFormat(
        /* [in] */ const String& dialStr,
        /* [in] */ Int32 currFormat,
        /* [in] */ Int32 defaultFormat,
        /* [out] */ String* res);

    static CARAPI IsUriNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI GetUsernameFromUriNumber(
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

    static CARAPI IndexOfLastNetworkChar(
        /* [in] */ const String& a,
        /* [out] */ Int32* val);

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
        /* [out] */ CountryCallingCodeAndNewIndex** cccani);

    static CARAPI MinPositive(
        /* [in] */ Int32 a,
        /* [in] */ Int32  b,
        /* [out] */ Int32* val);


    static const String LOG_TAG;
    //================ Plus Code formatting =========================
    static const Char32 PLUS_SIGN_CHAR = '+';
    static const String PLUS_SIGN_STRING/* = "+"*/;
    static const String NANP_IDP_STRING/* = "011"*/;
    static const Int32 NANP_LENGTH = 10;
    static const Boolean DBG = FALSE;
    /*
     * Calling Line Identification Restriction (CLIR)
     */
    static const String CLIR_ON/* = "*31#"*/;
    static const String CLIR_OFF/* = "#31#"*/;
    // Three and four digit phone numbers for either special services,
    // or 3-6 digit addresses from the network (eg carrier-originated SMS messages) should
    // not match.
    //
    // This constant used to be 5, but SMS short codes has increased in length and
    // can be easily 6 digits now days. Most countries have SMS short code length between
    // 3 to 6 digits. The exceptions are
    //
    // Australia: Short codes are six or eight digits in length, starting with the prefix "19"
    //            followed by an additional four or six digits and two.
    // Czech Republic: Codes are seven digits in length for MO and five (not billed) or
    //            eight (billed) for MT direction
    //
    // see http://en.wikipedia.org/wiki/Short_code#Regional_differences for reference
    //
    // However, in order to loose match 650-555-1212 and 555-1212, we need to set the min match
    // to 7.
    static const Int32 MIN_MATCH = 7;
    /*
     * global-phone-number = ["+"] 1*( DIGIT / written-sep )
     * written-sep         = ("-"/".")
     */
    static const AutoPtr<IPattern> GLOBAL_PHONE_NUMBER_PATTERN;

};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILS_H__
