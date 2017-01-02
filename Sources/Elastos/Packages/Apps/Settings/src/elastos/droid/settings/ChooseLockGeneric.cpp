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

#include "Elastos.Droid.AccessibilityService.h"
#include "elastos/droid/settings/ChooseLockGeneric.h"
#include "elastos/droid/settings/CEncryptionInterstitial.h"
#include "elastos/droid/settings/CChooseLockPassword.h"
#include "elastos/droid/settings/CChooseLockPattern.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
using Elastos::Droid::KeyStore::Security::CKeyStoreHelper;
using Elastos::Droid::KeyStore::Security::IKeyStore;
using Elastos::Droid::KeyStore::Security::IKeyStoreHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::CBoolean;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String ChooseLockGeneric::CONFIRM_CREDENTIALS("confirm_credentials");

const String ChooseLockGeneric::ChooseLockGenericFragment::MINIMUM_QUALITY_KEY("minimum_quality");
const String ChooseLockGeneric::ChooseLockGenericFragment::ENCRYPT_REQUESTED_QUALITY("encrypt_requested_quality");
const String ChooseLockGeneric::ChooseLockGenericFragment::ENCRYPT_REQUESTED_DISABLED("encrypt_requested_disabled");

const Int32 ChooseLockGeneric::ChooseLockGenericFragment::MIN_PASSWORD_LENGTH = 4;
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_BACKUP_INFO("unlock_backup_info");
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_SET_OFF("unlock_set_off");
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_SET_NONE("unlock_set_none");
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_SET_BIOMETRIC_WEAK("unlock_set_biometric_weak");
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_SET_PIN("unlock_set_pin");
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_SET_PASSWORD("unlock_set_password");
const String ChooseLockGeneric::ChooseLockGenericFragment::KEY_UNLOCK_SET_PATTERN("unlock_set_pattern");
const Int32 ChooseLockGeneric::ChooseLockGenericFragment::CONFIRM_EXISTING_REQUEST = 100;
const Int32 ChooseLockGeneric::ChooseLockGenericFragment::FALLBACK_REQUEST = 101;
const Int32 ChooseLockGeneric::ChooseLockGenericFragment::ENABLE_ENCRYPTION_REQUEST = 102;
const String ChooseLockGeneric::ChooseLockGenericFragment::PASSWORD_CONFIRMED("password_confirmed");

const String ChooseLockGeneric::ChooseLockGenericFragment::WAITING_FOR_CONFIRMATION("waiting_for_confirmation");
const String ChooseLockGeneric::ChooseLockGenericFragment::FINISH_PENDING("finish_pending");

const Boolean ChooseLockGeneric::ChooseLockGenericFragment::ALWAY_SHOW_TUTORIAL = TRUE;

//===============================================================================
//                  ChooseLockGeneric::ChooseLockGenericFragment
//===============================================================================

ChooseLockGeneric::ChooseLockGenericFragment::ChooseLockGenericFragment()
    : mPasswordConfirmed(FALSE)
    , mWaitingForConfirmation(FALSE)
    , mFinishPending(FALSE)
    , mEncryptionRequestQuality(0)
    , mEncryptionRequestDisabled(FALSE)
    , mRequirePassword(FALSE)
{}

ChooseLockGeneric::ChooseLockGenericFragment::~ChooseLockGenericFragment()
{}

ECode ChooseLockGeneric::ChooseLockGenericFragment::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);

    mDPM = IDevicePolicyManager::Probe(GetSystemService(IContext::DEVICE_POLICY_SERVICE));

    AutoPtr<IKeyStoreHelper> helper;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
    helper->GetInstance((IKeyStore**)&mKeyStore);
    AutoPtr<IActivity> thisActivity;
    this->GetActivity((IActivity**)&thisActivity);
    mChooseLockSettingsHelper = new ChooseLockSettingsHelper();
    mChooseLockSettingsHelper->constructor(thisActivity);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&mLockPatternUtils);

    // Defaults to needing to confirm credentials
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean confirmCredentials;
    intent->GetBooleanExtra(CONFIRM_CREDENTIALS, TRUE, &confirmCredentials);
    if (IChooseLockGenericInternalActivity::Probe(activity) != NULL) {
        mPasswordConfirmed = !confirmCredentials;
    }

    if (savedInstanceState != NULL) {
        savedInstanceState->GetBoolean(PASSWORD_CONFIRMED, &mPasswordConfirmed);
        savedInstanceState->GetBoolean(WAITING_FOR_CONFIRMATION, &mWaitingForConfirmation);
        savedInstanceState->GetBoolean(FINISH_PENDING, &mFinishPending);
        savedInstanceState->GetInt32(ENCRYPT_REQUESTED_QUALITY, &mEncryptionRequestQuality);
        savedInstanceState->GetBoolean(ENCRYPT_REQUESTED_DISABLED, &mEncryptionRequestDisabled);
    }

    if (mPasswordConfirmed) {
        UpdatePreferencesOrFinish();
    }
    else if (!mWaitingForConfirmation) {
        AutoPtr<ChooseLockSettingsHelper> helper = new ChooseLockSettingsHelper();
        helper->constructor(thisActivity, this);
        if (!helper->LaunchConfirmationActivity(CONFIRM_EXISTING_REQUEST, NULL, NULL)) {
            mPasswordConfirmed = TRUE; // no password set, so no need to confirm
            UpdatePreferencesOrFinish();
        }
        else {
            mWaitingForConfirmation = TRUE;
        }
    }
    return NOERROR;
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    if (mFinishPending) {
        mFinishPending = FALSE;
        Finish();
    }
    return NOERROR;
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String key;
    preference->GetKey(&key);
    Boolean handled = TRUE;

    // EventLog->WriteEvent(IEventLogTags::LOCK_SCREEN_TYPE, key);

    if (KEY_UNLOCK_SET_OFF.Equals(key)) {
        UpdateUnlockMethodAndFinish(
                IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, TRUE);
    }
    else if (KEY_UNLOCK_SET_NONE.Equals(key)) {
        UpdateUnlockMethodAndFinish(
                IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, FALSE);
    }
    else if (KEY_UNLOCK_SET_BIOMETRIC_WEAK.Equals(key)) {
        MaybeEnableEncryption(
                IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK, FALSE);
    }
    else if (KEY_UNLOCK_SET_PATTERN.Equals(key)) {
        MaybeEnableEncryption(
                IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING, FALSE);
    }
    else if (KEY_UNLOCK_SET_PIN.Equals(key)) {
        MaybeEnableEncryption(
                IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC, FALSE);
    }
    else if (KEY_UNLOCK_SET_PASSWORD.Equals(key)) {
        MaybeEnableEncryption(
                IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC, FALSE);
    }
    else {
        handled = FALSE;
    }
    *result = handled;
    return NOERROR;
}

void ChooseLockGeneric::ChooseLockGenericFragment::MaybeEnableEncryption(
    /* [in] */ Int32 quality,
    /* [in] */ Boolean disabled)
{
    AutoPtr<IUserHandle> handle;
    Process::MyUserHandle((IUserHandle**)&handle);
    Boolean res1;
    handle->IsOwner(&res1);

    AutoPtr<ILockPatternUtilsHelper> helper;
    CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
    Boolean res2;
    helper->IsDeviceEncryptionEnabled(&res2);
    if (res1 && res2) {
        mEncryptionRequestQuality = quality;
        mEncryptionRequestDisabled = disabled;
        // If accessibility is enabled and the user hasn't seen this dialog before, set the
        // default state to agree with that which is compatible with accessibility
        // (password not required).
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IAccessibilityManagerHelper> amHelper;
        CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&amHelper);
        AutoPtr<IAccessibilityManager> manager;
        amHelper->GetInstance(IContext::Probe(activity), (IAccessibilityManager**)&manager);
        Boolean accEn;
        manager->IsEnabled(&accEn);
        Boolean required;
        mLockPatternUtils->IsCredentialRequiredToDecrypt(!accEn, &required);
        AutoPtr<IIntent> intent = CEncryptionInterstitial::CreateStartIntent(
                IContext::Probe(activity), quality, required);
        StartActivityForResult(intent, ENABLE_ENCRYPTION_REQUEST);
    }
    else {
        mRequirePassword = FALSE; // device encryption not enabled or not device owner.
        UpdateUnlockMethodAndFinish(quality, disabled);
    }
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> v;
    SettingsPreferenceFragment::OnCreateView(inflater, container, savedInstanceState, (IView**)&v);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean onlyShowFallback;
    intent->GetBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE, &onlyShowFallback);
    if (onlyShowFallback) {
        AutoPtr<IView> header;
        Elastos::Droid::View::View::Inflate(IContext::Probe(activity),
                R::layout::weak_biometric_fallback_header, NULL, (IView**)&header);
        AutoPtr<IView> tmp;
        v->FindViewById(Elastos::Droid::R::id::list, (IView**)&tmp);
        IListView::Probe(tmp)->AddHeaderView(header, NULL, FALSE);
    }

    *view = v;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    SettingsPreferenceFragment::OnActivityResult(requestCode, resultCode, data);
    mWaitingForConfirmation = FALSE;
    if (requestCode == CONFIRM_EXISTING_REQUEST && resultCode == IActivity::RESULT_OK) {
        mPasswordConfirmed = TRUE;
        UpdatePreferencesOrFinish();
    }
    else if (requestCode == FALLBACK_REQUEST) {
        mChooseLockSettingsHelper->Utils()->DeleteTempGallery();
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        activity->SetResult(resultCode);
        Finish();
    }
    else if (requestCode == ENABLE_ENCRYPTION_REQUEST
            && resultCode == IActivity::RESULT_OK) {
        data->GetBooleanExtra(
                CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, TRUE, &mRequirePassword);
        UpdateUnlockMethodAndFinish(mEncryptionRequestQuality, mEncryptionRequestDisabled);
    }
    else {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        activity->SetResult(IActivity::RESULT_CANCELED);
        Finish();
    }
    return NOERROR;
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    SettingsPreferenceFragment::OnSaveInstanceState(outState);
    // Saved so we don't force user to re-enter their password if configuration changes
    outState->PutBoolean(PASSWORD_CONFIRMED, mPasswordConfirmed);
    outState->PutBoolean(WAITING_FOR_CONFIRMATION, mWaitingForConfirmation);
    outState->PutBoolean(FINISH_PENDING, mFinishPending);
    outState->PutInt32(ENCRYPT_REQUESTED_QUALITY, mEncryptionRequestQuality);
    outState->PutBoolean(ENCRYPT_REQUESTED_DISABLED, mEncryptionRequestDisabled);
    return NOERROR;
}

void ChooseLockGeneric::ChooseLockGenericFragment::UpdatePreferencesOrFinish()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Int32 quality;
    intent->GetInt32Extra(ILockPatternUtils::PASSWORD_TYPE_KEY, -1, &quality);
    if (quality == -1) {
        // If caller didn't specify password quality, show UI and allow the user to choose.
        intent->GetInt32Extra(MINIMUM_QUALITY_KEY, -1, &quality);
        AutoPtr<IBoolean> allowBiometric;
        CBoolean::New(FALSE, (IBoolean**)&allowBiometric);
        quality = UpgradeQuality(quality, allowBiometric);
        AutoPtr<IPreferenceScreen> prefScreen;
        GetPreferenceScreen((IPreferenceScreen**)&prefScreen);
        if (prefScreen != NULL) {
            IPreferenceGroup::Probe(prefScreen)->RemoveAll();
        }
        AddPreferencesFromResource(R::xml::security_settings_picker);
        DisableUnusablePreferences(quality, allowBiometric);
        UpdatePreferenceSummaryIfNeeded();
    }
    else {
        UpdateUnlockMethodAndFinish(quality, FALSE);
    }
}

Int32 ChooseLockGeneric::ChooseLockGenericFragment::UpgradeQuality(
    /* [in] */ Int32 quality,
    /* [in] */ IBoolean* allowBiometric)
{
    quality = UpgradeQualityForDPM(quality);
    quality = UpgradeQualityForKeyStore(quality);
    return quality;
}

Int32 ChooseLockGeneric::ChooseLockGenericFragment::UpgradeQualityForDPM(
    /* [in] */ Int32 quality)
{
    // Compare min allowed password quality
    Int32 minQuality;
    mDPM->GetPasswordQuality(NULL, &minQuality);
    if (quality < minQuality) {
        quality = minQuality;
    }
    return quality;
}

Int32 ChooseLockGeneric::ChooseLockGenericFragment::UpgradeQualityForKeyStore(
    /* [in] */ Int32 quality)
{
    Boolean res;
    mKeyStore->IsEmpty(&res);
    if (!res) {
       if (quality < IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING/*CredentialStorage::MIN_PASSWORD_QUALITY*/) {
           quality = IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING/*CredentialStorage::MIN_PASSWORD_QUALITY*/;
       }
    }
    return quality;
}

void ChooseLockGeneric::ChooseLockGenericFragment::DisableUnusablePreferences(
    /* [in] */ Int32 quality,
    /* [in] */ IBoolean* allowBiometric)
{
    AutoPtr<IPreferenceScreen> entries;
    GetPreferenceScreen((IPreferenceScreen**)&entries);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean onlyShowFallback;
    intent->GetBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE, &onlyShowFallback);
    Boolean weakBiometricAvailable;
    mChooseLockSettingsHelper->Utils()->IsBiometricWeakInstalled(&weakBiometricAvailable);

    // if there are multiple users, disable "None" setting
    AutoPtr<IUserManager> mUm = IUserManager::Probe(GetSystemService(IContext::USER_SERVICE));
    AutoPtr<IList> users; // List<UserInfo>
    mUm->GetUsers(TRUE, (IList**)&users);
    Int32 size;
    Boolean singleUser = (users->GetSize(&size), size) == 1;

    IPreferenceGroup* _entries = IPreferenceGroup::Probe(entries);
    Int32 count;
    _entries->GetPreferenceCount(&count);
    for (Int32 i = count - 1; i >= 0; --i) {
        AutoPtr<IPreference> pref;
        _entries->GetPreference(i, (IPreference**)&pref);
        if (IPreferenceScreen::Probe(pref) != NULL) {
            String key;
            pref->GetKey(&key);
            Boolean enabled = TRUE;
            Boolean visible = TRUE;
            if (KEY_UNLOCK_SET_OFF.Equals(key)) {
                enabled = quality <= IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;
                visible = singleUser; // don't show when there's more than 1 user
            }
            else if (KEY_UNLOCK_SET_NONE.Equals(key)) {
                enabled = quality <= IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;
            }
            else if (KEY_UNLOCK_SET_BIOMETRIC_WEAK.Equals(key)) {
                Boolean value;
                allowBiometric->GetValue(&value);
                enabled = quality <= IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK || value;
                visible = weakBiometricAvailable; // If not available, then don't show it.
            }
            else if (KEY_UNLOCK_SET_PATTERN.Equals(key)) {
                enabled = quality <= IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING;
            }
            else if (KEY_UNLOCK_SET_PIN.Equals(key)) {
                enabled = quality <= IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX;
            }
            else if (KEY_UNLOCK_SET_PASSWORD.Equals(key)) {
                enabled = quality <= IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX;
            }
            if (!visible || (onlyShowFallback && !AllowedForFallback(key))) {
                Boolean res;
                _entries->RemovePreference(pref, &res);
            }
            else if (!enabled) {
                pref->SetSummary(R::string::unlock_set_unlock_disabled_summary);
                pref->SetEnabled(FALSE);
            }
        }
    }
}

void ChooseLockGeneric::ChooseLockGenericFragment::UpdatePreferenceSummaryIfNeeded()
{
    AutoPtr<ILockPatternUtilsHelper> helper;
    CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
    Boolean res;
    helper->IsDeviceEncrypted(&res);
    if (res) {
        return;
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAccessibilityManagerHelper> amHelper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&amHelper);
    AutoPtr<IAccessibilityManager> manager;
    amHelper->GetInstance(IContext::Probe(activity), (IAccessibilityManager**)&manager);
    AutoPtr<IList> list;
    manager->GetEnabledAccessibilityServiceList(IAccessibilityServiceInfo::FEEDBACK_ALL_MASK, (IList**)&list);
    if (list->IsEmpty(&res), res) {
        return;
    }

    String str;
    GetString(R::string::secure_lock_encryption_warning, &str);
    AutoPtr<ICharSequence> summary = CoreUtils::Convert(str);

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    IPreferenceGroup* _screen = IPreferenceGroup::Probe(screen);
    Int32 preferenceCount;
    _screen->GetPreferenceCount(&preferenceCount);
    for (Int32 i = 0; i < preferenceCount; i++) {
        AutoPtr<IPreference> preference;
        _screen->GetPreference(i, (IPreference**)&preference);
        String str;
        preference->GetKey(&str);
        if (str.Equals(KEY_UNLOCK_SET_PATTERN) || str.Equals(KEY_UNLOCK_SET_PIN) || str.Equals(KEY_UNLOCK_SET_PASSWORD)) {
            preference->SetSummary(summary);
        }
    }
}

Boolean ChooseLockGeneric::ChooseLockGenericFragment::AllowedForFallback(
    /* [in] */ const String& key)
{
    return KEY_UNLOCK_BACKUP_INFO.Equals(key)  ||
            KEY_UNLOCK_SET_PATTERN.Equals(key) || KEY_UNLOCK_SET_PIN.Equals(key);
}

AutoPtr<IIntent> ChooseLockGeneric::ChooseLockGenericFragment::GetBiometricSensorIntent()
{
    AutoPtr<IIntent> fallBackIntent;
    CIntent::New((IIntent**)&fallBackIntent);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    fallBackIntent->SetClass(IContext::Probe(activity), ECLSID_CChooseLockGenericInternalActivity);
    fallBackIntent->PutBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, TRUE);
    fallBackIntent->PutBooleanExtra(CONFIRM_CREDENTIALS, FALSE);
    fallBackIntent->PutExtra(EXTRA_SHOW_FRAGMENT_TITLE, R::string::backup_lock_settings_picker_title);

    Boolean res;
    Boolean showTutorial = ALWAY_SHOW_TUTORIAL ||
            (mChooseLockSettingsHelper->Utils()->IsBiometricWeakEverChosen(&res), !res);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(String("com.android.facelock"), String("com.android.facelock.SetupIntro"));
    intent->PutBooleanExtra(String("showTutorial"), showTutorial);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pending;
    helper->GetActivity(IContext::Probe(activity), 0, fallBackIntent, 0, (IPendingIntent**)&pending);
    intent->PutExtra(String("PendingIntent"), IParcelable::Probe(pending));
    return intent;
}

ECode ChooseLockGeneric::ChooseLockGenericFragment::UpdateUnlockMethodAndFinish(
    /* [in] */ Int32 quality,
    /* [in] */ Boolean disabled)
{
    // Sanity check. We should never get here without confirming user's existing password.
    if (!mPasswordConfirmed) {
        Slogger::E("ChooseLockGeneric::UpdateUnlockMethodAndFinish", "Tried to update password without confirming it");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean isFallback;
    intent->GetBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE, &isFallback);

    quality = UpgradeQuality(quality, NULL);

    if (quality >= IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC) {
        Int32 minLength;
        mDPM->GetPasswordMinimumLength(NULL, &minLength);
        if (minLength < MIN_PASSWORD_LENGTH) {
            minLength = MIN_PASSWORD_LENGTH;
        }
        Int32 maxLength;
        mDPM->GetPasswordMaximumLength(quality, &maxLength);
        AutoPtr<IIntent> intent = CChooseLockPassword::CreateIntent(IContext::Probe(activity), quality, isFallback,
                minLength, maxLength, mRequirePassword, FALSE /* confirm credentials */);
        if (isFallback) {
            StartActivityForResult(intent, FALLBACK_REQUEST);
            return NOERROR;
        }
        else {
            mFinishPending = TRUE;
            intent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
            StartActivity(intent);
        }
    }
    else if (quality == IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING) {
        AutoPtr<IIntent> intent = CChooseLockPattern::CreateIntent(IContext::Probe(activity),
                isFallback, mRequirePassword, FALSE /* confirm credentials */);
        if (isFallback) {
            StartActivityForResult(intent, FALLBACK_REQUEST);
            return NOERROR;
        }
        else {
            mFinishPending = TRUE;
            intent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
            StartActivity(intent);
        }
    }
    else if (quality == IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK) {
        AutoPtr<IIntent> intent = GetBiometricSensorIntent();
        mFinishPending = TRUE;
        StartActivity(intent);
    }
    else if (quality == IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
        mChooseLockSettingsHelper->Utils()->ClearLock(FALSE);
        mChooseLockSettingsHelper->Utils()->SetLockScreenDisabled(disabled);
        activity->SetResult(IActivity::RESULT_OK);
        Finish();
    }
    else {
        Finish();
    }
    return NOERROR;
}

Int32 ChooseLockGeneric::ChooseLockGenericFragment::GetHelpResource()
{
    return R::string::help_url_choose_lockscreen;
}

//===============================================================================
//                  ChooseLockGeneric
//===============================================================================

ChooseLockGeneric::ChooseLockGeneric()
{}

ChooseLockGeneric::~ChooseLockGeneric()
{}

ECode ChooseLockGeneric::constructor()
{
    return SettingsActivity::constructor();
}

ECode ChooseLockGeneric::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> tmp;
    SettingsActivity::GetIntent((IIntent**)&tmp);
    AutoPtr<IIntent> modIntent;
    CIntent::New(tmp, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.CChooseLockGenericFragment"));
    *intent = modIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

Boolean ChooseLockGeneric::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    if (String("Elastos.Droid.Settings.CChooseLockGenericFragment").Equals(fragmentName)) return TRUE;
    return FALSE;
}

//===============================================================================
//                  InternalActivity
//===============================================================================

CAR_INTERFACE_IMPL(InternalActivity, ChooseLockGeneric, IChooseLockGenericInternalActivity)

InternalActivity::InternalActivity()
{}

InternalActivity::~InternalActivity()
{}

ECode InternalActivity::constructor()
{
    return ChooseLockGeneric::constructor();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos