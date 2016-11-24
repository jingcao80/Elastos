
#include "Elastos.Droid.Nfc.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/WirelessSettings.h"
#include "elastos/droid/settings/AirplaneModeEnabler.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Settings::Search::EIID_IIndexableSearchIndexProvider;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::CSmsApplication;
using Elastos::Droid::Internal::Telephony::ISmsApplication;
using Elastos::Droid::Internal::Telephony::ISmsApplicationData;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Nfc::INfcManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String WirelessSettings::EXIT_ECM_RESULT("exit_ecm_result");
const Int32 WirelessSettings::REQUEST_CODE_EXIT_ECM = 1;

const String WirelessSettings::TAG("WirelessSettings");

const String WirelessSettings::KEY_TOGGLE_AIRPLANE("toggle_airplane");
const String WirelessSettings::KEY_TOGGLE_NFC("toggle_nfc");
const String WirelessSettings::KEY_WIMAX_SETTINGS("wimax_settings");
const String WirelessSettings::KEY_ANDROID_BEAM_SETTINGS("android_beam_settings");
const String WirelessSettings::KEY_VPN_SETTINGS("vpn_settings");
const String WirelessSettings::KEY_TETHER_SETTINGS("tether_settings");
const String WirelessSettings::KEY_PROXY_SETTINGS("proxy_settings");
const String WirelessSettings::KEY_MOBILE_NETWORK_SETTINGS("mobile_network_settings");
const String WirelessSettings::KEY_MANAGE_MOBILE_PLAN("manage_mobile_plan");
const String WirelessSettings::KEY_SMS_APPLICATION("sms_application");
const String WirelessSettings::KEY_TOGGLE_NSD("toggle_nsd"); //network service discovery
const String WirelessSettings::KEY_CELL_BROADCAST_SETTINGS("cell_broadcast_settings");

const Int32 WirelessSettings::MANAGE_MOBILE_PLAN_DIALOG_ID = 1;
const String WirelessSettings::SAVED_MANAGE_MOBILE_PLAN_MSG("mManageMobilePlanMessage");

AutoPtr<IIndexableSearchIndexProvider> WirelessSettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> WirelessSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new WirelessSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

//===============================================================================
//                  WirelessSettings::MyBaseSearchIndexProvider
//===============================================================================

WirelessSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

WirelessSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode WirelessSettings::MyBaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) /* List<SearchIndexableResource> */
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::wireless_settings);

    AutoPtr< ArrayOf<ISearchIndexableResource*> > args = ArrayOf<ISearchIndexableResource*>::Alloc(1);
    args->Set(0, sir);
    return Arrays::AsList(args, list);
}

ECode WirelessSettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** list) /* List<String>  */
{
    VALIDATE_NOT_NULL(list)

    AutoPtr<IArrayList> result; //ArrayList<String>
    CArrayList::New((IArrayList**)&result);

    result->Add(CoreUtils::Convert(KEY_TOGGLE_NSD));

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean isSecondaryUser = UserHandle::GetMyUserId() != IUserHandle::USER_OWNER;
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    Boolean isWimaxEnabled = !isSecondaryUser &&
            (resources->GetBoolean(Elastos::Droid::R::bool_::config_wimaxEnabled, &res), res);
    if (!isWimaxEnabled
            || (um->HasUserRestriction(IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS, &res), res)) {
        result->Add(CoreUtils::Convert(KEY_WIMAX_SETTINGS));
    }

    if (isSecondaryUser) { // Disable VPN
        result->Add(CoreUtils::Convert(KEY_VPN_SETTINGS));
    }

    // Remove NFC if not available
    obj = NULL;
    context->GetSystemService(IContext::NFC_SERVICE, (IInterface**)&obj);
    INfcManager* manager = INfcManager::Probe(obj);
    if (manager != NULL) {
        AutoPtr<INfcAdapter> adapter;
        manager->GetDefaultAdapter((INfcAdapter**)&adapter);
        if (adapter == NULL) {
            result->Add(CoreUtils::Convert(KEY_TOGGLE_NFC));
            result->Add(CoreUtils::Convert(KEY_ANDROID_BEAM_SETTINGS));
        }
    }

    // Remove Mobile Network Settings and Manage Mobile Plan if it's a wifi-only device.
    if (isSecondaryUser || Utils::IsWifiOnly(context)) {
        result->Add(CoreUtils::Convert(KEY_MOBILE_NETWORK_SETTINGS));
        result->Add(CoreUtils::Convert(KEY_MANAGE_MOBILE_PLAN));
    }

    // Remove Mobile Network Settings and Manage Mobile Plan
    // if config_show_mobile_plan sets FALSE.
    Boolean isMobilePlanEnabled;
    resources->GetBoolean(R::bool_::config_show_mobile_plan, &isMobilePlanEnabled);
    if (!isMobilePlanEnabled) {
        result->Add(CoreUtils::Convert(KEY_MANAGE_MOBILE_PLAN));
    }

    // Remove SMS Application if the device does not support SMS
    obj = NULL;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    ITelephonyManager* tm = ITelephonyManager::Probe(obj);
    if (tm->IsSmsCapable(&res), !res) {
        result->Add(CoreUtils::Convert(KEY_SMS_APPLICATION));
    }

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);

    // Remove Airplane Mode settings if it's a stationary device such as a TV.
    if (pm->HasSystemFeature(IPackageManager::FEATURE_TELEVISION, &res), res) {
        result->Add(CoreUtils::Convert(KEY_TOGGLE_AIRPLANE));
    }

    // proxy UI disabled until we have better app support
    result->Add(CoreUtils::Convert(KEY_PROXY_SETTINGS));

    // Disable Tethering if it's not allowed or if it's a wifi-only device
    obj = NULL;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    IConnectivityManager* cm = IConnectivityManager::Probe(obj);
    if (isSecondaryUser || (cm->IsTetheringSupported(&res), !res)) {
        result->Add(CoreUtils::Convert(KEY_TETHER_SETTINGS));
    }

    // Enable link to CMAS app settings depending on the value in config.xml.
    Boolean isCellBroadcastAppLinkEnabled;
    resources->GetBoolean(
            Elastos::Droid::R::bool_::config_cellBroadcastAppLinks, &isCellBroadcastAppLinkEnabled);
    // try {
        if (isCellBroadcastAppLinkEnabled) {
            Int32 value;
            ECode ec = pm->GetApplicationEnabledSetting(String("com.android.cellbroadcastreceiver"), &value);
            if (SUCCEEDED(ec)) {
                if (value == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED) {
                    isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app disabled
                }
            }
            else {
            // } catch (IllegalArgumentException ignored) {
                isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app not installed
            // }
            }
        }
    if (isSecondaryUser || !isCellBroadcastAppLinkEnabled) {
        result->Add(CoreUtils::Convert(KEY_CELL_BROADCAST_SETTINGS));
    }

    *list = IList::Probe(result);
    REFCOUNT_ADD(*list);
    return NOERROR;
}

//===============================================================================
//                  WirelessSettings
//===============================================================================

CAR_INTERFACE_IMPL_2(WirelessSettings, SettingsPreferenceFragment,
        IPreferenceOnPreferenceChangeListener, IIndexable);

WirelessSettings::WirelessSettings()
{}

WirelessSettings::~WirelessSettings()
{}

ECode WirelessSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode WirelessSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder builder("onPreferenceTreeClick: preference=");
    builder += preference;

    Log(builder.ToString());
    AutoPtr<IPreference> pref;
    if (preference == IPreference::Probe(mAirplaneModePreference)) {
        AutoPtr<ISystemProperties> sys;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sys);
        String str;
        sys->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &str);
        if (StringUtils::ParseBoolean(str)) {
            // In ECM mode launch ECM app dialog
            AutoPtr<IIntent> intent;
            CIntent::New(ITelephonyIntents::ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, NULL, (IIntent**)&intent);
            StartActivityForResult(intent, REQUEST_CODE_EXIT_ECM);
            *result = TRUE;
            return NOERROR;
        }
    }
    else if (preference == (FindPreference(CoreUtils::Convert(KEY_MANAGE_MOBILE_PLAN), (IPreference**)&pref), pref.Get())) {
        OnManageMobilePlanClick();
    }
    //Let the intents be launched by the Preference manager
    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
}

ECode WirelessSettings::OnManageMobilePlanClick()
{
    Log(String("onManageMobilePlanClick:"));

    mManageMobilePlanMessage = NULL;
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IResources> resources;
    IContext::Probe(activity)->GetResources((IResources**)&resources);

    AutoPtr<INetworkInfo> ni;
    mCm->GetProvisioningOrActiveNetworkInfo((INetworkInfo**)&ni);
    Boolean res;
    mTm->HasIccCard(&res);
    if (res && (ni != NULL)) {
        // Check for carrier apps that can handle provisioning first
        AutoPtr<IIntent> provisioningIntent;
        CIntent::New(ITelephonyIntents::ACTION_CARRIER_SETUP, (IIntent**)&provisioningIntent);
        AutoPtr<IList> carrierPackages;
        mTm->GetCarrierPackageNamesForIntent(provisioningIntent, (IList**)&carrierPackages);
        if (carrierPackages != NULL && (carrierPackages->IsEmpty(&res), !res)) {
            Int32 size;
            carrierPackages->GetSize(&size);
            if (size != 1) {
                Logger::W(TAG, "Multiple matching carrier apps found, launching the first.");
            }
            AutoPtr<IInterface> obj;
            carrierPackages->Get(0, (IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            provisioningIntent->SetPackage(str);
            StartActivity(provisioningIntent);
            return NOERROR;
        }

        // Get provisioning URL
        String url;
        mCm->GetMobileProvisioningUrl(&url);
        if (!TextUtils::IsEmpty(url)) {
            AutoPtr<IIntentHelper> intentHelper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&intentHelper);
            AutoPtr<IIntent> intent;
            intentHelper->MakeMainSelectorActivity(IIntent::ACTION_MAIN,
                    IIntent::CATEGORY_APP_BROWSER, (IIntent**)&intent);
            AutoPtr<IUriHelper> uriHelper;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
            AutoPtr<IUri> uri;
            uriHelper->Parse(url, (IUri**)&uri);
            intent->SetData(uri);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT |
                    IIntent::FLAG_ACTIVITY_NEW_TASK);
            // try {
            FAIL_RETURN(StartActivity(intent));
            // } catch (ActivityNotFoundException e) {
            //     Logger::W(TAG, "onManageMobilePlanClick: startActivity failed" + e);
            // }
        }
        else {
            // No provisioning URL
            String operatorName;
            mTm->GetSimOperatorName(&operatorName);
            if (TextUtils::IsEmpty(operatorName)) {
                // Use NetworkOperatorName as second choice in case there is no
                // SPN (Service Provider Name on the SIM). Such as with T-mobile.
                mTm->GetNetworkOperatorName(&operatorName);
                if (TextUtils::IsEmpty(operatorName)) {
                    resources->GetString(
                            R::string::mobile_unknown_sim_operator, &mManageMobilePlanMessage);
                }
                else {
                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                    args->Set(0, CoreUtils::Convert(operatorName));
                    resources->GetString(
                            R::string::mobile_no_provisioning_url, args, &mManageMobilePlanMessage);
                }
            }
            else {
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(operatorName));
                resources->GetString(
                        R::string::mobile_no_provisioning_url, args, &mManageMobilePlanMessage);
            }
        }
    }
    else if ((mTm->HasIccCard(&res), res) == FALSE) {
        // No sim card
        resources->GetString(R::string::mobile_insert_sim_card, &mManageMobilePlanMessage);
    }
    else {
        // NetworkInfo is NULL, there is no connection
        resources->GetString(R::string::mobile_connect_to_internet, &mManageMobilePlanMessage);
    }
    if (!TextUtils::IsEmpty(mManageMobilePlanMessage)) {
        Log(String("onManageMobilePlanClick: message=") + mManageMobilePlanMessage);
        ShowDialog(MANAGE_MOBILE_PLAN_DIALOG_ID);
    }
    return NOERROR;
}

void WirelessSettings::InitSmsApplicationSetting()
{
    Log(String("initSmsApplicationSetting:"));
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<ISmsApplication> sa;
    CSmsApplication::AcquireSingleton((ISmsApplication**)&sa);
    AutoPtr<ICollection> smsApplications; //  Collection<SmsApplicationData>
    sa->GetApplicationCollection(IContext::Probe(activity), (ICollection**)&smsApplications);

    // If the list is empty the dialog will be empty, but we will not crash.
    Int32 count;
    smsApplications->GetSize(&count);
    AutoPtr< ArrayOf<String> > packageNames = ArrayOf<String>::Alloc(count);
    Int32 i = 0;
    AutoPtr<IIterator> iterator;
    smsApplications->GetIterator((IIterator**)&iterator);
    Boolean res;
    while (iterator->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        assert(0 && "TODO");
        // ISmsApplicationData* smsApplicationData = ISmsApplicationData::Probe(obj);
        // (*packageNames)[i] = ((SmsApplicationData*)smsApplicationData)->mPackageName;
        i++;
    }

    String defaultPackageName;
    AutoPtr<IComponentName> appName;
    sa->GetDefaultSmsApplication(IContext::Probe(activity), TRUE, (IComponentName**)&appName);
    if (appName != NULL) {
        appName->GetPackageName(&defaultPackageName);
    }
    mSmsApplicationPreference->SetPackageNames(packageNames, defaultPackageName);
}

ECode WirelessSettings::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);

    Log(String("onCreateDialog: dialogId=") + StringUtils::ToString(dialogId));
    assert(0 && "TODO");

    // switch (dialogId) {
    //     case MANAGE_MOBILE_PLAN_DIALOG_ID:
    //         return new AlertDialog->Builder(GetActivity())
    //                     .SetMessage(mManageMobilePlanMessage)
    //                     .SetCancelable(FALSE)
    //                     .SetPositiveButton(R::string::ok,
    //                             new DialogInterface.OnClickListener() {
    //                         //@Override
    //                         ECode OnClick(DialogInterface dialog, Int32 id) {
    //                             Log("MANAGE_MOBILE_PLAN_DIALOG.onClickListener id=" + id);
    //                             mManageMobilePlanMessage = NULL;
    //                         }
    //                     })
    //                     .Create();
    // }
    return SettingsPreferenceFragment::OnCreateDialog(dialogId, dialog);
}

void WirelessSettings::Log(
    /* [in] */ const String& s)
{
    Logger::D(TAG, s);
}

Boolean WirelessSettings::IsRadioAllowed(
    /* [in] */ IContext* context,
    /* [in] */ const String& type)
{
    if (!AirplaneModeEnabler::IsAirplaneModeOn(context)) {
        return TRUE;
    }
    // Here we use the same logic in OnCreate().
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    String toggleable;
    global->GetString(resolver,
            ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS, &toggleable);
    return !toggleable.IsNull() && toggleable.Contains(type);
}

Boolean WirelessSettings::IsSmsSupported()
{
    // Some tablet has sim card but could not do telephony operations. Skip those.
    Boolean res;
    mTm->IsSmsCapable(&res);
    return res;
}

ECode WirelessSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);
    assert(0 && "TODO");
    // if (savedInstanceState != NULL) {
    //     savedInstanceState->GetString(SAVED_MANAGE_MOBILE_PLAN_MSG, &mManageMobilePlanMessage);
    // }
    // Log(String("onCreate: mManageMobilePlanMessage=") + mManageMobilePlanMessage);

    // mCm = (ConnectivityManager) GetSystemService(IContext::CONNECTIVITY_SERVICE);
    // mTm = (TelephonyManager) GetSystemService(IContext::TELEPHONY_SERVICE);
    // mPm = GetPackageManager();
    // mUm = (UserManager) GetSystemService(IContext::USER_SERVICE);

    // AddPreferencesFromResource(R::xml::wireless_settings);

    // final Boolean isSecondaryUser = UserHandle->MyUserId() != UserHandle.USER_OWNER;

    // final Activity activity = GetActivity();
    // mAirplaneModePreference = (SwitchPreference) FindPreference(KEY_TOGGLE_AIRPLANE);
    // SwitchPreference nfc = (SwitchPreference) FindPreference(KEY_TOGGLE_NFC);
    // PreferenceScreen androidBeam = (PreferenceScreen) FindPreference(KEY_ANDROID_BEAM_SETTINGS);
    // CheckBoxPreference nsd = (CheckBoxPreference) FindPreference(KEY_TOGGLE_NSD);

    // mAirplaneModeEnabler = new AirplaneModeEnabler(activity, mAirplaneModePreference);
    // mNfcEnabler = new NfcEnabler(activity, nfc, androidBeam);

    // mSmsApplicationPreference = (AppListPreference) FindPreference(KEY_SMS_APPLICATION);
    // mSmsApplicationPreference->SetOnPreferenceChangeListener(this);// writer to InnerListener after open
    // InitSmsApplicationSetting();

    // // Remove NSD checkbox by default
    // GetPreferenceScreen()->RemovePreference(nsd);
    // //mNsdEnabler = new NsdEnabler(activity, nsd);

    // String toggleable = Settings::Global::>GetString(activity->GetContentResolver(),
    //         Settings::Global::AIRPLANE_MODE_TOGGLEABLE_RADIOS);

    // //enable/disable wimax depending on the value in config.xml
    // final Boolean isWimaxEnabled = !isSecondaryUser && this->GetResources()->GetBoolean(
    //         R::bool_::config_wimaxEnabled);
    // if (!isWimaxEnabled
    //         || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
    //     PreferenceScreen root = GetPreferenceScreen();
    //     Preference ps = (Preference) FindPreference(KEY_WIMAX_SETTINGS);
    //     if (ps != NULL) root->RemovePreference(ps);
    // } else {
    //     if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_WIMAX )
    //             && isWimaxEnabled) {
    //         Preference ps = (Preference) FindPreference(KEY_WIMAX_SETTINGS);
    //         ps->SetDependency(KEY_TOGGLE_AIRPLANE);
    //     }
    // }

    // // Manually set dependencies for Wifi when not toggleable.
    // if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_WIFI)) {
    //     FindPreference(KEY_VPN_SETTINGS).SetDependency(KEY_TOGGLE_AIRPLANE);
    // }
    // // Disable VPN.
    // if (isSecondaryUser || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_VPN)) {
    //     RemovePreference(KEY_VPN_SETTINGS);
    // }

    // // Manually set dependencies for Bluetooth when not toggleable.
    // if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_BLUETOOTH)) {
    //     // No bluetooth-dependent items in the list. Code kept in case one is added later.
    // }

    // // Manually set dependencies for NFC when not toggleable.
    // if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_NFC)) {
    //     FindPreference(KEY_TOGGLE_NFC).SetDependency(KEY_TOGGLE_AIRPLANE);
    //     FindPreference(KEY_ANDROID_BEAM_SETTINGS).SetDependency(KEY_TOGGLE_AIRPLANE);
    // }

    // // Remove NFC if not available
    // mNfcAdapter = NfcAdapter->GetDefaultAdapter(activity);
    // if (mNfcAdapter == NULL) {
    //     GetPreferenceScreen()->RemovePreference(nfc);
    //     GetPreferenceScreen()->RemovePreference(androidBeam);
    //     mNfcEnabler = NULL;
    // }

    // // Remove Mobile Network Settings and Manage Mobile Plan for secondary users,
    // // if it's a wifi-only device, or if the settings are restricted.
    // if (isSecondaryUser || Utils->IsWifiOnly(GetActivity())
    //         || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
    //     RemovePreference(KEY_MOBILE_NETWORK_SETTINGS);
    //     RemovePreference(KEY_MANAGE_MOBILE_PLAN);
    // }
    // // Remove Mobile Network Settings and Manage Mobile Plan
    // // if config_show_mobile_plan sets FALSE.
    // final Boolean isMobilePlanEnabled = this->GetResources()->GetBoolean(
    //         R::bool_::config_show_mobile_plan);
    // if (!isMobilePlanEnabled) {
    //     Preference pref = FindPreference(KEY_MANAGE_MOBILE_PLAN);
    //     if (pref != NULL) {
    //         RemovePreference(KEY_MANAGE_MOBILE_PLAN);
    //     }
    // }

    // // Remove SMS Application if the device does not support SMS
    // if (!IsSmsSupported()) {
    //     RemovePreference(KEY_SMS_APPLICATION);
    // }

    // // Remove Airplane Mode settings if it's a stationary device such as a TV.
    // if (mPm->HasSystemFeature(PackageManager.FEATURE_TELEVISION)) {
    //     RemovePreference(KEY_TOGGLE_AIRPLANE);
    // }

    // // Enable Proxy selector settings if allowed.
    // Preference mGlobalProxy = FindPreference(KEY_PROXY_SETTINGS);
    // final DevicePolicyManager mDPM = (DevicePolicyManager)
    //         activity->GetSystemService(IContext::DEVICE_POLICY_SERVICE);
    // // proxy UI disabled until we have better app support
    // GetPreferenceScreen()->RemovePreference(mGlobalProxy);
    // mGlobalProxy->SetEnabled(mDPM->GetGlobalProxyAdmin() == NULL);

    // // Disable Tethering if it's not allowed or if it's a wifi-only device
    // final ConnectivityManager cm =
    //         (ConnectivityManager) activity->GetSystemService(IContext::CONNECTIVITY_SERVICE);
    // if (isSecondaryUser || !cm->IsTetheringSupported()
    //         || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_TETHERING)) {
    //     GetPreferenceScreen()->RemovePreference(FindPreference(KEY_TETHER_SETTINGS));
    // } else {
    //     Preference p = FindPreference(KEY_TETHER_SETTINGS);
    //     p->SetTitle(Utils->GetTetheringLabel(cm));

    //     // Grey out if provisioning is not available.
    //     p->SetEnabled(!TetherSettings
    //             .IsProvisioningNeededButUnavailable(GetActivity()));
    // }

    // // Enable link to CMAS app settings depending on the value in config.xml.
    // Boolean isCellBroadcastAppLinkEnabled = this->GetResources()->GetBoolean(
    //         R::bool_::config_cellBroadcastAppLinks);
    // try {
    //     if (isCellBroadcastAppLinkEnabled) {
    //         if (mPm->GetApplicationEnabledSetting("com.android.cellbroadcastreceiver")
    //                 == PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
    //             isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app disabled
    //         }
    //     }
    // } catch (IllegalArgumentException ignored) {
    //     isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app not installed
    // }
    // if (isSecondaryUser || !isCellBroadcastAppLinkEnabled
    //         || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_CELL_BROADCASTS)) {
    //     PreferenceScreen root = GetPreferenceScreen();
    //     Preference ps = FindPreference(KEY_CELL_BROADCAST_SETTINGS);
    //     if (ps != NULL) root->RemovePreference(ps);
    // }
    return NOERROR;
}

ECode WirelessSettings::OnStart()
{
    SettingsPreferenceFragment::OnStart();

    InitSmsApplicationSetting();
    return NOERROR;
}

ECode WirelessSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    mAirplaneModeEnabler->Resume();
    assert(0 && "TODO");
    // if (mNfcEnabler != NULL) {
    //     mNfcEnabler->Resume();
    // }
    // if (mNsdEnabler != NULL) {
    //     mNsdEnabler->Resume();
    // }
    return NOERROR;
}

ECode WirelessSettings::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    SettingsPreferenceFragment::OnSaveInstanceState(outState);

    assert(0 && "TODO");
    // if (!TextUtils::IsEmpty(mManageMobilePlanMessage)) {
    //     outState->PutString(SAVED_MANAGE_MOBILE_PLAN_MSG, mManageMobilePlanMessage);
    // }
    return NOERROR;
}

ECode WirelessSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();

    mAirplaneModeEnabler->Pause();
    assert(0 && "TODO");
    // if (mNfcEnabler != NULL) {
    //     mNfcEnabler->Pause();
    // }
    // if (mNsdEnabler != NULL) {
    //     mNsdEnabler->Pause();
    // }
    return NOERROR;
}

ECode WirelessSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    assert(0 && "TODO");
    // if (requestCode == REQUEST_CODE_EXIT_ECM) {
    //     Boolean isChoiceYes = data->GetBooleanExtra(EXIT_ECM_RESULT, FALSE);
    //     // Set Airplane mode based on the return value and checkbox state
    //     mAirplaneModeEnabler->SetAirplaneModeInECM(isChoiceYes,
    //             mAirplaneModePreference->IsChecked());
    // }
    return SettingsPreferenceFragment::OnActivityResult(requestCode, resultCode, data);
}

Int32 WirelessSettings::GetHelpResource()
{
    return R::string::help_url_more_networks;
}

ECode WirelessSettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // if (preference == mSmsApplicationPreference && newValue != NULL) {
    //     SmsApplication->SetDefaultApplication(newValue->ToString(), GetActivity());
    //     *result = TRUE;
    //     return NOERROR;
    // }
    // *result = FALSE;
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
