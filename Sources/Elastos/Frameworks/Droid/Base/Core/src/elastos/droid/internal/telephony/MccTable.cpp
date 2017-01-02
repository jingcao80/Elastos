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

#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/ServiceStateTracker.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Core::CString;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Slogger;
using Libcore::ICU::CTimeZoneNames;
using Libcore::ICU::ITimeZoneNames;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String MccTable::TAG("MccTable");
AutoPtr<IArrayList> MccTable::sTable = InitTable(); //ArrayList<MccEntry>

CAR_INTERFACE_IMPL(MccTable::MccEntry, Object, IComparable)
MccTable::MccEntry::MccEntry(
    /* [in] */ Int32 mnc,
    /* [in] */ const String& iso,
    /* [in] */ Int32 smallestDigitsMCC)
{
    Init(mnc, iso, smallestDigitsMCC, String(NULL));
}

MccTable::MccEntry::MccEntry(
    /* [in] */ Int32 mnc,
    /* [in] */ const String iso,
    /* [in] */ Int32 smallestDigitsMCC,
    /* [in] */ const String language)
{
    Init(mnc, iso, smallestDigitsMCC, language);
}

ECode MccTable::MccEntry::CompareTo(
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMcc - ((MccEntry*)IComparable::Probe(o))->mMcc;
    return NOERROR;
}

void MccTable::MccEntry::Init(
    /* [in] */ Int32 mnc,
    /* [in] */ const String iso,
    /* [in] */ Int32 smallestDigitsMCC,
    /* [in] */ const String language)
{
    mMcc = mnc;
    mIso = iso;
    mSmallestDigitsMnc = smallestDigitsMCC;
    mLanguage = language;
}

AutoPtr<MccTable::MccEntry> MccTable::EntryForMcc(
    /* [in] */ Int32 mcc)
{
    Int32 index;

    AutoPtr<MccEntry> m;

    m = new MccEntry(mcc, String(NULL), 0);

    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->BinarySearch(IList::Probe(sTable), TO_IINTERFACE(m), &index);

    if (index < 0) {
        return NULL;
    }

    AutoPtr<IInterface> obj;
    sTable->Get(index, (IInterface**)&obj);
    return (MccEntry*)IComparable::Probe(obj);
}

/**
 * Returns a default time zone ID for the given MCC.
 * @param mcc Mobile Country Code
 * @return default TimeZone ID, or NULL if not specified
 */
String MccTable::DefaultTimeZoneForMcc(
    /* [in] */ Int32 mcc)
{
    AutoPtr<MccEntry> entry;

    entry = EntryForMcc(mcc);
    if (entry == NULL || entry->mIso == NULL) {
        return String(NULL);
    }

    AutoPtr<ILocale> locale;
    if (entry->mLanguage == NULL) {
        CLocale::New(entry->mIso, (ILocale**)&locale);
    }
    else {
        CLocale::New(entry->mLanguage, entry->mIso, (ILocale**)&locale);
    }

    AutoPtr<ITimeZoneNames> helper;
    CTimeZoneNames::AcquireSingleton((ITimeZoneNames**)&helper);
    AutoPtr<ArrayOf<String> > tz;
    helper->ForLocale(locale, (ArrayOf<String>**)&tz);
    if (tz->GetLength() == 0) return String(NULL);
    return (*tz)[0];

}

/**
 * Given a GSM Mobile Country Code, returns
 * an ISO two-character country code if available.
 * Returns "" if unavailable.
 */
String MccTable::CountryCodeForMcc(
    /* [in] */ Int32 mcc)
{
    AutoPtr<MccEntry> entry;

    entry = EntryForMcc(mcc);

    if (entry == NULL) {
        return String("");
    }

    return entry->mIso;
}

/**
 * Given a GSM Mobile Country Code, returns
 * an ISO 2-3 character language code if available.
 * Returns NULL if unavailable.
 */
String MccTable::DefaultLanguageForMcc(
    /* [in] */ Int32 mcc)
{
    AutoPtr<MccEntry> entry;

    entry = EntryForMcc(mcc);

    if (entry == NULL) {
        return String(NULL);
    }
    return entry->mLanguage;
}

/**
 * Given a GSM Mobile Country Code, returns
 * the smallest number of digits that M if available.
 * Returns 2 if unavailable.
 */
Int32 MccTable::SmallestDigitsMccForMnc(
    /* [in] */ Int32 mcc)
{
    AutoPtr<MccEntry> entry;

    entry = EntryForMcc(mcc);

    if (entry == NULL) {
        return 2;
    }
    return entry->mSmallestDigitsMnc;
}

/**
 * Updates MCC and MNC device configuration information for application retrieving
 * correct version of resources.  If MCC is 0, MCC and MNC will be ignored (not set).
 * @param context Context to act on.
 * @param mccmnc truncated imsi with just the MCC and MNC - MNC assumed to be from 4th to end
 * @param fromServiceState TRUE if coming from the radio service state, FALSE if from SIM
 */
void MccTable::UpdateMccMncConfiguration(
    /* [in] */ IContext* context,
    /* [in] */ const String& mccmnc,
    /* [in] */ Boolean fromServiceState)
{
    Slogger::D(TAG, "updateMccMncConfiguration mccmnc='%s' fromServiceState=%d"
            , mccmnc.string(), fromServiceState);
    if (!TextUtils::IsEmpty(mccmnc)) {
        Int32 mcc, mnc;

        AutoPtr<ITelephonyManager> tm;
        CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
        String defaultMccMnc;
        tm->GetSimOperator(&defaultMccMnc);
        Slogger::D(TAG, "updateMccMncConfiguration defaultMccMnc=%d", defaultMccMnc.string());
        //Update mccmnc only for default subscription in case of MultiSim.
//            if (!defaultMccMnc.equals(mccmnc)) {
//                Slogger::D(TAG, "Not a Default subscription, ignoring mccmnc config update.");
//                return;
//            }

        if (FAILED(StringUtils::Parse(mccmnc.Substring(0, 3), &mcc))) {
            Slogger::E(TAG, "Error parsing IMSI");
            return;
        }
        if (FAILED(StringUtils::Parse(mccmnc.Substring(3), &mnc))) {
            Slogger::E(TAG, "Error parsing IMSI");
            return;
        }

        Slogger::D(TAG, "updateMccMncConfiguration: mcc=%d, mnc=%d", mcc, mnc);

        AutoPtr<ILocale> locale;
        if (mcc != 0) {
            SetTimezoneFromMccIfNeeded(context, mcc);
            locale = GetLocaleFromMcc(context, mcc);
        }
        if (fromServiceState) {
            SetWifiCountryCodeFromMcc(context, mcc);
        }
        else {
            // from SIM
            // try {
            AutoPtr<IConfiguration> config;
            CConfiguration::New((IConfiguration**)&config);
            Boolean updateConfig = FALSE;
            if (mcc != 0) {
                config->SetMcc(mcc);
                config->SetMnc(mnc == 0 ? IConfiguration::MNC_ZERO : mnc);
                updateConfig = TRUE;
            }
            if (locale != NULL) {
                config->SetLocale(locale);
                updateConfig = TRUE;
            }
            if (updateConfig) {
                Slogger::D(TAG, "updateMccMncConfiguration updateConfig config=%s", TO_CSTR(config));
                if (FAILED(ActivityManagerNative::GetDefault()->UpdateConfiguration(config))) {
                    Slogger::E(TAG, "Can't update configuration");
                }
            }
            else {
                Slogger::D(TAG, "updateMccMncConfiguration nothing to update");
            }
            // } catch (RemoteException e) {
            //     Slogger::E(TAG, "Can't update configuration", e);
            // }
        }
    }
    else {
        if (fromServiceState) {
            // an empty mccmnc means no signal - tell wifi we don't know
            SetWifiCountryCodeFromMcc(context, 0);
        }
    }
}

/**
 * Return Locale for the language and country or NULL if no good match.
 *
 * @param context Context to act on.
 * @param language Two character language code desired
 * @param country Two character country code desired
 *
 * @return Locale or NULL if no appropriate value
 */
AutoPtr<ILocale> MccTable::GetLocaleForLanguageCountry(
    /* [in] */ IContext* context,
    /* [in] */ const String& language,
    /* [in] */ const String& _country)
{
    String country = _country;
    if (language == NULL) {
        Slogger::D(TAG, "getLocaleForLanguageCountry: skipping no language");
        return NULL; // no match possible
    }
    if (country == NULL) {
        country = String(""); // The Locale constructor throws if passed NULL.
    }

    // Note: persist.always.persist.locale actually means the opposite!
    Boolean alwaysPersist = FALSE;
    if (Build::IS_DEBUGGABLE) {
        SystemProperties::GetBoolean(String("persist.always.persist.locale"), FALSE, &alwaysPersist);
    }
    String persistSysLanguage;
    SystemProperties::Get(String("persist.sys.language"), String(""), &persistSysLanguage);
    String persistSysCountry;
    SystemProperties::Get(String("persist.sys.country"), String(""), &persistSysCountry);
    if (!(alwaysPersist || (persistSysLanguage.IsEmpty() && persistSysCountry.IsEmpty()))) {
        Slogger::D(TAG, "getLocaleForLanguageCountry: skipping already persisted");
        return NULL;
    }

    // Find the best match we actually have a localization for.
    // TODO: this should really follow the CLDR chain of parent locales!
    AutoPtr<ILocale> target;
    CLocale::New(language, country, (ILocale**)&target);
    // try {
    AutoPtr<IAssetManager> am;
    context->GetAssets((IAssetManager**)&am);
    AutoPtr<ArrayOf<String> > localeArray;
    am->GetLocales((ArrayOf<String>**)&localeArray);

    AutoPtr<IList> l;
    Arrays::AsList(localeArray, (IList**)&l);
    AutoPtr<IList> locales; ///*<String*/
    CArrayList::New(ICollection::Probe(l), (IList**)&locales);

    // Even in developer mode, you don't want the pseudolocales.
    AutoPtr<ICharSequence> cs;
    CString::New(String("ar-XB"), (ICharSequence**)&cs);
    locales->Remove(cs);

    cs = NULL;
    CString::New(String("en-XA"), (ICharSequence**)&cs);
    locales->Remove(cs);

    AutoPtr<ILocale> firstMatch;

    AutoPtr<IIterator> iter;
    locales->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    String locale;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        ICharSequence::Probe(value)->ToString(&locale);

        AutoPtr<ILocale> l;
        helper->ForLanguageTag(locale.Replace('_', '-'), (ILocale**)&l);

        // Only consider locales with both language and country.
        String lan, ct;
        if (l == NULL || (l->GetLanguage(&lan), lan.Equals("und")) ||
                lan.IsEmpty() || (l->GetCountry(&ct), ct).IsEmpty()) {
            continue;
        }

        l->GetLanguage(&lan);
        String lan2;
        if (lan.Equals((target->GetLanguage(&lan2), lan2))) {
            // If we got a perfect match, we're done.
            l->GetCountry(&ct);
            String ct2;
            target->GetCountry(&ct2);
            if (ct.Equals(ct2)) {
                String v;
                l->ToLanguageTag(&v);
                Slogger::D(TAG, "getLocaleForLanguageCountry: got perfect match: %s", v.string());
                return l;
            }
            // Otherwise somewhat arbitrarily take the first locale for the language,
            // unless we get a perfect match later. Note that these come back in no
            // particular order, so there's no reason to think the first match is
            // a particularly good match.
            if (firstMatch == NULL) {
                firstMatch = l;
            }
        }
    }

    // We didn't find the exact locale, so return whichever locale we saw first where
    // the language matched (if any).
    if (firstMatch != NULL) {
        String v;
        firstMatch->ToLanguageTag(&v);
        Slogger::D(TAG, "getLocaleForLanguageCountry: got a language-only match: %s", v.string());
        return firstMatch;
    }
    else {
        Slogger::D(TAG, "getLocaleForLanguageCountry: no locales for language %s", language.string());
    }
    // } catch (Exception e) {
    //     Slogger::D(TAG, "getLocaleForLanguageCountry: exception", e);
    // }

    return NULL;
}

/**
 * Utility code to set the system locale if it's not set already
 * @param context Context to act on.
 * @param language Two character language code desired
 * @param country Two character country code desired
 *
 *  {@hide}
 */
void MccTable::SetSystemLocale(
    /* [in] */ IContext* context,
    /* [in] */ const String& language,
    /* [in] */ const String& country)
{
    AutoPtr<ILocale> locale = GetLocaleForLanguageCountry(context, language, country);
    if (locale != NULL) {
        AutoPtr<IConfiguration> config;
        CConfiguration::New((IConfiguration**)&config);
        config->SetLocale(locale);
        config->SetUserSetLocale(FALSE);
        Slogger::D(TAG, "setSystemLocale: updateLocale config=%s", TO_CSTR(config));
        if (FAILED(ActivityManagerNative::GetDefault()->UpdateConfiguration(config))) {
            Slogger::D(TAG, "setSystemLocale exception");
        }
    }
    else {
        Slogger::D(TAG, "setSystemLocale: no locale");
    }
}

/**
 * If the timezone is not already set, set it based on the MCC of the SIM.
 * @param context Context to act on.
 * @param mcc Mobile Country Code of the SIM or SIM-like entity (build prop on CDMA)
 */
void MccTable::SetTimezoneFromMccIfNeeded(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mcc)
{
    String timezone;
    SystemProperties::Get(ServiceStateTracker::TIMEZONE_PROPERTY, &timezone);
    if (timezone == NULL || timezone.GetLength() == 0) {
        String zoneId = DefaultTimeZoneForMcc(mcc);
        if (zoneId != NULL && zoneId.GetLength() > 0) {
            // Set time zone based on MCC
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
            AutoPtr<IAlarmManager> alarm = IAlarmManager::Probe(obj);
            alarm->SetTimeZone(zoneId);
            Slogger::D(TAG, "timezone set to %s", zoneId.string());
        }
    }
}

/**
 * Get Locale based on the MCC of the SIM.
 * @param context Context to act on.
 * @param mcc Mobile Country Code of the SIM or SIM-like entity (build prop on CDMA)
 *
 * @return locale for the mcc or NULL if none
 */
AutoPtr<ILocale> MccTable::GetLocaleFromMcc(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mcc)
{
    String language = MccTable::DefaultLanguageForMcc(mcc);
    String country = MccTable::CountryCodeForMcc(mcc);

    Slogger::D(TAG, "getLocaleFromMcc to %s_%s mcc=%d", language.string(), country.string(), mcc);
    return GetLocaleForLanguageCountry(context, language, country);
}

/**
 * Set the country code for wifi.  This sets allowed wifi channels based on the
 * country of the carrier we see.  If we can't see any, reset to 0 so we don't
 * broadcast on forbidden channels.
 * @param context Context to act on.
 * @param mcc Mobile Country Code of the operator.  0 if not known
 */
void MccTable::SetWifiCountryCodeFromMcc(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mcc)
{
    String country = MccTable::CountryCodeForMcc(mcc);
    Slogger::D(TAG, "WIFI_COUNTRY_CODE set to %s", country.string());
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    AutoPtr<IWifiManager> wM = IWifiManager::Probe(obj);
    //persist
    wM->SetCountryCode(country, TRUE);
}

AutoPtr<IComparable> MccTable::GetObj(
    /* [in] */ MccEntry* base)
{
    AutoPtr<IComparable> obj = base;
    return obj;
}

AutoPtr<IArrayList> MccTable::InitTable()
{
    AutoPtr<IArrayList> table;
    CArrayList::New(240, (IArrayList**)&table);

    /*
     * The table below is built from two resources:
     *
     * 1) ITU "Mobile Network Code (MNC) for the international
     *   identification plan for mobile terminals and mobile users"
     *   which is available as an annex to the ITU operational bulletin
     *   available here: http://www.itu.Int32/itu-t/bulletin/annex.html
     *
     * 2) The ISO 3166 country codes list, available here:
     *    http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/index.html
     *
     * This table has not been verified.
     *
     */
    table->Add(GetObj(new MccEntry(202, String("gr"),2)));   //Greece
    table->Add(GetObj(new MccEntry(204, String("nl"),2, String("nl"))));  //Netherlands (Kingdom of the)
    table->Add(GetObj(new MccEntry(206, String("be"),2)));   //Belgium
    table->Add(GetObj(new MccEntry(208, String("fr"),2, String("fr"))));  //France
    table->Add(GetObj(new MccEntry(212, String("mc"),2)));   //Monaco (Principality of)
    table->Add(GetObj(new MccEntry(213, String("ad"),2)));   //Andorra (Principality of)
    table->Add(GetObj(new MccEntry(214, String("es"),2, String("es"))));  //Spain
    table->Add(GetObj(new MccEntry(216, String("hu"),2, String("hu"))));  //Hungary (Republic of)
    table->Add(GetObj(new MccEntry(218, String("ba"),2)));   //Bosnia and Herzegovina
    table->Add(GetObj(new MccEntry(219, String("hr"),2, String("hr"))));  //Croatia (Republic of)
    table->Add(GetObj(new MccEntry(220, String("rs"),2)));   //Serbia and Montenegro
    table->Add(GetObj(new MccEntry(222, String("it"),2, String("it"))));  //Italy
    table->Add(GetObj(new MccEntry(225, String("va"),2, String("it"))));  //Vatican City State
    table->Add(GetObj(new MccEntry(226, String("ro"),2)));   //Romania
    table->Add(GetObj(new MccEntry(228, String("ch"),2, String("de"))));  //Switzerland (Confederation of)
    table->Add(GetObj(new MccEntry(230, String("cz"),2, String("cs"))));  //Czech Republic
    table->Add(GetObj(new MccEntry(231, String("sk"),2)));   //Slovak Republic
    table->Add(GetObj(new MccEntry(232, String("at"),2, String("de"))));  //Austria
    table->Add(GetObj(new MccEntry(234, String("gb"),2, String("en"))));  //United Kingdom of Great Britain and Northern Ireland
    table->Add(GetObj(new MccEntry(235, String("gb"),2, String("en"))));  //United Kingdom of Great Britain and Northern Ireland
    table->Add(GetObj(new MccEntry(238, String("dk"),2)));   //Denmark
    table->Add(GetObj(new MccEntry(240, String("se"),2, String("sv"))));  //Sweden
    table->Add(GetObj(new MccEntry(242, String("no"),2)));   //Norway
    table->Add(GetObj(new MccEntry(244, String("fi"),2)));   //Finland
    table->Add(GetObj(new MccEntry(246, String("lt"),2)));   //Lithuania (Republic of)
    table->Add(GetObj(new MccEntry(247, String("lv"),2)));   //Latvia (Republic of)
    table->Add(GetObj(new MccEntry(248, String("ee"),2)));   //Estonia (Republic of)
    table->Add(GetObj(new MccEntry(250, String("ru"),2)));   //Russian Federation
    table->Add(GetObj(new MccEntry(255, String("ua"),2)));   //Ukraine
    table->Add(GetObj(new MccEntry(257, String("by"),2)));   //Belarus (Republic of)
    table->Add(GetObj(new MccEntry(259, String("md"),2)));   //Moldova (Republic of)
    table->Add(GetObj(new MccEntry(260, String("pl"),2)));   //Poland (Republic of)
    table->Add(GetObj(new MccEntry(262, String("de"),2, String("de"))));  //Germany (Federal Republic of)
    table->Add(GetObj(new MccEntry(266, String("gi"),2)));   //Gibraltar
    table->Add(GetObj(new MccEntry(268, String("pt"),2)));   //Portugal
    table->Add(GetObj(new MccEntry(270, String("lu"),2)));   //Luxembourg
    table->Add(GetObj(new MccEntry(272, String("ie"),2, String("en"))));  //Ireland
    table->Add(GetObj(new MccEntry(274, String("is"),2)));   //Iceland
    table->Add(GetObj(new MccEntry(276, String("al"),2)));   //Albania (Republic of)
    table->Add(GetObj(new MccEntry(278, String("mt"),2)));   //Malta
    table->Add(GetObj(new MccEntry(280, String("cy"),2)));   //Cyprus (Republic of)
    table->Add(GetObj(new MccEntry(282, String("ge"),2)));   //Georgia
    table->Add(GetObj(new MccEntry(283, String("am"),2)));   //Armenia (Republic of)
    table->Add(GetObj(new MccEntry(284, String("bg"),2)));   //Bulgaria (Republic of)
    table->Add(GetObj(new MccEntry(286, String("tr"),2)));   //Turkey
    table->Add(GetObj(new MccEntry(288, String("fo"),2)));   //Faroe Islands
    table->Add(GetObj(new MccEntry(289, String("ge"),2)));    //Abkhazia (Georgia)
    table->Add(GetObj(new MccEntry(290, String("gl"),2)));   //Greenland (Denmark)
    table->Add(GetObj(new MccEntry(292, String("sm"),2)));   //San Marino (Republic of)
    table->Add(GetObj(new MccEntry(293, String("si"),2)));   //Slovenia (Republic of)
    table->Add(GetObj(new MccEntry(294, String("mk"),2)));   //The Former Yugoslav Republic of Macedonia
    table->Add(GetObj(new MccEntry(295, String("li"),2)));   //Liechtenstein (Principality of)
    table->Add(GetObj(new MccEntry(297, String("me"),2)));    //Montenegro (Republic of)
    table->Add(GetObj(new MccEntry(302, String("ca"),3, String("en"))));  //Canada
    table->Add(GetObj(new MccEntry(308, String("pm"),2)));   //Saint Pierre and Miquelon (Collectivit territoriale de la Rpublique franaise)
    table->Add(GetObj(new MccEntry(310, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(311, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(312, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(313, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(314, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(315, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(316, String("us"),3, String("en"))));  //United States of America
    table->Add(GetObj(new MccEntry(330, String("pr"),2)));   //Puerto Rico
    table->Add(GetObj(new MccEntry(332, String("vi"),2)));   //United States Virgin Islands
    table->Add(GetObj(new MccEntry(334, String("mx"),3)));   //Mexico
    table->Add(GetObj(new MccEntry(338, String("jm"),3)));   //Jamaica
    table->Add(GetObj(new MccEntry(340, String("gp"),2)));   //Guadeloupe (French Department of)
    table->Add(GetObj(new MccEntry(342, String("bb"),3)));   //Barbados
    table->Add(GetObj(new MccEntry(344, String("ag"),3)));   //Antigua and Barbuda
    table->Add(GetObj(new MccEntry(346, String("ky"),3)));   //Cayman Islands
    table->Add(GetObj(new MccEntry(348, String("vg"),3)));   //British Virgin Islands
    table->Add(GetObj(new MccEntry(350, String("bm"),2)));   //Bermuda
    table->Add(GetObj(new MccEntry(352, String("gd"),2)));   //Grenada
    table->Add(GetObj(new MccEntry(354, String("ms"),2)));   //Montserrat
    table->Add(GetObj(new MccEntry(356, String("kn"),2)));   //Saint Kitts and Nevis
    table->Add(GetObj(new MccEntry(358, String("lc"),2)));   //Saint Lucia
    table->Add(GetObj(new MccEntry(360, String("vc"),2)));   //Saint Vincent and the Grenadines
    table->Add(GetObj(new MccEntry(362, String("ai"),2)));   //Netherlands Antilles
    table->Add(GetObj(new MccEntry(363, String("aw"),2)));   //Aruba
    table->Add(GetObj(new MccEntry(364, String("bs"),2)));   //Bahamas (Commonwealth of the)
    table->Add(GetObj(new MccEntry(365, String("ai"),3)));   //Anguilla
    table->Add(GetObj(new MccEntry(366, String("dm"),2)));   //Dominica (Commonwealth of)
    table->Add(GetObj(new MccEntry(368, String("cu"),2)));   //Cuba
    table->Add(GetObj(new MccEntry(370, String("do"),2)));   //Dominican Republic
    table->Add(GetObj(new MccEntry(372, String("ht"),2)));   //Haiti (Republic of)
    table->Add(GetObj(new MccEntry(374, String("tt"),2)));   //Trinidad and Tobago
    table->Add(GetObj(new MccEntry(376, String("tc"),2)));   //Turks and Caicos Islands
    table->Add(GetObj(new MccEntry(400, String("az"),2)));   //Azerbaijani Republic
    table->Add(GetObj(new MccEntry(401, String("kz"),2)));   //Kazakhstan (Republic of)
    table->Add(GetObj(new MccEntry(402, String("bt"),2)));   //Bhutan (Kingdom of)
    table->Add(GetObj(new MccEntry(404, String("in"),2)));   //India (Republic of)
    table->Add(GetObj(new MccEntry(405, String("in"),2)));   //India (Republic of)
    table->Add(GetObj(new MccEntry(410, String("pk"),2)));   //Pakistan (Islamic Republic of)
    table->Add(GetObj(new MccEntry(412, String("af"),2)));   //Afghanistan
    table->Add(GetObj(new MccEntry(413, String("lk"),2)));   //Sri Lanka (Democratic Socialist Republic of)
    table->Add(GetObj(new MccEntry(414, String("mm"),2)));   //Myanmar (Union of)
    table->Add(GetObj(new MccEntry(415, String("lb"),2)));   //Lebanon
    table->Add(GetObj(new MccEntry(416, String("jo"),2)));   //Jordan (Hashemite Kingdom of)
    table->Add(GetObj(new MccEntry(417, String("sy"),2)));   //Syrian Arab Republic
    table->Add(GetObj(new MccEntry(418, String("iq"),2)));   //Iraq (Republic of)
    table->Add(GetObj(new MccEntry(419, String("kw"),2)));   //Kuwait (State of)
    table->Add(GetObj(new MccEntry(420, String("sa"),2)));   //Saudi Arabia (Kingdom of)
    table->Add(GetObj(new MccEntry(421, String("ye"),2)));   //Yemen (Republic of)
    table->Add(GetObj(new MccEntry(422, String("om"),2)));   //Oman (Sultanate of)
    table->Add(GetObj(new MccEntry(423, String("ps"),2)));    //Palestine
    table->Add(GetObj(new MccEntry(424, String("ae"),2)));   //United Arab Emirates
    table->Add(GetObj(new MccEntry(425, String("il"),2)));   //Israel (State of)
    table->Add(GetObj(new MccEntry(426, String("bh"),2)));   //Bahrain (Kingdom of)
    table->Add(GetObj(new MccEntry(427, String("qa"),2)));   //Qatar (State of)
    table->Add(GetObj(new MccEntry(428, String("mn"),2)));   //Mongolia
    table->Add(GetObj(new MccEntry(429, String("np"),2)));   //Nepal
    table->Add(GetObj(new MccEntry(430, String("ae"),2)));   //United Arab Emirates
    table->Add(GetObj(new MccEntry(431, String("ae"),2)));   //United Arab Emirates
    table->Add(GetObj(new MccEntry(432, String("ir"),2)));   //Iran (Islamic Republic of)
    table->Add(GetObj(new MccEntry(434, String("uz"),2)));   //Uzbekistan (Republic of)
    table->Add(GetObj(new MccEntry(436, String("tj"),2)));   //Tajikistan (Republic of)
    table->Add(GetObj(new MccEntry(437, String("kg"),2)));   //Kyrgyz Republic
    table->Add(GetObj(new MccEntry(438, String("tm"),2)));   //Turkmenistan
    table->Add(GetObj(new MccEntry(440, String("jp"),2, String("ja"))));  //Japan
    table->Add(GetObj(new MccEntry(441, String("jp"),2, String("ja"))));  //Japan
    table->Add(GetObj(new MccEntry(450, String("kr"),2, String("ko"))));  //Korea (Republic of)
    table->Add(GetObj(new MccEntry(452, String("vn"),2)));   //Viet Nam (Socialist Republic of)
    table->Add(GetObj(new MccEntry(454, String("hk"),2)));   //"Hong Kong, China"
    table->Add(GetObj(new MccEntry(455, String("mo"),2)));   //"Macao, China"
    table->Add(GetObj(new MccEntry(456, String("kh"),2)));   //Cambodia (Kingdom of)
    table->Add(GetObj(new MccEntry(457, String("la"),2)));   //Lao People's Democratic Republic
    table->Add(GetObj(new MccEntry(460, String("cn"),2, String("zh"))));  //China (People's Republic of)
    table->Add(GetObj(new MccEntry(461, String("cn"),2, String("zh"))));  //China (People's Republic of)
    table->Add(GetObj(new MccEntry(466, String("tw"),2)));   //"Taiwan, China"
    table->Add(GetObj(new MccEntry(467, String("kp"),2)));   //Democratic People's Republic of Korea
    table->Add(GetObj(new MccEntry(470, String("bd"),2)));   //Bangladesh (People's Republic of)
    table->Add(GetObj(new MccEntry(472, String("mv"),2)));   //Maldives (Republic of)
    table->Add(GetObj(new MccEntry(502, String("my"),2)));   //Malaysia
    table->Add(GetObj(new MccEntry(505, String("au"),2, String("en"))));  //Australia
    table->Add(GetObj(new MccEntry(510, String("id"),2)));   //Indonesia (Republic of)
    table->Add(GetObj(new MccEntry(514, String("tl"),2)));   //Democratic Republic of Timor-Leste
    table->Add(GetObj(new MccEntry(515, String("ph"),2)));   //Philippines (Republic of the)
    table->Add(GetObj(new MccEntry(520, String("th"),2)));   //Thailand
    table->Add(GetObj(new MccEntry(525, String("sg"),2, String("en"))));  //Singapore (Republic of)
    table->Add(GetObj(new MccEntry(528, String("bn"),2)));   //Brunei Darussalam
    table->Add(GetObj(new MccEntry(530, String("nz"),2, String("en")))); //New Zealand
    table->Add(GetObj(new MccEntry(534, String("mp"),2)));   //Northern Mariana Islands (Commonwealth of the)
    table->Add(GetObj(new MccEntry(535, String("gu"),2)));   //Guam
    table->Add(GetObj(new MccEntry(536, String("nr"),2)));   //Nauru (Republic of)
    table->Add(GetObj(new MccEntry(537, String("pg"),2)));   //Papua New Guinea
    table->Add(GetObj(new MccEntry(539, String("to"),2)));   //Tonga (Kingdom of)
    table->Add(GetObj(new MccEntry(540, String("sb"),2)));   //Solomon Islands
    table->Add(GetObj(new MccEntry(541, String("vu"),2)));   //Vanuatu (Republic of)
    table->Add(GetObj(new MccEntry(542, String("fj"),2)));   //Fiji (Republic of)
    table->Add(GetObj(new MccEntry(543, String("wf"),2)));   //Wallis and Futuna (Territoire franais d'outre-mer)
    table->Add(GetObj(new MccEntry(544, String("as"),2)));   //American Samoa
    table->Add(GetObj(new MccEntry(545, String("ki"),2)));   //Kiribati (Republic of)
    table->Add(GetObj(new MccEntry(546, String("nc"),2)));   //New Caledonia (Territoire franais d'outre-mer)
    table->Add(GetObj(new MccEntry(547, String("pf"),2)));   //French Polynesia (Territoire franais d'outre-mer)
    table->Add(GetObj(new MccEntry(548, String("ck"),2)));   //Cook Islands
    table->Add(GetObj(new MccEntry(549, String("ws"),2)));   //Samoa (Independent State of)
    table->Add(GetObj(new MccEntry(550, String("fm"),2)));   //Micronesia (Federated States of)
    table->Add(GetObj(new MccEntry(551, String("mh"),2)));   //Marshall Islands (Republic of the)
    table->Add(GetObj(new MccEntry(552, String("pw"),2)));   //Palau (Republic of)
    table->Add(GetObj(new MccEntry(553, String("tv"),2)));   //Tuvalu
    table->Add(GetObj(new MccEntry(555, String("nu"),2)));   //Niue
    table->Add(GetObj(new MccEntry(602, String("eg"),2)));   //Egypt (Arab Republic of)
    table->Add(GetObj(new MccEntry(603, String("dz"),2)));   //Algeria (People's Democratic Republic of)
    table->Add(GetObj(new MccEntry(604, String("ma"),2)));   //Morocco (Kingdom of)
    table->Add(GetObj(new MccEntry(605, String("tn"),2)));   //Tunisia
    table->Add(GetObj(new MccEntry(606, String("ly"),2)));   //Libya (Socialist People's Libyan Arab Jamahiriya)
    table->Add(GetObj(new MccEntry(607, String("gm"),2)));   //Gambia (Republic of the)
    table->Add(GetObj(new MccEntry(608, String("sn"),2)));   //Senegal (Republic of)
    table->Add(GetObj(new MccEntry(609, String("mr"),2)));   //Mauritania (Islamic Republic of)
    table->Add(GetObj(new MccEntry(610, String("ml"),2)));   //Mali (Republic of)
    table->Add(GetObj(new MccEntry(611, String("gn"),2)));   //Guinea (Republic of)
    table->Add(GetObj(new MccEntry(612, String("ci"),2)));   //Cte d'Ivoire (Republic of)
    table->Add(GetObj(new MccEntry(613, String("bf"),2)));   //Burkina Faso
    table->Add(GetObj(new MccEntry(614, String("ne"),2)));   //Niger (Republic of the)
    table->Add(GetObj(new MccEntry(615, String("tg"),2)));   //Togolese Republic
    table->Add(GetObj(new MccEntry(616, String("bj"),2)));   //Benin (Republic of)
    table->Add(GetObj(new MccEntry(617, String("mu"),2)));   //Mauritius (Republic of)
    table->Add(GetObj(new MccEntry(618, String("lr"),2)));   //Liberia (Republic of)
    table->Add(GetObj(new MccEntry(619, String("sl"),2)));   //Sierra Leone
    table->Add(GetObj(new MccEntry(620, String("gh"),2)));   //Ghana
    table->Add(GetObj(new MccEntry(621, String("ng"),2)));   //Nigeria (Federal Republic of)
    table->Add(GetObj(new MccEntry(622, String("td"),2)));   //Chad (Republic of)
    table->Add(GetObj(new MccEntry(623, String("cf"),2)));   //Central African Republic
    table->Add(GetObj(new MccEntry(624, String("cm"),2)));   //Cameroon (Republic of)
    table->Add(GetObj(new MccEntry(625, String("cv"),2)));   //Cape Verde (Republic of)
    table->Add(GetObj(new MccEntry(626, String("st"),2)));   //Sao Tome and Principe (Democratic Republic of)
    table->Add(GetObj(new MccEntry(627, String("gq"),2)));   //Equatorial Guinea (Republic of)
    table->Add(GetObj(new MccEntry(628, String("ga"),2)));   //Gabonese Republic
    table->Add(GetObj(new MccEntry(629, String("cg"),2)));   //Congo (Republic of the)
    table->Add(GetObj(new MccEntry(630, String("cd"),2)));   //Democratic Republic of the Congo
    table->Add(GetObj(new MccEntry(631, String("ao"),2)));   //Angola (Republic of)
    table->Add(GetObj(new MccEntry(632, String("gw"),2)));   //Guinea-Bissau (Republic of)
    table->Add(GetObj(new MccEntry(633, String("sc"),2)));   //Seychelles (Republic of)
    table->Add(GetObj(new MccEntry(634, String("sd"),2)));   //Sudan (Republic of the)
    table->Add(GetObj(new MccEntry(635, String("rw"),2)));   //Rwanda (Republic of)
    table->Add(GetObj(new MccEntry(636, String("et"),2)));   //Ethiopia (Federal Democratic Republic of)
    table->Add(GetObj(new MccEntry(637, String("so"),2)));   //Somali Democratic Republic
    table->Add(GetObj(new MccEntry(638, String("dj"),2)));   //Djibouti (Republic of)
    table->Add(GetObj(new MccEntry(639, String("ke"),2)));   //Kenya (Republic of)
    table->Add(GetObj(new MccEntry(640, String("tz"),2)));   //Tanzania (United Republic of)
    table->Add(GetObj(new MccEntry(641, String("ug"),2)));   //Uganda (Republic of)
    table->Add(GetObj(new MccEntry(642, String("bi"),2)));   //Burundi (Republic of)
    table->Add(GetObj(new MccEntry(643, String("mz"),2)));   //Mozambique (Republic of)
    table->Add(GetObj(new MccEntry(645, String("zm"),2)));   //Zambia (Republic of)
    table->Add(GetObj(new MccEntry(646, String("mg"),2)));   //Madagascar (Republic of)
    table->Add(GetObj(new MccEntry(647, String("re"),2)));   //Reunion (French Department of)
    table->Add(GetObj(new MccEntry(648, String("zw"),2)));   //Zimbabwe (Republic of)
    table->Add(GetObj(new MccEntry(649, String("na"),2)));   //Namibia (Republic of)
    table->Add(GetObj(new MccEntry(650, String("mw"),2)));   //Malawi
    table->Add(GetObj(new MccEntry(651, String("ls"),2)));   //Lesotho (Kingdom of)
    table->Add(GetObj(new MccEntry(652, String("bw"),2)));   //Botswana (Republic of)
    table->Add(GetObj(new MccEntry(653, String("sz"),2)));   //Swaziland (Kingdom of)
    table->Add(GetObj(new MccEntry(654, String("km"),2)));   //Comoros (Union of the)
    table->Add(GetObj(new MccEntry(655, String("za"),2, String("en"))));  //South Africa (Republic of)
    table->Add(GetObj(new MccEntry(657, String("er"),2)));   //Eritrea
    table->Add(GetObj(new MccEntry(659, String("ss"),2)));   //South Sudan (Republic of)
    table->Add(GetObj(new MccEntry(702, String("bz"),2)));   //Belize
    table->Add(GetObj(new MccEntry(704, String("gt"),2)));   //Guatemala (Republic of)
    table->Add(GetObj(new MccEntry(706, String("sv"),2)));   //El Salvador (Republic of)
    table->Add(GetObj(new MccEntry(708, String("hn"),3)));   //Honduras (Republic of)
    table->Add(GetObj(new MccEntry(710, String("ni"),2)));   //Nicaragua
    table->Add(GetObj(new MccEntry(712, String("cr"),2)));   //Costa Rica
    table->Add(GetObj(new MccEntry(714, String("pa"),2)));   //Panama (Republic of)
    table->Add(GetObj(new MccEntry(716, String("pe"),2)));   //Peru
    table->Add(GetObj(new MccEntry(722, String("ar"),3)));   //Argentine Republic
    table->Add(GetObj(new MccEntry(724, String("br"),2)));   //Brazil (Federative Republic of)
    table->Add(GetObj(new MccEntry(730, String("cl"),2)));   //Chile
    table->Add(GetObj(new MccEntry(732, String("co"),3)));   //Colombia (Republic of)
    table->Add(GetObj(new MccEntry(734, String("ve"),2)));   //Venezuela (Bolivarian Republic of)
    table->Add(GetObj(new MccEntry(736, String("bo"),2)));   //Bolivia (Republic of)
    table->Add(GetObj(new MccEntry(738, String("gy"),2)));   //Guyana
    table->Add(GetObj(new MccEntry(740, String("ec"),2)));   //Ecuador
    table->Add(GetObj(new MccEntry(742, String("gf"),2)));   //French Guiana (French Department of)
    table->Add(GetObj(new MccEntry(744, String("py"),2)));   //Paraguay (Republic of)
    table->Add(GetObj(new MccEntry(746, String("sr"),2)));   //Suriname (Republic of)
    table->Add(GetObj(new MccEntry(748, String("uy"),2)));   //Uruguay (Eastern Republic of)
    table->Add(GetObj(new MccEntry(750, String("fk"),2)));   //Falkland Islands (Malvinas)
    //table->Add(new MccEntry(901, String(""), 2));    //"International Mobile, shared code"

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(IList::Probe(table));
    return table;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
