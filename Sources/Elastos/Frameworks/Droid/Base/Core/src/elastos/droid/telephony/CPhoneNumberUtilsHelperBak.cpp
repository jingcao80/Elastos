
#include "CPhoneNumberUtilsHelper.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Provider::IContactsContract;

namespace Elastos {
namespace Droid {
namespace Telephony {


ECode CPhoneNumberUtilsHelper::IsISODigit(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c >= '0' && c <= '9');
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::Is12Key(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#');
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+' || c == IPhoneNumberUtils::WILD);
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsReallyDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+');
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsNonSeparator(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+'
                    || c == IPhoneNumberUtils::WILD || c == IPhoneNumberUtils::WAIT || c == IPhoneNumberUtils::PAUSE);
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsStartsPostDial(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c== IPhoneNumberUtils::PAUSE || c == IPhoneNumberUtils::WAIT);
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::GetNumberFromIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IContext* context,
    /* [out] */ String* res)
{
    // VALIDATE_NOT_NULL(res);
    // String number;

    // AutoPtr<IUri> uri;
    // intent->GetData((IUri**)&uri);
    // String scheme;
    // uri->GetScheme(&scheme);

    // if (scheme.Equals(String("tel")) || scheme.Equals(String("sip"))) {
    //     uri->GetSchemeSpecificPart(res);
    //     return NOERROR;
    // }

    // // TODO: We don't check for SecurityException here (requires
    // // CALL_PRIVILEGED permission).
    // if (scheme.Equals(String("voicemail"))) {
    //     AutoPtr<ITelephonyManager> tm;
    //     CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    //     tm->GetCompleteVoiceMailNumber(res);
    //     return NOERROR;
    // }

    // if (context == NULL) {
    //     *res = String(NULL);
    //     return NOERROR;
    // }

    // String type;
    // intent->ResolveType(context, &type);
    // String phoneColumn;

    // // Correctly read out the phone entry based on requested provider
    // String authority;
    // uri->GetAuthority(&authority);
    // if (authority.Equals(IContacts::AUTHORITY)) {
    //     phoneColumn = IContactsPeoplePhones::NUMBER;
    // } else if (authority.Equals(IContactsContract::AUTHORITY)) {
    //     phoneColumn = IContactsContractCommonDataKindsPhone::NUMBER;
    // }

    // AutoPtr<ICursor> c;
    // AutoPtr<IContentResolver> cResolver;
    // context->GetContentResolver((IContentResolver**)&cResolver);
    // AutoPtr< ArrayOf<String> > strPro = ArrayOf<String>::Alloc(1);
    // (*strPro)[0] = phoneColumn;
    // cResolver->Query(uri, strPro, NULL, NULL, NULL, (ICursor**)&c);
    // if (c != NULL) {
    //     //try {
    //     Boolean succeeded = FALSE;
    //     c->MoveToFirst(&succeeded);
    //     if (succeeded) {
    //         Int32 columnIndex = 0;
    //         c->GetColumnIndex(phoneColumn, &columnIndex);
    //         c->GetString(columnIndex, &number);
    //     }
    //     c->Close();

    //     // } finally {
    //     //     c.close();
    //     // }
    // }

    // *res = number;
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ExtractNetworkPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    // VALIDATE_NOT_NULL(res);
    // if (phoneNumber.IsNullOrEmpty()) {
    //     *res = String(NULL);
    //     return NOERROR;
    // }

    // Int32 len = phoneNumber.GetLength();
    // AutoPtr<StringBuilder> ret = new StringBuilder(len);

    // for (Int32 i = 0; i < len; i++) {
    //     Char32 c = phoneNumber.GetChar(i);
    //     // Character.digit() supports ASCII and Unicode digits (fullwidth, Arabic-Indic, etc.)
    //     Int32 digit = Character::ToDigit(c, 10);
    //     if (digit != -1) {
    //         ret.AppendInt32(digit);
    //     } else if (c == '+') {
    //         // Allow '+' as first character or after CLIR MMI prefix
    //         String prefix = ret.ToString();
    //         if (prefix.GetLength() == 0 || prefix.Equals(IPhoneNumberUtils::CLIR_ON) || prefix.Equals(IPhoneNumberUtils::CLIR_OFF)) {
    //             ret.AppendChar(c);
    //         }
    //     } else if (IsDialable(c)) {
    //         ret.AppendChar(c);
    //     } else if (IsStartsPostDial(c)) {
    //         break;
    //     }
    // }

    // res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ExtractNetworkPortionAlt(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::StripSeparators(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ConvertAndStrip(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ConvertPreDial(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ExtractPostDialPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::Compare(
    /* [in] */ IContext* context,
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean useStrictComparation,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CompareLoosely(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean acceptInvalidCCCPrefix,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ToCallerIDMinMatch(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::GetStrippedReversed(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::StringFromStringAndTOA(
    /* [in] */ const String& s,
    /* [in] */ Int32 TOA,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ToaFromString(
    /* [in] */ const String& s,
    /* [out] */ Int32* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CalledPartyBCDToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CalledPartyBCDFragmentToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsWellFormedSmsAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsGlobalPhoneNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::NetworkPortionToCalledPartyBCD(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::NetworkPortionToCalledPartyBCDWithLength(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::NumberToCalledPartyBCD(
    /* [in] */ const String& number,
    /* [out, callee] */ ArrayOf<Byte>** res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& source,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::GetFormatTypeForLocale(
    /* [in] */ Elastos::Utility::ILocale* locale,
    /* [out] */ Int32* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ IEditable* text,
    /* [in] */ Int32 defaultFormattingType)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatNanpNumber(
    /* [in] */ IEditable* text)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatJapaneseNumber(
    /* [in] */ IEditable* text)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatNumberToE164(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& phoneNumberE164,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::NormalizeNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ReplaceUnicodeDigits(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsLocalEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsPotentialLocalEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsVoiceMailNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ConvertKeypadLettersToDigits(
    /* [in] */ const String& input,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CdmaCheckAndProcessPlusCode(
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CdmaCheckAndProcessPlusCodeByNumberFormat(
    /* [in] */ const String& dialStr,
    /* [in] */ Int32 currFormat,
    /* [in] */ Int32 defaultFormat,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsUriNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::GetUsernameFromUriNumber(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{

    return NOERROR;
}

CPhoneNumberUtilsHelper::CountryCallingCodeAndNewIndex::CountryCallingCodeAndNewIndex(
    /* [in] */ Int32 countryCode,
    /* [in] */ Int32 newIndex)
{
    mCountryCallingCode = countryCode;
    mNewIndex = newIndex;
}

ECode CPhoneNumberUtilsHelper::IsPause(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c == 'p'||c == 'P');
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsToneWait(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c == 'w'||c == 'W');
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsSeparator(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean isdisable;
    IsDialable(ch, &isdisable);
    *res = (!isdisable && !(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')));
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsDialable(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    for(Int32 i = 0, count = address.GetLength(); i < count; i++) {
        Boolean isdisable;
        IsDialable(address.GetChar(i), &isdisable);
        if (!isdisable) {
            *res = FALSE;
            return NOERROR;
        }
    }
    *res = TRUE;
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsNonSeparator(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+'
                || c == IPhoneNumberUtils::WILD || c == IPhoneNumberUtils::WAIT || c == IPhoneNumberUtils::PAUSE);
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    IsEmergencyNumberInternal(number, NULL, useExactMatch, res);
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    // VALIDATE_NOT_NULL(res);
    // // If the number passed in is null, just return false:
    // if (number.IsNullOrEmpty()) {
    //     *res = FALSE;
    //     return NOERROR;
    // }

    // // If the number passed in is a SIP address, return false, since the
    // // concept of "emergency numbers" is only meaningful for calls placed
    // // over the cell network.
    // // (Be sure to do this check *before* calling extractNetworkPortionAlt(),
    // // since the whole point of extractNetworkPortionAlt() is to filter out
    // // any non-dialable characters (which would turn 'abc911def@example.com'
    // // into '911', for example.))
    // Boolean isUriNumber;
    // if (IsUriNumber(number, &isUriNumber), isUriNumber) {
    //     *res = FALSE;
    //     return NOERROR;
    // }

    // // Strip the separators from the number before comparing it
    // // to the list.
    // ExtractNetworkPortionAlt(&number);

    // // retrieve the list of emergency numbers
    // // check read-write ecclist property first
    // String numbers = SystemProperties::Get(String("ril.ecclist"));
    // if (numbers.IsEmpty()) {
    //     // then read-only ecclist property since old RIL only uses this
    //     numbers = SystemProperties::Get(String("ro.ril.ecclist"));
    // }

    // if (!numbers.IsEmpty()) {
    //     // searches through the comma-separated list for a match,
    //     // return true if one is found.
    //     for (String emergencyNum : numbers.split(",")) {
    //         // It is not possible to append additional digits to an emergency number to dial
    //         // the number in Brazil - it won't connect.
    //         if (useExactMatch || "BR".equalsIgnoreCase(defaultCountryIso)) {
    //             if (number.equals(emergencyNum)) {
    //                 return true;
    //             }
    //         } else {
    //             if (number.startsWith(emergencyNum)) {
    //                 return true;
    //             }
    //         }
    //     }
    //     // no matches found against the list!
    //     return false;
    // }

    // Log.d(LOG_TAG, "System property doesn't provide any emergency numbers."
    //         + " Use embedded logic for determining ones.");

    // // No ecclist system property, so use our own list.
    // if (defaultCountryIso != null) {
    //     ShortNumberUtil util = new ShortNumberUtil();
    //     if (useExactMatch) {
    //         return util.isEmergencyNumber(number, defaultCountryIso);
    //     } else {
    //         return util.connectsToEmergencyNumber(number, defaultCountryIso);
    //     }
    // } else {
    //     if (useExactMatch) {
    //         return (number.equals("112") || number.equals("911"));
    //     } else {
    //         return (number.startsWith("112") || number.startsWith("911"));
    //     }
    // }

    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsLocalEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsTwoToNine(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsNanp(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsOneNanp(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::MatchIntlPrefix(
    /* [in] */ const String& a,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::MatchIntlPrefixAndCC(
    /* [in] */ const String& a,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::MatchTrunkPrefix(
    /* [in] */ const String& a,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::IsCountryCallingCode(
    /* [in] */ Int32 countryCallingCodeCandidate,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CheckPrefixIsIgnorable(
    /* [in] */ const String& str,
    /* [in] */ Int32 forwardIndex,
    /* [in] */ Int32 backwardIndex,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::AppendPwCharBackToOrigDialStr(
    /* [in] */ Int32 dialableIndex,
    /* [in] */ const String& origStr,
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::GetDefaultIdp(
    /* [out] */ String* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::InternalGetStrippedReversed(
    /* [in] */ const String& np,
    /* [in] */ Int32 numDigits,
    /* [out] */ String* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::ProcessPlusCodeWithinNanp(
    /* [in] */ const String& networkDialStr,
    /* [out] */ String* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::CharToBCD(
    /* [in] */ Char32 c,
    /* [out] */ Int32* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::FindDialableIndexFromPostDialStr(
    /* [in] */ const String& postDialStr,
    /* [out] */ Int32* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::GetFormatTypeFromCountryCode(
    /* [in] */ const String& country,
    /* [out] */ Int32* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::TryGetISODigit(
    /* [in] */ Char32 ch,
    /* [out] */ Int32* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::TryGetTrunkPrefixOmittedIndex(
    /* [in] */ const String& str,
    /* [in] */ Int32 currentIndex,
    /* [out] */ Int32* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::InternalCalledPartyBCDFragmentToString(
    /* [in] */ StringBuilder* sb,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::Log(
    /* [in] */ const String& msg)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::RemoveDashes(
    /* [in] */ IEditable* text)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::NumberToCalledPartyBCDHelper(
    /* [in] */ const String& number,
    /* [in] */ Boolean includeLength,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::BcdToChar(
    /* [in] */ Byte b,
    /* [out] */ Char32* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtilsHelper::TryGetCountryCallingCodeAndNewIndex(
    /* [in] */ const String& str,
    /* [in] */ Boolean acceptThailandCase,
    /* [out] */ CountryCallingCodeAndNewIndex* cccani)
{
    return NOERROR;
}

}
}
}

