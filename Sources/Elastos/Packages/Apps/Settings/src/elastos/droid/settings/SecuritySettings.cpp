
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/settings/SecuritySettings.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/TrustAgentUtils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::Index;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::TrustAgentUtils;
using Elastos::Droid::Settings::Search::EIID_IIndexable;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
// using Elastos::Droid::KeyStore::Security::CKeyStoreHelper;
using Elastos::Droid::KeyStore::Security::IKeyStoreHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IBoolean;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

AutoPtr<IIndexableSearchIndexProvider> SecuritySettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> SecuritySettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new SecuritySettings::SecuritySearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

const String SecuritySettings::TAG("SecuritySettings");

const String SecuritySettings::TRUST_AGENT_CLICK_INTENT("trust_agent_click_intent");

static const AutoPtr<IIntent> InitTRUST_AGENT_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITrustAgentService::SERVICE_INTERFACE, (IIntent**)&intent);
    return intent;
}
const AutoPtr<IIntent> SecuritySettings::TRUST_AGENT_INTENT = InitTRUST_AGENT_INTENT();

const String SecuritySettings::KEY_UNLOCK_SET_OR_CHANGE("unlock_set_or_change");
const String SecuritySettings::KEY_BIOMETRIC_WEAK_IMPROVE_MATCHING("biometric_weak_improve_matching");
const String SecuritySettings::KEY_BIOMETRIC_WEAK_LIVELINESS("biometric_weak_liveliness");
const String SecuritySettings::KEY_LOCK_ENABLED("lockenabled");
const String SecuritySettings::KEY_VISIBLE_PATTERN("visiblepattern");
const String SecuritySettings::KEY_SECURITY_CATEGORY("security_category");
const String SecuritySettings::KEY_DEVICE_ADMIN_CATEGORY("device_admin_category");
const String SecuritySettings::KEY_LOCK_AFTER_TIMEOUT("lock_after_timeout");
const String SecuritySettings::KEY_OWNER_INFO_SETTINGS("owner_info_settings");
const String SecuritySettings::KEY_ADVANCED_SECURITY("advanced_security");
const String SecuritySettings::KEY_MANAGE_TRUST_AGENTS("manage_trust_agents");

const Int32 SecuritySettings::SET_OR_CHANGE_LOCK_METHOD_REQUEST = 123;
const Int32 SecuritySettings::CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST = 124;
const Int32 SecuritySettings::CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF = 125;
const Int32 SecuritySettings::CHANGE_TRUST_AGENT_SETTINGS = 126;

const String SecuritySettings::KEY_SIM_LOCK("sim_lock");
const String SecuritySettings::KEY_SHOW_PASSWORD("show_password");
const String SecuritySettings::KEY_CREDENTIAL_STORAGE_TYPE("credential_storage_type");
const String SecuritySettings::KEY_RESET_CREDENTIALS("credentials_reset");
const String SecuritySettings::KEY_CREDENTIALS_INSTALL("credentials_install");
const String SecuritySettings::KEY_TOGGLE_INSTALL_APPLICATIONS("toggle_install_applications");
const String SecuritySettings::KEY_POWER_INSTANTLY_LOCKS("power_button_instantly_locks");
const String SecuritySettings::KEY_CREDENTIALS_MANAGER("credentials_management");
const String SecuritySettings::PACKAGE_MIME_TYPE("application/vnd.android.package-archive");
const String SecuritySettings::KEY_TRUST_AGENT("trust_agent");
const String SecuritySettings::KEY_SCREEN_PINNING("screen_pinning_settings");

const AutoPtr<ArrayOf<String> > SecuritySettings::InitSWITCH_PREFERENCE_KEYS()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(7);
    (*args)[0] = KEY_LOCK_AFTER_TIMEOUT;
    (*args)[1] = KEY_LOCK_ENABLED;
    (*args)[2] = KEY_VISIBLE_PATTERN;
    (*args)[3] = KEY_BIOMETRIC_WEAK_LIVELINESS;
    (*args)[4] = KEY_POWER_INSTANTLY_LOCKS;
    (*args)[5] = KEY_SHOW_PASSWORD;
    (*args)[6] = KEY_TOGGLE_INSTALL_APPLICATIONS;
    return args;
}
const AutoPtr<ArrayOf<String> > SecuritySettings::SWITCH_PREFERENCE_KEYS = InitSWITCH_PREFERENCE_KEYS();

const Boolean SecuritySettings::ONLY_ONE_TRUST_AGENT = TRUE;

//===============================================================================
//                  SecuritySettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(SecuritySettings::InnerListener, Object, IPreferenceOnPreferenceChangeListener)

SecuritySettings::InnerListener::InnerListener(
    /* [in] */ SecuritySettings* host)
    : mHost(host)
{}

ECode SecuritySettings::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* res)
{
    return mHost->OnPreferenceChange(preference, value, res);
}

//===============================================================================
//                  SecuritySettings::SecuritySearchIndexProvider
//===============================================================================

SecuritySettings::SecuritySearchIndexProvider::SecuritySearchIndexProvider()
    : mIsPrimary(FALSE)
{
    BaseSearchIndexProvider();

    mIsPrimary = UserHandle::GetMyUserId() == IUserHandle::USER_OWNER;
}

ECode SecuritySettings::SecuritySearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) // List<SearchIndexableResource>
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IList> result; //List<SearchIndexableResource>
    CArrayList::New((IList**)&result);

    AutoPtr<ILockPatternUtils> lockPatternUtils;
    CLockPatternUtils::New(context, (ILockPatternUtils**)&lockPatternUtils);
    // Add options for lock/unlock screen
    Int32 resId = GetResIdForLockUnlockScreen(context, lockPatternUtils);

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(resId);
    result->Add(sir);

    if (mIsPrimary) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
        IDevicePolicyManager* dpm = IDevicePolicyManager::Probe(obj);

        Int32 status;
        dpm->GetStorageEncryptionStatus(&status);
        switch (status) {
            case IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVE:
                // The device is currently encrypted.
                resId = R::xml::security_settings_encrypted;
                break;
            case IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE:
                // This device supports encryption but isn't encrypted.
                resId = R::xml::security_settings_unencrypted;
                break;
        }

        sir = NULL;
        CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
        sir->SetXmlResId(resId);
        result->Add(sir);
    }

    // Append the rest of the settings
    sir = NULL;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::security_settings_misc);
    result->Add(sir);

    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode SecuritySettings::SecuritySearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list) //List<SearchIndexableRaw>
{
    VALIDATE_NOT_NULL(list)

    AutoPtr<IList> result; // List<SearchIndexableRaw>
    CArrayList::New((IList**)&result);
    AutoPtr<IResources> resource;
    context->GetResources((IResources**)&resource);

    String screenTitle;
    resource->GetString(R::string::security_settings_title, &screenTitle);

    AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw();
    data->constructor(context);
    data->mTitle = screenTitle;
    data->mScreenTitle = screenTitle;
    result->Add((ISearchIndexableRaw*)data);

    Boolean res;
    if (!mIsPrimary) {
        Int32 resId = R::string::user_info_settings_title;
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> userManager;
        helper->Get(context, (IUserManager**)&userManager);
        userManager->IsLinkedUser(&res);
        if (res) {
            resId = R::string::profile_info_settings_title;
        }

        data = NULL;
        data = new SearchIndexableRaw();
        data->constructor(context);
        resource->GetString(resId, &data->mTitle);
        data->mScreenTitle = screenTitle;
        result->Add((ISearchIndexableRaw*)data);
    }

    // Credential storage
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);

    Logger::I(TAG, " >> TODO wait for CKeyStoreHelper");
    // if (um->HasUserRestriction(IUserManager::DISALLOW_CONFIG_CREDENTIALS, &res), !res) {
    //     assert(0 && "TODO");
    //     AutoPtr<IKeyStoreHelper> helper;
    //     // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    //     AutoPtr<IKeyStore> keyStore;
    //     // helper->GetInstance((IKeyStore**)&keyStore);

    //     Int32 storageSummaryRes = R::string::credential_storage_type_software;
    //     if (keyStore->IsHardwareBacked(&res), res) {
    //         storageSummaryRes = R::string::credential_storage_type_hardware;
    //     }

    //     data = NULL;
    //     data = new SearchIndexableRaw();
    //     data->constructor(context);
    //     resource->GetString(storageSummaryRes, &data->mTitle);
    //     data->mScreenTitle = screenTitle;
    //     result->Add((ISearchIndexableRaw*)data);
    // }

    // Advanced
    AutoPtr<ILockPatternUtils> lockPatternUtils;
    CLockPatternUtils::New(context, (ILockPatternUtils**)&lockPatternUtils);
    if (lockPatternUtils->IsSecure(&res), res) {
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IArrayList> agents = GetActiveTrustAgents(pm, lockPatternUtils); // ArrayList<TrustAgentComponentInfo>
        Int32 size;
        agents->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            agents->Get(i, (IInterface**)&obj);
            TrustAgentUtils::TrustAgentComponentInfo* agent = (TrustAgentUtils::TrustAgentComponentInfo*) IObject::Probe(obj);
            data = NULL;
            data = new SearchIndexableRaw();
            data->constructor(context);
            data->mTitle = agent->mTitle;
            data->mScreenTitle = screenTitle;
            result->Add((ISearchIndexableRaw*)data);
        }
    }
    *list = result;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode SecuritySettings::SecuritySearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** list) //List<String>
{
    VALIDATE_NOT_NULL(list)

    AutoPtr<IList> keys; // List<String>
    CArrayList::New((IList**)&keys);

    AutoPtr<ILockPatternUtils> lockPatternUtils;
    CLockPatternUtils::New(context, (ILockPatternUtils**)&lockPatternUtils);
    // Add options for lock/unlock screen
    Int32 resId = GetResIdForLockUnlockScreen(context, lockPatternUtils);

    // don't display visible pattern if biometric and backup is not pattern
    Int32 quality;
    if (resId == R::xml::security_settings_biometric_weak &&
            (lockPatternUtils->GetKeyguardStoredPasswordQuality(&quality), quality) !=
                    IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING) {
        keys->Add(CoreUtils::Convert(KEY_VISIBLE_PATTERN));
    }

    // Do not display SIM lock for devices without an Icc card
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> tm;
    helper->GetDefault((ITelephonyManager**)&tm);
    Boolean res;
    if (!mIsPrimary || (tm->HasIccCard(&res), !res)) {
        keys->Add(CoreUtils::Convert(KEY_SIM_LOCK));
    }

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    if (um->HasUserRestriction(IUserManager::DISALLOW_CONFIG_CREDENTIALS, &res), res) {
        keys->Add(CoreUtils::Convert(KEY_CREDENTIALS_MANAGER));
    }

    // TrustAgent settings disappear when the user has no primary security.
    if (lockPatternUtils->IsSecure(&res), !res) {
        keys->Add(CoreUtils::Convert(KEY_TRUST_AGENT));
        keys->Add(CoreUtils::Convert(KEY_MANAGE_TRUST_AGENTS));
    }

    *list = keys;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

//===============================================================================
//                 SecuritySettings
//===============================================================================

CAR_INTERFACE_IMPL_2(SecuritySettings, SettingsPreferenceFragment, IDialogInterfaceOnClickListener, IIndexable);

SecuritySettings::SecuritySettings()
    : mIsPrimary(FALSE)
{}

SecuritySettings::~SecuritySettings()
{}

ECode SecuritySettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode SecuritySettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);

    mDPM = IDevicePolicyManager::Probe(GetSystemService(IContext::DEVICE_POLICY_SERVICE));

    mChooseLockSettingsHelper = new ChooseLockSettingsHelper();
    mChooseLockSettingsHelper->constructor(activity);

    Boolean result;
    if (savedInstanceState != NULL
            && (savedInstanceState->ContainsKey(TRUST_AGENT_CLICK_INTENT, &result), result)) {
        AutoPtr<IParcelable> parcelable;
        savedInstanceState->GetParcelable(TRUST_AGENT_CLICK_INTENT, (IParcelable**)&parcelable);
        mTrustAgentClickIntent = IIntent::Probe(parcelable);
    }
    return NOERROR;
}

Int32 SecuritySettings::GetResIdForLockUnlockScreen(
    /* [in] */ IContext* context,
    /* [in] */ ILockPatternUtils* lockPatternUtils)
{
    Int32 resid = 0;
    Boolean res, res1, res2;
    Int32 quality;
    if (lockPatternUtils->IsSecure(&res), !res) {
        // if there are multiple users, disable "None" setting
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
        IUserManager* mUm = IUserManager::Probe(obj);
        AutoPtr<IList> users; // List<UserInfo>
        mUm->GetUsers(TRUE, (IList**)&users);
        Int32 size;
        users->GetSize(&size);
        const Boolean singleUser = size == 1;

        if (singleUser && (lockPatternUtils->IsLockScreenDisabled(&res), res)) {
            resid = R::xml::security_settings_lockscreen;
        }
        else {
            resid = R::xml::security_settings_chooser;
        }
    }
    else if ((lockPatternUtils->UsingBiometricWeak(&res1), res1) &&
            (lockPatternUtils->IsBiometricWeakInstalled(&res2), res2)) {
        resid = R::xml::security_settings_biometric_weak;
    }
    else {
        switch (lockPatternUtils->GetKeyguardStoredPasswordQuality(&quality), quality) {
            case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
                resid = R::xml::security_settings_pattern;
                break;
            case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
                resid = R::xml::security_settings_pin;
                break;
            case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
                resid = R::xml::security_settings_password;
                break;
        }
    }
    return resid;
}

AutoPtr<IPreferenceScreen> SecuritySettings::CreatePreferenceHierarchy()
{
    AutoPtr<IPreferenceScreen> root;
    GetPreferenceScreen((IPreferenceScreen**)&root);
    if (root != NULL) {
        IPreferenceGroup::Probe(root)->RemoveAll();
    }
    AddPreferencesFromResource(R::xml::security_settings);
    root = NULL;
    GetPreferenceScreen((IPreferenceScreen**)&root);

    // Add options for lock/unlock screen
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    const Int32 resid = GetResIdForLockUnlockScreen(IContext::Probe(activity), mLockPatternUtils);
    AddPreferencesFromResource(resid);

    // Add options for device encryption
    mIsPrimary = UserHandle::GetMyUserId() == IUserHandle::USER_OWNER;

    Boolean res;
    if (!mIsPrimary) {
        // Rename owner info settings
        AutoPtr<IPreference> ownerInfoPref;
        FindPreference(CoreUtils::Convert(KEY_OWNER_INFO_SETTINGS), (IPreference**)&ownerInfoPref);
        if (ownerInfoPref != NULL) {
            AutoPtr<IUserManagerHelper> helper;
            CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
            AutoPtr<IUserManager> um;
            helper->Get(IContext::Probe(activity), (IUserManager**)&um);
            um->IsLinkedUser(&res);
            if (res) {
                ownerInfoPref->SetTitle(R::string::profile_info_settings_title);
            }
            else {
                ownerInfoPref->SetTitle(R::string::user_info_settings_title);
            }
        }
    }

    if (mIsPrimary) {
        AutoPtr<ILockPatternUtilsHelper> helper;
        CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
        if (helper->IsDeviceEncryptionEnabled(&res), res) {
            // The device is currently encrypted.
            AddPreferencesFromResource(R::xml::security_settings_encrypted);
        }
        else {
            // This device supports encryption but isn't encrypted.
            AddPreferencesFromResource(R::xml::security_settings_unencrypted);
        }
    }

    // Trust Agent preferences
    AutoPtr<IPreference> pref;
    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);
    _root->FindPreference(CoreUtils::Convert(KEY_SECURITY_CATEGORY), (IPreference**)&pref);
    AutoPtr<IPreferenceGroup> securityCategory = IPreferenceGroup::Probe(pref);
    if (securityCategory != NULL) {
        Boolean hasSecurity;
        mLockPatternUtils->IsSecure(&hasSecurity);
        AutoPtr<IArrayList> agents = GetActiveTrustAgents(GetPackageManager(), mLockPatternUtils); // ArrayList<TrustAgentComponentInfo>
        Int32 size;
        agents->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            agents->Get(i, (IInterface**)&obj);
            TrustAgentUtils::TrustAgentComponentInfo* agent = (TrustAgentUtils::TrustAgentComponentInfo*)IObject::Probe(obj);
            AutoPtr<IContext> context;
            IPreference::Probe(securityCategory)->GetContext((IContext**)&context);
            AutoPtr<IPreference> trustAgentPreference;
            CPreference::New(context, (IPreference**)&trustAgentPreference);
            trustAgentPreference->SetKey(KEY_TRUST_AGENT);
            trustAgentPreference->SetTitle(CoreUtils::Convert(agent->mTitle));
            trustAgentPreference->SetSummary(CoreUtils::Convert(agent->mSummary));
            // Create intent for this preference.

            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetComponent(agent->mComponentName);
            intent->SetAction(IIntent::ACTION_MAIN);
            trustAgentPreference->SetIntent(intent);
            // Add preference to the settings menu.
            securityCategory->AddPreference(trustAgentPreference, &res);
            if (!hasSecurity) {
                trustAgentPreference->SetEnabled(FALSE);
                trustAgentPreference->SetSummary(R::string::disabled_because_no_backup_security);
            }
        }
    }

    // lock after preference
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_LOCK_AFTER_TIMEOUT), (IPreference**)&pref);
    mLockAfter = IListPreference::Probe(pref);
    if (mLockAfter != NULL) {
        SetupLockAfterPreference();
        UpdateLockAfterPreferenceSummary();
    }

    // biometric weak liveliness
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_BIOMETRIC_WEAK_LIVELINESS), (IPreference**)&pref);
    mBiometricWeakLiveliness = ISwitchPreference::Probe(pref);

    // visible pattern
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_VISIBLE_PATTERN), (IPreference**)&pref);
    mVisiblePattern = ISwitchPreference::Probe(pref);

    // lock instantly on power key press
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_POWER_INSTANTLY_LOCKS), (IPreference**)&pref);
    mPowerButtonInstantlyLocks = ISwitchPreference::Probe(pref);
    AutoPtr<IPreference> trustAgentPreference;
    _root->FindPreference(CoreUtils::Convert(KEY_TRUST_AGENT), (IPreference**)&trustAgentPreference);
    if (mPowerButtonInstantlyLocks != NULL && trustAgentPreference != NULL) {
        AutoPtr<ICharSequence> cs;
        trustAgentPreference->GetTitle((ICharSequence**)&cs);
        Int32 length;
        cs->GetLength(&length);
        if (length > 0) {
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, cs);
            String str;
            GetString(R::string::lockpattern_settings_power_button_instantly_locks_summary,
                    args, &str);
            IPreference::Probe(mPowerButtonInstantlyLocks)->SetSummary(CoreUtils::Convert(str));
        }
    }

    // don't display visible pattern if biometric and backup is not pattern
    Int32 quality;
    if (resid == R::xml::security_settings_biometric_weak &&
            (mLockPatternUtils->GetKeyguardStoredPasswordQuality(&quality), quality) !=
            IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING) {
        if (securityCategory != NULL && mVisiblePattern != NULL) {
            pref = NULL;
            _root->FindPreference(CoreUtils::Convert(KEY_VISIBLE_PATTERN), (IPreference**)&pref);
            securityCategory->RemovePreference(pref, &res);
        }
    }

    // Append the rest of the settings
    AddPreferencesFromResource(R::xml::security_settings_misc);

    // Do not display SIM lock for devices without an Icc card
    AutoPtr<ITelephonyManagerHelper> tmHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmHelper);
    AutoPtr<ITelephonyManager> tm;
    tmHelper->GetDefault((ITelephonyManager**)&tm);
    AutoPtr<ISettingsSystem> settingSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingSystem);
    if (!mIsPrimary || (tm->HasIccCard(&res), !res)) {
        pref = NULL;
        _root->FindPreference(CoreUtils::Convert(KEY_SIM_LOCK), (IPreference**)&pref);
        _root->RemovePreference(pref, &res);
    }
    else {
        // Disable SIM lock if sim card is missing or unknown
        Int32 simState;
        tm->GetSimState(&simState);
        if ((simState == ITelephonyManager::SIM_STATE_ABSENT) ||
                (simState == ITelephonyManager::SIM_STATE_UNKNOWN)) {
            pref = NULL;
            _root->FindPreference(CoreUtils::Convert(KEY_SIM_LOCK), (IPreference**)&pref);
            pref->SetEnabled(FALSE);
        }
    }
    Int32 data;
    if ((settingSystem->GetInt32(GetContentResolver(),
            ISettingsSystem::LOCK_TO_APP_ENABLED, 0, &data), data) != 0) {
        pref = NULL;
        _root->FindPreference(CoreUtils::Convert(KEY_SCREEN_PINNING), (IPreference**)&pref);
        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        String str;
        resource->GetString(R::string::switch_on_text, &str);
        pref->SetSummary(CoreUtils::Convert(str));
    }

    // Show password
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_SHOW_PASSWORD), (IPreference**)&pref);
    mShowPassword = ISwitchPreference::Probe(pref);

    Logger::I(TAG, " >> TODO wait for CKeyStoreHelper");
    // _root->FindPreference(CoreUtils::Convert(KEY_RESET_CREDENTIALS), (IPreference**)&mResetCredentials);

    // Credential storage
    AutoPtr<IInterface> tmp;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&tmp);
    IUserManager* um = IUserManager::Probe(tmp);

    // AutoPtr<IKeyStoreHelper> helper;
    // assert(0 && "TODO");
    // // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    // helper->GetInstance((IKeyStore**)&mKeyStore); // needs to be initialized for OnResume()
    // if (um->HasUserRestriction(IUserManager::DISALLOW_CONFIG_CREDENTIALS, &res), !res) {
    //     AutoPtr<IPreference> credentialStorageType;
    //     _root->FindPreference(CoreUtils::Convert(KEY_CREDENTIAL_STORAGE_TYPE), (IPreference**)&credentialStorageType);

    //     Int32 storageSummaryRes = R::string::credential_storage_type_software;
    //     if (mKeyStore->IsHardwareBacked(&res), res) {
    //         storageSummaryRes = R::string::credential_storage_type_hardware;
    //     }
    //     credentialStorageType->SetSummary(storageSummaryRes);
    // }
    // else {
    //     pref = NULL;
    //     _root->FindPreference(CoreUtils::Convert(KEY_CREDENTIALS_MANAGER), (IPreference**)&pref);
    //     AutoPtr<IPreferenceGroup> credentialsManager = IPreferenceGroup::Probe(pref);
    //     pref = NULL;
    //     _root->FindPreference(CoreUtils::Convert(KEY_RESET_CREDENTIALS), (IPreference**)&pref);
    //     credentialsManager->RemovePreference(pref, &res);
    //     pref = NULL;
    //     _root->FindPreference(CoreUtils::Convert(KEY_CREDENTIALS_INSTALL), (IPreference**)&pref);
    //     credentialsManager->RemovePreference(pref, &res);
    //     pref = NULL;
    //     _root->FindPreference(CoreUtils::Convert(KEY_CREDENTIAL_STORAGE_TYPE), (IPreference**)&pref);
    //     credentialsManager->RemovePreference(pref, &res);
    // }

    // Application install
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_DEVICE_ADMIN_CATEGORY), (IPreference**)&pref);
    AutoPtr<IPreferenceGroup> deviceAdminCategory = IPreferenceGroup::Probe(pref);
    pref = NULL;
    FindPreference(CoreUtils::Convert(KEY_TOGGLE_INSTALL_APPLICATIONS), (IPreference**)&pref);
    mToggleAppInstallation = ISwitchPreference::Probe(pref);
    ITwoStatePreference::Probe(mToggleAppInstallation)->SetChecked(IsNonMarketAppsAllowed());
    // Side loading of apps.
    IPreference::Probe(mToggleAppInstallation)->SetEnabled(mIsPrimary);
    Boolean res1;
    if ((um->HasUserRestriction(IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES, &res), res)
            || (um->HasUserRestriction(IUserManager::DISALLOW_INSTALL_APPS, &res1), res1)) {
        IPreference::Probe(mToggleAppInstallation)->SetEnabled(FALSE);
    }

    // Advanced Security features
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_ADVANCED_SECURITY), (IPreference**)&pref);
    AutoPtr<IPreferenceGroup> advancedCategory = IPreferenceGroup::Probe(pref);
    if (advancedCategory != NULL) {
        AutoPtr<IPreference> manageAgents;
        advancedCategory->FindPreference(CoreUtils::Convert(KEY_MANAGE_TRUST_AGENTS), (IPreference**)&manageAgents);
        if (manageAgents != NULL && (mLockPatternUtils->IsSecure(&res), !res)) {
            manageAgents->SetEnabled(FALSE);
            manageAgents->SetSummary(R::string::disabled_because_no_backup_security);
        }
    }

    // The above preferences come and go based on security state, so we need to update
    // the index. This call is expected to be fairly cheap, but we may want to do something
    // smarter in the future.
    Index::GetInstance(IContext::Probe(activity))->UpdateFromClassNameResource(
            String("Elastos.Droid.Settings.CSecuritySettings"), TRUE, TRUE);

    for (Int32 i = 0; i < SWITCH_PREFERENCE_KEYS->GetLength(); i++) {
        AutoPtr<IPreference> pref;
        FindPreference(CoreUtils::Convert((*SWITCH_PREFERENCE_KEYS)[i]), (IPreference**)&pref);
        AutoPtr<InnerListener> listener = new InnerListener(this);
        if (pref != NULL) pref->SetOnPreferenceChangeListener(listener);
    }
    return root;
}

/* ArrayList<TrustAgentComponentInfo> */
AutoPtr<IArrayList> SecuritySettings::GetActiveTrustAgents(
    /* [in] */ IPackageManager* pm,
    /* [in] */ ILockPatternUtils* utils)
{
    AutoPtr<IArrayList> result; // ArrayList<TrustAgentComponentInfo>
    CArrayList::New((IArrayList**)&result);
    AutoPtr<IList> resolveInfos; // List<ResolveInfo>
    pm->QueryIntentServices(TRUST_AGENT_INTENT,
            IPackageManager::GET_META_DATA, (IList**)&resolveInfos);
    AutoPtr<IList> enabledTrustAgents; // List<ComponentName>
    utils->GetEnabledTrustAgents((IList**)&enabledTrustAgents);
    Boolean res;
    if (enabledTrustAgents != NULL && (enabledTrustAgents->IsEmpty(&res), !res)) {
        Int32 size;
        resolveInfos->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            resolveInfos->Get(i, (IInterface**)&obj);
            IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
            AutoPtr<IServiceInfo> serviceInfo;
            resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
            if (serviceInfo == NULL) continue;
            if (!TrustAgentUtils::CheckProvidePermission(resolveInfo, pm)) continue;
            AutoPtr<TrustAgentUtils::TrustAgentComponentInfo> trustAgentComponentInfo =
                    TrustAgentUtils::GetSettingsComponent(pm, resolveInfo);
            if (trustAgentComponentInfo->mComponentName == NULL ||
                    (enabledTrustAgents->Contains(
                            TrustAgentUtils::GetComponentName(resolveInfo), &res), !res) ||
                    TextUtils::IsEmpty(trustAgentComponentInfo->mTitle)) continue;
            result->Add((IObject*)trustAgentComponentInfo);
            if (ONLY_ONE_TRUST_AGENT) break;
        }
    }
    return result;
}

Boolean SecuritySettings::IsNonMarketAppsAllowed()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(GetContentResolver(),
            ISettingsGlobal::INSTALL_NON_MARKET_APPS, 0, &data);
    return data > 0;
}

void SecuritySettings::SetNonMarketAppsAllowed(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    Boolean res;
    if (um->HasUserRestriction(IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES, &res), res) {
        return;
    }
    // Change the system setting
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->PutInt32(GetContentResolver(), ISettingsGlobal::INSTALL_NON_MARKET_APPS,
                            enabled ? 1 : 0, &res);
}

void SecuritySettings::WarnAppInstallation()
{
    // TODO: DialogFragment?
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);
    String str;
    resource->GetString(R::string::error_title, &str);
    builder->SetTitle(CoreUtils::Convert(str));
    builder->SetIcon(Elastos::Droid::R::drawable::ic_dialog_alert);
    resource->GetString(R::string::install_all_warning, &str);
    builder->SetMessage(CoreUtils::Convert(str));
    builder->SetPositiveButton(Elastos::Droid::R::string::yes, this);
    builder->SetNegativeButton(Elastos::Droid::R::string::no, this);
    AutoPtr<IAlertDialog> dialog;
    builder->Show((IAlertDialog**)&dialog);
    mWarnInstallApps = IDialogInterface::Probe(dialog);
}

ECode SecuritySettings::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (dialog == mWarnInstallApps.Get()) {
        Boolean turnOn = which == IDialogInterface::BUTTON_POSITIVE;
        SetNonMarketAppsAllowed(turnOn);
        if (mToggleAppInstallation != NULL) {
            ITwoStatePreference::Probe(mToggleAppInstallation)->SetChecked(turnOn);
        }
    }
    return NOERROR;
}

ECode SecuritySettings::OnDestroy()
{
    SettingsPreferenceFragment::OnDestroy();
    if (mWarnInstallApps != NULL) {
        mWarnInstallApps->Dismiss();
    }
    return NOERROR;
}

void SecuritySettings::SetupLockAfterPreference()
{
    // Compatible with pre-Froyo
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int64 currentTimeout;
    secure->GetInt64(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_LOCK_AFTER_TIMEOUT, 5000, &currentTimeout);
    mLockAfter->SetValue(StringUtils::ToString(currentTimeout));
    AutoPtr<InnerListener> listener = new InnerListener(this);
    IPreference::Probe(mLockAfter)->SetOnPreferenceChangeListener(listener);
    Int64 adminTimeout = 0;
    if (mDPM != NULL) {
        mDPM->GetMaximumTimeToLock(NULL, &adminTimeout);
    }

    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Int32 data;
    ss->GetInt32(GetContentResolver(), ISettingsSystem::SCREEN_OFF_TIMEOUT, 0, &data);
    using Elastos::Core::Math;
    const Int64 displayTimeout = Math::Max((Int64)0, (Int64)data);
    if (adminTimeout > 0) {
        // This setting is a slave to display timeout when a device policy is enforced.
        // As such, maxLockTimeout = adminTimeout - displayTimeout.
        // If there isn't enough time, shows "immediately" setting.
        DisableUnusableTimeouts(Math::Max((Int64)0, adminTimeout - displayTimeout));
    }
}

void SecuritySettings::UpdateLockAfterPreferenceSummary()
{
    // Update summary message with current value
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int64 currentTimeout;
    secure->GetInt64(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_LOCK_AFTER_TIMEOUT, 5000, &currentTimeout);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mLockAfter->GetEntries((ArrayOf<ICharSequence*>**)&entries);
    AutoPtr< ArrayOf<ICharSequence*> > values;
    mLockAfter->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    Int32 best = 0;
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Int64 timeout = StringUtils::ParseInt64(CoreUtils::Unbox((*values)[i]));
        if (currentTimeout >= timeout) {
            best = i;
        }
    }

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(screen)->FindPreference(CoreUtils::Convert(KEY_TRUST_AGENT), (IPreference**)&preference);
    if (preference != NULL) {
        AutoPtr<ICharSequence> cs;
        preference->GetTitle((ICharSequence**)&cs);
        Int32 length;
        cs->GetLength(&length);
        if (length > 0) {
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
            args->Set(0, (*entries)[best]);
            args->Set(1, cs);
            String str;
            GetString(R::string::lock_after_timeout_summary_with_exception, args, &str);
            IPreference::Probe(mLockAfter)->SetSummary(CoreUtils::Convert(str));
        }
    }
    else {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (*entries)[best]);
        String str;
        GetString(R::string::lock_after_timeout_summary, args, &str);
        IPreference::Probe(mLockAfter)->SetSummary(CoreUtils::Convert(str));
    }
}

void SecuritySettings::DisableUnusableTimeouts(
    /* [in] */ Int64 maxTimeout)
{
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mLockAfter->GetEntries((ArrayOf<ICharSequence*>**)&entries);
    AutoPtr< ArrayOf<ICharSequence*> > values;
    mLockAfter->GetEntryValues((ArrayOf<ICharSequence*>**)&values);

    AutoPtr<IArrayList> revisedEntries, revisedValues;
    CArrayList::New((IArrayList**)&revisedEntries);
    CArrayList::New((IArrayList**)&revisedValues);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Int64 timeout = StringUtils::ParseInt64(CoreUtils::Unbox((*values)[i]));
        if (timeout <= maxTimeout) {
            revisedEntries->Add((*entries)[i]);
            revisedValues->Add((*values)[i]);
        }
    }
    Int32 size1, size2;
    Int32 size;
    if ((revisedEntries->GetSize(&size1), size1) != entries->GetLength()
            || (revisedValues->GetSize(&size2), size2) != values->GetLength()) {
        revisedEntries->GetSize(&size);
        AutoPtr< ArrayOf<IInterface*> > args1 = ArrayOf<IInterface*>::Alloc(size);
        AutoPtr< ArrayOf<IInterface*> > resultArgs1;
        revisedEntries->ToArray(args1, (ArrayOf<IInterface*>**)&resultArgs1);
        AutoPtr< ArrayOf<ICharSequence*> > resultArgsCS1 = ArrayOf<ICharSequence*>::Alloc(resultArgs1->GetLength());
        for (Int32 i  = 0; i < resultArgs1->GetLength(); i++) {
            resultArgsCS1->Set(i, ICharSequence::Probe((*resultArgs1)[i]));
        }
        mLockAfter->SetEntries(resultArgsCS1);

        revisedValues->GetSize(&size);
        AutoPtr< ArrayOf<IInterface*> > args2 = ArrayOf<IInterface*>::Alloc(size);
        AutoPtr< ArrayOf<IInterface*> > resultArgs2;
        revisedValues->ToArray(args2, (ArrayOf<IInterface*>**)&resultArgs2);
        AutoPtr< ArrayOf<ICharSequence*> > resultArgsCS2 = ArrayOf<ICharSequence*>::Alloc(resultArgs2->GetLength());
        for (Int32 i  = 0; i < resultArgs2->GetLength(); i++) {
            resultArgsCS2->Set(i, ICharSequence::Probe((*resultArgs2)[i]));
        }
        mLockAfter->SetEntryValues(resultArgsCS2);

        String str;
        mLockAfter->GetValue(&str);
        const Int32 userPreference = StringUtils::ParseInt32(str);
        if (userPreference <= maxTimeout) {
            mLockAfter->SetValue(StringUtils::ToString(userPreference));
        }
        else {
            // There will be no highlighted selection since nothing in the list matches
            // maxTimeout. The user can still select anything less than maxTimeout.
            // TODO: maybe append maxTimeout to the list and mark selected.
        }
    }
    revisedEntries->GetSize(&size);
    IPreference::Probe(mLockAfter)->SetEnabled(size > 0);
}

ECode SecuritySettings::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    SettingsPreferenceFragment::OnSaveInstanceState(outState);
    if (mTrustAgentClickIntent != NULL) {
        outState->PutParcelable(TRUST_AGENT_CLICK_INTENT, IParcelable::Probe(mTrustAgentClickIntent));
    }
    return NOERROR;
}

ECode SecuritySettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    // Make sure we reload the preference hierarchy since some of these settings
    // depend on others...
    CreatePreferenceHierarchy();

    AutoPtr<ILockPatternUtils> lockPatternUtils = mChooseLockSettingsHelper->Utils();
    Boolean res;
    if (mBiometricWeakLiveliness != NULL) {
        lockPatternUtils->IsBiometricWeakLivelinessEnabled(&res);
        ITwoStatePreference::Probe(mBiometricWeakLiveliness)->SetChecked(res);
    }
    if (mVisiblePattern != NULL) {
        lockPatternUtils->IsVisiblePatternEnabled(&res);
        ITwoStatePreference::Probe(mVisiblePattern)->SetChecked(res);
    }
    if (mPowerButtonInstantlyLocks != NULL) {
        lockPatternUtils->GetPowerButtonInstantlyLocks(&res);
        ITwoStatePreference::Probe(mPowerButtonInstantlyLocks)->SetChecked(res);
    }

    if (mShowPassword != NULL) {
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        Int32 data;
        ss->GetInt32(GetContentResolver(),
                ISettingsSystem::TEXT_SHOW_PASSWORD, 1, &data);
        ITwoStatePreference::Probe(mShowPassword)->SetChecked(data != 0);
    }

    if (mResetCredentials != NULL) {
        mKeyStore->IsEmpty(&res);
        mResetCredentials->SetEnabled(!res);
    }
    return NOERROR;
}

ECode SecuritySettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    String key;
    preference->GetKey(&key);
    if (KEY_UNLOCK_SET_OR_CHANGE.Equals(key)) {
        StartFragment(this, String("Elastos.Droid.Settings.CChooseLockGenericFragment"),
                R::string::lock_settings_picker_title, SET_OR_CHANGE_LOCK_METHOD_REQUEST, NULL);
    }
    else if (KEY_BIOMETRIC_WEAK_IMPROVE_MATCHING.Equals(key)) {
        AutoPtr<IActivity> activity;
        this->GetActivity((IActivity**)&activity);
        AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
        helper->constructor(activity, this);
        if (!helper->LaunchConfirmationActivity(
                CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST, NULL, NULL)) {
            // If this returns FALSE, it means no password confirmation is required, so
            // go ahead and start improve.
            // Note: currently a backup is required for biometric_weak so this code path
            // can't be reached, but is here in case things change in the future
            StartBiometricWeakImprove();
        }
    }
    else if (KEY_TRUST_AGENT.Equals(key)) {
        AutoPtr<IActivity> activity;
        this->GetActivity((IActivity**)&activity);
        AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
        helper->constructor(activity, this);
        mTrustAgentClickIntent = NULL;
        preference->GetIntent((IIntent**)&mTrustAgentClickIntent);
        if (!helper->LaunchConfirmationActivity(CHANGE_TRUST_AGENT_SETTINGS, NULL, NULL) &&
                mTrustAgentClickIntent != NULL) {
            // If this returns FALSE, it means no password confirmation is required.
            StartActivity(mTrustAgentClickIntent);
            mTrustAgentClickIntent = NULL;
        }
    }
    else {
        // If we didn't handle it, let preferences handle it.
        return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, res);
    }
    *res = TRUE;
    return NOERROR;
}

ECode SecuritySettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    SettingsPreferenceFragment::OnActivityResult(requestCode, resultCode, data);
    if (requestCode == CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST &&
            resultCode == IActivity::RESULT_OK) {
        StartBiometricWeakImprove();
        return NOERROR;
    }
    else if (requestCode == CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF &&
            resultCode == IActivity::RESULT_OK) {
        AutoPtr<ILockPatternUtils> lockPatternUtils = mChooseLockSettingsHelper->Utils();
        lockPatternUtils->SetBiometricWeakLivelinessEnabled(FALSE);
        // Setting the mBiometricWeakLiveliness checked value to FALSE is handled when onResume
        // is called by grabbing the value from lockPatternUtils.  We can't set it here
        // because mBiometricWeakLiveliness could be NULL
        return NOERROR;
    }
    else if (requestCode == CHANGE_TRUST_AGENT_SETTINGS && resultCode == IActivity::RESULT_OK) {
        if (mTrustAgentClickIntent != NULL) {
            StartActivity(mTrustAgentClickIntent);
            mTrustAgentClickIntent = NULL;
        }
        return NOERROR;
    }
    CreatePreferenceHierarchy();
    return NOERROR;
}

ECode SecuritySettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Boolean result = TRUE;
    String key;
    preference->GetKey(&key);
    AutoPtr<ILockPatternUtils> lockPatternUtils = mChooseLockSettingsHelper->Utils();
    Boolean val, suc;
    if (KEY_LOCK_AFTER_TIMEOUT.Equals(key)) {
        Int32 timeout = StringUtils::ParseInt32(CoreUtils::Unbox(ICharSequence::Probe(value)));
        // try {
            AutoPtr<ISettingsSecure> secure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
            ECode ec = secure->PutInt32(GetContentResolver(),
                    ISettingsSecure::LOCK_SCREEN_LOCK_AFTER_TIMEOUT, timeout, &suc);
            if (FAILED(ec)) {
                Logger::E("SecuritySettings", "could not persist lockAfter timeout setting %0x08", ec);
            }
        // } catch (NumberFormatException e) {
        //     Logger::E("SecuritySettings", "could not persist lockAfter timeout setting", e);
        // }
        UpdateLockAfterPreferenceSummary();
    }
    else if (KEY_LOCK_ENABLED.Equals(key)) {
        IBoolean::Probe(value)->GetValue(&val);
        lockPatternUtils->SetLockPatternEnabled(val);
    }
    else if (KEY_VISIBLE_PATTERN.Equals(key)) {
        IBoolean::Probe(value)->GetValue(&val);
        lockPatternUtils->SetVisiblePatternEnabled(val);
    }
    else  if (KEY_BIOMETRIC_WEAK_LIVELINESS.Equals(key)) {
        IBoolean::Probe(value)->GetValue(&val);
        if (val) {
            lockPatternUtils->SetBiometricWeakLivelinessEnabled(TRUE);
        }
        else {
            // In this case the user has just unchecked the checkbox, but this action requires
            // them to confirm their password.  We need to re-check the checkbox until
            // they've confirmed their password
            ITwoStatePreference::Probe(mBiometricWeakLiveliness)->SetChecked(TRUE);
            AutoPtr<IActivity> activity;
            this->GetActivity((IActivity**)&activity);
            AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
            helper->constructor(activity, this);
            if (!helper->LaunchConfirmationActivity(
                            CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF, NULL, NULL)) {
                // If this returns FALSE, it means no password confirmation is required, so
                // go ahead and uncheck it here.
                // Note: currently a backup is required for biometric_weak so this code path
                // can't be reached, but is here in case things change in the future
                lockPatternUtils->SetBiometricWeakLivelinessEnabled(FALSE);
                ITwoStatePreference::Probe(mBiometricWeakLiveliness)->SetChecked(FALSE);
            }
        }
    }
    else if (KEY_POWER_INSTANTLY_LOCKS.Equals(key)) {
        IBoolean::Probe(value)->GetValue(&val);
        mLockPatternUtils->SetPowerButtonInstantlyLocks(val);
    }
    else if (KEY_SHOW_PASSWORD.Equals(key)) {
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        IBoolean::Probe(value)->GetValue(&val);
        ss->PutInt32(GetContentResolver(), ISettingsSystem::TEXT_SHOW_PASSWORD,
                val ? 1 : 0, &suc);
    }
    else if (KEY_TOGGLE_INSTALL_APPLICATIONS.Equals(key)) {
        IBoolean::Probe(value)->GetValue(&val);
        if (val) {
            ITwoStatePreference::Probe(mToggleAppInstallation)->SetChecked(FALSE);
            WarnAppInstallation();
            // Don't change Switch status until user makes choice in dialog, so return FALSE.
            result = FALSE;
        }
        else {
            SetNonMarketAppsAllowed(FALSE);
        }
    }
    *res = result;
    return NOERROR;
}

Int32 SecuritySettings::GetHelpResource()
{
    return R::string::help_url_security;
}

ECode SecuritySettings::StartBiometricWeakImprove()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(String("com.android.facelock"), String("com.android.facelock.AddToSetup"));
    StartActivity(intent);
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos