
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/droid/settings/wifi/AdvancedWifiSettings.h"
#include "elastos/droid/settings/wifi/CAdvancedWifiSettingsWpsFragment.h"
#include "elastos/droid/settings/wifi/WifiSettings.h"
#include "elastos/droid/settings/wifi/CWpsDialog.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Utils;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::CNetworkScorerAppManager;
using Elastos::Droid::Net::INetworkScorerAppManager;
using Elastos::Droid::Net::INetworkScorerAppData;
using Elastos::Droid::KeyStore::Security::ICredentials;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String AdvancedWifiSettings::TAG("AdvancedWifiSettings");
const String AdvancedWifiSettings::KEY_MAC_ADDRESS("mac_address");
const String AdvancedWifiSettings::KEY_CURRENT_IP_ADDRESS("current_ip_address");
const String AdvancedWifiSettings::KEY_FREQUENCY_BAND("frequency_band");
const String AdvancedWifiSettings::KEY_NOTIFY_OPEN_NETWORKS("notify_open_networks");
const String AdvancedWifiSettings::KEY_SLEEP_POLICY("sleep_policy");
const String AdvancedWifiSettings::KEY_SCAN_ALWAYS_AVAILABLE("wifi_scan_always_available");
const String AdvancedWifiSettings::KEY_INSTALL_CREDENTIALS("install_credentials");
const String AdvancedWifiSettings::KEY_WIFI_ASSISTANT("wifi_assistant");
const String AdvancedWifiSettings::KEY_WIFI_DIRECT("wifi_direct");
const String AdvancedWifiSettings::KEY_WPS_PUSH("wps_push_button");
const String AdvancedWifiSettings::KEY_WPS_PIN("wps_pin_entry");

//===============================================================================
//                  AdvancedWifiSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(AdvancedWifiSettings::InnerListener, Object, IPreferenceOnPreferenceChangeListener)

AdvancedWifiSettings::InnerListener::InnerListener(
    /* [in] */ AdvancedWifiSettings* host)
    : mHost(host)
{}

ECode AdvancedWifiSettings::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, newValue, result);
}

//===============================================================================
//                  AdvancedWifiSettings::WpsFragment
//===============================================================================

Int32 AdvancedWifiSettings::WpsFragment::mWpsSetup;

AdvancedWifiSettings::WpsFragment::WpsFragment()
{}

AdvancedWifiSettings::WpsFragment::~WpsFragment()
{}

ECode AdvancedWifiSettings::WpsFragment::constructor()
{
    return DialogFragment::constructor();
}

ECode AdvancedWifiSettings::WpsFragment::constructor(
    /* [in] */ Int32 wpsSetup)
{
    mWpsSetup = wpsSetup;
    return DialogFragment::constructor();
}

ECode AdvancedWifiSettings::WpsFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return CWpsDialog::New(IContext::Probe(activity), mWpsSetup, dialog);
}

//===============================================================================
//                  AdvancedWifiSettings::MyBroadcastReceiver
//===============================================================================

AdvancedWifiSettings::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ AdvancedWifiSettings* host)
    : mHost(host)
{}

AdvancedWifiSettings::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode AdvancedWifiSettings::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION) ||
        action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        mHost->RefreshWifiInfo();
    }
    return NOERROR;
}

//===============================================================================
//                  AdvancedWifiSettings::InitOnPreferenceClickListener
//===============================================================================

CAR_INTERFACE_IMPL(AdvancedWifiSettings::InitOnPreferenceClickListener, Object, IPreferenceOnPreferenceClickListener)

AdvancedWifiSettings::InitOnPreferenceClickListener::InitOnPreferenceClickListener(
    /* [in] */ AdvancedWifiSettings* host,
    /* [in] */ Int32 id,
    /* [in] */ const String& key)
    : mHost(host)
    , mId(id)
    , mKey(key)
{}

AdvancedWifiSettings::InitOnPreferenceClickListener::~InitOnPreferenceClickListener()
{}

ECode AdvancedWifiSettings::InitOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<CAdvancedWifiSettingsWpsFragment> wpsFragment;
    CAdvancedWifiSettingsWpsFragment::NewByFriend(mId, (CAdvancedWifiSettingsWpsFragment**)&wpsFragment);
    AutoPtr<IFragmentManager> manager;
    mHost->GetFragmentManager((IFragmentManager**)&manager);
    wpsFragment->Show(manager, mKey);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  AdvancedWifiSettings
//===============================================================================

AdvancedWifiSettings::AdvancedWifiSettings()
{}

AdvancedWifiSettings::~AdvancedWifiSettings()
{}

ECode AdvancedWifiSettings::constructor()
{
    mReceiver = new MyBroadcastReceiver(this);
    mReceiver->constructor();
    return SettingsPreferenceFragment::constructor();
}

ECode AdvancedWifiSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);
    return AddPreferencesFromResource(R::xml::wifi_advanced_settings);
}

ECode AdvancedWifiSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);
    AutoPtr<IInterface> obj = GetSystemService(IContext::WIFI_SERVICE);
    mWifiManager = IWifiManager::Probe(obj);
    CIntentFilter::New((IIntentFilter**)&mFilter);
    mFilter->AddAction(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION);
    mFilter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    obj = GetSystemService(IContext::NETWORK_SCORE_SERVICE);
    mNetworkScoreManager = INetworkScoreManager::Probe(obj);
    return NOERROR;
}

ECode AdvancedWifiSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    InitPreferences();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(mReceiver, mFilter, (IIntent**)&intent);
    RefreshWifiInfo();
    return NOERROR;
}

ECode AdvancedWifiSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mReceiver);
    return NOERROR;
}

void AdvancedWifiSettings::InitPreferences()
{
    AutoPtr<IPreference> preference;
    FindPreference(CoreUtils::Convert(KEY_NOTIFY_OPEN_NETWORKS), (IPreference**)&preference);
    ISwitchPreference* notifyOpenNetworks = ISwitchPreference::Probe(preference);

    AutoPtr<IContentResolver> resolver = GetContentResolver();

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, 0, &data);
    ITwoStatePreference::Probe(notifyOpenNetworks)->SetChecked(data == 1);
    Boolean enabled;
    mWifiManager->IsWifiEnabled(&enabled);
    preference->SetEnabled(enabled);

    preference = NULL;
    FindPreference(CoreUtils::Convert(KEY_SCAN_ALWAYS_AVAILABLE), (IPreference**)&preference);
    ISwitchPreference* scanAlwaysAvailable = ISwitchPreference::Probe(preference);
    global->GetInt32(resolver,
            ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, 0, &data);
    ITwoStatePreference::Probe(scanAlwaysAvailable)->SetChecked(data == 1);

    AutoPtr<IIntent> intent;
    CIntent::New(ICredentials::INSTALL_AS_USER_ACTION, (IIntent**)&intent);
    intent->SetClassName(String("com.android.certinstaller"),
            String("com.android.certinstaller.CertInstallerMain"));
    intent->PutExtra(ICredentials::EXTRA_INSTALL_AS_UID, IProcess::WIFI_UID);
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(KEY_INSTALL_CREDENTIALS), (IPreference**)&pref);
    pref->SetIntent(intent);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<INetworkScorerAppData> scorer = WifiSettings::GetWifiAssistantApp(context);
    preference = NULL;
    FindPreference(CoreUtils::Convert(KEY_WIFI_ASSISTANT), (IPreference**)&preference);
    ISwitchPreference* wifiAssistant = ISwitchPreference::Probe(preference);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    if (scorer != NULL) {
        AutoPtr<INetworkScorerAppManager> nsam;
        CNetworkScorerAppManager::AcquireSingleton((INetworkScorerAppManager**)&nsam);
        AutoPtr<INetworkScorerAppData> appData;
        nsam->GetActiveScorer(context, (INetworkScorerAppData**)&appData);
        Boolean checked = appData != NULL;
        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        AutoPtr<ICharSequence> cs;
        scorer->GetScorerName((ICharSequence**)&cs);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, cs);
        String str;
        resource->GetString(R::string::wifi_automatically_manage_summary, args, &str);
        preference->SetSummary(CoreUtils::Convert(str));
        preference->SetOnPreferenceChangeListener(listener);
        ITwoStatePreference::Probe(wifiAssistant)->SetChecked(checked);
    }
    else {
        if (wifiAssistant != NULL) {
            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            Boolean res;
            IPreferenceGroup::Probe(screen)->RemovePreference(preference, &res);
        }
    }

    AutoPtr<IIntent> wifiDirectIntent;
    assert(0 && "TODO");
    // CIntent::New(context, ECLSID_CWifiP2pSettingsActivity, (IIntent**)&wifiDirectIntent);
    AutoPtr<IPreference> wifiDirectPref;
    FindPreference(CoreUtils::Convert(KEY_WIFI_DIRECT), (IPreference**)&wifiDirectPref);
    wifiDirectPref->SetIntent(wifiDirectIntent);

    // WpsDialog: Create the dialog like WifiSettings does.
    AutoPtr<IPreference> wpsPushPref;
    FindPreference(CoreUtils::Convert(KEY_WPS_PUSH), (IPreference**)&wpsPushPref);
    AutoPtr<InitOnPreferenceClickListener> listener1 = new InitOnPreferenceClickListener(
            this, IWpsInfo::PBC, KEY_WPS_PUSH);
    wpsPushPref->SetOnPreferenceClickListener(listener1);

    // WpsDialog: Create the dialog like WifiSettings does.
    AutoPtr<IPreference> wpsPinPref;
    FindPreference(CoreUtils::Convert(KEY_WPS_PIN), (IPreference**)&wpsPinPref);
    AutoPtr<InitOnPreferenceClickListener> listener2 = new InitOnPreferenceClickListener(
            this, IWpsInfo::DISPLAY, KEY_WPS_PIN);
    wpsPinPref->SetOnPreferenceClickListener(listener2);

    preference = NULL;
    FindPreference(CoreUtils::Convert(KEY_FREQUENCY_BAND), (IPreference**)&preference);
    IListPreference* frequencyPref = IListPreference::Probe(preference);
    Boolean res;
    mWifiManager->IsDualBandSupported(&res);
    if (res) {
        preference->SetOnPreferenceChangeListener(listener);
        Int32 value;
        mWifiManager->GetFrequencyBand(&value);
        if (value != -1) {
            frequencyPref->SetValue(StringUtils::ToString(value));
            UpdateFrequencyBandSummary(preference, value);
        }
        else {
            Logger::E(TAG, "Failed to fetch frequency band");
        }
    }
    else {
        if (frequencyPref != NULL) {
            // NULL if it has already been removed before resume
            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            Boolean res;
            IPreferenceGroup::Probe(screen)->RemovePreference(preference, &res);
        }
    }

    preference = NULL;
    FindPreference(CoreUtils::Convert(KEY_SLEEP_POLICY), (IPreference**)&preference);
    IListPreference* sleepPolicyPref = IListPreference::Probe(preference);
    if (sleepPolicyPref != NULL) {
        if (Utils::IsWifiOnly(context)) {
            sleepPolicyPref->SetEntries(R::array::wifi_sleep_policy_entries_wifi_only);
        }
        preference->SetOnPreferenceChangeListener(listener);
        Int32 value;
        global->GetInt32(resolver,
                ISettingsGlobal::WIFI_SLEEP_POLICY,
                ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER, &value);
        String stringValue = StringUtils::ToString(value);
        sleepPolicyPref->SetValue(stringValue);
        UpdateSleepPolicySummary(preference, stringValue);
    }
}

void AdvancedWifiSettings::UpdateSleepPolicySummary(
    /* [in] */ IPreference* sleepPolicyPref,
    /* [in] */ const String& value)
{
    if (!value.IsNull()) {
        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        AutoPtr< ArrayOf<String> > values;
        resource->GetStringArray(R::array::wifi_sleep_policy_values, (ArrayOf<String>**)&values);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        Int32 summaryArrayResId = Utils::IsWifiOnly(IContext::Probe(activity)) ?
                R::array::wifi_sleep_policy_entries_wifi_only : R::array::wifi_sleep_policy_entries;

        AutoPtr< ArrayOf<String> > summaries;
        resource->GetStringArray(summaryArrayResId, (ArrayOf<String>**)&summaries);
        for (Int32 i = 0; i < values->GetLength(); i++) {
            if (value.Equals((*values)[i])) {
                if (i < summaries->GetLength()) {
                    sleepPolicyPref->SetSummary(CoreUtils::Convert((*summaries)[i]));
                    return;
                }
            }
        }
    }

    sleepPolicyPref->SetSummary(CoreUtils::Convert(""));
    Logger::E(TAG, "Invalid sleep policy value: %s", value.string());
}

void AdvancedWifiSettings::UpdateFrequencyBandSummary(
    /* [in] */ IPreference* frequencyBandPref,
    /* [in] */ Int32 index)
{
    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);

    AutoPtr< ArrayOf<String> > summaries;
    resource->GetStringArray(R::array::wifi_frequency_band_entries, (ArrayOf<String>**)&summaries);
    frequencyBandPref->SetSummary(CoreUtils::Convert((*summaries)[index]));
}

ECode AdvancedWifiSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* screen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String key;
    preference->GetKey(&key);

    AutoPtr<IContentResolver> resolver = GetContentResolver();

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Boolean res;
    ITwoStatePreference::Probe(preference)->IsChecked(&res);

    if (KEY_NOTIFY_OPEN_NETWORKS.Equals(key)) {
        global->PutInt32(resolver,
                ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, res ? 1 : 0, &res);
    }
    else if (KEY_SCAN_ALWAYS_AVAILABLE.Equals(key)) {
        global->PutInt32(resolver,
                ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, res ? 1 : 0, &res);
    }
    else {
        return SettingsPreferenceFragment::OnPreferenceTreeClick(screen, preference, result);
    }
    *result = TRUE;
    return NOERROR;
}

ECode AdvancedWifiSettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    String key;
    preference->GetKey(&key);

    if (KEY_FREQUENCY_BAND.Equals(key)) {
        // try {
            String str;
            ICharSequence::Probe(newValue)->ToString(&str);
            Int32 value = StringUtils::ParseInt32(str);
            mWifiManager->SetFrequencyBand(value, TRUE);
            UpdateFrequencyBandSummary(preference, value);
        // } catch (NumberFormatException e) {
        //     AutoPtr<IToastHelper> helper;
        //     CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        //     AutoPtr<IToast> toast;
        //     helper->MakeText(context, R::string::wifi_setting_frequency_band_error,
        //             IToast::LENGTH_SHORT, (IToast**)&toast);
        //     toast->Show();
        //     *result = FALSE;
        //     return NOERROR;
        // }
    }
    else if (KEY_WIFI_ASSISTANT.Equals(key)) {
        Boolean res;
        IBoolean::Probe(newValue)->GetValue(&res);
        if (res == FALSE) {
            mNetworkScoreManager->SetActiveScorer(String(NULL), &res);
            *result = TRUE;
            return NOERROR;
        }

        AutoPtr<INetworkScorerAppData> wifiAssistant = WifiSettings::GetWifiAssistantApp(context);
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        String packageName;
        wifiAssistant->GetPackageName(&packageName);
        String configurationActivityClassName;
        wifiAssistant->GetConfigurationActivityClassName(&configurationActivityClassName);
        if (!configurationActivityClassName.IsNull()) {
            // App has a custom configuration activity; launch that.
            // This custom activity will be responsible for launching the system
            // dialog.
            intent->SetClassName(packageName, configurationActivityClassName);
        }
        else {
            // Fall back on the system dialog.
            intent->SetAction(INetworkScoreManager::ACTION_CHANGE_ACTIVE);
            intent->PutExtra(INetworkScoreManager::EXTRA_PACKAGE_NAME, packageName);
        }

        StartActivity(intent);
    }

    if (KEY_SLEEP_POLICY.Equals(key)) {
        // try {
            String stringValue;
            ICharSequence::Probe(newValue)->ToString(&stringValue);
            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            Boolean res;
            global->PutInt32(GetContentResolver(), ISettingsGlobal::WIFI_SLEEP_POLICY,
                    StringUtils::ParseInt32(stringValue), &res);
            UpdateSleepPolicySummary(preference, stringValue);
        // } catch (NumberFormatException e) {
        //     AutoPtr<IToastHelper> helper;
        //     CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        //     AutoPtr<IToast> toast;
        //     helper->MakeText(context, R::string::wifi_setting_sleep_policy_error,
        //             IToast::LENGTH_SHORT, (IToast**)&toast);
        //     toast->Show();
        //     *result = FALSE;
        //     return NOERROR;
        // }
    }

    *result = TRUE;
    return NOERROR;
}

void AdvancedWifiSettings::RefreshWifiInfo()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    AutoPtr<IWifiInfo> wifiInfo;
    mWifiManager->GetConnectionInfo((IWifiInfo**)&wifiInfo);

    AutoPtr<IPreference> wifiMacAddressPref;
    FindPreference(CoreUtils::Convert(KEY_MAC_ADDRESS), (IPreference**)&wifiMacAddressPref);
    String macAddress;
    if (wifiInfo != NULL) {
        wifiInfo->GetMacAddress(&macAddress);
    }
    String summary = macAddress;
    if (TextUtils::IsEmpty(macAddress)) {
        context->GetString(R::string::status_unavailable, &summary);
    }
    wifiMacAddressPref->SetSummary(CoreUtils::Convert(summary));
    wifiMacAddressPref->SetSelectable(FALSE);

    AutoPtr<IPreference> wifiIpAddressPref;
    FindPreference(CoreUtils::Convert(KEY_CURRENT_IP_ADDRESS), (IPreference**)&wifiIpAddressPref);

    String ipAddress = Utils::GetWifiIpAddresses(context);

    summary = ipAddress;
    if (ipAddress.IsNull()) {
        context->GetString(R::string::status_unavailable, &summary);
    }
    wifiIpAddressPref->SetSummary(CoreUtils::Convert(summary));
    wifiIpAddressPref->SetSelectable(FALSE);
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos