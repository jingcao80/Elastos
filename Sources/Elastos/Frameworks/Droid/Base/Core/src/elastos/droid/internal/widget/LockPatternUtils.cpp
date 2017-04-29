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

#include "elastos/droid/internal/widget/LockPatternUtils.h"
#include "elastos/droid/internal/widget/CLockPatternUtilsCache.h"
#include "elastos/droid/internal/widget/CLockPatternViewCellHelper.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.CoreLibrary.Security.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/CUserManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Character.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IProfile;
using Elastos::Droid::App::IProfileLockMode;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::CUserManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::Storage::EIID_IIMountService;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::EIID_IIWindowManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CString;
using Elastos::Core::Character;
using Elastos::Core::StringUtils;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::ISecureRandom;
using Elastos::Security::ISecureRandomHelper;
using Elastos::Security::CSecureRandomHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

LockPatternUtils::LockAsyncTask::LockAsyncTask(
    /* [in] */ Int32 type,
    /* [in] */ const String& password,
    /* [in] */ IInterface* service)
    : mType(type)
    , mPassword(password)
    , mService(service)
{
    REFCOUNT_ADD(this);
}

ECode LockPatternUtils::LockAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIMountService> mountService = IIMountService::Probe(mService);
    Int32 value = 0;
    if (FAILED(mountService->ChangeEncryptionPassword(mType, mPassword, &value))) {
        Slogger::E(LockPatternUtils::TAG, "Error changing encryption password");
    }
    *result = NULL;
    return NOERROR;
}

ECode LockPatternUtils::LockAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    REFCOUNT_RELEASE(this);
    return NOERROR;
}

const String LockPatternUtils::TAG("LockPatternUtils");
const Boolean LockPatternUtils::DEBUG = FALSE;
const String LockPatternUtils::LOCK_SCREEN_OWNER_INFO = ISettingsSecure::LOCK_SCREEN_OWNER_INFO;
const String LockPatternUtils::LOCK_SCREEN_OWNER_INFO_ENABLED = ISettingsSecure::LOCK_SCREEN_OWNER_INFO_ENABLED;
const String LockPatternUtils::ENABLED_TRUST_AGENTS("lockscreen.enabledtrustagents");
volatile Int32 LockPatternUtils::sCurrentUserId = IUserHandle::USER_NULL;
ECode LockPatternUtils::GetDevicePolicyManager(
    /* [out] */ IDevicePolicyManager** mger)
{
    VALIDATE_NOT_NULL(mger);
    if (!mDevicePolicyManager) {
        AutoPtr<IInterface> temp;
        mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&temp);
        mDevicePolicyManager = IDevicePolicyManager::Probe(temp);
        if (!mDevicePolicyManager) {
            Slogger::E(TAG, "Can't get DevicePolicyManagerService: is it running?");
        }
    }
    *mger = mDevicePolicyManager;
    REFCOUNT_ADD(*mger);
    return NOERROR;
}

AutoPtr<IDevicePolicyManager> LockPatternUtils::GetDevicePolicyManager()
{
    AutoPtr<IDevicePolicyManager> mger;
    GetDevicePolicyManager((IDevicePolicyManager**)&mger);
    return mger;
}

AutoPtr<ITrustManager> LockPatternUtils::GetTrustManager()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TRUST_SERVICE, (IInterface**)&obj);
    AutoPtr<ITrustManager> trust = ITrustManager::Probe(obj);
    if (trust == NULL) {
        Slogger::E(TAG, "Can't get TrustManagerService: is it running?"/*,
                new IllegalStateException("Stack trace:")*/);
    }
    return trust;
}

CAR_INTERFACE_IMPL(LockPatternUtils, Object, ILockPatternUtils);
LockPatternUtils::LockPatternUtils()
    : mMultiUserMode(FALSE)
{
}

ECode LockPatternUtils::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mContentResolver);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::PROFILE_SERVICE, (IInterface**)&obj);
    mProfileManager = IProfileManager::Probe(obj);

    // If this is being called by the system or by an application like keyguard that
    // has permision INTERACT_ACROSS_USERS, then LockPatternUtils will operate in multi-user
    // mode where calls are for the current user rather than the user of the calling process.
    Int32 result = 0;
    context->CheckCallingOrSelfPermission(
            Manifest::permission::INTERACT_ACROSS_USERS_FULL, &result);
    mMultiUserMode = result == IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

AutoPtr<IILockSettings> LockPatternUtils::GetLockSettings()
{
    if (mLockSettingsService == NULL) {
        AutoPtr<IInterface> temp = ServiceManager::GetService(String("lock_settings"));
        mLockSettingsService = IILockSettings::Probe(
                CLockPatternUtilsCache::GetInstance(IILockSettings::Probe(temp)));
    }
    return mLockSettingsService;
}

ECode LockPatternUtils::GetRequestedMinimumPasswordLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumLength(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordQuality(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordQuality(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordHistoryLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordHistoryLength(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordMinimumLetters(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumLetters(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordMinimumUpperCase(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumUpperCase(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordMinimumLowerCase(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumLowerCase(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordMinimumNumeric(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumNumeric(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordMinimumSymbols(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumSymbols(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetRequestedPasswordMinimumNonLetter(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return GetDevicePolicyManager()->GetPasswordMinimumNonLetter(NULL, GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::ReportFailedPasswordAttempt()
{
    Int32 userId = GetCurrentOrCallingUserId();
    GetDevicePolicyManager()->ReportFailedPasswordAttempt(userId);
    GetTrustManager()->ReportUnlockAttempt(FALSE /* authenticated */, userId);
    GetTrustManager()->ReportRequireCredentialEntry(userId);
    return NOERROR;
}

ECode LockPatternUtils::ReportSuccessfulPasswordAttempt()
{
    GetDevicePolicyManager()->ReportSuccessfulPasswordAttempt(GetCurrentOrCallingUserId());
    GetTrustManager()->ReportUnlockAttempt(TRUE /* authenticated */,
            GetCurrentOrCallingUserId());
    return NOERROR;
}

ECode LockPatternUtils::SetCurrentUser(
    /* [in] */ Int32 userId)
{
    sCurrentUserId = userId;
    return NOERROR;
}

ECode LockPatternUtils::GetCurrentUser(
    /* [out] */ Int32* user)
{
    VALIDATE_NOT_NULL(user);
    if (sCurrentUserId != IUserHandle::USER_NULL) {
        *user = sCurrentUserId;
        return NOERROR;
    }
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IUserInfo> info;
    ECode ec = am->GetCurrentUser((IUserInfo**)&info);
    Int32 id;
    info->GetId(&id);
    if (FAILED(ec)) {
        *user = IUserHandle::USER_OWNER;
        return NOERROR;
    }
    *user = id;
    return NOERROR;
}

ECode LockPatternUtils::RemoveUser(
    /* [in] */ Int32 userId)
{
    ECode ec = GetLockSettings()->RemoveUser(userId);
    if (FAILED(ec)) {
        Slogger::E(TAG, String("Couldn't remove lock settings for user ") + StringUtils::ToString(userId));
    }
    return NOERROR;
}

Int32 LockPatternUtils::GetCurrentOrCallingUserId()
{
    if (mMultiUserMode) {
        Int32 user = 0;
        GetCurrentUser(&user);
        return user;
    }
    else {
        return UserHandle::GetCallingUserId();
    }
}

ECode LockPatternUtils::CheckPattern(
    /* [in] */ IList* pattern,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 userId = GetCurrentOrCallingUserId();
    String str;
    PatternToString(pattern, &str);
    ECode ec = GetLockSettings()->CheckPattern(str, userId, result);
    if (FAILED(ec)) {
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode LockPatternUtils::CheckPassword(
    /* [in] */ const String& password,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 userId = GetCurrentOrCallingUserId();
    ECode ec = GetLockSettings()->CheckPassword(password, userId, result);
    if (FAILED(ec)) {
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode LockPatternUtils::CheckVoldPassword(
    /* [out] */ Boolean* match)
{
    VALIDATE_NOT_NULL(match);
    const Int32 userId = GetCurrentOrCallingUserId();
    if (FAILED(GetLockSettings()->CheckVoldPassword(userId, match))) {
        *match = FALSE;
    }
    return NOERROR;
}

ECode LockPatternUtils::CheckPasswordHistory(
    /* [in] */ const String& password,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > bytes;
    PasswordToHash(password, GetCurrentOrCallingUserId(), (ArrayOf<Byte>**)&bytes);
    if (bytes == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    String passwordHashString = String(*bytes);
    String passwordHistory = GetString(PASSWORD_HISTORY_KEY);
    if (passwordHistory.IsNullOrEmpty()) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 passwordHashLength = passwordHashString.GetLength();
    Int32 passwordHistoryLength = 0;
    GetRequestedPasswordHistoryLength(&passwordHistoryLength);
    if(passwordHistoryLength == 0) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 neededPasswordHistoryLength = passwordHashLength * passwordHistoryLength
            + passwordHistoryLength - 1;
    if (passwordHistory.GetLength() > neededPasswordHistoryLength) {
        passwordHistory = passwordHistory.Substring(0, neededPasswordHistoryLength);
    }
    *result = passwordHistory.Contains(passwordHashString);
    return NOERROR;
}

ECode LockPatternUtils::SavedPatternExists(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ECode ec = GetLockSettings()->HavePattern(GetCurrentOrCallingUserId(), result);
    if (FAILED(ec)) {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode LockPatternUtils::SavedPasswordExists(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ECode ec = GetLockSettings()->HavePassword(GetCurrentOrCallingUserId(), result);
    if (FAILED(ec)) {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode LockPatternUtils::IsPatternEverChosen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBoolean(PATTERN_EVER_CHOSEN_KEY, FALSE);
    return NOERROR;
}

ECode LockPatternUtils::IsBiometricWeakEverChosen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBoolean(BIOMETRIC_WEAK_EVER_CHOSEN_KEY, FALSE);
    return NOERROR;
}

ECode LockPatternUtils::GetActivePasswordQuality(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;

    Int32 quality = (Int32) GetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING);
    Boolean value = FALSE;
    switch (quality) {
        case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
            if (IsLockPatternEnabled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING;
            }
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK:
            if (IsBiometricWeakInstalled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK;
            }
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
            if (IsLockPasswordEnabled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC;
            }
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
            if (IsLockPasswordEnabled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX;
            }
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
            if (IsLockPasswordEnabled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC;
            }
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
            if (IsLockPasswordEnabled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC;
            }
            break;
        case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
            if (IsLockPasswordEnabled(&value), value) {
                activePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX;
            }
            break;
    }

    *result = activePasswordQuality;
    return NOERROR;
}

ECode LockPatternUtils::ClearLock(
    /* [in] */ Boolean isFallback)
{
    if(!isFallback) DeleteGallery();
    SaveLockPassword(String(""), IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING);
    SetLockPatternEnabled(FALSE);
    SaveLockPattern(NULL);
    SetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED);
    SetInt64(PASSWORD_TYPE_ALTERNATE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED);
    OnAfterChangingPassword();
    return NOERROR;
}

ECode LockPatternUtils::SetLockScreenDisabled(
    /* [in] */ Boolean disable)
{
    SetInt64(DISABLE_LOCKSCREEN_KEY, disable ? 1 : 0);

    return NOERROR;
}

ECode LockPatternUtils::IsLockScreenDisabled(
    /* [out] */ Boolean* disabled)
{
    VALIDATE_NOT_NULL(disabled);
    Boolean value = FALSE;
    if ((IsSecure(&value), !value) && GetInt64(DISABLE_LOCKSCREEN_KEY, 0) != 0) {
        // Check if the number of switchable users forces the lockscreen.
        AutoPtr<IList> users;/*<UserInfo*/
        CUserManager::Get(mContext)->GetUsers(TRUE, (IList**)&users);
        Int32 userCount = 0;
        users->GetSize(&userCount);
        Int32 switchableUsers = 0;
        for (Int32 i = 0; i < userCount; i++) {
            AutoPtr<IInterface> item;
            users->Get(i, (IInterface**)&item);
            if (IUserInfo::Probe(item)->SupportsSwitchTo(&value), value) {
                switchableUsers++;
            }
        }
        *disabled = switchableUsers < 2;
        return NOERROR;
    }
    *disabled = FALSE;
    return NOERROR;
}

ECode LockPatternUtils::DeleteTempGallery()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetAction(String("com.android.facelock.DELETE_GALLERY"));
    AutoPtr< ArrayOf<Boolean> > bArray = ArrayOf<Boolean>::Alloc(1);
    (*bArray)[0] = TRUE;
    intent->PutBooleanArrayExtra(String("deleteTempGallery"), bArray);
    mContext->SendBroadcast(intent);
    return NOERROR;
}

ECode LockPatternUtils::DeleteGallery()
{
    Boolean result = FALSE;
    if (UsingBiometricWeak(&result), result) {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetAction(String("com.android.facelock.DELETE_GALLERY"));
        AutoPtr< ArrayOf<Boolean> > bArray = ArrayOf<Boolean>::Alloc(1);
        (*bArray)[0] = TRUE;
        intent->PutBooleanArrayExtra(String("deleteGallery"), bArray);
        mContext->SendBroadcast(intent);
    }
    return NOERROR;
}

ECode LockPatternUtils::SaveLockPattern(
    /* [in] */ IList* pattern)
{
    return SaveLockPattern(pattern, FALSE);
}

ECode LockPatternUtils::SaveLockPattern(
    /* [in] */ IList* pattern,
    /* [in] */ Boolean isFallback)
{
    // try {
    Int32 userId = GetCurrentOrCallingUserId();
    AutoPtr<IDevicePolicyManager> dpm;
    Boolean required = FALSE;
    Int32 size = 0;
    ECode ec = NOERROR;
    String str;
    PatternToString(pattern, &str);
    ec = GetLockSettings()->SetLockPattern(str, userId);
    FAIL_GOTO(ec, Error);
    dpm = GetDevicePolicyManager();
    if (pattern != NULL) {
        // Update the device encryption password.
        if (userId == IUserHandle::USER_OWNER && LockPatternUtils::IsDeviceEncryptionEnabled()) {
            IsCredentialRequiredToDecrypt(TRUE, &required);
            if (!required) {
                ClearEncryptionPassword();
            }
            else {
                String stringPattern;
                PatternToString(pattern, &stringPattern);
                UpdateEncryptionPassword(IStorageManager::CRYPT_TYPE_PATTERN, stringPattern);
            }
        }

        SetBoolean(PATTERN_EVER_CHOSEN_KEY, TRUE);
        if (!isFallback) {
            DeleteGallery();
            SetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING);
            pattern->GetSize(&size);
            ec = dpm->SetActivePasswordState(IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING,
                    size, 0, 0, 0, 0, 0, 0, userId);
            FAIL_GOTO(ec, Error);
        }
        else {
            SetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK);
            SetInt64(PASSWORD_TYPE_ALTERNATE_KEY,
                    IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING);
            FinishBiometricWeak();
            ec = dpm->SetActivePasswordState(IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK,
                    0, 0, 0, 0, 0, 0, 0, userId);
            FAIL_GOTO(ec, Error);
        }
    }
    else {
        ec = dpm->SetActivePasswordState(IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, 0, 0,
                0, 0, 0, 0, 0, userId);
        FAIL_GOTO(ec, Error);
    }
    OnAfterChangingPassword();

Error:
    if (FAILED(ec)) {
        Slogger::E(TAG, "Couldn't save lock pattern ");
    }
    // } catch (RemoteException re) {
    //     Log.e(TAG, "Couldn't save lock pattern " + re);
    // }
    return NOERROR;
}

void LockPatternUtils::UpdateCryptoUserInfo()
{
    Int32 userId = GetCurrentOrCallingUserId();
    if (userId != IUserHandle::USER_OWNER) {
        return;
    }

    Boolean enabled = FALSE;
    IsOwnerInfoEnabled(&enabled);
    String ownerInfo;
    if (enabled) {
        GetOwnerInfo(userId, &ownerInfo);
    }
    else {
        ownerInfo = String("");
    }

    AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
    if (service == NULL) {
        Slogger::E(TAG, "Could not find the mount service to update the user info");
        return;
    }

    AutoPtr<IIMountService> mountService = IIMountService::Probe(service);
    Slogger::D(TAG, "Setting owner info");
    if (FAILED(mountService->SetField(IStorageManager::OWNER_INFO_KEY, ownerInfo))) {
        Slogger::E(TAG, "Error changing user info");
    }
}

ECode LockPatternUtils::SetOwnerInfo(
    /* [in] */ const String& info,
    /* [in] */ Int32 userId)
{
    SetString(LOCK_SCREEN_OWNER_INFO, info, userId);
    UpdateCryptoUserInfo();
    return NOERROR;
}

ECode LockPatternUtils::SetOwnerInfoEnabled(
    /* [in] */ Boolean enabled)
{
    SetBoolean(LOCK_SCREEN_OWNER_INFO_ENABLED, enabled);
    UpdateCryptoUserInfo();
    return NOERROR;
}

ECode LockPatternUtils::GetOwnerInfo(
    /* [in] */ Int32 userId,
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = GetString(LOCK_SCREEN_OWNER_INFO);
    return NOERROR;
}

ECode LockPatternUtils::IsOwnerInfoEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = GetBoolean(LOCK_SCREEN_OWNER_INFO_ENABLED, FALSE);
    return NOERROR;
}

Int32 LockPatternUtils::ComputePasswordQuality(
    /* [in] */ const String& password)
{
    Boolean hasDigit = FALSE;
    Boolean hasNonDigit = FALSE;
    Int32 len = password.GetLength();
    for (Int32 i = 0; i < len; i++) {
        if (Character::IsDigit(password.GetChar(i))) {
            hasDigit = TRUE;
        }
        else {
            hasNonDigit = TRUE;
        }
    }

    if (hasNonDigit && hasDigit) {
        return IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC;
    }
    if (hasNonDigit) {
        return IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC;
    }
    if (hasDigit) {
        return MaxLengthSequence(password) > MAX_ALLOWED_SEQUENCE
                ? IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC
                : IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX;
    }
    return IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;
}

Int32 LockPatternUtils::CategoryChar(
    /* [in] */ Char32 c)
{
    if ('a' <= c && c <= 'z') return 0;
    if ('A' <= c && c <= 'Z') return 1;
    if ('0' <= c && c <= '9') return 2;
    return 3;
}

Int32 LockPatternUtils::MaxDiffCategory(
    /* [in] */ Int32 category)
{
    if (category == 0 || category == 1) return 1;
    else if (category == 2) return 10;
    return 0;
}

Int32 LockPatternUtils::MaxLengthSequence(
    /* [in] */ const String& string)
{
    if (string.GetLength() == 0) return 0;
    Char32 previousChar = string.GetChar(0);
    Int32 category = CategoryChar(previousChar); //current category of the sequence
    Int32 diff = 0; //difference between two consecutive characters
    Boolean hasDiff = FALSE; //if we are currently targeting a sequence
    Int32 maxLength = 0; //maximum length of a sequence already found
    Int32 startSequence = 0; //where the current sequence started
    for (Int32 current = 1; current < string.GetLength(); current++) {
        Char32 currentChar = string.GetChar(current);
        Int32 categoryCurrent = CategoryChar(currentChar);
        Int32 currentDiff = (Int32) currentChar - (Int32) previousChar;
        if (categoryCurrent != category || Elastos::Core::Math::Abs(currentDiff) > MaxDiffCategory(category)) {
            maxLength = Elastos::Core::Math::Max(maxLength, current - startSequence);
            startSequence = current;
            hasDiff = FALSE;
            category = categoryCurrent;
        }
        else {
            if(hasDiff && currentDiff != diff) {
                maxLength = Elastos::Core::Math::Max(maxLength, current - startSequence);
                startSequence = current - 1;
            }
            diff = currentDiff;
            hasDiff = TRUE;
        }
        previousChar = currentChar;
    }
    maxLength = Elastos::Core::Math::Max(maxLength, string.GetLength() - startSequence);
    return maxLength;
}

void LockPatternUtils::UpdateEncryptionPassword(
    /* [in] */ Int32 type,
    /* [in] */ const String& password)
{
    if (!IsDeviceEncryptionEnabled()) {
        return;
    }
    AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
    if (service == NULL) {
        Slogger::E(TAG, "Could not find the mount service to update the encryption password");
        return;
    }

    AutoPtr<LockAsyncTask> task = new LockAsyncTask(type, password, service);
    task->Execute((ArrayOf<IInterface*>*)NULL);
}

ECode LockPatternUtils::SaveLockPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 quality)
{
    return SaveLockPassword(password, quality, FALSE, GetCurrentOrCallingUserId());
}

ECode LockPatternUtils::SaveLockPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 quality,
    /* [in] */ Boolean isFallback)
{
    return SaveLockPassword(password, quality, isFallback, GetCurrentOrCallingUserId());
}

ECode LockPatternUtils::SaveLockPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 quality,
    /* [in] */ Boolean isFallback,
    /* [in] */ Int32 userHandle)
{
    // try {
    AutoPtr<IDevicePolicyManager> dpm = GetDevicePolicyManager();
    if (!TextUtils::IsEmpty(password)) {
        GetLockSettings()->SetLockPassword(password, userHandle);
        Int32 computedQuality = ComputePasswordQuality(password);

        // Update the device encryption password.
        if (userHandle == IUserHandle::USER_OWNER
                && LockPatternUtils::IsDeviceEncryptionEnabled()) {
            Boolean value = FALSE;
            if (IsCredentialRequiredToDecrypt(TRUE, &value), !value) {
                ClearEncryptionPassword();
            }
            else {
                Boolean numeric = computedQuality
                        == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC;
                Boolean numericComplex = computedQuality
                        == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX;
                Int32 type = numeric || numericComplex ? IStorageManager::CRYPT_TYPE_PIN
                        : IStorageManager::CRYPT_TYPE_PASSWORD;
                UpdateEncryptionPassword(type, password);
            }
        }
        using Elastos::Core::Math;
        if (!isFallback) {
            DeleteGallery();
            SetInt64(PASSWORD_TYPE_KEY, Math::Max(quality, computedQuality), userHandle);
            if (computedQuality != IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
                Int32 letters = 0;
                Int32 uppercase = 0;
                Int32 lowercase = 0;
                Int32 numbers = 0;
                Int32 symbols = 0;
                Int32 nonletter = 0;
                for (Int32 i = 0; i < password.GetLength(); i++) {
                    Char32 c = password.GetChar(i);
                    if (c >= 'A' && c <= 'Z') {
                        letters++;
                        uppercase++;
                    }
                    else if (c >= 'a' && c <= 'z') {
                        letters++;
                        lowercase++;
                    }
                    else if (c >= '0' && c <= '9') {
                        numbers++;
                        nonletter++;
                    }
                    else {
                        symbols++;
                        nonletter++;
                    }
                }
                dpm->SetActivePasswordState(Math::Max(quality, computedQuality),
                        password.GetLength(), letters, uppercase, lowercase,
                        numbers, symbols, nonletter, userHandle);
            }
            else {
                // The password is not anything.
                dpm->SetActivePasswordState(
                        IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED,
                        0, 0, 0, 0, 0, 0, 0, userHandle);
            }
        }
        else {
            // Case where it's a fallback for biometric weak
            SetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK,
                    userHandle);
            SetInt64(PASSWORD_TYPE_ALTERNATE_KEY, Math::Max(quality, computedQuality),
                    userHandle);
            FinishBiometricWeak();
            dpm->SetActivePasswordState(IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK,
                    0, 0, 0, 0, 0, 0, 0, userHandle);
        }
        // Add the password to the password history. We assume all
        // password
        // hashes have the same length for simplicity of implementation.
        String passwordHistory = GetString(PASSWORD_HISTORY_KEY, userHandle);
        if (passwordHistory.IsNullOrEmpty()) {
            passwordHistory = String("");
        }
        Int32 passwordHistoryLength = 0;
        GetRequestedPasswordHistoryLength(&passwordHistoryLength);
        if (passwordHistoryLength == 0) {
            passwordHistory = String("");
        }
        else {
            AutoPtr<ArrayOf<Byte> > hash;
            PasswordToHash(password, userHandle, (ArrayOf<Byte>**)&hash);
            passwordHistory = String((char*)hash->GetPayload()) + "," + passwordHistory;
            // Cut it to contain passwordHistoryLength hashes
            // and passwordHistoryLength -1 commas.
            passwordHistory = passwordHistory.Substring(0, Math::Min(hash->GetLength()
                    * passwordHistoryLength + passwordHistoryLength - 1, passwordHistory
                    .GetLength()));
        }
        SetString(PASSWORD_HISTORY_KEY, passwordHistory, userHandle);
    }
    else {
        // Empty password
        GetLockSettings()->SetLockPassword(String(NULL), userHandle);
        if (userHandle == IUserHandle::USER_OWNER) {
            // Set the encryption password to default.
            UpdateEncryptionPassword(IStorageManager::CRYPT_TYPE_DEFAULT, String(NULL));
        }
        dpm->SetActivePasswordState(
                IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, 0, 0, 0, 0, 0, 0, 0,
                userHandle);
    }

    OnAfterChangingPassword();
    //} catch (RemoteException re) {
    //    // Cant do much
    //    Log.e(TAG, "Unable to save lock password " + re);
    //}
    return NOERROR;
}

Boolean LockPatternUtils::IsDeviceEncrypted()
{
    AutoPtr<IIMountService> mountService = IIMountService::Probe(
            ServiceManager::GetService(String("mount")));

    Int32 state = 0, type = 0;
    if (FAILED(mountService->GetEncryptionState(&state))) {
        Slogger::E(TAG, "Error getting encryption state");
        return TRUE;
    }
    if (FAILED(mountService->GetPasswordType(&type))) {
        Slogger::E(TAG, "Error getting encryption type");
        return TRUE;
    }

    return state != IIMountService::ENCRYPTION_STATE_NONE
            && type != IStorageManager::CRYPT_TYPE_DEFAULT;
}

Boolean LockPatternUtils::IsDeviceEncryptionEnabled()
{
    String status;
    SystemProperties::Get(String("ro.crypto.state"), String("unsupported"), &status);
    return status.EqualsIgnoreCase("encrypted");
}

ECode LockPatternUtils::ClearEncryptionPassword()
{
    UpdateEncryptionPassword(IStorageManager::CRYPT_TYPE_DEFAULT, String(NULL));
    return NOERROR;
}

ECode LockPatternUtils::GetKeyguardStoredPasswordQuality(
    /* [out] */ Int32* result)
{
    return GetKeyguardStoredPasswordQuality(GetCurrentOrCallingUserId(), result);
}

ECode LockPatternUtils::GetKeyguardStoredPasswordQuality(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 quality = (Int32) GetInt64(PASSWORD_TYPE_KEY,
            IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, userHandle);
    // If the user has chosen to use weak biometric sensor, then return the backup locking
    // method and treat biometric as a special case.
    if (quality == IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK) {
        quality = (Int32) GetInt64(PASSWORD_TYPE_ALTERNATE_KEY,
                    IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, userHandle);
    }
    *result = quality;
    return NOERROR;
}

ECode LockPatternUtils::UsingBiometricWeak(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 quality = (Int32) GetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED);
    *result = quality == IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK;
    return NOERROR;
}

ECode LockPatternUtils::StringToPattern(
    /* [in] */ const String& string,
    /* [out] */ IList** list)
{
    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    AutoPtr<ILockPatternViewCellHelper> helper;
    CLockPatternViewCellHelper::AcquireSingleton((ILockPatternViewCellHelper**)&helper);

    Byte size;
    GetLockPatternSize(&size);
    helper->UpdateSize(size);

    AutoPtr<ArrayOf<Byte> > bytes = string.GetBytes();
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        // add 1 when call PatternToString, so minus 1 here
        Byte b = (*bytes)[i] - 1;

        AutoPtr<ILockPatternViewCell> cell;
        FAIL_RETURN(helper->Of(b / size, b % size, size, (ILockPatternViewCell**)&cell));
        result->Add(cell);
    }
    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode LockPatternUtils::PatternToString(
    /* [in] */ IList* pattern,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Byte size;
    GetLockPatternSize(&size);
    return PatternToString(pattern, size, str);
}

ECode LockPatternUtils::PatternToString(
    /* [in] */ IList* pattern,
    /* [in] */ Int32 patternGridSize,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    if (pattern == NULL) {
        *str = String("");
        return NOERROR;
    }
    Int32 patternSize = 0;
    pattern->GetSize(&patternSize);

    AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(patternSize);
    for (Int32 i = 0; i < patternSize; i++) {
        AutoPtr<IInterface> item;
        pattern->Get(i, (IInterface**)&item);
        AutoPtr<ILockPatternViewCell> cell = ILockPatternViewCell::Probe(item);
        Int32 row = 0, column = 0;
        cell->GetRow(&row);
        cell->GetColumn(&column);
        // row and column may be 0, so add 1 to prevent (*res)[i] be 0
        (*res)[i] = (Byte) (row * patternGridSize + column + 1);
    }
    *str = String(*res);
    return NOERROR;
}

ECode LockPatternUtils::PatternToHash(
    /* [in] */ IList* pattern,
    /* [out, callee] */ ArrayOf<Byte>** arr)
{
    if (pattern == NULL) {
        *arr = NULL;
        REFCOUNT_ADD(*arr)
        return NOERROR;
    }

    Int32 patternSize = 0;
    pattern->GetSize(&patternSize);
    AutoPtr<ArrayOf<Byte> > res = ArrayOf<Byte>::Alloc(patternSize);
    for (Int32 i = 0; i < patternSize; i++) {
        AutoPtr<IInterface> item;
        pattern->Get(i, (IInterface**)&item);
        AutoPtr<ILockPatternViewCell> cell = ILockPatternViewCell::Probe(item);
        Int32 row = 0, column = 0;
        cell->GetRow(&row);
        cell->GetColumn(&column);

        Byte size;
        GetLockPatternSize(&size);
        // row and column may be 0, so add 1 to prevent (*res)[i] be 0
        (*res)[i] = (Byte) (row * size + column + 1);
    }

    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    AutoPtr<IMessageDigest> md;
    ECode ec = helper->GetInstance(String("SHA-1"), (IMessageDigest**)&md);
    if (FAILED(ec)) {
        *arr = res;
        REFCOUNT_ADD(*arr)
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > hash;
    ec = md->Digest(res, (ArrayOf<Byte>**)&hash);
    if (FAILED(ec)) {
        *arr = res;
        REFCOUNT_ADD(*arr)
        return NOERROR;
    }

    *arr = hash;
    REFCOUNT_ADD(*arr)
    return NOERROR;
}

ECode LockPatternUtils::GetSalt(
    /* [in] */ Int32 userId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int64 salt = GetInt64(LOCK_PASSWORD_SALT_KEY, 0, userId);
    if (salt == 0) {
        AutoPtr<ISecureRandomHelper> helper;
        CSecureRandomHelper::AcquireSingleton((ISecureRandomHelper**)&helper);
        AutoPtr<ISecureRandom> sr;
        ECode ec = helper->GetInstance(String("SHA1PRNG"), (ISecureRandom**)&sr);
        if (FAILED(ec)) { //NoSuchAlgorithmException
            // Throw an exception rather than storing a password we'll never be able to recover
            Slogger::D(TAG, "Couldn't get SecureRandom number 0x%08x", ec);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        IRandom::Probe(sr)->NextInt64(&salt);
        SetInt64(LOCK_PASSWORD_SALT_KEY, salt, userId);
        Slogger::V(TAG, "Initialized lock password salt for user: %d", userId);
    }
    *result = StringUtils::ToHexString(salt);
    return NOERROR;
}

ECode LockPatternUtils::PasswordToHash(
    /* [in] */ const String& password,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Byte>** arr)
{
    VALIDATE_NOT_NULL(arr);
    *arr = NULL;

    if (password.IsNullOrEmpty()) {
        return NOERROR;
    }

    AutoPtr<IMessageDigestHelper> helper;
    AutoPtr<IMessageDigest> md;
    AutoPtr<ArrayOf<Byte> > saltedPassword;
    AutoPtr<ArrayOf<Byte> > sha1;
    AutoPtr<ArrayOf<Byte> > md5;
    AutoPtr<ArrayOf<Byte> > hashed;
    String algo;
    String salt;
    ECode ec = GetSalt(userId, &salt);
    FAIL_GOTO(ec, EXIT);

    saltedPassword = (password + salt).GetBytes();
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    algo = "SHA-1";
    ec = helper->GetInstance(algo, (IMessageDigest**)&md);
    FAIL_GOTO(ec, EXIT);

    ec = md->Digest(saltedPassword, (ArrayOf<Byte>**)&sha1);
    FAIL_GOTO(ec, EXIT);

    algo = "MD5";
    md = NULL;
    ec = helper->GetInstance(algo, (IMessageDigest**)&md);
    FAIL_GOTO(ec, EXIT);

    ec = md->Digest(saltedPassword, (ArrayOf<Byte>**)&md5);
    FAIL_GOTO(ec, EXIT);

    hashed = (ToHex(sha1) + ToHex(md5)).GetBytes();
    *arr = hashed;
    REFCOUNT_ADD(*arr);
    return NOERROR;

EXIT:
    Slogger::W(TAG, "Failed to encode string because of missing algorithm: %s", algo.string());
    return NOERROR;
}

String LockPatternUtils::ToHex(
    /* [in] */ ArrayOf<Byte>* ary)
{
    const String hex("0123456789ABCDEF");
    String ret("");
    for (Int32 i = 0; i < ary->GetLength(); i++) {
        ret.Append(hex.GetChar(((*ary)[i] >> 4) & 0xf));
        ret.Append(hex.GetChar((*ary)[i] & 0xf));
    }
    return ret;
}

ECode LockPatternUtils::IsLockPasswordEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 mode = GetInt64(PASSWORD_TYPE_KEY, 0);
    Int64 backupMode = GetInt64(PASSWORD_TYPE_ALTERNATE_KEY, 0);
    Boolean passwordEnabled = mode == IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX;
    Boolean backupEnabled = backupMode == IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC
            || backupMode == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC
            || backupMode == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX
            || backupMode == IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC
            || backupMode == IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX;

    Boolean tmp = FALSE;
    SavedPasswordExists(&tmp);
    *result = tmp && (passwordEnabled ||
            ((UsingBiometricWeak(&tmp), tmp) && backupEnabled));
    return NOERROR;
}

ECode LockPatternUtils::IsLockPatternEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean backupEnabled =
            GetInt64(PASSWORD_TYPE_ALTERNATE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED)
            == IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING;

    Boolean tmp = FALSE;
    *result = GetBoolean(ISettingsSecure::LOCK_PATTERN_ENABLED, FALSE)
            && (GetInt64(PASSWORD_TYPE_KEY, IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED)
                    == IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING ||
                    ((UsingBiometricWeak(&tmp), tmp) && backupEnabled));
    return NOERROR;
}

ECode LockPatternUtils::IsBiometricWeakInstalled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    //try {
    AutoPtr<IPackageInfo> info;
    ECode ec = pm->GetPackageInfo(String("com.android.facelock"), IPackageManager::GET_ACTIVITIES, (IPackageInfo**)&info);
    if(FAILED(ec)) {
        return NOERROR;
    }

    Boolean res = FALSE;
    pm->HasSystemFeature(IPackageManager::FEATURE_CAMERA_FRONT, &res);
    if (!res) {
        return NOERROR;
    }
    Boolean cameraDisable = FALSE;
    GetDevicePolicyManager()->GetCameraDisabled(NULL, GetCurrentOrCallingUserId(), &cameraDisable);
    if (cameraDisable) {
        return NOERROR;
    }
    // TODO: If we decide not to proceed with Face Unlock as a trustlet, this must be changed
    // back to returning TRUE.  If we become certain that Face Unlock will be a trustlet, this
    // entire function and a lot of other code can be removed.
    if (DEBUG) Slogger::D(TAG, "Forcing isBiometricWeakInstalled() to return FALSE to disable it");
    return NOERROR;
}

ECode LockPatternUtils::SetBiometricWeakLivelinessEnabled(
    /* [in] */ Boolean enabled)
{
    Int64 currentFlag = GetInt64(ISettingsSecure::LOCK_BIOMETRIC_WEAK_FLAGS, 0L);
    Int64 newFlag = 0;
    if (enabled) {
        newFlag = currentFlag | FLAG_BIOMETRIC_WEAK_LIVELINESS;
    }
    else {
        newFlag = currentFlag & ~FLAG_BIOMETRIC_WEAK_LIVELINESS;
    }
    SetInt64(ISettingsSecure::LOCK_BIOMETRIC_WEAK_FLAGS, newFlag);
    return NOERROR;
}

ECode LockPatternUtils::IsBiometricWeakLivelinessEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    Int64 currentFlag = GetInt64(ISettingsSecure::LOCK_BIOMETRIC_WEAK_FLAGS, 0L);
    *enabled = ((currentFlag & FLAG_BIOMETRIC_WEAK_LIVELINESS) != 0);
    return NOERROR;
}

ECode LockPatternUtils::SetLockPatternEnabled(
    /* [in] */ Boolean enabled)
{
    SetBoolean(ISettingsSecure::LOCK_PATTERN_ENABLED, enabled);

    return NOERROR;
}

ECode LockPatternUtils::IsVisiblePatternEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = GetBoolean(ISettingsSecure::LOCK_PATTERN_VISIBLE, FALSE);
    return NOERROR;
}

ECode LockPatternUtils::SetVisiblePatternEnabled(
    /* [in] */ Boolean enabled)
{
    SetBoolean(ISettingsSecure::LOCK_PATTERN_VISIBLE, enabled);

    // Update for crypto if owner
    Int32 userId = GetCurrentOrCallingUserId();
    if (userId != IUserHandle::USER_OWNER) {
        return NOERROR;
    }

    AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
    if (service == NULL) {
        Slogger::E(TAG, "Could not find the mount service to update the user info");
        return NOERROR;
    }

    AutoPtr<IIMountService> mountService = IIMountService::Probe(service);
    if (FAILED(mountService->SetField(IStorageManager::PATTERN_VISIBLE_KEY, enabled ? String("1") : String("0")))) {
        Slogger::E(TAG, "Error changing pattern visible state");
    }
    return NOERROR;
}

ECode LockPatternUtils::IsTactileFeedbackEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    Int32 value;
    Settings::System::GetInt32ForUser(mContentResolver,
            ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 1, IUserHandle::USER_CURRENT, &value);
    *enabled = value != 0;
    return NOERROR;
}

ECode LockPatternUtils::GetLockPatternSize(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    return GetLockSettings()->GetLockPatternSize(GetCurrentOrCallingUserId(), result);
    // } catch (RemoteException re) {
    //     return PATTERN_SIZE_DEFAULT;
    // }
}

ECode LockPatternUtils::SetLockPatternSize(
    /* [in] */ Int64 size)
{
    SetInt64(ISettingsSecure::LOCK_PATTERN_SIZE, size);
    return NOERROR;
}

ECode LockPatternUtils::SetVisibleDotsEnabled(
    /* [in] */ Boolean enabled)
{
    SetBoolean(ISettingsSecure::LOCK_DOTS_VISIBLE, enabled);
    return NOERROR;
}

ECode LockPatternUtils::IsVisibleDotsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBoolean(ISettingsSecure::LOCK_DOTS_VISIBLE, TRUE);
    return NOERROR;
}

ECode LockPatternUtils::SetShowErrorPath(
    /* [in] */ Boolean enabled)
{
    SetBoolean(ISettingsSecure::LOCK_SHOW_ERROR_PATH, enabled);
    return NOERROR;
}

ECode LockPatternUtils::IsShowErrorPath(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBoolean(ISettingsSecure::LOCK_SHOW_ERROR_PATH, TRUE);
    return NOERROR;
}

ECode LockPatternUtils::SetLockoutAttemptDeadline(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 deadline = SystemClock::GetElapsedRealtime() + FAILED_ATTEMPT_TIMEOUT_MS;
    SetInt64(LOCKOUT_ATTEMPT_DEADLINE, deadline);
    *result = deadline;
    return NOERROR;
}

ECode LockPatternUtils::GetLockoutAttemptDeadline(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 deadline = GetInt64(LOCKOUT_ATTEMPT_DEADLINE, 0L);
    Int64 now = SystemClock::GetElapsedRealtime();
    if (deadline < now || deadline > (now + FAILED_ATTEMPT_TIMEOUT_MS)) {
        *result = 0L;
        return NOERROR;
    }
    *result = deadline;
    return NOERROR;
}

ECode LockPatternUtils::IsPermanentlyLocked(
    /* [out] */ Boolean* locked)
{
    VALIDATE_NOT_NULL(locked);
    *locked = GetBoolean(LOCKOUT_PERMANENT_KEY, FALSE);
    return NOERROR;
}

ECode LockPatternUtils::SetPermanentlyLocked(
    /* [in] */ Boolean locked)
{
    SetBoolean(LOCKOUT_PERMANENT_KEY, locked);

    return NOERROR;
}

ECode LockPatternUtils::IsEmergencyCallCapable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> rs;
    mContext->GetResources((IResources**)&rs);

    return rs->GetBoolean(R::bool_::config_voice_capable, result);
}

ECode LockPatternUtils::IsPukUnlockScreenEnable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> rs;
    mContext->GetResources((IResources**)&rs);

    return rs->GetBoolean(R::bool_::config_enable_puk_unlock_screen, result);
}

ECode LockPatternUtils::IsEmergencyCallEnabledWhileSimLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> rs;
    mContext->GetResources((IResources**)&rs);

    return rs->GetBoolean(R::bool_::config_enable_emergency_call_while_sim_locked, result);
}

ECode LockPatternUtils::GetNextAlarm(
    /* [out] */ IAlarmClockInfo** alarm)
{
    VALIDATE_NOT_NULL(alarm);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarmManager = IAlarmManager::Probe(obj);
    return alarmManager->GetNextAlarmClock(IUserHandle::USER_CURRENT, alarm);
}

Boolean LockPatternUtils::GetBoolean(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Boolean defaultValue,
    /* [in] */ Int32 userId)
{
    Boolean res = FALSE;
    ECode ec = GetLockSettings()->GetBoolean(secureSettingKey, defaultValue,
            userId, &res);
    if (FAILED(ec)) {
        return defaultValue;
    }
    return res;
}

Boolean LockPatternUtils::GetBoolean(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Boolean defaultValue)
{
    return GetBoolean(secureSettingKey, defaultValue, GetCurrentOrCallingUserId());
}

void LockPatternUtils::SetBoolean(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId)
{
    ECode ec= GetLockSettings()->SetBoolean(secureSettingKey, enabled, userId);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Couldn't write Boolean");
    }
}

void LockPatternUtils::SetBoolean(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Boolean enabled)
{
    SetBoolean(secureSettingKey, enabled, GetCurrentOrCallingUserId());
}

ECode LockPatternUtils::GetAppWidgets(
    /* [out, callee] */ ArrayOf<Int32>** widgets)
{
    VALIDATE_NOT_NULL(widgets);
    AutoPtr<ArrayOf<Int32> > ws = GetAppWidgets(IUserHandle::USER_CURRENT);
    *widgets = ws;
    REFCOUNT_ADD(*widgets);
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > LockPatternUtils::GetAppWidgets(
    /* [in] */ Int32 userId)
{
    String appWidgetIdString;
    Settings::Secure::GetStringForUser(mContentResolver, ISettingsSecure::LOCK_SCREEN_APPWIDGET_IDS,
            userId, &appWidgetIdString);
    String delims = String(",");
    if (!appWidgetIdString.IsNullOrEmpty() && appWidgetIdString.GetLength() > 0) {
        AutoPtr< ArrayOf<String> > appWidgetStringIds;
        StringUtils::Split(appWidgetIdString, delims, (ArrayOf<String>**)&appWidgetStringIds);

        AutoPtr< ArrayOf<Int32> > appWidgetIds = ArrayOf<Int32>::Alloc(appWidgetStringIds->GetLength());
        for (Int32 i = 0; i < appWidgetStringIds->GetLength(); i++) {
            String appWidget = (*appWidgetStringIds)[i];
            // try {
            (*appWidgetIds)[i] = StringUtils::ParseInt32(appWidget);
            // } catch (NumberFormatException e) {
            //     Log.d(TAG, "Error when parsing widget id " + appWidget);
            //     return null;
            // }
        }
        return appWidgetIds;
    }
    return NULL;
}

String LockPatternUtils::CombineStrings(
    /* [in] */ ArrayOf<Int32>* list,
    /* [in] */ const String& separator)
{
    Int32 listLength = list->GetLength();

    switch (listLength) {
        case 0: {
            return String("");
        }
        case 1: {
            return StringUtils::ToString((*list)[0]);
        }
    }

    Int32 strLength = 0;
    Int32 separatorLength = separator.GetLength();

    AutoPtr<ArrayOf<String> > stringList = ArrayOf<String>::Alloc(list->GetLength());
    for (Int32 i = 0; i < listLength; i++) {
        (*stringList)[i] = StringUtils::ToString((*list)[i]);
        strLength += (*stringList)[i].GetLength();
        if (i < listLength - 1) {
            strLength += separatorLength;
        }
    }

    StringBuilder sb(strLength);

    for (Int32 i = 0; i < listLength; i++) {
        sb.Append((*list)[i]);
        if (i < listLength - 1) {
            sb.Append(separator);
        }
    }
    String result;
    sb.ToString(&result);
    return result;
}

ECode LockPatternUtils::WriteFallbackAppWidgetId(
    /* [in] */ Int32 appWidgetId)
{
    Boolean result;
    return Settings::Secure::PutInt32ForUser(mContentResolver,
            ISettingsSecure::LOCK_SCREEN_FALLBACK_APPWIDGET_ID,
            appWidgetId, IUserHandle::USER_CURRENT, &result);
}

ECode LockPatternUtils::GetFallbackAppWidgetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    Settings::Secure::GetInt32ForUser(
        mContentResolver, ISettingsSecure::LOCK_SCREEN_FALLBACK_APPWIDGET_ID,
        IAppWidgetManager::INVALID_APPWIDGET_ID, IUserHandle::USER_CURRENT, id);
    return NOERROR;
}

void LockPatternUtils::WriteAppWidgets(
    /* [in] */ ArrayOf<Int32>* appWidgetIds)
{
    Boolean result;
    Settings::Secure::PutStringForUser(mContentResolver,
            ISettingsSecure::LOCK_SCREEN_APPWIDGET_IDS,
            CombineStrings(appWidgetIds, String(",")),
            IUserHandle::USER_CURRENT, &result);
}

ECode LockPatternUtils::AddAppWidget(
    /* [in] */ Int32 widgetId,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<ArrayOf<Int32> > widgets;
    GetAppWidgets((ArrayOf<Int32>**)&widgets);
    if (widgets == NULL) {
        return NOERROR;
    }
    if (index < 0 || index > widgets->GetLength()) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > newWidgets = ArrayOf<Int32>::Alloc(widgets->GetLength()+ 1);
    for (Int32 i = 0, j = 0; i < newWidgets->GetLength(); i++) {
        if (index == i) {
            (*newWidgets)[i] = widgetId;
            i++;
        }
        if (i < newWidgets->GetLength()) {
            (*newWidgets)[i] = (*widgets)[j];
            j++;
        }
    }
    WriteAppWidgets(newWidgets);
    *result = TRUE;
    return NOERROR;
}

ECode LockPatternUtils::RemoveAppWidget(
    /* [in] */ Int32 widgetId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<ArrayOf<Int32> > widgets;
    GetAppWidgets((ArrayOf<Int32>**)&widgets);

    if (widgets->GetLength() == 0) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > newWidgets = ArrayOf<Int32>::Alloc(widgets->GetLength() - 1);
    for (Int32 i = 0, j = 0; i < widgets->GetLength(); i++) {
        if ((*widgets)[i] == widgetId) {
            // continue...
        }
        else if (j >= newWidgets->GetLength()) {
            // we couldn't find the widget
            return NOERROR;
        }
        else {
            (*newWidgets)[j] = (*widgets)[i];
            j++;
        }
    }
    WriteAppWidgets(newWidgets);
    *result = TRUE;
    return NOERROR;
}

Int64 LockPatternUtils::GetInt64(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Int64 defaultValue,
    /* [in] */ Int32 userHandle)
{
    Int64 result = 0;
    ECode ec = GetLockSettings()->GetInt64(secureSettingKey, defaultValue, userHandle, &result);
    if (FAILED(ec)) {
        return defaultValue;
    }
    return result;
}

Int64 LockPatternUtils::GetInt64(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Int64 defaultValue)
{
    Int64 result = 0;
    ECode ec = GetLockSettings()->GetInt64(secureSettingKey, defaultValue,
            GetCurrentOrCallingUserId(), &result);
    if (FAILED(ec)) {
        return defaultValue;
    }
    return result;
}

void LockPatternUtils::SetInt64(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Int64 value)
{
    SetInt64(secureSettingKey, value, GetCurrentOrCallingUserId());
}

void LockPatternUtils::SetInt64(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userHandle)
{
    ECode ec = GetLockSettings()->SetInt64(secureSettingKey, value, userHandle);

    if (FAILED(ec)) {
        Slogger::E(TAG, "Couldn't write Int64 ");
    }
}

String LockPatternUtils::GetString(
    /* [in] */ const String& secureSettingKey)
{
    return GetString(secureSettingKey, GetCurrentOrCallingUserId());
}

String LockPatternUtils::GetString(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ Int32 userHandle)
{
    String result;
    ECode ec = GetLockSettings()->GetString(secureSettingKey, String(""), userHandle, &result);
    if (FAILED(ec)) {
        return String("");
    }
    return result;
}

void LockPatternUtils::SetString(
    /* [in] */ const String& secureSettingKey,
    /* [in] */ const String& value,
    /* [in] */ Int32 userHandle)
{
    ECode ec = GetLockSettings()->SetString(secureSettingKey, value, userHandle);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Couldn't write string ");
    }
}

ECode LockPatternUtils::IsSecure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 m = 0;
    GetKeyguardStoredPasswordQuality(&m);
    Int64 mode = (Int64)m;
    Boolean isPattern = mode == IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING;
    Boolean isPassword = mode == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC
            || mode == IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX;

    Boolean tmp = FALSE;
    Boolean secure = (isPattern && (IsLockPatternEnabled(&tmp), tmp) && (SavedPatternExists(&tmp), tmp))
            || (isPassword && (SavedPasswordExists(&tmp), tmp));

    Int32 data = 0;
    *result = secure && (GetActiveProfileLockMode(&data), data == IProfileLockMode::DEFAULT);
    return NOERROR;
}

ECode LockPatternUtils::GetActiveProfileLockMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    // Check device policy
    AutoPtr<IDevicePolicyManager> dpm = GetDevicePolicyManager();
    Boolean b;
    if (dpm->RequireSecureKeyguard(GetCurrentOrCallingUserId(), &b), b) {
        // Always enforce lock screen
        *result = IProfileLockMode::DEFAULT;
        return NOERROR;
    }
    AutoPtr<IProfile> profile;
    mProfileManager->GetActiveProfile((IProfile**)&profile);
    if (profile == NULL) {
        *result = IProfileLockMode::DEFAULT;
    }
    else {
        profile->GetScreenLockMode(result);
    }
    return NOERROR;
}

ECode LockPatternUtils::UpdateEmergencyCallButtonState(
    /* [in] */ IButton* button,
    /* [in] */ Boolean shown,
    /* [in] */ Boolean showIcon)
{
    Boolean tmp = FALSE;
    if ((IsEmergencyCallCapable(&tmp), tmp) && shown) {
        IView::Probe(button)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(button)->SetVisibility(IView::GONE);
        return NOERROR;
    }

    Int32 textId = 0;
    if (IsInCall(&tmp), tmp) {
        // show "return to call" text and show phone icon
        textId = R::string::lockscreen_return_to_call;
        Int32 phoneCallIcon = showIcon ? R::drawable::stat_sys_phone_call : 0;
        ITextView::Probe(button)->SetCompoundDrawablesWithIntrinsicBounds(phoneCallIcon, 0, 0, 0);
    }
    else {
        textId = R::string::lockscreen_emergency_call;
        Int32 emergencyIcon = showIcon ? R::drawable::ic_emergency : 0;
        ITextView::Probe(button)->SetCompoundDrawablesWithIntrinsicBounds(emergencyIcon, 0, 0, 0);
    }

    ITextView::Probe(button)->SetText(textId);
    return NOERROR;
}

ECode LockPatternUtils::ResumeCall()
{
    return GetTelecommManager()->ShowInCallScreen(FALSE);
}

ECode LockPatternUtils::IsInCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return GetTelecommManager()->IsInCall(result);
}

AutoPtr<ITelecomManager> LockPatternUtils::GetTelecommManager()
{
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    return ITelecomManager::Probe(service);
}

void LockPatternUtils::FinishBiometricWeak()
{
    SetBoolean(BIOMETRIC_WEAK_EVER_CHOSEN_KEY, TRUE);

    // Launch intent to show final screen, this also
    // moves the temporary gallery to the actual gallery
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    //is right?
    assert(0 && "TODO");
    // intent.setClassName("com.android.facelock",
    //         "com.android.facelock.SetupEndScreen");
    intent->SetClassName(String("Elastos.Droid.Core.eco"), String("SetupEndScreen"));
    mContext->StartActivity(intent);
}

ECode LockPatternUtils::SetPowerButtonInstantlyLocks(
    /* [in] */ Boolean enabled)
{
    SetBoolean(LOCKSCREEN_POWER_BUTTON_INSTANTLY_LOCKS, enabled);

    return NOERROR;
}

ECode LockPatternUtils::GetPowerButtonInstantlyLocks(
    /* [out] */ Boolean* locks)
{
    VALIDATE_NOT_NULL(locks);
    *locks = GetBoolean(LOCKSCREEN_POWER_BUTTON_INSTANTLY_LOCKS, TRUE);
    return NOERROR;
}

Boolean LockPatternUtils::IsSafeModeEnabled()
{
    AutoPtr<IInterface> window = ServiceManager::GetService(String("window"));
    AutoPtr<IIWindowManager> manager = IIWindowManager::Probe(window);
    Boolean result = FALSE;
    if(manager) {
        manager->IsSafeModeEnabled(&result);
    }
    return result;
}

ECode LockPatternUtils::HasWidgetsEnabledInKeyguard(
    /* [in] */ Int32 userid,
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    AutoPtr<ArrayOf<Int32> > widgets = GetAppWidgets(userid);
    for (Int32 i = 0; i < widgets->GetLength(); i++) {
        if ((*widgets)[i] > 0) {
            *has = TRUE;
            return NOERROR;
        }
    }
    *has = FALSE;
    return NOERROR;
}

ECode LockPatternUtils::GetWidgetsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    return GetWidgetsEnabled(GetCurrentOrCallingUserId(), enabled);
}

ECode LockPatternUtils::GetWidgetsEnabled(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = GetBoolean(LOCKSCREEN_WIDGETS_ENABLED, FALSE, userId);
    return NOERROR;
}

ECode LockPatternUtils::SetWidgetsEnabled(
    /* [in] */ Boolean enabled)
{
    return SetWidgetsEnabled(enabled, GetCurrentOrCallingUserId());
}

ECode LockPatternUtils::SetWidgetsEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId)
{
    SetBoolean(LOCKSCREEN_WIDGETS_ENABLED, enabled, userId);
    return NOERROR;
}

ECode LockPatternUtils::SetEnabledTrustAgents(
    /* [in] */ ICollection/*<ComponentName>*/* activeTrustAgents)
{
    SetEnabledTrustAgents(activeTrustAgents, GetCurrentOrCallingUserId());
    return NOERROR;
}

ECode LockPatternUtils::GetEnabledTrustAgents(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    return GetEnabledTrustAgents(GetCurrentOrCallingUserId(), list);
}

ECode LockPatternUtils::SetEnabledTrustAgents(
    /* [in] */ ICollection/*<ComponentName>*/* activeTrustAgents,
    /* [in] */ Int32 userId)
{
    StringBuilder sb;
    AutoPtr<IIterator> ator;
    activeTrustAgents->GetIterator((IIterator**)&ator);
    Boolean hasNext = FALSE;
    Int32 len = 0;
    while (ator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IComponentName> cn = IComponentName::Probe(obj);

        if ((sb.GetLength(&len), len) > 0) {
            sb.AppendChar(',');
        }
        String name;
        cn->FlattenToShortString(&name);
        sb.Append(name);
    }
    SetString(ENABLED_TRUST_AGENTS, sb.ToString(), userId);
    GetTrustManager()->ReportEnabledTrustAgentsChanged(GetCurrentOrCallingUserId());
    return NOERROR;
}

ECode LockPatternUtils::GetEnabledTrustAgents(
    /* [in] */ Int32 userId,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    String serialized = GetString(ENABLED_TRUST_AGENTS, userId);
    if (TextUtils::IsEmpty(serialized)) {
        *list = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > split = TextUtils::Split(serialized, String(","));
    AutoPtr<IArrayList> activeTrustAgents;
    CArrayList::New(split->GetLength(), (IArrayList**)&activeTrustAgents);
    for (Int32 i = 0; i < split->GetLength(); i++) {
        String s = (*split)[i];
        if (!TextUtils::IsEmpty(s)) {
            AutoPtr<IComponentName> cn;
            CComponentName::UnflattenFromString(s, (IComponentName**)&cn);
            activeTrustAgents->Add(cn);
        }
    }
    *list = IList::Probe(activeTrustAgents);
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode LockPatternUtils::RequireCredentialEntry(
    /* [in] */ Int32 userId)
{
    return GetTrustManager()->ReportRequireCredentialEntry(userId);
}

void LockPatternUtils::OnAfterChangingPassword()
{
    GetTrustManager()->ReportEnabledTrustAgentsChanged(GetCurrentOrCallingUserId());
}

ECode LockPatternUtils::IsCredentialRequiredToDecrypt(
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 value = 0;
    Settings::Global::GetInt32(mContentResolver,
            ISettingsGlobal::REQUIRE_PASSWORD_TO_DECRYPT, -1, &value);
    *result = value == -1 ? defaultValue : (value != 0);
    return NOERROR;
}

ECode LockPatternUtils::SetCredentialRequiredToDecrypt(
    /* [in] */ Boolean required)
{
    Int32 user = 0;
    if ((GetCurrentUser(&user), user) != IUserHandle::USER_OWNER) {
        Slogger::W(TAG, "Only device owner may call setCredentialRequiredForDecrypt()");
        return NOERROR;
    }
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean result = FALSE;
    Settings::Global::PutInt32(resolver,
            ISettingsGlobal::REQUIRE_PASSWORD_TO_DECRYPT, required ? 1 : 0, &result);
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
