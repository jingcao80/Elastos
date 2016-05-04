/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ISlog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using libcore::Icu::ITimeZoneNames;

/**
 * Mobile Country Code
 *
 * {@hide}
 */
public class MccTable
{
    static const String LOG_TAG = "MccTable";

    static ArrayList<MccEntry> sTable;

    static class MccEntry implements Comparable<MccEntry>
    {
        Int32 mMcc;
        String mIso;
        Int32 mSmallestDigitsMnc;
        String mLanguage;

        MccEntry(Int32 mnc, String iso, Int32 smallestDigitsMCC) {
            This(mnc, iso, smallestDigitsMCC, NULL);
        }

        MccEntry(Int32 mnc, String iso, Int32 smallestDigitsMCC, String language) {
            mMcc = mnc;
            mIso = iso;
            mSmallestDigitsMnc = smallestDigitsMCC;
            mLanguage = language;
        }


        //@Override
        public Int32 CompareTo(MccEntry o)
        {
            return mMcc - o.mMcc;
        }
    }

    private static MccEntry
    EntryForMcc(Int32 mcc)
    {
        Int32 index;

        MccEntry m;

        m = new MccEntry(mcc, NULL, 0);

        index = Collections->BinarySearch(sTable, m);

        If (index < 0) {
            return NULL;
        } else {
            return sTable->Get(index);
        }
    }

    /**
     * Returns a default time zone ID for the given MCC.
     * @param mcc Mobile Country Code
     * @return default TimeZone ID, or NULL if not specified
     */
    public static String DefaultTimeZoneForMcc(Int32 mcc) {
        MccEntry entry;

        entry = EntryForMcc(mcc);
        If (entry == NULL || entry.mIso == NULL) {
            return NULL;
        } else {
            Locale locale;
            If (entry.mLanguage == NULL) {
                locale = new Locale(entry.mIso);
            } else {
                locale = new Locale(entry.mLanguage, entry.mIso);
            }
            String[] tz = TimeZoneNames->ForLocale(locale);
            If (tz.length == 0) return NULL;
            return tz[0];
        }
    }

    /**
     * Given a GSM Mobile Country Code, returns
     * an ISO two-character country code if available.
     * Returns "" if unavailable.
     */
    public static String
    CountryCodeForMcc(Int32 mcc)
    {
        MccEntry entry;

        entry = EntryForMcc(mcc);

        If (entry == NULL) {
            return "";
        } else {
            return entry.mIso;
        }
    }

    /**
     * Given a GSM Mobile Country Code, returns
     * an ISO 2-3 character language code if available.
     * Returns NULL if unavailable.
     */
    public static String DefaultLanguageForMcc(Int32 mcc) {
        MccEntry entry;

        entry = EntryForMcc(mcc);

        If (entry == NULL) {
            return NULL;
        } else {
            return entry.mLanguage;
        }
    }

    /**
     * Given a GSM Mobile Country Code, returns
     * the smallest number of digits that M if available.
     * Returns 2 if unavailable.
     */
    public static Int32
    SmallestDigitsMccForMnc(Int32 mcc)
    {
        MccEntry entry;

        entry = EntryForMcc(mcc);

        If (entry == NULL) {
            return 2;
        } else {
            return entry.mSmallestDigitsMnc;
        }
    }

    /**
     * Updates MCC and MNC device configuration information for application retrieving
     * correct version of resources.  If MCC is 0, MCC and MNC will be Ignored (not set).
     * @param context Context to act on.
     * @param mccmnc truncated imsi with just the MCC and MNC - MNC assumed to be from 4th to end
     * @param fromServiceState TRUE if coming from the radio service state, FALSE if from SIM
     */
    public static void UpdateMccMncConfiguration(Context context, String mccmnc,
            Boolean fromServiceState) {
        Slogger::D(LOG_TAG, "updateMccMncConfiguration mccmnc='" + mccmnc + "' fromServiceState=" + fromServiceState);
        If (!TextUtils->IsEmpty(mccmnc)) {
            Int32 mcc, mnc;

            String defaultMccMnc = TelephonyManager->GetDefault()->GetSimOperator();
            Slogger::D(LOG_TAG, "updateMccMncConfiguration defaultMccMnc=" + defaultMccMnc);
            //Update mccmnc only for default subscription in case of MultiSim.
//            If (!defaultMccMnc->Equals(mccmnc)) {
//                Slogger::D(LOG_TAG, "Not a Default subscription, ignoring mccmnc config update.");
//                return;
//            }

            try {
                mcc = Integer->ParseInt(mccmnc->Substring(0,3));
                mnc = Integer->ParseInt(mccmnc->Substring(3));
            } Catch (NumberFormatException e) {
                Slogger::E(LOG_TAG, "Error parsing IMSI");
                return;
            }

            Slogger::D(LOG_TAG, "updateMccMncConfiguration: mcc=" + mcc + ", mnc=" + mnc);

            Locale locale = NULL;
            If (mcc != 0) {
                SetTimezoneFromMccIfNeeded(context, mcc);
                locale = GetLocaleFromMcc(context, mcc);
            }
            If (fromServiceState) {
                SetWifiCountryCodeFromMcc(context, mcc);
            } else {
                // from SIM
                try {
                    Configuration config = new Configuration();
                    Boolean updateConfig = FALSE;
                    If (mcc != 0) {
                        config.mcc = mcc;
                        config.mnc = mnc == 0 ? Configuration.MNC_ZERO : mnc;
                        updateConfig = TRUE;
                    }
                    If (locale != NULL) {
                        config->SetLocale(locale);
                        updateConfig = TRUE;
                    }
                    If (updateConfig) {
                        Slogger::D(LOG_TAG, "updateMccMncConfiguration updateConfig config=" + config);
                        ActivityManagerNative->GetDefault()->UpdateConfiguration(config);
                    } else {
                        Slogger::D(LOG_TAG, "updateMccMncConfiguration nothing to update");
                    }
                } Catch (RemoteException e) {
                    Slogger::E(LOG_TAG, "Can't update configuration", e);
                }
            }
        } else {
            If (fromServiceState) {
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
    public static Locale GetLocaleForLanguageCountry(Context context, String language,
            String country) {
        If (language == NULL) {
            Slogger::D(LOG_TAG, "getLocaleForLanguageCountry: skipping no language");
            return NULL; // no match possible
        }
        If (country == NULL) {
            country = ""; // The Locale constructor throws if passed NULL.
        }

        // Note: persist.always.persist.locale actually means the opposite!
        Boolean alwaysPersist = FALSE;
        If (Build.IS_DEBUGGABLE) {
            alwaysPersist = SystemProperties->GetBoolean("persist.always.persist.locale", FALSE);
        }
        String persistSysLanguage = SystemProperties->Get("persist.sys.language", "");
        String persistSysCountry = SystemProperties->Get("persist.sys.country", "");
        If (!(alwaysPersist || (persistSysLanguage->IsEmpty() && persistSysCountry->IsEmpty()))) {
            Slogger::D(LOG_TAG, "getLocaleForLanguageCountry: skipping already persisted");
            return NULL;
        }

        // Find the best match we actually have a localization for.
        // TODO: this should really follow the CLDR chain of parent locales!
        final Locale target = new Locale(language, country);
        try {
            String[] localeArray = context->GetAssets()->GetLocales();
            List<String> locales = new ArrayList<>(Arrays->AsList(localeArray));

            // Even in developer mode, you don't want the pseudolocales.
            locales->Remove("ar-XB");
            locales->Remove("en-XA");

            Locale firstMatch = NULL;
            For (String locale : locales) {
                final Locale l = Locale->ForLanguageTag(locale->Replace('_', '-'));

                // Only consider locales with both language and country.
                If (l == NULL || "und".Equals(l->GetLanguage()) ||
                        l->GetLanguage()->IsEmpty() || l->GetCountry()->IsEmpty()) {
                    continue;
                }
                If (l->GetLanguage()->Equals(target->GetLanguage())) {
                    // If we got a perfect match, we're done.
                    If (l->GetCountry()->Equals(target->GetCountry())) {
                        Slogger::D(LOG_TAG, "getLocaleForLanguageCountry: got perfect match: " +
                               l->ToLanguageTag());
                        return l;
                    }
                    // Otherwise somewhat arbitrarily take the first locale for the language,
                    // unless we get a perfect match later. Note that these come back in no
                    // particular order, so there's no reason to think the first match is
                    // a particularly good match.
                    If (firstMatch == NULL) {
                        firstMatch = l;
                    }
                }
            }

            // We didn't find the exact locale, so return whichever locale we saw first where
            // the language Matched (if any).
            If (firstMatch != NULL) {
                Slogger::D(LOG_TAG, "getLocaleForLanguageCountry: got a language-only match: " +
                       firstMatch->ToLanguageTag());
                return firstMatch;
            } else {
                Slogger::D(LOG_TAG, "getLocaleForLanguageCountry: no locales for language " +
                       language);
            }
        } Catch (Exception e) {
            Slogger::D(LOG_TAG, "getLocaleForLanguageCountry: exception", e);
        }

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
    public static void SetSystemLocale(Context context, String language, String country) {
        Locale locale = GetLocaleForLanguageCountry(context, language, country);
        If (locale != NULL) {
            Configuration config = new Configuration();
            config->SetLocale(locale);
            config.userSetLocale = FALSE;
            Slogger::D(LOG_TAG, "setSystemLocale: updateLocale config=" + config);
            try {
                ActivityManagerNative->GetDefault()->UpdateConfiguration(config);
            } Catch (RemoteException e) {
                Slogger::D(LOG_TAG, "setSystemLocale exception", e);
            }
        } else {
            Slogger::D(LOG_TAG, "setSystemLocale: no locale");
        }
    }

    /**
     * If the timezone is not already set, set it based on the MCC of the SIM.
     * @param context Context to act on.
     * @param mcc Mobile Country Code of the SIM or SIM-like Entity (build prop on CDMA)
     */
    private static void SetTimezoneFromMccIfNeeded(Context context, Int32 mcc) {
        String timezone = SystemProperties->Get(ServiceStateTracker.TIMEZONE_PROPERTY);
        If (timezone == NULL || timezone->Length() == 0) {
            String zoneId = DefaultTimeZoneForMcc(mcc);
            If (zoneId != NULL && zoneId->Length() > 0) {
                // Set time zone based on MCC
                AlarmManager alarm =
                        (AlarmManager) context->GetSystemService(Context.ALARM_SERVICE);
                alarm->SetTimeZone(zoneId);
                Slogger::D(LOG_TAG, "timezone set to "+zoneId);
            }
        }
    }

    /**
     * Get Locale based on the MCC of the SIM.
     * @param context Context to act on.
     * @param mcc Mobile Country Code of the SIM or SIM-like Entity (build prop on CDMA)
     *
     * @return locale for the mcc or NULL if none
     */
    private static Locale GetLocaleFromMcc(Context context, Int32 mcc) {
        String language = MccTable->DefaultLanguageForMcc(mcc);
        String country = MccTable->CountryCodeForMcc(mcc);

        Slogger::D(LOG_TAG, "getLocaleFromMcc to " + language + "_" + country + " mcc=" + mcc);
        return GetLocaleForLanguageCountry(context, language, country);
    }

    /**
     * Set the country code for wifi.  This sets allowed wifi channels based on the
     * country of the carrier we see.  If we can't see any, reset to 0 so we don't
     * broadcast on forbidden channels.
     * @param context Context to act on.
     * @param mcc Mobile Country Code of the operator.  0 if not known
     */
    private static void SetWifiCountryCodeFromMcc(Context context, Int32 mcc) {
        String country = MccTable->CountryCodeForMcc(mcc);
        Slogger::D(LOG_TAG, "WIFI_COUNTRY_CODE set to " + country);
        WifiManager wM = (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
        //persist
        wM->SetCountryCode(country, TRUE);
    }

    static {
        sTable = new ArrayList<MccEntry>(240);


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

		sTable->Add(new MccEntry(202,"gr",2));	//Greece
		sTable->Add(new MccEntry(204,"nl",2,"nl"));	//Netherlands (Kingdom of the)
		sTable->Add(new MccEntry(206,"be",2));	//Belgium
		sTable->Add(new MccEntry(208,"fr",2,"fr"));	//France
		sTable->Add(new MccEntry(212,"mc",2));	//Monaco (Principality of)
		sTable->Add(new MccEntry(213,"ad",2));	//Andorra (Principality of)
		sTable->Add(new MccEntry(214,"es",2,"es"));	//Spain
		sTable->Add(new MccEntry(216,"hu",2,"hu"));	//Hungary (Republic of)
		sTable->Add(new MccEntry(218,"ba",2));	//Bosnia and Herzegovina
		sTable->Add(new MccEntry(219,"hr",2,"hr"));	//Croatia (Republic of)
		sTable->Add(new MccEntry(220,"rs",2));	//Serbia and Montenegro
		sTable->Add(new MccEntry(222,"it",2,"it"));	//Italy
		sTable->Add(new MccEntry(225,"va",2,"it"));	//Vatican City State
		sTable->Add(new MccEntry(226,"ro",2));	//Romania
		sTable->Add(new MccEntry(228,"ch",2,"de"));	//Switzerland (Confederation of)
		sTable->Add(new MccEntry(230,"cz",2,"cs"));	//Czech Republic
		sTable->Add(new MccEntry(231,"sk",2));	//Slovak Republic
		sTable->Add(new MccEntry(232,"at",2,"de"));	//Austria
		sTable->Add(new MccEntry(234,"gb",2,"en"));	//United Kingdom of Great Britain and Northern Ireland
		sTable->Add(new MccEntry(235,"gb",2,"en"));	//United Kingdom of Great Britain and Northern Ireland
		sTable->Add(new MccEntry(238,"dk",2));	//Denmark
		sTable->Add(new MccEntry(240,"se",2,"sv"));	//Sweden
		sTable->Add(new MccEntry(242,"no",2));	//Norway
		sTable->Add(new MccEntry(244,"fi",2));	//Finland
		sTable->Add(new MccEntry(246,"lt",2));	//Lithuania (Republic of)
		sTable->Add(new MccEntry(247,"lv",2));	//Latvia (Republic of)
		sTable->Add(new MccEntry(248,"ee",2));	//Estonia (Republic of)
		sTable->Add(new MccEntry(250,"ru",2));	//Russian Federation
		sTable->Add(new MccEntry(255,"ua",2));	//Ukraine
		sTable->Add(new MccEntry(257,"by",2));	//Belarus (Republic of)
		sTable->Add(new MccEntry(259,"md",2));	//Moldova (Republic of)
		sTable->Add(new MccEntry(260,"pl",2));	//Poland (Republic of)
		sTable->Add(new MccEntry(262,"de",2,"de"));	//Germany (Federal Republic of)
		sTable->Add(new MccEntry(266,"gi",2));	//Gibraltar
		sTable->Add(new MccEntry(268,"pt",2));	//Portugal
		sTable->Add(new MccEntry(270,"lu",2));	//Luxembourg
		sTable->Add(new MccEntry(272,"ie",2,"en"));	//Ireland
		sTable->Add(new MccEntry(274,"is",2));	//Iceland
		sTable->Add(new MccEntry(276,"al",2));	//Albania (Republic of)
		sTable->Add(new MccEntry(278,"mt",2));	//Malta
		sTable->Add(new MccEntry(280,"cy",2));	//Cyprus (Republic of)
		sTable->Add(new MccEntry(282,"ge",2));	//Georgia
		sTable->Add(new MccEntry(283,"am",2));	//Armenia (Republic of)
		sTable->Add(new MccEntry(284,"bg",2));	//Bulgaria (Republic of)
		sTable->Add(new MccEntry(286,"tr",2));	//Turkey
		sTable->Add(new MccEntry(288,"fo",2));	//Faroe Islands
                sTable->Add(new MccEntry(289,"ge",2));    //Abkhazia (Georgia)
		sTable->Add(new MccEntry(290,"gl",2));	//Greenland (Denmark)
		sTable->Add(new MccEntry(292,"sm",2));	//San Marino (Republic of)
		sTable->Add(new MccEntry(293,"si",2));	//Slovenia (Republic of)
                sTable->Add(new MccEntry(294,"mk",2));   //The Former Yugoslav Republic of Macedonia
		sTable->Add(new MccEntry(295,"li",2));	//Liechtenstein (Principality of)
                sTable->Add(new MccEntry(297,"me",2));    //Montenegro (Republic of)
		sTable->Add(new MccEntry(302,"ca",3,"en"));	//Canada
		sTable->Add(new MccEntry(308,"pm",2));	//Saint Pierre and Miquelon (Collectivit territoriale de la Rpublique franaise)
		sTable->Add(new MccEntry(310,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(311,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(312,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(313,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(314,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(315,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(316,"us",3,"en"));	//United States of America
		sTable->Add(new MccEntry(330,"pr",2));	//Puerto Rico
		sTable->Add(new MccEntry(332,"vi",2));	//United States Virgin Islands
		sTable->Add(new MccEntry(334,"mx",3));	//Mexico
		sTable->Add(new MccEntry(338,"jm",3));	//Jamaica
		sTable->Add(new MccEntry(340,"gp",2));	//Guadeloupe (French Department of)
		sTable->Add(new MccEntry(342,"bb",3));	//Barbados
		sTable->Add(new MccEntry(344,"ag",3));	//Antigua and Barbuda
		sTable->Add(new MccEntry(346,"ky",3));	//Cayman Islands
		sTable->Add(new MccEntry(348,"vg",3));	//British Virgin Islands
		sTable->Add(new MccEntry(350,"bm",2));	//Bermuda
		sTable->Add(new MccEntry(352,"gd",2));	//Grenada
		sTable->Add(new MccEntry(354,"ms",2));	//Montserrat
		sTable->Add(new MccEntry(356,"kn",2));	//Saint Kitts and Nevis
		sTable->Add(new MccEntry(358,"lc",2));	//Saint Lucia
		sTable->Add(new MccEntry(360,"vc",2));	//Saint Vincent and the Grenadines
		sTable->Add(new MccEntry(362,"ai",2));	//Netherlands Antilles
		sTable->Add(new MccEntry(363,"aw",2));	//Aruba
		sTable->Add(new MccEntry(364,"bs",2));	//Bahamas (Commonwealth of the)
		sTable->Add(new MccEntry(365,"ai",3));	//Anguilla
		sTable->Add(new MccEntry(366,"dm",2));	//Dominica (Commonwealth of)
		sTable->Add(new MccEntry(368,"cu",2));	//Cuba
		sTable->Add(new MccEntry(370,"do",2));	//Dominican Republic
		sTable->Add(new MccEntry(372,"ht",2));	//Haiti (Republic of)
		sTable->Add(new MccEntry(374,"tt",2));	//Trinidad and Tobago
		sTable->Add(new MccEntry(376,"tc",2));	//Turks and Caicos Islands
		sTable->Add(new MccEntry(400,"az",2));	//Azerbaijani Republic
		sTable->Add(new MccEntry(401,"kz",2));	//Kazakhstan (Republic of)
		sTable->Add(new MccEntry(402,"bt",2));	//Bhutan (Kingdom of)
		sTable->Add(new MccEntry(404,"in",2));	//India (Republic of)
		sTable->Add(new MccEntry(405,"in",2));	//India (Republic of)
		sTable->Add(new MccEntry(410,"pk",2));	//Pakistan (Islamic Republic of)
		sTable->Add(new MccEntry(412,"af",2));	//Afghanistan
		sTable->Add(new MccEntry(413,"lk",2));	//Sri Lanka (Democratic Socialist Republic of)
		sTable->Add(new MccEntry(414,"mm",2));	//Myanmar (Union of)
		sTable->Add(new MccEntry(415,"lb",2));	//Lebanon
		sTable->Add(new MccEntry(416,"jo",2));	//Jordan (Hashemite Kingdom of)
		sTable->Add(new MccEntry(417,"sy",2));	//Syrian Arab Republic
		sTable->Add(new MccEntry(418,"iq",2));	//Iraq (Republic of)
		sTable->Add(new MccEntry(419,"kw",2));	//Kuwait (State of)
		sTable->Add(new MccEntry(420,"sa",2));	//Saudi Arabia (Kingdom of)
		sTable->Add(new MccEntry(421,"ye",2));	//Yemen (Republic of)
		sTable->Add(new MccEntry(422,"om",2));	//Oman (Sultanate of)
                sTable->Add(new MccEntry(423,"ps",2));    //Palestine
		sTable->Add(new MccEntry(424,"ae",2));	//United Arab Emirates
		sTable->Add(new MccEntry(425,"il",2));	//Israel (State of)
		sTable->Add(new MccEntry(426,"bh",2));	//Bahrain (Kingdom of)
		sTable->Add(new MccEntry(427,"qa",2));	//Qatar (State of)
		sTable->Add(new MccEntry(428,"mn",2));	//Mongolia
		sTable->Add(new MccEntry(429,"np",2));	//Nepal
		sTable->Add(new MccEntry(430,"ae",2));	//United Arab Emirates
		sTable->Add(new MccEntry(431,"ae",2));	//United Arab Emirates
		sTable->Add(new MccEntry(432,"ir",2));	//Iran (Islamic Republic of)
		sTable->Add(new MccEntry(434,"uz",2));	//Uzbekistan (Republic of)
		sTable->Add(new MccEntry(436,"tj",2));	//Tajikistan (Republic of)
		sTable->Add(new MccEntry(437,"kg",2));	//Kyrgyz Republic
		sTable->Add(new MccEntry(438,"tm",2));	//Turkmenistan
		sTable->Add(new MccEntry(440,"jp",2,"ja"));	//Japan
		sTable->Add(new MccEntry(441,"jp",2,"ja"));	//Japan
		sTable->Add(new MccEntry(450,"kr",2,"ko"));	//Korea (Republic of)
		sTable->Add(new MccEntry(452,"vn",2));	//Viet Nam (Socialist Republic of)
		sTable->Add(new MccEntry(454,"hk",2));	//"Hong Kong, China"
		sTable->Add(new MccEntry(455,"mo",2));	//"Macao, China"
		sTable->Add(new MccEntry(456,"kh",2));	//Cambodia (Kingdom of)
		sTable->Add(new MccEntry(457,"la",2));	//Lao People's Democratic Republic
		sTable->Add(new MccEntry(460,"cn",2,"zh"));	//China (People's Republic of)
		sTable->Add(new MccEntry(461,"cn",2,"zh"));	//China (People's Republic of)
		sTable->Add(new MccEntry(466,"tw",2));	//"Taiwan, China"
		sTable->Add(new MccEntry(467,"kp",2));	//Democratic People's Republic of Korea
		sTable->Add(new MccEntry(470,"bd",2));	//Bangladesh (People's Republic of)
		sTable->Add(new MccEntry(472,"mv",2));	//Maldives (Republic of)
		sTable->Add(new MccEntry(502,"my",2));	//Malaysia
		sTable->Add(new MccEntry(505,"au",2,"en"));	//Australia
		sTable->Add(new MccEntry(510,"id",2));	//Indonesia (Republic of)
		sTable->Add(new MccEntry(514,"tl",2));	//Democratic Republic of Timor-Leste
		sTable->Add(new MccEntry(515,"ph",2));	//Philippines (Republic of the)
		sTable->Add(new MccEntry(520,"th",2));	//Thailand
		sTable->Add(new MccEntry(525,"sg",2,"en"));	//Singapore (Republic of)
		sTable->Add(new MccEntry(528,"bn",2));	//Brunei Darussalam
		sTable->Add(new MccEntry(530,"nz",2, "en"));	//New Zealand
		sTable->Add(new MccEntry(534,"mp",2));	//Northern Mariana Islands (Commonwealth of the)
		sTable->Add(new MccEntry(535,"gu",2));	//Guam
		sTable->Add(new MccEntry(536,"nr",2));	//Nauru (Republic of)
		sTable->Add(new MccEntry(537,"pg",2));	//Papua New Guinea
		sTable->Add(new MccEntry(539,"to",2));	//Tonga (Kingdom of)
		sTable->Add(new MccEntry(540,"sb",2));	//Solomon Islands
		sTable->Add(new MccEntry(541,"vu",2));	//Vanuatu (Republic of)
		sTable->Add(new MccEntry(542,"fj",2));	//Fiji (Republic of)
		sTable->Add(new MccEntry(543,"wf",2));	//Wallis and Futuna (Territoire franais d'outre-mer)
		sTable->Add(new MccEntry(544,"as",2));	//American Samoa
		sTable->Add(new MccEntry(545,"ki",2));	//Kiribati (Republic of)
		sTable->Add(new MccEntry(546,"nc",2));	//New Caledonia (Territoire franais d'outre-mer)
		sTable->Add(new MccEntry(547,"pf",2));	//French Polynesia (Territoire franais d'outre-mer)
		sTable->Add(new MccEntry(548,"ck",2));	//Cook Islands
		sTable->Add(new MccEntry(549,"ws",2));	//Samoa (Independent State of)
		sTable->Add(new MccEntry(550,"fm",2));	//Micronesia (Federated States of)
		sTable->Add(new MccEntry(551,"mh",2));	//Marshall Islands (Republic of the)
		sTable->Add(new MccEntry(552,"pw",2));	//Palau (Republic of)
		sTable->Add(new MccEntry(553,"tv",2));	//Tuvalu
		sTable->Add(new MccEntry(555,"nu",2));	//Niue
		sTable->Add(new MccEntry(602,"eg",2));	//Egypt (Arab Republic of)
		sTable->Add(new MccEntry(603,"dz",2));	//Algeria (People's Democratic Republic of)
		sTable->Add(new MccEntry(604,"ma",2));	//Morocco (Kingdom of)
		sTable->Add(new MccEntry(605,"tn",2));	//Tunisia
		sTable->Add(new MccEntry(606,"ly",2));	//Libya (Socialist People's Libyan Arab Jamahiriya)
		sTable->Add(new MccEntry(607,"gm",2));	//Gambia (Republic of the)
		sTable->Add(new MccEntry(608,"sn",2));	//Senegal (Republic of)
		sTable->Add(new MccEntry(609,"mr",2));	//Mauritania (Islamic Republic of)
		sTable->Add(new MccEntry(610,"ml",2));	//Mali (Republic of)
		sTable->Add(new MccEntry(611,"gn",2));	//Guinea (Republic of)
		sTable->Add(new MccEntry(612,"ci",2));	//Cte d'Ivoire (Republic of)
		sTable->Add(new MccEntry(613,"bf",2));	//Burkina Faso
		sTable->Add(new MccEntry(614,"ne",2));	//Niger (Republic of the)
		sTable->Add(new MccEntry(615,"tg",2));	//Togolese Republic
		sTable->Add(new MccEntry(616,"bj",2));	//Benin (Republic of)
		sTable->Add(new MccEntry(617,"mu",2));	//Mauritius (Republic of)
		sTable->Add(new MccEntry(618,"lr",2));	//Liberia (Republic of)
		sTable->Add(new MccEntry(619,"sl",2));	//Sierra Leone
		sTable->Add(new MccEntry(620,"gh",2));	//Ghana
		sTable->Add(new MccEntry(621,"ng",2));	//Nigeria (Federal Republic of)
		sTable->Add(new MccEntry(622,"td",2));	//Chad (Republic of)
		sTable->Add(new MccEntry(623,"cf",2));	//Central African Republic
		sTable->Add(new MccEntry(624,"cm",2));	//Cameroon (Republic of)
		sTable->Add(new MccEntry(625,"cv",2));	//Cape Verde (Republic of)
		sTable->Add(new MccEntry(626,"st",2));	//Sao Tome and Principe (Democratic Republic of)
		sTable->Add(new MccEntry(627,"gq",2));	//Equatorial Guinea (Republic of)
		sTable->Add(new MccEntry(628,"ga",2));	//Gabonese Republic
		sTable->Add(new MccEntry(629,"cg",2));	//Congo (Republic of the)
		sTable->Add(new MccEntry(630,"cd",2));	//Democratic Republic of the Congo
		sTable->Add(new MccEntry(631,"ao",2));	//Angola (Republic of)
		sTable->Add(new MccEntry(632,"gw",2));	//Guinea-Bissau (Republic of)
		sTable->Add(new MccEntry(633,"sc",2));	//Seychelles (Republic of)
		sTable->Add(new MccEntry(634,"sd",2));	//Sudan (Republic of the)
		sTable->Add(new MccEntry(635,"rw",2));	//Rwanda (Republic of)
		sTable->Add(new MccEntry(636,"et",2));	//Ethiopia (Federal Democratic Republic of)
		sTable->Add(new MccEntry(637,"so",2));	//Somali Democratic Republic
		sTable->Add(new MccEntry(638,"dj",2));	//Djibouti (Republic of)
		sTable->Add(new MccEntry(639,"ke",2));	//Kenya (Republic of)
		sTable->Add(new MccEntry(640,"tz",2));	//Tanzania (United Republic of)
		sTable->Add(new MccEntry(641,"ug",2));	//Uganda (Republic of)
		sTable->Add(new MccEntry(642,"bi",2));	//Burundi (Republic of)
		sTable->Add(new MccEntry(643,"mz",2));	//Mozambique (Republic of)
		sTable->Add(new MccEntry(645,"zm",2));	//Zambia (Republic of)
		sTable->Add(new MccEntry(646,"mg",2));	//Madagascar (Republic of)
		sTable->Add(new MccEntry(647,"re",2));	//Reunion (French Department of)
		sTable->Add(new MccEntry(648,"zw",2));	//Zimbabwe (Republic of)
		sTable->Add(new MccEntry(649,"na",2));	//Namibia (Republic of)
		sTable->Add(new MccEntry(650,"mw",2));	//Malawi
		sTable->Add(new MccEntry(651,"ls",2));	//Lesotho (Kingdom of)
		sTable->Add(new MccEntry(652,"bw",2));	//Botswana (Republic of)
		sTable->Add(new MccEntry(653,"sz",2));	//Swaziland (Kingdom of)
		sTable->Add(new MccEntry(654,"km",2));	//Comoros (Union of the)
		sTable->Add(new MccEntry(655,"za",2,"en"));	//South Africa (Republic of)
		sTable->Add(new MccEntry(657,"er",2));	//Eritrea
		sTable->Add(new MccEntry(659,"ss",2));	//South Sudan (Republic of)
		sTable->Add(new MccEntry(702,"bz",2));	//Belize
		sTable->Add(new MccEntry(704,"gt",2));	//Guatemala (Republic of)
		sTable->Add(new MccEntry(706,"sv",2));	//El Salvador (Republic of)
		sTable->Add(new MccEntry(708,"hn",3));	//Honduras (Republic of)
		sTable->Add(new MccEntry(710,"ni",2));	//Nicaragua
		sTable->Add(new MccEntry(712,"cr",2));	//Costa Rica
		sTable->Add(new MccEntry(714,"pa",2));	//Panama (Republic of)
		sTable->Add(new MccEntry(716,"pe",2));	//Peru
		sTable->Add(new MccEntry(722,"ar",3));	//Argentine Republic
		sTable->Add(new MccEntry(724,"br",2));	//Brazil (Federative Republic of)
		sTable->Add(new MccEntry(730,"cl",2));	//Chile
		sTable->Add(new MccEntry(732,"co",3));	//Colombia (Republic of)
		sTable->Add(new MccEntry(734,"ve",2));	//Venezuela (Bolivarian Republic of)
		sTable->Add(new MccEntry(736,"bo",2));	//Bolivia (Republic of)
		sTable->Add(new MccEntry(738,"gy",2));	//Guyana
		sTable->Add(new MccEntry(740,"ec",2));	//Ecuador
		sTable->Add(new MccEntry(742,"gf",2));	//French Guiana (French Department of)
		sTable->Add(new MccEntry(744,"py",2));	//Paraguay (Republic of)
		sTable->Add(new MccEntry(746,"sr",2));	//Suriname (Republic of)
		sTable->Add(new MccEntry(748,"uy",2));	//Uruguay (Eastern Republic of)
		sTable->Add(new MccEntry(750,"fk",2));	//Falkland Islands (Malvinas)
        //table->Add(new MccEntry(901,"",2));	//"International Mobile, shared code"

        Collections->Sort(sTable);
    }
}
