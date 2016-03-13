
#ifndef __ELASTOS_DROID_TELEPHONY_PHONENUMBERUTILS_H__
#define __ELASTOS_DROID_TELEPHONY_PHONENUMBERUTILS_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Utility::ILocale;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Telephony {

class PhoneNumberUtils
{
private:
    class CountryCallingCodeAndNewIndex
        : public Object
    {
    public:
        CountryCallingCodeAndNewIndex(
            /* [in] */ Int32 countryCode,
            /* [in] */ Int32 newIndex);

        Int32 mCountryCallingCode;
        Int32 mNewIndex;
    };

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
        /* [in] */ Int32 defaultFormattingType,
        /* [out] */ IEditable** result);

    static CARAPI FormatNanpNumber(
        /* [in] */ IEditable* text,
        /* [out] */ IEditable** result);

    static CARAPI FormatJapaneseNumber(
        /* [in] */ IEditable* text,
        /* [out] */ IEditable** result);

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

    static CARAPI IsEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    static CARAPI IsLocalEmergencyNumber(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsLocalEmergencyNumber(
        /* [in] */ IContext* context,
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialLocalEmergencyNumber(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsPotentialLocalEmergencyNumber(
        /* [in] */ IContext* context,
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsVoiceMailNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI IsVoiceMailNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI ConvertKeypadLettersToDigits(
        /* [in] */ const String& input,
        /* [out] */ String* res);

    static CARAPI CdmaCheckAndProcessPlusCode(
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    static CARAPI CdmaCheckAndProcessPlusCodeForSms(
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    static CARAPI CdmaCheckAndProcessPlusCodeByNumberFormat(
        /* [in] */ const String& dialStr,
        /* [in] */ Int32 currFormat,
        /* [in] */ Int32 defaultFormat,
        /* [out] */ String* res);

    static CARAPI IsNanp(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* res);

    static CARAPI IsUriNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    static CARAPI GetUsernameFromUriNumber(
        /* [in] */ const String& number,
        /* [out] */ String* res);

private:
    static CARAPI IsPause(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI IsToneWait(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI IsSeparator(
        /* [in] */ Char32 ch,
        /* [out] */ Boolean* res);

    static CARAPI MinPositive(
        /* [in] */ Int32 a,
        /* [in] */ Int32  b,
        /* [out] */ Int32* val);

    static CARAPI Log(
        /* [in] */ const String& msg);

    static CARAPI IndexOfLastNetworkChar(
        /* [in] */ const String& a,
        /* [out] */ Int32* val);

    static CARAPI InternalGetStrippedReversed(
        /* [in] */ const String& np,
        /* [in] */ Int32 numDigits,
        /* [out] */ String* res);

    static CARAPI InternalCalledPartyBCDFragmentToString(
        /* [in] */ StringBuilder* sb,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    static CARAPI BcdToChar(
        /* [in] */ Byte b,
        /* [out] */ Char32* res);

    static CARAPI CharToBCD(
        /* [in] */ Char32 c,
        /* [out] */ Int32* res);

    static CARAPI IsDialable(
        /* [in] */ const String& address,
        /* [out] */ Boolean* res);

    static CARAPI IsNonSeparator(
        /* [in] */ const String& address,
        /* [out] */ Boolean* res);

    static CARAPI NumberToCalledPartyBCDHelper(
        /* [in] */ const String& number,
        /* [in] */ Boolean includeLength,
        /* [out, callee] */ ArrayOf<Byte>** res);

    static CARAPI RemoveDashes(
        /* [in] */ IEditable* text,
        /* [out] */ IEditable** result);

    static CARAPI IsEmergencyNumberInternal(
        /* [in] */ const String& number,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumberInternal(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumberInternal(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsEmergencyNumberInternal(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsLocalEmergencyNumberInternal(
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI IsLocalEmergencyNumberInternal(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ IContext* context,
        /* [in] */ Boolean useExactMatch,
        /* [out] */ Boolean* res);

    static CARAPI GetCurrentIdp(
        /* [in] */ Boolean useNanp,
        /* [out] */ String* res);

    static CARAPI IsTwoToNine(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    static CARAPI GetFormatTypeFromCountryCode(
        /* [in] */ const String& country,
        /* [out] */ Int32* res);

    static CARAPI IsOneNanp(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* res);

    static CARAPI ProcessPlusCode(
        /* [in] */ const String& networkDialStr,
        /* [in] */ Boolean useNanp,
        /* [out] */ String* res);

    static CARAPI FindDialableIndexFromPostDialStr(
        /* [in] */ const String& postDialStr,
        /* [out] */ Int32* res);

    static CARAPI AppendPwCharBackToOrigDialStr(
        /* [in] */ Int32 dialableIndex,
        /* [in] */ const String& origStr,
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

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

    static CARAPI TryGetISODigit(
        /* [in] */ Char32 ch,
        /* [out] */ Int32* res);

    static CARAPI TryGetCountryCallingCodeAndNewIndex(
        /* [in] */ const String& str,
        /* [in] */ Boolean acceptThailandCase,
        /* [out] */ CountryCallingCodeAndNewIndex** cccani);

    static CARAPI TryGetTrunkPrefixOmittedIndex(
        /* [in] */ const String& str,
        /* [in] */ Int32 currentIndex,
        /* [out] */ Int32* res);

    static CARAPI CheckPrefixIsIgnorable(
        /* [in] */ const String& str,
        /* [in] */ Int32 forwardIndex,
        /* [in] */ Int32 backwardIndex,
        /* [out] */ Boolean* res);

    static CARAPI GetDefaultVoiceSubId(
        /* [out] */ Int64* res);

private:
    /*
     * Calling Line Identification Restriction (CLIR)
     */
    static const String CLIR_ON;
    static const String CLIR_OFF;

    static const String TAG;
    static const Boolean DBG;

    //================ Plus Code formatting =========================
    static const Char32 PLUS_SIGN_CHAR;
    static const String PLUS_SIGN_STRING;
    static const String NANP_IDP_STRING;
    static const Int32 NANP_LENGTH;

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
    static const Int32 MIN_MATCH;

    /*
     * global-phone-number = ["+"] 1*( DIGIT / written-sep )
     * written-sep         = ("-"/".")
     */
    static const AutoPtr<IPattern> GLOBAL_PHONE_NUMBER_PATTERN;

    /** List of country codes for countries that use the NANP */
    static AutoPtr<ArrayOf<String> > NANP_COUNTRIES;

    static const Int32 NANP_STATE_DIGIT;
    static const Int32 NANP_STATE_PLUS;
    static const Int32 NANP_STATE_ONE;
    static const Int32 NANP_STATE_DASH;

    /**
     * The phone keypad letter mapping (see ITU E.161 or ISO/IEC 9995-8.)
     * TODO: This should come from a resource.
     */
    static AutoPtr<ISparseInt32Array> KEYPAD_MAP;

    static const Boolean COUNTRY_CALLING_CALL[];

    static const Int32 CCC_LENGTH;
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_PHONENUMBERUTILS_H__
