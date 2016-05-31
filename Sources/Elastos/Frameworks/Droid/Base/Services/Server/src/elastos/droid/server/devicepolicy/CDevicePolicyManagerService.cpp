
#include "elastos/droid/server/devicepolicy/CDevicePolicyManagerService.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/UserHandle.h"
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.Droid.AccessibilityService.h>
#include <Elastos.Droid.Accounts.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.KeyStore.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/R.h>
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/app/AppGlobals.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/server/LocalServices.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

// using Elastos::Droid::App::Admin::CDeviceAdminInfo;
// using Elastos::Droid::KeyStore::Security::CKeyChain;
// using Elastos::Droid::KeyStore::Security::CKeyStoreHelper;
// using Elastos::Droid::Os::CEnvironment;
// using Elastos::Droid::Os::CServiceManager;
// using Elastos::Droid::Utility::CPrintWriterPrinter;
#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::EIID_IDevicePolicyManagerInternal;
using Elastos::Droid::App::Admin::EIID_IIDevicePolicyManager;
using Elastos::Droid::App::Admin::IDeviceAdminInfoPolicyInfo;
using Elastos::Droid::App::Admin::IDeviceAdminReceiver;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Internal::Os::Storage::IExternalStorageFormatter;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::CJournaledFile;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Droid::Internal::Utility::IJournaledFile;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
using Elastos::Droid::KeyStore::Security::IIKeyChainService;
using Elastos::Droid::KeyStore::Security::IKeyChain;
using Elastos::Droid::KeyStore::Security::IKeyChainConnection;
using Elastos::Droid::KeyStore::Security::IKeyStore;
using Elastos::Droid::KeyStore::Security::IKeyStoreHelper;
using Elastos::Droid::Manifest;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioService;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Net::CProxyInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CRecoverySystem;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IRecoverySystem;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::View::InputMethod::IInputMethodInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::Math;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFile;;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Text::IDateFormat;
using Elastos::Text::IDateFormatHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CDate;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace DevicePolicy {

//=============================================================================
// CDevicePolicyManagerService::RemoveWarningReceiver
//=============================================================================
CDevicePolicyManagerService::RemoveWarningReceiver::RemoveWarningReceiver(
    /* [in] */ IRemoteCallback* result)
    : mResult(result)
{}

ECode CDevicePolicyManagerService::RemoveWarningReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IBundle> data;
    GetResultExtras(FALSE, (IBundle**)&data);
    return mResult->SendResult(data);
}

ECode CDevicePolicyManagerService::RemoveWarningReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)

    *info = "CDevicePolicyManagerService::RemoveWarningReceiver: ";
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::ActiveAdminLockedReceiver
//=============================================================================
CDevicePolicyManagerService::ActiveAdminLockedReceiver::ActiveAdminLockedReceiver(
    /* [in] */ CDevicePolicyManagerService* host,
    /* [in] */ ActiveAdmin* admin,
    /* [in] */ IComponentName* adminReceiver)
    : mHost(host)
    , mAdmin(admin)
    , mAdminReceiver(adminReceiver)
{}

ECode CDevicePolicyManagerService::ActiveAdminLockedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    {    AutoLock syncLock(mHost);
        AutoPtr<IUserHandle> uHandle = mAdmin->GetUserHandle();
        Int32 userHandle;
        uHandle->GetIdentifier(&userHandle);

        AutoPtr<DevicePolicyData> policy = mHost->GetUserData(userHandle);
        Boolean doProxyCleanup;
        mAdmin->mInfo->UsesPolicy(IDeviceAdminInfo::USES_POLICY_SETS_GLOBAL_PROXY, &doProxyCleanup);

        policy->mAdminList->Remove(TO_IINTERFACE(mAdmin));
        policy->mAdminMap->Remove(mAdminReceiver);
        mHost->ValidatePasswordOwnerLocked(policy);
        mHost->SyncDeviceCapabilitiesLocked(policy);
        if (doProxyCleanup) {
            mHost->ResetGlobalProxyLocked(mHost->GetUserData(userHandle));
        }
        mHost->SaveSettingsLocked(userHandle);
        mHost->UpdateMaximumTimeToLockLocked(policy);
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::ActiveAdminLockedReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)

    *info = "CDevicePolicyManagerService::ActiveAdminLockedReceiver: ";
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::DevicePolicyReceiver
//=============================================================================
CDevicePolicyManagerService::DevicePolicyReceiver::DevicePolicyReceiver(
    /* [in] */ CDevicePolicyManagerService* host)
    : mHost(host)
{}

ECode CDevicePolicyManagerService::DevicePolicyReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    Int32 id;
    GetSendingUserId(&id);
    Int32 userHandle;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, id, &userHandle);

    if (action.Equals(IIntent::ACTION_BOOT_COMPLETED) || action.Equals(ACTION_EXPIRED_PASSWORD_NOTIFICATION)) {
        StringBuffer buf;
        buf += "Sending password expiration notifications for action ";
        buf += action;
        buf += " for user ";
        buf += userHandle;
        if (DBG) Slogger::V(LOG__TAG, buf.ToString());

        AutoPtr<IRunnable> runnable = new CDevicePolicyManagerService::DevicePolicyReceiverRunnable(mHost, userHandle);
        Boolean result;
        mHost->mHandler->Post(runnable, &result);
    }
    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)
            || IKeyChain::ACTION_STORAGE_CHANGED.Equals(action)) {
        AutoPtr<MonitoringCertNotificationTask> task = new MonitoringCertNotificationTask(mHost);
        AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
        params->Set(0, intent);
        task->Execute(params);
    }
    if (IIntent::ACTION_USER_REMOVED.Equals(action)) {
        mHost->RemoveUserData(userHandle);
    } else if (action.Equals(IIntent::ACTION_USER_STARTED)
            || action.Equals(IIntent::ACTION_PACKAGE_CHANGED)
            || action.Equals(IIntent::ACTION_PACKAGE_REMOVED)
            || action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
        if (action.Equals(IIntent::ACTION_USER_STARTED)) {
            // Reset the policy data
            {    AutoLock syncLock(mHost);
                mHost->mUserData->Remove(userHandle);
            }
        }

        mHost->HandlePackagesChanged(userHandle);
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::DevicePolicyReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)

    *info = "CDevicePolicyManagerService::DevicePolicyReceiver: ";
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::DevicePolicyReceiverRunnable
//=============================================================================
CAR_INTERFACE_IMPL(CDevicePolicyManagerService::DevicePolicyReceiverRunnable, Object, IRunnable)

CDevicePolicyManagerService::DevicePolicyReceiverRunnable::DevicePolicyReceiverRunnable(
    /* [in] */ CDevicePolicyManagerService* host,
    /* [in] */ Int32 userHandle)
    : mHost(host)
    , mUserHandle(userHandle)
{}

ECode CDevicePolicyManagerService::DevicePolicyReceiverRunnable::Run()
{
    mHost->HandlePasswordExpirationNotification(mUserHandle);
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::DeviceOrUserLockedRunnable
//=============================================================================
CAR_INTERFACE_IMPL(CDevicePolicyManagerService::DeviceOrUserLockedRunnable, Object, IRunnable)

CDevicePolicyManagerService::DeviceOrUserLockedRunnable::DeviceOrUserLockedRunnable(
    /* [in] */ CDevicePolicyManagerService* host,
    /* [in] */ Int32 userHandle)
    : mHost(host)
    , mUserHandle(userHandle)
{}

ECode CDevicePolicyManagerService::DeviceOrUserLockedRunnable::Run()
{
    // try {
    ECode ec;
    do {
        AutoPtr<IIActivityManager> activityMgr = ActivityManagerNative::GetDefault();
        Boolean result;
        ec = activityMgr->SwitchUser(IUserHandle::USER_OWNER, &result);
        if (FAILED(ec)) break;

        ec = mHost->mUserManager->RemoveUser(mUserHandle, &result);
        if (FAILED(ec)) break;
        if (!result) {
            Slogger::W(LOG__TAG, "Couldn't remove user %d", mUserHandle);
        }
    } while(FALSE);
    // } catch (RemoteException re) {
    if ((ECode) E_REMOTE_EXCEPTION == ec) {
        // Shouldn't happen
        return NOERROR;
    }
    // }
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::DevicePolicyData
//=============================================================================
CDevicePolicyManagerService::DevicePolicyData::DevicePolicyData(
    /* [in] */ Int32 userHandle)
    : mActivePasswordQuality(IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED)
    , mActivePasswordLength(0)
    , mActivePasswordUpperCase(0)
    , mActivePasswordLowerCase(0)
    , mActivePasswordLetters(0)
    , mActivePasswordNumeric(0)
    , mActivePasswordSymbols(0)
    , mActivePasswordNonLetter(0)
    , mFailedPasswordAttempts(0)
    , mUserHandle(userHandle)
    , mPasswordOwner(-1)
    , mLastMaximumTimeToLock(-1)
    , mUserSetupComplete(FALSE)
{
    CArrayList::New((IList**)&mLockTaskPackages);
}

//=============================================================================
// CDevicePolicyManagerService::ActiveAdmin
//=============================================================================
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_LENGTH = 0;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_PASSWORD_HISTORY_LENGTH = 0;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_UPPER_CASE = 0;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_LOWER_CASE = 0;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_LETTERS = 1;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_NUMERIC = 1;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_SYMBOLS = 1;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MINIMUM_PASSWORD_NON_LETTER = 0;
const Int64 CDevicePolicyManagerService::ActiveAdmin::DEF_MAXIMUM_TIME_TO_UNLOCK = 0;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE = 0;
const Int64 CDevicePolicyManagerService::ActiveAdmin::DEF_PASSWORD_EXPIRATION_TIMEOUT = 0;
const Int64 CDevicePolicyManagerService::ActiveAdmin::DEF_PASSWORD_EXPIRATION_DATE = 0;
const Int32 CDevicePolicyManagerService::ActiveAdmin::DEF_KEYGUARD_FEATURES_DISABLED = 0; // none

const String CDevicePolicyManagerService::ActiveAdmin::TAG_DISABLE_KEYGUARD_FEATURES("disable-keyguard-features");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_DISABLE_CAMERA("disable-camera");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_DISABLE_CALLER_ID("disable-caller-id");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_DISABLE_SCREEN_CAPTURE("disable-screen-capture");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_DISABLE_ACCOUNT_MANAGEMENT("disable-account-management");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_REQUIRE_AUTO_TIME("require_auto_time");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_ACCOUNT_TYPE("account-type");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PERMITTED_ACCESSIBILITY_SERVICES("permitted-accessiblity-services");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_ENCRYPTION_REQUESTED("encryption-requested");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MANAGE_TRUST_AGENT_FEATURES("manage-trust-agent-features");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_TRUST_AGENT_FEATURE("feature");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_TRUST_AGENT_COMPONENT("component");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PASSWORD_EXPIRATION_DATE("password-expiration-date");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PASSWORD_EXPIRATION_TIMEOUT("password-expiration-timeout");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_GLOBAL_PROXY_EXCLUSION_LIST("global-proxy-exclusion-list");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_GLOBAL_PROXY_SPEC("global-proxy-spec");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_SPECIFIES_GLOBAL_PROXY("specifies-global-proxy");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PERMITTED_IMES("permitted-imes");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MAX_FAILED_PASSWORD_WIPE("max-failed-password-wipe");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MAX_TIME_TO_UNLOCK("max-time-to-unlock");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_NONLETTER("min-password-nonletter");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_SYMBOLS("min-password-symbols");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_NUMERIC("min-password-numeric");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_LETTERS("min-password-letters");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_LOWERCASE("min-password-lowercase");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_UPPERCASE("min-password-uppercase");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PASSWORD_HISTORY_LENGTH("password-history-length");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_MIN_PASSWORD_LENGTH("min-password-length");
const String CDevicePolicyManagerService::ActiveAdmin::ATTR_VALUE("value");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PASSWORD_QUALITY("password-quality");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_POLICIES("policies");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_CROSS_PROFILE_WIDGET_PROVIDERS("cross-profile-widget-providers");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PROVIDER("provider");
const String CDevicePolicyManagerService::ActiveAdmin::TAG_PACKAGE_LIST_ITEM("item");

CDevicePolicyManagerService::ActiveAdmin::ActiveAdmin(
    /* [in] */ IDeviceAdminInfo* info)
    : mInfo(info)
    , mPasswordQuality(IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED)
    , mMinimumPasswordLength(DEF_MINIMUM_PASSWORD_LENGTH)
    , mPasswordHistoryLength(DEF_PASSWORD_HISTORY_LENGTH)
    , mMinimumPasswordUpperCase(DEF_MINIMUM_PASSWORD_UPPER_CASE)
    , mMinimumPasswordLowerCase(DEF_MINIMUM_PASSWORD_LOWER_CASE)
    , mMinimumPasswordLetters(DEF_MINIMUM_PASSWORD_LETTERS)
    , mMinimumPasswordNumeric(DEF_MINIMUM_PASSWORD_NUMERIC)
    , mMinimumPasswordSymbols(DEF_MINIMUM_PASSWORD_SYMBOLS)
    , mMinimumPasswordNonLetter(DEF_MINIMUM_PASSWORD_NON_LETTER)
    , mMaximumTimeToUnlock(DEF_MAXIMUM_TIME_TO_UNLOCK)
    , mMaximumFailedPasswordsForWipe(DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE)
    , mPasswordExpirationTimeout(DEF_PASSWORD_EXPIRATION_TIMEOUT)
    , mPasswordExpirationDate(DEF_PASSWORD_EXPIRATION_DATE)
    , mDisabledKeyguardFeatures(DEF_KEYGUARD_FEATURES_DISABLED)
    , mEncryptionRequested(FALSE)
    , mDisableCamera(FALSE)
    , mDisableCallerId(FALSE)
    , mDisableScreenCapture(FALSE)
    , mRequireAutoTime(FALSE)
    , mSpecifiesGlobalProxy(FALSE)
    , mGlobalProxySpec(NULL)
    , mGlobalProxyExclusionList(NULL)
{
    CHashSet::New((ISet**)&mAccountTypesWithManagementDisabled);
    CHashMap::New((IHashMap**)&mTrustAgentFeatures);
}

Int32 CDevicePolicyManagerService::ActiveAdmin::GetUid()
{
    AutoPtr<IActivityInfo> activityInfo;
    mInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 uid;
    appInfo->GetUid(&uid);

    return uid;
}

AutoPtr<IUserHandle> CDevicePolicyManagerService::ActiveAdmin::GetUserHandle()
{
    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    Int32 userId;
    handleHelper->GetUserId(GetUid(), &userId);

    AutoPtr<IUserHandle> handle;
    CUserHandle::New(userId, (IUserHandle**)&handle);
    return handle;
}

void CDevicePolicyManagerService::ActiveAdmin::WriteToXml(
    /* [in] */ IXmlSerializer* out)
{
    out->WriteStartTag(String(NULL), TAG_POLICIES);
    mInfo->WritePoliciesToXml(out);
    out->WriteEndTag(String(NULL), TAG_POLICIES);
    if (mPasswordQuality != IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
        out->WriteStartTag(String(NULL), TAG_PASSWORD_QUALITY);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mPasswordQuality));
        out->WriteEndTag(String(NULL), TAG_PASSWORD_QUALITY);
        if (mMinimumPasswordLength != DEF_MINIMUM_PASSWORD_LENGTH) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_LENGTH);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordLength));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_LENGTH);
        }
        if(mPasswordHistoryLength != DEF_PASSWORD_HISTORY_LENGTH) {
            out->WriteStartTag(String(NULL), TAG_PASSWORD_HISTORY_LENGTH);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mPasswordHistoryLength));
            out->WriteEndTag(String(NULL), TAG_PASSWORD_HISTORY_LENGTH);
        }
        if (mMinimumPasswordUpperCase != DEF_MINIMUM_PASSWORD_UPPER_CASE) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_UPPERCASE);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordUpperCase));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_UPPERCASE);
        }
        if (mMinimumPasswordLowerCase != DEF_MINIMUM_PASSWORD_LOWER_CASE) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_LOWERCASE);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordLowerCase));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_LOWERCASE);
        }
        if (mMinimumPasswordLetters != DEF_MINIMUM_PASSWORD_LETTERS) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_LETTERS);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordLetters));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_LETTERS);
        }
        if (mMinimumPasswordNumeric != DEF_MINIMUM_PASSWORD_NUMERIC) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_NUMERIC);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordNumeric));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_NUMERIC);
        }
        if (mMinimumPasswordSymbols != DEF_MINIMUM_PASSWORD_SYMBOLS) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_SYMBOLS);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordSymbols));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_SYMBOLS);
        }
        if (mMinimumPasswordNonLetter > DEF_MINIMUM_PASSWORD_NON_LETTER) {
            out->WriteStartTag(String(NULL), TAG_MIN_PASSWORD_NONLETTER);
            out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMinimumPasswordNonLetter));
            out->WriteEndTag(String(NULL), TAG_MIN_PASSWORD_NONLETTER);
        }
    }
    if (mMaximumTimeToUnlock != DEF_MAXIMUM_TIME_TO_UNLOCK) {
        out->WriteStartTag(String(NULL), TAG_MAX_TIME_TO_UNLOCK);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMaximumTimeToUnlock));
        out->WriteEndTag(String(NULL), TAG_MAX_TIME_TO_UNLOCK);
    }
    if (mMaximumFailedPasswordsForWipe != DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE) {
        out->WriteStartTag(String(NULL), TAG_MAX_FAILED_PASSWORD_WIPE);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mMaximumFailedPasswordsForWipe));
        out->WriteEndTag(String(NULL), TAG_MAX_FAILED_PASSWORD_WIPE);
    }
    if (mSpecifiesGlobalProxy) {
        out->WriteStartTag(String(NULL), TAG_SPECIFIES_GLOBAL_PROXY);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::BooleanToString(mSpecifiesGlobalProxy));
        out->WriteEndTag(String(NULL), TAG_SPECIFIES_GLOBAL_PROXY);
        if (!mGlobalProxySpec.IsNull()) {
            out->WriteStartTag(String(NULL), TAG_GLOBAL_PROXY_SPEC);
            out->WriteAttribute(String(NULL), ATTR_VALUE, mGlobalProxySpec);
            out->WriteEndTag(String(NULL), TAG_GLOBAL_PROXY_SPEC);
        }
        if (!mGlobalProxyExclusionList.IsNull()) {
            out->WriteStartTag(String(NULL), TAG_GLOBAL_PROXY_EXCLUSION_LIST);
            out->WriteAttribute(String(NULL), ATTR_VALUE, mGlobalProxyExclusionList);
            out->WriteEndTag(String(NULL), TAG_GLOBAL_PROXY_EXCLUSION_LIST);
        }
    }
    if (mPasswordExpirationTimeout != DEF_PASSWORD_EXPIRATION_TIMEOUT) {
        out->WriteStartTag(String(NULL), TAG_PASSWORD_EXPIRATION_TIMEOUT);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mPasswordExpirationTimeout));
        out->WriteEndTag(String(NULL), TAG_PASSWORD_EXPIRATION_TIMEOUT);
    }
    if (mPasswordExpirationDate != DEF_PASSWORD_EXPIRATION_DATE) {
        out->WriteStartTag(String(NULL), TAG_PASSWORD_EXPIRATION_DATE);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mPasswordExpirationDate));
        out->WriteEndTag(String(NULL), TAG_PASSWORD_EXPIRATION_DATE);
    }
    if (mEncryptionRequested) {
        out->WriteStartTag(String(NULL), TAG_ENCRYPTION_REQUESTED);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::BooleanToString(mEncryptionRequested));
        out->WriteEndTag(String(NULL), TAG_ENCRYPTION_REQUESTED);
    }
    if (mDisableCamera) {
        out->WriteStartTag(String(NULL), TAG_DISABLE_CAMERA);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::BooleanToString(mDisableCamera));
        out->WriteEndTag(String(NULL), TAG_DISABLE_CAMERA);
    }
    if (mDisableCallerId) {
        out->WriteStartTag(String(NULL), TAG_DISABLE_CALLER_ID);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mDisableCallerId));
        out->WriteEndTag(String(NULL), TAG_DISABLE_CALLER_ID);
    }
    if (mDisableScreenCapture) {
        out->WriteStartTag(String(NULL), TAG_DISABLE_SCREEN_CAPTURE);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mDisableScreenCapture));
        out->WriteEndTag(String(NULL), TAG_DISABLE_SCREEN_CAPTURE);
    }
    if (mRequireAutoTime) {
        out->WriteStartTag(String(NULL), TAG_REQUIRE_AUTO_TIME);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mRequireAutoTime));
        out->WriteEndTag(String(NULL), TAG_REQUIRE_AUTO_TIME);
    }
    if (mDisabledKeyguardFeatures != DEF_KEYGUARD_FEATURES_DISABLED) {
        out->WriteStartTag(String(NULL), TAG_DISABLE_KEYGUARD_FEATURES);
        out->WriteAttribute(String(NULL), ATTR_VALUE, StringUtils::ToString(mDisabledKeyguardFeatures));
        out->WriteEndTag(String(NULL), TAG_DISABLE_KEYGUARD_FEATURES);
    }
    Boolean isEmpty;
    mAccountTypesWithManagementDisabled->IsEmpty(&isEmpty);
    if (!isEmpty) {
        out->WriteStartTag(String(NULL), TAG_DISABLE_ACCOUNT_MANAGEMENT);
        AutoPtr<IIterator> it;
        ICollection::Probe(mAccountTypesWithManagementDisabled)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String ac;
            IObject::Probe(obj)->ToString(&ac);
            out->WriteStartTag(String(NULL), TAG_ACCOUNT_TYPE);
            out->WriteAttribute(String(NULL), ATTR_VALUE, ac);
            out->WriteEndTag(String(NULL), TAG_ACCOUNT_TYPE);
        }
        out->WriteEndTag(String(NULL),  TAG_DISABLE_ACCOUNT_MANAGEMENT);
    }
    mTrustAgentFeatures->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<ISet> set;
        mTrustAgentFeatures->GetEntrySet((ISet**)&set);
        out->WriteStartTag(String(NULL), TAG_MANAGE_TRUST_AGENT_FEATURES);
        AutoPtr<IIterator> it;
        ICollection::Probe(set)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> component = IMapEntry::Probe(obj);
            out->WriteStartTag(String(NULL), TAG_TRUST_AGENT_COMPONENT);
            obj = NULL;
            component->GetKey((IInterface**)&obj);
            out->WriteAttribute(String(NULL), ATTR_VALUE, TO_STR(obj));
            obj = NULL;
            component->GetValue((IInterface**)&obj);
            AutoPtr<IIterator> iter;
            ICollection::Probe(obj)->GetIterator((IIterator**)&iter);
            Boolean hasNext;
            while (iter->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj2;
                iter->GetNext((IInterface**)&obj2);
                String feature;
                IObject::Probe(obj2)->ToString(&feature);
                out->WriteStartTag(String(NULL), TAG_TRUST_AGENT_FEATURE);
                out->WriteAttribute(String(NULL), ATTR_VALUE, feature);
                out->WriteEndTag(String(NULL), TAG_TRUST_AGENT_FEATURE);
            }
            out->WriteEndTag(String(NULL), TAG_TRUST_AGENT_COMPONENT);
        }
        out->WriteEndTag(String(NULL), TAG_MANAGE_TRUST_AGENT_FEATURES);
    }
    mCrossProfileWidgetProviders->IsEmpty(&isEmpty);
    if (mCrossProfileWidgetProviders != NULL && !isEmpty) {
        out->WriteStartTag(String(NULL), TAG_CROSS_PROFILE_WIDGET_PROVIDERS);
        Int32 providerCount;
        mCrossProfileWidgetProviders->GetSize(&providerCount);
        for (Int32 i = 0; i < providerCount; i++) {
            AutoPtr<IInterface> obj;
            mCrossProfileWidgetProviders->Get(i, (IInterface**)&obj);
            String provider;
            IObject::Probe(obj)->ToString(&provider);
            out->WriteStartTag(String(NULL), TAG_PROVIDER);
            out->WriteAttribute(String(NULL), ATTR_VALUE, provider);
            out->WriteEndTag(String(NULL), TAG_PROVIDER);
        }
        out->WriteEndTag(String(NULL), TAG_CROSS_PROFILE_WIDGET_PROVIDERS);
    }
    WritePackageListToXml(out, TAG_PERMITTED_ACCESSIBILITY_SERVICES,
            mPermittedAccessiblityServices);
    WritePackageListToXml(out, TAG_PERMITTED_IMES, mPermittedInputMethods);
}

ECode CDevicePolicyManagerService::ActiveAdmin::WritePackageListToXml(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& outerTag,
    /* [in] */ IList* packageList)
{
    if (packageList == NULL) {
        return NOERROR;
    }

    out->WriteStartTag(String(NULL), outerTag);
    AutoPtr<IIterator> it;
    ICollection::Probe(packageList)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String packageName;
        IObject::Probe(obj)->ToString(&packageName);
        out->WriteStartTag(String(NULL), TAG_PACKAGE_LIST_ITEM);
        out->WriteAttribute(String(NULL), ATTR_VALUE, packageName);
        out->WriteEndTag(String(NULL), TAG_PACKAGE_LIST_ITEM);
    }
    out->WriteEndTag(String(NULL), outerTag);
    return NOERROR;
}

void CDevicePolicyManagerService::ActiveAdmin::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type;
    parser->Next(&type);
    Int32 nextDepth;
    parser->GetDepth(&nextDepth);
    while (type != IXmlPullParser::END_DOCUMENT
           && (type != IXmlPullParser::END_TAG || nextDepth > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tag;
        parser->GetName(&tag);
        if (TAG_POLICIES.Equals(tag)) {
            mInfo->ReadPoliciesFromXml(parser);
        } else if (TAG_PASSWORD_QUALITY.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mPasswordQuality = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_LENGTH.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordLength = StringUtils::ParseInt32(value);
        } else if (TAG_PASSWORD_HISTORY_LENGTH.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mPasswordHistoryLength = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_UPPERCASE.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordUpperCase = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_LOWERCASE.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordLowerCase = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_LETTERS.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordLetters = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_NUMERIC.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordNumeric = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_SYMBOLS.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordSymbols = StringUtils::ParseInt32(value);
        } else if (TAG_MIN_PASSWORD_NONLETTER.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMinimumPasswordNonLetter = StringUtils::ParseInt32(value);
        } else if (TAG_MAX_TIME_TO_UNLOCK.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMaximumTimeToUnlock = StringUtils::ParseInt64(value);
        } else if (TAG_MAX_FAILED_PASSWORD_WIPE.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mMaximumFailedPasswordsForWipe = StringUtils::ParseInt32(value);
        } else if (TAG_SPECIFIES_GLOBAL_PROXY.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mSpecifiesGlobalProxy = StringUtils::ParseInt32(value);
        } else if (TAG_GLOBAL_PROXY_SPEC.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mGlobalProxySpec = value;
        } else if (TAG_GLOBAL_PROXY_EXCLUSION_LIST.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mGlobalProxyExclusionList = value;
        } else if (TAG_PASSWORD_EXPIRATION_TIMEOUT.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mPasswordExpirationTimeout = StringUtils::ParseInt64(value);
        } else if (TAG_PASSWORD_EXPIRATION_DATE.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mPasswordExpirationDate = StringUtils::ParseInt64(value);
        } else if (TAG_ENCRYPTION_REQUESTED.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mEncryptionRequested = StringUtils::ParseInt32(value);
        } else if (TAG_DISABLE_CAMERA.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mDisableCamera = StringUtils::ParseInt32(value);
        } else if (TAG_DISABLE_CALLER_ID.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mDisableCallerId = StringUtils::ParseBoolean(value);
        } else if (TAG_DISABLE_SCREEN_CAPTURE.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mDisableScreenCapture = StringUtils::ParseBoolean(value);
        } else if (TAG_REQUIRE_AUTO_TIME.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mRequireAutoTime= StringUtils::ParseBoolean(value);
        } else if (TAG_DISABLE_KEYGUARD_FEATURES.Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &value);
            mDisabledKeyguardFeatures = StringUtils::ParseInt32(value);
        } else if (TAG_DISABLE_ACCOUNT_MANAGEMENT.Equals(tag)) {
            ReadDisableAccountInfo(parser, tag, (ISet**)&mAccountTypesWithManagementDisabled);
        } else if (TAG_MANAGE_TRUST_AGENT_FEATURES.Equals(tag)) {
            GetAllTrustAgentFeatures(parser, tag, (IHashMap**)&mTrustAgentFeatures);
        } else if (TAG_CROSS_PROFILE_WIDGET_PROVIDERS.Equals(tag)) {
            GetCrossProfileWidgetProviders(parser, tag, (IList**)&mCrossProfileWidgetProviders);
        } else if (TAG_PERMITTED_ACCESSIBILITY_SERVICES.Equals(tag)) {
            ReadPackageList(parser, tag, (IList**)&mPermittedAccessiblityServices);
        } else if (TAG_PERMITTED_IMES.Equals(tag)) {
            ReadPackageList(parser, tag, (IList**)&mPermittedInputMethods);
        } else {
            Slogger::W(LOG__TAG, "Unknown admin tag: %s", tag.string());
        }
        XmlUtils::SkipCurrentTag(parser);
    }
}

ECode CDevicePolicyManagerService::ActiveAdmin::ReadPackageList(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tag,
    /* [out] */ IList** packageList)
{
    VALIDATE_NOT_NULL(packageList)

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 outerType;
    Int32 outerDepthNow;
    while (parser->Next(&outerType), parser->GetDepth(&outerDepthNow),
            outerType != IXmlPullParser::END_DOCUMENT
            && (outerType != IXmlPullParser::END_TAG || outerDepthNow > outerDepth)) {
        if (outerType == IXmlPullParser::END_TAG || outerType == IXmlPullParser::TEXT) {
            continue;
        }
        String outerTag;
        parser->GetName(&outerTag);
        if (TAG_PACKAGE_LIST_ITEM.Equals(outerTag)) {
            String packageName;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &packageName);
            if (packageName != NULL) {
                result->Add(StringUtils::ParseCharSequence(packageName));
            } else {
                Slogger::W(LOG__TAG, "Package name missing under %s", outerTag.string());
            }
        } else {
            Slogger::W(LOG__TAG, "Unknown tag under %s: %s", tag.string(), outerTag.string());
        }
    }
    *packageList = result;
    REFCOUNT_ADD(*packageList)
    return NOERROR;
}

ECode CDevicePolicyManagerService::ActiveAdmin::ReadDisableAccountInfo(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tag,
    /* [out] */ ISet** accountInfo)
{
    VALIDATE_NOT_NULL(accountInfo)

    Int32 outerDepthDAM;
    parser->GetDepth(&outerDepthDAM);
    Int32 typeDAM;
    AutoPtr<ISet> result;
    CHashSet::New((ISet**)&result);
    Int32 outerDepthNow;
    while (parser->Next(&typeDAM), parser->GetDepth(&outerDepthNow), typeDAM != IXmlPullParser::END_DOCUMENT
            && (typeDAM != IXmlPullParser::END_TAG || outerDepthNow > outerDepthDAM)) {
        if (typeDAM == IXmlPullParser::END_TAG || typeDAM == IXmlPullParser::TEXT) {
            continue;
        }
        String tagDAM;
        parser->GetName(&tagDAM);
        if (TAG_ACCOUNT_TYPE.Equals(tagDAM)) {
            String attrValue;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &attrValue);
            result->Add(StringUtils::ParseCharSequence(attrValue));
        } else {
            Slogger::W(LOG__TAG, "Unknown tag under %s: %s", tag.string(), tagDAM.string());
        }
    }
    *accountInfo = result;
    REFCOUNT_ADD(*accountInfo)
    return NOERROR;
}

ECode CDevicePolicyManagerService::ActiveAdmin::GetAllTrustAgentFeatures(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tag,
    /* [out] */ IHashMap** features)
{
    VALIDATE_NOT_NULL(features)

    Int32 outerDepthDAM;
    parser->GetDepth(&outerDepthDAM);
    Int32 typeDAM;
    // HashMap<String, List<String> >
    AutoPtr<IHashMap> result;
    CHashMap::New((IHashMap**)&result);
    Int32 parserDepthNow;
    while ((parser->Next(&typeDAM), parser->GetDepth(&parserDepthNow),
            typeDAM != IXmlPullParser::END_DOCUMENT)
        && (typeDAM != IXmlPullParser::END_TAG || parserDepthNow > outerDepthDAM)) {
        if (typeDAM == IXmlPullParser::END_TAG || typeDAM == IXmlPullParser::TEXT) {
            continue;
        }
        String tagDAM;
        parser->GetName(&tagDAM);
        if (TAG_TRUST_AGENT_COMPONENT.Equals(tagDAM)) {
            String component;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &component);
            AutoPtr<IList> features;
            GetTrustAgentFeatures(parser, tag, (IList**)&features);
            result->Put(StringUtils::ParseCharSequence(component), features);
        } else {
            Slogger::W(LOG__TAG, "Unknown tag under %s: %s", tag.string(), tagDAM.string());
        }
    }
    *features = result;
    REFCOUNT_ADD(*features)
    return NOERROR;
}

ECode CDevicePolicyManagerService::ActiveAdmin::GetTrustAgentFeatures(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tag,
    /* [out] */ IList** features)
{
    VALIDATE_NOT_NULL(features)

    Int32 outerDepthDAM;
    parser->GetDepth(&outerDepthDAM);
    Int32 typeDAM;
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    Int32 parserDepthNow;
    while (parser->Next(&typeDAM), parser->GetDepth(&parserDepthNow),
            typeDAM != IXmlPullParser::END_DOCUMENT && (typeDAM != IXmlPullParser::END_TAG || parserDepthNow > outerDepthDAM)) {
        if (typeDAM == IXmlPullParser::END_TAG || typeDAM == IXmlPullParser::TEXT) {
            continue;
        }
        String tagDAM;
        parser->GetName(&tagDAM);
        if (TAG_TRUST_AGENT_FEATURE.Equals(tagDAM)) {
            String feature;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &feature);
            result->Add(StringUtils::ParseCharSequence(feature));
        } else {
            Slogger::W(LOG__TAG, "Unknown tag under %s: %s", tag.string(), tagDAM.string());
        }
    }
    *features = IList::Probe(result);
    REFCOUNT_ADD(*features)
    return NOERROR;
}

ECode CDevicePolicyManagerService::ActiveAdmin::GetCrossProfileWidgetProviders(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tag,
    /* [out] */ IList** providers)
{
    VALIDATE_NOT_NULL(providers)

    Int32 outerDepthDAM;
    parser->GetDepth(&outerDepthDAM);
    Int32 typeDAM;
    AutoPtr<IArrayList> result;
    Int32 parserDepthNow;
    while (parser->Next(&typeDAM), parser->GetDepth(&parserDepthNow),
            typeDAM != IXmlPullParser::END_DOCUMENT && (typeDAM != IXmlPullParser::END_TAG || parserDepthNow > outerDepthDAM)) {
        if (typeDAM == IXmlPullParser::END_TAG || typeDAM == IXmlPullParser::TEXT) {
            continue;
        }
        String tagDAM;
        parser->GetName(&tagDAM);
        if (TAG_PROVIDER.Equals(tagDAM)) {
            String provider;
            parser->GetAttributeValue(String(NULL), ATTR_VALUE, &provider);
            if (result == NULL) {
                CArrayList::New((IArrayList**)&result);
            }
            result->Add(StringUtils::ParseCharSequence(provider));
        } else {
            Slogger::W(LOG__TAG, "Unknown tag under %s: %s", tag.string(), tagDAM.string());
        }
    }
    *providers = IList::Probe(result);
    REFCOUNT_ADD(*providers)
    return NOERROR;
}

void CDevicePolicyManagerService::ActiveAdmin::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(prefix);
    pw->Print(String("uid="));
    pw->Println(GetUid());
    pw->Print(prefix);
    pw->Println(String("policies:"));
    AutoPtr<IArrayList> pols;
    mInfo->GetUsedPolicies((IArrayList**)&pols);
    if (pols != NULL) {
        Int32 size;
        pols->GetSize(&size);
        for (Int32 i=0; i< size; i++) {
            pw->Print(prefix);
            pw->Print(String("  "));
            AutoPtr<IInterface> obj;
            pols->Get(i, (IInterface**)&obj);
            AutoPtr<IDeviceAdminInfoPolicyInfo> info;
            String tag;
            info->GetTag(&tag);
            pw->Println(tag);
        }
    }
    pw->Print(prefix);
    pw->Print(String("passwordQuality=0x"));
    pw->Println(StringUtils::ToHexString(mPasswordQuality));
    pw->Print(prefix);
    pw->Print(String("minimumPasswordLength="));
    pw->Println(mMinimumPasswordLength);
    pw->Print(prefix);
    pw->Print(String("passwordHistoryLength="));
    pw->Println(mPasswordHistoryLength);
    pw->Print(prefix);
    pw->Print(String("minimumPasswordUpperCase="));
    pw->Println(mMinimumPasswordUpperCase);
    pw->Print(prefix);
    pw->Print(String("minimumPasswordLowerCase="));
    pw->Println(mMinimumPasswordLowerCase);
    pw->Print(prefix);
    pw->Print(String("minimumPasswordLetters="));
    pw->Println(mMinimumPasswordLetters);
    pw->Print(prefix);
    pw->Print(String("minimumPasswordNumeric="));
    pw->Println(mMinimumPasswordNumeric);
    pw->Print(prefix);
    pw->Print(String("minimumPasswordSymbols="));
    pw->Println(mMinimumPasswordSymbols);
    pw->Print(prefix);
    pw->Print(String("minimumPasswordNonLetter="));
    pw->Println(mMinimumPasswordNonLetter);
    pw->Print(prefix);
    pw->Print(String("maximumTimeToUnlock="));
    pw->Println(mMaximumTimeToUnlock);
    pw->Print(prefix);
    pw->Print(String("maximumFailedPasswordsForWipe="));
    pw->Println(mMaximumFailedPasswordsForWipe);
    pw->Print(prefix);
    pw->Print(String("specifiesGlobalProxy="));
    pw->Println(mSpecifiesGlobalProxy);
    pw->Print(prefix);
    pw->Print(String("passwordExpirationTimeout="));
    pw->Println(mPasswordExpirationTimeout);
    pw->Print(prefix);
    pw->Print(String("passwordExpirationDate="));
    pw->Println(mPasswordExpirationDate);
    if (mGlobalProxySpec != NULL) {
        pw->Print(prefix);
        pw->Print(String("globalProxySpec="));
        pw->Println(mGlobalProxySpec);
    }
    if (mGlobalProxyExclusionList != NULL) {
        pw->Print(prefix);
        pw->Print(String("globalProxyEclusionList="));
        pw->Println(mGlobalProxyExclusionList);
    }
    pw->Print(prefix);
    pw->Print(String("encryptionRequested="));
    pw->Println(mEncryptionRequested);
    pw->Print(prefix);
    pw->Print(String("disableCamera="));
    pw->Println(mDisableCamera);
    pw->Print(prefix);
    pw->Print(String("disableCallerId="));
    pw->Println(mDisableCallerId);
    pw->Print(prefix);
    pw->Print(String("disableScreenCapture="));
    pw->Println(mDisableScreenCapture);
    pw->Print(prefix);
    pw->Print(String("requireAutoTime="));
    pw->Println(mRequireAutoTime);
    pw->Print(prefix);
    pw->Print(String("disabledKeyguardFeatures="));
    pw->Println(mDisabledKeyguardFeatures);
    pw->Print(prefix);
    pw->Print(String("crossProfileWidgetProviders="));
    pw->Println(mCrossProfileWidgetProviders);
    if (!(mPermittedAccessiblityServices == NULL)) {
        pw->Print(prefix);
        pw->Print(String("permittedAccessibilityServices="));
        pw->Println(Object::ToString(mPermittedAccessiblityServices));
    }
    if (!(mPermittedInputMethods == NULL)) {
        pw->Print(prefix);
        pw->Print(String("permittedInputMethods="));
        pw->Println(Object::ToString(mPermittedInputMethods));
    }
}

//=============================================================================
// CDevicePolicyManagerService::Lifecycle
//=============================================================================
ECode CDevicePolicyManagerService::Lifecycle::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    CDevicePolicyManagerService::NewByFriend(context, (CDevicePolicyManagerService**)&mService);
    return NOERROR;
}

ECode CDevicePolicyManagerService::Lifecycle::OnStart()
{
    PublishBinderService(IContext::DEVICE_POLICY_SERVICE, mService);
    return NOERROR;
}

ECode CDevicePolicyManagerService::Lifecycle::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == PHASE_LOCK_SETTINGS_READY) {
        mService->SystemReady();
    }
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::MonitoringCertNotificationTask
//=============================================================================
CDevicePolicyManagerService::MonitoringCertNotificationTask::MonitoringCertNotificationTask(
    /* [in] */ CDevicePolicyManagerService* host)
    : mHost(host)
{}

ECode CDevicePolicyManagerService::MonitoringCertNotificationTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 userHandle;
    IIntent::Probe((*params)[0])->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_ALL, &userHandle);
    if (userHandle == IUserHandle::USER_ALL) {
        AutoPtr<IList> users;
        mHost->mUserManager->GetUsers((IList**)&users);
        AutoPtr<IIterator> it;
        ICollection::Probe(users)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            ManageNotification(userHandle);
        }
    } else {
        AutoPtr<IUserHandle> newUserHandle;
        CUserHandle::New(userHandle, (IUserHandle**)&newUserHandle);
        ManageNotification(newUserHandle);
    }
    *result = NULL;
    return NOERROR;
}

ECode CDevicePolicyManagerService::MonitoringCertNotificationTask::ManageNotification(
    /* [in] */ IUserHandle* userHandle)
{
    Boolean isUserRunning;
    mHost->mUserManager->IsUserRunning(userHandle, &isUserRunning);
    if (!isUserRunning) {
        return NOERROR;
    }
    Boolean hasCert = FALSE;
    const Int64 id = Binder::ClearCallingIdentity();
    // try {
    ECode ec = NOERROR;
    do {
        AutoPtr<IKeyChain> helper;
        // TODO: Waiting for CKeyChain
        assert(0);
        // CKeyChain::AcquireSingleton((IKeyChain**)&helper);
        AutoPtr<IKeyChainConnection> kcs;
        ec = helper->BindAsUser(mHost->mContext, userHandle, (IKeyChainConnection**)&kcs);
        if (FAILED(ec)) break;
        // try {
        do {
            AutoPtr<IIKeyChainService> service;
            ec = kcs->GetService((IIKeyChainService**)&service);
            if (FAILED(ec)) break;
            AutoPtr<IParceledListSlice> listSlice;
            ec = service->GetUserCaAliases((IParceledListSlice**)&listSlice);
            if (FAILED(ec)) break;
            AutoPtr<IList> list;
            ec = listSlice->GetList((IList**)&list);
            if (FAILED(ec)) break;
            Boolean isEmpty;
            ec = list->IsEmpty(&isEmpty);
            if (FAILED(ec)) break;
            if (!isEmpty) {
                hasCert = TRUE;
            }
        } while(FALSE);
        if (FAILED(ec)) {
        // } catch (RemoteException e) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Logger::E(LOG__TAG, "Could not connect to KeyChain service %d", ec);
            }
        }
        // } finally {
        ICloseable::Probe(kcs)->Close();
        // }
    } while(FALSE);
    if (FAILED(ec)) {
    // } catch (InterruptedException e) {
        if ((ECode) E_INTERRUPTED_EXCEPTION == ec) {
            Thread::GetCurrentThread()->Interrupt();
        }
        else if ((ECode) E_RUNTIME_EXCEPTION == ec) {
    // } catch (RuntimeException e) {
            Logger::E(LOG__TAG, "Could not connect to KeyChain service %d", ec);
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(id);
    // }
    if (FAILED(ec)) return ec;
    if (!hasCert) {
        mHost->GetNotificationManager()->CancelAsUser(
                String(NULL), CDevicePolicyManagerService::MONITORING_CERT_NOTIFICATION_ID, userHandle);
        return NOERROR;
    }
    Int32 smallIconId;
    String contentText;
    String ownerName;
    mHost->GetDeviceOwnerName(&ownerName);
    if (ownerName != NULL) {
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, StringUtils::ParseCharSequence(ownerName));
        mHost->mContext->GetString(R::string::ssl_ca_cert_noti_managed, args, &contentText);
        smallIconId = R::drawable::stat_sys_certificate_info;
    } else {
        mHost->mContext->GetString(R::string::ssl_ca_cert_noti_by_unknown, &contentText);
        smallIconId = R::drawable::stat_sys_warning;
    }
    AutoPtr<IIntent> dialogIntent;
    CIntent::New(ISettings::ACTION_MONITORING_CERT_INFO, (IIntent**)&dialogIntent);
    dialogIntent->SetFlags(
            IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TASK);
    dialogIntent->SetPackage(String("com.android.settings"));
    AutoPtr<IPendingIntentHelper> intentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&intentHelper);
    AutoPtr<IPendingIntent> notifyIntent;
    intentHelper->GetActivityAsUser(mHost->mContext, 0,
            dialogIntent, IPendingIntent::FLAG_UPDATE_CURRENT, NULL, userHandle, (IPendingIntent**)&notifyIntent);
    AutoPtr<IContext> userContext;
    // try {
    ec = mHost->mContext->CreatePackageContextAsUser(String("android"), 0, userHandle, (IContext**)&userContext);
    // } catch (PackageManager.NameNotFoundException e) {
    if (FAILED(ec)) {
        if ((ECode) E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION == ec) {
            Logger::E(LOG__TAG, "Create context as %s failed %d", TO_CSTR(userHandle), ec);
            return NOERROR;
        }
        else
            return ec;
    }
    // }
    AutoPtr<INotificationBuilder> notificationBuilder;
    CNotificationBuilder::New(userContext, (INotificationBuilder**)&notificationBuilder);
    notificationBuilder->SetSmallIcon(smallIconId);
    String warningStr;
    mHost->mContext->GetString(R::string::ssl_ca_cert_warning, &warningStr);
    notificationBuilder->SetContentTitle(StringUtils::ParseCharSequence(warningStr));
    notificationBuilder->SetContentText(StringUtils::ParseCharSequence(contentText));
    notificationBuilder->SetContentIntent(notifyIntent);
    notificationBuilder->SetPriority(INotification::PRIORITY_HIGH);
    notificationBuilder->SetShowWhen(FALSE);
    AutoPtr<IResources> res;
    mHost->mContext->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(R::color::system_notification_accent_color, &color);
    notificationBuilder->SetColor(color);
    AutoPtr<INotification> noti;
    notificationBuilder->Build((INotification**)&noti);
    mHost->GetNotificationManager()->NotifyAsUser(
            String(NULL), MONITORING_CERT_NOTIFICATION_ID, noti, userHandle);
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::SetupContentObserver
//=============================================================================
CDevicePolicyManagerService::SetupContentObserver::SetupContentObserver(
    /* [in] */ CDevicePolicyManagerService* host)
    : mHost(host)
{
    Settings::Secure::GetUriFor(
            ISettingsSecure::USER_SETUP_COMPLETE, (IUri**)&mUserSetupComplete);
}

ECode CDevicePolicyManagerService::SetupContentObserver::constructor(
    /* [in] */ IHandler* handler)
{
    return ContentObserver::constructor(handler);
}

ECode CDevicePolicyManagerService::SetupContentObserver::Register(
    /* [in] */ IContentResolver* resolver)
{
    return resolver->RegisterContentObserver(mUserSetupComplete, FALSE, this, IUserHandle::USER_ALL);
}

ECode CDevicePolicyManagerService::SetupContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Boolean isEquals;
    IObject::Probe(mUserSetupComplete)->Equals(uri, &isEquals);
    if (isEquals) {
        mHost->UpdateUserSetupComplete();
    }
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService::LocalService
//=============================================================================
CAR_INTERFACE_IMPL(CDevicePolicyManagerService::LocalService, Object, IDevicePolicyManagerInternal)

CDevicePolicyManagerService::LocalService::LocalService(
    /* [in] */ CDevicePolicyManagerService* host)
    : mHost(host)
{}

ECode CDevicePolicyManagerService::LocalService::GetCrossProfileWidgetProviders(
    /* [in] */ Int32 profileId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(mHost);
        if (mHost->mDeviceOwner == NULL) {
            AutoPtr<ICollections> collectionsHelper;
            CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
            return collectionsHelper->GetEmptyList(result);
        }
        AutoPtr<IComponentName> ownerComponent;
        mHost->mDeviceOwner->GetProfileOwnerComponent(profileId, (IComponentName**)&ownerComponent);
        if (ownerComponent == NULL) {
            AutoPtr<ICollections> collectionsHelper;
            CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
            return collectionsHelper->GetEmptyList(result);
        }
        AutoPtr<DevicePolicyData> policy = mHost->GetUserData(profileId);
        AutoPtr<IInterface> obj;
        policy->mAdminMap->Get(ownerComponent, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
        Boolean isEmpty;
        admin->mCrossProfileWidgetProviders->IsEmpty(&isEmpty);
        if (admin == NULL || admin->mCrossProfileWidgetProviders == NULL || isEmpty) {
            AutoPtr<ICollections> collectionsHelper;
            CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
            return collectionsHelper->GetEmptyList(result);
        }
        *result = admin->mCrossProfileWidgetProviders;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::LocalService::AddOnCrossProfileWidgetProvidersChangeListener(
    /* [in] */ IDevicePolicyManagerInternalOnCrossProfileWidgetProvidersChangeListener* listener)
{
    {    AutoLock syncLock(mHost);
        if (mWidgetProviderListeners == NULL) {
            CArrayList::New((IList**)&mWidgetProviderListeners);
        }
        Boolean isContains;
        mWidgetProviderListeners->Contains(listener, &isContains);
        if (!isContains) {
            mWidgetProviderListeners->Add(listener);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::LocalService::NotifyCrossProfileProvidersChanged(
    /* [in] */ Int32 userId,
    /* [in] */ IList* packages)
{
    AutoPtr<IList> listeners;
    {    AutoLock syncLock(mHost);
        CArrayList::New(ICollection::Probe(mWidgetProviderListeners), (IList**)&listeners);
    }
    Int32 listenerCount;
    listeners->GetSize(&listenerCount);
    for (Int32 i = 0; i < listenerCount; i++) {
        AutoPtr<IInterface> obj;
        listeners->Get(i, (IInterface**)&obj);
        AutoPtr<IDevicePolicyManagerInternalOnCrossProfileWidgetProvidersChangeListener> listener;
        // TODO: Waiting for IDevicePolicyManagerInternalOnCrossProfileWidgetProvidersChangeListener
        assert(0);
        // listener->OnCrossProfileWidgetProvidersChanged(userId, packages);
    }
    return NOERROR;
}

//=============================================================================
// CDevicePolicyManagerService
//=============================================================================
const String CDevicePolicyManagerService::LOG__TAG("DevicePolicyManagerService");

const String CDevicePolicyManagerService::DEVICE_POLICIES_XML("device_policies.xml");

const String CDevicePolicyManagerService::LOCK_TASK_COMPONENTS_XML("lock-task-component");

const Int32 CDevicePolicyManagerService::REQUEST_EXPIRE_PASSWORD = 5571;

const Int64 CDevicePolicyManagerService::MS_PER_DAY = 86400 * 1000;

const Int64 CDevicePolicyManagerService::EXPIRATION_GRACE_PERIOD_MS = 5 * MS_PER_DAY; // 5 days, in ms

const String CDevicePolicyManagerService::ACTION_EXPIRED_PASSWORD_NOTIFICATION(
    "com.android.server.ACTION_EXPIRED_PASSWORD_NOTIFICATION");

const Int32 CDevicePolicyManagerService::MONITORING_CERT_NOTIFICATION_ID = R::string::ssl_ca_cert_warning;

const Boolean CDevicePolicyManagerService::DBG = FALSE;

const String CDevicePolicyManagerService::ATTR_PERMISSION_PROVIDER("permission-provider");
const String CDevicePolicyManagerService::ATTR_SETUP_COMPLETE("setup-complete");

const AutoPtr<ISet> CDevicePolicyManagerService::DEVICE_OWNER_USER_RESTRICTIONS
    = InitDEVICE_OWNER_USER_RESTRICTIONS();

const AutoPtr<ISet> CDevicePolicyManagerService::SECURE_SETTINGS_WHITELIST
    = InitSECURE_SETTINGS_WHITELIST();
const AutoPtr<ISet> CDevicePolicyManagerService::SECURE_SETTINGS_DEVICEOWNER_WHITELIST
    = InitSECURE_SETTINGS_DEVICEOWNER_WHITELIST();
const AutoPtr<ISet> CDevicePolicyManagerService::GLOBAL_SETTINGS_WHITELIST
    = InitGLOBAL_SETTINGS_WHITELIST();

const String CDevicePolicyManagerService::SYSTEM_PROP_DISABLE_CAMERA("sys.secpolicy.camera.disabled");

CAR_OBJECT_IMPL(CDevicePolicyManagerService)

CAR_INTERFACE_IMPL_2(CDevicePolicyManagerService, Object, IBinder, IIDevicePolicyManager)

CDevicePolicyManagerService::CDevicePolicyManagerService()
{
    CSparseArray::New((ISparseArray**)&mUserData);
    CHandler::New((IHandler**)&mHandler);
    mReceiver = new DevicePolicyReceiver(this);
}

ECode CDevicePolicyManagerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    helper->Get(mContext, (IUserManager**)&mUserManager);
    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    packageManager->HasSystemFeature(
            IPackageManager::FEATURE_DEVICE_ADMIN, &mHasFeature);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> powerMgr = IPowerManager::Probe(service);
    powerMgr->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
        String("DPM"), (IPowerManagerWakeLock**)&mWakeLock);

    mLocalService = new LocalService(this);
    if (!mHasFeature) {
        // Skip the rest of the initialization
        return NOERROR;
    }
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_BOOT_COMPLETED);
    filter->AddAction(ACTION_EXPIRED_PASSWORD_NOTIFICATION);
    filter->AddAction(IIntent::ACTION_USER_REMOVED);
    filter->AddAction(IIntent::ACTION_USER_STARTED);
    filter->AddAction(IKeyChain::ACTION_STORAGE_CHANGED);
    filter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);

    AutoPtr<IIntent> bootCompletedIntent;
    context->RegisterReceiverAsUser(mReceiver,
        UserHandle::ALL, filter, String(NULL), mHandler, (IIntent**)&bootCompletedIntent);

    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    filter->AddDataScheme(String("package"));

    AutoPtr<IIntent> pkgChangedIntent;
    context->RegisterReceiverAsUser(mReceiver,
        UserHandle::ALL, filter, String(NULL), mHandler, (IIntent**)&pkgChangedIntent);

    LocalServices::AddService(EIID_IDevicePolicyManagerInternal, TO_IINTERFACE(mLocalService));
    return NOERROR;
}

CDevicePolicyManagerService::~CDevicePolicyManagerService()
{}

ECode CDevicePolicyManagerService::SetPasswordQuality(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 quality,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(ValidateQualityConstant(quality))
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))
        if (ap->mPasswordQuality != quality) {
            ap->mPasswordQuality = quality;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordQuality(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 mode = IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);

            if (admin != NULL) {
                *password = admin->mPasswordQuality;
            } else {
                *password = mode;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (mode < admin->mPasswordQuality) {
                    mode = admin->mPasswordQuality;
                }
            }
        }
        *password = mode;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Slogger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))

        if (ap->mMinimumPasswordLength != length) {
            ap->mMinimumPasswordLength = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordLength;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordLength) {
                    length = admin->mMinimumPasswordLength;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Slogger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))

        if (ap->mMinimumPasswordUpperCase != length) {
            ap->mMinimumPasswordUpperCase = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)
    *password = 0;

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordUpperCase;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordUpperCase) {
                    length = admin->mMinimumPasswordUpperCase;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Slogger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))

        if (ap->mMinimumPasswordLowerCase != length) {
            ap->mMinimumPasswordLowerCase = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return 0;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordLowerCase;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordLowerCase) {
                    length = admin->mMinimumPasswordLowerCase;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumLetters(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))

        if (ap->mMinimumPasswordLetters != length) {
            ap->mMinimumPasswordLetters = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumLetters(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordLetters;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordLetters) {
                    length = admin->mMinimumPasswordLetters;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumNumeric(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))
        if (ap->mMinimumPasswordNumeric != length) {
            ap->mMinimumPasswordNumeric = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumNumeric(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordNumeric;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordNumeric) {
                    length = admin->mMinimumPasswordNumeric;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumSymbols(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))
        if (ap->mMinimumPasswordSymbols != length) {
            ap->mMinimumPasswordSymbols = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumSymbols(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);

        Int32 length = 0;
        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordSymbols;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordSymbols) {
                    length = admin->mMinimumPasswordSymbols;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))
        if (ap->mMinimumPasswordNonLetter != length) {
            ap->mMinimumPasswordNonLetter = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mMinimumPasswordNonLetter;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i = 0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mMinimumPasswordNonLetter) {
                    length = admin->mMinimumPasswordNonLetter;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordHistoryLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))

        if (ap->mPasswordHistoryLength != length) {
            ap->mPasswordHistoryLength = length;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordHistoryLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int32 length = 0;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mPasswordHistoryLength;
            } else {
                *password = length;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (length < admin->mPasswordHistoryLength) {
                    length = admin->mPasswordHistoryLength;
                }
            }
        }
        *password = length;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordExpirationTimeout(
    /* [in] */ IComponentName* who,
    /* [in] */ Int64 timeout,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        if (timeout < 0) {
            Logger::E(LOG__TAG, "Timeout must be >= 0 ms");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_EXPIRE_PASSWORD, (ActiveAdmin**)&ap))

        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        // Calling this API automatically bumps the expiration date
        Int64 expiration = timeout > 0L ? (timeout + millis) : 0L;
        ap->mPasswordExpirationDate = expiration;
        ap->mPasswordExpirationTimeout = timeout;
        if (timeout > 0L) {
            AutoPtr<IDateFormatHelper> helper;
            CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
            AutoPtr<IDateFormat> dateFormat;
            helper->GetDateTimeInstance(IDateFormat::DEFAULT, IDateFormat::DEFAULT, (IDateFormat**)&dateFormat);
            AutoPtr<IDate> date;
            CDate::New(expiration, (IDate**)&date);
            String fmt;
            dateFormat->Format(date, &fmt);
            Slogger::W(LOG__TAG, "setPasswordExpiration(): password will expire on ", fmt.string());
        }
        SaveSettingsLocked(userHandle);
        // in case this is the first one
        SetExpirationAlarmCheckLocked(mContext, GetUserData(userHandle));
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordExpirationTimeout(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int64 timeout = 0L;

        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *password = admin->mPasswordExpirationTimeout;
            } else {
                *password = timeout;
            }
            return NOERROR;
        }

        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (timeout == 0L || (admin->mPasswordExpirationTimeout != 0L
                        && timeout > admin->mPasswordExpirationTimeout)) {
                    timeout = admin->mPasswordExpirationTimeout;
                }
            }
        }
        *password = timeout;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::AddCrossProfileWidgetProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    const Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<IList> changedProviders;

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> activeAdmin;
        GetActiveAdminForCallerLocked(admin,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&activeAdmin);
        if (activeAdmin->mCrossProfileWidgetProviders == NULL) {
            CArrayList::New((IList**)&(activeAdmin->mCrossProfileWidgetProviders));
        }
        AutoPtr<IList> providers = activeAdmin->mCrossProfileWidgetProviders;
        Boolean isContains;
        providers->Contains(StringUtils::ParseCharSequence(packageName), &isContains);
        if (!isContains) {
            providers->Add(StringUtils::ParseCharSequence(packageName));
            CArrayList::New(ICollection::Probe(providers), (IList**)&changedProviders);
            SaveSettingsLocked(userId);
        }
    }

    if (changedProviders != NULL) {
        mLocalService->NotifyCrossProfileProvidersChanged(userId, changedProviders);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::RemoveCrossProfileWidgetProvider(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean* result)
{
    const Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<IList> changedProviders;

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> activeAdmin;
        GetActiveAdminForCallerLocked(admin,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&activeAdmin);
        if (activeAdmin->mCrossProfileWidgetProviders == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<IList> providers = activeAdmin->mCrossProfileWidgetProviders;
        if (providers->Remove(StringUtils::ParseCharSequence(packageName))) {
            CArrayList::New(ICollection::Probe(providers), (IList**)&changedProviders);
            SaveSettingsLocked(userId);
        }
    }

    if (changedProviders != NULL) {
        mLocalService->NotifyCrossProfileProvidersChanged(userId, changedProviders);
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCrossProfileWidgetProviders(
    /* [in] */ IComponentName* admin,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> activeAdmin;
        GetActiveAdminForCallerLocked(admin,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&activeAdmin);
        Boolean isEmpty;
        activeAdmin->mCrossProfileWidgetProviders->IsEmpty(&isEmpty);
        if (activeAdmin->mCrossProfileWidgetProviders == NULL || isEmpty) {
            return NOERROR;
        }
        if (Binder::GetCallingUid() == Process::MyUid()) {
            return CArrayList::New(ICollection::Probe(activeAdmin->mCrossProfileWidgetProviders), result);
        } else {
            *result = activeAdmin->mCrossProfileWidgetProviders;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordExpiration(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return 0L;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        *password = GetPasswordExpirationLocked(who, userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsActivePasswordSufficient(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        // The active password is stored in the user that runs the launcher
        // If the user this is called from is part of a profile group, that is the parent
        // of the group.
        AutoPtr<IUserInfo> parent = GetProfileParent(userHandle);
        Int32 id;
        if (parent == NULL)
            id = userHandle;
        else
            parent->GetId(&id);
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(NULL,
            IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap))

        Int32 quality;
        FAIL_RETURN(GetPasswordQuality(NULL, userHandle, &quality))
        Int32 length;
        FAIL_RETURN(GetPasswordMinimumLength(NULL, userHandle, &length))

        if (policy->mActivePasswordQuality < quality || policy->mActivePasswordLength < length) {
            *result = FALSE;
            return NOERROR;
        }
        if (policy->mActivePasswordQuality != IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX) {
            *result = TRUE;
            return NOERROR;
        }

        Int32 upperCase;
        FAIL_RETURN(GetPasswordMinimumUpperCase(NULL, userHandle, &upperCase))
        Int32 lowerCase;
        FAIL_RETURN(GetPasswordMinimumLowerCase(NULL, userHandle, &lowerCase))
        Int32 letters;
        FAIL_RETURN(GetPasswordMinimumLetters(NULL, userHandle, &letters))
        Int32 numeric;
        FAIL_RETURN(GetPasswordMinimumNumeric(NULL, userHandle, &numeric))
        Int32 symbols;
        FAIL_RETURN(GetPasswordMinimumSymbols(NULL, userHandle, &symbols))
        Int32 nonLetter;
        FAIL_RETURN(GetPasswordMinimumNonLetter(NULL, userHandle, &nonLetter))

        *result = policy->mActivePasswordUpperCase >= upperCase
                && policy->mActivePasswordLowerCase >= lowerCase
                && policy->mActivePasswordLetters >= letters
                && policy->mActivePasswordNumeric >= numeric
                && policy->mActivePasswordSymbols >= symbols
                && policy->mActivePasswordNonLetter >= nonLetter;
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCurrentFailedPasswordAttempts(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    {    AutoLock syncLock(this);
        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, (ActiveAdmin**)&ap))


        // The active password is stored in the parent.
        Int32 id;
        GetProfileParent(userHandle)->GetId(&id);
        AutoPtr<DevicePolicyData> policy = GetUserData(id);

        *password = policy->mFailedPasswordAttempts;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 num,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_WIPE_DATA, (ActiveAdmin**)&ap))
        ap = NULL;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, (ActiveAdmin**)&ap))

        if (ap->mMaximumFailedPasswordsForWipe != num) {
            ap->mMaximumFailedPasswordsForWipe = num;
            SaveSettingsLocked(userHandle);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin = (who != NULL) ? GetActiveAdminUncheckedLocked(who, userHandle)
                : GetAdminWithMinimumFailedPasswordsForWipeLocked(userHandle);
        *password = admin != NULL ? admin->mMaximumFailedPasswordsForWipe : 0;
    }
    return NOERROR;
}

AutoPtr<CDevicePolicyManagerService::ActiveAdmin> CDevicePolicyManagerService::GetAdminWithMinimumFailedPasswordsForWipeLocked(
    /* [in] */ Int32 userHandle)
{
    Int32 count = 0;
    AutoPtr<ActiveAdmin> strictestAdmin;
    AutoPtr<IList> profiles;
    mUserManager->GetProfiles(userHandle, (IList**)&profiles);
    AutoPtr<IIterator> it;
    ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
        AutoPtr<IUserHandle> userHandle;
        userInfo->GetUserHandle((IUserHandle**)&userHandle);
        Int32 identifier;
        userHandle->GetIdentifier(&identifier);
        AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
        FOR_EACH(it, policy->mAdminList) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            if (admin->mMaximumFailedPasswordsForWipe ==
                    ActiveAdmin::DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE) {
                continue;  // No max number of failed passwords policy set for this profile.
            }

            // We always favor the primary profile if several profiles have the same value set.
            Boolean isPrimary;
            userInfo->IsPrimary(&isPrimary);
            if (count == 0 ||
                    count > admin->mMaximumFailedPasswordsForWipe ||
                    (isPrimary && count >= admin->mMaximumFailedPasswordsForWipe)) {
                count = admin->mMaximumFailedPasswordsForWipe;
                strictestAdmin = admin;
            }
        }
    }
    return strictestAdmin;
}

ECode CDevicePolicyManagerService::ResetPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    EnforceNotManagedProfile(userHandle, String("reset the password"));

    Int32 quality;
    {    AutoLock syncLock(this);

        // This api can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_RESET_PASSWORD, (ActiveAdmin**)&ap))
        GetPasswordQuality(NULL, userHandle, &quality);
        if (quality != IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
            AutoPtr<ILockPatternUtilsHelper> helper;
            CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
            Int32 realQuality;
            // TODO: Waiting for ILockPatternUtilsHelper
            assert(0);
            // helper->ComputePasswordQuality(password, &realQuality);

            if (realQuality < quality && quality != IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX) {
                StringBuffer buf;
                buf += "resetPassword: password quality 0x";
                buf += StringUtils::ToString(quality, 16);
                buf += " does not meet required quality 0x";
                buf += StringUtils::ToString(realQuality, 16);
                Slogger::W(LOG__TAG, buf.ToString());
                *result = FALSE;
                return NOERROR;
            }

            quality = Elastos::Core::Math::Max(realQuality, quality);
        }

        Int32 length;
        GetPasswordMinimumLength(NULL, userHandle, &length);
        if ((Int32)password.GetLength() < length) {
            StringBuffer buf;
            buf += "resetPassword: password length ";
            buf += password.GetLength();
            buf += " does not meet required length ";
            buf += length;
            Slogger::W(LOG__TAG, buf.ToString());
            return NOERROR;
        }

        if (quality == IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX) {
            Int32 letters = 0;
            Int32 uppercase = 0;
            Int32 lowercase = 0;
            Int32 numbers = 0;
            Int32 symbols = 0;
            Int32 nonletter = 0;

            for (Int32 i = 0; i < (Int32)password.GetLength(); i++) {
                Char32 c = password.GetChar(i);
                if (c >= 'A' && c <= 'Z') {
                    letters++;
                    uppercase++;
                } else if (c >= 'a' && c <= 'z') {
                    letters++;
                    lowercase++;
                } else if (c >= '0' && c <= '9') {
                    numbers++;
                    nonletter++;
                } else {
                    symbols++;
                    nonletter++;
                }
            }
            Int32 neededLetters;
            GetPasswordMinimumLetters(NULL, userHandle, &neededLetters);
            if(letters < neededLetters) {
                StringBuffer buf;
                buf += "resetPassword: number of letters ";
                buf += letters;
                buf += " does not meet required number of letters ";
                buf += neededLetters;
                Slogger::W(LOG__TAG, buf.ToString());
                return NOERROR;
            }

            Int32 neededNumbers;
            GetPasswordMinimumNumeric(NULL, userHandle, &neededNumbers);
            if (numbers < neededNumbers) {
                StringBuffer buf;
                buf += "resetPassword: number of numerical digits ";
                buf += numbers;
                buf += " does not meet required number of numerical digits ";
                buf += neededNumbers;
                Slogger::W(LOG__TAG, buf.ToString());
                return NOERROR;
            }

            Int32 neededLowerCase;
            GetPasswordMinimumLowerCase(NULL, userHandle, &neededLowerCase);
            if (lowercase < neededLowerCase) {
                StringBuffer buf;
                buf += "resetPassword: number of lowercase letters ";
                buf += lowercase;
                buf += " does not meet required number of lowercase letters ";
                buf += neededLowerCase;
                Slogger::W(LOG__TAG, buf.ToString());
                return NOERROR;
            }

            Int32 neededUpperCase;
            GetPasswordMinimumUpperCase(NULL, userHandle, &neededUpperCase);
            if (uppercase < neededUpperCase) {
                StringBuffer buf;
                buf += "resetPassword: number of uppercase letters ";
                buf += uppercase;
                buf += " does not meet required number of uppercase letters ";
                buf += neededUpperCase;
                Slogger::W(LOG__TAG, buf.ToString());
                return NOERROR;
            }

            Int32 neededSymbols;
            GetPasswordMinimumSymbols(NULL, userHandle, &neededSymbols);
            if (symbols < neededSymbols) {
                StringBuffer buf;
                buf += "resetPassword: number of special symbols ";
                buf += symbols;
                buf += " does not meet required number of special symbols ";
                buf += neededSymbols;
                Slogger::W(LOG__TAG, buf.ToString());
                return NOERROR;
            }

            Int32 neededNonLetter;
            GetPasswordMinimumNonLetter(NULL, userHandle, &neededNonLetter);
            if (nonletter < neededNonLetter) {
                StringBuffer buf;
                buf += "resetPassword: number of non-letter characters ";
                buf += nonletter;
                buf += " does not meet required number of non-letter characters ";
                buf += neededNonLetter;
                Slogger::W(LOG__TAG, buf.ToString());
                return NOERROR;
            }
        }
    }

    Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    if (policy->mPasswordOwner >= 0 && policy->mPasswordOwner != callingUid) {
        Slogger::W(LOG__TAG, "resetPassword: already set by another uid and not entered by user");
        return NOERROR;
    }

    // Don't do this with the lock held, because it is going to call
    // back in to the service.
    Int64 ident = Binder::ClearCallingIdentity();

    // TODO:
    AutoPtr<ILockPatternUtils> utils;
    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&utils);
    utils->SaveLockPassword(password, quality, FALSE, userHandle);
    Boolean requireEntry = (flags & IDevicePolicyManager::RESET_PASSWORD_REQUIRE_ENTRY) != 0;
    if (requireEntry) {
        utils->RequireCredentialEntry(IUserHandle::USER_ALL);
    }
    {    AutoLock syncLock(this);
        Int32 newOwner = requireEntry ? callingUid : -1;
        if (policy->mPasswordOwner != newOwner) {
            policy->mPasswordOwner = newOwner;
            SaveSettingsLocked(userHandle);
        }
    }

    Binder::RestoreCallingIdentity(ident);
    *result = TRUE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetMaximumTimeToLock(
    /* [in] */ IComponentName* who,
    /* [in] */ Int64 timeMs,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who,
            IDeviceAdminInfo::USES_POLICY_FORCE_LOCK, (ActiveAdmin**)&ap))

        if (ap->mMaximumTimeToUnlock != timeMs) {
            ap->mMaximumTimeToUnlock = timeMs;
            SaveSettingsLocked(userHandle);
            AutoPtr<DevicePolicyData> data = GetUserData(userHandle);
            UpdateMaximumTimeToLockLocked(data);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetMaximumTimeToLock(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        Int64 time = 0;
        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin != NULL) {
                *value = admin->mMaximumTimeToUnlock;
            } else {
                *value = time;
            }
            return NOERROR;
        }

        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                if (time == 0) {
                    time = admin->mMaximumTimeToUnlock;
                } else if (admin->mMaximumTimeToUnlock != 0
                        && time > admin->mMaximumTimeToUnlock) {
                    time = admin->mMaximumTimeToUnlock;
                }
            }
        }
        *value = time;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::LockNow()
{
    if (!mHasFeature) {
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> admin;
        FAIL_RETURN(GetActiveAdminForCallerLocked(NULL,
            IDeviceAdminInfo::USES_POLICY_FORCE_LOCK, (ActiveAdmin**)&admin))
        LockNowUnchecked();
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::WipeData(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> admin;
        FAIL_RETURN(GetActiveAdminForCallerLocked(NULL,
            IDeviceAdminInfo::USES_POLICY_WIPE_DATA, (ActiveAdmin**)&admin))

        String source;
        if (admin != NULL && admin->mInfo != NULL) {
            AutoPtr<IComponentName> cname;
            admin->mInfo->GetComponent((IComponentName**)&cname);
            if (cname != NULL) {
                cname->FlattenToShortString(&source);
            } else {
                admin->mInfo->GetPackageName(&source);
            }
        } else {
            source = "?";
        }

        Int64 ident = Binder::ClearCallingIdentity();

        WipeDeviceOrUserLocked(flags, userHandle,
                            String("DevicePolicyManager.wipeData() from ") + source);

        Binder::RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetGlobalProxy(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& proxySpec,
    /* [in] */ const String& exclusionList,
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (!mHasFeature) {
        return NOERROR;
    }

    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        // Only check if owner has set global proxy. We don't allow other users to set it.
        AutoPtr<DevicePolicyData> policy = GetUserData(IUserHandle::USER_OWNER);
        AutoPtr<ActiveAdmin> admin;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_SETS_GLOBAL_PROXY, (ActiveAdmin**)&admin))

        // Scan through active admins and find if anyone has already
        // set the global proxy.
        AutoPtr<ISet> compSet;
        policy->mAdminMap->GetKeySet((ISet**)&compSet);
        AutoPtr<IIterator> it;
        ICollection::Probe(compSet)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IComponentName> component = IComponentName::Probe(obj);
            obj = NULL;
            policy->mAdminMap->Get(component, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> ap = (ActiveAdmin*) IObject::Probe(obj);

            Boolean isEqual;
            IObject::Probe(component)->Equals(who, &isEqual);

            if (ap->mSpecifiesGlobalProxy && !isEqual) {
                // Another admin already sets the global proxy
                // Return it to the caller.
                *result = component;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }

        // If the user is not the owner, don't set the global proxy. Fail silently.
        AutoPtr<IUserHandleHelper> handleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
        Int32 callingUserId;
        handleHelper->GetCallingUserId(&callingUserId);

        if (callingUserId != IUserHandle::USER_OWNER) {
            StringBuffer buf;
            buf += "Only the owner is allowed to set the global proxy. User ";
            buf += userHandle;
            buf += " is not permitted.";

            Slogger::W(LOG__TAG, buf.ToString());
            return NOERROR;
        }

        if (proxySpec.IsNull()) {
            admin->mSpecifiesGlobalProxy = FALSE;
            admin->mGlobalProxySpec = String(NULL);
            admin->mGlobalProxyExclusionList = String(NULL);
        } else {
            admin->mSpecifiesGlobalProxy = TRUE;
            admin->mGlobalProxySpec = proxySpec;
            admin->mGlobalProxyExclusionList = exclusionList;
        }

        // Reset the global proxy accordingly
        // Do this using system permissions, as apps cannot write to secure settings
        Int64 origId = Binder::ClearCallingIdentity();
        // try {
        ResetGlobalProxyLocked(policy);
        // } finally {
        Binder::RestoreCallingIdentity(origId);
        // }
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::GetGlobalProxyAdmin(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component)
    *component = NULL;

    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    {
        {    AutoLock syncLock(this);
            AutoPtr<DevicePolicyData> policy = GetUserData(IUserHandle::USER_OWNER);
            // Scan through active admins and find if anyone has already
            // set the global proxy.
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> ap = (ActiveAdmin*) IObject::Probe(obj);
                if (ap->mSpecifiesGlobalProxy) {
                    // Device admin sets the global proxy
                    // Return it to the caller.
                    ap->mInfo->GetComponent(component);
                    return NOERROR;
                }
            }
        }
        // No device admin sets the global proxy.
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetRecommendedGlobalProxy(
    /* [in] */ IComponentName* who,
    /* [in] */ IProxyInfo* proxyInfo)
{
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
    }
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IInterface> obj;
        ec = mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
        if (FAILED(ec)) break;
        AutoPtr<IConnectivityManager> connectivityManager = IConnectivityManager::Probe(obj);

        ec = connectivityManager->SetGlobalProxy(proxyInfo);
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetStorageEncryption(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean encrypt,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (!mHasFeature) {
        return IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        // Check for permissions
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<IUserHandleHelper> handleHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
        Int32 callingUserId;
        handleHelper->GetCallingUserId(&callingUserId);

        // Only owner can set storage encryption
        if (userHandle != IUserHandle::USER_OWNER || callingUserId != IUserHandle::USER_OWNER) {
            StringBuffer buf;
            buf += "Only owner is allowed to set storage encryption. User ";
            buf += callingUserId;
            buf += " is not permitted.";
            Slogger::W(LOG__TAG, buf.ToString());
            *value = 0;
            return NOERROR;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_ENCRYPTED_STORAGE, (ActiveAdmin**)&ap))

        // Quick exit:  If the filesystem does not support encryption, we can exit early.
        if (!IsEncryptionSupported()) {
            *value = IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
            return NOERROR;
        }

        // (1) Record the value for the admin so it's sticky
        if (ap->mEncryptionRequested != encrypt) {
            ap->mEncryptionRequested = encrypt;
            SaveSettingsLocked(userHandle);
        }

        AutoPtr<DevicePolicyData> policy = GetUserData(IUserHandle::USER_OWNER);
        // (2) Compute "max" for all admins
        Boolean newRequested = FALSE;
        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            newRequested |= ((ActiveAdmin*) IObject::Probe(obj))->mEncryptionRequested;
        }

        // Notify OS of new request
        SetEncryptionRequested(newRequested);

        // Return the new global request status
        *value = (newRequested) ? IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVE : IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetStorageEncryption(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        // Check for permissions if a particular caller is specified
        if (who != NULL) {
            // When checking for a single caller, status is based on caller's request
            AutoPtr<ActiveAdmin> ap = GetActiveAdminUncheckedLocked(who, userHandle);

            if (ap == NULL) return NOERROR;

            *result = ap->mEncryptionRequested;
            return NOERROR;
        }

        // If no particular caller is specified, return the aggregate set of requests.
        // This is short circuited by returning true on the first hit.
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            if (((ActiveAdmin*) IObject::Probe(obj))->mEncryptionRequested) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetStorageEncryptionStatus(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* st)
{
    VALIDATE_NOT_NULL(st)

    if (!mHasFeature) {
        // Ok to return current status.
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    return GetEncryptionStatus(st);
}

ECode CDevicePolicyManagerService::SetCameraDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean disabled,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DISABLE_CAMERA, (ActiveAdmin**)&ap))

        if (ap->mDisableCamera != disabled) {
            ap->mDisableCamera = disabled;
            SaveSettingsLocked(userHandle);
        }
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        SyncDeviceCapabilitiesLocked(policy);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCameraDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            if (admin == NULL) return NOERROR;

            *result = admin->mDisableCamera;
            return NOERROR;
        }

        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        // Determine whether or not the device camera is disabled for any active admins.
        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            if (admin->mDisableCamera) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 which,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    EnforceNotManagedProfile(userHandle, String("disable keyguard features"));

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DISABLE_KEYGUARD_FEATURES, (ActiveAdmin**)&ap))
        if (ap->mDisabledKeyguardFeatures != which) {
            ap->mDisabledKeyguardFeatures = which;
            SaveSettingsLocked(userHandle);
        }
        SyncDeviceCapabilitiesLocked(GetUserData(userHandle));
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* features)
{
    VALIDATE_NOT_NULL(features)

    if (!mHasFeature) {
        return 0;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);

            if (admin != NULL) {
                *features = admin->mDisabledKeyguardFeatures;
            } else {
                *features = 0;
            }
            return NOERROR;
        }

        // Determine which keyguard features are disabled for any active admins.
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        Int32 which = 0;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            which |= admin->mDisabledKeyguardFeatures;
        }
        *features = which;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetDeviceOwner(
    /* [in] */ const String& packageName,
    /* [in] */ const String& ownerName,
    /* [out] */ Boolean* result)
{
    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    Boolean isInstalled;
    DeviceOwner::IsInstalled(packageName, packageManager, &isInstalled);
    if (packageName == NULL
            || !isInstalled) {
        Logger::E(LOG__TAG, "Invalid package name %s for device owner", packageName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        if (!AllowedToSetDeviceOwnerOnDevice()) {
            Logger::E(LOG__TAG, "Trying to set device owner but device is already provisioned.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        Boolean hasDeviceOwner;
        mDeviceOwner->HasDeviceOwner(&hasDeviceOwner);
        if (mDeviceOwner != NULL && hasDeviceOwner) {
            Logger::E(LOG__TAG, "Trying to set device owner but device owner is already set.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        if (mDeviceOwner == NULL) {
            // Device owner is not set and does not exist, set it.
            DeviceOwner::CreateWithDeviceOwner(packageName, ownerName, (DeviceOwner**)&mDeviceOwner);
            mDeviceOwner->WriteOwnerFile();
            *result = TRUE;
            return NOERROR;
        } else {
            // Device owner is not set but a profile owner exists, update Device owner state.
            mDeviceOwner->SetDeviceOwner(packageName, ownerName);
            mDeviceOwner->WriteOwnerFile();
            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsDeviceOwner(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        Boolean hasDeviceOwner;
        mDeviceOwner->HasDeviceOwner(&hasDeviceOwner);
        String name;
        mDeviceOwner->GetDeviceOwnerPackageName(&name);
        *result = mDeviceOwner != NULL
                && hasDeviceOwner
                && name.Equals(packageName);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetDeviceOwner(
    /* [out] */ String* result)
{
    if (!mHasFeature) {
        *result = String(NULL);
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        Boolean hasDeviceOwner;
        mDeviceOwner->HasDeviceOwner(&hasDeviceOwner);
        if (mDeviceOwner != NULL && hasDeviceOwner) {
            return mDeviceOwner->GetDeviceOwnerPackageName(result);
        }
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetDeviceOwnerName(
    /* [out] */ String* result)
{
    if (!mHasFeature) {
        *result = String(NULL);
        return NOERROR;
    }
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::MANAGE_USERS, String(NULL));
    {    AutoLock syncLock(this);
        if (mDeviceOwner != NULL) {
            return mDeviceOwner->GetDeviceOwnerName(result);
        }
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CDevicePolicyManagerService::RequireSecureKeyguard(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
   if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 passwordQuality;
    GetPasswordQuality(NULL, userHandle, &passwordQuality);
    if (passwordQuality > IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 encryptionStatus;
    GetStorageEncryptionStatus(userHandle, &encryptionStatus);
    if (encryptionStatus == IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVE
            || encryptionStatus == IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVATING) {
        *result = TRUE;
        return NOERROR;
    }

    // Keystore.isEmpty() requires system UID
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IKeyStoreHelper> helper;
        // TODO: Waiting for CKeyStoreHelper
        assert(0);
        // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
        AutoPtr<IKeyStore> keyStore;
        ec = helper->GetInstance((IKeyStore**)&keyStore);
        if (FAILED(ec)) break;
        Boolean isEmpty;
        ec = keyStore->IsEmpty(&isEmpty);
        if (FAILED(ec)) break;
        if (!isEmpty) {
            *result = TRUE;
            return NOERROR;
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }

    *result = FALSE;
    return NOERROR;
}

AutoPtr<CDevicePolicyManagerService::ActiveAdmin> CDevicePolicyManagerService::GetDeviceOwnerAdmin()
{
    String deviceOwnerPackageName;
    GetDeviceOwner(&deviceOwnerPackageName);
    if (deviceOwnerPackageName == NULL) {
        return NULL;
    }

    AutoPtr<DevicePolicyData> policy = GetUserData(IUserHandle::USER_OWNER);
    Int32 n;
    policy->mAdminList->GetSize(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> obj;
        policy->mAdminList->Get(i, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
        String packageName;
        admin->mInfo->GetPackageName(&packageName);
        if (deviceOwnerPackageName.Equals(packageName)) {
            return admin;
        }
    }
    return NULL;
}

ECode CDevicePolicyManagerService::ClearDeviceOwner(
    /* [in] */ const String& packageName)
{
    if (packageName == NULL) {
        Logger::E(LOG__TAG, "packageName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    ECode ec;
    do {
        AutoPtr<IPackageManager> packageManager;
        ec = mContext->GetPackageManager((IPackageManager**)&packageManager);
        if (FAILED(ec)) break;
        Int32 uid;
        ec = packageManager->GetPackageUid(packageName, 0, &uid);
        if (FAILED(ec)) break;
        if (uid != Binder::GetCallingUid()) {
            Logger::E(LOG__TAG, "Invalid packageName");
            return E_SECURITY_EXCEPTION;
        }
    } while(FALSE);
    // } catch (NameNotFoundException e) {
    if (FAILED(ec)) {
        if ((ECode) E_NAME_NOT_FOUND_EXCEPTION == ec) {
            Logger::E(LOG__TAG, "%d", ec);
            return E_SECURITY_EXCEPTION;
        }
        else
            return ec;
    }
    // }
    Boolean isDeviceOwner;
    IsDeviceOwner(packageName, &isDeviceOwner);
    if (!isDeviceOwner) {
        Logger::E(LOG__TAG, "clearDeviceOwner can only be called by the device owner");
        return E_SECURITY_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        do {
            ClearUserRestrictions(new UserHandle(IUserHandle::USER_OWNER));
            if (mDeviceOwner != NULL) {
                ec = mDeviceOwner->ClearDeviceOwner();
                if (FAILED(ec)) break;
                ec = mDeviceOwner->WriteOwnerFile();
            }
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
    }
    return ec;
}

ECode CDevicePolicyManagerService::SetProfileOwner(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& ownerName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::MANAGE_USERS, String(NULL));

    AutoPtr<IUserInfo> info;
    mUserManager->GetUserInfo(userHandle, (IUserInfo**)&info);
    if (info == NULL) {
        // User doesn't exist.
        Logger::E(LOG__TAG, "Attempted to set profile owner for invalid userId: %d", userHandle);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isGuest;
    info->IsGuest(&isGuest);
    if (isGuest) {
        Logger::E(LOG__TAG, "Cannot set a profile owner on a guest");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Boolean isInstalledForUser;
    String packageName;
    who->GetPackageName(&packageName);
    DeviceOwner::IsInstalledForUser(packageName, userHandle, &isInstalledForUser);
    if (who == NULL || !isInstalledForUser) {
        Logger::E(LOG__TAG, "Component %s not installed for userId:%d", TO_CSTR(who), userHandle);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        // Only SYSTEM_UID can override the userSetupComplete
        if (UserHandle::GetAppId(Binder::GetCallingUid()) != IProcess::SYSTEM_UID
                && HasUserSetupCompleted(userHandle)) {
            Logger::E(LOG__TAG, "Trying to set profile owner but user is already set-up.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        if (mDeviceOwner == NULL) {
            // Device owner state does not exist, create it.
            DeviceOwner::CreateWithProfileOwner(who, ownerName,
                    userHandle, (DeviceOwner**)&mDeviceOwner);
            mDeviceOwner->WriteOwnerFile();
            *result = TRUE;
            return NOERROR;
        } else {
            // Device owner already exists, update it.
            mDeviceOwner->SetProfileOwner(who, ownerName, userHandle);
            mDeviceOwner->WriteOwnerFile();
            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::ClearProfileOwner(
    /* [in] */ IComponentName* who)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    AutoPtr<IUserHandle> callingUser = Binder::GetCallingUserHandle();
    // Check if this is the profile owner who is calling
    AutoPtr<ActiveAdmin> objNoUse;
    GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
    ECode ec;
    {    AutoLock syncLock(this);
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        do {
            ClearUserRestrictions(callingUser);
            if (mDeviceOwner != NULL) {
                Int32 identifier;
                ec = callingUser->GetIdentifier(&identifier);
                if (FAILED(ec)) break;
                ec = mDeviceOwner->RemoveProfileOwner(identifier);
                if (FAILED(ec)) break;
                ec = mDeviceOwner->WriteOwnerFile();
            }
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
    }
    return ec;
}

void CDevicePolicyManagerService::ClearUserRestrictions(
    /* [in] */ IUserHandle* userHandle)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    AutoPtr<IBundle> userRestrictions;
    mUserManager->GetUserRestrictions((IBundle**)&userRestrictions);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    mUserManager->SetUserRestrictions(bundle, userHandle);
    Boolean b;
    userRestrictions->GetBoolean(IUserManager::DISALLOW_ADJUST_VOLUME, &b);
    if (b) {
        audioManager->SetMasterMute(FALSE);
    }
    userRestrictions->GetBoolean(IUserManager::DISALLOW_UNMUTE_MICROPHONE, &b);
    if (b) {
        audioManager->SetMicrophoneMute(FALSE);
    }
}

ECode CDevicePolicyManagerService::HasUserSetupCompleted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = HasUserSetupCompleted(UserHandle::GetCallingUserId());
    return NOERROR;
}

Boolean CDevicePolicyManagerService::HasUserSetupCompleted(
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return TRUE;
    }
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    // If policy is null, return true, else check if the setup has completed.
    return policy == NULL || policy->mUserSetupComplete;
}

ECode CDevicePolicyManagerService::SetProfileEnabled(
    /* [in] */ IComponentName* who)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    const Int32 userHandle = UserHandle::GetCallingUserId();
    ECode ec;
    {    AutoLock syncLock(this);
        // Check for permissions
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        // Check if this is the profile owner who is calling
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int32 userId = UserHandle::GetCallingUserId();

        Int64 id = Binder::ClearCallingIdentity();
        // try {
        do {
            // TODO: Waiting for IUserManager
            assert(0);
            // ec = mUserManager->SetUserEnabled(userId);
            if (FAILED(ec)) break;
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_MANAGED_PROFILE_ADDED, (IIntent**)&intent);
            AutoPtr<IUserHandle> newUserHandle;
            CUserHandle::New(userHandle, (IUserHandle**)&newUserHandle);
            ec = intent->PutExtra(IIntent::EXTRA_USER, IParcelable::Probe(newUserHandle));
            if (FAILED(ec)) break;
            ec = intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY |
                    IIntent::FLAG_RECEIVER_FOREGROUND);
            if (FAILED(ec)) break;
            // TODO This should send to parent of profile (which is always owner at the moment).
            ec = mContext->SendBroadcastAsUser(intent, IUserHandle::USER_OWNER);
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetProfileName(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& profileName)
{
    Int32 userId = UserHandle::GetCallingUserId();

    if (who == NULL) {
        Logger::E(LOG__TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    // Check if this is the profile owner (includes device owner).
    AutoPtr<ActiveAdmin> objNoUse;
    GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);

    Int64 id = Binder::ClearCallingIdentity();
    // try {
    ECode ec = mUserManager->SetUserName(userId, profileName);
    // } finally {
    Binder::RestoreCallingIdentity(id);
    // }
    return ec;
}

ECode CDevicePolicyManagerService::GetProfileOwner(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasFeature) {
        *result = NULL;
        return NOERROR;
    }

    {    AutoLock syncLock(this);
        if (mDeviceOwner != NULL) {
            return mDeviceOwner->GetProfileOwnerComponent(userHandle, result);
        }
    }
    *result = NULL;
    return NOERROR;
}

AutoPtr<CDevicePolicyManagerService::ActiveAdmin> CDevicePolicyManagerService::GetProfileOwnerAdmin(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IComponentName> profileOwner;
    if (mDeviceOwner != NULL)
        mDeviceOwner->GetProfileOwnerComponent(userHandle, (IComponentName**)&profileOwner);
    if (profileOwner == NULL) {
        return NULL;
    }

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    Int32 n;
    policy->mAdminList->GetSize(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> obj;
        policy->mAdminList->Get(i, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
        Boolean isEquals;
        IObject::Probe(profileOwner)->Equals(admin->mInfo, &isEquals);
        if (isEquals) {
            return admin;
        }
    }
    return NULL;
}


ECode CDevicePolicyManagerService::GetProfileOwnerName(
    /* [in] */ Int32 userHandle,
    /* [out] */ String* result)
{
    if (!mHasFeature) {
        *result = NULL;
        return NOERROR;
    }
    mContext->EnforceCallingOrSelfPermission(Manifest::permission::MANAGE_USERS, String(NULL));

    {    AutoLock syncLock(this);
        if (mDeviceOwner != NULL) {
            return mDeviceOwner->GetProfileOwnerName(userHandle, result);
        }
    }
    *result = NULL;
    return NOERROR;
}

Boolean CDevicePolicyManagerService::AllowedToSetDeviceOwnerOnDevice()
{
    Int32 callingId = Binder::GetCallingUid();
    if (callingId == IProcess::SHELL_UID || callingId == Process::ROOT_UID) {
        AutoPtr<IAccountManagerHelper> helper;
        CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper);
        AutoPtr<IAccountManager> accountManager;
        helper->Get(mContext, (IAccountManager**)&accountManager);
        AutoPtr<ArrayOf<IAccount*> > accounts;
        accountManager->GetAccounts((ArrayOf<IAccount*>**)&accounts);
        return accounts->GetLength() == 0;
    } else {
        Boolean hasUserSetupCompleted = HasUserSetupCompleted(IUserHandle::USER_OWNER);
        return !hasUserSetupCompleted;
    }
    return FALSE;
}

ECode CDevicePolicyManagerService::SetActiveAdmin(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Boolean refreshing,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    SetActiveAdmin(adminReceiver, refreshing, userHandle, NULL);
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetActiveAdmin(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Boolean refreshing,
    /* [in] */ Int32 userHandle,
    /* [in] */ IBundle* onEnableData)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_DEVICE_ADMINS, String(NULL)))
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<IDeviceAdminInfo> info;
    FindAdmin(adminReceiver, userHandle, (IDeviceAdminInfo**)&info);
    if (info == NULL) {
        Logger::E(LOG__TAG, "Bad admin: %s", TO_CSTR(adminReceiver));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {    AutoLock syncLock(this);
        Int64 ident = Binder::ClearCallingIdentity();

        if (!refreshing && GetActiveAdminUncheckedLocked(adminReceiver, userHandle) != NULL) {
            Logger::E(LOG__TAG, "Admin is already added");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<ActiveAdmin> newAdmin = new ActiveAdmin(info);
        policy->mAdminMap->Put(adminReceiver, TO_IINTERFACE(newAdmin));
        Int32 replaceIndex = -1;

        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i=0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> oldAdmin = (ActiveAdmin*) IObject::Probe(obj);
            AutoPtr<IComponentName> componentName;
            oldAdmin->mInfo->GetComponent((IComponentName**)&componentName);
            Boolean isEquals;
            IObject::Probe(componentName)->Equals(adminReceiver, &isEquals);
            if (isEquals) {
                replaceIndex = i;
                break;
            }
        }
        if (replaceIndex == -1) {
            policy->mAdminList->Add(TO_IINTERFACE(newAdmin));
            String packageName;
            info->GetPackageName(&packageName);
            EnableIfNecessary(packageName, userHandle);
        } else {
            policy->mAdminList->Set(replaceIndex, TO_IINTERFACE(newAdmin));
        }
        SaveSettingsLocked(userHandle);
        SendAdminCommandLocked(newAdmin, IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_ENABLED,
                            onEnableData, NULL);
        Binder::RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsAdminActive(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* isActive)
{
    VALIDATE_NOT_NULL(isActive)

    if (!mHasFeature) {
        *isActive = FALSE;
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
        *isActive = admin != NULL;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetActiveAdmins(
    /* [in] */ Int32 userHandle,
    /* [out] */ IList** admins)
{
    VALIDATE_NOT_NULL(admins)
    *admins = NULL;

    if (!mHasFeature) {
        AutoPtr<ICollections> collectionsHelper;
        CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
        return collectionsHelper->GetEmptyList(admins);
    }

    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        if (N <= 0) {
            *admins = NULL;
            return NOERROR;
        }
        AutoPtr<IArrayList> res;
        CArrayList::New(N, (IArrayList**)&res);
        for (Int32 i=0; i<N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<IComponentName> comp;
            ((ActiveAdmin*) IObject::Probe(obj))->mInfo->GetComponent((IComponentName**)&comp);
            res->Add(comp);
        }
        *admins = IList::Probe(res);
        REFCOUNT_ADD(*admins)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::PackageHasActiveAdmins(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* hasActive)
{
    VALIDATE_NOT_NULL(hasActive)
    *hasActive = FALSE;

    if (!mHasFeature) {
        *hasActive = FALSE;
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i=0; i<N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            String name;
            ((ActiveAdmin*) IObject::Probe(obj))->mInfo->GetPackageName(&name);
            if (name.Equals(packageName)) {
                *hasActive = TRUE;
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetRemoveWarning(
    /* [in] */ IComponentName* comp,
    /* [in] */ IRemoteCallback* result,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)))

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(comp, userHandle);
        if (admin == NULL) {
            result->SendResult(NULL);
            return NOERROR;
        }

        AutoPtr<IIntent> intent;
        CIntent::New(IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_DISABLE_REQUESTED, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        AutoPtr<IComponentName> component;
        admin->mInfo->GetComponent((IComponentName**)&component);
        intent->SetComponent(component);

        AutoPtr<IUserHandle> handle;
        CUserHandle::New(userHandle, (IUserHandle**)&handle);
        AutoPtr<IBroadcastReceiver> receiver = new RemoveWarningReceiver(result);
        mContext->SendOrderedBroadcastAsUser(intent, handle, String(NULL), receiver,
                NULL, IActivity::RESULT_OK, String(NULL), NULL);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::RemoveActiveAdmin(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
        if (admin == NULL) {
            return NOERROR;
        }
        if (admin->GetUid() != Binder::GetCallingUid()) {
            // If trying to remove device owner, refuse when the caller is not the owner.
            String packageName;
            adminReceiver->GetPackageName(&packageName);
            Boolean isDeviceOwner;
            IsDeviceOwner(packageName, &isDeviceOwner);
            if (isDeviceOwner) {
                return NOERROR;
            }
            FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::MANAGE_DEVICE_ADMINS, String(NULL)))
        }
        Int64 ident = Binder::ClearCallingIdentity();

        RemoveActiveAdminLocked(adminReceiver, userHandle);

        Binder::RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::HasGrantedPolicy(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 policyId,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* hasGrant)
{
    VALIDATE_NOT_NULL(hasGrant)
    *hasGrant = FALSE;

     if (!mHasFeature) {
        *hasGrant = FALSE;
        return NOERROR;
    }
   FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> administrator = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
        if (administrator == NULL) {
            Logger::E(LOG__TAG, "No active admin %s", TO_CSTR(adminReceiver));
            return E_SECURITY_EXCEPTION;
        }
        administrator->mInfo->UsesPolicy(policyId, hasGrant);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetActivePasswordState(
    /* [in] */ Int32 quality,
    /* [in] */ Int32 length,
    /* [in] */ Int32 letters,
    /* [in] */ Int32 uppercase,
    /* [in] */ Int32 lowercase,
    /* [in] */ Int32 numbers,
    /* [in] */ Int32 symbols,
    /* [in] */ Int32 nonletter,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    FAIL_RETURN(EnforceNotManagedProfile(userHandle, String("set the active password")))

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)))

    AutoPtr<DevicePolicyData> p = GetUserData(userHandle);
    ValidateQualityConstant(quality);

    ECode ec;
    {    AutoLock syncLock(this);
        if (p->mActivePasswordQuality != quality || p->mActivePasswordLength != length
                || p->mFailedPasswordAttempts != 0 || p->mActivePasswordLetters != letters
                || p->mActivePasswordUpperCase != uppercase
                || p->mActivePasswordLowerCase != lowercase
                || p->mActivePasswordNumeric != numbers
                || p->mActivePasswordSymbols != symbols
                || p->mActivePasswordNonLetter != nonletter) {
            Int64 ident = Binder::ClearCallingIdentity();

            // try {
            do {
                p->mActivePasswordQuality = quality;
                p->mActivePasswordLength = length;
                p->mActivePasswordLetters = letters;
                p->mActivePasswordLowerCase = lowercase;
                p->mActivePasswordUpperCase = uppercase;
                p->mActivePasswordNumeric = numbers;
                p->mActivePasswordSymbols = symbols;
                p->mActivePasswordNonLetter = nonletter;
                p->mFailedPasswordAttempts = 0;
                ec = SaveSettingsLocked(userHandle);
                if (FAILED(ec)) break;
                UpdatePasswordExpirationsLocked(userHandle);
                SetExpirationAlarmCheckLocked(mContext, p);
                SendAdminCommandToSelfAndProfilesLocked(IDeviceAdminReceiver::ACTION_PASSWORD_CHANGED, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, userHandle);
            } while(FALSE);
            // } finally {
            Binder::RestoreCallingIdentity(ident);
            // }
        }
    }
    return ec;
}

ECode CDevicePolicyManagerService::ReportFailedPasswordAttempt(
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    FAIL_RETURN(EnforceNotManagedProfile(userHandle, String("report failed password attempt")))
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)))

    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        Boolean wipeData = FALSE;
        Int32 identifier = 0;
        {    AutoLock syncLock(this);
            AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
            policy->mFailedPasswordAttempts++;
            ec = SaveSettingsLocked(userHandle);
            if (FAILED(ec)) break;
            if (mHasFeature) {
                AutoPtr<ActiveAdmin> strictestAdmin =
                        GetAdminWithMinimumFailedPasswordsForWipeLocked(userHandle);
                Int32 max = strictestAdmin != NULL
                        ? strictestAdmin->mMaximumFailedPasswordsForWipe : 0;
                if (max > 0 && policy->mFailedPasswordAttempts >= max) {
                    // Wipe the user/profile associated with the policy that was violated. This
                    // is not necessarily calling user: if the policy that fired was from a
                    // managed profile rather than the main user profile, we wipe former only.
                    wipeData = TRUE;
                    AutoPtr<IUserHandle> userHandle = strictestAdmin->GetUserHandle();
                    ec = userHandle->GetIdentifier(&identifier);
                    if (FAILED(ec)) break;
                }
                SendAdminCommandToSelfAndProfilesLocked(
                        IDeviceAdminReceiver::ACTION_PASSWORD_FAILED,
                        IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, userHandle);
            }
        }
        if (wipeData) {
            // Call without holding lock.
            WipeDeviceOrUserLocked(0, identifier, String("reportFailedPasswordAttempt()"));
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    return ec;
    // }
}

ECode CDevicePolicyManagerService::ReportSuccessfulPasswordAttempt(
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)))

    {    AutoLock syncLock(this);
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        if (policy->mFailedPasswordAttempts != 0 || policy->mPasswordOwner >= 0) {
            Int64 ident = Binder::ClearCallingIdentity();

            policy->mFailedPasswordAttempts = 0;
            policy->mPasswordOwner = -1;
            SaveSettingsLocked(userHandle);
            if (mHasFeature) {
                SendAdminCommandToSelfAndProfilesLocked(
                    IDeviceAdminReceiver::ACTION_PASSWORD_SUCCEEDED,
                    IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, userHandle);
            }

            Binder::RestoreCallingIdentity(ident);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::FindAdmin(
    /* [in] */ IComponentName* adminName,
    /* [in] */ Int32 userHandle,
    /* [out] */ IDeviceAdminInfo** adminInfo)
{
    VALIDATE_NOT_NULL(adminInfo)
    *adminInfo = NULL;

    if (!mHasFeature) {
        *adminInfo = NULL;
        return NOERROR;
    }
    FAIL_RETURN(EnforceCrossUserPermission(userHandle))

    AutoPtr<IIntent> resolveIntent;
    CIntent::New((IIntent**)&resolveIntent);
    resolveIntent->SetComponent(adminName);

    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);

    AutoPtr<IList> infos;
    pkgMgr->QueryBroadcastReceivers(
        resolveIntent,
        IPackageManager::GET_META_DATA | IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS,
        userHandle,
        (IList**)&infos);

    if (infos == NULL) {
        Logger::E(LOG__TAG, "Unknown admin: %s", TO_CSTR(adminName));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 count;
    infos->GetSize(&count);
    if (count <= 0) {
        Logger::E(LOG__TAG, "Unknown admin: %s", TO_CSTR(adminName));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInterface> value;
    infos->Get(0, (IInterface**)&value);
    AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(value);
    ECode ec;
    // TODO: Waiting for CDeviceAdminInfo
    assert(0);
    // ec = CDeviceAdminInfo::New(mContext, resolveInfo, adminInfo);

    if (FAILED(ec)) {
        StringBuffer buf;
        buf += "Bad device admin requested for user=";
        buf += userHandle;
        Slogger::I(LOG__TAG, buf.ToString());
        *adminInfo = NULL;
        return NOERROR;
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::SystemReady()
{
    if (!mHasFeature) {
        return NOERROR;
    }
    GetUserData(IUserHandle::USER_OWNER);
    LoadDeviceOwner();
    CleanUpOldUsers();
    // Register an observer for watching for user setup complete.
    AutoPtr<SetupContentObserver> observer;
    observer->constructor(mHandler);
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    observer->Register(contentResolver);
    // Initialize the user setup state, to handle the upgrade case.
    UpdateUserSetupComplete();

    // Update the screen capture disabled cache in the window manager
    AutoPtr<IList> users;
    mUserManager->GetUsers(TRUE, (IList**)&users);
    Int32 N;
    users->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        Int32 userHandle;
        IUserInfo::Probe(obj)->GetId(&userHandle);
        Boolean bScreenCaptureDisabled;
        GetScreenCaptureDisabled(NULL, userHandle, &bScreenCaptureDisabled);
        UpdateScreenCaptureDisabledInWindowManager(userHandle,
                bScreenCaptureDisabled);
    }
    return NOERROR;
}

void CDevicePolicyManagerService::CleanUpOldUsers()
{
    // This is needed in case the broadcast {@link Intent.ACTION_USER_REMOVED} was not handled
    // before reboot
    AutoPtr<ISet> usersWithProfileOwners;
    AutoPtr<ISet> usersWithData;
    {    AutoLock syncLock(this);
        if (mDeviceOwner != NULL)
            mDeviceOwner->GetProfileOwnerKeys((ISet**)&usersWithProfileOwners);
        else
            CHashSet::New((ISet**)&usersWithProfileOwners);
        CHashSet::New((ISet**)&usersWithData);
        Int32 size;
        mUserData->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            Int32 key;
            mUserData->KeyAt(i, &key);
            usersWithData->Add(CoreUtils::Convert(key));
        }
    }
    AutoPtr<IList> allUsers;
    mUserManager->GetUsers((IList**)&allUsers);

    AutoPtr<ISet> deletedUsers;
    CHashSet::New((ISet**)&deletedUsers);
    deletedUsers->AddAll(ICollection::Probe(usersWithProfileOwners));
    deletedUsers->AddAll(ICollection::Probe(usersWithData));
    AutoPtr<IIterator> it;
    ICollection::Probe(allUsers)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
        Int32 id;
        userInfo->GetId(&id);
        deletedUsers->Remove(CoreUtils::Convert(id));
    }
    AutoPtr<IIterator> iter;
    ICollection::Probe(deletedUsers)->GetIterator((IIterator**)&iter);
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IInteger32> userId = IInteger32::Probe(obj);
        Int32 iUserId;
        userId->GetValue(&iUserId);
        RemoveUserData(iUserId);
    }
}

ECode CDevicePolicyManagerService::EnforceCrossUserPermission(
    /* [in] */ Int32 userHandle)
{
    if (userHandle < 0) {
        Logger::E(LOG__TAG, "Invalid userId " + userHandle);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    const Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    Int32 userId;
    handleHelper->GetUserId(callingUid, &userId);

    if (userHandle == userId) return NOERROR;

    if (callingUid != IProcess::SYSTEM_UID && callingUid != 0) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL,
                String("Must be system or have INTERACT_ACROSS_USERS_FULL permission")));
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::EnforceSystemProcess(
    /* [in] */ const String& message)
{
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Logger::E(LOG__TAG, message);
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::EnforceNotManagedProfile(
    /* [in] */ Int32 userHandle,
    /* [in] */ const String& message)
{
    if(IsManagedProfile(userHandle)) {
        Logger::E(LOG__TAG, "You can not %s for a managed profile. ", message.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IUserInfo> CDevicePolicyManagerService::GetProfileParent(
    /* [in] */ Int32 userHandle)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUserInfo> rev;
    mUserManager->GetProfileParent(userHandle, (IUserInfo**)&rev);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return rev;
}

Boolean CDevicePolicyManagerService::IsManagedProfile(
    /* [in] */ Int32 userHandle)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IUserInfo> userInfo;
    mUserManager->GetUserInfo(userHandle, (IUserInfo**)&userInfo);
    Boolean rev;
    userInfo->IsManagedProfile(&rev);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return rev;
}

ECode CDevicePolicyManagerService::EnableIfNecessary(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    // try {
    ECode ec;
    do {
        AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
        AutoPtr<IApplicationInfo> ai;
        ec = ipm->GetApplicationInfo(packageName,
                IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS,
                userId, (IApplicationInfo**)&ai);
        if (FAILED(ec)) break;
        Int32 enabledSetting;
        ec = ai->GetEnabledSetting(&enabledSetting);
        if (FAILED(ec)) break;
        if (enabledSetting
                == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_UNTIL_USED) {
            ec = ipm->SetApplicationEnabledSetting(packageName,
                    IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT,
                    IPackageManager::DONT_KILL_APP, userId, String("DevicePolicyManager"));
        }
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetActiveAdminForCallerLocked(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 reqPolicy,
    /* [out] */ ActiveAdmin** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    Int32 userHandle;
    handleHelper->GetUserId(callingUid, &userHandle);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

    AutoPtr<IList> candidates;
    CArrayList::New((IList**)&candidates);

    // Build a list of admins for this uid matching the given ComponentName
    if (who != NULL) {
        AutoPtr<IInterface> obj;
        policy->mAdminMap->Get(who, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
        if (admin == NULL) {
            Logger::E(LOG__TAG, "No active admin %s", TO_CSTR(who));
            return E_SECURITY_EXCEPTION;
        }

        Int32 uid = admin->GetUid();
        if (uid != callingUid) {
            Logger::E(LOG__TAG, "Admin %s is not owned by uid %d", TO_CSTR(who), Binder::GetCallingUid());
            return E_SECURITY_EXCEPTION;
        }
        candidates->Add(TO_IINTERFACE(admin));
    } else {
        FOR_EACH(it, policy->mAdminList) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            if (admin->GetUid() == callingUid) {
                candidates->Add(TO_IINTERFACE(admin));
            }
        }
    }

    // Try to find an admin which can use reqPolicy
    AutoPtr<IIterator> it;
    ICollection::Probe(candidates)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
        String infoPackageName;
        admin->mInfo->GetPackageName(&infoPackageName);
        Boolean ownsDevice;
        IsDeviceOwner(infoPackageName, &ownsDevice);
        AutoPtr<IComponentName> componentName;
        GetProfileOwner(userHandle, (IComponentName**)&componentName);
        String packageName;
        componentName->GetPackageName(&packageName);
        Boolean ownsProfile = (componentName != NULL &&
                    packageName.Equals(infoPackageName));

        if (reqPolicy == IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER) {
            if (ownsDevice) {
                *result = admin;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        } else if (reqPolicy == IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER) {
            if (ownsDevice || ownsProfile) {
                *result = admin;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        } else {
            Boolean bUsesPolicy;
            admin->mInfo->UsesPolicy(reqPolicy, &bUsesPolicy);
            if (bUsesPolicy) {
                *result = admin;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }

    if (who != NULL) {
        AutoPtr<IInterface> obj;
        candidates->Get(0, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> candidate0 = (ActiveAdmin*) IObject::Probe(obj);
        AutoPtr<IComponentName> componentName;
        candidate0->mInfo->GetComponent((IComponentName**)&componentName);
        if (reqPolicy == IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER) {
            Logger::E(LOG__TAG, "Admin %s does not own the device", TO_CSTR(componentName));
            return E_SECURITY_EXCEPTION;
        }
        if (reqPolicy == IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER) {
            Logger::E(LOG__TAG, "Admin %s does not own the profile", TO_CSTR(componentName));
            return E_SECURITY_EXCEPTION;
        }
        String tagForPolicy;
        candidate0->mInfo->GetTagForPolicy(reqPolicy, &tagForPolicy);
        Logger::E(LOG__TAG, "Admin %s did not specify uses-policy for: %s", TO_CSTR(componentName), tagForPolicy.string());
        return E_SECURITY_EXCEPTION;
    } else {
        Logger::E(LOG__TAG, "No active admin owned by uid %d for policy #%d", Binder::GetCallingUid(), reqPolicy);
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<CDevicePolicyManagerService::DevicePolicyData> CDevicePolicyManagerService::GetUserData(
    /* [in] */ Int32 userHandle)
{
    {    AutoLock syncLock(this);
        AutoPtr<IInterface> obj;
        mUserData->Get(userHandle, (IInterface**)&obj);
        AutoPtr<DevicePolicyData> policy = (DevicePolicyData*) IObject::Probe(obj);
        if (policy == NULL) {
            policy = new DevicePolicyData(userHandle);
            mUserData->Append(userHandle, TO_IINTERFACE(policy));
            LoadSettingsLocked(policy, userHandle);
        }
        return policy;
    }
    return NULL;
}

void CDevicePolicyManagerService::RemoveUserData(
    /* [in] */ Int32 userHandle)
{
    {    AutoLock syncLock(this);

        if (userHandle == IUserHandle::USER_OWNER) {
            Slogger::W(LOG__TAG, "Tried to remove device policy file for user 0! Ignoring.");
            return;
        }
        if (mDeviceOwner != NULL) {
            mDeviceOwner->RemoveProfileOwner(userHandle);
            mDeviceOwner->WriteOwnerFile();
        }

        AutoPtr<IInterface> obj;
        mUserData->Get(userHandle, (IInterface**)&obj);
        AutoPtr<DevicePolicyData> policy = (DevicePolicyData*) IObject::Probe(obj);
        if (policy != NULL) {
            mUserData->Remove(userHandle);
        }

        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> sysDir;
        env->GetUserSystemDirectory(userHandle, (IFile**)&sysDir);
        AutoPtr<IFile> policyFile;
        CFile::New(sysDir, DEVICE_POLICIES_XML, (IFile**)&policyFile);

        Boolean deletedResult;
        policyFile->Delete(&deletedResult);

        String path;
        policyFile->GetAbsolutePath(&path);
        StringBuffer buf;
        buf += "Removed device policy file ";
        buf += path;
        Slogger::I(LOG__TAG, buf.ToString());
    }
    UpdateScreenCaptureDisabledInWindowManager(userHandle, FALSE /* default value */);
}

void CDevicePolicyManagerService::LoadDeviceOwner()
{
    {    AutoLock syncLock(this);
        DeviceOwner::Load((DeviceOwner**)&mDeviceOwner);
    }
}

ECode CDevicePolicyManagerService::LoadSettingsLocked(
    /* [in] */ DevicePolicyData* policy,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IJournaledFile> journal = MakeJournaledFile(userHandle);
    AutoPtr<IFile> file;
    journal->ChooseForRead((IFile**)&file);

    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(file, (IFileInputStream**)&stream);
    AutoPtr<IXmlPullParser> parser;
    Xml::NewPullParser((IXmlPullParser**)&parser);
    FAIL_RETURN(parser->SetInput(IInputStream::Probe(stream), String(NULL)))

    Int32 type;
    while (parser->Next(&type), type != IXmlPullParser::END_DOCUMENT && type != IXmlPullParser::START_TAG)
    {}

    String tag;
    FAIL_RETURN(parser->GetName(&tag))
    if (!tag.Equals("policies")) {
        Logger::E("Settings do not start with policies tag: found %s", tag.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }


    // Extract the permission provider component name if available
    String permissionProvider;
    parser->GetAttributeValue(String(NULL), ATTR_PERMISSION_PROVIDER, &permissionProvider);
    if (permissionProvider != NULL) {
        AutoPtr<IComponentNameHelper> cnHelper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
        cnHelper->UnflattenFromString(permissionProvider, (IComponentName**)&(policy->mRestrictionsProvider));
    }
    String userSetupComplete;
    parser->GetAttributeValue(String(NULL), ATTR_SETUP_COMPLETE, &userSetupComplete);
    if (userSetupComplete != NULL && StringUtils::ToString(TRUE).Equals(userSetupComplete)) {
        policy->mUserSetupComplete = TRUE;
    }

    FAIL_RETURN(parser->Next(&type))
    Int32 outerDepth;
    FAIL_RETURN(parser->GetDepth(&outerDepth))
    policy->mLockTaskPackages->Clear();
    policy->mAdminList->Clear();
    policy->mAdminMap->Clear();
    FAIL_RETURN(parser->Next(&type))
    Int32 currentDepth;
    while (parser->GetDepth(&currentDepth),
            type != IXmlPullParser::END_DOCUMENT && (type != IXmlPullParser::END_TAG || currentDepth > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        FAIL_RETURN(parser->GetName(&tag))
        if (tag.Equals("admin")) {
            String name;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &name))

            AutoPtr<IComponentNameHelper> cnHelper;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
            AutoPtr<IComponentName> cn;
            cnHelper->UnflattenFromString(name, (IComponentName**)&cn);
            AutoPtr<IDeviceAdminInfo> dai;
            FindAdmin(cn, userHandle, (IDeviceAdminInfo**)&dai);

            AutoPtr<IActivityInfo> aInfo;
            dai->GetActivityInfo((IActivityInfo**)&aInfo);
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            if (DBG && (UserHandle::GetUserId(uid) != userHandle)) {
                StringBuffer buf;
                buf += "findAdmin returned an incorrect uid ";
                buf += uid;
                buf += " for user ";
                buf += userHandle;
                Slogger::W(LOG__TAG, buf.ToString());
            }
            if (dai != NULL) {
                AutoPtr<ActiveAdmin> ap = new ActiveAdmin(dai);
                ap->ReadFromXml(parser);

                AutoPtr<IComponentName> comp;
                ap->mInfo->GetComponent((IComponentName**)&comp);
                policy->mAdminMap->Put(comp, TO_IINTERFACE(ap));
            }
        } else if (tag.Equals("failed-password-attempts")) {
            String str;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), ActiveAdmin::ATTR_VALUE, &str))
            policy->mFailedPasswordAttempts = StringUtils::ParseInt32(str);
            XmlUtils::SkipCurrentTag(parser);
        } else if (tag.Equals("password-owner")) {
            String str;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), ActiveAdmin::ATTR_VALUE, &str))
            policy->mPasswordOwner = StringUtils::ParseInt32(str);
            XmlUtils::SkipCurrentTag(parser);
        } else if (tag.Equals("active-password")) {
            String str;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("quality"), &str))
            policy->mActivePasswordQuality = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(), String("length"), &str))
            policy->mActivePasswordLength = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("uppercase"), &str))
            policy->mActivePasswordUpperCase = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("lowercase"), &str))
            policy->mActivePasswordLowerCase = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("letters"), &str))
            policy->mActivePasswordLetters = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("numeric"), &str))
            policy->mActivePasswordNumeric = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("symbols"), &str))
            policy->mActivePasswordSymbols = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("nonletter"), &str))
            policy->mActivePasswordNonLetter = StringUtils::ParseInt32(str);

            XmlUtils::SkipCurrentTag(parser);
        } else if (LOCK_TASK_COMPONENTS_XML.Equals(tag)) {
            String attrValue;
            parser->GetAttributeValue(String(NULL), String("name"), &attrValue);
            policy->mLockTaskPackages->Add(StringUtils::ParseCharSequence(attrValue));
            XmlUtils::SkipCurrentTag(parser);
        } else {
            StringBuffer buf;
            buf += "Unknown tag: ";
            buf += tag;
            Slogger::W(LOG__TAG, buf.ToString());
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    if (stream != NULL) {
        ICloseable::Probe(stream)->Close();
    }

    // Generate a list of admins from the admin map
    AutoPtr<ICollection> values;
    policy->mAdminMap->GetValues((ICollection**)&values);
    policy->mAdminList->AddAll(values);

    // Validate that what we stored for the password quality matches
    // sufficiently what is currently set.  Note that this is only
    // a sanity check in case the two get out of sync; this should
    // never normally happen.
    AutoPtr<ILockPatternUtils> utils;
    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&utils);
    Int32 quality;
    utils->GetActivePasswordQuality(&quality);
    if (quality < policy->mActivePasswordQuality) {
        StringBuffer buf;
        buf += "Active password quality 0x";
        buf += StringUtils::ToString(policy->mActivePasswordQuality, 16);
        buf += " does not match actual quality 0x";
        buf += StringUtils::ToString(quality, 16);
        Slogger::W(LOG__TAG, buf.ToString());

        policy->mActivePasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;
        policy->mActivePasswordLength = 0;
        policy->mActivePasswordUpperCase = 0;
        policy->mActivePasswordLowerCase = 0;
        policy->mActivePasswordLetters = 0;
        policy->mActivePasswordNumeric = 0;
        policy->mActivePasswordSymbols = 0;
        policy->mActivePasswordNonLetter = 0;
    }

    ValidatePasswordOwnerLocked(policy);
    SyncDeviceCapabilitiesLocked(policy);
    UpdateMaximumTimeToLockLocked(policy);
    return NOERROR;
}

ECode CDevicePolicyManagerService::SaveSettingsLocked(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<IJournaledFile> journal = MakeJournaledFile(userHandle);

    AutoPtr<IFile> fileFromJournal;
    journal->ChooseForWrite((IFile**)&fileFromJournal);
    // try {
    ECode ec;
    AutoPtr<IFileOutputStream> stream;
    do {
        ec = CFileOutputStream::New(fileFromJournal, FALSE, (IFileOutputStream**)&stream);
        if (FAILED(ec)) break;

        AutoPtr<IFastXmlSerializer> serializer;
        ec = CFastXmlSerializer::New((IFastXmlSerializer**)&serializer);
        if (FAILED(ec)) break;
        AutoPtr<IXmlSerializer> out = IXmlSerializer::Probe(serializer);
        ec = out->SetOutput(IOutputStream::Probe(stream), String("utf-8"));
        if (FAILED(ec)) break;
        ec = out->StartDocument(String(NULL), TRUE);
        if (FAILED(ec)) break;
        ec = out->WriteStartTag(String(NULL), String("policies"));
        if (FAILED(ec)) break;
        if (policy->mRestrictionsProvider != NULL) {
            String sFlatten;
            policy->mRestrictionsProvider->FlattenToString(&sFlatten);
            ec = out->WriteAttribute(String(NULL), ATTR_PERMISSION_PROVIDER, sFlatten);
            if (FAILED(ec)) break;
        }
        if (policy->mUserSetupComplete) {
            ec = out->WriteAttribute(String(NULL), ATTR_SETUP_COMPLETE,
                    StringUtils::ToString(TRUE));
            if (FAILED(ec)) break;
        }

        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i=0; i<N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> ap = (ActiveAdmin*) IObject::Probe(obj);
            if (ap != NULL) {
                ec = out->WriteStartTag(String(NULL), String("admin"));
                if (FAILED(ec)) break;
                AutoPtr<IComponentName> component;
                ap->mInfo->GetComponent((IComponentName**)&component);
                String str;
                component->FlattenToString(&str);
                ec = out->WriteAttribute(String(NULL), String("name"), str);
                if (FAILED(ec)) break;
                ap->WriteToXml(out);
                ec = out->WriteEndTag(String(NULL), String("admin"));
                if (FAILED(ec)) break;
            }
        }
        if (FAILED(ec)) break;

        if (policy->mPasswordOwner >= 0) {
            ec = out->WriteStartTag(String(NULL), String("password-owner"));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), ActiveAdmin::ATTR_VALUE, StringUtils::ToString(policy->mPasswordOwner));
            if (FAILED(ec)) break;
            ec = out->WriteEndTag(String(NULL), String("password-owner"));
            if (FAILED(ec)) break;
        }

        if (policy->mFailedPasswordAttempts != 0) {
            ec = out->WriteStartTag(String(NULL), String("failed-password-attempts"));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), ActiveAdmin::ATTR_VALUE, StringUtils::ToString(policy->mFailedPasswordAttempts));
            if (FAILED(ec)) break;
            ec = out->WriteEndTag(String(NULL), String("failed-password-attempts"));
            if (FAILED(ec)) break;
        }

        if (policy->mActivePasswordQuality != 0 || policy->mActivePasswordLength != 0
                || policy->mActivePasswordUpperCase != 0 || policy->mActivePasswordLowerCase != 0
                || policy->mActivePasswordLetters != 0 || policy->mActivePasswordNumeric != 0
                || policy->mActivePasswordSymbols != 0 || policy->mActivePasswordNonLetter != 0)
        {
            ec = out->WriteStartTag(String(NULL), String("active-password"));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("quality"), StringUtils::ToString(policy->mActivePasswordQuality));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("length"), StringUtils::ToString(policy->mActivePasswordLength));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("uppercase"), StringUtils::ToString(policy->mActivePasswordUpperCase));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("lowercase"), StringUtils::ToString(policy->mActivePasswordLowerCase));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("letters"), StringUtils::ToString(policy->mActivePasswordLetters));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("numeric"), StringUtils::ToString(policy->mActivePasswordNumeric));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("symbols"), StringUtils::ToString(policy->mActivePasswordSymbols));
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("nonletter"), StringUtils::ToString(policy->mActivePasswordNonLetter));
            if (FAILED(ec)) break;
            ec = out->WriteEndTag(String(NULL), String("active-password"));
            if (FAILED(ec)) break;
        }

        Int32 size;
        policy->mLockTaskPackages->GetSize(&size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> obj;
            policy->mLockTaskPackages->Get(i, (IInterface**)&obj);
            String component;
            IObject::Probe(obj)->ToString(&component);
            ec = out->WriteStartTag(String(NULL), LOCK_TASK_COMPONENTS_XML);
            if (FAILED(ec)) break;
            ec = out->WriteAttribute(String(NULL), String("name"), component);
            if (FAILED(ec)) break;
            ec = out->WriteEndTag(String(NULL), LOCK_TASK_COMPONENTS_XML);
            if (FAILED(ec)) break;
        }
        if (FAILED(ec)) break;

        ec = out->WriteEndTag(String(NULL), ActiveAdmin::TAG_POLICIES);
        if (FAILED(ec)) break;
        ec = out->EndDocument();
        if (FAILED(ec)) break;
        ec = ICloseable::Probe(stream)->Close();
        if (FAILED(ec)) break;
        ec = journal->Commit();
        if (FAILED(ec)) break;
        SendChangedNotification(userHandle);
    } while(FALSE);
    // } catch (IOException e) {
    if (FAILED(ec)) {
        if ((ECode) E_IO_EXCEPTION == ec) {
            // try {
            if (stream != NULL) {
                ICloseable::Probe(stream)->Close();
            }
            // } catch (IOException ex) {
                // Ignore
            // }
            journal->Rollback();
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

void CDevicePolicyManagerService::SendChangedNotification(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IDevicePolicyManager::ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    Int64 ident = Binder::ClearCallingIdentity();

    AutoPtr<IUserHandle> handle;
    CUserHandle::New(userHandle, (IUserHandle**)&handle);
    mContext->SendBroadcastAsUser(intent, handle);

    Binder::RestoreCallingIdentity(ident);
}

AutoPtr<CDevicePolicyManagerService::ActiveAdmin> CDevicePolicyManagerService::GetActiveAdminUncheckedLocked(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<IInterface> obj;
    policy->mAdminMap->Get(who, (IInterface**)&obj);
    AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
    if (admin != NULL) {
        String pkgName;
        who->GetPackageName(&pkgName);
        String clsName;
        who->GetClassName(&clsName);
        AutoPtr<IActivityInfo> activityInfo;
        admin->mInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        String infoPkgName;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&infoPkgName);
        String infoClsName;
        IPackageItemInfo::Probe(activityInfo)->GetName(&infoClsName);

        if (pkgName.Equals(infoPkgName) && clsName.Equals(infoClsName)) {
            return admin;
        }
    }

    return NULL;
}

void CDevicePolicyManagerService::UpdateMaximumTimeToLockLocked(
    /* [in] */ DevicePolicyData* policy)
{
    Int64 timeMs;
    GetMaximumTimeToLock(NULL, policy->mUserHandle, &timeMs);
    if (policy->mLastMaximumTimeToLock == timeMs) {
        return;
    }

    Int64 ident = Binder::ClearCallingIdentity();
    if (timeMs <= 0) {
        //timeMs = Integer.MAX_VALUE;
        timeMs = 0x7fffffff;
    } else {
        // Make sure KEEP_SCREEN_ON is disabled, since that
        // would allow bypassing of the maximum time to lock.
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsGlobal> globalSetting;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&globalSetting);
        Boolean result;
        globalSetting->PutInt32(resolver, ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, 0, &result);
    }

    policy->mLastMaximumTimeToLock = timeMs;
    ECode ec = GetIPowerManager()->SetMaximumScreenOffTimeoutFromDeviceAdmin((Int32)timeMs);
    if (FAILED(ec)) {
        Slogger::W(LOG__TAG, "Failure talking with power manager");
    }

    Binder::RestoreCallingIdentity(ident);
}

AutoPtr<IIPowerManager> CDevicePolicyManagerService::GetIPowerManager()
{
    if (mIPowerManager == NULL) {
        AutoPtr<IServiceManager> manager;
        CServiceManager::AcquireSingleton((IServiceManager**)&manager);
        AutoPtr<IInterface> b;
        manager->GetService(IContext::POWER_SERVICE, (IInterface**)&b);
        mIPowerManager = IIPowerManager::Probe(b);
    }
    return mIPowerManager;
}

AutoPtr<IIWindowManager> CDevicePolicyManagerService::GetWindowManager()
{
    if (mIWindowManager == NULL) {
        AutoPtr<IServiceManager> manager;
        CServiceManager::AcquireSingleton((IServiceManager**)&manager);
        AutoPtr<IInterface> b;
        manager->GetService(IContext::WINDOW_SERVICE, (IInterface**)&b);
        mIWindowManager = IIWindowManager::Probe(b);
    }
    return mIWindowManager;
}

AutoPtr<INotificationManager> CDevicePolicyManagerService::GetNotificationManager()
{
    if (mNotificationManager == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
        mNotificationManager = INotificationManager::Probe(obj);
    }
    return mNotificationManager;
}

void CDevicePolicyManagerService::SyncDeviceCapabilitiesLocked(
    /* [in] */ DevicePolicyData* policy)
{
    // Ensure the status of the camera is synced down to the system. Interested native services
    // should monitor this value and act accordingly.
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Boolean systemState;
    sysProp->GetBoolean(SYSTEM_PROP_DISABLE_CAMERA, FALSE, &systemState);
    Boolean cameraDisabled;
    GetCameraDisabled(NULL, policy->mUserHandle, &cameraDisabled);

    if (cameraDisabled != systemState) {
        Int64 token = Binder::ClearCallingIdentity();

        String value = cameraDisabled ? String("1") : String("0");
        StringBuffer buf;
        buf += "Change in camera state [";
        buf += SYSTEM_PROP_DISABLE_CAMERA;
        buf += "] = ";
        buf += value;
        Slogger::W(LOG__TAG, buf.ToString());

        sysProp->Set(SYSTEM_PROP_DISABLE_CAMERA, value);
        Binder::RestoreCallingIdentity(token);
    }
}

Boolean CDevicePolicyManagerService::IsEncryptionSupported()
{
    // Note, this can be implemented as
    //   return getEncryptionStatus() != DevicePolicyManager.ENCRYPTION_STATUS_UNSUPPORTED;
    // But is provided as a separate internal method if there's a faster way to do a
    // simple check for supported-or-not.
    Int32 encryptionStatus;
    GetEncryptionStatus(&encryptionStatus);
    return encryptionStatus != IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
}

ECode CDevicePolicyManagerService::GetEncryptionStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String pfeStatus;
    sysProp->Get(String("vold.pfe"), String(""), &pfeStatus);
    if (String("activated").EqualsIgnoreCase(pfeStatus)) {
        *result = IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
        return NOERROR;
    }

    String status;
    sysProp->Get(String("ro.crypto.state"), String("unsupported"), &status);

    if (status.EqualsIgnoreCase("encrypted")) {
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    Boolean isDeviceEncrypted;
    AutoPtr<ILockPatternUtilsHelper> helper;
    CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
    ECode ec = helper->IsDeviceEncrypted(&isDeviceEncrypted);
    *result = isDeviceEncrypted
                ? IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVE
                : IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE;
    // } finally {
    Binder::RestoreCallingIdentity(token);
    // }
    if (FAILED(ec)) return ec;
    } else if (status.EqualsIgnoreCase("unencrypted")) {
        *result = IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE;
        return NOERROR;
    } else {
        *result = IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
        return NOERROR;
    }
    return NOERROR;
}

void CDevicePolicyManagerService::SetEncryptionRequested(
    /* [in] */ Boolean encrypt)
{}

ECode CDevicePolicyManagerService::SetScreenCaptureDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [in] */ Boolean disabled)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    EnforceCrossUserPermission(userHandle);
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> ap;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&ap);
        if (ap->mDisableScreenCapture != disabled) {
            ap->mDisableScreenCapture = disabled;
            SaveSettingsLocked(userHandle);
            UpdateScreenCaptureDisabledInWindowManager(userHandle, disabled);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetScreenCaptureDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        if (who != NULL) {
            AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
            *result = (admin != NULL) ? admin->mDisableScreenCapture : FALSE;
            return NOERROR;
        }

        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            if (admin->mDisableScreenCapture) {
                *result = TRUE;
                return NOERROR;
            }
        }
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::UpdateScreenCaptureDisabledInWindowManager(
    /* [in] */ Int32 userHandle,
    /* [in] */ Boolean disabled)
{
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    ECode ec = GetWindowManager()->SetScreenCaptureDisabled(userHandle, disabled);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Logger::W(LOG__TAG, "Unable to notify WindowManager.%d", ec);
            ec = NOERROR;
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
    return ec;
}

ECode CDevicePolicyManagerService::SetAutoTimeRequired(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [in] */ Boolean required)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    EnforceCrossUserPermission(userHandle);
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&admin);
        if (admin->mRequireAutoTime != required) {
            admin->mRequireAutoTime = required;
            SaveSettingsLocked(userHandle);
        }
    }

    // Turn AUTO_TIME on in settings if it is required
    if (required) {
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            AutoPtr<IContentResolver> contentResolver;
            ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
            if (FAILED(ec)) break;
            Boolean bPutInt32;
            ec = Settings::Global::PutInt32(contentResolver,
                    ISettingsGlobal::AUTO_TIME, 1 /* AUTO_TIME on */, &bPutInt32);
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(ident);
        // }
        if (FAILED(ec)) return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetAutoTimeRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> deviceOwner = GetDeviceOwnerAdmin();
        *result = (deviceOwner != NULL) ? deviceOwner->mRequireAutoTime : FALSE;
    }
    return NOERROR;
}

void CDevicePolicyManagerService::ResetGlobalProxyLocked(
    /* [in] */ DevicePolicyData* policy)
{
    Int32 N;
    policy->mAdminList->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        policy->mAdminList->Get(i, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> ap = (ActiveAdmin*) IObject::Probe(obj);
        if (ap->mSpecifiesGlobalProxy) {
            SaveGlobalProxyLocked(ap->mGlobalProxySpec, ap->mGlobalProxyExclusionList);
            return;
        }

    }
    // No device admins defining global proxies - reset global proxy settings to none
    SaveGlobalProxyLocked(String(NULL), String(NULL));
}

void CDevicePolicyManagerService::SaveGlobalProxyLocked(
    /* [in] */ const String& proxySpec,
    /* [in] */ const String& exclusionList)
{
    String newExclusionList;

    if (exclusionList.IsNull()) {
        newExclusionList = String("");
    } else {
        newExclusionList = exclusionList;
    }

    String newProxySpec;

    if (proxySpec.IsNull()) {
        newProxySpec = String("");
    } else {
        newProxySpec = proxySpec;
    }

    // Remove white spaces
    newProxySpec = newProxySpec.Trim();
    AutoPtr< ArrayOf<String> > data;
    StringUtils::Split(newProxySpec, String(":"), (ArrayOf<String>**)&data);
    Int32 proxyPort = 8080;
    if (data->GetLength() > 1) {
        proxyPort = StringUtils::ParseInt32((*data)[1]);
    }

    newExclusionList = newExclusionList.Trim();
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);

    AutoPtr<IProxyInfo> proxyProperties;
    CProxyInfo::New((*data)[0], proxyPort, exclusionList, (IProxyInfo**)&proxyProperties);
    Boolean isValid;
    proxyProperties->IsValid(&isValid);
    if (!isValid) {
        Slogger::E(LOG__TAG, "Invalid proxy properties, ignoring: %s", TO_CSTR(proxyProperties));
        return;
    }
    AutoPtr<ISettingsGlobal> globalSetting;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&globalSetting);
    Boolean result;
    globalSetting->PutString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_HOST, (*data)[0], &result);
    globalSetting->PutInt32(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_PORT, proxyPort, &result);
    globalSetting->PutString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_EXCLUSION_LIST, newExclusionList, &result);
}

void CDevicePolicyManagerService::SetExpirationAlarmCheckLocked(
    /* [in] */ IContext* context,
    /* [in] */ DevicePolicyData* policy)
{
    Int64 expiration = GetPasswordExpirationLocked(NULL, policy->mUserHandle);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    Int64 timeToExpire = expiration - now;
    Int64 alarmTime;
    if (expiration == 0) {
        // No expirations are currently configured:  Cancel alarm.
        alarmTime = 0;
    } else if (timeToExpire <= 0) {
        // The password has already expired:  Repeat every 24 hours.
        alarmTime = now + MS_PER_DAY;
    } else {
        // Selecting the next alarm time:  Roll forward to the next 24 hour multiple before
        // the expiration time.
        Int64 alarmInterval = timeToExpire % MS_PER_DAY;
        if (alarmInterval == 0) {
            alarmInterval = MS_PER_DAY;
        }
        alarmTime = now + alarmInterval;
    }

    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&service);
    AutoPtr<IAlarmManager> am = IAlarmManager::Probe(service);

    AutoPtr<IPendingIntentHelper> intentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&intentHelper);
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_EXPIRED_PASSWORD_NOTIFICATION, (IIntent**)&intent);
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(policy->mUserHandle, (IUserHandle**)&userHandle);
    AutoPtr<IPendingIntent> pi;
    intentHelper->GetBroadcastAsUser(context, REQUEST_EXPIRE_PASSWORD,
            intent, IPendingIntent::FLAG_ONE_SHOT | IPendingIntent::FLAG_UPDATE_CURRENT,
            userHandle, (IPendingIntent**)&pi);

    am->Cancel(pi);
    if (alarmTime != 0) {
        am->Set(IAlarmManager::RTC, alarmTime, pi);
    }

    Binder::RestoreCallingIdentity(token);
}

Int64 CDevicePolicyManagerService::GetPasswordExpirationLocked(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle)
{
    Int64 timeout = 0L;

    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
        return admin != NULL ? admin->mPasswordExpirationDate : timeout;
    }

    AutoPtr<IList> profiles;
    mUserManager->GetProfiles(userHandle, (IList**)&profiles);
    AutoPtr<IIterator> it;
    ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
        AutoPtr<IUserHandle> userHandle;
        userInfo->GetUserHandle((IUserHandle**)&userHandle);
        Int32 identifier;
        userHandle->GetIdentifier(&identifier);
        AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            if (timeout == 0L || (admin->mPasswordExpirationDate != 0
                    && timeout > admin->mPasswordExpirationDate)) {
                timeout = admin->mPasswordExpirationDate;
            }
        }
    }

    return timeout;
}

void CDevicePolicyManagerService::WipeDeviceOrUserLocked(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle,
    /* [in] */ const String& reason)
{
    if (userHandle == IUserHandle::USER_OWNER) {
        WipeDataLocked(flags, reason);
    }
    else {
        AutoPtr<IRunnable> runnable = new DeviceOrUserLockedRunnable(this, userHandle);
        Boolean result;
        mHandler->Post(runnable, &result);
    }
}

void CDevicePolicyManagerService::SendAdminCommandLocked(
    /* [in] */ ActiveAdmin* admin,
    /* [in] */ const String& action)
{
    SendAdminCommandLocked(admin, action, NULL);
}

void CDevicePolicyManagerService::SendAdminCommandLocked(
    /* [in] */ ActiveAdmin* admin,
    /* [in] */ const String& action,
    /* [in] */ IBroadcastReceiver* result)
{
    SendAdminCommandLocked(admin, action, NULL, result);
}

void CDevicePolicyManagerService::SendAdminCommandLocked(
    /* [in] */ ActiveAdmin* admin,
    /* [in] */ const String& action,
    /* [in] */ IBundle* adminExtras,
    /* [in] */ IBroadcastReceiver* result)
{
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);

    AutoPtr<IComponentName> comp;
    admin->mInfo->GetComponent((IComponentName**)&comp);
    intent->SetComponent(comp);


    if (action.Equals(IDeviceAdminReceiver::ACTION_PASSWORD_EXPIRING)) {
        intent->PutExtra(String("expiration"), admin->mPasswordExpirationDate);
    }

    if (adminExtras != NULL) {
        intent->PutExtras(adminExtras);
    }
    if (result != NULL) {
        AutoPtr<IUserHandle> userHandle = admin->GetUserHandle();
        mContext->SendOrderedBroadcastAsUser(intent, userHandle,
                String(NULL), result, mHandler, IActivity::RESULT_OK, String(NULL), NULL);
    } else {
        AutoPtr<IUserHandle> userHandle = admin->GetUserHandle();
        mContext->SendBroadcastAsUser(intent, userHandle);
    }
}

void CDevicePolicyManagerService::SendAdminCommandLocked(
    /* [in] */ const String& action,
    /* [in] */ Int32 reqPolicy,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    Int32 count;
    policy->mAdminList->GetSize(&count);
    if (count > 0) {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            Boolean result;
            admin->mInfo->UsesPolicy(reqPolicy, &result);
            if (result) {
                SendAdminCommandLocked(admin, action);
            }
        }
    }
}

void CDevicePolicyManagerService::SendAdminCommandToSelfAndProfilesLocked(
    /* [in] */ const String& action,
    /* [in] */ Int32 reqPolicy,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IList> profiles;
    mUserManager->GetProfiles(userHandle, (IList**)&profiles);
    AutoPtr<IIterator> it;
    ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> ui = IUserInfo::Probe(obj);
        AutoPtr<IUserHandle> userHandle;
        ui->GetUserHandle((IUserHandle**)&userHandle);
        Int32 id;
        userHandle->GetIdentifier(&id);
        SendAdminCommandLocked(action, reqPolicy, id);
    }
}

void CDevicePolicyManagerService::RemoveActiveAdminLocked(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
    if (admin != NULL) {
        AutoPtr<IBroadcastReceiver> receiver = new ActiveAdminLockedReceiver(this, admin, adminReceiver);
        SendAdminCommandLocked(admin, IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_DISABLED, receiver);
    }
}

void CDevicePolicyManagerService::LockNowUnchecked()
{
    Int64 ident = Binder::ClearCallingIdentity();

    // Power off the display
    AutoPtr<IIPowerManager> powerMgr = GetIPowerManager();
    powerMgr->GoToSleep(SystemClock::GetUptimeMillis(), IPowerManager::GO_TO_SLEEP_REASON_DEVICE_ADMIN, 0);
    // Ensure the device is locked
    AutoPtr<ILockPatternUtils> utils;
    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&utils);
    utils->RequireCredentialEntry(IUserHandle::USER_ALL);
    GetWindowManager()->LockNow(NULL);
    Binder::RestoreCallingIdentity(ident);
}

void CDevicePolicyManagerService::HandlePackagesChanged(
    /* [in] */ Int32 userHandle)
{
    Boolean removed = FALSE;

    if (DBG) {
        StringBuffer buf;
        buf += "Handling package changes for user ";
        buf += userHandle;
        Slogger::W(LOG__TAG, buf.ToString());
    }

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    Int32 size;
    policy->mAdminList->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        policy->mAdminList->Get(i, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> aa = (ActiveAdmin*) IObject::Probe(obj);

        String name;
        aa->mInfo->GetPackageName(&name);
        AutoPtr<IComponentName> comp;
        aa->mInfo->GetComponent((IComponentName**)&comp);

        AutoPtr<IPackageInfo> pkgInfo;
        pm->GetPackageInfo(name, 0, userHandle, (IPackageInfo**)&pkgInfo);
        AutoPtr<IActivityInfo> receiverInfo;
        pm->GetReceiverInfo(comp, 0, userHandle, (IActivityInfo**)&receiverInfo);
        if (pkgInfo == NULL || receiverInfo == NULL) {
            removed = TRUE;
            policy->mAdminList->Remove(i);
            AutoPtr<IComponentName> componentName;
            aa->mInfo->GetComponent((IComponentName**)&componentName);
            policy->mAdminMap->Remove(componentName);
        }
    }
    if (removed) {
        ValidatePasswordOwnerLocked(policy);
        SyncDeviceCapabilitiesLocked(policy);
        SaveSettingsLocked(policy->mUserHandle);
    }
}

void CDevicePolicyManagerService::ValidatePasswordOwnerLocked(
    /* [in] */ DevicePolicyData* policy)
{
    if (policy->mPasswordOwner < 0) {
        return;
    }

    Boolean haveOwner = FALSE;
    Int32 size;
    policy->mAdminList->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        policy->mAdminList->Get(i, (IInterface**)&obj);
        AutoPtr<ActiveAdmin> aa = (ActiveAdmin*) IObject::Probe(obj);
        if (aa->GetUid() == policy->mPasswordOwner) {
            haveOwner = TRUE;
            break;
        }
    }
    if (!haveOwner) {
        StringBuffer buf;
        buf += "Previous password owner ";
        buf += policy->mPasswordOwner;
        buf += " no longer active; disabling";
        Slogger::W(LOG__TAG, buf.ToString());

        policy->mPasswordOwner = -1;
    }
}

void CDevicePolicyManagerService::HandlePasswordExpirationNotification(
    /* [in] */ Int32 userHandle)
{
    {    AutoLock syncLock(this);
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);

        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> ui = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            ui->GetUserHandle((IUserHandle**)&userHandle);
            Int32 profileUserHandle;
            userHandle->GetIdentifier(&profileUserHandle);;
            AutoPtr<DevicePolicyData> policy = GetUserData(profileUserHandle);
            Int32 count;
            policy->mAdminList->GetSize(&count);
            if (count > 0) {
                for (Int32 i = 0; i < count; i++) {
                    AutoPtr<IInterface> obj;
                    policy->mAdminList->Get(i, (IInterface**)&obj);
                    AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                    Boolean bUsesPolicy;
                    admin->mInfo->UsesPolicy(IDeviceAdminInfo::USES_POLICY_EXPIRE_PASSWORD, &bUsesPolicy);
                    if (bUsesPolicy
                            && admin->mPasswordExpirationTimeout > 0L
                            && now >= admin->mPasswordExpirationDate - EXPIRATION_GRACE_PERIOD_MS
                            && admin->mPasswordExpirationDate > 0L) {
                        SendAdminCommandLocked(admin,
                                IDeviceAdminReceiver::ACTION_PASSWORD_EXPIRING);
                    }
                }
            }
        }
        SetExpirationAlarmCheckLocked(mContext, GetUserData(userHandle));
    }
}

void CDevicePolicyManagerService::UpdatePasswordExpirationsLocked(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<IList> profiles;
    mUserManager->GetProfiles(userHandle, (IList**)&profiles);
    AutoPtr<IIterator> it;
    ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
        AutoPtr<IUserHandle> iUserHandle;
        userInfo->GetUserHandle((IUserHandle**)&iUserHandle);
        Int32 profileId;
        iUserHandle->GetIdentifier(&profileId);;
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        if (N > 0) {
            for (Int32 i = 0; i < N ; ++i) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                Boolean result;
                admin->mInfo->UsesPolicy(IDeviceAdminInfo::USES_POLICY_EXPIRE_PASSWORD, &result);
                if (result) {
                    Int64 timeout = admin->mPasswordExpirationTimeout;
                    AutoPtr<ISystem> system;
                    CSystem::AcquireSingleton((ISystem**)&system);
                    Int64 millis;
                    system->GetCurrentTimeMillis(&millis);
                    Int64 expiration = timeout > 0L ? (timeout + millis) : 0L;
                    admin->mPasswordExpirationDate = expiration;
                }
            }
        }
        SaveSettingsLocked(userHandle);
    }
}

void CDevicePolicyManagerService::WipeDataLocked(
    /* [in] */ Int32 flags,
    /* [in] */ const String& reason)
{
    // If the SD card is encrypted and non-removable, we have to force a wipe.
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    Boolean isEmulated;
    env->IsExternalStorageEmulated(&isEmulated);
    Boolean forceExtWipe = !isEmulated && IsExtStorageEncrypted();
    Boolean wipeExtRequested = (flags & IDevicePolicyManager::WIPE_EXTERNAL_STORAGE) != 0;

    // Note: we can only do the wipe via ExternalStorageFormatter if the volume is not emulated.
    if ((forceExtWipe || wipeExtRequested) && !isEmulated) {
        AutoPtr<IIntent> intent;
        CIntent::New(IExternalStorageFormatter::FORMAT_AND_FACTORY_RESET, (IIntent**)&intent);
        intent->PutBooleanExtra(IExternalStorageFormatter::EXTRA_ALWAYS_RESET, TRUE);
        intent->PutExtra(IIntent::EXTRA_REASON, reason);
        // TODO: Waiting for ExternalStorageFormatter
        assert(0);
        // intent->SetComponent(IExternalStorageFormatter::COMPONENT_NAME);
        mWakeLock->AcquireLock(10000);

        AutoPtr<IComponentName> tmpComp;
        mContext->StartService(intent, (IComponentName**)&tmpComp);
    } else {
        AutoPtr<IRecoverySystem> helper;
        CRecoverySystem::AcquireSingleton((IRecoverySystem**)&helper);
        ECode ec = helper->RebootWipeUserData(mContext);
        if (FAILED(ec)) {
            if ((ECode) E_IO_EXCEPTION == ec) {
                Slogger::W(LOG__TAG, "Failed requesting data wipe");
            }
            else if ((ECode) E_SECURITY_EXCEPTION == ec) {
                Slogger::W(LOG__TAG, "Failed requesting data wipe %d", ec);
            }
        }
    }
}

Boolean CDevicePolicyManagerService::IsExtStorageEncrypted()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String state;
    sysProp->Get(String("vold.decrypt"), &state);
    return !state.Equals("");
}

ECode CDevicePolicyManagerService::EnforceCanManageCaCerts(
    /* [in] */ IComponentName* who)
{
    if (who == NULL) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::MANAGE_CA_CERTIFICATES, String(NULL));
    } else {
        {    AutoLock syncLock(this);
            AutoPtr<ActiveAdmin> objNoUse;
            GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::InstallCaCert(
    /* [in] */ IComponentName* admin,
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    EnforceCanManageCaCerts(admin);

    AutoPtr<ArrayOf<Byte> > pemCert;
    ECode ec;
    // try {
    do {
        AutoPtr<IX509Certificate> cert;
        ec = ParseCert(certBuffer, (IX509Certificate**)&cert);
        if (FAILED(ec)) break;
        // TODO: Waiting for Credentials
        assert(0);
        // ec = Credentials::ConvertToPem(cert, (ArrayOf<Byte>**)&pemCert);
    } while(FALSE);
    if (FAILED(ec)) {
    // } catch (CertificateException ce) {
        if ((ECode) E_CERTIFICATE_EXCEPTION == ec) {
            Logger::E(LOG__TAG, "Problem converting cert %d", ec);
            *result = FALSE;
            return NOERROR;
        }
    // } catch (IOException ioe) {
        if ((ECode) E_IO_EXCEPTION == ec) {
            Logger::E(LOG__TAG, "Problem reading cert %d", ec);
            *result = FALSE;
            return NOERROR;
        }
        return ec;
    // }
    }

    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(UserHandle::GetCallingUserId(), (IUserHandle**)&userHandle);
    Int64 id = Binder::ClearCallingIdentity();
    // try {
    do {
        AutoPtr<IKeyChainConnection> keyChainConnection;
        // TODO: Waiting for KeyChain
        assert(0);
        // ec = KeyChain::BindAsUser(mContext, userHandle, (IKeyChainConnection**)&keyChainConnection);
        if (FAILED(ec)) break;
        // try {
        do {
            AutoPtr<IIKeyChainService> service;
            ec = keyChainConnection->GetService((IIKeyChainService**)&service);
            if (FAILED(ec)) break;
            ec = service->InstallCaCertificate(pemCert);
            if (FAILED(ec)) break;
            *result = TRUE;
            return NOERROR;
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Logger::E(LOG__TAG, "installCaCertsToKeyChain(): %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        ICloseable::Probe(keyChainConnection)->Close();
        // }
    } while(FALSE);
    // } catch (InterruptedException e1) {
    if (FAILED(ec)) {
        if ((ECode) E_INTERRUPTED_EXCEPTION == ec) {
            Logger::W(LOG__TAG, "installCaCertsToKeyChain(): %d", ec);
            Thread::GetCurrentThread()->Interrupt();
            ec = NOERROR;
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(id);
    if (FAILED(ec)) return ec;
    // }
    *result = FALSE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::ParseCert(
    /* [in] */ ArrayOf<Byte>* certBuffer,
    /* [out] */ IX509Certificate** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> certFactory;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);
    AutoPtr<ICertificate> cert;
    AutoPtr<IByteArrayInputStream> stream;
    CByteArrayInputStream::New(certBuffer, (IByteArrayInputStream**)&stream);
    certFactory->GenerateCertificate(IInputStream::Probe(stream), (ICertificate**)&cert);
    *result = IX509Certificate::Probe(cert);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDevicePolicyManagerService::UninstallCaCert(
    /* [in] */ IComponentName* admin,
    /* [in] */ const String& alias)
{
    EnforceCanManageCaCerts(admin);

    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(UserHandle::GetCallingUserId(), (IUserHandle**)&userHandle);
    Int64 id = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IKeyChainConnection> keyChainConnection;
        // TODO: Waiting for KeyChain
        assert(0);
        // ec = KeyChain::BindAsUser(mContext, userHandle, (IKeyChainConnection**)&keyChainConnection);
        if (FAILED(ec)) break;
        // try {
        do {
            AutoPtr<IIKeyChainService> service;
            ec = keyChainConnection->GetService((IIKeyChainService**)&service);
            if (FAILED(ec)) break;
            Boolean bNoUse;
            ec = service->DeleteCaCertificate(alias, &bNoUse);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Logger::E(LOG__TAG, "from CaCertUninstaller: %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        ICloseable::Probe(keyChainConnection)->Close();
        // }
    } while(FALSE);
    // } catch (InterruptedException ie) {
    if (FAILED(ec)) {
        if ((ECode) E_INTERRUPTED_EXCEPTION == ec) {
            Logger::W(LOG__TAG, "CaCertUninstaller: %d", ec);
            Thread::GetCurrentThread()->Interrupt();
            ec = NOERROR;
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(id);
    // }
    return ec;
}

ECode CDevicePolicyManagerService::InstallKeyPair(
    /* [in] */ IComponentName* who,
    /* [in] */ ArrayOf<Byte>* privKey,
    /* [in] */ ArrayOf<Byte>* cert,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    if (who == NULL) {
        Logger::E(LOG__TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
    }
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(UserHandle::GetCallingUserId(), (IUserHandle**)&userHandle);
    const Int64 id = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IKeyChainConnection> keyChainConnection;
        // TODO: Waiting for KeyChain
        assert(0);
        // ec = KeyChain::BindAsUser(mContext, userHandle, (IKeyChainConnection**)&keyChainConnection);
        if (FAILED(ec)) break;
        // try {
        do {
            AutoPtr<IIKeyChainService> keyChain;
            ec = keyChainConnection->GetService((IIKeyChainService**)&keyChain);
            if (FAILED(ec)) break;
            ec = keyChain->InstallKeyPair(privKey, cert, alias, result);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Logger::E(LOG__TAG, "Installing certificate %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        ICloseable::Probe(keyChainConnection)->Close();
        // }
    } while(FALSE);
    // } catch (InterruptedException e) {
    if (FAILED(ec)) {
        if ((ECode) E_INTERRUPTED_EXCEPTION == ec) {
            Logger::W(LOG__TAG, "Interrupted while installing certificate %d", ec);
            Thread::GetCurrentThread()->Interrupt();
            ec = NOERROR;
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(id);
    if (FAILED(ec)) return ec;
    // }
    *result = FALSE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::ValidateQualityConstant(
    /* [in] */ Int32 quality)
{
    switch (quality) {
        case IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED:
        case IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK:
        case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
            return NOERROR;
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

AutoPtr<IJournaledFile> CDevicePolicyManagerService::MakeJournaledFile(
    /* [in] */ Int32 userHandle)
{
    String base;
    if (userHandle == 0) {
        base = String("/data/system/") + DEVICE_POLICIES_XML;
    } else {
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> sysDir;
        env->GetUserSystemDirectory(userHandle, (IFile**)&sysDir);
        AutoPtr<IFile> file;
        CFile::New(sysDir, DEVICE_POLICIES_XML, (IFile**)&file);
        file->GetAbsolutePath(&base);
    }

    AutoPtr<IFile> bFile;
    CFile::New(base, (IFile**)&bFile);
    AutoPtr<IFile> tFile;
    CFile::New(base + ".tmp", (IFile**)&tFile);
    AutoPtr<IJournaledFile> journaledFile;
    CJournaledFile::New(bFile, tFile, (IJournaledFile**)&journaledFile);
    return journaledFile;
}

void CDevicePolicyManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 permissionId;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &permissionId);
    if (permissionId != IPackageManager::PERMISSION_GRANTED) {

        String s;
        s.AppendFormat("Permission Denial: can't dump DevicePolicyManagerService from from pid=%d, uid=%d",
                Binder::GetCallingPid(), Binder::GetCallingUid());
        pw->Println(s);
        return;
    }

    AutoPtr<IPrinter> p;
    // TODO: Waiting for CPrintWriterPrinter
    assert(0);
    // CPrintWriterPrinter::New(pw, (IPrinter**)&p);

    {    AutoLock syncLock(this);
        p->Println(String("Current Device Policy Manager state:"));

        Int32 userCount;
        mUserData->GetSize(&userCount);
        for (Int32 u = 0; u < userCount; u++) {
            Int32 key;
            mUserData->KeyAt(u, &key);
            AutoPtr<DevicePolicyData> policy = GetUserData(key);
            String s;
            s.AppendFormat("  Enabled Device Admins (User %d):", policy->mUserHandle);
            p->Println(s);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> ap = (ActiveAdmin*) IObject::Probe(obj);
                if (ap != NULL) {
                    pw->Print(String("  "));
                    AutoPtr<IComponentName> componentName;
                    ap->mInfo->GetComponent((IComponentName**)&componentName);
                    String sFlatten;
                    componentName->FlattenToShortString(&sFlatten);
                    pw->Print(sFlatten);
                    pw->Println(String(":"));
                    ap->Dump(String("    "), pw);
                }
            }

            pw->Println(String(" "));
            pw->Print(String("  mPasswordOwner="));
            pw->Println(policy->mPasswordOwner);
        }
    }
}

AutoPtr<ISet> CDevicePolicyManagerService::InitDEVICE_OWNER_USER_RESTRICTIONS()
{
    AutoPtr<ISet> rev;
    CHashSet::New((ISet**)&rev);
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_USB_FILE_TRANSFER));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_CONFIG_TETHERING));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_FACTORY_RESET));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_ADD_USER));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_CONFIG_CELL_BROADCASTS));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_CONFIG_MOBILE_NETWORKS));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_MOUNT_PHYSICAL_MEDIA));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_UNMUTE_MICROPHONE));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_ADJUST_VOLUME));
    rev->Add(StringUtils::ParseCharSequence(IUserManager::DISALLOW_SMS));
    return rev;
}

AutoPtr<ISet> CDevicePolicyManagerService::InitSECURE_SETTINGS_WHITELIST()
{
    AutoPtr<ISet> rev;
    CHashSet::New((ISet**)&rev);
    rev->Add(StringUtils::ParseCharSequence(ISettingsSecure::DEFAULT_INPUT_METHOD));
    rev->Add(StringUtils::ParseCharSequence(ISettingsSecure::SKIP_FIRST_USE_HINTS));
    return rev;
}

AutoPtr<ISet> CDevicePolicyManagerService::InitSECURE_SETTINGS_DEVICEOWNER_WHITELIST()
{
    AutoPtr<ISet> rev;
    CHashSet::New((ISet**)&rev);
    rev->AddAll(ICollection::Probe(SECURE_SETTINGS_WHITELIST));
    rev->Add(StringUtils::ParseCharSequence(ISettingsSecure::LOCATION_MODE));
    return rev;
}

AutoPtr<ISet> CDevicePolicyManagerService::InitGLOBAL_SETTINGS_WHITELIST()
{
    AutoPtr<ISet> rev;
    CHashSet::New((ISet**)&rev);
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::ADB_ENABLED));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::AUTO_TIME));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::AUTO_TIME_ZONE));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::BLUETOOTH_ON));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::DATA_ROAMING));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::MODE_RINGER));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::NETWORK_PREFERENCE));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::USB_MASS_STORAGE_ENABLED));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::WIFI_ON));
    rev->Add(StringUtils::ParseCharSequence(ISettingsGlobal::WIFI_SLEEP_POLICY));
    return rev;
}

ECode CDevicePolicyManagerService::AddPersistentPreferredActivity(
    /* [in] */ IComponentName* who,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IComponentName* activity)
{
    const Int32 userHandle = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec = pm->AddPersistentPreferredActivity(filter, activity, userHandle);
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // Shouldn't happen
            ec = NOERROR;
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::ClearPackagePersistentPreferredActivities(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName)
{
    const Int32 userHandle = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec = pm->ClearPackagePersistentPreferredActivities(packageName, userHandle);
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // Shouldn't happen
            ec = NOERROR;
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetApplicationRestrictions(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName,
    /* [in] */ IBundle* settings)
{
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(UserHandle::GetCallingUserId(), (IUserHandle**)&userHandle);
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        // TODO: Waiting for IUserManager
        assert(0);
        // ec = mUserManager->SetApplicationRestrictions(packageName, settings, userHandle);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        // }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetTrustAgentFeaturesEnabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* agent,
    /* [in] */ IList* features,
    /* [in] */ Int32 userHandle)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    EnforceCrossUserPermission(userHandle);
    EnforceNotManagedProfile(userHandle, String("manage trust agent features"));
    {    AutoLock syncLock(this);
        if (admin == NULL) {
            Logger::E(LOG__TAG, "admin is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        if (agent == NULL) {
            Logger::E(LOG__TAG, "agent is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> ap;
        GetActiveAdminForCallerLocked(admin,
                IDeviceAdminInfo::USES_POLICY_DISABLE_KEYGUARD_FEATURES,
                (ActiveAdmin**)&ap);
        String sFlatten;
        agent->FlattenToString(&sFlatten);
        ap->mTrustAgentFeatures->Put(StringUtils::ParseCharSequence(sFlatten), features);
        SaveSettingsLocked(userHandle);
        SyncDeviceCapabilitiesLocked(GetUserData(userHandle));
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetTrustAgentFeaturesEnabled(
    /* [in] */ IComponentName* admin,
    /* [in] */ IComponentName* agent,
    /* [in] */ Int32 userHandle,
    /* [out] */ IList** features)
{
    VALIDATE_NOT_NULL(features)
    *features = NULL;

    if (!mHasFeature) {
        *features = NULL;
        return NOERROR;
    }
    EnforceCrossUserPermission(userHandle);
    {    AutoLock syncLock(this);
        if (agent == NULL) {
            Logger::E(LOG__TAG, "agent is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        String componentName;
        agent->FlattenToString(&componentName);
        if (admin != NULL) {
            AutoPtr<ActiveAdmin> ap = GetActiveAdminUncheckedLocked(admin, userHandle);
            if (ap != NULL) {
                AutoPtr<IInterface> obj;
                ap->mTrustAgentFeatures->Get(StringUtils::ParseCharSequence(componentName), (IInterface**)&obj);
                *features = IList::Probe(obj);
                REFCOUNT_ADD(*features)
                return NOERROR;
            }
            else {
                *features = NULL;
                return NOERROR;
            }
        }
        // Return strictest policy for this user and profiles that are visible from this user.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userHandle, (IList**)&profiles);
        AutoPtr<IList> result;
        AutoPtr<IIterator> it;
        ICollection::Probe(profiles)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(obj);
            AutoPtr<IUserHandle> userHandle;
            userInfo->GetUserHandle((IUserHandle**)&userHandle);
            Int32 identifier;
            userHandle->GetIdentifier(&identifier);
            AutoPtr<DevicePolicyData> policy = GetUserData(identifier);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 i=0; i<N; i++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(i, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> ap = (ActiveAdmin*) IObject::Probe(obj);
                // Compute the intersection of all features for active admins that disable
                // trust agents:
                if ((ap->mDisabledKeyguardFeatures
                        & IDevicePolicyManager::KEYGUARD_DISABLE_TRUST_AGENTS) != 0) {
                    AutoPtr<IInterface> obj;
                    ap->mTrustAgentFeatures->Get(StringUtils::ParseCharSequence(componentName), (IInterface**)&obj);
                    AutoPtr<IList> features = IList::Probe(obj);
                    if (result == NULL) {
                        Int32 size;
                        features->GetSize(&size);
                        if (features == NULL || size == 0) {
                            CArrayList::New((IList**)&result);
                            String packageName;
                            ap->mInfo->GetPackageName(&packageName);
                            Slogger::W(LOG__TAG, "admin %s has null trust agent feature set; all will be disabled",
                                    packageName.string());
                        } else {
                            Int32 size;
                            features->GetSize(&size);
                            CArrayList::New(size, (IList**)&result);
                            result->AddAll(ICollection::Probe(features));
                        }
                    } else {
                        result->RetainAll(ICollection::Probe(features));
                    }
                }
            }
        }
        *features = result;
        REFCOUNT_ADD(*features)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetRestrictionsProvider(
    /* [in] */ IComponentName* who,
    /* [in] */ IComponentName* permissionProvider)
{
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int32 userHandle = UserHandle::GetCallingUserId();
        AutoPtr<DevicePolicyData> userData = GetUserData(userHandle);
        userData->mRestrictionsProvider = permissionProvider;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetRestrictionsProvider(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    {    AutoLock syncLock(this);
        if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
            Logger::E(LOG__TAG, "Only the system can query the permission provider");
            return E_SECURITY_EXCEPTION;
        }
        AutoPtr<DevicePolicyData> userData = GetUserData(userHandle);
        *result = userData != NULL ? userData->mRestrictionsProvider : NULL;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::AddCrossProfileIntentFilter(
    /* [in] */ IComponentName* who,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ Int32 flags)
{
    Int32 callingUserId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            if ((flags & IDevicePolicyManager::FLAG_PARENT_CAN_ACCESS_MANAGED) != 0) {
                String packageName;
                ec = who->GetPackageName(&packageName);
                if (FAILED(ec)) break;
                Int32 userId;
                ec = mContext->GetUserId(&userId);
                if (FAILED(ec)) break;
                ec = pm->AddCrossProfileIntentFilter(filter, packageName,
                        userId, callingUserId, IUserHandle::USER_OWNER, 0);
                if (FAILED(ec)) break;
            }
            if ((flags & IDevicePolicyManager::FLAG_MANAGED_CAN_ACCESS_PARENT) != 0) {
                String packageName;
                ec = who->GetPackageName(&packageName);
                if (FAILED(ec)) break;
                Int32 userId;
                ec = mContext->GetUserId(&userId);
                if (FAILED(ec)) break;
                ec = pm->AddCrossProfileIntentFilter(filter, packageName,
                        userId, IUserHandle::USER_OWNER, callingUserId, 0);
            }
        } while(FALSE);
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // Shouldn't happen
            ec = NOERROR;
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        // }
        return ec;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::ClearCrossProfileIntentFilters(
    /* [in] */ IComponentName* who)
{
    Int32 callingUserId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            String packageName;
            ec = who->GetPackageName(&packageName);
            if (FAILED(ec)) break;
            ec = pm->ClearCrossProfileIntentFilters(callingUserId, packageName,
                    callingUserId);
            if (FAILED(ec)) break;
            // If we want to support multiple managed profiles, we will have to only remove
            // those that have callingUserId as their target.
            ec = pm->ClearCrossProfileIntentFilters(IUserHandle::USER_OWNER, packageName,
                    callingUserId);
        } while(FALSE);
        // } catch (RemoteException re) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            // Shouldn't happen
            ec = NOERROR;
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::CheckPackagesInPermittedListOrSystem(
    /* [in] */ IList* enabledPackages,
    /* [in] */ IList* permittedList,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 userIdToCheck = UserHandle::GetCallingUserId();
    Int64 id = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        // If we have an enabled packages list for a managed profile the packages
        // we should check are installed for the parent user.
        AutoPtr<IUserInfo> user;
        mUserManager->GetUserInfo(userIdToCheck, (IUserInfo**)&user);
        Boolean isManagedProfile;
        user->IsManagedProfile(&isManagedProfile);
        if (isManagedProfile) {
            user->GetProfileGroupId(&userIdToCheck);
        }
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        AutoPtr<IIterator> it;
        ICollection::Probe(enabledPackages)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String enabledPackage;
            IObject::Probe(obj)->ToString(&enabledPackage);
            Boolean systemService = FALSE;
            // try {
            do {
                AutoPtr<IApplicationInfo> applicationInfo;
                ec = pm->GetApplicationInfo(enabledPackage,
                        IPackageManager::GET_UNINSTALLED_PACKAGES, userIdToCheck, (IApplicationInfo**)&applicationInfo);
                if (FAILED(ec)) break;
                Int32 flags;
                applicationInfo->GetFlags(&flags);
                systemService = (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
            } while(FALSE);
            // } catch (RemoteException e) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Logger::I(LOG__TAG, "Can't talk to package managed %d", ec);
                ec = NOERROR;
            }
            if (FAILED(ec)) break;
            // }
            Boolean isContains;
            permittedList->Contains(StringUtils::ParseCharSequence(enabledPackage), &isContains);
            if (!systemService && !isContains) {
                *result = FALSE;
                return NOERROR;
            }
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(id);
    if (FAILED(ec)) return ec;
    // }
    *result = TRUE;
    return NOERROR;
}

AutoPtr<IAccessibilityManager> CDevicePolicyManagerService::GetAccessibilityManagerForUser(
    /* [in] */ Int32 userId)
{
    // Not using AccessibilityManager.getInstance because that guesses
    // at the user you require based on callingUid and caches for a given
    // process.
    AutoPtr<IBinder> iBinder = IBinder::Probe(ServiceManager::GetService(IContext::ACCESSIBILITY_SERVICE));
    AutoPtr<IIAccessibilityManager> service;
    if (iBinder == NULL)
        service = NULL;
    else
        service = IIAccessibilityManager::Probe(iBinder);
    AutoPtr<IAccessibilityManager> rev;
    CAccessibilityManager::New(mContext, service, userId, (IAccessibilityManager**)&rev);
    return rev;
}

ECode CDevicePolicyManagerService::SetPermittedAccessibilityServices(
    /* [in] */ IComponentName* who,
    /* [in] */ IList* packageList,
    /* [out] */ Boolean* result)
{
    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    if (who == NULL) {
        Logger::E(LOG__TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (packageList != NULL) {
        Int32 userId = UserHandle::GetCallingUserId();
        AutoPtr<IList> enabledServices;
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            AutoPtr<IUserInfo> user;
            ec = mUserManager->GetUserInfo(userId, (IUserInfo**)&user);
            if (FAILED(ec)) break;
            Boolean isManagedProfile;
            ec = user->IsManagedProfile(&isManagedProfile);
            if (FAILED(ec)) break;
            if (isManagedProfile) {
                user->GetProfileGroupId(&userId);
            }
            AutoPtr<IAccessibilityManager> accessibilityManager = GetAccessibilityManagerForUser(userId);
            ec = accessibilityManager->GetEnabledAccessibilityServiceList(
                    IAccessibilityServiceInfo::FEEDBACK_ALL_MASK, (IList**)&enabledServices);
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
        if (enabledServices != NULL) {
            AutoPtr<IList> enabledPackages;
            CArrayList::New((IList**)&enabledPackages);
            AutoPtr<IIterator> it;
            ICollection::Probe(enabledServices)->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IAccessibilityServiceInfo> service = IAccessibilityServiceInfo::Probe(obj);
                AutoPtr<IResolveInfo> resolveInfo;
                service->GetResolveInfo((IResolveInfo**)&resolveInfo);
                AutoPtr<IServiceInfo> serviceInfo;
                resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
                String packageName;
                IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
                enabledPackages->Add(StringUtils::ParseCharSequence(packageName));
            }
            Boolean bCheckPackagesInPermittedListOrSystem;
            CheckPackagesInPermittedListOrSystem(enabledPackages, packageList, &bCheckPackagesInPermittedListOrSystem);
            if (!bCheckPackagesInPermittedListOrSystem) {
                Slogger::E(LOG__TAG, "Cannot set permitted accessibility services, "
                        "because it contains already enabled accesibility services.");
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&admin);
        admin->mPermittedAccessiblityServices = packageList;
        SaveSettingsLocked(UserHandle::GetCallingUserId());
    }
    *result = TRUE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPermittedAccessibilityServices(
    /* [in] */ IComponentName* who,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (!mHasFeature) {
        *result = NULL;
        return NOERROR;
    }
    if (who == NULL) {
        Logger::E(LOG__TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&admin);
        *result = admin->mPermittedAccessiblityServices;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPermittedAccessibilityServicesForUser(
    /* [in] */ Int32 userId,
    /* [out] */ IList** services)
{
    VALIDATE_NOT_NULL(services)
    *services = NULL;

    if (!mHasFeature) {
        return NOERROR;
    }
    AutoPtr<IList> result;
    {    AutoLock syncLock(this);
        // If we have multiple profiles we return the intersection of the
        // permitted lists. This can happen in cases where we have a device
        // and profile owner.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userId, (IList**)&profiles);
        Int32 PROFILES_SIZE;
        profiles->GetSize(&PROFILES_SIZE);
        for (Int32 i = 0; i < PROFILES_SIZE; ++i) {
            // Just loop though all admins, only device or profiles
            // owners can have permitted lists set.
            AutoPtr<IInterface> userInfo;
            profiles->Get(i, (IInterface**)&userInfo);
            Int32 id;
            IUserInfo::Probe(userInfo)->GetId(&id);
            AutoPtr<DevicePolicyData> policy = GetUserData(id);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 j = 0; j < N; j++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(j, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                AutoPtr<IList> fromAdmin = admin->mPermittedAccessiblityServices;
                if (fromAdmin != NULL) {
                    if (result == NULL) {
                        CArrayList::New(ICollection::Probe(fromAdmin), (IList**)&result);
                    } else {
                        result->RetainAll(ICollection::Probe(fromAdmin));
                    }
                }
            }
        }
        // If we have a permitted list add all system accessibility services.
        if (result != NULL) {
            Int64 id = Binder::ClearCallingIdentity();
            // try {
            ECode ec;
            do {
                AutoPtr<IUserInfo> user;
                ec = mUserManager->GetUserInfo(userId, (IUserInfo**)&user);
                if (FAILED(ec)) break;
                Boolean isManagedProfile;
                ec = user->IsManagedProfile(&isManagedProfile);
                if (FAILED(ec)) break;
                if (isManagedProfile) {
                    user->GetProfileGroupId(&userId);
                }
                AutoPtr<IAccessibilityManager> accessibilityManager =
                        GetAccessibilityManagerForUser(userId);
                AutoPtr<IList> installedServices;
                ec = accessibilityManager->GetInstalledAccessibilityServiceList((IList**)&installedServices);
                if (FAILED(ec)) break;
                AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
                if (installedServices != NULL) {
                    AutoPtr<IIterator> it;
                    ICollection::Probe(installedServices)->GetIterator((IIterator**)&it);
                    Boolean hasNext;
                    while (it->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> obj;
                        it->GetNext((IInterface**)&obj);
                        AutoPtr<IAccessibilityServiceInfo> service = IAccessibilityServiceInfo::Probe(obj);
                        AutoPtr<IResolveInfo> resolveInfo;
                        ec = service->GetResolveInfo((IResolveInfo**)&resolveInfo);
                        if (FAILED(ec)) break;
                        AutoPtr<IServiceInfo> serviceInfo;
                        ec = resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
                        if (FAILED(ec)) break;
                        String packageName;
                        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);
                        // try {
                        do {
                            AutoPtr<IApplicationInfo> applicationInfo;
                            ec = pm->GetApplicationInfo(packageName,
                                    IPackageManager::GET_UNINSTALLED_PACKAGES, userId, (IApplicationInfo**)&applicationInfo);
                            if (FAILED(ec)) break;
                            Int32 flags;
                            applicationInfo->GetFlags(&flags);
                            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                                result->Add(StringUtils::ParseCharSequence(packageName));
                            }
                        } while(FALSE);
                        // } catch (RemoteException e) {
                        if (FAILED(ec)) {
                            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                                Logger::I(LOG__TAG, "Accessibility service in missing package %d", ec);
                            }
                            else
                                break;
                        }
                        // }
                    }
                    if (FAILED(ec)) break;
                }
            } while(FALSE);
            // } finally {
            Binder::RestoreCallingIdentity(id);
            if (FAILED(ec)) return ec;
            // }
        }
        *services = result;
        REFCOUNT_ADD(*services)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::CheckCallerIsCurrentUserOrProfile(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 callingUserId = UserHandle::GetCallingUserId();
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        AutoPtr<IUserInfo> currentUser;
        AutoPtr<IUserInfo> callingUser;
        ec = mUserManager->GetUserInfo(callingUserId, (IUserInfo**)&callingUser);
        if (FAILED(ec)) break;
        // try {
        ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&currentUser);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Slogger::E(LOG__TAG, "Failed to talk to activity managed. %d", ec);
                *result = FALSE;
                return NOERROR;
            }
            else
                break;
        }
        // }
        Boolean isManagedProfile;
        callingUser->IsManagedProfile(&isManagedProfile);
        Int32 id;
        currentUser->GetId(&id);
        Int32 profileGroupId;
        callingUser->GetProfileGroupId(&profileGroupId);
        if (isManagedProfile && profileGroupId != id) {
            Slogger::E(LOG__TAG, "Cannot set permitted input methods for managed profile "
                    "of a user that isn't the foreground user.");
            *result = FALSE;
            return NOERROR;
        }
        if (!isManagedProfile && callingUserId != id ) {
            Slogger::E(LOG__TAG, "Cannot set permitted input methods "
                    "of a user that isn't the foreground user.");
            *result = FALSE;
            return NOERROR;
        }
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(token);
    if (FAILED(ec)) return ec;
    // }
    *result = TRUE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPermittedInputMethods(
    /* [in] */ IComponentName* who,
    /* [in] */ IList* packageList,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    if (who == NULL) {
        Logger::E(LOG__TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // TODO When InputMethodManager supports per user calls remove
    //      this restriction.
    Boolean bCheckCallerIsCurrentUserOrProfile;
    CheckCallerIsCurrentUserOrProfile(&bCheckCallerIsCurrentUserOrProfile);
    if (!bCheckCallerIsCurrentUserOrProfile) {
        *result = FALSE;
        return NOERROR;
    }
    if (packageList != NULL) {
        // InputMethodManager fetches input methods for current user.
        // So this can only be set when calling user is the current user
        // or parent is current user in case of managed profiles.
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
        AutoPtr<IInputMethodManager> inputMethodManager = IInputMethodManager::Probe(obj);
        AutoPtr<IList> enabledImes;
        inputMethodManager->GetEnabledInputMethodList((IList**)&enabledImes);
        if (enabledImes != NULL) {
            AutoPtr<IList> enabledPackages;
            CArrayList::New((IList**)&enabledPackages);
            AutoPtr<IIterator> it;
            ICollection::Probe(enabledImes)->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IInputMethodInfo> ime = IInputMethodInfo::Probe(obj);
                String packageName;
                ime->GetPackageName(&packageName);
                enabledPackages->Add(StringUtils::ParseCharSequence(packageName));
            }
            Boolean bCheckPackagesInPermittedListOrSystem;
            CheckPackagesInPermittedListOrSystem(enabledPackages,
                packageList, &bCheckPackagesInPermittedListOrSystem);
            if (!bCheckPackagesInPermittedListOrSystem) {
                Slogger::E(LOG__TAG, "Cannot set permitted input methods, "
                        "because it contains already enabled input method.");
                *result = FALSE;
                return NOERROR;
            }
        }
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&admin);
        admin->mPermittedInputMethods = packageList;
        SaveSettingsLocked(UserHandle::GetCallingUserId());
    }
    *result = TRUE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPermittedInputMethods(
    /* [in] */ IComponentName* who,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (!mHasFeature) {
        return NOERROR;
    }
    if (who == NULL) {
        Logger::E(LOG__TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&admin);
        *result = admin->mPermittedInputMethods;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPermittedInputMethodsForCurrentUser(
    /* [out] */ IList** methods)
{
    VALIDATE_NOT_NULL(methods)
    *methods = NULL;

    AutoPtr<IUserInfo> currentUser;
    // try {
    ECode ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&currentUser);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(LOG__TAG, "Failed to make remote calls to get current user %d", ec);
            // Activity managed is dead, just allow all IMEs
            *methods = NULL;
            return NOERROR;
        }
        return ec;
    }
    // }
    Int32 userId;
    currentUser->GetId(&userId);
    {    AutoLock syncLock(this);
        AutoPtr<IList> result;
        // If we have multiple profiles we return the intersection of the
        // permitted lists. This can happen in cases where we have a device
        // and profile owner.
        AutoPtr<IList> profiles;
        mUserManager->GetProfiles(userId, (IList**)&profiles);
        Int32 PROFILES_SIZE;
        profiles->GetSize(&PROFILES_SIZE);
        for (Int32 i = 0; i < PROFILES_SIZE; ++i) {
            // Just loop though all admins, only device or profiles
            // owners can have permitted lists set.
            AutoPtr<IInterface> userInfo;
            profiles->Get(i, (IInterface**)&userInfo);
            Int32 id;
            IUserInfo::Probe(userInfo)->GetId(&id);
            AutoPtr<DevicePolicyData> policy = GetUserData(id);
            Int32 N;
            policy->mAdminList->GetSize(&N);
            for (Int32 j = 0; j < N; j++) {
                AutoPtr<IInterface> obj;
                policy->mAdminList->Get(j, (IInterface**)&obj);
                AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
                AutoPtr<IList> fromAdmin = admin->mPermittedInputMethods;
                if (fromAdmin != NULL) {
                    if (result == NULL) {
                        CArrayList::New(ICollection::Probe(fromAdmin), (IList**)&result);
                    } else {
                        result->RetainAll(ICollection::Probe(fromAdmin));
                    }
                }
            }
        }
        // If we have a permitted list add all system input methods.
        if (result != NULL) {
            AutoPtr<IInterface> obj;
            mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
            AutoPtr<IInputMethodManager> inputMethodManager = IInputMethodManager::Probe(obj);
            AutoPtr<IList> imes;
            inputMethodManager->GetInputMethodList((IList**)&imes);
            Int64 id = Binder::ClearCallingIdentity();
            // try {
            ECode ec;
            do {
                AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
                if (imes != NULL) {
                    AutoPtr<IIterator> it;
                    ICollection::Probe(imes)->GetIterator((IIterator**)&it);
                    Boolean hasNext;
                    while (it->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> obj;
                        it->GetNext((IInterface**)&obj);
                        AutoPtr<IInputMethodInfo> ime = IInputMethodInfo::Probe(obj);
                        String packageName;
                        ime->GetPackageName(&packageName);
                        // try {
                        do {
                            AutoPtr<IApplicationInfo> applicationInfo;
                            ec = pm->GetApplicationInfo(
                                    packageName, IPackageManager::GET_UNINSTALLED_PACKAGES,
                                    userId, (IApplicationInfo**)&applicationInfo);
                            if (FAILED(ec)) break;
                            Int32 flags;
                            applicationInfo->GetFlags(&flags);
                            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                                result->Add(StringUtils::ParseCharSequence(packageName));
                            }
                        } while(FALSE);
                        // } catch (RemoteException e) {
                        if (FAILED(ec)) {
                            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                                Logger::I(LOG__TAG, "Input method for missing package %d", ec);
                                ec = NOERROR;
                            }
                            else
                                break;
                        }
                        // }
                    }
                    if (FAILED(ec)) break;
                }
            } while(FALSE);
            // } finally {
            Binder::RestoreCallingIdentity(id);
            if (FAILED(ec)) return ec;
            // }
        }
        *methods = result;
        REFCOUNT_ADD(*methods)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::CreateUser(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& name,
    /* [out] */ IUserHandle** user)
{
    VALIDATE_NOT_NULL(user)
    *user = NULL;

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            AutoPtr<IUserInfo> userInfo;
            ec = mUserManager->CreateUser(name, 0 /* flags */, (IUserInfo**)&userInfo);
            if (FAILED(ec)) break;
            if (userInfo != NULL) {
                AutoPtr<IUserHandle> userHandle;
                ec = userInfo->GetUserHandle((IUserHandle**)&userHandle);
                if (FAILED(ec)) break;
                *user = userHandle;
                REFCOUNT_ADD(*user)
                break;
            }
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::CreateAndInitializeUser(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& name,
    /* [in] */ const String& ownerName,
    /* [in] */ IComponentName* profileOwnerComponent,
    /* [in] */ IBundle* adminExtras,
    /* [out] */ IUserHandle** uh)
{
    VALIDATE_NOT_NULL(uh)
    *uh = NULL;

    AutoPtr<IUserHandle> user;
    CreateUser(who, name, (IUserHandle**)&user);
    Int64 id = Binder::ClearCallingIdentity();
    // try {
    ECode ec;
    do {
        String profileOwnerPkg;
        ec = profileOwnerComponent->GetPackageName(&profileOwnerPkg);
        AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
        AutoPtr<IIActivityManager> activityManager = ActivityManagerNative::GetDefault();
        Int32 identifier;
        ec = user->GetIdentifier(&identifier);
        if (FAILED(ec)) break;
        // try {
        do {
            // Install the profile owner if not present.
            Boolean isPackageAvailable;
            ipm->IsPackageAvailable(profileOwnerPkg, identifier, &isPackageAvailable);
            if (!isPackageAvailable) {
                Int32 iNoUse;
                ec = ipm->InstallExistingPackageAsUser(profileOwnerPkg, identifier, &iNoUse);
                if (FAILED(ec)) break;
            }
            // Start user in background.
            Boolean bNoUse;
            ec = activityManager->StartUserInBackground(identifier, &bNoUse);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Slogger::E(LOG__TAG, "Failed to make remote calls for configureUser %d", ec);
                ec = NOERROR;
            }
        }
        // }
        ec = SetActiveAdmin(profileOwnerComponent, TRUE, identifier, adminExtras);
        if (FAILED(ec)) break;
        Boolean bNoUse;
        ec = SetProfileOwner(profileOwnerComponent, ownerName, identifier, &bNoUse);
        *uh = user;
        REFCOUNT_ADD(*uh)
    } while(FALSE);
    // } finally {
    Binder::RestoreCallingIdentity(id);
    return ec;
    // }
}

ECode CDevicePolicyManagerService::RemoveUser(
    /* [in] */ IComponentName* who,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            Int32 identifier;
            ec = userHandle->GetIdentifier(&identifier);
            if (FAILED(ec)) break;
            ec = mUserManager->RemoveUser(identifier, result);
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SwitchUser(
    /* [in] */ IComponentName* who,
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ Boolean* result)
{
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            Int32 userId = IUserHandle::USER_OWNER;
            if (userHandle != NULL) {
                Int32 identifier;
                ec = userHandle->GetIdentifier(&identifier);
                if (FAILED(ec)) break;
                userId = identifier;
            }
            ec = ActivityManagerNative::GetDefault()->SwitchUser(userId, result);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            Logger::E(LOG__TAG, "Couldn't switch user %d", ec);
            *result = FALSE;
            ec = NOERROR;
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetApplicationRestrictions(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(UserHandle::GetCallingUserId(), (IUserHandle**)&userHandle);
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        // TODO: Waiting for UserManager
        assert(0);
        // ec = mUserManager->GetApplicationRestrictions(packageName, userHandle, result);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetUserRestriction(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& key,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IUserHandle> user;
    CUserHandle::New(UserHandle::GetCallingUserId(), (IUserHandle**)&user);
    Int32 userHandle;
    user->GetIdentifier(&userHandle);;
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> activeAdmin;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&activeAdmin);
        String packageName;
        activeAdmin->mInfo->GetPackageName(&packageName);
        Boolean isDeviceOwner;
        IsDeviceOwner(packageName, &isDeviceOwner);
        Boolean isContains;
        DEVICE_OWNER_USER_RESTRICTIONS->Contains(StringUtils::ParseCharSequence(key), &isContains);
        if (!isDeviceOwner && userHandle != IUserHandle::USER_OWNER
                && isContains) {
            Logger::E(LOG__TAG, "Profile owners cannot set user restriction %s", key.string());
            return E_SECURITY_EXCEPTION;
        }
        Boolean alreadyRestricted;
        mUserManager->HasUserRestriction(key, user, &alreadyRestricted);
        AutoPtr<IIAudioService> iAudioService;
        if (IUserManager::DISALLOW_UNMUTE_MICROPHONE.Equals(key)
                || IUserManager::DISALLOW_ADJUST_VOLUME.Equals(key)) {
            iAudioService = IIAudioService::Probe(ServiceManager::GetService(IContext::AUDIO_SERVICE));
        }
        if (enabled && !alreadyRestricted) {
            // try {
            ECode ec;
            do {
                if (IUserManager::DISALLOW_UNMUTE_MICROPHONE.Equals(key)) {
                    String packageName;
                    who->GetPackageName(&packageName);
                    ec = iAudioService->SetMicrophoneMute(TRUE, packageName);
                } else if (IUserManager::DISALLOW_ADJUST_VOLUME.Equals(key)) {
                    String packageName;
                    who->GetPackageName(&packageName);
                    ec = iAudioService->SetMasterMute(TRUE, 0, packageName, NULL);
                }
            } while(FALSE);
            // } catch (RemoteException re) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(LOG__TAG, "Failed to talk to AudioService. %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            if (enabled && !alreadyRestricted) {
                if (IUserManager::DISALLOW_CONFIG_WIFI.Equals(key)) {
                    AutoPtr<IContentResolver> contentResolver;
                    ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
                    if (FAILED(ec)) break;
                    Boolean bNoUse;
                    ec = Settings::Secure::PutInt32ForUser(contentResolver,
                            ISettingsSecure::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, 0,
                            userHandle, &bNoUse);
                    if (FAILED(ec)) break;
                } else if (IUserManager::DISALLOW_USB_FILE_TRANSFER.Equals(key)) {
                    AutoPtr<IInterface> obj;
                    ec = mContext->GetSystemService(IContext::USB_SERVICE, (IInterface**)&obj);
                    if (FAILED(ec)) break;
                    AutoPtr<IUsbManager> manager = IUsbManager::Probe(obj);
                    ec = manager->SetCurrentFunction(String("none"), FALSE);
                    if (FAILED(ec)) break;
                } else if (IUserManager::DISALLOW_SHARE_LOCATION.Equals(key)) {
                    AutoPtr<IContentResolver> contentResolver;
                    ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
                    if (FAILED(ec)) break;
                    Boolean bNoUse;
                    ec = Settings::Secure::PutInt32ForUser(contentResolver,
                            ISettingsSecure::LOCATION_MODE, ISettingsSecure::LOCATION_MODE_OFF,
                            userHandle, &bNoUse);
                    if (FAILED(ec)) break;
                    ec = Settings::Secure::PutStringForUser(contentResolver,
                            ISettingsSecure::LOCATION_PROVIDERS_ALLOWED, String(""),
                            userHandle, &bNoUse);
                    if (FAILED(ec)) break;
                } else if (IUserManager::DISALLOW_DEBUGGING_FEATURES.Equals(key)) {
                    // Only disable adb if changing for primary user, since it is global
                    if (userHandle == IUserHandle::USER_OWNER) {
                        AutoPtr<IContentResolver> contentResolver;
                        ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
                        if (FAILED(ec)) break;
                        Boolean bNoUse;
                        Settings::Global::PutStringForUser(contentResolver,
                                ISettingsGlobal::ADB_ENABLED, String("0"), userHandle, &bNoUse);
                        if (FAILED(ec)) break;
                    }
                } else if (IUserManager::ENSURE_VERIFY_APPS.Equals(key)) {
                    AutoPtr<IContentResolver> contentResolver;
                    ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
                    if (FAILED(ec)) break;
                    Boolean bNoUse;
                    ec = Settings::Global::PutStringForUser(contentResolver,
                            ISettingsGlobal::PACKAGE_VERIFIER_ENABLE, String("1"),
                            userHandle, &bNoUse);
                    if (FAILED(ec)) break;
                    ec = Settings::Global::PutStringForUser(contentResolver,
                            ISettingsGlobal::PACKAGE_VERIFIER_INCLUDE_ADB, String("1"),
                            userHandle, &bNoUse);
                    if (FAILED(ec)) break;
                } else if (IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES.Equals(key)) {
                    AutoPtr<IContentResolver> contentResolver;
                    ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
                    if (FAILED(ec)) break;
                    Boolean bNoUse;
                    ec = Settings::Secure::PutInt32ForUser(contentResolver,
                            ISettingsSecure::INSTALL_NON_MARKET_APPS, 0,
                            userHandle, &bNoUse);
                    if (FAILED(ec)) break;
                }
            }
            ec = mUserManager->SetUserRestriction(key, enabled, user);
        } while(FALSE);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
        if (!enabled && alreadyRestricted) {
            // try {
            do {
                if (IUserManager::DISALLOW_UNMUTE_MICROPHONE.Equals(key)) {
                    String packageName;
                    ec = who->GetPackageName(&packageName);
                    if (FAILED(ec)) break;
                    ec = iAudioService->SetMicrophoneMute(FALSE, packageName);
                    if (FAILED(ec)) break;
                } else if (IUserManager::DISALLOW_ADJUST_VOLUME.Equals(key)) {
                    String packageName;
                    ec = who->GetPackageName(&packageName);
                    if (FAILED(ec)) break;
                    ec = iAudioService->SetMasterMute(FALSE, 0, packageName, NULL);
                }
            } while(FALSE);
            // } catch (RemoteException re) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(LOG__TAG, "Failed to talk to AudioService. %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetApplicationHidden(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean hidden,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 callingUserId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Boolean bNoUse;
        ECode ec = pm->SetApplicationHiddenSettingAsUser(packageName, hidden, callingUserId, &bNoUse);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // shouldn't happen
                Slogger::E(LOG__TAG, "Failed to setApplicationHiddenSetting %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
        *result = FALSE;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsApplicationHidden(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 callingUserId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Boolean bNoUse;
        ECode ec = pm->GetApplicationHiddenSettingAsUser(packageName, callingUserId, &bNoUse);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // shouldn't happen
                Slogger::E(LOG__TAG, "Failed to getApplicationHiddenSettingAsUser %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
        *result = FALSE;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::EnableSystemApp(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName)
{
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int32 userId = UserHandle::GetCallingUserId();
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            if (DBG) {
                Slogger::V(LOG__TAG, "installing %s for %d",
                        packageName.string(), userId);
            }
            AutoPtr<IUserManager> um;
            AutoPtr<IUserManagerHelper> helper;
            CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
            ec = helper->Get(mContext, (IUserManager**)&um);
            if (FAILED(ec)) break;
            AutoPtr<IUserInfo> primaryUser;
            ec = um->GetProfileParent(userId, (IUserInfo**)&primaryUser);
            if (FAILED(ec)) break;
            // Call did not come from a managed profile
            if (primaryUser == NULL) {
                ec = um->GetUserInfo(userId, (IUserInfo**)&primaryUser);
                if (FAILED(ec)) break;
            }
            AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
            Boolean isSystemApp;
            Int32 id;
            primaryUser->GetId(&id);
            ec = IsSystemApp(pm, packageName, id, &isSystemApp);
            if (FAILED(ec)) break;
            if (!isSystemApp) {
                Logger::E(LOG__TAG, "Only system apps can be enabled this way.");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            // Install the app.
            Int32 iNoUse;
            ec = pm->InstallExistingPackageAsUser(packageName, userId, &iNoUse);
        } while(FALSE);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // shouldn't happen
                Slogger::W(LOG__TAG, "Failed to install %s %d", packageName.string(), ec);
                ec = NOERROR;
            }
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::EnableSystemAppWithIntent(
    /* [in] */ IComponentName* who,
    /* [in] */ IIntent* intent,
    /* [out] */ Int32* result)
{
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int32 userId = UserHandle::GetCallingUserId();
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        ECode ec;
        do {
            AutoPtr<IUserManager> um;
            AutoPtr<IUserManagerHelper> helper;
            CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
            ec = helper->Get(mContext, (IUserManager**)&um);
            if(FAILED(ec)) break;
            AutoPtr<IUserInfo> primaryUser;
            ec = um->GetProfileParent(userId, (IUserInfo**)&primaryUser);
            if(FAILED(ec)) break;
            // Call did not come from a managed profile.
            if (primaryUser == NULL) {
                ec = um->GetUserInfo(userId, (IUserInfo**)&primaryUser);
                if(FAILED(ec)) break;
            }
            AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
            AutoPtr<IContentResolver> contentResolver;
            ec = mContext->GetContentResolver((IContentResolver**)&contentResolver);
            if(FAILED(ec)) break;
            AutoPtr<IList> activitiesToEnable;
            Int32 id;
            primaryUser->GetId(&id);
            String type;
            intent->ResolveTypeIfNeeded(contentResolver, &type);
            ec = pm->QueryIntentActivities(intent,
                    type,
                    0, // no flags
                    id,
                    (IList**)&activitiesToEnable);
            if(FAILED(ec)) break;
            if (DBG) Slogger::D(LOG__TAG, "Enabling system activities: %s", TO_CSTR(activitiesToEnable));
            Int32 numberOfAppsInstalled = 0;
            if (activitiesToEnable != NULL) {
                AutoPtr<IIterator> it;
                ICollection::Probe(activitiesToEnable)->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);
                    AutoPtr<IActivityInfo> activityInfo;
                    info->GetActivityInfo((IActivityInfo**)&activityInfo);
                    if (activityInfo != NULL) {
                        Boolean isSystemApp;
                        AutoPtr<IActivityInfo> activityInfo;
                        info->GetActivityInfo((IActivityInfo**)&activityInfo);
                        String packageName;
                        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
                        IsSystemApp(pm, packageName, id, &isSystemApp);
                        if(FAILED(ec)) break;
                        if (!isSystemApp) {
                            Logger::E(LOG__TAG, "Only system apps can be enabled this way.");
                            return E_ILLEGAL_ARGUMENT_EXCEPTION;
                        }
                        numberOfAppsInstalled++;
                        Int32 iNoUse;
                        ec = pm->InstallExistingPackageAsUser(packageName, userId, &iNoUse);
                        if(FAILED(ec)) break;
                    }
                }
                if(FAILED(ec)) break;
            }
            *result = numberOfAppsInstalled;
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // shouldn't happen
                Slogger::W(LOG__TAG, "Failed to resolve intent for: %s", TO_CSTR(intent));
                *result = 0;
                ec = NOERROR;
            }
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsSystemApp(
    /* [in] */ IIPackageManager* pm,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IApplicationInfo> appInfo;
    pm->GetApplicationInfo(packageName, IPackageManager::GET_UNINSTALLED_PACKAGES,
            userId, (IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    *result = (flags & IApplicationInfo::FLAG_SYSTEM) > 0;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetAccountManagementDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean disabled)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> ap;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&ap);
        if (disabled) {
            ap->mAccountTypesWithManagementDisabled->Add(StringUtils::ParseCharSequence(accountType));
        } else {
            ap->mAccountTypesWithManagementDisabled->Remove(StringUtils::ParseCharSequence(accountType));
        }
        SaveSettingsLocked(UserHandle::GetCallingUserId());
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetAccountTypesWithManagementDisabled(
    /* [out, callee] */ ArrayOf<String>** result)
{
    return GetAccountTypesWithManagementDisabledAsUser(UserHandle::GetCallingUserId(), result);
}

ECode CDevicePolicyManagerService::GetAccountTypesWithManagementDisabledAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    EnforceCrossUserPermission(userId);
    if (!mHasFeature) {
        *result = NULL;
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        AutoPtr<DevicePolicyData> policy = GetUserData(userId);
        Int32 N;
        policy->mAdminList->GetSize(&N);
        AutoPtr<IHashSet> resultSet;
        CHashSet::New((IHashSet**)&resultSet);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            policy->mAdminList->Get(i, (IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            resultSet->AddAll(ICollection::Probe(admin->mAccountTypesWithManagementDisabled));
        }
        Int32 size;
        resultSet->GetSize(&size);
        AutoPtr<ArrayOf<IInterface*> > rev;
        resultSet->ToArray(ArrayOf<IInterface*>::Alloc(size), (ArrayOf<IInterface*>**)&rev);
        size = rev->GetLength();
        *result = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            String s;
            IObject::Probe((*rev)[i])->ToString(&s);
            (*result)->Set(i, s);
        }
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetUninstallBlocked(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean uninstallBlocked)
{
    const Int32 userId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Boolean bNoUse;
        ECode ec = pm->SetBlockUninstallForUser(packageName, uninstallBlocked, userId, &bNoUse);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // Shouldn't happen.
                Slogger::E(LOG__TAG, "Failed to setBlockUninstallForUser %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsUninstallBlocked(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    const Int32 userId = UserHandle::GetCallingUserId();
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();
        Boolean bNoUse;
        ECode ec = pm->GetBlockUninstallForUser(packageName, userId, &bNoUse);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                // Shouldn't happen.
                Slogger::E(LOG__TAG, "Failed to getBlockUninstallForUser %d", ec);
                ec = NOERROR;
            }
        }
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetCrossProfileCallerIdDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean disabled)
{
    if (!mHasFeature) {
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&admin);
        if (admin->mDisableCallerId != disabled) {
            admin->mDisableCallerId = disabled;
            SaveSettingsLocked(UserHandle::GetCallingUserId());
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCrossProfileCallerIdDisabled(
    /* [in] */ IComponentName* who,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (!mHasFeature) {
        *result = FALSE;
        return NOERROR;
    }
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> admin;
        GetActiveAdminForCallerLocked(who,
                IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER,
                (ActiveAdmin**)&admin);
        *result = admin->mDisableCallerId;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCrossProfileCallerIdDisabledForUser(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // TODO: Should there be a check to make sure this relationship is within a profile group?
    //enforceSystemProcess("getCrossProfileCallerIdDisabled can only be called by system");
    {    AutoLock syncLock(this);
        AutoPtr<ActiveAdmin> admin = GetProfileOwnerAdmin(userId);
        *result = (admin != NULL) ? admin->mDisableCallerId : FALSE;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetLockTaskPackages(
    /* [in] */ IComponentName* who,
    /* [in] */ ArrayOf<String>* packages)
{
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
        Int32 userHandle;
        Binder::GetCallingUserHandle()->GetIdentifier(&userHandle);;
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        policy->mLockTaskPackages->Clear();
        if (packages != NULL) {
            for (Int32 j = 0; j < packages->GetLength(); j++) {
                String pkg = (*packages)[j];
                policy->mLockTaskPackages->Add(StringUtils::ParseCharSequence(pkg));
            }
        }
        // Store the settings persistently.
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetLockTaskPackages(
    /* [in] */ IComponentName* who,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
        Int32 userHandle;
        Binder::GetCallingUserHandle()->GetIdentifier(&userHandle);;
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        AutoPtr<ArrayOf<IInterface*> > rev;
        policy->mLockTaskPackages->ToArray(ArrayOf<IInterface*>::Alloc(0), (ArrayOf<IInterface*>**)&rev);
        Int32 size = rev->GetLength();
        *result = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            String s;
            IObject::Probe((*rev)[i])->ToString(&s);
            (*result)->Set(i, s);
        }
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsLockTaskPermitted(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Get current user's devicepolicy
    Int32 uid = Binder::GetCallingUid();
    Int32 userHandle = UserHandle::GetUserId(uid);
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    {    AutoLock syncLock(this);
        Int32 size;
        policy->mLockTaskPackages->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            policy->mLockTaskPackages->Get(i, (IInterface**)&obj);
            String lockTaskPackage;
            IObject::Probe(obj)->ToString(&lockTaskPackage);
            // If the given package equals one of the packages stored our list,
            // we allow this package to start lock task mode.
            if (lockTaskPackage.Equals(pkg)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::NotifyLockTaskModeChanged(
    /* [in] */ Boolean isEnabled,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 userHandle)
{
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Logger::E(LOG__TAG, "notifyLockTaskModeChanged can only be called by system");
        return E_SECURITY_EXCEPTION;
    }
    {    AutoLock syncLock(this);
        AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
        AutoPtr<IBundle> adminExtras;
        CBundle::New((IBundle**)&adminExtras);
        adminExtras->PutString(IDeviceAdminReceiver::EXTRA_LOCK_TASK_PACKAGE, pkg);
        FOR_EACH(it, policy->mAdminList) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ActiveAdmin> admin = (ActiveAdmin*) IObject::Probe(obj);
            String packageName;
            admin->mInfo->GetPackageName(&packageName);
            Boolean ownsDevice;
            IsDeviceOwner(packageName, &ownsDevice);
            AutoPtr<IComponentName> componentName;
            GetProfileOwner(userHandle, (IComponentName**)&componentName);
            Boolean isEquals;
            IObject::Probe(componentName)->Equals(StringUtils::ParseCharSequence(packageName), &isEquals);
            Boolean ownsProfile = (componentName != NULL && isEquals);
            if (ownsDevice || ownsProfile) {
                if (isEnabled) {
                    SendAdminCommandLocked(admin, IDeviceAdminReceiver::ACTION_LOCK_TASK_ENTERING,
                            adminExtras, NULL);
                } else {
                    SendAdminCommandLocked(admin, IDeviceAdminReceiver::ACTION_LOCK_TASK_EXITING);
                }
            }
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetGlobalSetting(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& setting,
    /* [in] */ const String& value)
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DEVICE_OWNER, (ActiveAdmin**)&objNoUse);
        Boolean isContains;
        ICollection::Probe(GLOBAL_SETTINGS_WHITELIST)->Contains(StringUtils::ParseCharSequence(setting), &isContains);
        if (!isContains) {
            Logger::E(LOG__TAG, "Permission denial: device owners cannot update %s", setting.string());
            return E_SECURITY_EXCEPTION;
        }
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        Boolean bNoUse;
        ECode ec =Settings::Global::PutString(contentResolver, setting, value, &bNoUse);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetSecureSetting(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& setting,
    /* [in] */ const String& value)
{
    Int32 callingUserId = UserHandle::GetCallingUserId();
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> activeAdmin;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&activeAdmin);
        String packageName;
        activeAdmin->mInfo->GetPackageName(&packageName);
        Boolean isDeviceOwner;
        IsDeviceOwner(packageName, &isDeviceOwner);
        if (isDeviceOwner) {
            Boolean isContains;
            ICollection::Probe(SECURE_SETTINGS_DEVICEOWNER_WHITELIST)->Contains(
                StringUtils::ParseCharSequence(setting), &isContains);
            if (!isContains) {
                Logger::E(LOG__TAG, "Permission denial: Device owners cannot update %s", setting.string());
                return E_SECURITY_EXCEPTION;
            }
        } else {
            Boolean isContains;
            ICollection::Probe(SECURE_SETTINGS_WHITELIST)->Contains(
                StringUtils::ParseCharSequence(setting), &isContains);
            if (!isContains) {
                Logger::E(LOG__TAG, "Permission denial: Profile owners cannot update %s", setting.string());
                return E_SECURITY_EXCEPTION;
            }
        }
        Int64 id = Binder::ClearCallingIdentity();
        // try {
        Boolean bNoUse;
        ECode ec = Settings::Secure::PutStringForUser(contentResolver, setting, value, callingUserId, &bNoUse);
        // } finally {
        Binder::RestoreCallingIdentity(id);
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetMasterVolumeMuted(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean on)
{
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);;
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        AutoPtr<IIAudioService> iAudioService =
            IIAudioService::Probe(ServiceManager::GetService(IContext::AUDIO_SERVICE));
        // try{
        ECode ec;
        do {
            String packageName;
            ec = who->GetPackageName(&packageName);
            if (FAILED(ec)) break;
            ec = iAudioService->SetMasterMute(on, 0, packageName, NULL);
        } while(FALSE);
        // } catch (RemoteException re) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Slogger::E(LOG__TAG, "Failed to setMasterMute %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsMasterVolumeMuted(
    /* [in] */ IComponentName* who,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);;
    {    AutoLock syncLock(this);
        if (who == NULL) {
            Logger::E(LOG__TAG, "ComponentName is null");
            return E_NULL_POINTER_EXCEPTION;
        }
        AutoPtr<ActiveAdmin> objNoUse;
        GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_PROFILE_OWNER, (ActiveAdmin**)&objNoUse);
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
        AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
        Boolean isMasterMute;
        audioManager->IsMasterMute(&isMasterMute);
        *result = isMasterMute;
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::UpdateUserSetupComplete()
{
    AutoPtr<IList> users;
    mUserManager->GetUsers(TRUE, (IList**)&users);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);;
    Int32 N;
    users->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> userInfo;
        users->Get(i, (IInterface**)&userInfo);
        Int32 userHandle;
        IUserInfo::Probe(userInfo)->GetId(&userHandle);
        Int32 i32ForUser;
        Settings::Secure::GetInt32ForUser(resolver, ISettingsSecure::USER_SETUP_COMPLETE, 0,
                userHandle, &i32ForUser);
        if (i32ForUser != 0) {
            AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
            if (!policy->mUserSetupComplete) {
                policy->mUserSetupComplete = TRUE;
                {
                    AutoLock lock(this);
                    SaveSettingsLocked(userHandle);
                }
            }
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::ToString(
    /* [in] */ String* result)
{
    return ((IObject*)this)->ToString(result);
}

} // namespace DevicePolicy
} // namespace Server
} // namespace Droid
} // namespace Elastos
