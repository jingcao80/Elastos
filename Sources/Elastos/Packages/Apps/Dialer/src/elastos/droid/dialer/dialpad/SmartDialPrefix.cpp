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

#include "elastos/droid/dialer/dialpad/SmartDialPrefix.h"
#include "elastos/droid/dialer/dialpad/CLatinSmartDialMap.h"
#include "elastos/droid/dialer/dialpad/SmartDialNameMatcher.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

//=================================================================
// SmartDialPrefix::PhoneNumberTokens
//=================================================================
CAR_INTERFACE_IMPL(SmartDialPrefix::PhoneNumberTokens, Object, ISmartDialPrefixPhoneNumberTokens);

SmartDialPrefix::PhoneNumberTokens::PhoneNumberTokens(
    /* [in] */ const String& countryCode,
    /* [in] */ Int32 countryCodeOffset,
    /* [in] */ Int32 nanpCodeOffset)
    : mCountryCodeOffset(countryCodeOffset)
    , mNanpCodeOffset(nanpCodeOffset)
{
    mCountryCode = countryCode;
}

//=================================================================
// SmartDialPrefix
//=================================================================
AutoPtr<ISmartDialMap> CreateMap()
{
    AutoPtr<ISmartDialMap> map;
    CLatinSmartDialMap::New((ISmartDialMap**)&map);
    return map;
}

const Int32 SmartDialPrefix::LAST_TOKENS_FOR_INITIALS = 2;
const Int32 SmartDialPrefix::FIRST_TOKENS_FOR_INITIALS = 2;

const String SmartDialPrefix::PREF_USER_SIM_COUNTRY_CODE("DialtactsActivity_user_sim_country_code");
const String SmartDialPrefix::PREF_USER_SIM_COUNTRY_CODE_DEFAULT(NULL);
String SmartDialPrefix::sUserSimCountryCode(PREF_USER_SIM_COUNTRY_CODE_DEFAULT);

Boolean SmartDialPrefix::sUserInNanpRegion = FALSE;

AutoPtr<ISet> SmartDialPrefix::sNanpCountries;

AutoPtr<ISet> SmartDialPrefix::sCountryCodes;

const AutoPtr<ISmartDialMap> SmartDialPrefix::mMap = CreateMap();

Boolean SmartDialPrefix::sNanpInitialized = FALSE;

void SmartDialPrefix::InitializeNanpSettings(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> manager;
    context->GetSystemService(
            IContext::TELEPHONY_SERVICE, (IInterface**)&manager);
    if (manager != NULL) {
         ITelephonyManager::Probe(manager)->GetSimCountryIso(&sUserSimCountryCode);
    }

    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> prefs;
    helper->GetDefaultSharedPreferences(context, (ISharedPreferences**)&prefs);

    if (!sUserSimCountryCode.IsNull()) {
        /** Updates shared preferences with the latest country obtained from getSimCountryIso.*/
        AutoPtr<ISharedPreferencesEditor> editor;
        prefs->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutString(PREF_USER_SIM_COUNTRY_CODE, sUserSimCountryCode);
        editor->Apply();
    }
    else {
        /** Uses previously stored country code if loading fails. */
        prefs->GetString(PREF_USER_SIM_COUNTRY_CODE,
                PREF_USER_SIM_COUNTRY_CODE_DEFAULT, &sUserSimCountryCode);
    }
    /** Queries the NANP country list to find out whether user is in a NANP region.*/
    sUserInNanpRegion = IsCountryNanp(sUserSimCountryCode);
    sNanpInitialized = TRUE;
}

void SmartDialPrefix::SetUserInNanpRegion(
    /* [in] */ Boolean userInNanpRegion)
{
    sUserInNanpRegion = userInNanpRegion;
}

AutoPtr<IArrayList> SmartDialPrefix::ParseToIndexTokens(
    /* [in] */ const String& contactName)
{
    const Int32 length = contactName.GetLength();
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    Char32 c;
    AutoPtr<StringBuilder> currentIndexToken = new StringBuilder();
    /**
     * Iterates through the whole name string. If the current character is a valid character,
     * append it to the current token. If the current character is not a valid character, for
     * example space " ", mark the current token as complete and add it to the list of tokens.
     */
    Boolean isValidDialpadCharacter;
    for (Int32 i = 0; i < length; i++) {
        mMap->NormalizeCharacter(contactName.GetChar(i), &c);
        if (mMap->IsValidDialpadCharacter(c, &isValidDialpadCharacter), isValidDialpadCharacter) {
            /** Converts a character into the number on dialpad that represents the character.*/
            Byte index;
            mMap->GetDialpadIndex(c, &index);
            currentIndexToken->Append(index);
        }
        else {
            Int32 tokenLength;
            currentIndexToken->GetLength(&tokenLength);
            if (tokenLength != 0) {
                result->Add(CoreUtils::Convert(currentIndexToken->ToString()));
            }
            currentIndexToken->Delete(0, tokenLength);
        }
    }

    /** Adds the last token in case it has not been added.*/
    Int32 tokenLength;
    currentIndexToken->GetLength(&tokenLength);
    if (tokenLength != 0) {
        result->Add(CoreUtils::Convert(currentIndexToken->ToString()));
    }
    return result;
}

AutoPtr<IArrayList> SmartDialPrefix::GenerateNamePrefixes(
    /* [in] */ const String& index)
{
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);

    /** Parses the name into a list of tokens.*/
    AutoPtr<IArrayList> indexTokens = ParseToIndexTokens(index);

    Int32 size;
    if (indexTokens->GetSize(&size), size > 0) {
        /** Adds the full token combinations to the list. For example, a contact with name
         * "Albert Ben Ed Foster" can be looked up by any prefix of the following strings
         * "Foster" "EdFoster" "BenEdFoster" and "AlbertBenEdFoster". This covers all cases of
         * look up that contains only one token, and that spans multiple continuous tokens.
         */
        AutoPtr<StringBuilder> fullNameToken = new StringBuilder();
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> item;
            indexTokens->Get(i, (IInterface**)&item);
            fullNameToken->Insert(0, CoreUtils::Unbox(ICharSequence::Probe(item)));
            result->Add(CoreUtils::Convert(fullNameToken->ToString()));
        }

        /** Adds initial combinations to the list, with the number of initials restricted by
         * {@link #LAST_TOKENS_FOR_INITIALS} and {@link #FIRST_TOKENS_FOR_INITIALS}.
         * For example, a contact with name "Albert Ben Ed Foster" can be looked up by any
         * prefix of the following strings "EFoster" "BFoster" "BEFoster" "AFoster" "ABFoster"
         * "AEFoster" and "ABEFoster". This covers all cases of initial lookup.
         */
        AutoPtr<IArrayList> fullNames;
        CArrayList::New((IArrayList**)&fullNames);
        AutoPtr<IInterface> token;
        indexTokens->Get(size - 1, (IInterface**)&token);
        fullNames->Add(token);
        Int32 recursiveNameStart;
        result->GetSize(&recursiveNameStart);
        Int32 recursiveNameEnd = recursiveNameStart;
        String initial("");

        for (Int32 i = size - 2; i >= 0; i--) {
            if ((i >= size - LAST_TOKENS_FOR_INITIALS) ||
                    (i < FIRST_TOKENS_FOR_INITIALS)) {
                AutoPtr<IInterface> item;
                indexTokens->Get(i, (IInterface**)&item);
                initial = CoreUtils::Unbox(ICharSequence::Probe(item)).Substring(0, 1);

                /** Recursively adds initial combinations to the list.*/
                Int32 fullNamesSize;
                fullNames->GetSize(&fullNamesSize);
                for (Int32 j = 0; j < fullNamesSize; ++j) {
                    AutoPtr<IInterface> nameItem;
                    fullNames->Get(j, (IInterface**)&nameItem);
                    String str = initial + CoreUtils::Unbox(ICharSequence::Probe(nameItem));
                    result->Add(CoreUtils::Convert(str));
                }
                for (Int32 j = recursiveNameStart; j < recursiveNameEnd; ++j) {
                    AutoPtr<IInterface> resItem;
                    result->Get(j, (IInterface**)&resItem);
                    String str = initial + CoreUtils::Unbox(ICharSequence::Probe(resItem));
                    result->Add(CoreUtils::Convert(str));
                }
                result->GetSize(&recursiveNameEnd);
                AutoPtr<IInterface> nameItem;
                fullNames->Get(fullNamesSize - 1, (IInterface**)&nameItem);
                const String currentFullName = CoreUtils::Unbox(ICharSequence::Probe(nameItem));

                item = NULL;
                indexTokens->Get(i, (IInterface**)&item);
                String str = currentFullName + CoreUtils::Unbox(ICharSequence::Probe(item));
                fullNames->Add(CoreUtils::Convert(str));
            }
        }
    }

    return result;
}

AutoPtr<IArrayList> SmartDialPrefix::ParseToNumberTokens(
    /* [in] */ const String& number)
{
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    if (!TextUtils::IsEmpty(number)) {
        /** Adds the full number to the list.*/
        result->Add(CoreUtils::Convert(SmartDialNameMatcher::NormalizeNumber(number, mMap)));

        AutoPtr<ISmartDialPrefixPhoneNumberTokens> phoneNumberTokens = ParsePhoneNumber(number);
        if (phoneNumberTokens == NULL) {
            return result;
        }

        if (((PhoneNumberTokens*)phoneNumberTokens.Get())->mCountryCodeOffset != 0) {
            result->Add(CoreUtils::Convert(SmartDialNameMatcher::NormalizeNumber(number,
                    ((PhoneNumberTokens*)phoneNumberTokens.Get())->mCountryCodeOffset, mMap)));
        }

        if (((PhoneNumberTokens*)phoneNumberTokens.Get())->mNanpCodeOffset != 0) {
            result->Add(CoreUtils::Convert(SmartDialNameMatcher::NormalizeNumber(number,
                    ((PhoneNumberTokens*)phoneNumberTokens.Get())->mNanpCodeOffset, mMap)));
        }
    }
    return result;
}

AutoPtr<ISmartDialPrefixPhoneNumberTokens> SmartDialPrefix::ParsePhoneNumber(
    /* [in] */ const String& number)
{
    String countryCode("");
    Int32 countryCodeOffset = 0;
    Int32 nanpNumberOffset = 0;

    if (!TextUtils::IsEmpty(number)) {
        String normalizedNumber = SmartDialNameMatcher::NormalizeNumber(number, mMap);
        if (number.GetChar(0) == '+') {
            /** If the number starts with '+', tries to find valid country code. */
            for (Int32 i = 1; i <= 1 + 3; i++) {
                if (number.GetLength() <= i) {
                    break;
                }
                countryCode = number.Substring(1, i);
                if (IsValidCountryCode(countryCode)) {
                    countryCodeOffset = i;
                    break;
                }
            }
        }
        else {
            /** If the number does not start with '+', finds out whether it is in NANP
             * format and has '1' preceding the number.
             */
            if ((normalizedNumber.GetLength() == 11) && (normalizedNumber.GetChar(0) == '1') &&
                    (sUserInNanpRegion)) {
                countryCode = "1";
                countryCodeOffset = number.IndexOf(normalizedNumber.GetChar(1));
                if (countryCodeOffset == -1) {
                    countryCodeOffset = 0;
                }
            }
        }

        /** If user is in NANP region, finds out whether a number is in NANP format.*/
        if (sUserInNanpRegion)  {
            String areaCode("");
            if (countryCode.Equals("") && normalizedNumber.GetLength() == 10) {
                /** if the number has no country code but fits the NANP format, extracts the
                 * NANP area code, and finds out offset of the local number.
                 */
                areaCode = normalizedNumber.Substring(0, 3);
            }
            else if (countryCode.Equals("1") && normalizedNumber.GetLength() == 11) {
                /** If the number has country code '1', finds out area code and offset of the
                 * local number.
                 */
                areaCode = normalizedNumber.Substring(1, 4);
            }
            if (!areaCode.Equals("")) {
                const Int32 areaCodeIndex = number.IndexOf(areaCode);
                if (areaCodeIndex != -1) {
                    nanpNumberOffset = number.IndexOf(areaCode) + 3;
                }
            }
        }
    }
    return (ISmartDialPrefixPhoneNumberTokens*)new PhoneNumberTokens(
            countryCode, countryCodeOffset, nanpNumberOffset);
}

Boolean SmartDialPrefix::IsValidCountryCode(
    /* [in] */ const String& countryCode)
{
    if (sCountryCodes == NULL) {
        sCountryCodes = InitCountryCodes();
    }
    Boolean result;
    sCountryCodes->Contains(CoreUtils::Convert(countryCode), &result);
    return result;
}

AutoPtr<ISet> SmartDialPrefix::InitCountryCodes()
{
    AutoPtr<IHashSet> result;
    CHashSet::New((IHashSet**)&result);
    result->Add(CoreUtils::Convert(String("1")));
    result->Add(CoreUtils::Convert(String("7")));
    result->Add(CoreUtils::Convert(String("20")));
    result->Add(CoreUtils::Convert(String("27")));
    result->Add(CoreUtils::Convert(String("30")));
    result->Add(CoreUtils::Convert(String("31")));
    result->Add(CoreUtils::Convert(String("32")));
    result->Add(CoreUtils::Convert(String("33")));
    result->Add(CoreUtils::Convert(String("34")));
    result->Add(CoreUtils::Convert(String("36")));
    result->Add(CoreUtils::Convert(String("39")));
    result->Add(CoreUtils::Convert(String("40")));
    result->Add(CoreUtils::Convert(String("41")));
    result->Add(CoreUtils::Convert(String("43")));
    result->Add(CoreUtils::Convert(String("44")));
    result->Add(CoreUtils::Convert(String("45")));
    result->Add(CoreUtils::Convert(String("46")));
    result->Add(CoreUtils::Convert(String("47")));
    result->Add(CoreUtils::Convert(String("48")));
    result->Add(CoreUtils::Convert(String("49")));
    result->Add(CoreUtils::Convert(String("51")));
    result->Add(CoreUtils::Convert(String("52")));
    result->Add(CoreUtils::Convert(String("53")));
    result->Add(CoreUtils::Convert(String("54")));
    result->Add(CoreUtils::Convert(String("55")));
    result->Add(CoreUtils::Convert(String("56")));
    result->Add(CoreUtils::Convert(String("57")));
    result->Add(CoreUtils::Convert(String("58")));
    result->Add(CoreUtils::Convert(String("60")));
    result->Add(CoreUtils::Convert(String("61")));
    result->Add(CoreUtils::Convert(String("62")));
    result->Add(CoreUtils::Convert(String("63")));
    result->Add(CoreUtils::Convert(String("64")));
    result->Add(CoreUtils::Convert(String("65")));
    result->Add(CoreUtils::Convert(String("66")));
    result->Add(CoreUtils::Convert(String("81")));
    result->Add(CoreUtils::Convert(String("82")));
    result->Add(CoreUtils::Convert(String("84")));
    result->Add(CoreUtils::Convert(String("86")));
    result->Add(CoreUtils::Convert(String("90")));
    result->Add(CoreUtils::Convert(String("91")));
    result->Add(CoreUtils::Convert(String("92")));
    result->Add(CoreUtils::Convert(String("93")));
    result->Add(CoreUtils::Convert(String("94")));
    result->Add(CoreUtils::Convert(String("95")));
    result->Add(CoreUtils::Convert(String("98")));
    result->Add(CoreUtils::Convert(String("211")));
    result->Add(CoreUtils::Convert(String("212")));
    result->Add(CoreUtils::Convert(String("213")));
    result->Add(CoreUtils::Convert(String("216")));
    result->Add(CoreUtils::Convert(String("218")));
    result->Add(CoreUtils::Convert(String("220")));
    result->Add(CoreUtils::Convert(String("221")));
    result->Add(CoreUtils::Convert(String("222")));
    result->Add(CoreUtils::Convert(String("223")));
    result->Add(CoreUtils::Convert(String("224")));
    result->Add(CoreUtils::Convert(String("225")));
    result->Add(CoreUtils::Convert(String("226")));
    result->Add(CoreUtils::Convert(String("227")));
    result->Add(CoreUtils::Convert(String("228")));
    result->Add(CoreUtils::Convert(String("229")));
    result->Add(CoreUtils::Convert(String("230")));
    result->Add(CoreUtils::Convert(String("231")));
    result->Add(CoreUtils::Convert(String("232")));
    result->Add(CoreUtils::Convert(String("233")));
    result->Add(CoreUtils::Convert(String("234")));
    result->Add(CoreUtils::Convert(String("235")));
    result->Add(CoreUtils::Convert(String("236")));
    result->Add(CoreUtils::Convert(String("237")));
    result->Add(CoreUtils::Convert(String("238")));
    result->Add(CoreUtils::Convert(String("239")));
    result->Add(CoreUtils::Convert(String("240")));
    result->Add(CoreUtils::Convert(String("241")));
    result->Add(CoreUtils::Convert(String("242")));
    result->Add(CoreUtils::Convert(String("243")));
    result->Add(CoreUtils::Convert(String("244")));
    result->Add(CoreUtils::Convert(String("245")));
    result->Add(CoreUtils::Convert(String("246")));
    result->Add(CoreUtils::Convert(String("247")));
    result->Add(CoreUtils::Convert(String("248")));
    result->Add(CoreUtils::Convert(String("249")));
    result->Add(CoreUtils::Convert(String("250")));
    result->Add(CoreUtils::Convert(String("251")));
    result->Add(CoreUtils::Convert(String("252")));
    result->Add(CoreUtils::Convert(String("253")));
    result->Add(CoreUtils::Convert(String("254")));
    result->Add(CoreUtils::Convert(String("255")));
    result->Add(CoreUtils::Convert(String("256")));
    result->Add(CoreUtils::Convert(String("257")));
    result->Add(CoreUtils::Convert(String("258")));
    result->Add(CoreUtils::Convert(String("260")));
    result->Add(CoreUtils::Convert(String("261")));
    result->Add(CoreUtils::Convert(String("262")));
    result->Add(CoreUtils::Convert(String("263")));
    result->Add(CoreUtils::Convert(String("264")));
    result->Add(CoreUtils::Convert(String("265")));
    result->Add(CoreUtils::Convert(String("266")));
    result->Add(CoreUtils::Convert(String("267")));
    result->Add(CoreUtils::Convert(String("268")));
    result->Add(CoreUtils::Convert(String("269")));
    result->Add(CoreUtils::Convert(String("290")));
    result->Add(CoreUtils::Convert(String("291")));
    result->Add(CoreUtils::Convert(String("297")));
    result->Add(CoreUtils::Convert(String("298")));
    result->Add(CoreUtils::Convert(String("299")));
    result->Add(CoreUtils::Convert(String("350")));
    result->Add(CoreUtils::Convert(String("351")));
    result->Add(CoreUtils::Convert(String("352")));
    result->Add(CoreUtils::Convert(String("353")));
    result->Add(CoreUtils::Convert(String("354")));
    result->Add(CoreUtils::Convert(String("355")));
    result->Add(CoreUtils::Convert(String("356")));
    result->Add(CoreUtils::Convert(String("357")));
    result->Add(CoreUtils::Convert(String("358")));
    result->Add(CoreUtils::Convert(String("359")));
    result->Add(CoreUtils::Convert(String("370")));
    result->Add(CoreUtils::Convert(String("371")));
    result->Add(CoreUtils::Convert(String("372")));
    result->Add(CoreUtils::Convert(String("373")));
    result->Add(CoreUtils::Convert(String("374")));
    result->Add(CoreUtils::Convert(String("375")));
    result->Add(CoreUtils::Convert(String("376")));
    result->Add(CoreUtils::Convert(String("377")));
    result->Add(CoreUtils::Convert(String("378")));
    result->Add(CoreUtils::Convert(String("379")));
    result->Add(CoreUtils::Convert(String("380")));
    result->Add(CoreUtils::Convert(String("381")));
    result->Add(CoreUtils::Convert(String("382")));
    result->Add(CoreUtils::Convert(String("385")));
    result->Add(CoreUtils::Convert(String("386")));
    result->Add(CoreUtils::Convert(String("387")));
    result->Add(CoreUtils::Convert(String("389")));
    result->Add(CoreUtils::Convert(String("420")));
    result->Add(CoreUtils::Convert(String("421")));
    result->Add(CoreUtils::Convert(String("423")));
    result->Add(CoreUtils::Convert(String("500")));
    result->Add(CoreUtils::Convert(String("501")));
    result->Add(CoreUtils::Convert(String("502")));
    result->Add(CoreUtils::Convert(String("503")));
    result->Add(CoreUtils::Convert(String("504")));
    result->Add(CoreUtils::Convert(String("505")));
    result->Add(CoreUtils::Convert(String("506")));
    result->Add(CoreUtils::Convert(String("507")));
    result->Add(CoreUtils::Convert(String("508")));
    result->Add(CoreUtils::Convert(String("509")));
    result->Add(CoreUtils::Convert(String("590")));
    result->Add(CoreUtils::Convert(String("591")));
    result->Add(CoreUtils::Convert(String("592")));
    result->Add(CoreUtils::Convert(String("593")));
    result->Add(CoreUtils::Convert(String("594")));
    result->Add(CoreUtils::Convert(String("595")));
    result->Add(CoreUtils::Convert(String("596")));
    result->Add(CoreUtils::Convert(String("597")));
    result->Add(CoreUtils::Convert(String("598")));
    result->Add(CoreUtils::Convert(String("599")));
    result->Add(CoreUtils::Convert(String("670")));
    result->Add(CoreUtils::Convert(String("672")));
    result->Add(CoreUtils::Convert(String("673")));
    result->Add(CoreUtils::Convert(String("674")));
    result->Add(CoreUtils::Convert(String("675")));
    result->Add(CoreUtils::Convert(String("676")));
    result->Add(CoreUtils::Convert(String("677")));
    result->Add(CoreUtils::Convert(String("678")));
    result->Add(CoreUtils::Convert(String("679")));
    result->Add(CoreUtils::Convert(String("680")));
    result->Add(CoreUtils::Convert(String("681")));
    result->Add(CoreUtils::Convert(String("682")));
    result->Add(CoreUtils::Convert(String("683")));
    result->Add(CoreUtils::Convert(String("685")));
    result->Add(CoreUtils::Convert(String("686")));
    result->Add(CoreUtils::Convert(String("687")));
    result->Add(CoreUtils::Convert(String("688")));
    result->Add(CoreUtils::Convert(String("689")));
    result->Add(CoreUtils::Convert(String("690")));
    result->Add(CoreUtils::Convert(String("691")));
    result->Add(CoreUtils::Convert(String("692")));
    result->Add(CoreUtils::Convert(String("800")));
    result->Add(CoreUtils::Convert(String("808")));
    result->Add(CoreUtils::Convert(String("850")));
    result->Add(CoreUtils::Convert(String("852")));
    result->Add(CoreUtils::Convert(String("853")));
    result->Add(CoreUtils::Convert(String("855")));
    result->Add(CoreUtils::Convert(String("856")));
    result->Add(CoreUtils::Convert(String("870")));
    result->Add(CoreUtils::Convert(String("878")));
    result->Add(CoreUtils::Convert(String("880")));
    result->Add(CoreUtils::Convert(String("881")));
    result->Add(CoreUtils::Convert(String("882")));
    result->Add(CoreUtils::Convert(String("883")));
    result->Add(CoreUtils::Convert(String("886")));
    result->Add(CoreUtils::Convert(String("888")));
    result->Add(CoreUtils::Convert(String("960")));
    result->Add(CoreUtils::Convert(String("961")));
    result->Add(CoreUtils::Convert(String("962")));
    result->Add(CoreUtils::Convert(String("963")));
    result->Add(CoreUtils::Convert(String("964")));
    result->Add(CoreUtils::Convert(String("965")));
    result->Add(CoreUtils::Convert(String("966")));
    result->Add(CoreUtils::Convert(String("967")));
    result->Add(CoreUtils::Convert(String("968")));
    result->Add(CoreUtils::Convert(String("970")));
    result->Add(CoreUtils::Convert(String("971")));
    result->Add(CoreUtils::Convert(String("972")));
    result->Add(CoreUtils::Convert(String("973")));
    result->Add(CoreUtils::Convert(String("974")));
    result->Add(CoreUtils::Convert(String("975")));
    result->Add(CoreUtils::Convert(String("976")));
    result->Add(CoreUtils::Convert(String("977")));
    result->Add(CoreUtils::Convert(String("979")));
    result->Add(CoreUtils::Convert(String("992")));
    result->Add(CoreUtils::Convert(String("993")));
    result->Add(CoreUtils::Convert(String("994")));
    result->Add(CoreUtils::Convert(String("995")));
    result->Add(CoreUtils::Convert(String("996")));
    result->Add(CoreUtils::Convert(String("998")));
    return ISet::Probe(result);
}

AutoPtr<ISmartDialMap> SmartDialPrefix::GetMap()
{
    return mMap;
}

Boolean SmartDialPrefix::IsCountryNanp(
    /* [in] */ const String& country)
{
    if (TextUtils::IsEmpty(country)) {
        return FALSE;
    }
    if (sNanpCountries == NULL) {
        sNanpCountries = InitNanpCountries();
    }
    Boolean result;
    sNanpCountries->Contains(CoreUtils::Convert(country.ToUpperCase()), &result);
    return result;
}

AutoPtr<ISet> SmartDialPrefix::InitNanpCountries()
{
    AutoPtr<IHashSet> result;
    CHashSet::New((IHashSet**)&result);
    result->Add(CoreUtils::Convert(String("US"))); // United States
    result->Add(CoreUtils::Convert(String("CA"))); // Canada
    result->Add(CoreUtils::Convert(String("AS"))); // American Samoa
    result->Add(CoreUtils::Convert(String("AI"))); // Anguilla
    result->Add(CoreUtils::Convert(String("AG"))); // Antigua and Barbuda
    result->Add(CoreUtils::Convert(String("BS"))); // Bahamas
    result->Add(CoreUtils::Convert(String("BB"))); // Barbados
    result->Add(CoreUtils::Convert(String("BM"))); // Bermuda
    result->Add(CoreUtils::Convert(String("VG"))); // British Virgin Islands
    result->Add(CoreUtils::Convert(String("KY"))); // Cayman Islands
    result->Add(CoreUtils::Convert(String("DM"))); // Dominica
    result->Add(CoreUtils::Convert(String("DO"))); // Dominican Republic
    result->Add(CoreUtils::Convert(String("GD"))); // Grenada
    result->Add(CoreUtils::Convert(String("GU"))); // Guam
    result->Add(CoreUtils::Convert(String("JM"))); // Jamaica
    result->Add(CoreUtils::Convert(String("PR"))); // Puerto Rico
    result->Add(CoreUtils::Convert(String("MS"))); // Montserrat
    result->Add(CoreUtils::Convert(String("MP"))); // Northern Mariana Islands
    result->Add(CoreUtils::Convert(String("KN"))); // Saint Kitts and Nevis
    result->Add(CoreUtils::Convert(String("LC"))); // Saint Lucia
    result->Add(CoreUtils::Convert(String("VC"))); // Saint Vincent and the Grenadines
    result->Add(CoreUtils::Convert(String("TT"))); // Trinidad and Tobago
    result->Add(CoreUtils::Convert(String("TC"))); // Turks and Caicos Islands
    result->Add(CoreUtils::Convert(String("VI"))); // U.S. Virgin Islands
    return ISet::Probe(result);
}

Boolean SmartDialPrefix::GetUserInNanpRegion()
{
    return sUserInNanpRegion;
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos
