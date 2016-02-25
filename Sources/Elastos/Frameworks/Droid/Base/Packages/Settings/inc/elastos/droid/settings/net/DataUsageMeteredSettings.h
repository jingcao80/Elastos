/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.settings.net;

using static android::Net::NetworkPolicy::ILIMIT_DISABLED;
using static android::Net::Wifi::WifiInfo::IremoveDoubleQuotes;
using static com::Android::Settings::DataUsageSummary::IhasReadyMobileRadio;
using static com::Android::Settings::DataUsageSummary::IhasWifiRadio;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::INetworkPolicy;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Panel to configure {@link NetworkPolicy#metered} for networks.
 */
public class DataUsageMeteredSettings extends SettingsPreferenceFragment implements Indexable {

    private static const Boolean SHOW_MOBILE_CATEGORY = FALSE;

    private NetworkPolicyManager mPolicyManager;
    private WifiManager mWifiManager;

    private NetworkPolicyEditor mPolicyEditor;

    private PreferenceCategory mMobileCategory;
    private PreferenceCategory mWifiCategory;
    private Preference mWifiDisabled;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        final Context context = GetActivity();

        mPolicyManager = NetworkPolicyManager->From(context);
        mWifiManager = (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);

        mPolicyEditor = new NetworkPolicyEditor(mPolicyManager);
        mPolicyEditor->Read();

        AddPreferencesFromResource(R.xml.data_usage_metered_prefs);
        mMobileCategory = (PreferenceCategory) FindPreference("mobile");
        mWifiCategory = (PreferenceCategory) FindPreference("wifi");
        mWifiDisabled = FindPreference("wifi_disabled");

        UpdateNetworks(context);
    }

    private void UpdateNetworks(Context context) {
        if (SHOW_MOBILE_CATEGORY && HasReadyMobileRadio(context)) {
            mMobileCategory->RemoveAll();
            mMobileCategory->AddPreference(BuildMobilePref(context));
        } else {
            GetPreferenceScreen()->RemovePreference(mMobileCategory);
        }

        mWifiCategory->RemoveAll();
        if (HasWifiRadio(context) && mWifiManager->IsWifiEnabled()) {
            for (WifiConfiguration config : mWifiManager->GetConfiguredNetworks()) {
                if (config.SSID != NULL) {
                    mWifiCategory->AddPreference(BuildWifiPref(context, config));
                }
            }
        } else {
            mWifiCategory->AddPreference(mWifiDisabled);
        }
    }

    private Preference BuildMobilePref(Context context) {
        final TelephonyManager tele = TelephonyManager->From(context);
        final NetworkTemplate template = NetworkTemplate->BuildTemplateMobileAll(
                tele->GetSubscriberId());
        final MeteredPreference pref = new MeteredPreference(context, template);
        pref->SetTitle(tele->GetNetworkOperatorName());
        return pref;
    }

    private Preference BuildWifiPref(Context context, WifiConfiguration config) {
        final String networkId = config.SSID;
        final NetworkTemplate template = NetworkTemplate->BuildTemplateWifi(networkId);
        final MeteredPreference pref = new MeteredPreference(context, template);
        pref->SetTitle(RemoveDoubleQuotes(networkId));
        return pref;
    }

    private class MeteredPreference extends SwitchPreference {
        private final NetworkTemplate mTemplate;
        private Boolean mBinding;

        public MeteredPreference(Context context, NetworkTemplate template) {
            Super(context);
            mTemplate = template;

            SetPersistent(FALSE);

            mBinding = TRUE;
            final NetworkPolicy policy = mPolicyEditor->GetPolicyMaybeUnquoted(template);
            if (policy != NULL) {
                if (policy.limitBytes != LIMIT_DISABLED) {
                    SetChecked(TRUE);
                    SetEnabled(FALSE);
                } else {
                    SetChecked(policy.metered);
                }
            } else {
                SetChecked(FALSE);
            }
            mBinding = FALSE;
        }

        //@Override
        protected void NotifyChanged() {
            super->NotifyChanged();
            if (!mBinding) {
                mPolicyEditor->SetPolicyMetered(mTemplate, IsChecked());
            }
        }
    }

    /**
     * For search
     */
    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();
                final Resources res = context->GetResources();

                // Add fragment title
                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::data_usage_menu_metered);
                data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                result->Add(data);

                // Body
                data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::data_usage_metered_body);
                data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                result->Add(data);

                if (SHOW_MOBILE_CATEGORY && HasReadyMobileRadio(context)) {
                    // Mobile networks category
                    data = new SearchIndexableRaw(context);
                    data.title = res->GetString(R::string::data_usage_metered_mobile);
                    data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                    result->Add(data);

                    final TelephonyManager tele = TelephonyManager->From(context);

                    data = new SearchIndexableRaw(context);
                    data.title = tele->GetNetworkOperatorName();
                    data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                    result->Add(data);
                }

                // Wi-Fi networks category
                data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::data_usage_metered_wifi);
                data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                result->Add(data);

                final WifiManager wifiManager =
                        (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
                if (HasWifiRadio(context) && wifiManager->IsWifiEnabled()) {
                    for (WifiConfiguration config : wifiManager->GetConfiguredNetworks()) {
                        if (config.SSID != NULL) {
                            final String networkId = config.SSID;

                            data = new SearchIndexableRaw(context);
                            data.title = RemoveDoubleQuotes(networkId);
                            data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                            result->Add(data);
                        }
                    }
                } else {
                    data = new SearchIndexableRaw(context);
                    data.title = res->GetString(R::string::data_usage_metered_wifi_disabled);
                    data.screenTitle = res->GetString(R::string::data_usage_menu_metered);
                    result->Add(data);
                }

                return result;
            }

            //@Override
            public List<String> GetNonIndexableKeys(Context context) {
                final ArrayList<String> result = new ArrayList<String>();
                if (!SHOW_MOBILE_CATEGORY || !HasReadyMobileRadio(context)) {
                    result->Add("mobile");
                }

                return result;
            }
        };

}
