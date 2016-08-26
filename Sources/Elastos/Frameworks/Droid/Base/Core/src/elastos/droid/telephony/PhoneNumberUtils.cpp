#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryDetector;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String PhoneNumberUtils::TAG("PhoneNumberUtils");
const String PhoneNumberUtils::PLUS_SIGN_STRING("+");
const String PhoneNumberUtils::NANP_IDP_STRING("011");
const String PhoneNumberUtils::CLIR_ON("*31#");
const String PhoneNumberUtils::CLIR_OFF("#31#");

const Boolean PhoneNumberUtils::DBG = FALSE;
const Char32 PhoneNumberUtils::PLUS_SIGN_CHAR = '+';
const Int32 PhoneNumberUtils::NANP_LENGTH = 10;
const Int32 PhoneNumberUtils::MIN_MATCH = 7;

const Int32 PhoneNumberUtils::NANP_STATE_DIGIT = 1;
const Int32 PhoneNumberUtils::NANP_STATE_PLUS = 2;
const Int32 PhoneNumberUtils::NANP_STATE_ONE = 3;
const Int32 PhoneNumberUtils::NANP_STATE_DASH = 4;

const Boolean PhoneNumberUtils::COUNTRY_CALLING_CALL[] = {
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

const Int32 PhoneNumberUtils::CCC_LENGTH = sizeof(COUNTRY_CALLING_CALL) / sizeof(Boolean);

static AutoPtr<ArrayOf<String> > InitNANP_COUNTRIES()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(24);
    array->Set(0, String("US")); // United States
    array->Set(1, String("CA")); // Canada
    array->Set(2, String("AS")); // American Samoa
    array->Set(3, String("AI")); // Anguilla
    array->Set(4, String("AG")); // Antigua and Barbuda
    array->Set(5, String("BS")); // Bahamas
    array->Set(6, String("BB")); // Barbados
    array->Set(7, String("BM")); // Bermuda
    array->Set(8, String("VG")); // British Virgin Islands
    array->Set(9, String("KY")); // Cayman Islands
    array->Set(10, String("DM")); // Dominica
    array->Set(11, String("DO")); // Dominican Republic
    array->Set(12, String("GD")); // Grenada
    array->Set(13, String("GU")); // Guam
    array->Set(14, String("JM")); // Jamaica
    array->Set(15, String("PR")); // Puerto Rico
    array->Set(16, String("MS")); // Montserrat
    array->Set(17, String("MP")); // Northern Mariana Islands
    array->Set(18, String("KN")); // Saint Kitts and Nevis
    array->Set(19, String("LC")); // Saint Lucia
    array->Set(20, String("VC")); // Saint Vincent and the Grenadines
    array->Set(21, String("TT")); // Trinidad and Tobago
    array->Set(22, String("TC")); // Turks and Caicos Islands
    array->Set(23, String("VI")); // U.S. Virgin Islands
    return array;
}

AutoPtr<ArrayOf<String> > PhoneNumberUtils::NANP_COUNTRIES = InitNANP_COUNTRIES();

static AutoPtr<IPattern> InitGLOBAL_PHONE_NUMBER_PATTERN() {
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> p;
    helper->Compile(String("[\\+]?[0-9.-]+"), (IPattern**)&p);
    return p;
}

const AutoPtr<IPattern> PhoneNumberUtils::GLOBAL_PHONE_NUMBER_PATTERN = InitGLOBAL_PHONE_NUMBER_PATTERN();

static AutoPtr<ISparseInt32Array> InitKEYPAD_MAP()
{
    AutoPtr<ISparseInt32Array> array;
    CSparseInt32Array::New((ISparseInt32Array**)&array);

    array->Put('a', '2');
    array->Put('b', '2');
    array->Put('c', '2');
    array->Put('A', '2');
    array->Put('B', '2');
    array->Put('C', '2');

    array->Put('d', '3');
    array->Put('e', '3');
    array->Put('f', '3');
    array->Put('D', '3');
    array->Put('E', '3');
    array->Put('F', '3');

    array->Put('g', '4');
    array->Put('h', '4');
    array->Put('i', '4');
    array->Put('G', '4');
    array->Put('H', '4');
    array->Put('I', '4');

    array->Put('j', '5');
    array->Put('k', '5');
    array->Put('l', '5');
    array->Put('J', '5');
    array->Put('K', '5');
    array->Put('L', '5');

    array->Put('m', '6');
    array->Put('n', '6');
    array->Put('o', '6');
    array->Put('M', '6');
    array->Put('N', '6');
    array->Put('O', '6');

    array->Put('p', '7');
    array->Put('q', '7');
    array->Put('r', '7');
    array->Put('s', '7');
    array->Put('P', '7');
    array->Put('Q', '7');
    array->Put('R', '7');
    array->Put('S', '7');

    array->Put('t', '8');
    array->Put('u', '8');
    array->Put('v', '8');
    array->Put('T', '8');
    array->Put('U', '8');
    array->Put('V', '8');

    array->Put('w', '9');
    array->Put('x', '9');
    array->Put('y', '9');
    array->Put('z', '9');
    array->Put('W', '9');
    array->Put('X', '9');
    array->Put('Y', '9');
    array->Put('Z', '9');

    return array;
}

AutoPtr<ISparseInt32Array> PhoneNumberUtils::KEYPAD_MAP = InitKEYPAD_MAP();

PhoneNumberUtils::CountryCallingCodeAndNewIndex::CountryCallingCodeAndNewIndex(
    /* [in] */ Int32 countryCode,
    /* [in] */ Int32 newIndex)
{
    mCountryCallingCode = countryCode;
    mNewIndex = newIndex;
}

ECode PhoneNumberUtils::IsISODigit(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c >= '0' && c <= '9');
    return NOERROR;
}

ECode PhoneNumberUtils::Is12Key(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#');
    return NOERROR;
}

ECode PhoneNumberUtils::IsDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+' || c == IPhoneNumberUtils::WILD);
    return NOERROR;
}

ECode PhoneNumberUtils::IsReallyDialable(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+');
    return NOERROR;
}

ECode PhoneNumberUtils::IsNonSeparator(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ((c >= '0' && c <= '9') || c == '*' || c == '#' || c == '+'
                    || c == IPhoneNumberUtils::WILD || c == IPhoneNumberUtils::WAIT || c == IPhoneNumberUtils::PAUSE);
    return NOERROR;
}

ECode PhoneNumberUtils::IsStartsPostDial(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c== IPhoneNumberUtils::PAUSE || c == IPhoneNumberUtils::WAIT);
    return NOERROR;
}

ECode PhoneNumberUtils::GetNumberFromIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IContext* context,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String number;

    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    if (uri == NULL) {
        *res = String(NULL);
        return NOERROR;
    }

    String scheme;
    uri->GetScheme(&scheme);

    if (scheme.Equals(String("tel")) || scheme.Equals(String("sip"))) {
        uri->GetSchemeSpecificPart(res);
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
        ICloseable::Probe(c)->Close();

        // } finally {
        //     c.close();
        // }
    }

    // *res = number;
    return NOERROR;
}

ECode PhoneNumberUtils::ExtractNetworkPortion(
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
            ret.Append(digit);
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

ECode PhoneNumberUtils::ExtractNetworkPortionAlt(
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

ECode PhoneNumberUtils::StripSeparators(
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
            ret.Append(digit);
        } else if (IsNonSeparator(c, &tempRes), tempRes) {
            ret.AppendChar(c);
        }
    }

    *res = ret.ToString();
    return NOERROR;
}

ECode PhoneNumberUtils::ConvertAndStrip(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String str;
    ConvertKeypadLettersToDigits(phoneNumber, &str);
    return StripSeparators(str, res);
}

ECode PhoneNumberUtils::ConvertPreDial(
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

ECode PhoneNumberUtils::ExtractPostDialPortion(
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

ECode PhoneNumberUtils::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    // We've used loose comparation at least Eclair, which may change in the future.

    return Compare(a, b, FALSE, res);
}

ECode PhoneNumberUtils::Compare(
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

ECode PhoneNumberUtils::Compare(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [in] */ Boolean useStrictComparation,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return (useStrictComparation ? CompareStrictly(a, b, res) : CompareLoosely(a, b, res));
}

ECode PhoneNumberUtils::CompareLoosely(
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

ECode PhoneNumberUtils::CompareStrictly(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return CompareStrictly(a, b, TRUE, res);
}

ECode PhoneNumberUtils::CompareStrictly(
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

ECode PhoneNumberUtils::ToCallerIDMinMatch(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    String np;
    ExtractNetworkPortionAlt(phoneNumber, &np);
    return InternalGetStrippedReversed(np, MIN_MATCH, res);
}

ECode PhoneNumberUtils::GetStrippedReversed(
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

ECode PhoneNumberUtils::StringFromStringAndTOA(
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

ECode PhoneNumberUtils::ToaFromString(
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

ECode PhoneNumberUtils::CalledPartyBCDToString(
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
        //Logger::E("leliang", "line:%d, func:%s, retString:%s\n", __LINE__, __func__, retString.string());
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        AutoPtr<IPattern> p;
        helper->Compile(String("(^[#*])(.*)([#*])(.*)(#)$"), (IPattern**)&p);
        AutoPtr<IMatcher> m;
        p->Matcher(retString, (IMatcher**)&m);
        Boolean tempRes;
        String str;

        if (m->Matches(&tempRes), tempRes) {
            if (String("").Equals((IMatchResult::Probe(m)->Group(2, &str), str))) {
                // Started with two [#*] ends with #
                // So no dialing number and we'll just
                // append a +, this handles **21#+
                ret = NULL;
                ret = new StringBuilder();
                ret->Append((IMatchResult::Probe(m)->Group(1, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(3, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(4, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(5, &str), str));
                ret->Append("+");
            } else {
                // Starts with [#*] and ends with #
                // Assume group 4 is a dialing number
                // such as *21*+1234554#
                ret = NULL;
                ret = new StringBuilder();
                ret->Append((IMatchResult::Probe(m)->Group(1, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(2, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(3, &str), str));
                ret->Append("+");
                ret->Append((IMatchResult::Probe(m)->Group(4, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(5, &str), str));
            }
        } else {
            p = NULL;
            m = NULL;
            helper->Compile(String("(^[#*])(.*)([#*])(.*)"), (IPattern**)&p);
            p->Matcher(retString, (IMatcher**)&m);
            if (m->Matches(&tempRes), tempRes){
                // Starts with [#*] and only one other [#*]
                // Assume the data after last [#*] is dialing
                // number (i.e. group 4) such as *31#+11234567890.
                // This also includes the odd ball *21#+
                ret = NULL;
                ret = new StringBuilder();
                ret->Append((IMatchResult::Probe(m)->Group(1, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(2, &str), str));
                ret->Append((IMatchResult::Probe(m)->Group(3, &str), str));
                ret->Append("+");
                ret->Append((IMatchResult::Probe(m)->Group(4, &str), str));
            } else {
                // Does NOT start with [#*] just prepend '+'
                ret = NULL;
                ret = new StringBuilder();
                ret->AppendChar('+');
                ret->Append(retString);
            }
        }
    }

    *res = ret->ToString();

    return NOERROR;
}

ECode PhoneNumberUtils::CalledPartyBCDFragmentToString(
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

ECode PhoneNumberUtils::IsWellFormedSmsAddress(
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

ECode PhoneNumberUtils::IsGlobalPhoneNumber(
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

ECode PhoneNumberUtils::NetworkPortionToCalledPartyBCD(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    String networkPortion;
    ExtractNetworkPortion(s, &networkPortion);
    return NumberToCalledPartyBCDHelper(networkPortion, FALSE, res);
}

ECode PhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    String networkPortion;
    ExtractNetworkPortion(s, &networkPortion);
    return NumberToCalledPartyBCDHelper(networkPortion, TRUE, res);
}

ECode PhoneNumberUtils::NumberToCalledPartyBCD(
    /* [in] */ const String& number,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res);
    return NumberToCalledPartyBCDHelper(number, FALSE, res);
}

ECode PhoneNumberUtils::FormatNumber(
    /* [in] */ const String& source,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<ICharSequence> cs;
    CString::New(source, (ICharSequence**)&cs);
    AutoPtr<ISpannableStringBuilder> text;
    CSpannableStringBuilder::New(cs, (ISpannableStringBuilder**)&text);

    Int32 defaultFormattingType;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    GetFormatTypeForLocale(locale, &defaultFormattingType);
    AutoPtr<IEditable> e;
    FormatNumber(IEditable::Probe(text), defaultFormattingType, (IEditable**)&e);
    ICharSequence::Probe(e)->ToString(res);
    return NOERROR;
}

ECode PhoneNumberUtils::FormatNumber(
    /* [in] */ const String& source,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<ICharSequence> cs;
    CString::New(source, (ICharSequence**)&cs);
    AutoPtr<ISpannableStringBuilder> text;
    CSpannableStringBuilder::New(cs, (ISpannableStringBuilder**)&text);
    AutoPtr<IEditable> e;
    FormatNumber(IEditable::Probe(text), defaultFormattingType, (IEditable**)&e);
    ICharSequence::Probe(e)->ToString(res);
    return NOERROR;
}

ECode PhoneNumberUtils::GetFormatTypeForLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    String country;
    locale->GetCountry(&country);

    return GetFormatTypeFromCountryCode(country, res);
}

ECode PhoneNumberUtils::FormatNumber(
    /* [in] */ IEditable* _text,
    /* [in] */ Int32 defaultFormattingType,
    /* [out] */ IEditable** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IEditable> text = _text;
    Int32 formatType = defaultFormattingType;

    Int32 len;
    ICharSequence::Probe(text)->GetLength(&len);
    Char32 c;
    if (len > 2 && (ICharSequence::Probe(text)->GetCharAt(0, &c), c) == '+') {
        if ((ICharSequence::Probe(text)->GetCharAt(1, &c), c) == '1') {
            formatType = IPhoneNumberUtils::FORMAT_NANP;
        }
        else if (len >= 3 &&
                (ICharSequence::Probe(text)->GetCharAt(1, &c), c) == '8' &&
                (ICharSequence::Probe(text)->GetCharAt(2, &c), c) == '1') {
            formatType = IPhoneNumberUtils::FORMAT_JAPAN;
        }
        else {
            formatType = IPhoneNumberUtils::FORMAT_UNKNOWN;
        }
    }

    switch (formatType) {
        case IPhoneNumberUtils::FORMAT_NANP:
            FormatNanpNumber(text, (IEditable**)&text);
            break;
        case IPhoneNumberUtils::FORMAT_JAPAN:
            FormatJapaneseNumber(text, (IEditable**)&text);
            break;
        case IPhoneNumberUtils::FORMAT_UNKNOWN:
            RemoveDashes(text, (IEditable**)&text);
            break;
    }

    *result = text;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneNumberUtils::FormatNanpNumber(
    /* [in] */ IEditable* _text,
    /* [out] */ IEditable** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IEditable> text = _text;

    Int32 length;
    ICharSequence::Probe(text)->GetLength(&length);
    if (length > String("+1-nnn-nnn-nnnn").GetLength()) {
        // The string is too long to be formatted
        return NOERROR;
    }
    else if (length <= 5) {
        // The string is either a shortcode or too short to be formatted
        return NOERROR;
    }

    AutoPtr<ICharSequence> saved;
    ICharSequence::Probe(text)->SubSequence(0, length, (ICharSequence**)&saved);

    // Strip the dashes first, as we're going to add them back
    RemoveDashes(text, (IEditable**)&text);
    ICharSequence::Probe(text)->GetLength(&length);

    // When scanning the number we record where dashes need to be added,
    // if they're non-0 at the end of the scan the dashes will be added in
    // the proper places.
    AutoPtr<ArrayOf<Int32> > dashPositions = ArrayOf<Int32>::Alloc(3);
    Int32 numDashes = 0;

    Int32 state = NANP_STATE_DIGIT;
    Int32 numDigits = 0;
    for (Int32 i = 0; i < length; i++) {
        Char32 c;
        ICharSequence::Probe(text)->GetCharAt(i, &c);
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
                    text->Replace(0, length, saved);

                    *result = text;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
                else if (state == NANP_STATE_ONE) {
                    // Found either +1 or 1, follow it up with a dash
                    dashPositions->Set(numDashes++, i);
                }
                else if (state != NANP_STATE_DASH && (numDigits == 3 || numDigits == 6)) {
                    // Found a digit that should be after a dash that isn't
                    dashPositions->Set(numDashes++, i);
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
                text->Replace(0, length, saved);

                *result = text;
                REFCOUNT_ADD(*result)
                return NOERROR;
        }
    }

    if (numDigits == 7) {
        // With 7 digits we want xxx-xxxx, not xxx-xxx-x
        numDashes--;
    }

    // Actually put the dashes in place
    for (Int32 i = 0; i < numDashes; i++) {
        Int32 pos = (*dashPositions)[i];
        AutoPtr<ICharSequence> cs;
        CString::New(String("-"), (ICharSequence**)&cs);
        text->Replace(pos + i, pos + i, cs);
    }

    // Remove trailing dashes
    Int32 len;
    ICharSequence::Probe(text)->GetLength(&len);
    while (len > 0) {
        Char32 c;
        ICharSequence::Probe(text)->GetCharAt(len - 1, &c);
        if (c == '-') {
            text->Delete(len - 1, len);
            len--;
        }
        else {
            break;
        }
    }

    *result = text;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneNumberUtils::FormatJapaneseNumber(
    /* [in] */ IEditable* text,
    /* [out] */ IEditable** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode PhoneNumberUtils::FormatNumberToE164(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    *res = String("");
    Logger::D(TAG, "[TODO] Need i18n.phonenumbers.PhoneNumberUtil");
// TODO: Need i18n.phonenumbers.PhoneNumberUtil
    // PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    // String result = null;
    // try {
    //     PhoneNumber pn = util.parse(phoneNumber, defaultCountryIso);
    //     if (util.isValidNumber(pn)) {
    //         result = util.format(pn, PhoneNumberFormat.E164);
    //     }
    // } catch (NumberParseException e) {
    // }
    // return result;
    return E_NOT_IMPLEMENTED;
}

ECode PhoneNumberUtils::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    // Do not attempt to format numbers that start with a hash or star symbol.
    if (phoneNumber.StartWith("#") || phoneNumber.StartWith("*")) {
        *res = phoneNumber;
        return NOERROR;
    }

    assert(0);
// TODO: Need i18n.phonenumbers.PhoneNumberUtil
    // PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    // String result = null;
    // try {
    //     PhoneNumber pn = util.parseAndKeepRawInput(phoneNumber, defaultCountryIso);
    //     result = util.formatInOriginalFormat(pn, defaultCountryIso);
    // } catch (NumberParseException e) {
    // }
    // return result;
    return E_NOT_IMPLEMENTED;
}

ECode PhoneNumberUtils::FormatNumber(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& phoneNumberE164,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 len = phoneNumber.GetLength();
    for (Int32 i = 0; i < len; i++) {
        Boolean b;
        if (IsDialable(phoneNumber.GetChar(i), &b), b) {
            *res = phoneNumber;
            return NOERROR;
        }
    }

    assert(0);
// TODO: Need i18n.phonenumbers.PhoneNumberUtil
    // PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    // // Get the country code from phoneNumberE164
    // if (phoneNumberE164 != null && phoneNumberE164.length() >= 2
    //         && phoneNumberE164.charAt(0) == '+') {
    //     try {
    //         // The number to be parsed is in E164 format, so the default region used doesn't
    //         // matter.
    //         PhoneNumber pn = util.parse(phoneNumberE164, "ZZ");
    //         String regionCode = util.getRegionCodeForNumber(pn);
    //         if (!TextUtils.isEmpty(regionCode) &&
    //             // This makes sure phoneNumber doesn't contain an IDD
    //             normalizeNumber(phoneNumber).indexOf(phoneNumberE164.substring(1)) <= 0) {
    //             defaultCountryIso = regionCode;
    //         }
    //     } catch (NumberParseException e) {
    //     }
    // }
    // String result = formatNumber(phoneNumber, defaultCountryIso);
    // return result != null ? result : phoneNumber;
    return E_NOT_IMPLEMENTED;
}

ECode PhoneNumberUtils::NormalizeNumber(
    /* [in] */ const String& phoneNumber,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    if (phoneNumber.IsEmpty()) {
        *res = String("");
        return NOERROR;
    }

    StringBuilder sb;
    Int32 len = phoneNumber.GetLength();
    for (Int32 i = 0; i < len; i++) {
        Char32 c = phoneNumber.GetChar(i);
        // Character.digit() supports ASCII and Unicode digits (fullwidth, Arabic-Indic, etc.)
        Int32 digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            sb.Append(digit);
        }
        else if (i == 0 && c == '+') {
            sb.AppendChar(c);
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            String str;
            PhoneNumberUtils::ConvertKeypadLettersToDigits(phoneNumber, &str);
            return NormalizeNumber(str, res);
        }
    }
    sb.ToString(res);
    return NOERROR;
}

ECode PhoneNumberUtils::ReplaceUnicodeDigits(
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    StringBuilder normalizedDigits;
    AutoPtr<ArrayOf<Char32> > array = number.GetChars();
    for (Int32 i = 0; i < array->GetLength(); i++) {
        Char32 c = (*array)[i];
        Int32 digit = Character::ToDigit(c, 10);
        if (digit != -1) {
            normalizedDigits.Append(digit);
        }
        else {
            normalizedDigits.AppendChar(c);
        }
    }
    normalizedDigits.ToString(res);
    return NOERROR;
}

ECode PhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsEmergencyNumber(id, number, res);
}

ECode PhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    // Return true only if the specified number *exactly* matches
    // one of the emergency numbers listed by the RIL / SIM.
    return IsEmergencyNumberInternal(subId, number, TRUE /* useExactMatch */, res);
}

ECode PhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsPotentialEmergencyNumber(id, number, res);
}

ECode PhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    // Check against the emergency numbers listed by the RIL / SIM,
    // and *don't* require an exact match.
    return IsEmergencyNumberInternal(subId, number, FALSE /* useExactMatch */, res);
}

ECode PhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsEmergencyNumber(id, number, defaultCountryIso, res);
}

ECode PhoneNumberUtils::IsEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return IsEmergencyNumberInternal(subId, number,
            defaultCountryIso, TRUE /* useExactMatch */, res);
}

ECode PhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsPotentialEmergencyNumber(id, number, defaultCountryIso, res);
}

ECode PhoneNumberUtils::IsPotentialEmergencyNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [out] */ Boolean* res)
{
    return IsEmergencyNumberInternal(subId, number,
            defaultCountryIso, FALSE /* useExactMatch */, res);
}

ECode PhoneNumberUtils::IsLocalEmergencyNumber(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsLocalEmergencyNumber(context, id, number, res);
}

ECode PhoneNumberUtils::IsLocalEmergencyNumber(
    /* [in] */ IContext* context,
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return IsLocalEmergencyNumberInternal(subId, number,
            context, TRUE /* useExactMatch */, res);
}

ECode PhoneNumberUtils::IsPotentialLocalEmergencyNumber(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsPotentialLocalEmergencyNumber(context, id, number, res);
}

ECode PhoneNumberUtils::IsPotentialLocalEmergencyNumber(
    /* [in] */ IContext* context,
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    return IsLocalEmergencyNumberInternal(subId, number,
            context, FALSE /* useExactMatch */, res);
}

ECode PhoneNumberUtils::IsVoiceMailNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* res)
{
    Int64 id;
    SubscriptionManager::GetDefaultSubId(&id);
    return IsVoiceMailNumber(id, number, res);
}

ECode PhoneNumberUtils::IsVoiceMailNumber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& _number,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    String vmNumber;

    // try {
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    tm->GetVoiceMailNumber(subId, &vmNumber);
    // } catch (SecurityException ex) {
    //     return false;
    // }

    // Strip the separators from the number before comparing it
    // to the list.
    String number;
    ExtractNetworkPortionAlt(_number, &number);

    // compare tolerates null so we need to make sure that we
    // don't return true when both are null.
    Boolean b;
    *res = (!number.IsEmpty()) && (Compare(number, vmNumber, &b), b);
    return NOERROR;
}

ECode PhoneNumberUtils::ConvertKeypadLettersToDigits(
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
        Int32 v = 0;
        KEYPAD_MAP->Get(c, c, &v);
        (*out)[i] = (Char32) v;
    }

    *res = String(*out);
    return NOERROR;
}

ECode PhoneNumberUtils::CdmaCheckAndProcessPlusCode(
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

ECode PhoneNumberUtils::CdmaCheckAndProcessPlusCodeForSms(
    /* [in] */ const String& dialStr,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    if (!dialStr.IsEmpty()) {
        Char32 c = dialStr.GetChar(0);
        Boolean b1, b2;
        IsReallyDialable(c, &b1);
        IsNonSeparator(dialStr, &b2);
        if (b1 && b2) {
            String defaultIso;
            SystemProperties::Get(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, String(""), &defaultIso);
            if (!defaultIso.IsEmpty()) {
                Int32 format;
                GetFormatTypeFromCountryCode(defaultIso, &format);
                return CdmaCheckAndProcessPlusCodeByNumberFormat(dialStr, format, format, res);
            }
        }
    }

    *res = dialStr;
    return NOERROR;
}

ECode PhoneNumberUtils::CdmaCheckAndProcessPlusCodeByNumberFormat(
    /* [in] */ const String& dialStr,
    /* [in] */ Int32 currFormat,
    /* [in] */ Int32 defaultFormat,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    String retStr = dialStr;

    Boolean useNanp = (currFormat == defaultFormat) &&
            (currFormat == IPhoneNumberUtils::FORMAT_NANP);

    // Checks if the plus sign character is in the passed-in dial string
    if (!dialStr.IsNull() && dialStr.LastIndexOf(PLUS_SIGN_STRING) != -1) {
        // Handle case where default and current telephone numbering plans are NANP.
        String postDialStr;
        String tempDialStr = dialStr;

        // Sets the retStr to null since the conversion will be performed below.
        retStr = String(NULL);
        if (DBG) Logger::I(TAG, "checkAndProcessPlusCode,dialStr=%s", dialStr.string());
        // This routine is to process the plus sign in the dial string by loop through
        // the network portion, post dial portion 1, post dial portion 2... etc. if
        // applied
        do {
            String networkDialStr, tempStr;
            // Format the string based on the rules for the country the number is from,
            // and the current country the phone is camped
            if (useNanp) {
                ExtractNetworkPortion(tempDialStr, &networkDialStr);
            }
            else {
                ExtractNetworkPortionAlt(tempDialStr, &networkDialStr);

            }

            ProcessPlusCode(networkDialStr, useNanp, &networkDialStr);

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
                Logger::E(TAG, "checkAndProcessPlusCode: null newDialStr %s", networkDialStr.string());
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
                    Logger::E(TAG, "wrong postDialStr=%s", postDialStr.string());
                }
            }
            if (DBG) Logger::I(TAG, "checkAndProcessPlusCode,postDialStr=%s", postDialStr.string());
        } while (!postDialStr.IsEmpty() && !tempDialStr.IsEmpty());
    }
    *res = retStr;
    return NOERROR;
}

ECode PhoneNumberUtils::IsNanp(
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
    }
    else {
        Logger::E(TAG, "isNanp: NULL dialStr passed in %s", dialStr.string());
    }
    *res = retVal;
    return NOERROR;
}

ECode PhoneNumberUtils::IsUriNumber(
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

ECode PhoneNumberUtils::GetUsernameFromUriNumber(
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
        Logger::W(TAG,
              "getUsernameFromUriNumber: no delimiter found in SIP addr '%s'", number.string());
        delimiterIndex = number.GetLength();
    }
    *res = number.Substring(0, delimiterIndex);

    return NOERROR;
}

ECode PhoneNumberUtils::IsPause(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c == 'p'||c == 'P');
    return NOERROR;
}

ECode PhoneNumberUtils::IsToneWait(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = (c == 'w'||c == 'W');
    return NOERROR;
}

ECode PhoneNumberUtils::IsSeparator(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean isdisable;
    IsDialable(ch, &isdisable);
    *res = (!isdisable && !(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')));
    return NOERROR;
}

ECode PhoneNumberUtils::MinPositive(
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

ECode PhoneNumberUtils::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
    return NOERROR;
}

ECode PhoneNumberUtils::IndexOfLastNetworkChar(
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

ECode PhoneNumberUtils::InternalGetStrippedReversed(
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

ECode PhoneNumberUtils::InternalCalledPartyBCDFragmentToString(
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

ECode PhoneNumberUtils::BcdToChar(
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

ECode PhoneNumberUtils::CharToBCD(
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

ECode PhoneNumberUtils::IsDialable(
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

ECode PhoneNumberUtils::IsNonSeparator(
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

ECode PhoneNumberUtils::NumberToCalledPartyBCDHelper(
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

ECode PhoneNumberUtils::RemoveDashes(
    /* [in] */ IEditable* _text,
    /* [out] */ IEditable** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IEditable> text = _text;
    Int32 p = 0;
    Int32 len;
    ICharSequence::Probe(text)->GetLength(&len);
    while (p < len) {
        Char32 c;
        ICharSequence::Probe(text)->GetCharAt(p, &c);
        if (c == '-') {
            text->Delete(p, p + 1);
        }
        else {
            p++;
        }
    }
    *result = text;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneNumberUtils::IsEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsEmergencyNumberInternal(id, number, useExactMatch, res);
}

ECode PhoneNumberUtils::IsEmergencyNumberInternal(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    return IsEmergencyNumberInternal(subId, number, String(NULL), useExactMatch, res);
}

ECode PhoneNumberUtils::IsEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ const String& defaultCountryIso,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsEmergencyNumberInternal(id, number, defaultCountryIso, useExactMatch, res);
}

ECode PhoneNumberUtils::IsEmergencyNumberInternal(
    /* [in] */ Int64 subId,
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
    Int32 slotId;
    SubscriptionManager::GetSlotId(subId, &slotId);
    // retrieve the list of emergency numbers
    // check read-write ecclist property first
    StringBuilder sb("ril.ecclist");
    if ((slotId > 0)) {
        sb += slotId;
    }
    String ecclist =  sb.ToString();
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Get(ecclist, &numbers);

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

    Logger::D(TAG, "System property doesn't provide any emergency numbers. Use embedded logic for determining ones.");

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

ECode PhoneNumberUtils::IsLocalEmergencyNumberInternal(
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return IsLocalEmergencyNumberInternal(id, number, context,
            useExactMatch, res);
}

ECode PhoneNumberUtils::IsLocalEmergencyNumberInternal(
    /* [in] */ Int64 subId,
    /* [in] */ const String& number,
    /* [in] */ IContext* context,
    /* [in] */ Boolean useExactMatch,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    String countryIso;
    AutoPtr<IInterface> obj;
    Logger::W(TAG, "TODO COUNTRY_DETECTOR service is not implemented!!");
    //TODO leliang COUNTRY_DETECTOR service is not implemented
    //TODO context->GetSystemService(IContext::COUNTRY_DETECTOR, (IInterface**)&obj);
    AutoPtr<ICountryDetector> detector = ICountryDetector::Probe(obj);

    if (detector != NULL) {
        AutoPtr<ICountry> country;
        detector->DetectCountry((ICountry**)&country);
        if (country != NULL) {
            country->GetCountryIso(&countryIso);
        }
    }
    else {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> locale;
        config->GetLocale((ILocale**)&locale);
        locale->GetCountry(&countryIso);
        Logger::W(TAG, String("No CountryDetector; falling back to countryIso based on locale: ")
                + countryIso);
    }
    return IsEmergencyNumberInternal(subId, number, countryIso, useExactMatch, res);
}

ECode PhoneNumberUtils::GetCurrentIdp(
    /* [in] */ Boolean useNanp,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // in case, there is no IDD is found, we shouldn't convert it.
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    return sp->Get(ITelephonyProperties::PROPERTY_OPERATOR_IDP_STRING, useNanp ? NANP_IDP_STRING : PLUS_SIGN_STRING, res);
}

ECode PhoneNumberUtils::IsTwoToNine(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    if (c >= '2' && c <= '9') {
        *res = TRUE;
    }
    else {
        *res = FALSE;
    }
    return NOERROR;
}

ECode PhoneNumberUtils::GetFormatTypeFromCountryCode(
    /* [in] */ const String& country,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    // Check for the NANP countries
    Int32 length = NANP_COUNTRIES->GetLength();
    for (Int32 i = 0; i < length; i++) {
        if ((*NANP_COUNTRIES)[i].CompareIgnoreCase(country) == 0) {
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

ECode PhoneNumberUtils::IsOneNanp(
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
        Logger::E(TAG, "isOneNanp: NULL dialStr passed in %s", dialStr.string());
    }
    *res = retVal;
    return NOERROR;
}

ECode PhoneNumberUtils::ProcessPlusCode(
    /* [in] */ const String& networkDialStr,
    /* [in] */ Boolean useNanp,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    String retStr = networkDialStr;

    if (DBG) Logger::I(TAG, "processPlusCode,networkDialStr = %s, for NANP = %d",
            networkDialStr.string(), useNanp);
    // If there is a plus sign at the beginning of the dial string,
    // Convert the plus sign to the default IDP since it's an international number
    if (!networkDialStr.IsNull() &&
        networkDialStr.GetChar(0) == PLUS_SIGN_CHAR &&
        networkDialStr.GetLength() > 1) {
        String newStr = networkDialStr.Substring(1);
        Boolean tempRes;
        // TODO: for nonNanp, should the '+' be removed if following number is country code
        if (useNanp && (IsOneNanp(newStr, &tempRes), tempRes)) {
            // Remove the leading plus sign
            retStr = newStr;
         }
         else {
            String idpStr;
            GetCurrentIdp(useNanp, &idpStr);
            // Replaces the plus sign with the default IDP
            StringUtils::ReplaceFirst(networkDialStr, String("[+]"), idpStr, &retStr);
        }
    }
    if (DBG) Logger::I(TAG, "processPlusCode,retStr=%s", retStr.string());
    *res = retStr;
    return NOERROR;
}

ECode PhoneNumberUtils::FindDialableIndexFromPostDialStr(
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

ECode PhoneNumberUtils::AppendPwCharBackToOrigDialStr(
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

ECode PhoneNumberUtils::MatchIntlPrefix(
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

ECode PhoneNumberUtils::MatchIntlPrefixAndCC(
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

ECode PhoneNumberUtils::MatchTrunkPrefix(
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

ECode PhoneNumberUtils::IsCountryCallingCode(
    /* [in] */ Int32 countryCallingCodeCandidate,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (countryCallingCodeCandidate > 0 && countryCallingCodeCandidate < CCC_LENGTH &&
            COUNTRY_CALLING_CALL[countryCallingCodeCandidate]);

    return NOERROR;
}

ECode PhoneNumberUtils::TryGetISODigit(
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

ECode PhoneNumberUtils::TryGetCountryCallingCodeAndNewIndex(
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

ECode PhoneNumberUtils::TryGetTrunkPrefixOmittedIndex(
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

ECode PhoneNumberUtils::CheckPrefixIsIgnorable(
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

ECode PhoneNumberUtils::GetDefaultVoiceSubId(
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res)
    return SubscriptionManager::GetDefaultSubId(res);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

