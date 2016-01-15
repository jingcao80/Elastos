
#include "CPhoneNumberUtils.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "CTelephonyManager.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/SpannableStringBuilder.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Character.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Text::SpannableStringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::CLocale;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CPhoneNumberUtils::LOG_TAG("CPhoneNumberUtils");
const String CPhoneNumberUtils::PLUS_SIGN_STRING("+");
const String CPhoneNumberUtils::NANP_IDP_STRING("011");
const String CPhoneNumberUtils::CLIR_ON("*31#");
const String CPhoneNumberUtils::CLIR_OFF("#31#");

const Boolean COUNTRY_CALLING_CALL[] = {
    TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE,
    TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE,
    TRUE, FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
    FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE,
    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, FALSE, FALSE, TRUE,
    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, TRUE, FALSE
};

/** List of country codes for countries that use the NANP */
const String NANP_COUNTRIES[] = {
    String("US"), // United States
    String("CA"), // Canada
    String("AS"), // American Samoa
    String("AI"), // Anguilla
    String("AG"), // Antigua and Barbuda
    String("BS"), // Bahamas
    String("BB"), // Barbados
    String("BM"), // Bermuda
    String("VG"), // British Virgin Islands
    String("KY"), // Cayman Islands
    String("DM"), // Dominica
    String("DO"), // Dominican Republic
    String("GD"), // Grenada
    String("GU"), // Guam
    String("JM"), // Jamaica
    String("PR"), // Puerto Rico
    String("MS"), // Montserrat
    String("MP"), // Northern Mariana Islands
    String("KN"), // Saint Kitts and Nevis
    String("LC"), // Saint Lucia
    String("VC"), // Saint Vincent and the Grenadines
    String("TT"), // Trinidad and Tobago
    String("TC"), // Turks and Caicos Islands
    String("VI"), // U.S. Virgin Islands
};

const Int32 NANP_COUNTRIES_LEN = 24;

const Int32 CCC_LENGTH = sizeof(COUNTRY_CALLING_CALL) / sizeof(Boolean);

static AutoPtr<IPattern> InitGLOBAL_PHONE_NUMBER_PATTERN() {
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(String("[\\+]?[0-9.-]+"), (IPattern**)&p);
    return p;
}

const AutoPtr<IPattern> CPhoneNumberUtils::GLOBAL_PHONE_NUMBER_PATTERN = InitGLOBAL_PHONE_NUMBER_PATTERN();

ECode CPhoneNumberUtils::IsISODigit(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c >= '0' && c <= '9');
    return NOERROR;
}

ECode CPhoneNumberUtils::Is12Key(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#');
    return NOERROR;
}

ECode CPhoneNumberUtils::IsDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+' || c == IPhoneNumberUtils::WILD);
    return NOERROR;
}

ECode CPhoneNumberUtils::IsReallyDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+');
    return NOERROR;
}

ECode CPhoneNumberUtils::IsNonSeparator(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+'
                    || c == IPhoneNumberUtils::WILD || c == IPhoneNumberUtils::WAIT || c == IPhoneNumberUtils::PAUSE);
    return NOERROR;
}

ECode CPhoneNumberUtils::IsStartsPostDial(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c== IPhoneNumberUtils::PAUSE || c == IPhoneNumberUtils::WAIT);
    return NOERROR;
}

ECode CPhoneNumberUtils::GetNumberFromIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IContext* context,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String number;

    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    String scheme;
    uri->GetScheme(&scheme);

    if (scheme.Equals(String("tel")) || scheme.Equals(String("sip"))) {
        uri->GetSchemeSpecificPart(res);
        return NOERROR;
    }

    // TODO: We don't check for SecurityException here (requires
    // CALL_PRIVILEGED permission).
    if (scheme.Equals(String("voicemail"))) {
        AutoPtr<ITelephonyManager> tm;
        CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
        tm->GetCompleteVoiceMailNumber(res);
        return NOERROR;
    }

    if (context == NULL) {
        *res = String(NULL);
        return NOERROR;
    }

    String type;
    intent->ResolveType(context, &type);
    String phoneColumn;

    // Correctly read out the phone entry based on requested provider
    String authority;
    uri->GetAuthority(&authority);
    if (authority.Equals(String("contacts"))) {
        //*****************************************
        // here need to merge Contacts.car for constant use
        phoneColumn = String("number");
    } else if (authority.Equals(String("com.android.contacts"))) {
        phoneColumn = String("has_phone_number");
    }

    AutoPtr<ICursor> c;
    AutoPtr<IContentResolver> cResolver;
    context->GetContentResolver((IContentResolver**)&cResolver);
    AutoPtr< ArrayOf<String> > strPro = ArrayOf<String>::Alloc(1);
    (*strPro)[0] = phoneColumn;
    cResolver->Query(uri, strPro, String(NULL), (ArrayOf<String>*)NULL, String(NULL), (ICursor**)&c);
    if (c != NULL) {
        //try {
        Boolean succeeded = FALSE;
        c->MoveToFirst(&succeeded);
        if (succeeded) {
            Int32 columnIndex = 0;
            c->GetColumnIndex(phoneColumn, &columnIndex);
            c->GetString(columnIndex, &number);
        }
        c->Close();

        // } finally {
        //     c.close();
        // }
    }

    // *res = number;
    return NOERROR;
}

ECode CPhoneNumberUtils::ExtractNetworkPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    if (phoneNumber.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }

    Int32 len = phoneNumber.GetLength();
    StringBuilder ret;
    Boolean tempRes;

    for (Int32 i = 0; i < len; i++) {
        Char32 c = phoneNumber.GetChar(i);
        // Character.digit() supports ASCII and Unicode digits (fullwidth, Arabic-Indic, etc.)
        Int32 digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            ret.AppendInt32(digit);
        } else if (c == '+') {
            // Allow '+' as first character or after CLIR MMI prefix
            String prefix = ret.ToString();
            if (prefix.GetLength() == 0 || prefix.Equals(CLIR_ON) || prefix.Equals(CLIR_OFF)) {
                ret.AppendChar(c);
            }
        } else if (IsDialable(c, &tempRes), tempRes) {
            ret.AppendChar(c);
        } else if ((IsStartsPostDial(c, &tempRes), tempRes)) {
            break;
        }
    }

    *res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::ExtractNetworkPortionAlt(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    if (phoneNumber.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }

    Int32 len = phoneNumber.GetLength();
    StringBuilder ret;
    Boolean haveSeenPlus = FALSE, tempRes;

    for (Int32 i = 0; i < len; i++) {
        Char32 c = phoneNumber.GetChar(i);
        if (c == '+') {
            if (haveSeenPlus) {
                continue;
            }
            haveSeenPlus = TRUE;
        }
        if (IsDialable(c, &tempRes), tempRes) {
            ret.AppendChar(c);
        } else if (IsStartsPostDial(c, &tempRes), tempRes) {
            break;
        }
    }

    *res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::StripSeparators(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    if (phoneNumber.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }
    Int32 len = phoneNumber.GetLength();
    StringBuilder ret;
    Boolean tempRes;

    for (Int32 i = 0; i < len; i++) {
        Char32 c = phoneNumber.GetChar(i);
        // Character.digit() supports ASCII and Unicode digits (fullwidth, Arabic-Indic, etc.)
        Int32 digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            ret.AppendInt32(digit);
        } else if (IsNonSeparator(c, &tempRes), tempRes) {
            ret.AppendChar(c);
        }
    }

    *res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::ConvertAndStrip(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String str;
    ConvertKeypadLettersToDigits(phoneNumber, &str);
    return StripSeparators(str, res);
}

ECode CPhoneNumberUtils::ConvertPreDial(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    if (phoneNumber.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }
    Int32 len = phoneNumber.GetLength();
    StringBuilder ret;
    Boolean tempRes;

    for (Int32 i = 0; i < len; i++) {
        Char32 c = phoneNumber.GetChar(i);

        if (IsPause(c, &tempRes), tempRes) {
            c = IPhoneNumberUtils::PAUSE;
        } else if (IsToneWait(c, &tempRes), tempRes) {
            c = IPhoneNumberUtils::WAIT;
        }
        ret.AppendChar(c);
    }
    *res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::ExtractPostDialPortion(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    if (phoneNumber.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }

    Int32 trimIndex;
    StringBuilder ret;
    Boolean tempRes;

    IndexOfLastNetworkChar(phoneNumber, &trimIndex);

    for (Int32 i = trimIndex + 1, s = phoneNumber.GetLength(); i < s; i++) {
        Char32 c = phoneNumber.GetChar(i);
        if (IsNonSeparator(c, &tempRes), tempRes) {
            ret.AppendChar(c);
        }
    }

    *res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    // We've used loose comparation at least Eclair, which may change in the future.

    return Compare(a, b, FALSE, res);
}

ECode CPhoneNumberUtils::Compare(
    /* [in] */ IContext* context,
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    Boolean useStrict;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetBoolean(R::bool_::config_use_strict_phone_number_comparation, &useStrict);
    return Compare(a, b, useStrict, res);
}

ECode CPhoneNumberUtils::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean useStrictComparation,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return (useStrictComparation ? CompareStrictly(a, b, res) : CompareLoosely(a, b, res));
}

ECode CPhoneNumberUtils::CompareLoosely(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 ia, ib;
    Int32 matched;
    Int32 numNonDialableCharsInA = 0;
    Int32 numNonDialableCharsInB = 0;

    if (a.IsNull() || b.IsNull()) {
        *res = (a == b);
        return NOERROR;
    }

    if (a.GetLength() == 0 || b.GetLength() == 0) {
        *res = FALSE;
        return NOERROR;
    }

    IndexOfLastNetworkChar(a, &ia);
    IndexOfLastNetworkChar(b, &ib);
    matched = 0;
    Boolean tempRes;

    while (ia >= 0 && ib >=0) {
        Char32 ca, cb;
        Boolean skipCmp = FALSE;

        ca = a.GetChar(ia);

        if (IsDialable(ca, &tempRes), !tempRes) {
            ia--;
            skipCmp = TRUE;
            numNonDialableCharsInA++;
        }

        cb = b.GetChar(ib);

        if (IsDialable(cb, &tempRes), tempRes) {
            ib--;
            skipCmp = TRUE;
            numNonDialableCharsInB++;
        }

        if (!skipCmp) {
            if (cb != ca && ca != IPhoneNumberUtils::WILD && cb != IPhoneNumberUtils::WILD) {
                break;
            }
            ia--; ib--; matched++;
        }
    }

    if (matched < MIN_MATCH) {
        Int32 effectiveALen = a.GetLength() - numNonDialableCharsInA;
        Int32 effectiveBLen = b.GetLength() - numNonDialableCharsInB;


        // if the number of dialable chars in a and b match, but the matched chars < MIN_MATCH,
        // treat them as equal (i.e. 404-04 and 40404)
        if (effectiveALen == effectiveBLen && effectiveALen == matched) {
            *res = TRUE;
            return NOERROR;
        }

        *res = FALSE;
        return NOERROR;
    }

    // At least one string has matched completely;
    if (matched >= MIN_MATCH && (ia < 0 || ib < 0)) {
        *res = TRUE;
        return NOERROR;
    }

    /*
     * Now, what remains must be one of the following for a
     * match:
     *
     *  - a '+' on one and a '00' or a '011' on the other
     *  - a '0' on one and a (+,00)<country code> on the other
     *     (for this, a '0' and a '00' prefix would have succeeded above)
     */

    if ((MatchIntlPrefix(a, ia + 1, &tempRes), tempRes) && (MatchIntlPrefix(b, ib +1, &tempRes), tempRes)) {
        *res = TRUE;
        return NOERROR;
    }

    if ((MatchTrunkPrefix(a, ia + 1, &tempRes), tempRes) && (MatchIntlPrefixAndCC(b, ib +1, &tempRes), tempRes)) {
        *res = TRUE;
        return NOERROR;
    }

    if ((MatchTrunkPrefix(b, ib + 1, &tempRes), tempRes) && (MatchIntlPrefixAndCC(a, ia +1, &tempRes), tempRes)) {
        *res = TRUE;
        return NOERROR;
    }

    *res = FALSE;
    return NOERROR;
}

ECode CPhoneNumberUtils::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return CompareStrictly(a, b, TRUE, res);
}

ECode CPhoneNumberUtils::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean acceptInvalidCCCPrefix,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (a.IsNull() || b.IsNull()) {
        *res = (a == b);
        return NOERROR;
    } else if (a.GetLength() == 0 && b.GetLength() == 0) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 forwardIndexA = 0;
    Int32 forwardIndexB = 0;

    AutoPtr<CountryCallingCodeAndNewIndex> cccA;
    TryGetCountryCallingCodeAndNewIndex(a, acceptInvalidCCCPrefix, (CountryCallingCodeAndNewIndex**)&cccA);
    AutoPtr<CountryCallingCodeAndNewIndex> cccB;
    TryGetCountryCallingCodeAndNewIndex(b, acceptInvalidCCCPrefix, (CountryCallingCodeAndNewIndex**)&cccB);
    Boolean bothHasCountryCallingCode = FALSE;
    Boolean okToIgnorePrefix = TRUE;
    Boolean trunkPrefixIsOmittedA = FALSE;
    Boolean trunkPrefixIsOmittedB = FALSE;
    if (cccA != NULL && cccB != NULL) {
        if (cccA->mCountryCallingCode != cccB->mCountryCallingCode) {
            // Different Country Calling Code. Must be different phone number.
            *res = FALSE;
            return NOERROR;
        }
        // When both have ccc, do not ignore trunk prefix. Without this,
        // "+81123123" becomes same as "+810123123" (+81 == Japan)
        okToIgnorePrefix = FALSE;
        bothHasCountryCallingCode = TRUE;
        forwardIndexA = cccA->mNewIndex;
        forwardIndexB = cccB->mNewIndex;
    } else if (cccA == NULL && cccB == NULL) {
        // When both do not have ccc, do not ignore trunk prefix. Without this,
        // "123123" becomes same as "0123123"
        okToIgnorePrefix = FALSE;
    } else {
        if (cccA != NULL) {
            forwardIndexA = cccA->mNewIndex;
        } else {
            Int32 tmp;
            TryGetTrunkPrefixOmittedIndex(b, 0, &tmp);
            if (tmp >= 0) {
                forwardIndexA = tmp;
                trunkPrefixIsOmittedA = TRUE;
            }
        }
        if (cccB != NULL) {
            forwardIndexB = cccB->mNewIndex;
        } else {
            Int32 tmp;
            TryGetTrunkPrefixOmittedIndex(b, 0, &tmp);
            if (tmp >= 0) {
                forwardIndexB = tmp;
                trunkPrefixIsOmittedB = TRUE;
            }
        }
    }

    Int32 backwardIndexA = a.GetLength() - 1;
    Int32 backwardIndexB = b.GetLength() - 1;
    Boolean tempRes;
    while (backwardIndexA >= forwardIndexA && backwardIndexB >= forwardIndexB) {
        Boolean skip_compare = FALSE;
        Char32 chA = a.GetChar(backwardIndexA);
        Char32 chB = b.GetChar(backwardIndexB);
        if (IsSeparator(chA, &tempRes), tempRes) {
            backwardIndexA--;
            skip_compare = TRUE;
        }
        if (IsSeparator(chB, &tempRes), tempRes) {
            backwardIndexB--;
            skip_compare = TRUE;
        }

        if (!skip_compare) {
            if (chA != chB) {
                *res = FALSE;
                return NOERROR;
            }
            backwardIndexA--;
            backwardIndexB--;
        }
    }

    if (okToIgnorePrefix) {
        if ((trunkPrefixIsOmittedA && forwardIndexA <= backwardIndexA) ||
            (CheckPrefixIsIgnorable(a, forwardIndexA, backwardIndexA, &tempRes), !tempRes)) {
            if (acceptInvalidCCCPrefix) {
                // Maybe the code handling the special case for Thailand makes the
                // result garbled, so disable the code and try again.
                // e.g. "16610001234" must equal to "6610001234", but with
                //      Thailand-case handling code, they become equal to each other.
                //
                // Note: we select simplicity rather than adding some complicated
                //       logic here for performance(like "checking whether remaining
                //       numbers are just 66 or not"), assuming inputs are small
                //       enough.
                return Compare(a, b, FALSE, res);
            } else {
                *res = FALSE;
                return NOERROR;
            }
        }
        if ((trunkPrefixIsOmittedB && forwardIndexB <= backwardIndexB) ||
            (CheckPrefixIsIgnorable(b, forwardIndexA, backwardIndexB, &tempRes), !tempRes)) {
            if (acceptInvalidCCCPrefix) {
                return Compare(a, b, FALSE, res);
            } else {
                *res = FALSE;
                return NOERROR;
            }
        }
    } else {
        // In the US, 1-650-555-1234 must be equal to 650-555-1234,
        // while 090-1234-1234 must not be equal to 90-1234-1234 in Japan.
        // This request exists just in US (with 1 trunk (NDD) prefix).
        // In addition, "011 11 7005554141" must not equal to "+17005554141",
        // while "011 1 7005554141" must equal to "+17005554141"
        //
        // In this comparison, we ignore the prefix '1' just once, when
        // - at least either does not have CCC, or
        // - the remaining non-separator number is 1
        Boolean maybeNamp = !bothHasCountryCallingCode;
        while (backwardIndexA >= forwardIndexA) {
            Char32 chA = a.GetChar(backwardIndexA);
            if (IsDialable(chA, &tempRes), tempRes) {
                Int32 tmp;
                if (maybeNamp && (TryGetISODigit(chA, &tmp), tmp == 1)) {
                    maybeNamp = FALSE;
                } else {
                    *res = FALSE;
                    return NOERROR;
                }
            }
            backwardIndexA--;
        }
        while (backwardIndexB >= forwardIndexB) {
            Char32 chB = b.GetChar(backwardIndexB);
            if (IsDialable(chB, &tempRes), tempRes) {
                Int32 tmp;
                if (maybeNamp && (TryGetISODigit(chB, &tmp), tmp == 1)) {
                    maybeNamp = FALSE;
                } else {
                    *res = FALSE;
                    return NOERROR;
                }
            }
            backwardIndexB--;
        }
    }

    *res = TRUE;
    return NOERROR;
}

ECode CPhoneNumberUtils::ToCallerIDMinMatch(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String np;
    ExtractNetworkPortionAlt(phoneNumber, &np);
    return InternalGetStrippedReversed(np, MIN_MATCH, res);
}

ECode CPhoneNumberUtils::GetStrippedReversed(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String np;
    ExtractNetworkPortionAlt(phoneNumber, &np);

    if (np.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }

    return InternalGetStrippedReversed(np, np.GetLength(), res);
}

ECode CPhoneNumberUtils::StringFromStringAndTOA(
    /* [in] */ const String& s,
    /* [in] */ Int32 TOA,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    if (s.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }

    if (TOA == IPhoneNumberUtils::TOA_International && s.GetLength() > 0 && s.GetChar(0) != '+') {
        *res = String("+") + s;
        return NOERROR;
    }

    *res = s;
    return NOERROR;
}

ECode CPhoneNumberUtils::ToaFromString(
    /* [in] */ const String& s,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    if (!s.IsNull() && s.GetLength() > 0 && s.GetChar(0) == '+') {
        *res = IPhoneNumberUtils::TOA_International;
        return NOERROR;
    }

    *res = IPhoneNumberUtils::TOA_Unknown;
    return NOERROR;
}

ECode CPhoneNumberUtils::CalledPartyBCDToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean prependPlus = FALSE;
    AutoPtr<StringBuilder> ret = new StringBuilder(1 + length * 2);

    if (length < 2) {
        *res = String("");
        return NOERROR;
    }

    //Only TON field should be taken in consideration
    if (((*bytes)[offset] & 0xf0) == (IPhoneNumberUtils::TOA_International & 0xf0)) {
        prependPlus = TRUE;
    }

    InternalCalledPartyBCDFragmentToString(ret, bytes, offset + 1, length - 1);
    Int32 tmp;
    if (prependPlus && (ret->GetLength(&tmp), tmp == 0)) {
        // If the only thing there is a prepended plus, return ""
        *res = String("");
        return NOERROR;
    }

    if (prependPlus) {
        // This is an "international number" and should have
        // a plus prepended to the dialing number. But there
        // can also be GSM MMI codes as defined in TS 22.030 6.5.2
        // so we need to handle those also.
        //
        // http://web.telia.com/~u47904776/gsmkode.htm
        // has a nice list of some of these GSM codes.
        //
        // Examples are:
        //   **21*+886988171479#
        //   **21*8311234567#
        //   *21#
        //   #21#
        //   *#21#
        //   *31#+11234567890
        //   #31#+18311234567
        //   #31#8311234567
        //   18311234567
        //   +18311234567#
        //   +18311234567
        // Odd ball cases that some phones handled
        // where there is no dialing number so they
        // append the "+"
        //   *21#+
        //   **21#+
        String retString = ret->ToString();
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        AutoPtr<IPattern> p;
        helper->Compile(String("(^[#*])(.*)([#*])(.*)(#)$"), (IPattern**)&p);
        AutoPtr<IMatcher> m;
        p->Matcher(retString, (IMatcher**)&m);
        Boolean tempRes;
        String str;

        if (m->Matches(&tempRes), tempRes) {
            if (String("").Equals((m->Group(2, &str), str))) {
                // Started with two [#*] ends with #
                // So no dialing number and we'll just
                // append a +, this handles **21#+
                ret = NULL;
                ret = new StringBuilder();
                ret->AppendString((m->Group(1, &str), str));
                ret->AppendString((m->Group(3, &str), str));
                ret->AppendString((m->Group(4, &str), str));
                ret->AppendString((m->Group(5, &str), str));
                ret->AppendCStr("+");
            } else {
                // Starts with [#*] and ends with #
                // Assume group 4 is a dialing number
                // such as *21*+1234554#
                ret = NULL;
                ret = new StringBuilder();
                ret->AppendString((m->Group(1, &str), str));
                ret->AppendString((m->Group(2, &str), str));
                ret->AppendString((m->Group(3, &str), str));
                ret->AppendCStr("+");
                ret->AppendString((m->Group(4, &str), str));
                ret->AppendString((m->Group(5, &str), str));
            }
        } else {
            p = NULL;
            m = NULL;
            helper->Compile(String("(^[#*])(.*)([#*])(.*)"), (IPattern**&)p);
            p->Matcher(retString, (IMatcher**)&m);
            if (m->Matches(&tempRes), tempRes){
                // Starts with [#*] and only one other [#*]
                // Assume the data after last [#*] is dialing
                // number (i.e. group 4) such as *31#+11234567890.
                // This also includes the odd ball *21#+
                ret = NULL;
                ret = new StringBuilder();
                ret->AppendString((m->Group(1, &str), str));
                ret->AppendString((m->Group(2, &str), str));
                ret->AppendString((m->Group(3, &str), str));
                ret->AppendCStr("+");
                ret->AppendString((m->Group(4, &str), str));
            } else {
                // Does NOT start with [#*] just prepend '+'
                ret = NULL;
                ret = new StringBuilder();
                ret->AppendChar('+');
                ret->AppendString(retString);
            }
        }
    }

    *res = ret->ToString();

    return NOERROR;
}

ECode CPhoneNumberUtils::CalledPartyBCDFragmentToString(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<StringBuilder> ret = new StringBuilder(length * 2);
    InternalCalledPartyBCDFragmentToString(ret, bytes, offset, length);
    *res = ret->ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::IsWellFormedSmsAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    String networkPortion;
    ExtractNetworkPortion(address, &networkPortion);
    Boolean tempRes;
    *res = ((!(networkPortion.Equals("+")
              || networkPortion.IsEmpty()))
           && (IsDialable(networkPortion, &tempRes), tempRes));

    return NOERROR;
}

ECode CPhoneNumberUtils::IsGlobalPhoneNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (phoneNumber.IsEmpty()) {
        *res = FALSE;
        return NOERROR;
    }

    AutoPtr<IMatcher> match;
    GLOBAL_PHONE_NUMBER_PATTERN->Matcher(phoneNumber, (IMatcher**)&match);
    return match->Matches(res);
}

ECode CPhoneNumberUtils::NetworkPortionToCalledPartyBCD(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    String networkPortion;
    ExtractNetworkPortion(s, &networkPortion);
    return NumberToCalledPartyBCDHelper(networkPortion, FALSE, res);
}

ECode CPhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    String networkPortion;
    ExtractNetworkPortion(s, &networkPortion);
    return NumberToCalledPartyBCDHelper(networkPortion, TRUE, res);
}

ECode CPhoneNumberUtils::NumberToCalledPartyBCD(
    /* [in] */ const String& number,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    return NumberToCalledPartyBCDHelper(number, FALSE, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& source,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

/*    AutoPtr<SpannableStringBuilder> text = new SpannableStringBuilder(source);
    Int32 defaultFormattingType;
    GetFormatTypeForLocale(CLocale::GetDefault(), &defaultFormattingType);
    FormatNumber(text, defaultFormattingType, res);
    *res = text.ToString();*/
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& source,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::GetFormatTypeForLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    String country;
    locale->GetCountry(&country);

    return GetFormatTypeFromCountryCode(country, res);
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ IEditable* text,
    /* [in] */ Int32 defaultFormattingType)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::FormatNanpNumber(
    /* [in] */ IEditable* text)
{
    /*Int32 length = text->GetLength();
    if (length > String("+1-nnn-nnn-nnnn").GetLength()) {
        // The string is too long to be formatted
        return NOERROR;
    } else if (length <= 5) {
        // The string is either a shortcode or too short to be formatted
        return NOERROR;
    }

    AutoPtr<ICharSequence> saved;
    text->SubSequence(0, length, (ICharSequence**)&saved);

    // Strip the dashes first, as we're going to add them back
    RemoveDashes(text);
    length = text->GetLength();

    // When scanning the number we record where dashes need to be added,
    // if they're non-0 at the end of the scan the dashes will be added in
    // the proper places.
    Int32 dashPositions[] = new int[3];
    Int32 numDashes = 0;

    Int32 state = NANP_STATE_DIGIT;
    Int32 numDigits = 0;
    for (Int32 i = 0; i < length; i++) {
        char c = text->charAt(i);
        switch (c) {
            case '1':
                if (numDigits == 0 || state == NANP_STATE_PLUS) {
                    state = NANP_STATE_ONE;
                    break;
                }
                // fall through
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (state == NANP_STATE_PLUS) {
                    // Only NANP number supported for now
                    text->replace(0, length, saved);
                    return;
                } else if (state == NANP_STATE_ONE) {
                    // Found either +1 or 1, follow it up with a dash
                    dashPositions[numDashes++] = i;
                } else if (state != NANP_STATE_DASH && (numDigits == 3 || numDigits == 6)) {
                    // Found a digit that should be after a dash that isn't
                    dashPositions[numDashes++] = i;
                }
                state = NANP_STATE_DIGIT;
                numDigits++;
                break;

            case '-':
                state = NANP_STATE_DASH;
                break;

            case '+':
                if (i == 0) {
                    // Plus is only allowed as the first character
                    state = NANP_STATE_PLUS;
                    break;
                }
                // Fall through
            default:
                // Unknown character, bail on formatting
                text->replace(0, length, saved);
                return;
        }
    }

    if (numDigits == 7) {
        // With 7 digits we want xxx-xxxx, not xxx-xxx-x
        numDashes--;
    }

    // Actually put the dashes in place
    for (Int32 i = 0; i < numDashes; i++) {
        Int32 pos = dashPositions[i];
        text->replace(pos + i, pos + i, "-");
    }

    // Remove trailing dashes
    Int32 len = text->GetLength();
    while (len > 0) {
        if (text->charAt(len - 1) == '-') {
            text->delete(len - 1, len);
            len--;
        } else {
            break;
        }
    }*/
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::FormatJapaneseNumber(
    /* [in] */ IEditable* text)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::FormatNumberToE164(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& phoneNumberE164,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CPhoneNumberUtils::NormalizeNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::ReplaceUnicodeDigits(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsLocalEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsPotentialLocalEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::IsVoiceMailNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{

    return NOERROR;
}

ECode CPhoneNumberUtils::ConvertKeypadLettersToDigits(
    /* [in] */ const String& input,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    if (input.IsNull()) {
        *res = input;
        return NOERROR;
    }
    Int32 len = input.GetLength();
    if (len == 0) {
        *res = input;
        return NOERROR;
    }

    const AutoPtr<ArrayOf<Char32> > out = input.GetChars();

    for (Int32 i = 0; i < len; i++) {
        Char32 c = (*out)[i];
        // If this char isn't in KEYPAD_MAP at all, just leave it alone.
        //out[i] = (Char32) KEYPAD_MAP.Get(c, c);
    }

    *res = String(*out);
    return NOERROR;
}

ECode CPhoneNumberUtils::CdmaCheckAndProcessPlusCode(
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean tempRes;
    if (!dialStr.IsEmpty()) {
        if ((IsReallyDialable(dialStr.GetChar(0), &tempRes), tempRes) &&
            (IsNonSeparator(dialStr, &tempRes), tempRes)) {
            String currIso,  defaultIso;
            AutoPtr<ISystemProperties> sp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
            sp->Get(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""), &currIso);
            sp->Get(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, String(""), &defaultIso);
            if (!currIso.IsEmpty() && !defaultIso.IsEmpty()) {
                Int32 currIsocc, defaultIsocc;
                GetFormatTypeFromCountryCode(currIso, &currIsocc);
                GetFormatTypeFromCountryCode(defaultIso, &defaultIsocc);
                return CdmaCheckAndProcessPlusCodeByNumberFormat(dialStr, currIsocc, defaultIsocc, res);
            }
        }
    }
    *res = dialStr;
    return NOERROR;
}

ECode CPhoneNumberUtils::CdmaCheckAndProcessPlusCodeByNumberFormat(
    /* [in] */ const String& dialStr,
    /* [in] */ Int32 currFormat,
    /* [in] */ Int32 defaultFormat,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

     String retStr = dialStr;

    // Checks if the plus sign character is in the passed-in dial string
    if (!dialStr.IsNull() &&
     dialStr.LastIndexOf(PLUS_SIGN_STRING) != -1) {
     // Format the string based on the rules for the country the number is from,
     // and the current country the phone is camped on.
     if ((currFormat == defaultFormat) && (currFormat == IPhoneNumberUtils::FORMAT_NANP)) {
         // Handle case where default and current telephone numbering plans are NANP.
         String postDialStr;
         String tempDialStr = dialStr;

         // Sets the retStr to null since the conversion will be performed below.
         retStr = String(NULL);
         if (DBG) Slogger::I(LOG_TAG, "checkAndProcessPlusCode,dialStr=%s", dialStr.string());
         // This routine is to process the plus sign in the dial string by loop through
         // the network portion, post dial portion 1, post dial portion 2... etc. if
         // applied
         do {
             String networkDialStr, tempStr;
             ExtractNetworkPortion(tempDialStr, &tempStr);
             // Handles the conversion within NANP
             ProcessPlusCodeWithinNanp(tempStr, &networkDialStr);

             // Concatenates the string that is converted from network portion
             if (!networkDialStr.IsEmpty()) {
                 if (retStr.IsNull()) {
                     retStr = networkDialStr;
                 } else {
                     retStr = retStr + networkDialStr;
                 }
             } else {
                 // This should never happen since we checked the if dialStr is null
                 // and if it contains the plus sign in the beginning of this function.
                 // The plus sign is part of the network portion.
                 Slogger::E(LOG_TAG, "checkAndProcessPlusCode: null newDialStr %s", networkDialStr.string());
                 *res = dialStr;
                 return NOERROR;
             }
             ExtractPostDialPortion(tempDialStr, &postDialStr);
             if (!postDialStr.IsEmpty()) {
                 Int32 dialableIndex;
                 FindDialableIndexFromPostDialStr(postDialStr, &dialableIndex);

                 // dialableIndex should always be greater than 0
                 if (dialableIndex >= 1) {
                     AppendPwCharBackToOrigDialStr(dialableIndex, retStr, postDialStr, &retStr);
                     // Skips the P/W character, extracts the dialable portion
                     tempDialStr = postDialStr.Substring(dialableIndex);
                 } else {
                     // Non-dialable character such as P/W should not be at the end of
                     // the dial string after P/W processing in CdmaConnection.java
                     // Set the postDialStr to "" to break out of the loop
                     if (dialableIndex < 0) {
                         postDialStr = String("");
                     }
                     Slogger::E(LOG_TAG, "wrong postDialStr=%s", postDialStr.string());
                 }
             }
             if (DBG) Slogger::I(LOG_TAG, "checkAndProcessPlusCode,postDialStr=%s", postDialStr.string());
         } while (!postDialStr.IsEmpty() && !tempDialStr.IsEmpty());
     } else {
         // TODO: Support NANP international conversion and other telephone numbering plans.
         // Currently the phone is never used in non-NANP system, so return the original
         // dial string.
         Slogger::E(LOG_TAG, "checkAndProcessPlusCode:non-NANP not supported %s", dialStr.string());
     }
    }
    *res = retStr;
    return NOERROR;
}

ECode CPhoneNumberUtils::IsUriNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Note we allow either "@" or "%40" to indicate a URI, in case
    // the passed-in string is URI-escaped.  (Neither "@" nor "%40"
    // will ever be found in a legal PSTN number.)
    *res = (!number.IsNull() && (number.Contains("@") || number.Contains("%40")));
    return NOERROR;
}

ECode CPhoneNumberUtils::GetUsernameFromUriNumber(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    // The delimiter between username and domain name can be
    // either "@" or "%40" (the URI-escaped equivalent.)
    Int32 delimiterIndex = number.IndexOf('@');
    if (delimiterIndex < 0) {
        delimiterIndex = number.IndexOf("%40");
    }
    if (delimiterIndex < 0) {
        Slogger::W(LOG_TAG,
              "getUsernameFromUriNumber: no delimiter found in SIP addr '%s'", number.string());
        delimiterIndex = number.GetLength();
    }
    *res = number.Substring(0, delimiterIndex);

    return NOERROR;
}

CPhoneNumberUtils::CountryCallingCodeAndNewIndex::CountryCallingCodeAndNewIndex(
    /* [in] */ Int32 countryCode,
    /* [in] */ Int32 newIndex)
{
    mCountryCallingCode = countryCode;
    mNewIndex = newIndex;
}

ECode CPhoneNumberUtils::IsPause(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c == 'p'||c == 'P');
    return NOERROR;
}

ECode CPhoneNumberUtils::IsToneWait(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c == 'w'||c == 'W');
    return NOERROR;
}

ECode CPhoneNumberUtils::IsSeparator(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean isdisable;
    IsDialable(ch, &isdisable);
    *res = (!isdisable && !(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')));
    return NOERROR;
}

ECode CPhoneNumberUtils::IsDialable(
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

ECode CPhoneNumberUtils::IsNonSeparator(
    /* [in] */ const String& address,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    for (Int32 i = 0, count = address.GetLength(); i < count; i++) {
        Boolean isNonSeparator;
        IsNonSeparator(address.GetChar(i), &isNonSeparator);
        if (!isNonSeparator) {
            *res = FALSE;
        }
    }
    *res = TRUE;
    return NOERROR;
}

ECode CPhoneNumberUtils::IsEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    IsEmergencyNumberInternal(number, String(NULL), useExactMatch, res);
    return NOERROR;
}

ECode CPhoneNumberUtils::IsEmergencyNumberInternal(
    /* [in] */ const String& _number,
    /* [in] */ const String& defaultCountryIso,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // If the number passed in is NULL, just return FALSE:
    if (_number.IsNull()) {
        *res = FALSE;
        return NOERROR;
    }

    String number = _number;
    // If the number passed in is a SIP address, return FALSE, since the
    // concept of "emergency numbers" is only meaningful for calls placed
    // over the cell network.
    // (Be sure to do this check *before* calling extractNetworkPortionAlt(),
    // since the whole poInt32 of extractNetworkPortionAlt() is to filter out
    // any non-dialable characters (which would turn 'abc911def@example.com'
    // into '911', for example.))
    Boolean isUriNumber;
    if (IsUriNumber(number, &isUriNumber), isUriNumber) {
        *res = FALSE;
        return NOERROR;
    }

    // Strip the separators from the number before comparing it
    // to the list.
    String tempStr(number);
    ExtractNetworkPortionAlt(tempStr, &number);

    // retrieve the list of emergency numbers
    // check read-write ecclist property first

    String numbers;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Get(String("ril.ecclist"), &numbers);
    if (numbers.IsEmpty()) {
        // then read-only ecclist property since old RIL only uses this
        sp->Get(String("ro.ril.ecclist"), &numbers);
    }

    if (!numbers.IsEmpty()) {
        // searches through the comma-separated list for a match,
        // return TRUE if one is found.
        AutoPtr< ArrayOf<String> > numArr;
        StringUtils::Split(numbers, String(","), (ArrayOf<String>**)&numArr);
        for (Int32 i = 0; i < numArr->GetLength(); i++) {
            String emergencyNum = (*numArr)[i];
            // It is not possible to append additional digits to an emergency number to dial
            // the number in Brazil - it won't connect.
            if (useExactMatch || String("BR").CompareIgnoreCase(defaultCountryIso)) {
                if (number.Equals(emergencyNum)) {
                    *res = TRUE;
                    return NOERROR;
                }
            } else {
                if (number.StartWith(emergencyNum)) {
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
        // no matches found against the list!
        *res = FALSE;
        return NOERROR;
    }

    Slogger::D(LOG_TAG, "System property doesn't provide any emergency numbers. Use embedded logic for determining ones.");

    // No ecclist system property, so use our own list.
    if (!defaultCountryIso.IsNull()) {
        /*AutoPtr<ShortNumberUtil> util = new ShortNumberUtil();
        if (useExactMatch) {
            return util.isEmergencyNumber(number, defaultCountryIso);
        } else {
            return util.connectsToEmergencyNumber(number, defaultCountryIso);
        }*/
    } else {
        if (useExactMatch) {
            *res = (number.Equals("112") || number.Equals("911"));
            return NOERROR;
        } else {
            *res = (number.StartWith("112") || number.StartWith("911"));
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CPhoneNumberUtils::IsLocalEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtils::IsTwoToNine(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

ECode CPhoneNumberUtils::IsNanp(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean retVal = FALSE;
    if (!dialStr.IsNull()) {
        if (dialStr.GetLength() == NANP_LENGTH) {
            Boolean tempRes;
            if ((IsTwoToNine(dialStr.GetChar(0), &tempRes), tempRes) &&
                (IsTwoToNine(dialStr.GetChar(3), &tempRes), tempRes)) {
                retVal = TRUE;
                for (Int32 i=1; i < NANP_LENGTH; i++ ) {
                    Char32 c=dialStr.GetChar(i);
                    if ((IsISODigit(c, &tempRes), !tempRes)) {
                        retVal = FALSE;
                        break;
                    }
                }
            }
        }
    } else {
        Slogger::E(LOG_TAG, "isNanp: NULL dialStr passed in %s", dialStr.string());
    }
    *res = retVal;
    return NOERROR;
}

ECode CPhoneNumberUtils::IsOneNanp(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean retVal = FALSE;
    if (!dialStr.IsNull()) {
        String newDialStr = dialStr.Substring(1);
        Boolean tempRes;
        if ((dialStr.GetChar(0) == '1') && (IsNanp(newDialStr, &tempRes), tempRes)) {
            retVal = TRUE;
        }
    } else {
        Slogger::E(LOG_TAG, "isOneNanp: NULL dialStr passed in %s", dialStr.string());
    }
    *res = retVal;
    return NOERROR;
}

ECode CPhoneNumberUtils::MatchIntlPrefix(
    /* [in] */ const String& a,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    /* '([^0-9*#+pwn]\+[^0-9*#+pwn] | [^0-9*#+pwn]0(0|11)[^0-9*#+pwn] )$' */
    /*        0       1                           2 3 45               */
    Boolean tempRes;
    Int32 state = 0;
    for (Int32 i = 0 ; i < len ; i++) {
        Char32 c = a.GetChar(i);

        switch (state) {
            case 0:
                if      (c == '+') state = 1;
                else if (c == '0') state = 2;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            case 2:
                if      (c == '0') state = 3;
                else if (c == '1') state = 4;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            case 4:
                if      (c == '1') state = 5;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            default:
                if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

        }
    }

    *res = (state == 1 || state == 3 || state == 5);
    return NOERROR;
}

ECode CPhoneNumberUtils::MatchIntlPrefixAndCC(
    /* [in] */ const String& a,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    /*  [^0-9*#+pwn]*(\+|0(0|11)\d\d?\d? [^0-9*#+pwn] $ */
    /*      0          1 2 3 45  6 7  8                 */

    Int32 state = 0;
    Boolean tempRes;
    for (Int32 i = 0 ; i < len ; i++ ) {
        Char32 c = a.GetChar(i);

        switch (state) {
            case 0:
                if      (c == '+') state = 1;
                else if (c == '0') state = 2;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            case 2:
                if      (c == '0') state = 3;
                else if (c == '1') state = 4;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            case 4:
                if      (c == '1') state = 5;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            case 1:
            case 3:
            case 5:
                if      (IsISODigit(c, &tempRes), tempRes) state = 6;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            case 6:
            case 7:
                if      (IsISODigit(c, &tempRes), tempRes) state++;
                else if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
            break;

            default:
                if (IsNonSeparator(c, &tempRes), tempRes) {
                    *res = FALSE;
                    return NOERROR;
                }
        }
    }

    *res = (state == 6 || state == 7 || state == 8);
    return NOERROR;
}

ECode CPhoneNumberUtils::MatchTrunkPrefix(
    /* [in] */ const String& a,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean found, tempRes;

    found = FALSE;

    for (Int32 i = 0 ; i < len ; i++) {
        Char32 c = a.GetChar(i);

        if (c == '0' && !found) {
            found = TRUE;
        } else if (IsNonSeparator(c, &tempRes), tempRes) {
            *res = FALSE;
            return NOERROR;
        }
    }

    *res = found;
    return NOERROR;
}

ECode CPhoneNumberUtils::IsCountryCallingCode(
    /* [in] */ Int32 countryCallingCodeCandidate,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (countryCallingCodeCandidate > 0 && countryCallingCodeCandidate < CCC_LENGTH &&
            COUNTRY_CALLING_CALL[countryCallingCodeCandidate]);

    return NOERROR;
}

ECode CPhoneNumberUtils::CheckPrefixIsIgnorable(
    /* [in] */ const String& str,
    /* [in] */ Int32 forwardIndex,
    /* [in] */ Int32 backwardIndex,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean trunk_prefix_was_read = FALSE;
    Int32 tmp;
    Boolean tempRes;
    while (backwardIndex >= forwardIndex) {
        if (TryGetISODigit(str.GetChar(backwardIndex), &tmp), tmp >= 0) {
            if (trunk_prefix_was_read) {
                // More than one digit appeared, meaning that "a" and "b"
                // is different.
                *res = FALSE;
                return NOERROR;
            } else {
                // Ignore just one digit, assuming it is trunk prefix.
                trunk_prefix_was_read = TRUE;
            }
        } else if (IsDialable(str.GetChar(backwardIndex), &tempRes), tempRes) {
            // Trunk prefix is a digit, not "*", "#"...
            *res = FALSE;
            return NOERROR;
        }
        backwardIndex--;
    }

    *res = TRUE;
    return NOERROR;
}

ECode CPhoneNumberUtils::AppendPwCharBackToOrigDialStr(
    /* [in] */ Int32 dialableIndex,
    /* [in] */ const String& origStr,
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    String retStr;

    // There is only 1 P/W character before the dialable characters
    if (dialableIndex == 1) {
        StringBuilder ret(origStr);
        ret.AppendChar(dialStr.GetChar(0));
        retStr = ret.ToString();
    } else {
        // It means more than 1 P/W characters in the post dial string,
        // appends to retStr
        String nonDigitStr = dialStr.Substring(0,dialableIndex);
        retStr = origStr + nonDigitStr;
    }
    *res = retStr;
    return NOERROR;
}

ECode CPhoneNumberUtils::GetDefaultIdp(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    String ps;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Get(ITelephonyProperties::PROPERTY_IDP_STRING, ps, &ps);
    if (ps.IsNullOrEmpty()) {
        ps = NANP_IDP_STRING;
    }
    *res = ps;
    return NOERROR;
}

ECode CPhoneNumberUtils::InternalGetStrippedReversed(
    /* [in] */ const String& np,
    /* [in] */ Int32 numDigits,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    if (np.IsNull()) {
        *res = String(NULL);
        return NOERROR;
    }

    StringBuilder ret(numDigits);
    Int32 length = np.GetLength();

    for (Int32 i = length - 1, s = length
        ; i >= 0 && (s - i) <= numDigits ; i--) {
        Char32 c = np.GetChar(i);

        ret.AppendChar(c);
    }

    *res = ret.ToString();
    return NOERROR;
}

ECode CPhoneNumberUtils::ProcessPlusCodeWithinNanp(
    /* [in] */ const String& networkDialStr,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    String retStr = networkDialStr;

    if (DBG) Slogger::I(LOG_TAG, "processPlusCodeWithinNanp,networkDialStr=%s", networkDialStr.string());
    // If there is a plus sign at the beginning of the dial string,
    // Convert the plus sign to the default IDP since it's an international number
    if (!networkDialStr.IsNull() &&
        networkDialStr.GetChar(0) == PLUS_SIGN_CHAR &&
        networkDialStr.GetLength() > 1) {
        String newStr = networkDialStr.Substring(1);
        Boolean tempRes;
        if ((IsOneNanp(newStr, &tempRes), tempRes)) {
            // Remove the leading plus sign
            retStr = newStr;
         } else {
             String idpStr;
             GetDefaultIdp(&idpStr);
             // Replaces the plus sign with the default IDP
             StringUtils::ReplaceFirst(networkDialStr, String("[+]"), idpStr, &retStr);
        }
    }
    if (DBG) Slogger::I(LOG_TAG, "processPlusCodeWithinNanp,retStr=%s", retStr.string());
    *res = retStr;
    return NOERROR;
}

ECode CPhoneNumberUtils::CharToBCD(
    /* [in] */ Char32 c,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    if (c >= '0' && c <= '9') {
        *res = c - '0';
        return NOERROR;
    } else if (c == '*') {
        *res = 0xa;
        return NOERROR;
    } else if (c == '#') {
        *res = 0xb;
        return NOERROR;
    } else if (c == IPhoneNumberUtils::PAUSE) {
        *res = 0xc;
        return NOERROR;
    } else if (c == IPhoneNumberUtils::WILD) {
        *res = 0xd;
        return NOERROR;
    } else {
        //throw new RuntimeException ("invalid char for BCD " + c);
        return E_RUNTIME_EXCEPTION;
    }
}

ECode CPhoneNumberUtils::FindDialableIndexFromPostDialStr(
    /* [in] */ const String& postDialStr,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean tempRes;
    for (Int32 index = 0;index < postDialStr.GetLength(); index++) {
         Char32 c = postDialStr.GetChar(index);
         if (IsReallyDialable(c, &tempRes), tempRes) {
            *res = index;
            return NOERROR;
         }
    }
    *res = -1;
    return NOERROR;
}

ECode CPhoneNumberUtils::GetFormatTypeFromCountryCode(
    /* [in] */ const String& country,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    // Check for the NANP countries
    Int32 length = NANP_COUNTRIES_LEN;
    for (Int32 i = 0; i < length; i++) {
        if (NANP_COUNTRIES[i].CompareIgnoreCase(country) == 0) {
            *res = IPhoneNumberUtils::FORMAT_NANP;
            return NOERROR;
        }
    }
    if (String("jp").CompareIgnoreCase(country) == 0) {
        *res = IPhoneNumberUtils::FORMAT_JAPAN;
    }
    *res = IPhoneNumberUtils::FORMAT_UNKNOWN;
    return NOERROR;
}

ECode CPhoneNumberUtils::TryGetISODigit(
    /* [in] */ Char32 ch,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    if ('0' <= ch && ch <= '9') {
        *res = ch - '0';
        return NOERROR;
    } else {
        *res = -1;
        return NOERROR;
    }
}

ECode CPhoneNumberUtils::TryGetTrunkPrefixOmittedIndex(
    /* [in] */ const String& str,
    /* [in] */ Int32 currentIndex,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 length = str.GetLength(), tmp;
    Boolean tmpRes;
    for (Int32 i = currentIndex ; i < length ; i++) {
        Char32 ch = str.GetChar(i);
        if (TryGetISODigit(ch, &tmp), tmp >= 0) {
            *res = i + 1;
            return NOERROR;
        } else if (IsDialable(ch, &tmpRes), tmpRes) {
            *res = tmpRes;
            return NOERROR;
        }
    }
    *res = -1;
    return NOERROR;
}

ECode CPhoneNumberUtils::InternalCalledPartyBCDFragmentToString(
    /* [in] */ StringBuilder* sb,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    for(Int32 i = offset ; i < length + offset ; i++) {
        Byte b;
        Char32 c;

        BcdToChar((Byte)((*bytes)[i] & 0xf), &c);

        if (c == 0) {
            return NOERROR;
        }
        sb->AppendChar(c);

        // FIXME(mkf) TS 23.040 9.1.2.3 says
        // "if a mobile receives 1111 in a position prior to
        // the last semi-octet then processing shall commence with
        // the next semi-octet and the intervening
        // semi-octet shall be ignored"
        // How does this jive with 24.008 10.5.4.7

        b = (byte)(((*bytes)[i] >> 4) & 0xf);

        if (b == 0xf && i + 1 == length + offset) {
            //ignore final 0xf
            break;
        }

        BcdToChar(b, &c);
        if (c == 0) {
            return NOERROR;
        }

        sb->AppendChar(c);
    }
    return NOERROR;
}

ECode CPhoneNumberUtils::Log(
    /* [in] */ const String& msg)
{
    Slogger::D(LOG_TAG, msg);
    return NOERROR;
}

ECode CPhoneNumberUtils::IndexOfLastNetworkChar(
    /* [in] */ const String& a,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    Int32 pIndex, wIndex;
    Int32 origLength;
    Int32 trimIndex;

    origLength = a.GetLength();

    pIndex = a.IndexOf(IPhoneNumberUtils::PAUSE);
    wIndex = a.IndexOf(IPhoneNumberUtils::WAIT);

    MinPositive(pIndex, wIndex, &trimIndex);

    if (trimIndex < 0) {
        *val = origLength - 1;
        return NOERROR;
    } else {
        *val = trimIndex - 1;
        return NOERROR;
    }
}

ECode CPhoneNumberUtils::RemoveDashes(
    /* [in] */ IEditable* text)
{
    return NOERROR;
}

ECode CPhoneNumberUtils::NumberToCalledPartyBCDHelper(
    /* [in] */ const String& number,
    /* [in] */ Boolean includeLength,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    Int32 numberLenReal = number.GetLength();
    Int32 numberLenEffective = numberLenReal;
    Boolean hasPlus = number.IndexOf('+') != -1;
    if (hasPlus) numberLenEffective--;

    if (numberLenEffective == 0) {
        *res = NULL;
        return NOERROR;
    }

    Int32 resultLen = (numberLenEffective + 1) / 2;  // Encoded numbers require only 4 bits each.
    Int32 extraBytes = 1;                            // Prepended TOA byte.
    if (includeLength) extraBytes++;               // Optional prepended length byte.
    resultLen += extraBytes;

    AutoPtr< ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(resultLen);
    Int32 tmp;

    Int32 digitCount = 0;
    for (Int32 i = 0; i < numberLenReal; i++) {
        Char32 c = number.GetChar(i);
        if (c == '+') continue;
        Int32 shift = ((digitCount & 0x01) == 1) ? 4 : 0;
        (*result)[extraBytes + (digitCount >> 1)] |= (Byte)(((CharToBCD(c, &tmp), tmp) & 0x0F) << shift);
        digitCount++;
    }

    // 1-fill any trailing odd nibble/quartet.
    if ((digitCount & 0x01) == 1) (*result)[extraBytes + (digitCount >> 1)] |= 0xF0;

    Int32 offset = 0;
    if (includeLength) (*result)[offset++] = (Byte)(resultLen - 1);
    (*result)[offset] = (Byte)(hasPlus ? IPhoneNumberUtils::TOA_International : IPhoneNumberUtils::TOA_Unknown);

    *res = result;
    REFCOUNT_ADD(*res);

    return NOERROR;
}

ECode CPhoneNumberUtils::BcdToChar(
    /* [in] */ Byte b,
    /* [out] */ Char32* res)
{
    VALIDATE_NOT_NULL(res);

    if (b < 0xa) {
        *res = (Char32)('0' + b);
    } else switch (b) {
        case 0xa: *res = '*';
        case 0xb: *res = '#';
        case 0xc: *res = IPhoneNumberUtils::PAUSE;
        case 0xd: *res = IPhoneNumberUtils::WILD;

        default: *res = 0;
    }
    return NOERROR;
}

ECode CPhoneNumberUtils::TryGetCountryCallingCodeAndNewIndex(
    /* [in] */ const String& str,
    /* [in] */ Boolean acceptThailandCase,
    /* [out] */ CountryCallingCodeAndNewIndex** cccani)
{
    VALIDATE_NOT_NULL(cccani);

    // Rough regexp:
    //  ^[^0-9*#+]*((\+|0(0|11)\d\d?|166) [^0-9*#+] $
    //         0        1 2 3 45  6 7  89
    //
    // In all the states, this function ignores separator characters.
    // "166" is the special case for the call from Thailand to the US. Uguu!
    Int32 state = 0;
    Int32 ccc = 0;
    Int32 length = str.GetLength();
    Boolean tempRes;
    for (Int32 i = 0 ; i < length ; i++ ) {
        Char32 ch = str.GetChar(i);
        switch (state) {
            case 0:
                if      (ch == '+') state = 1;
                else if (ch == '0') state = 2;
                else if (ch == '1') {
                    if (acceptThailandCase) {
                        state = 8;
                    } else {
                        *cccani = NULL;
                        return NOERROR;
                    }
                } else if (IsDialable(ch, &tempRes), tempRes) {
                    *cccani = NULL;
                    return NOERROR;
                }
            break;

            case 2:
                if      (ch == '0') state = 3;
                else if (ch == '1') state = 4;
                else if (IsDialable(ch, &tempRes), tempRes) {
                    *cccani = NULL;
                    return NOERROR;
                }
            break;

            case 4:
                if      (ch == '1') state = 5;
                else if (IsDialable(ch, &tempRes), tempRes) {
                    *cccani = NULL;
                    return NOERROR;
                }
            break;

            case 1:
            case 3:
            case 5:
            case 6:
            case 7:
                {
                    Int32 ret;
                    TryGetISODigit(ch, &ret);
                    if (ret > 0) {
                        ccc = ccc * 10 + ret;
                        if (ccc >= 100 || (IsCountryCallingCode(ccc, &tempRes), tempRes)) {
                            *cccani = new CountryCallingCodeAndNewIndex(ccc, i + 1);
                            REFCOUNT_ADD(*cccani);
                        }
                        if (state == 1 || state == 3 || state == 5) {
                            state = 6;
                        } else {
                            state++;
                        }
                    } else if (IsDialable(ch, &tempRes), tempRes) {
                        *cccani = NULL;
                        return NOERROR;
                    }
                }
                break;
            case 8:
                if (ch == '6') state = 9;
                else if (IsDialable(ch, &tempRes), tempRes) {
                    *cccani = NULL;
                    return NOERROR;
                }
                break;
            case 9:
                if (ch == '6') {
                    *cccani = new CountryCallingCodeAndNewIndex(66, i + 1);
                    REFCOUNT_ADD(*cccani);
                } else {
                    *cccani = NULL;
                    return NOERROR;
                }
            default:
                *cccani = NULL;
                return NOERROR;
        }
    }

    *cccani = NULL;
    return NOERROR;
}

ECode CPhoneNumberUtils::MinPositive(
    /* [in] */ Int32 a,
    /* [in] */ Int32  b,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    if (a >= 0 && b >= 0) {
        *val = (a < b) ? a : b;
        return NOERROR;
    } else if (a >= 0) { /* && b < 0 */
        *val = a;
        return NOERROR;
    } else if (b >= 0) { /* && a < 0 */
        *val = b;
        return NOERROR;
    } else { /* a < 0 && b < 0 */
        *val = -1;
    }

    return NOERROR;
}

}
}
}

