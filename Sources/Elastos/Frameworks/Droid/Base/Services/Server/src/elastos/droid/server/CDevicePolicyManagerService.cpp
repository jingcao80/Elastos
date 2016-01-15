
#include "CDevicePolicyManagerService.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "util/Xml.h"
#include "util/XmlUtils.h"
#include <os/SystemClock.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuffer.h>

using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::Core::CObjectContainer;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Storage::IExternalStorageFormatter;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::CDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDeviceAdminReceiver;
using Elastos::Droid::Widget::Internal::ILockPatternUtils;
using Elastos::Droid::Widget::Internal::CLockPatternUtils;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::XmlUtils;
using Elastos::Droid::Utility::IFastXmlSerializer;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CDevicePolicyManagerService::ACTION_EXPIRED_PASSWORD_NOTIFICATION = String("com.android.server.ACTION_EXPIRED_PASSWORD_NOTIFICATION");

const String CDevicePolicyManagerService::SYSTEM_PROP_DISABLE_CAMERA = String("sys.secpolicy.camera.disabled");

const String CDevicePolicyManagerService::DEVICE_POLICIES_XML = String("device_policies.xml");

const String CDevicePolicyManagerService::TAG("DevicePolicyManagerService");

const Int32 CDevicePolicyManagerService::REQUEST_EXPIRE_PASSWORD = 5571;

const Int64 CDevicePolicyManagerService::MS_PER_DAY = 86400 * 1000;

const Int64 CDevicePolicyManagerService::EXPIRATION_GRACE_PERIOD_MS = 5 * MS_PER_DAY; // 5 days, in ms

const Boolean CDevicePolicyManagerService::DBG = FALSE;

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
    FAIL_RETURN(mResult->SendResult(data));
    return NOERROR;
}

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
    AutoLock lock(mHost->mLock);

    AutoPtr<IUserHandle> uHandle = mAdmin->GetUserHandle();
    Int32 userHandle;
    uHandle->GetIdentifier(&userHandle);

    AutoPtr<DevicePolicyData> policy =mHost->GetUserData(userHandle);
    Boolean doProxyCleanup;
    mAdmin->mInfo->UsesPolicy(IDeviceAdminInfo::USES_POLICY_SETS_GLOBAL_PROXY, &doProxyCleanup);

    policy->mAdminList.Remove(mAdmin);
    policy->mAdminMap.Erase(mAdminReceiver);
    mHost->ValidatePasswordOwnerLocked(policy);
    mHost->SyncDeviceCapabilitiesLocked(policy);
    if (doProxyCleanup) {
        mHost->ResetGlobalProxyLocked(mHost->GetUserData(userHandle));
    }
    mHost->SaveSettingsLocked(userHandle);
    mHost->UpdateMaximumTimeToLockLocked(policy);

    return NOERROR;
}

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
        if (DBG) Slogger::V(TAG, buf.ToString());

        AutoPtr<IRunnable> runnable = new CDevicePolicyManagerService::DevicePolicyReceiverRunnable(mHost, userHandle);
        Boolean result;
        mHost->mHandler->Post(runnable, &result);
    } else if (action.Equals(IIntent::ACTION_USER_REMOVED)) {
        mHost->RemoveUserData(userHandle);
    } else if (action.Equals(IIntent::ACTION_USER_STARTED)
            || action.Equals(IIntent::ACTION_PACKAGE_CHANGED)
            || action.Equals(IIntent::ACTION_PACKAGE_REMOVED)
            || action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
        if (action.Equals(IIntent::ACTION_USER_STARTED)) {
            // Reset the policy data
            AutoLock lock(mHost->mLock);

            HashMap<Int32, AutoPtr<DevicePolicyData> >::Iterator it = mHost->mUserData.Find(userHandle);
            if (it != mHost->mUserData.End()) {
                mHost->mUserData.Erase(it);

            }
        }

        mHost->HandlePackagesChanged(userHandle);
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CDevicePolicyManagerService::DevicePolicyReceiverRunnable, IRunnable)

CDevicePolicyManagerService::DevicePolicyReceiverRunnable::DevicePolicyReceiverRunnable(
    /* [in] */ CDevicePolicyManagerService* host,
    /* [in] */ Int32 userHandle)
    : mHost(host)
    , mUserHandle(userHandle)
{}

ECode CDevicePolicyManagerService::DevicePolicyReceiverRunnable::Run()
{
    mHost->HandlePasswordExpirationNotification(mHost->GetUserData(mUserHandle));
    return NOERROR;
}

CAR_INTERFACE_IMPL(CDevicePolicyManagerService::DeviceOrUserLockedRunnable, IRunnable)

CDevicePolicyManagerService::DeviceOrUserLockedRunnable::DeviceOrUserLockedRunnable(
    /* [in] */ CDevicePolicyManagerService* host,
    /* [in] */ Int32 userHandle)
    : mHost(host)
    , mUserHandle(userHandle)
{}

ECode CDevicePolicyManagerService::DeviceOrUserLockedRunnable::Run()
{
    AutoPtr<IIActivityManager> activityMgr = ActivityManagerNative::GetDefault();
    Boolean result;
    activityMgr->SwitchUser(0, &result);

    AutoPtr<IInterface> service;
    FAIL_RETURN(mHost->mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service));
    AutoPtr<IUserManager> userMgr = IUserManager::Probe(service);
    userMgr->RemoveUser(mUserHandle, &result);
    return NOERROR;
}

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

Int32 CDevicePolicyManagerService::ActiveAdmin::GetUid()
{
    AutoPtr<IActivityInfo> activityInfo;
    mInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> appInfo;
    activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
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
    out->WriteStartTag(String(NULL), String("policies"));
    mInfo->WritePoliciesToXml(out);
    out->WriteEndTag(String(NULL), String("policies"));
    if (mPasswordQuality != IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
        out->WriteStartTag(String(NULL), String("password-quality"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mPasswordQuality));
        out->WriteEndTag(String(NULL), String("password-quality"));
        if (mMinimumPasswordLength != DEF_MINIMUM_PASSWORD_LENGTH) {
            out->WriteStartTag(String(NULL), String("min-password-length"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordLength));
            out->WriteEndTag(String(NULL), String("min-password-length"));
        }
        if(mPasswordHistoryLength != DEF_PASSWORD_HISTORY_LENGTH) {
            out->WriteStartTag(String(NULL), String("password-history-length"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mPasswordHistoryLength));
            out->WriteEndTag(String(NULL), String("password-history-length"));
        }
        if (mMinimumPasswordUpperCase != DEF_MINIMUM_PASSWORD_UPPER_CASE) {
            out->WriteStartTag(String(NULL), String("min-password-uppercase"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordUpperCase));
            out->WriteEndTag(String(NULL), String("min-password-uppercase"));
        }
        if (mMinimumPasswordLowerCase != DEF_MINIMUM_PASSWORD_LOWER_CASE) {
            out->WriteStartTag(String(NULL), String("min-password-lowercase"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordLowerCase));
            out->WriteEndTag(String(NULL), String("min-password-lowercase"));
        }
        if (mMinimumPasswordLetters != DEF_MINIMUM_PASSWORD_LETTERS) {
            out->WriteStartTag(String(NULL), String("min-password-letters"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordLetters));
            out->WriteEndTag(String(NULL), String("min-password-letters"));
        }
        if (mMinimumPasswordNumeric != DEF_MINIMUM_PASSWORD_NUMERIC) {
            out->WriteStartTag(String(NULL), String("min-password-numeric"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordNumeric));
            out->WriteEndTag(String(NULL), String("min-password-numeric"));
        }
        if (mMinimumPasswordSymbols != DEF_MINIMUM_PASSWORD_SYMBOLS) {
            out->WriteStartTag(String(NULL), String("min-password-symbols"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordSymbols));
            out->WriteEndTag(String(NULL), String("min-password-symbols"));
        }
        if (mMinimumPasswordNonLetter > DEF_MINIMUM_PASSWORD_NON_LETTER) {
            out->WriteStartTag(String(NULL), String("min-password-nonletter"));
            out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMinimumPasswordNonLetter));
            out->WriteEndTag(String(NULL), String("min-password-nonletter"));
        }
    }
    if (mMaximumTimeToUnlock != DEF_MAXIMUM_TIME_TO_UNLOCK) {
        out->WriteStartTag(String(NULL), String("max-time-to-unlock"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int64ToString(mMaximumTimeToUnlock));
        out->WriteEndTag(String(NULL), String("max-time-to-unlock"));
    }
    if (mMaximumFailedPasswordsForWipe != DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE) {
        out->WriteStartTag(String(NULL), String("max-failed-password-wipe"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mMaximumFailedPasswordsForWipe));
        out->WriteEndTag(String(NULL), String("max-failed-password-wipe"));
    }
    if (mSpecifiesGlobalProxy) {
        out->WriteStartTag(String(NULL), String("specifies-global-proxy"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::BooleanToString(mSpecifiesGlobalProxy));
        out->WriteEndTag(String(NULL), String("specifies_global_proxy"));
        if (!mGlobalProxySpec.IsNull()) {
            out->WriteStartTag(String(NULL), String("global-proxy-spec"));
            out->WriteAttribute(String(NULL), String("value"), mGlobalProxySpec);
            out->WriteEndTag(String(NULL), String("global-proxy-spec"));
        }
        if (!mGlobalProxyExclusionList.IsNull()) {
            out->WriteStartTag(String(NULL), String("global-proxy-exclusion-list"));
            out->WriteAttribute(String(NULL), String("value"), mGlobalProxyExclusionList);
            out->WriteEndTag(String(NULL), String("global-proxy-exclusion-list"));
        }
    }
    if (mPasswordExpirationTimeout != DEF_PASSWORD_EXPIRATION_TIMEOUT) {
        out->WriteStartTag(String(NULL), String("password-expiration-timeout"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int64ToString(mPasswordExpirationTimeout));
        out->WriteEndTag(String(NULL), String("password-expiration-timeout"));
    }
    if (mPasswordExpirationDate != DEF_PASSWORD_EXPIRATION_DATE) {
        out->WriteStartTag(String(NULL), String("password-expiration-date"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int64ToString(mPasswordExpirationDate));
        out->WriteEndTag(String(NULL), String("password-expiration-date"));
    }
    if (mEncryptionRequested) {
        out->WriteStartTag(String(NULL), String("encryption-requested"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::BooleanToString(mEncryptionRequested));
        out->WriteEndTag(String(NULL), String("encryption-requested"));
    }
    if (mDisableCamera) {
        out->WriteStartTag(String(NULL), String("disable-camera"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::BooleanToString(mDisableCamera));
        out->WriteEndTag(String(NULL), String("disable-camera"));
    }
    if (mDisabledKeyguardFeatures != DEF_KEYGUARD_FEATURES_DISABLED) {
        out->WriteStartTag(String(NULL), String("disable-keyguard-features"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(mDisabledKeyguardFeatures));
        out->WriteEndTag(String(NULL), String("disable-keyguard-features"));
    }
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
        if (String("policies").Equals(tag)) {
            mInfo->ReadPoliciesFromXml(parser);
        } else if (String("password-quality").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mPasswordQuality = StringUtils::ParseInt32(value);
        } else if (String("min-password-length").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordLength = StringUtils::ParseInt32(value);
        } else if (String("password-history-length").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mPasswordHistoryLength = StringUtils::ParseInt32(value);
        } else if (String("min-password-uppercase").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordUpperCase = StringUtils::ParseInt32(value);
        } else if (String("min-password-lowercase").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordLowerCase = StringUtils::ParseInt32(value);
        } else if (String("min-password-letters").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordLetters = StringUtils::ParseInt32(value);
        } else if (String("min-password-numeric").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordNumeric = StringUtils::ParseInt32(value);
        } else if (String("min-password-symbols").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordSymbols = StringUtils::ParseInt32(value);
        } else if (String("min-password-nonletter").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMinimumPasswordNonLetter = StringUtils::ParseInt32(value);
        } else if (String("max-time-to-unlock").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMaximumTimeToUnlock = StringUtils::ParseInt64(value);
        } else if (String("max-failed-password-wipe").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mMaximumFailedPasswordsForWipe = StringUtils::ParseInt32(value);
        } else if (String("specifies-global-proxy").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mSpecifiesGlobalProxy = StringUtils::ParseInt32(value);
        } else if (String("global-proxy-spec").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mGlobalProxySpec = value;
        } else if (String("global-proxy-exclusion-list").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mGlobalProxyExclusionList = value;
        } else if (String("password-expiration-timeout").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mPasswordExpirationTimeout = StringUtils::ParseInt64(value);
        } else if (String("password-expiration-date").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mPasswordExpirationDate = StringUtils::ParseInt64(value);
        } else if (String("encryption-requested").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mEncryptionRequested = StringUtils::ParseInt32(value);
        } else if (String("disable-camera").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mDisableCamera = StringUtils::ParseInt32(value);
        } else if (String("disable-keyguard-features").Equals(tag)) {
            String value;
            parser->GetAttributeValue(String(NULL), String("value"), &value);
            mDisabledKeyguardFeatures = StringUtils::ParseInt32(value);
        } else {
            Slogger::W(TAG, "Unknown admin tag: %s", tag.string());
        }
        XmlUtils::SkipCurrentTag(parser);
    }
}

void CDevicePolicyManagerService::ActiveAdmin::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
//     pw.print(prefix); pw.print("uid="); pw.println(getUid());
//     pw.print(prefix); pw.println("policies:");
//     ArrayList<DeviceAdminInfo.PolicyInfo> pols = info.getUsedPolicies();
//     if (pols != null) {
//         for (int i=0; i<pols.size(); i++) {
//             pw.print(prefix); pw.print("  "); pw.println(pols.get(i).tag);
//         }
//     }
//     pw.print(prefix); pw.print("passwordQuality=0x");
//             pw.println(Integer.toHexString(passwordQuality));
//     pw.print(prefix); pw.print("minimumPasswordLength=");
//             pw.println(minimumPasswordLength);
//     pw.print(prefix); pw.print("passwordHistoryLength=");
//             pw.println(passwordHistoryLength);
//     pw.print(prefix); pw.print("minimumPasswordUpperCase=");
//             pw.println(minimumPasswordUpperCase);
//     pw.print(prefix); pw.print("minimumPasswordLowerCase=");
//             pw.println(minimumPasswordLowerCase);
//     pw.print(prefix); pw.print("minimumPasswordLetters=");
//             pw.println(minimumPasswordLetters);
//     pw.print(prefix); pw.print("minimumPasswordNumeric=");
//             pw.println(minimumPasswordNumeric);
//     pw.print(prefix); pw.print("minimumPasswordSymbols=");
//             pw.println(minimumPasswordSymbols);
//     pw.print(prefix); pw.print("minimumPasswordNonLetter=");
//             pw.println(minimumPasswordNonLetter);
//     pw.print(prefix); pw.print("maximumTimeToUnlock=");
//             pw.println(maximumTimeToUnlock);
//     pw.print(prefix); pw.print("maximumFailedPasswordsForWipe=");
//             pw.println(maximumFailedPasswordsForWipe);
//     pw.print(prefix); pw.print("specifiesGlobalProxy=");
//             pw.println(specifiesGlobalProxy);
//     pw.print(prefix); pw.print("passwordExpirationTimeout=");
//             pw.println(passwordExpirationTimeout);
//     pw.print(prefix); pw.print("passwordExpirationDate=");
//             pw.println(passwordExpirationDate);
//     if (globalProxySpec != null) {
//         pw.print(prefix); pw.print("globalProxySpec=");
//                 pw.println(globalProxySpec);
//     }
//     if (globalProxyExclusionList != null) {
//         pw.print(prefix); pw.print("globalProxyEclusionList=");
//                 pw.println(globalProxyExclusionList);
//     }
//     pw.print(prefix); pw.print("encryptionRequested=");
//             pw.println(encryptionRequested);
//     pw.print(prefix); pw.print("disableCamera=");
//             pw.println(disableCamera);
//     pw.print(prefix); pw.print("disabledKeyguardFeatures=");
//             pw.println(disabledKeyguardFeatures);
}

CDevicePolicyManagerService::CDevicePolicyManagerService()
{
    CHandler::New((IHandler**)&mHandler);
    mReceiver = new DevicePolicyReceiver(this);
}

ECode CDevicePolicyManagerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> powerMgr = IPowerManager::Probe(service);
    powerMgr->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("DPM"), (IPowerManagerWakeLock**)&mWakeLock);

    AutoPtr<IIntentFilter> bootCompletedFilter;
    CIntentFilter::New((IIntentFilter**)&bootCompletedFilter);
    bootCompletedFilter->AddAction(IIntent::ACTION_BOOT_COMPLETED);
    bootCompletedFilter->AddAction(ACTION_EXPIRED_PASSWORD_NOTIFICATION);
    bootCompletedFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    bootCompletedFilter->AddAction(IIntent::ACTION_USER_STARTED);

    AutoPtr<IIntent> bootCompletedIntent;
    context->RegisterReceiverAsUser(mReceiver, UserHandle::ALL, bootCompletedFilter, String(NULL), mHandler, (IIntent**)&bootCompletedIntent);

    AutoPtr<IIntentFilter> pkgChangedFilter;
    CIntentFilter::New((IIntentFilter**)&pkgChangedFilter);
    pkgChangedFilter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    pkgChangedFilter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    pkgChangedFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    pkgChangedFilter->AddDataScheme(String("package"));

    AutoPtr<IIntent> pkgChangedIntent;
    context->RegisterReceiverAsUser(mReceiver, UserHandle::ALL, pkgChangedFilter, String(NULL), mHandler, (IIntent**)&pkgChangedIntent);
    return NOERROR;
}

CDevicePolicyManagerService::~CDevicePolicyManagerService()
{}

ECode CDevicePolicyManagerService::SetPasswordQuality(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 quality,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(ValidateQualityConstant(quality));
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));
    if (ap->mPasswordQuality != quality) {
        ap->mPasswordQuality = quality;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordQuality(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    Int32 mode = IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);

        if (admin != NULL) {
            *password = admin->mPasswordQuality;
        } else {
            *password = mode;
        }
        return NOERROR;
    }

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (mode < admin->mPasswordQuality) {
            mode = admin->mPasswordQuality;
        }
    }
    *password = mode;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        Slogger::E(TAG, "ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));

    if (ap->mMinimumPasswordLength != length) {
        ap->mMinimumPasswordLength = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
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

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordLength) {
            length = admin->mMinimumPasswordLength;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));

    if (ap->mMinimumPasswordUpperCase != length) {
        ap->mMinimumPasswordUpperCase = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumUpperCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    *password = 0;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordUpperCase) {
            length = admin->mMinimumPasswordUpperCase;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));

    if (ap->mMinimumPasswordLowerCase != length) {
        ap->mMinimumPasswordLowerCase = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumLowerCase(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordLowerCase) {
            length = admin->mMinimumPasswordLowerCase;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumLetters(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));

    if (ap->mMinimumPasswordLetters != length) {
        ap->mMinimumPasswordLetters = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumLetters(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordLetters) {
            length = admin->mMinimumPasswordLetters;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumNumeric(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));
    if (ap->mMinimumPasswordNumeric != length) {
        ap->mMinimumPasswordNumeric = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumNumeric(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordNumeric) {
            length = admin->mMinimumPasswordNumeric;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumSymbols(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));
    if (ap->mMinimumPasswordSymbols != length) {
        ap->mMinimumPasswordSymbols = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumSymbols(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordSymbols) {
            length = admin->mMinimumPasswordSymbols;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));
    if (ap->mMinimumPasswordNonLetter != length) {
        ap->mMinimumPasswordNonLetter = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordMinimumNonLetter(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mMinimumPasswordNonLetter) {
            length = admin->mMinimumPasswordNonLetter;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordHistoryLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 length,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));

    if (ap->mPasswordHistoryLength != length) {
        ap->mPasswordHistoryLength = length;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordHistoryLength(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
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

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (length < admin->mPasswordHistoryLength) {
            length = admin->mPasswordHistoryLength;
        }
    }
    *password = length;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetPasswordExpirationTimeout(
    /* [in] */ IComponentName* who,
    /* [in] */ Int64 timeout,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (timeout < 0) {
        //throw new IllegalArgumentException("Timeout must be >= 0 ms");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_EXPIRE_PASSWORD, (ActiveAdmin**)&ap));

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    // Calling this API automatically bumps the expiration date
    Int64 expiration = timeout > 0L ? (timeout + millis) : 0L;
    ap->mPasswordExpirationDate = expiration;
    ap->mPasswordExpirationTimeout = timeout;
    if (timeout > 0L) {
        // Slog.w(TAG, "setPasswordExpiration(): password will expire on "
        //         + DateFormat.getDateTimeInstance(DateFormat.DEFAULT, DateFormat.DEFAULT)
        //         .format(new Date(expiration)));
    }
    SaveSettingsLocked(userHandle);
    // in case this is the first one
    SetExpirationAlarmCheckLocked(mContext, GetUserData(userHandle));

    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordExpirationTimeout(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
        if (admin != NULL) {
            *password = admin->mPasswordExpirationTimeout;
        } else {
            *password = 0L;
        }
        return NOERROR;
    }

    Int64 timeout = 0L;
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (timeout == 0L || (admin->mPasswordExpirationTimeout != 0L && timeout > admin->mPasswordExpirationTimeout)) {
            timeout = admin->mPasswordExpirationTimeout;
        }
    }
    *password = timeout;
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetPasswordExpiration(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    *password = GetPasswordExpirationLocked(who, userHandle);
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsActivePasswordSufficient(
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    // This API can only be called by an active device admin,
    // so try to retrieve it to check that the caller is one.
    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, (ActiveAdmin**)&ap));

    Int32 quality;
    FAIL_RETURN(GetPasswordQuality(NULL, userHandle, &quality));
    Int32 length;
    FAIL_RETURN(GetPasswordMinimumLength(NULL, userHandle, &length));

    if (policy->mActivePasswordQuality < quality || policy->mActivePasswordLength < length) {
        *result = FALSE;
        return NOERROR;
    }
    if (policy->mActivePasswordQuality != IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 upperCase;
    FAIL_RETURN(GetPasswordMinimumUpperCase(NULL, userHandle, &upperCase));
    Int32 lowerCase;
    FAIL_RETURN(GetPasswordMinimumLowerCase(NULL, userHandle, &lowerCase));
    Int32 letters;
    FAIL_RETURN(GetPasswordMinimumLetters(NULL, userHandle, &letters));
    Int32 numeric;
    FAIL_RETURN(GetPasswordMinimumNumeric(NULL, userHandle, &numeric));
    Int32 symbols;
    FAIL_RETURN(GetPasswordMinimumSymbols(NULL, userHandle, &symbols));
    Int32 nonLetter;
    FAIL_RETURN(GetPasswordMinimumNonLetter(NULL, userHandle, &nonLetter));

    *result = policy->mActivePasswordUpperCase >= upperCase
            && policy->mActivePasswordLowerCase >= lowerCase
            && policy->mActivePasswordLetters >= letters
            && policy->mActivePasswordNumeric >= numeric
            && policy->mActivePasswordSymbols >= symbols
            && policy->mActivePasswordNonLetter >= nonLetter;

    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCurrentFailedPasswordAttempts(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);
    // This API can only be called by an active device admin,
    // so try to retrieve it to check that the caller is one.
    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, (ActiveAdmin**)&ap));

    *password = GetUserData(userHandle)->mFailedPasswordAttempts;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 num,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    // This API can only be called by an active device admin,
    // so try to retrieve it to check that the caller is one.
    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_WIPE_DATA, (ActiveAdmin**)&ap));
    ap = NULL;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, (ActiveAdmin**)&ap));

    if (ap->mMaximumFailedPasswordsForWipe != num) {
        ap->mMaximumFailedPasswordsForWipe = num;
        SaveSettingsLocked(userHandle);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetMaximumFailedPasswordsForWipe(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* password)
{
    VALIDATE_NOT_NULL(password);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    Int32 count = 0;

    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
        if (admin != NULL) {
            *password = admin->mMaximumFailedPasswordsForWipe;
        } else {
            *password = count;
        }
        return NOERROR;
    }

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (count == 0) {
            count = admin->mMaximumFailedPasswordsForWipe;
        } else if (admin->mMaximumFailedPasswordsForWipe != 0 && count > admin->mMaximumFailedPasswordsForWipe) {
            count = admin->mMaximumFailedPasswordsForWipe;
        }
    }
    *password = count;
    return NOERROR;
}

ECode CDevicePolicyManagerService::ResetPassword(
    /* [in] */ const String& password,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    Int32 quality;
    {
        AutoLock lock(mLock);

        // This API can only be called by an active device admin,
        // so try to retrieve it to check that the caller is one.
        AutoPtr<ActiveAdmin> ap;
        FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_RESET_PASSWORD, (ActiveAdmin**)&ap));
        GetPasswordQuality(NULL, userHandle, &quality);
        if (quality != IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
            // TODO:
            // Int32 realQuality = LockPatternUtils::computePasswordQuality(password);

            // if (realQuality < quality && quality != IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX) {
            //     StringBuffer buf;
            //     buf += "resetPassword: password quality 0x";
            //     buf += StringUtils::ToString(quality, 16);
            //     buf += " does not meet required quality 0x";
            //     buf += StringUtils::ToString(quality, 16);
            //     Slogger::W(TAG, buf.ToString());
            //     *result = FALSE;
            //     return NOERROR;
            // }

            // quality = Elastos::Core::Math::Max(realQuality, quality);
        }

        Int32 length;
        GetPasswordMinimumLength(NULL, userHandle, &length);
        if ((Int32)password.GetLength() < length) {
            StringBuffer buf;
            buf += "resetPassword: password length ";
            buf += password.GetLength();
            buf += " does not meet required length ";
            buf += length;
            Slogger::W(TAG, buf.ToString());
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
                Slogger::W(TAG, buf.ToString());
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
                Slogger::W(TAG, buf.ToString());
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
                Slogger::W(TAG, buf.ToString());
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
                Slogger::W(TAG, buf.ToString());
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
                Slogger::W(TAG, buf.ToString());
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
                Slogger::W(TAG, buf.ToString());
                return NOERROR;
            }
        }
    }

    Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    if (policy->mPasswordOwner >= 0 && policy->mPasswordOwner != callingUid) {
        Slogger::W(TAG, "resetPassword: already set by another uid and not entered by user");
        return NOERROR;
    }

    // Don't do this with the lock held, because it is going to call
    // back in to the service.
    Int64 ident = Binder::ClearCallingIdentity();

    // TODO:
    AutoPtr<ILockPatternUtils> utils;
    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&utils);
    utils->SaveLockPassword(password, quality, FALSE, userHandle);
    {
        AutoLock lock(mLock);

        Int32 newOwner = (flags & IDevicePolicyManager::RESET_PASSWORD_REQUIRE_ENTRY) != 0 ? callingUid : -1;
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
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_FORCE_LOCK, (ActiveAdmin**)&ap));

    if (ap->mMaximumTimeToUnlock != timeMs) {
        ap->mMaximumTimeToUnlock = timeMs;
        SaveSettingsLocked(userHandle);
        AutoPtr<DevicePolicyData> data = GetUserData(userHandle);
        UpdateMaximumTimeToLockLocked(data);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetMaximumTimeToLock(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    Int64 newtime = 0;
    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
        if (admin != NULL) {
            *value = admin->mMaximumTimeToUnlock;
        } else {
            *value = newtime;
        }
        return NOERROR;
    }

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (newtime == 0) {
            newtime = admin->mMaximumTimeToUnlock;
        } else if (admin->mMaximumTimeToUnlock != 0 && newtime > admin->mMaximumTimeToUnlock) {
            newtime = admin->mMaximumTimeToUnlock;
        }
    }
    *value = newtime;
    return NOERROR;
}

ECode CDevicePolicyManagerService::LockNow()
{
    AutoLock lock(mLock);

    // This API can only be called by an active device admin,
    // so try to retrieve it to check that the caller is one.
    AutoPtr<ActiveAdmin> admin;
    FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_FORCE_LOCK, (ActiveAdmin**)&admin));
    LockNowUnchecked();

    return NOERROR;
}

ECode CDevicePolicyManagerService::WipeData(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    // This API can only be called by an active device admin,
    // so try to retrieve it to check that the caller is one.
    AutoPtr<ActiveAdmin> admin;
    FAIL_RETURN(GetActiveAdminForCallerLocked(NULL, IDeviceAdminInfo::USES_POLICY_WIPE_DATA, (ActiveAdmin**)&admin));
    Int64 ident = Binder::ClearCallingIdentity();

    WipeDeviceOrUserLocked(flags, userHandle);

    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetGlobalProxy(
    /* [in] */ IComponentName* who,
    /* [in] */ const String& proxySpec,
    /* [in] */ const String& exclusionList,
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component);
    *component = NULL;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    // Only check if owner has set global proxy. We don't allow other users to set it.
    AutoPtr<DevicePolicyData> policy = GetUserData(IUserHandle::USER_OWNER);
    AutoPtr<ActiveAdmin> admin;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_SETS_GLOBAL_PROXY, (ActiveAdmin**)&admin));

    // Scan through active admins and find if anyone has already
    // set the global proxy.
    HashMap< AutoPtr<IComponentName>, AutoPtr<ActiveAdmin> >::Iterator it = policy->mAdminMap.Begin();
    for (; it != policy->mAdminMap.End(); ++it) {
        if (it->mSecond == NULL) {
            continue;
        }
        AutoPtr<IComponentName> tmpComponent = it->mFirst;
        AutoPtr<ActiveAdmin> ap = it->mSecond;

        Boolean isEqual;
        tmpComponent->Equals(who, &isEqual);

        if (ap->mSpecifiesGlobalProxy && !isEqual) {
            // Another admin already sets the global proxy
            // Return it to the caller.
            *component = tmpComponent;
            REFCOUNT_ADD(*component);
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

        Slogger::W(TAG, buf.ToString());
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
    ResetGlobalProxyLocked(policy);
    Binder::RestoreCallingIdentity(origId);

    return NOERROR;
}

ECode CDevicePolicyManagerService::GetGlobalProxyAdmin(
    /* [in] */ Int32 userHandle,
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component);
    *component = NULL;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));
    {
        AutoLock lock(mLock);

        AutoPtr<DevicePolicyData> policy = GetUserData(IUserHandle::USER_OWNER);
        // Scan through active admins and find if anyone has already
        // set the global proxy.
        List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
        for (; iter != policy->mAdminList.End(); ++iter) {
            AutoPtr<ActiveAdmin> ap = *iter;
            if (ap->mSpecifiesGlobalProxy) {
                // Device admin sets the global proxy
                // Return it to the caller.
                ap->mInfo->GetComponent(component);
                return NOERROR;
            }
        }
    }
    // No device admin sets the global proxy.
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetStorageEncryption(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean encrypt,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    // Check for permissions
    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
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
        Slogger::W(TAG, buf.ToString());
        *value = 0;
        return NOERROR;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_ENCRYPTED_STORAGE, (ActiveAdmin**)&ap));

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
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        newRequested |= (*iter)->mEncryptionRequested;
    }

    // Notify OS of new request
    SetEncryptionRequested(newRequested);

    // Return the new global request status
    *value = (newRequested) ? IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVE : IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE;
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetStorageEncryption(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        if ((*iter)->mEncryptionRequested) {
            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetStorageEncryptionStatus(
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* st)
{
    VALIDATE_NOT_NULL(st);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));
    *st = GetEncryptionStatus();
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetCameraDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Boolean disabled,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DISABLE_CAMERA, (ActiveAdmin**)&ap));

    if (ap->mDisableCamera != disabled) {
        ap->mDisableCamera = disabled;
        SaveSettingsLocked(userHandle);
    }
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    SyncDeviceCapabilitiesLocked(policy);
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetCameraDisabled(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoLock lock(mLock);

    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
        if (admin == NULL) return NOERROR;

        *result = admin->mDisableCamera;
        return NOERROR;
    }

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    // Determine whether or not the device camera is disabled for any active admins.
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        if ((*iter)->mDisableCamera) {
            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 which,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    if (who == NULL) {
        //throw new NullPointerException("ComponentName is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> ap;
    FAIL_RETURN(GetActiveAdminForCallerLocked(who, IDeviceAdminInfo::USES_POLICY_DISABLE_KEYGUARD_FEATURES, (ActiveAdmin**)&ap));
    if (ap->mDisabledKeyguardFeatures != which) {
        ap->mDisabledKeyguardFeatures = which;
        SaveSettingsLocked(userHandle);
    }
    SyncDeviceCapabilitiesLocked(GetUserData(userHandle));
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetKeyguardDisabledFeatures(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* features)
{
    VALIDATE_NOT_NULL(features);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

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
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    Int32 which = 0;
    for (; iter != policy->mAdminList.End(); ++iter) {
        which |= (*iter)->mDisabledKeyguardFeatures;
    }
    *features = which;
    return NOERROR;
}

ECode CDevicePolicyManagerService::SetActiveAdmin(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Boolean refreshing,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)));
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<IDeviceAdminInfo> info;
    FindAdmin(adminReceiver, userHandle, (IDeviceAdminInfo**)&info);
    if (info == NULL) {
        //throw new IllegalArgumentException("Bad admin: " + adminReceiver);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoLock lock(mLock);

    Int64 ident = Binder::ClearCallingIdentity();

    if (!refreshing && GetActiveAdminUncheckedLocked(adminReceiver, userHandle) != NULL) {
        //throw new IllegalArgumentException("Admin is already added");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ActiveAdmin> newAdmin = new ActiveAdmin(info);
    policy->mAdminMap[adminReceiver] = newAdmin;
    Int32 replaceIndex = -1;

    if (refreshing) {
        Int32 i = 0;
        List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
        for (; iter != policy->mAdminList.End(); ++iter, ++i) {
            AutoPtr<ActiveAdmin> oldAdmin = *iter;
            AutoPtr<IComponentName> comp;
            oldAdmin->mInfo->GetComponent((IComponentName**)&comp);
            Boolean isEqual;
            comp->Equals(adminReceiver, &isEqual);
            if (isEqual) {
                replaceIndex = i;
                break;
            }
        }
    }
    if (replaceIndex == -1) {
        policy->mAdminList.PushBack(newAdmin);
    } else {
        policy->mAdminList[replaceIndex] = newAdmin;
    }
    SaveSettingsLocked(userHandle);
    SendAdminCommandLocked(newAdmin, IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_ENABLED);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CDevicePolicyManagerService::IsAdminActive(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* isActive)
{
    VALIDATE_NOT_NULL(isActive);
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
    *isActive = admin != NULL;
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetActiveAdmins(
    /* [in] */ Int32 userHandle,
    /* [out] */ IObjectContainer** admins)
{
    VALIDATE_NOT_NULL(admins);
    *admins = NULL;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    if (policy->mAdminList.Begin() == policy->mAdminList.End()) {
        return NOERROR;
    }

    AutoPtr<IObjectContainer> res;
    CObjectContainer::New((IObjectContainer**)&res);

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<IComponentName> comp;
        (*iter)->mInfo->GetComponent((IComponentName**)&comp);
        res->Add(comp);
    }
    *admins = res;
    REFCOUNT_ADD(*admins);
    return NOERROR;
}

ECode CDevicePolicyManagerService::PackageHasActiveAdmins(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* hasActive)
{
    VALIDATE_NOT_NULL(hasActive);
    *hasActive = FALSE;

    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        String name;
        (*iter)->mInfo->GetPackageName(&name);
        if (name.Equals(packageName)) {
            *hasActive = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::GetRemoveWarning(
    /* [in] */ IComponentName* comp,
    /* [in] */ IRemoteCallback* result,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)));

    AutoLock lock(mLock);

    AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(comp, userHandle);
    if (admin == NULL) {
        result->SendResult(NULL);
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_DISABLE_REQUESTED, (IIntent**)&intent);
    AutoPtr<IComponentName> component;
    admin->mInfo->GetComponent((IComponentName**)&component);
    intent->SetComponent(component);

    AutoPtr<IUserHandle> handle;
    CUserHandle::New(userHandle, (IUserHandle**)&handle);
    AutoPtr<IBroadcastReceiver> receiver = new RemoveWarningReceiver(result);
    mContext->SendOrderedBroadcastAsUser(intent, handle, String(NULL), receiver,
            NULL, IActivity::RESULT_OK, String(NULL), NULL);

    return NOERROR;
}

ECode CDevicePolicyManagerService::RemoveActiveAdmin(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
    if (admin == NULL) {
        return NOERROR;
    }
    if (admin->GetUid() != Binder::GetCallingUid()) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)));
    }
    Int64 ident = Binder::ClearCallingIdentity();

    RemoveActiveAdminLocked(adminReceiver, userHandle);

    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CDevicePolicyManagerService::HasGrantedPolicy(
    /* [in] */ IComponentName* adminReceiver,
    /* [in] */ Int32 policyId,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* hasGrant)
{
    VALIDATE_NOT_NULL(hasGrant);
    *hasGrant = FALSE;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoLock lock(mLock);

    AutoPtr<ActiveAdmin> administrator = GetActiveAdminUncheckedLocked(adminReceiver, userHandle);
    if (administrator == NULL) {
        //throw new SecurityException("No active admin " + adminReceiver);
        return E_SECURITY_EXCEPTION;
    }
    administrator->mInfo->UsesPolicy(policyId, hasGrant);
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
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)));

    AutoPtr<DevicePolicyData> p = GetUserData(userHandle);
    ValidateQualityConstant(quality);

    AutoLock lock(mLock);

    if (p->mActivePasswordQuality != quality || p->mActivePasswordLength != length
            || p->mFailedPasswordAttempts != 0 || p->mActivePasswordLetters != letters
            || p->mActivePasswordUpperCase != uppercase
            || p->mActivePasswordLowerCase != lowercase || p->mActivePasswordNumeric != numbers
            || p->mActivePasswordSymbols != symbols || p->mActivePasswordNonLetter != nonletter)
    {
        Int64 ident = Binder::ClearCallingIdentity();

        p->mActivePasswordQuality = quality;
        p->mActivePasswordLength = length;
        p->mActivePasswordLetters = letters;
        p->mActivePasswordLowerCase = lowercase;
        p->mActivePasswordUpperCase = uppercase;
        p->mActivePasswordNumeric = numbers;
        p->mActivePasswordSymbols = symbols;
        p->mActivePasswordNonLetter = nonletter;
        p->mFailedPasswordAttempts = 0;
        SaveSettingsLocked(userHandle);
        UpdatePasswordExpirationsLocked(userHandle);
        SetExpirationAlarmCheckLocked(mContext, p);
        SendAdminCommandLocked(IDeviceAdminReceiver::ACTION_PASSWORD_CHANGED, IDeviceAdminInfo::USES_POLICY_LIMIT_PASSWORD, userHandle);

        Binder::RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::ReportFailedPasswordAttempt(
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    Int64 ident = Binder::ClearCallingIdentity();

    policy->mFailedPasswordAttempts++;
    SaveSettingsLocked(userHandle);
    Int32 max;
    FAIL_RETURN(GetMaximumFailedPasswordsForWipe(NULL, userHandle, &max));
    if (max > 0 && policy->mFailedPasswordAttempts >= max) {
        WipeDeviceOrUserLocked(0, userHandle);
    }
    SendAdminCommandLocked(IDeviceAdminReceiver::ACTION_PASSWORD_FAILED, IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, userHandle);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CDevicePolicyManagerService::ReportSuccessfulPasswordAttempt(
    /* [in] */ Int32 userHandle)
{
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BIND_DEVICE_ADMIN, String(NULL)));

    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    if (policy->mFailedPasswordAttempts != 0 || policy->mPasswordOwner >= 0) {
        Int64 ident = Binder::ClearCallingIdentity();

        policy->mFailedPasswordAttempts = 0;
        policy->mPasswordOwner = -1;
        SaveSettingsLocked(userHandle);
        SendAdminCommandLocked(IDeviceAdminReceiver::ACTION_PASSWORD_SUCCEEDED, IDeviceAdminInfo::USES_POLICY_WATCH_LOGIN, userHandle);

        Binder::RestoreCallingIdentity(ident);
    }
    return NOERROR;
}

ECode CDevicePolicyManagerService::FindAdmin(
    /* [in] */ IComponentName* adminName,
    /* [in] */ Int32 userHandle,
    /* [out] */ IDeviceAdminInfo** adminInfo)
{
    VALIDATE_NOT_NULL(adminInfo);
    *adminInfo = NULL;
    FAIL_RETURN(EnforceCrossUserPermission(userHandle));

    AutoPtr<IIntent> resolveIntent;
    CIntent::New((IIntent**)&resolveIntent);
    resolveIntent->SetComponent(adminName);

    AutoPtr<IPackageManager> pkgMgr;
    mContext->GetPackageManager((IPackageManager**)&pkgMgr);

    AutoPtr<IObjectContainer> infos;
    pkgMgr->QueryBroadcastReceivers(resolveIntent, IPackageManager::GET_META_DATA, userHandle, (IObjectContainer**)&infos);

    if (infos == NULL) {
        //throw new IllegalArgumentException("Unknown admin: " + adminName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 count;
    infos->GetObjectCount(&count);
    if (count <= 0) {
        //throw new IllegalArgumentException("Unknown admin: " + adminName);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IObjectEnumerator> enumerator;
    infos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);

    AutoPtr<IInterface> value;
    Boolean isSucceeded;
    enumerator->MoveNext(&isSucceeded);
    if (isSucceeded) enumerator->Current((IInterface**)&value);

    AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(value);

    ECode ec = CDeviceAdminInfo::New(mContext, resolveInfo, adminInfo);

    if (FAILED(ec)) {
        //Slog.w(TAG, "Bad device admin requested for user=" + userHandle + ": " + adminName, e);

        StringBuffer buf;
        buf += "Bad device admin requested for user=";
        buf += userHandle;
        Slogger::I(TAG, buf.ToString());
        *adminInfo = NULL;
        return NOERROR;
    }

    return NOERROR;
}

ECode CDevicePolicyManagerService::SystemReady()
{
    AutoLock lock(mLock);
    LoadSettingsLocked(GetUserData(IUserHandle::USER_OWNER), IUserHandle::USER_OWNER);
    return NOERROR;
}

ECode CDevicePolicyManagerService::EnforceCrossUserPermission(
    /* [in] */ Int32 userHandle)
{
    if (userHandle < 0) {
        // throw new IllegalArgumentException("Invalid userId " + userHandle);
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

ECode CDevicePolicyManagerService::GetActiveAdminForCallerLocked(
    /* [in] */ IComponentName* who,
    /* [in] */ Int32 reqPolicy,
    /* [out] */ ActiveAdmin** admin)
{
    VALIDATE_NOT_NULL(admin);
    *admin = NULL;

    Int32 callingUid = Binder::GetCallingUid();

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    Int32 userHandle;
    handleHelper->GetUserId(callingUid, &userHandle);

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    if (who != NULL) {
        AutoPtr<ActiveAdmin> tmpAdmin;
        HashMap< AutoPtr<IComponentName>, AutoPtr<ActiveAdmin> >::Iterator it = policy->mAdminMap.Find(who);
        if (it != policy->mAdminMap.End()) {
            tmpAdmin = it->mSecond;
        }
        if (tmpAdmin == NULL) {
            return E_SECURITY_EXCEPTION;
        }

        Int32 uid = tmpAdmin->GetUid();
        if (uid != callingUid) {
            //throw new SecurityException("Admin " + who + " is not owned by uid " + Binder.getCallingUid());
            return E_SECURITY_EXCEPTION;
        }

        Boolean policyResult;
        tmpAdmin->mInfo->UsesPolicy(reqPolicy, &policyResult);
        if (!policyResult) {
            //throw new SecurityException("Admin " + admin.info.getComponent() + " did not specify uses-policy for: " + admin.info.getTagForPolicy(reqPolicy));
            return E_SECURITY_EXCEPTION;
        }
        *admin = tmpAdmin;
        REFCOUNT_ADD(*admin);
        return NOERROR;
    } else {
        List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
        for (; iter != policy->mAdminList.End(); ++iter) {
            Int32 uid = (*iter)->GetUid();
            Boolean policyResult;
            (*iter)->mInfo->UsesPolicy(reqPolicy, &policyResult);

            if (uid == callingUid && policyResult) {
                *admin = *iter;
                REFCOUNT_ADD(*admin);
                return NOERROR;
            }
        }

        //throw new SecurityException("No active admin owned by uid " + Binder.getCallingUid() + " for policy #" + reqPolicy);
        return E_SECURITY_EXCEPTION;
    }
}

AutoPtr<CDevicePolicyManagerService::DevicePolicyData> CDevicePolicyManagerService::GetUserData(
    /* [in] */ Int32 userHandle)
{
    AutoLock lock(mLock);

    AutoPtr<DevicePolicyData> policy;
    HashMap<Int32, AutoPtr<DevicePolicyData> >::Iterator it = mUserData.Find(userHandle);
    if (it != mUserData.End()) policy = it->mSecond;
    if (policy == NULL) {
        policy = new DevicePolicyData(userHandle);
        mUserData[userHandle] = policy;
        LoadSettingsLocked(policy, userHandle);
    }

    return policy;
}

void CDevicePolicyManagerService::RemoveUserData(
    /* [in] */ Int32 userHandle)
{
    AutoLock lock(mLock);

    if (userHandle == IUserHandle::USER_OWNER) {
        Slogger::W(TAG, "Tried to remove device policy file for user 0! Ignoring.");
        return;
    }

    AutoPtr<DevicePolicyData> policy;

    HashMap<Int32, AutoPtr<DevicePolicyData> >::Iterator it = mUserData.Find(userHandle);
    if (it != mUserData.End()) {
        policy = it->mSecond;
    }

    if (policy != NULL) mUserData.Erase(userHandle);

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
    Slogger::I(TAG, buf.ToString());
}

ECode CDevicePolicyManagerService::LoadSettingsLocked(
    /* [in] */ DevicePolicyData* policy,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<JournaledFile> journal = MakeJournaledFile(userHandle);
    AutoPtr<IFile> file = journal->ChooseForRead();

    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(file, (IFileInputStream**)&stream);
    AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
    FAIL_RETURN(parser->SetInput(stream, String(NULL)));

    Int32 type;
    FAIL_RETURN(parser->Next(&type));
    while (type != IXmlPullParser::END_DOCUMENT && type != IXmlPullParser::START_TAG)
    {}

    String tag;
    FAIL_RETURN(parser->GetName(&tag));
    if (!tag.Equals("policies")) {
        //throw new XmlPullParserException("Settings do not start with policies tag: found " + tag);
        Slogger::E("Settings do not start with policies tag: found %s", tag.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    FAIL_RETURN(parser->Next(&type));
    Int32 outerDepth;
    FAIL_RETURN(parser->GetDepth(&outerDepth));
    FAIL_RETURN(parser->Next(&type));
    Int32 currentDepth;
    FAIL_RETURN(parser->GetDepth(&currentDepth));
    while (type != IXmlPullParser::END_DOCUMENT && (type != IXmlPullParser::END_TAG || currentDepth > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        FAIL_RETURN(parser->GetName(&tag));
        if (tag.Equals("admin")) {
            String name;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &name));

            AutoPtr<IComponentNameHelper> cnHelper;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
            AutoPtr<IComponentName> cn;
            cnHelper->UnflattenFromString(name, (IComponentName**)&cn);
            AutoPtr<IDeviceAdminInfo> dai;
            FindAdmin(cn, userHandle, (IDeviceAdminInfo**)&dai);

            AutoPtr<IActivityInfo> aInfo;
            dai->GetActivityInfo((IActivityInfo**)&aInfo);
            AutoPtr<IApplicationInfo> appInfo;
            aInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 uid;
            appInfo->GetUid(&uid);
            if (DBG && (UserHandle::GetUserId(uid) != userHandle)) {
                StringBuffer buf;
                buf += "findAdmin returned an incorrect uid ";
                buf += uid;
                buf += " for user ";
                buf += userHandle;
                Slogger::W(TAG, buf.ToString());
            }
            if (dai != NULL) {
                AutoPtr<ActiveAdmin> ap = new ActiveAdmin(dai);
                ap->ReadFromXml(parser);

                AutoPtr<IComponentName> comp;
                ap->mInfo->GetComponent((IComponentName**)&comp);
                (policy->mAdminMap)[comp] = ap;
                policy->mAdminList.PushBack(ap);
            }
        } else if (tag.Equals("failed-password-attempts")) {
            String str;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &str));
            policy->mFailedPasswordAttempts = StringUtils::ParseInt32(str);
            XmlUtils::SkipCurrentTag(parser);
        } else if (tag.Equals("password-owner")) {
            String str;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &str));
            policy->mPasswordOwner = StringUtils::ParseInt32(str);
            XmlUtils::SkipCurrentTag(parser);
        } else if (tag.Equals("active-password")) {
            String str;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("quality"), &str));
            policy->mActivePasswordQuality = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(), String("length"), &str));
            policy->mActivePasswordLength = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("uppercase"), &str));
            policy->mActivePasswordUpperCase = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("lowercase"), &str));
            policy->mActivePasswordLowerCase = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("letters"), &str));
            policy->mActivePasswordLetters = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("numeric"), &str));
            policy->mActivePasswordNumeric = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("symbols"), &str));
            policy->mActivePasswordSymbols = StringUtils::ParseInt32(str);

            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("nonletter"), &str));
            policy->mActivePasswordNonLetter = StringUtils::ParseInt32(str);

            XmlUtils::SkipCurrentTag(parser);
        } else {
            StringBuffer buf;
            buf += "Unknown tag: ";
            buf += tag;
            Slogger::W(TAG, buf.ToString());
            XmlUtils::SkipCurrentTag(parser);
        }
    }

    if (stream != NULL) {
        stream->Close();
    }

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
        Slogger::W(TAG, buf.ToString());

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

void CDevicePolicyManagerService::SaveSettingsLocked(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<JournaledFile> journal = MakeJournaledFile(userHandle);

    AutoPtr<IFile> fileFromJournal = journal->ChooseForWrite();
    AutoPtr<IFileOutputStream> stream;
    CFileOutputStream::New(fileFromJournal, FALSE, (IFileOutputStream**)&stream);

    AutoPtr<IFastXmlSerializer> serializer;
    CFastXmlSerializer::New((IFastXmlSerializer**)&serializer);
    AutoPtr<IXmlSerializer> out = IXmlSerializer::Probe(serializer);
    out->SetOutput(stream, String("utf-8"));
    AutoPtr<IBoolean> value;
    CBoolean::New(TRUE, (IBoolean**)&value);
    out->StartDocument(String(NULL), value);
    out->WriteStartTag(String(NULL), String("policies"));

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> ap = *iter;
        if (ap != NULL) {
            out->WriteStartTag(String(NULL), String("admin"));
            AutoPtr<IComponentName> component;
            ap->mInfo->GetComponent((IComponentName**)&component);
            String str;
            component->FlattenToString(&str);
            out->WriteAttribute(String(NULL), String("name"), str);
            ap->WriteToXml(out);
            out->WriteEndTag(String(NULL), String("admin"));
        }
    }

    if (policy->mPasswordOwner >= 0) {
        out->WriteStartTag(String(NULL), String("password-owner"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(policy->mPasswordOwner));
        out->WriteEndTag(String(NULL), String("password-owner"));
    }

    if (policy->mFailedPasswordAttempts != 0) {
        out->WriteStartTag(String(NULL), String("failed-password-attempts"));
        out->WriteAttribute(String(NULL), String("value"), StringUtils::Int32ToString(policy->mFailedPasswordAttempts));
        out->WriteEndTag(String(NULL), String("failed-password-attempts"));
    }

    if (policy->mActivePasswordQuality != 0 || policy->mActivePasswordLength != 0
            || policy->mActivePasswordUpperCase != 0 || policy->mActivePasswordLowerCase != 0
            || policy->mActivePasswordLetters != 0 || policy->mActivePasswordNumeric != 0
            || policy->mActivePasswordSymbols != 0 || policy->mActivePasswordNonLetter != 0)
    {
        out->WriteStartTag(String(NULL), String("active-password"));
        out->WriteAttribute(String(NULL), String("quality"), StringUtils::Int32ToString(policy->mActivePasswordQuality));
        out->WriteAttribute(String(NULL), String("length"), StringUtils::Int32ToString(policy->mActivePasswordLength));
        out->WriteAttribute(String(NULL), String("uppercase"), StringUtils::Int32ToString(policy->mActivePasswordUpperCase));
        out->WriteAttribute(String(NULL), String("lowercase"), StringUtils::Int32ToString(policy->mActivePasswordLowerCase));
        out->WriteAttribute(String(NULL), String("letters"), StringUtils::Int32ToString(policy->mActivePasswordLetters));
        out->WriteAttribute(String(NULL), String("numeric"), StringUtils::Int32ToString(policy->mActivePasswordNumeric));
        out->WriteAttribute(String(NULL), String("symbols"), StringUtils::Int32ToString(policy->mActivePasswordSymbols));
        out->WriteAttribute(String(NULL), String("nonletter"), StringUtils::Int32ToString(policy->mActivePasswordNonLetter));
        out->WriteEndTag(String(NULL), String("active-password"));
    }

    out->WriteEndTag(String(NULL), String("policies"));
    out->EndDocument();
    stream->Close();
    journal->Commit();
    SendChangedNotification(userHandle);

    /*
    try {
        ....
        ....
    } catch (IOException e) {
        try {
            if (stream != null) {
                stream.close();
            }
        } catch (IOException ex) {
            // Ignore
        }
        journal.rollback();
    }
    //*/
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

    AutoPtr<ActiveAdmin> admin;
    HashMap< AutoPtr<IComponentName>, AutoPtr<ActiveAdmin> >::Iterator it = policy->mAdminMap.Find(who);
    if (it != policy->mAdminMap.End()) {
        admin = it->mSecond;
    }

    if (admin != NULL) {
        String pkgName;
        who->GetPackageName(&pkgName);
        String clsName;
        who->GetClassName(&clsName);
        AutoPtr<IActivityInfo> activityInfo;
        admin->mInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
        String infoPkgName;
        activityInfo->GetPackageName(&infoPkgName);
        String infoClsName;
        activityInfo->GetName(&infoClsName);

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
        Slogger::W(TAG, "Failure talking with power manager");
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
        Slogger::W(TAG, buf.ToString());

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
    return GetEncryptionStatus() != IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
}

Int32 CDevicePolicyManagerService::GetEncryptionStatus()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String status;
    sysProp->Get(String("ro.crypto.state"), String("unsupported"), &status);

    if (status.EqualsIgnoreCase("encrypted")) {
        return IDevicePolicyManager::ENCRYPTION_STATUS_ACTIVE;
    } else if (status.EqualsIgnoreCase("unencrypted")) {
        return IDevicePolicyManager::ENCRYPTION_STATUS_INACTIVE;
    } else {
        return IDevicePolicyManager::ENCRYPTION_STATUS_UNSUPPORTED;
    }
}

void CDevicePolicyManagerService::SetEncryptionRequested(
    /* [in] */ Boolean encrypt)
{}

void CDevicePolicyManagerService::ResetGlobalProxyLocked(
    /* [in] */ DevicePolicyData* policy)
{
    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> ap = *iter;
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
    if (who != NULL) {
        AutoPtr<ActiveAdmin> admin = GetActiveAdminUncheckedLocked(who, userHandle);
        return admin != NULL ? admin->mPasswordExpirationDate : 0L;
    }

    Int64 timeout = 0L;
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        if (timeout == 0L || (admin->mPasswordExpirationDate != 0 && timeout > admin->mPasswordExpirationDate)) {
            timeout = admin->mPasswordExpirationDate;
        }
    }

    return timeout;
}

void CDevicePolicyManagerService::WipeDeviceOrUserLocked(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle)
{
    if (userHandle == IUserHandle::USER_OWNER) {
        WipeDataLocked(flags);
    }
    else {
        LockNowUnchecked();

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
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);

    AutoPtr<IComponentName> comp;
    admin->mInfo->GetComponent((IComponentName**)&comp);
    intent->SetComponent(comp);


    if (action.Equals(IDeviceAdminReceiver::ACTION_PASSWORD_EXPIRING)) {
        intent->PutExtra(String("expiration"), admin->mPasswordExpirationDate);
    }

    if (result != NULL) {
        mContext->SendOrderedBroadcastAsUser(intent, admin->GetUserHandle(),
                String(NULL), result, mHandler, IActivity::RESULT_OK, String(NULL), NULL);
    } else {
        mContext->SendBroadcastAsUser(intent, UserHandle::OWNER);
    }
}

void CDevicePolicyManagerService::SendAdminCommandLocked(
    /* [in] */ const String& action,
    /* [in] */ Int32 reqPolicy,
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    if (policy->mAdminList.Begin() != policy->mAdminList.End()) {
        List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
        for (; iter != policy->mAdminList.End(); ++iter) {
            AutoPtr<ActiveAdmin> admin = *iter;
            Boolean result;
            admin->mInfo->UsesPolicy(reqPolicy, &result);
            if (result) {
                SendAdminCommandLocked(admin, action);
            }
        }
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
    powerMgr->GoToSleep(SystemClock::GetUptimeMillis(), IPowerManager::GO_TO_SLEEP_REASON_DEVICE_ADMIN);
    // Ensure the device is locked
    GetWindowManager()->LockNow(NULL);
    Binder::RestoreCallingIdentity(ident);
}

void CDevicePolicyManagerService::HandlePackagesChanged(
    /* [in] */ Int32 userHandle)
{
    Boolean removed = FALSE;

    StringBuffer buf;
    buf += "Handling package changes for user ";
    buf += userHandle;
    Slogger::W(TAG, buf.ToString());

    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);
    AutoPtr<IIPackageManager> pm = AppGlobals::GetPackageManager();

    List< AutoPtr<ActiveAdmin> >::ReverseIterator rit = policy->mAdminList.RBegin();
    while (rit != policy->mAdminList.REnd()) {
        AutoPtr<ActiveAdmin> aa = *rit;

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
            List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Erase((++rit).GetBase());
            rit = (List< AutoPtr<ActiveAdmin> >::ReverseIterator)(iter);
        }
        else {
            ++rit;
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
    List< AutoPtr<ActiveAdmin> >::ReverseIterator riter = policy->mAdminList.RBegin();

    for (; riter != policy->mAdminList.REnd(); ++riter) {
        if ((*riter)->GetUid() == policy->mPasswordOwner) {
            haveOwner = TRUE;
            break;
        }
    }
    if (!haveOwner) {
        StringBuffer buf;
        buf += "Previous password owner ";
        buf += policy->mPasswordOwner;
        buf += " no longer active; disabling";
        Slogger::W(TAG, buf.ToString());

        policy->mPasswordOwner = -1;
    }
}

void CDevicePolicyManagerService::HandlePasswordExpirationNotification(
    /* [in] */ DevicePolicyData* policy)
{
    AutoLock lock(mLock);

    if (policy->mAdminList.Begin() == policy->mAdminList.End()) {
        return;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        Boolean result;
        admin->mInfo->UsesPolicy(IDeviceAdminInfo::USES_POLICY_EXPIRE_PASSWORD, &result);
        system->GetCurrentTimeMillis(&now);
        if (result && admin->mPasswordExpirationTimeout > 0L
            && admin->mPasswordExpirationDate > 0L
            && now >= admin->mPasswordExpirationDate - EXPIRATION_GRACE_PERIOD_MS)
        {
            SendAdminCommandLocked(admin, IDeviceAdminReceiver::ACTION_PASSWORD_EXPIRING);
        }
    }
    SetExpirationAlarmCheckLocked(mContext, policy);
}

void CDevicePolicyManagerService::UpdatePasswordExpirationsLocked(
    /* [in] */ Int32 userHandle)
{
    AutoPtr<DevicePolicyData> policy = GetUserData(userHandle);

    if (policy->mAdminList.Begin() == policy->mAdminList.End()) {
        return;
    }

    List< AutoPtr<ActiveAdmin> >::Iterator iter = policy->mAdminList.Begin();
    for (; iter != policy->mAdminList.End(); ++iter) {
        AutoPtr<ActiveAdmin> admin = *iter;
        Boolean result;
        admin->mInfo->UsesPolicy(IDeviceAdminInfo::USES_POLICY_EXPIRE_PASSWORD, &result);
        if (result) {
            Int64 timeout = admin->mPasswordExpirationTimeout;
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 millis;
            system->GetCurrentTimeMillis(&millis);
            Int64 expiration = timeout > 0L ? (timeout + millis) : 0L;
            admin->mPasswordExpirationDate = expiration;
        }
    }
    SaveSettingsLocked(userHandle);
}

void CDevicePolicyManagerService::WipeDataLocked(
    /* [in] */ Int32 flags)
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
        // TODO:
        // CIntent::New(IExternalStorageFormatter::FORMAT_AND_FACTORY_RESET, (IIntent**)&intent);
        // intent->PutBooleanExtra(IExternalStorageFormatter::EXTRA_ALWAYS_RESET, TRUE);
        // intent.setComponent(CExternalStorageFormatter::COMPONENT_NAME);
        mWakeLock->AcquireLock(10000);

        AutoPtr<IComponentName> tmpComp;
        mContext->StartService(intent, (IComponentName**)&tmpComp);
    } else {
        ECode ec;
        // TODO:
        // RecoverySystem.rebootWipeUserData(mContext);
        if (FAILED(ec)) {
            Slogger::W(TAG, "Failed requesting data wipe");
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

ECode CDevicePolicyManagerService::ValidateQualityConstant(
    /* [in] */ Int32 quality)
{
    switch (quality) {
        case IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED:
        case IDevicePolicyManager::PASSWORD_QUALITY_BIOMETRIC_WEAK:
        case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
        case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
        case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
            return NOERROR;
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

AutoPtr<JournaledFile> CDevicePolicyManagerService::MakeJournaledFile(
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
    AutoPtr<JournaledFile> journaledFile = new JournaledFile(bFile, tFile);
    return journaledFile;
}

void CDevicePolicyManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
//     if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
//             != PackageManager.PERMISSION_GRANTED) {

//         pw.println("Permission Denial: can't dump DevicePolicyManagerService from from pid="
//                 + Binder.getCallingPid()
//                 + ", uid=" + Binder.getCallingUid());
//         return;
//     }

//     final Printer p = new PrintWriterPrinter(pw);

//     synchronized(this) {
//         p.println("Current Device Policy Manager state:");

//         int userCount = mUserData.size();
//         for (int u = 0; u < userCount; u++) {
//             DevicePolicyData policy = getUserData(mUserData.keyAt(u));
//             p.println("  Enabled Device Admins (User " + policy.mUserHandle + "):");
//             final int N = policy.mAdminList.size();
//             for (int i=0; i<N; i++) {
//                 ActiveAdmin ap = policy.mAdminList.get(i);
//                 if (ap != null) {
//                     pw.print("  "); pw.print(ap.info.getComponent().flattenToShortString());
//                             pw.println(":");
//                     ap.dump("    ", pw);
//                 }
//             }

//             pw.println(" ");
//             pw.print("  mPasswordOwner="); pw.println(policy.mPasswordOwner);
//         }
//     }
}

} // Server
} // Droid
} // Elastos
