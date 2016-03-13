
#ifndef __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILS_H__
#define __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILS_H__

#include "_Elastos_Droid_Telephony_CPhoneNumberUtils.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Text::IEditable;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPhoneNumberUtils)
    , public Singleton
    , public IPhoneNumberUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsISODigit (
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI Is12Key(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI IsDialable(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI IsReallyDialable(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI IsNonSeparator(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI IsStartsPostDial(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* res);

    CARAPI GetNumberFromIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IContext* ctx,
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
        /* [in] */ IContext* ctx,
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

    CARAPI CalledPartyBCDToString (
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
        /* [out] */ String* res);

    CARAPI FormatNumber(
        /* [in] */ const String& source,
        /* [in] */ Int32 defaultFormattingType,
        /* [out] */ String* res);

    CARAPI GetFormatTypeForLocale(
        /* [in] */ ILocale* locale,
        /* [out] */ Int32* res);

    CARAPI FormatNumber(
        /* [in] */ IEditable* text,
        /* [in] */ Int32 defaultFormattingType,
        /* [out] */ IEditable** result);

    CARAPI FormatNanpNumber(
        /* [in] */ IEditable* text,
        /* [out] */ IEditable** result);

    CARAPI FormatJapaneseNumber(
        /* [in] */ IEditable* text,
        /* [out] */ IEditable** result);

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

    CARAPI IsEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    CARAPI IsEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialEmergencyNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [in] */ const String& defaultCountryIso,
        /* [out] */ Boolean* res);

    CARAPI IsLocalEmergencyNumber(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsLocalEmergencyNumber(
        /* [in] */ IContext* ctx,
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialLocalEmergencyNumber(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsPotentialLocalEmergencyNumber(
        /* [in] */ IContext* ctx,
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsVoiceMailNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI IsVoiceMailNumber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI ConvertKeypadLettersToDigits(
        /* [in] */ const String& input,
        /* [out] */ String* res);

    CARAPI CdmaCheckAndProcessPlusCode(
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    CARAPI CdmaCheckAndProcessPlusCodeForSms(
        /* [in] */ const String& dialStr,
        /* [out] */ String* res);

    CARAPI CdmaCheckAndProcessPlusCodeByNumberFormat(
        /* [in] */ const String& dialStr,
        /* [in] */ Int32 currFormat,
        /* [in] */ Int32 defaultFormat,
        /* [out] */ String* res);

    CARAPI IsNanp(
        /* [in] */ const String& dialStr,
        /* [out] */ Boolean* res);

    CARAPI IsUriNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* res);

    CARAPI GetUsernameFromUriNumber(
        /* [in] */ const String& number,
        /* [out] */ String* res);
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CPHONENUMBERUTILS_H__
