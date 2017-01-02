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

#ifndef __ELASTOS_DROID_SERVER_DEVICEPOLICY_CDEVICEPOLICYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_DEVICEPOLICY_CDEVICEPOLICYMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_DevicePolicy_CDevicePolicyManagerService.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/devicepolicy/DeviceOwner.h"
#include <_Elastos.Droid.Core.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/AsyncTask.h>

// using Elastos::Droid::Provider::Settings;
using Elastos::Droid::App::Admin::IDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::Admin::IDevicePolicyManagerInternal;
using Elastos::Droid::App::Admin::IDevicePolicyManagerInternalOnCrossProfileWidgetProvidersChangeListener;
using Elastos::Droid::App::Admin::IIDevicePolicyManager;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::Utility::IJournaledFile;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using namespace Elastos::Utility::Logging;

namespace Elastos {
namespace Droid {
namespace Server {
namespace DevicePolicy {

/**
 *Public interface for managing policies enforced on a device. Most clients of this
 * class must have published a DeviceAdminReceiver that the user has currently enabled.
 *
 * @sa DeviceAdminReceiver
 * <p>
 * <strong>Developer Guides</strong>
 * <p>
 * For more information about managing policies for device adminstration, read the Device Administration developer guide.
 *
 * @sa Administration
 */
CarClass(CDevicePolicyManagerService)
    , public Object
    , public IBinder
    , public IIDevicePolicyManager
{
public:
    class ActiveAdmin;

    class RemoveWarningReceiver
        : public BroadcastReceiver
    {
    public:
        RemoveWarningReceiver(
            /* [in] */ IRemoteCallback* result);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        AutoPtr<IRemoteCallback> mResult;
    };

    class ActiveAdminLockedReceiver
        : public BroadcastReceiver
    {
    public:
        ActiveAdminLockedReceiver(
            /* [in] */ CDevicePolicyManagerService* host,
            /* [in] */ ActiveAdmin* admin,
            /* [in] */ IComponentName* adminReceiver);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        CDevicePolicyManagerService* mHost;
        AutoPtr<ActiveAdmin> mAdmin;
        AutoPtr<IComponentName> mAdminReceiver;
    };

    class DevicePolicyReceiver
        : public BroadcastReceiver
    {
    public:
        DevicePolicyReceiver(
            /* [in] */ CDevicePolicyManagerService* host);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        CDevicePolicyManagerService* mHost;
    };

    class DevicePolicyReceiverRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        DevicePolicyReceiverRunnable(
            /* [in] */ CDevicePolicyManagerService* host,
            /* [in] */ Int32 userHandle);

        CARAPI Run();

    private:
        CDevicePolicyManagerService* mHost;
        Int32 mUserHandle;
    };

    class DeviceOrUserLockedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        DeviceOrUserLockedRunnable(
            /* [in] */ CDevicePolicyManagerService* host,
            /* [in] */ Int32 userHandle);

        CARAPI Run();

    private:
        CDevicePolicyManagerService* mHost;
        Int32 mUserHandle;
    };

    class DevicePolicyData
        : public Object
    {
    public:
        DevicePolicyData(
            /* [in] */ Int32 userHandle);

    public:
        Int32 mActivePasswordQuality;
        Int32 mActivePasswordLength;
        Int32 mActivePasswordUpperCase;
        Int32 mActivePasswordLowerCase;
        Int32 mActivePasswordLetters;
        Int32 mActivePasswordNumeric;
        Int32 mActivePasswordSymbols;
        Int32 mActivePasswordNonLetter;
        Int32 mFailedPasswordAttempts;

        Int32 mUserHandle;;
        Int32 mPasswordOwner;
        Int64 mLastMaximumTimeToLock;
        Boolean mUserSetupComplete;

        AutoPtr<IHashMap> mAdminMap;
        AutoPtr<IList> mAdminList;

        // This is the list of component allowed to start lock task mode.
        AutoPtr<IList> mLockTaskPackages;

        AutoPtr<IComponentName> mRestrictionsProvider;
    };

    class ActiveAdmin
        : public Object
    {
        friend class CDevicePolicyManagerService;
    public:
        ActiveAdmin(
            /* [in] */ IDeviceAdminInfo* info);

        CARAPI_(Int32) GetUid();

        CARAPI_(AutoPtr<IUserHandle>) GetUserHandle();

        CARAPI_(void) WriteToXml(
            /* [in] */ IXmlSerializer* out);

        CARAPI_(void) ReadFromXml(
            /* [in] */ IXmlPullParser* parser);

        CARAPI WritePackageListToXml(
            /* [in] */ IXmlSerializer* out,
            /* [in] */ const String& outerTag,
            /* [in] */ IList* packageList);

        CARAPI_(void) Dump(
            /* [in] */ const String& prefix,
            /* [in] */ IPrintWriter* pw);

    private:
        CARAPI ReadPackageList(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ const String& tag,
            /* [out] */ IList** result);

        CARAPI ReadDisableAccountInfo(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ const String& tag,
            /* [out] */ ISet** result);

        CARAPI GetAllTrustAgentFeatures(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ const String& tag,
            /* [out] */ IHashMap** result);

        CARAPI GetTrustAgentFeatures(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ const String& tag,
            /* [out] */ IList** result);

        CARAPI GetCrossProfileWidgetProviders(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ const String& tag,
            /* [out] */ IList** result);

    public:
        AutoPtr<IDeviceAdminInfo> mInfo;

        Int32 mPasswordQuality;

        static const Int32 DEF_MINIMUM_PASSWORD_LENGTH;
        Int32 mMinimumPasswordLength;

        static const Int32 DEF_PASSWORD_HISTORY_LENGTH;
        Int32 mPasswordHistoryLength;

        static const Int32 DEF_MINIMUM_PASSWORD_UPPER_CASE;
        Int32 mMinimumPasswordUpperCase;

        static const Int32 DEF_MINIMUM_PASSWORD_LOWER_CASE;
        Int32 mMinimumPasswordLowerCase;

        static const Int32 DEF_MINIMUM_PASSWORD_LETTERS;
        Int32 mMinimumPasswordLetters;

        static const Int32 DEF_MINIMUM_PASSWORD_NUMERIC;
        Int32 mMinimumPasswordNumeric;

        static const Int32 DEF_MINIMUM_PASSWORD_SYMBOLS;
        Int32 mMinimumPasswordSymbols;

        static const Int32 DEF_MINIMUM_PASSWORD_NON_LETTER;
        Int32 mMinimumPasswordNonLetter;

        static const Int64 DEF_MAXIMUM_TIME_TO_UNLOCK;
        Int64 mMaximumTimeToUnlock;

        static const Int32 DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE;
        Int32 mMaximumFailedPasswordsForWipe;

        static const Int64 DEF_PASSWORD_EXPIRATION_TIMEOUT;
        Int64 mPasswordExpirationTimeout;

        static const Int64 DEF_PASSWORD_EXPIRATION_DATE;
        Int64 mPasswordExpirationDate;

        static const Int32 DEF_KEYGUARD_FEATURES_DISABLED; // none
        Int32 mDisabledKeyguardFeatures;

        Boolean mEncryptionRequested;
        Boolean mDisableCamera;
        Boolean mDisableCallerId;
        Boolean mDisableScreenCapture; // Can only be set by a device/profile owner.
        Boolean mRequireAutoTime; // Can only be set by a device owner.

        AutoPtr<ISet> mAccountTypesWithManagementDisabled;

        // The list of permitted accessibility services package namesas set by a profile
        // or device owner. Null means all accessibility services are allowed, empty means
        // none except system services are allowed.
        AutoPtr<IList> mPermittedAccessiblityServices;

        // The list of permitted input methods package names as set by a profile or device owner.
        // Null means all input methods are allowed, empty means none except system imes are
        // allowed.
        AutoPtr<IList> mPermittedInputMethods;

        // TODO: review implementation decisions with frameworks team
        Boolean mSpecifiesGlobalProxy;
        String mGlobalProxySpec;
        String mGlobalProxyExclusionList;
        AutoPtr<IHashMap> mTrustAgentFeatures;

        AutoPtr<IList> mCrossProfileWidgetProviders;

    private:
        static const String TAG_DISABLE_KEYGUARD_FEATURES;
        static const String TAG_DISABLE_CAMERA;
        static const String TAG_DISABLE_CALLER_ID;
        static const String TAG_DISABLE_SCREEN_CAPTURE;
        static const String TAG_DISABLE_ACCOUNT_MANAGEMENT;
        static const String TAG_REQUIRE_AUTO_TIME;
        static const String TAG_ACCOUNT_TYPE;
        static const String TAG_PERMITTED_ACCESSIBILITY_SERVICES;
        static const String TAG_ENCRYPTION_REQUESTED;
        static const String TAG_MANAGE_TRUST_AGENT_FEATURES;
        static const String TAG_TRUST_AGENT_FEATURE;
        static const String TAG_TRUST_AGENT_COMPONENT;
        static const String TAG_PASSWORD_EXPIRATION_DATE;
        static const String TAG_PASSWORD_EXPIRATION_TIMEOUT;
        static const String TAG_GLOBAL_PROXY_EXCLUSION_LIST;
        static const String TAG_GLOBAL_PROXY_SPEC;
        static const String TAG_SPECIFIES_GLOBAL_PROXY;
        static const String TAG_PERMITTED_IMES;
        static const String TAG_MAX_FAILED_PASSWORD_WIPE;
        static const String TAG_MAX_TIME_TO_UNLOCK;
        static const String TAG_MIN_PASSWORD_NONLETTER;
        static const String TAG_MIN_PASSWORD_SYMBOLS;
        static const String TAG_MIN_PASSWORD_NUMERIC;
        static const String TAG_MIN_PASSWORD_LETTERS;
        static const String TAG_MIN_PASSWORD_LOWERCASE;
        static const String TAG_MIN_PASSWORD_UPPERCASE;
        static const String TAG_PASSWORD_HISTORY_LENGTH;
        static const String TAG_MIN_PASSWORD_LENGTH;
        static const String ATTR_VALUE;
        static const String TAG_PASSWORD_QUALITY;
        static const String TAG_POLICIES;
        static const String TAG_CROSS_PROFILE_WIDGET_PROVIDERS;
        static const String TAG_PROVIDER;
        static const String TAG_PACKAGE_LIST_ITEM;
    };

    class Lifecycle
        : public SystemService
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context);

        // @Override
        CARAPI OnStart();

        // @Override
        CARAPI OnBootPhase(
            /* [in] */ Int32 phase);

    private:
        AutoPtr<CDevicePolicyManagerService> mService;
    };

    class SetupContentObserver
        : public ContentObserver
    {
    public:
        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ IIDevicePolicyManager* host);

        CARAPI Register(
            /* [in] */ IContentResolver* resolver);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IUri> mUserSetupComplete;
        CDevicePolicyManagerService* mHost;
    };

private:
    class MonitoringCertNotificationTask
        : public AsyncTask
    {
    public:
        MonitoringCertNotificationTask(
            /* [in] */ CDevicePolicyManagerService* host);

        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        CARAPI ManageNotification(
            /* [in] */ IUserHandle* userHandle);

    private:
        CDevicePolicyManagerService* mHost;
    };

    class LocalService
        : public Object
        , public IDevicePolicyManagerInternal
    {
        friend class CDevicePolicyManagerService;
    public:
        CAR_INTERFACE_DECL()

        LocalService(
            /* [in] */ CDevicePolicyManagerService* host);

        // @Override
        CARAPI GetCrossProfileWidgetProviders(
            /* [in] */ Int32 profileId,
            /* [out] */ IList** result);

        // @Override
        CARAPI AddOnCrossProfileWidgetProvidersChangeListener(
            /* [in] */ IDevicePolicyManagerInternalOnCrossProfileWidgetProvidersChangeListener* listener);

    private:
        CARAPI NotifyCrossProfileProvidersChanged(
            /* [in] */ Int32 userId,
            /* [in] */ IList* packages);

    private:
        AutoPtr<IList> mWidgetProviderListeners;
        CDevicePolicyManagerService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDevicePolicyManagerService();

    ~CDevicePolicyManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates and loads the policy data from xml.
     * @param userHandle the user for whom to load the policy data
     * @return
     */
    CARAPI_(AutoPtr<DevicePolicyData>) GetUserData(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) RemoveUserData(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) LoadDeviceOwner();

    CARAPI_(AutoPtr<ActiveAdmin>) GetActiveAdminUncheckedLocked(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle);

    CARAPI GetActiveAdminForCallerLocked(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 reqPolicy,
        /* [out] */ ActiveAdmin** admin);

    CARAPI_(void) SendAdminCommandLocked(
        /* [in] */ ActiveAdmin* admin,
        /* [in] */ const String& action);

    CARAPI_(void) SendAdminCommandLocked(
        /* [in] */ ActiveAdmin* admin,
        /* [in] */ const String& action,
        /* [in] */ IBroadcastReceiver* result);

    /**
     * Send an update to one specific admin, get notified when that admin returns a result.
     */
    CARAPI_(void) SendAdminCommandLocked(
        /* [in] */ ActiveAdmin* admin,
        /* [in] */ const String& action,
        /* [in] */ IBundle* adminExtras,
        /* [in] */ IBroadcastReceiver* result);

    /**
     * Send an update to all admins of a user that enforce a specified policy.
     */
    CARAPI_(void) SendAdminCommandLocked(
        /* [in] */ const String& action,
        /* [in] */ Int32 reqPolicy,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) RemoveActiveAdminLocked(
        /* [in] */ IComponentName* adminReceiver,
        /* [in] */ Int32 userHandle);

    CARAPI FindAdmin(
        /* [in] */ IComponentName* adminName,
        /* [in] */ Int32 userHandle,
        /* [out] */ IDeviceAdminInfo** adminInfo);

    static CARAPI ValidateQualityConstant(
        /* [in] */ Int32 quality);

    CARAPI_(void) ValidatePasswordOwnerLocked(
        /* [in] */ DevicePolicyData* policy);

    /**
     * Pushes down policy information to the system for any policies related to general device
     * capabilities that need to be enforced by lower level services (e.g. Camera services).
     */
    CARAPI_(void) SyncDeviceCapabilitiesLocked(
        /* [in] */ DevicePolicyData* policy);

    CARAPI SystemReady();

    /**
     * @param adminReceiver The admin to add
     * @param refreshing true = update an active admin, no error
     */
    CARAPI SetActiveAdmin(
        /* [in] */ IComponentName* adminReceiver,
        /* [in] */ Boolean refreshing,
        /* [in] */ Int32 userHandle);

    CARAPI IsAdminActive(
        /* [in] */ IComponentName* adminReceiver,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* isActive);

    CARAPI HasGrantedPolicy(
        /* [in] */ IComponentName* adminReceiver,
        /* [in] */ Int32 policyId,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* hasGrant);

    // @SuppressWarnings("unchecked")
    CARAPI GetActiveAdmins(
        /* [in] */ Int32 userHandle,
        /* [out] */ IList** admins);

    CARAPI PackageHasActiveAdmins(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* hasActive);

    CARAPI RemoveActiveAdmin(
        /* [in] */ IComponentName* adminReceiver,
        /* [in] */ Int32 userHandle);

    CARAPI SetPasswordQuality(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 quality,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordQuality(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordMinimumLength(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumLength(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordHistoryLength(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordHistoryLength(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordExpirationTimeout(
        /* [in] */ IComponentName* who,
        /* [in] */ Int64 timeout,
        /* [in] */ Int32 userHandle);

    /**
     * Return a single admin's expiration cycle time, or the min of all cycle times.
     * Returns 0 if not configured.
     */
    CARAPI GetPasswordExpirationTimeout(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* password);

    // @Override
    CARAPI AddCrossProfileWidgetProvider(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RemoveCrossProfileWidgetProvider(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetCrossProfileWidgetProviders(
        /* [in] */ IComponentName* admin,
        /* [out] */ IList** result);

    CARAPI GetPasswordExpiration(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* password);

    CARAPI SetPasswordMinimumUpperCase(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumUpperCase(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordMinimumLowerCase(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumLowerCase(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordMinimumLetters(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumLetters(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordMinimumNumeric(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumNumeric(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordMinimumSymbols(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumSymbols(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetPasswordMinimumNonLetter(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 length,
        /* [in] */ Int32 userHandle);

    CARAPI GetPasswordMinimumNonLetter(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI IsActivePasswordSufficient(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI GetCurrentFailedPasswordAttempts(
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI SetMaximumFailedPasswordsForWipe(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 num,
        /* [in] */ Int32 userHandle);

    CARAPI GetMaximumFailedPasswordsForWipe(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* password);

    CARAPI ResetPassword(
        /* [in] */ const String& password,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI SetMaximumTimeToLock(
        /* [in] */ IComponentName* who,
        /* [in] */ Int64 timeMs,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) UpdateMaximumTimeToLockLocked(
        /* [in] */ DevicePolicyData* policy);

    CARAPI GetMaximumTimeToLock(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int64* time);

    CARAPI LockNow();

    // @Override
    CARAPI EnforceCanManageCaCerts(
        /* [in] */ IComponentName* who);

    // @Override
    CARAPI InstallCaCert(
        /* [in] */ IComponentName* admin,
        /* [in] */ ArrayOf<Byte>* certBuffer,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI UninstallCaCert(
        /* [in] */ IComponentName* admin,
        /* [in] */ const String& alias);

    // @Override
    CARAPI InstallKeyPair(
        /* [in] */ IComponentName* who,
        /* [in] */ ArrayOf<Byte>* privKey,
        /* [in] */ ArrayOf<Byte>* cert,
        /* [in] */ const String& alias,
        /* [out] */ Boolean* result);

    CARAPI_(void) WipeDataLocked(
        /* [in] */ Int32 flags,
        /* [in] */ const String& reason);

    // @Override
    CARAPI WipeData(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle);

    CARAPI GetRemoveWarning(
        /* [in] */ IComponentName* comp,
        /* [in] */ IRemoteCallback* result,
        /* [in] */ Int32 userHandle);

    CARAPI SetActivePasswordState(
        /* [in] */ Int32 quality,
        /* [in] */ Int32 length,
        /* [in] */ Int32 letters,
        /* [in] */ Int32 uppercase,
        /* [in] */ Int32 lowercase,
        /* [in] */ Int32 numbers,
        /* [in] */ Int32 symbols,
        /* [in] */ Int32 nonletter,
        /* [in] */ Int32 userHandle);

    CARAPI ReportFailedPasswordAttempt(
        /* [in] */ Int32 userHandle);

    CARAPI ReportSuccessfulPasswordAttempt(
        /* [in] */ Int32 userHandle);

    CARAPI SetGlobalProxy(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& proxySpec,
        /* [in] */ const String& exclusionList,
        /* [in] */ Int32 userHandle,
        /* [out] */ IComponentName** component);

    CARAPI GetGlobalProxyAdmin(
        /* [in] */ Int32 userHandle,
        /* [out] */ IComponentName** component);

    CARAPI SetRecommendedGlobalProxy(
        /* [in] */ IComponentName* who,
        /* [in] */ IProxyInfo* proxyInfo);

    /**
     * Set the storage encryption request for a single admin.  Returns the new total request
     * status (for all admins).
     */
    CARAPI SetStorageEncryption(
        /* [in] */ IComponentName* who,
        /* [in] */ Boolean encrypt,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* value);

    /**
     * Get the current storage encryption request status for a given admin, or aggregate of all
     * active admins.
     */
    CARAPI GetStorageEncryption(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Get the current encryption status of the device.
     */
    CARAPI GetStorageEncryptionStatus(
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* st);

    /**
     * Set whether the screen capture is disabled for the user managed by the specified admin.
     */
    CARAPI SetScreenCaptureDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [in] */ Boolean disabled);

    /**
     * Returns whether or not screen capture is disabled for a given admin, or disabled for any
     * active admin (if given admin is null).
     */
    CARAPI GetScreenCaptureDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Set whether auto time is required by the specified admin (must be device owner).
     */
    CARAPI SetAutoTimeRequired(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [in] */ Boolean required);

    /**
     * Returns whether or not auto time is required by the device owner.
     */
    ECode GetAutoTimeRequired(
        /* [out] */ Boolean* result);

    /**
     * Disables all device cameras according to the specified admin.
     */
    CARAPI SetCameraDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ Boolean disabled,
        /* [in] */ Int32 userHandle);

    /**
     * Gets whether or not all device cameras are disabled for a given admin, or disabled for any
     * active admins.
     */
    CARAPI GetCameraDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Selectively disable keyguard features.
     */
    CARAPI SetKeyguardDisabledFeatures(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 which,
        /* [in] */ Int32 userHandle);

    /**
     * Gets the disabled state for features in keyguard for the given admin,
     * or the aggregate of all active admins if who is null.
     */
    CARAPI GetKeyguardDisabledFeatures(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle,
        /* [out] */ Int32* features);

    // @Override
    CARAPI SetDeviceOwner(
        /* [in] */ const String& packageName,
        /* [in] */ const String& ownerName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsDeviceOwner(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetDeviceOwner(
        /* [out] */ String* result);

    // @Override
    CARAPI GetDeviceOwnerName(
        /* [out] */ String* result);

    // @Override
    CARAPI RequireSecureKeyguard(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearDeviceOwner(
        /* [in] */ const String& packageName);

    // @Override
    CARAPI SetProfileOwner(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& ownerName,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ClearProfileOwner(
        /* [in] */ IComponentName* who);

    // @Override
    CARAPI HasUserSetupCompleted(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetProfileEnabled(
        /* [in] */ IComponentName* who);

    // @Override
    CARAPI SetProfileName(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& profileName);

    // @Override
    CARAPI GetProfileOwner(
        /* [in] */ Int32 userHandle,
        /* [out] */ IComponentName** result);

    // @Override
    CARAPI GetProfileOwnerName(
        /* [in] */ Int32 userHandle,
        /* [out] */ String* result);

    // @Override
    CARAPI AddPersistentPreferredActivity(
        /* [in] */ IComponentName* who,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ IComponentName* activity);

    // @Override
    CARAPI ClearPackagePersistentPreferredActivities(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName);

    // @Override
    CARAPI SetApplicationRestrictions(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName,
        /* [in] */ IBundle* settings);

    CARAPI SetTrustAgentFeaturesEnabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ IComponentName* agent,
        /* [in] */ IList* features,
        /* [in] */ Int32 userHandle);

    CARAPI GetTrustAgentFeaturesEnabled(
        /* [in] */ IComponentName* admin,
        /* [in] */ IComponentName* agent,
        /* [in] */ Int32 userHandle,
        /* [out] */ IList** result);

    // @Override
    CARAPI SetRestrictionsProvider(
        /* [in] */ IComponentName* who,
        /* [in] */ IComponentName* permissionProvider);

    // @Override
    CARAPI GetRestrictionsProvider(
        /* [in] */ Int32 userHandle,
        /* [out] */ IComponentName** result);

    CARAPI AddCrossProfileIntentFilter(
        /* [in] */ IComponentName* who,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ Int32 flags);

    CARAPI ClearCrossProfileIntentFilters(
        /* [in] */ IComponentName* who);

    // @Override
    CARAPI SetPermittedAccessibilityServices(
        /* [in] */ IComponentName* who,
        /* [in] */ IList* packageList,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetPermittedAccessibilityServices(
        /* [in] */ IComponentName* who,
        /* [out] */ IList** result);

    // @Override
    CARAPI GetPermittedAccessibilityServicesForUser(
        /* [in] */ Int32 userId,
        /* [out] */ IList** result);

    // @Override
    CARAPI SetPermittedInputMethods(
        /* [in] */ IComponentName* who,
        /* [in] */ IList* packageList,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetPermittedInputMethods(
        /* [in] */ IComponentName* who,
        /* [out] */ IList** result);

    // @Override
    CARAPI GetPermittedInputMethodsForCurrentUser(
        /* [out] */ IList** result);

    // @Override
    CARAPI CreateUser(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& name,
        /* [out] */ IUserHandle** result);

    // @Override
    CARAPI CreateAndInitializeUser(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& name,
        /* [in] */ const String& ownerName,
        /* [in] */ IComponentName* profileOwnerComponent,
        /* [in] */ IBundle* adminExtras,
        /* [out] */ IUserHandle** result);

    // @Override
    CARAPI RemoveUser(
        /* [in] */ IComponentName* who,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SwitchUser(
        /* [in] */ IComponentName* who,
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetApplicationRestrictions(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName,
        /* [out] */ IBundle** result);

    // @Override
    CARAPI SetUserRestriction(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& key,
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI SetApplicationHidden(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean hidden,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsApplicationHidden(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI EnableSystemApp(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName);

    // @Override
    CARAPI EnableSystemAppWithIntent(
        /* [in] */ IComponentName* who,
        /* [in] */ IIntent* intent,
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetAccountManagementDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& accountType,
        /* [in] */ Boolean disabled);

    // @Override
    CARAPI GetAccountTypesWithManagementDisabled(
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetAccountTypesWithManagementDisabledAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<String>** result);

    // @Override
    CARAPI SetUninstallBlocked(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean uninstallBlocked);

    // @Override
    CARAPI IsUninstallBlocked(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetCrossProfileCallerIdDisabled(
        /* [in] */ IComponentName* who,
        /* [in] */ Boolean disabled);

    // @Override
    CARAPI GetCrossProfileCallerIdDisabled(
        /* [in] */ IComponentName* who,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetCrossProfileCallerIdDisabledForUser(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    /**
     * Sets which packages may enter lock task mode.
     *
     * This function can only be called by the device owner.
     * @param components The list of components allowed to enter lock task mode.
     */
    CARAPI SetLockTaskPackages(
        /* [in] */ IComponentName* who,
        /* [in] */ ArrayOf<String>* packages);

    /**
     * This function returns the list of components allowed to start the task lock mode.
     */
    CARAPI GetLockTaskPackages(
        /* [in] */ IComponentName* who,
        /* [out, callee] */ ArrayOf<String>** result);

    /**
     * This function lets the caller know whether the given package is allowed to start the
     * lock task mode.
     * @param pkg The package to check
     */
    CARAPI IsLockTaskPermitted(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI NotifyLockTaskModeChanged(
        /* [in] */ Boolean isEnabled,
        /* [in] */ const String& pkg,
        /* [in] */ Int32 userHandle);

    // @Override
    CARAPI SetGlobalSetting(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& setting,
        /* [in] */ const String& value);

    // @Override
    CARAPI SetSecureSetting(
        /* [in] */ IComponentName* who,
        /* [in] */ const String& setting,
        /* [in] */ const String& value);

    // @Override
    CARAPI SetMasterVolumeMuted(
        /* [in] */ IComponentName* who,
        /* [in] */ Boolean on);

    // @Override
    CARAPI IsMasterVolumeMuted(
        /* [in] */ IComponentName* who,
        /* [out] */ Boolean* result);

    /**
     * We need to update the internal state of whether a user has completed setup once. After
     * that, we ignore any changes that reset the Settings.Secure.USER_SETUP_COMPLETE changes
     * as we don't trust any apps that might try to reset it.
     * <p>
     * Unfortunately, we don't know which user's setup state was changed, so we write all of
     * them.
     */
    CARAPI UpdateUserSetupComplete();

    CARAPI ToString(
        /* [in] */ String* result);

protected:
    /**
     * Set an alarm for an upcoming event - expiration warning, expiration, or post-expiration
     * reminders.  Clears alarm if no expirations are configured.
     */
    CARAPI_(void) SetExpirationAlarmCheckLocked(
        /* [in] */ IContext* context,
        /* [in] */ DevicePolicyData* policy);

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) HandlePackagesChanged(
        /* [in] */ Int32 userHandle);

    CARAPI_(AutoPtr<IIPowerManager>) GetIPowerManager();

    CARAPI_(AutoPtr<IIWindowManager>) GetWindowManager();

    CARAPI_(AutoPtr<INotificationManager>) GetNotificationManager();

    /**
     * Send an update intent to all admins of a user and its profiles. Only send to admins that
     * enforce a specified policy.
     */
    CARAPI_(void) SendAdminCommandToSelfAndProfilesLocked(
        /* [in] */ const String& action,
        /* [in] */ Int32 reqPolicy,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) CleanUpOldUsers();

    CARAPI SetActiveAdmin(
        /* [in] */ IComponentName* adminReceiver,
        /* [in] */ Boolean refreshing,
        /* [in] */ Int32 userHandle,
        /* [in] */ IBundle* onEnableData);

    static CARAPI_(AutoPtr<IJournaledFile>) MakeJournaledFile(
        /* [in] */ Int32 userHandle);

    CARAPI SaveSettingsLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) SendChangedNotification(
        /* [in] */ Int32 userHandle);

    CARAPI LoadSettingsLocked(
        /* [in] */ DevicePolicyData* policy,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) HandlePasswordExpirationNotification(
        /* [in] */ Int32 userHandle);

    /**
     * Return a single admin's expiration date/time, or the min (soonest) for all admins.
     * Returns 0 if not configured.
     */
    CARAPI_(Int64) GetPasswordExpirationLocked(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle);

    /**
     * Returns the admin with the strictest policy on maximum failed passwords for this user and all
     * profiles that are visible from this user. If the policy for the primary and any other profile
     * are equal, it returns the admin for the primary profile.
     * Returns {@code null} if none of them have that policy set.
     */
    CARAPI_(AutoPtr<ActiveAdmin>) GetAdminWithMinimumFailedPasswordsForWipeLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) LockNowUnchecked();

    CARAPI_(Boolean) IsExtStorageEncrypted();

    static CARAPI ParseCert(
        /* [in] */ ArrayOf<Byte>* certBuffer,
        /* [out] */ IX509Certificate** result);

    CARAPI_(void) WipeDeviceOrUserLocked(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle,
        /* [in] */ const String& reason);

    /**
     * Called any time the device password is updated. Resets all password expiration clocks.
     */
    CARAPI_(void) UpdatePasswordExpirationsLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) ResetGlobalProxyLocked(
        /* [in] */ DevicePolicyData* policy);

    CARAPI_(void) SaveGlobalProxyLocked(
        /* [in] */ const String& proxySpec,
        /* [in] */ const String& exclusionList);

    /**
     * Hook to low-levels:  This should report if the filesystem supports encrypted storage.
     */
    CARAPI_(Boolean) IsEncryptionSupported();

    /**
     * Hook to low-levels:  Reporting the current status of encryption.
     * @return A value such as {@link DevicePolicyManager#ENCRYPTION_STATUS_UNSUPPORTED} or
     * {@link DevicePolicyManager#ENCRYPTION_STATUS_INACTIVE} or
     * {@link DevicePolicyManager#ENCRYPTION_STATUS_ACTIVE}.
     */
    CARAPI GetEncryptionStatus(
        /* [out] */ Int32* result);

    /**
     * Hook to low-levels:  If needed, record the new admin setting for encryption.
     */
    CARAPI_(void) SetEncryptionRequested(
        /* [in] */ Boolean encrypt);

    CARAPI UpdateScreenCaptureDisabledInWindowManager(
        /* [in] */ Int32 userHandle,
        /* [in] */ Boolean disabled);

    // Returns the active device owner or null if there is no device owner.
    CARAPI_(AutoPtr<ActiveAdmin>) GetDeviceOwnerAdmin();

    CARAPI_(void) ClearUserRestrictions(
        /* [in] */ IUserHandle* userHandle);

    CARAPI_(Boolean) HasUserSetupCompleted(
        /* [in] */ Int32 userHandle);

    // Returns the active profile owner for this user or null if the current user has no
    // profile owner.
    CARAPI_(AutoPtr<ActiveAdmin>) GetProfileOwnerAdmin(
        /* [in] */ Int32 userHandle);

    /**
     * Device owner can only be set on an unprovisioned device, unless it was initiated by "adb", in
     * which case we allow it if no account is associated with the device.
     */
    CARAPI_(Boolean) AllowedToSetDeviceOwnerOnDevice();

    CARAPI EnforceSystemProcess(
        /* [in] */ const String& message);

    CARAPI EnforceNotManagedProfile(
        /* [in] */ Int32 userHandle,
        /* [in] */ const String& message);

    CARAPI_(AutoPtr<IUserInfo>) GetProfileParent(
        /* [in] */ Int32 userHandle);

    CARAPI_(Boolean) IsManagedProfile(
        /* [in] */ Int32 userHandle);

    CARAPI EnableIfNecessary(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI EnforceCrossUserPermission(
        /* [in] */ Int32 userHandle);

    /**
     * @return true if all packages in enabledPackages are either in the list
     * permittedList or are a system app.
     */
    CARAPI CheckPackagesInPermittedListOrSystem(
        /* [in] */ IList* enabledPackages,
        /* [in] */ IList* permittedList,
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr<IAccessibilityManager>) GetAccessibilityManagerForUser(
        /* [in] */ Int32 userId);

    CARAPI CheckCallerIsCurrentUserOrProfile(
        /* [out] */ Boolean* result);

    CARAPI IsSystemApp(
        /* [in] */ IIPackageManager* pm,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<ISet>) InitDEVICE_OWNER_USER_RESTRICTIONS();

    static CARAPI_(AutoPtr<ISet>) InitSECURE_SETTINGS_WHITELIST();
    static CARAPI_(AutoPtr<ISet>) InitSECURE_SETTINGS_DEVICEOWNER_WHITELIST();
    static CARAPI_(AutoPtr<ISet>) InitGLOBAL_SETTINGS_WHITELIST();

public:
    static const String ACTION_EXPIRED_PASSWORD_NOTIFICATION;

    /**
     * The system property used to share the state of the camera. The native camera service
     * is expected to read this property and act accordingly.
     */
    static const String SYSTEM_PROP_DISABLE_CAMERA;

private:
    static const String DEVICE_POLICIES_XML;

    static const String LOCK_TASK_COMPONENTS_XML;

    static const Int32 REQUEST_EXPIRE_PASSWORD;

    static const Int64 MS_PER_DAY;

    static const Int64 EXPIRATION_GRACE_PERIOD_MS;

    static const Int32 MONITORING_CERT_NOTIFICATION_ID;

    static const Boolean DBG;

    static const String ATTR_PERMISSION_PROVIDER;
    static const String ATTR_SETUP_COMPLETE;

    static const AutoPtr<ISet> DEVICE_OWNER_USER_RESTRICTIONS;

    static const AutoPtr<ISet> SECURE_SETTINGS_WHITELIST;
    static const AutoPtr<ISet> SECURE_SETTINGS_DEVICEOWNER_WHITELIST;
    static const AutoPtr<ISet> GLOBAL_SETTINGS_WHITELIST;

    AutoPtr<IContext> mContext;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<LocalService> mLocalService;

    AutoPtr<IIPowerManager> mIPowerManager;
    AutoPtr<IIWindowManager> mIWindowManager;
    AutoPtr<INotificationManager> mNotificationManager;

    // Stores and loads state on device and profile owners.
    AutoPtr<DeviceOwner> mDeviceOwner;

    /**
     * Whether or not device admin feature is supported. If it isn't return defaults for all
     * public methods.
     */
    Boolean mHasFeature;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<ISparseArray> mUserData;
};

} // namespace DevicePolicy
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_DEVICEPOLICY_CDEVICEPOLICYMANAGERSERVICE_H__
