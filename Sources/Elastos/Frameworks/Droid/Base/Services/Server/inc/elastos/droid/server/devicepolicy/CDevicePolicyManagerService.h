
#ifndef __ELASTOS_DROID_SERVER_CDEVICEPOLICYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CDEVICEPOLICYMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CDevicePolicyManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "util/JournaledFile.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/HashMap.h>

using namespace Elastos::Utility::Logging;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::App::Admin::IDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Utility::JournaledFile;

namespace Elastos {
namespace Droid {
namespace Server {

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
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CDevicePolicyManagerService::RemoveWarningReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
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
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CDevicePolicyManagerService::ActiveAdminLockedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
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
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CDevicePolicyManagerService::DevicePolicyReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        CDevicePolicyManagerService* mHost;
    };

    class DevicePolicyReceiverRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        DevicePolicyReceiverRunnable(
            /* [in] */ CDevicePolicyManagerService* host,
            /* [in] */ Int32 userHandle);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        CDevicePolicyManagerService* mHost;
        Int32 mUserHandle;
    };

    class DeviceOrUserLockedRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        DeviceOrUserLockedRunnable(
            /* [in] */ CDevicePolicyManagerService* host,
            /* [in] */ Int32 userHandle);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        CDevicePolicyManagerService* mHost;
        Int32 mUserHandle;
    };

    class DevicePolicyData
        : public ElRefBase
    {
    public:
        DevicePolicyData(
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
        {}

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

        HashMap<AutoPtr<IComponentName>, AutoPtr<ActiveAdmin> > mAdminMap;
        List<AutoPtr<ActiveAdmin> > mAdminList;
    };

    class ActiveAdmin
        : public ElRefBase
    {
    public:
        ActiveAdmin(
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
            , mSpecifiesGlobalProxy(FALSE)
            , mGlobalProxySpec(NULL)
            , mGlobalProxyExclusionList(NULL)
        {}

        CARAPI_(Int32) GetUid();

        CARAPI_(AutoPtr<IUserHandle>) GetUserHandle();

        CARAPI_(void) WriteToXml(
            /* [in] */ IXmlSerializer* out);

        CARAPI_(void) ReadFromXml(
            /* [in] */ IXmlPullParser* parser);

        CARAPI_(void) Dump(
            /* [in] */ const String& prefix,
            /* [in] */ IPrintWriter* pw);

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

        // TODO: review implementation decisions with frameworks team
        Boolean mSpecifiesGlobalProxy;
        String mGlobalProxySpec;
        String mGlobalProxyExclusionList;
    };

public:
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

    CARAPI GetActiveAdmins(
        /* [in] */ Int32 userHandle,
        /* [out] */ IObjectContainer** admins);

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

    CARAPI_(void) WipeDataLocked(
        /* [in] */ Int32 flags);

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

    static CARAPI_(AutoPtr<JournaledFile>) MakeJournaledFile(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) SaveSettingsLocked(
        /* [in] */ Int32 userHandle);

    CARAPI_(void) SendChangedNotification(
        /* [in] */ Int32 userHandle);

    CARAPI LoadSettingsLocked(
        /* [in] */ DevicePolicyData* policy,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) HandlePasswordExpirationNotification(
        /* [in] */ DevicePolicyData* policy);

    /**
     * Return a single admin's expiration date/time, or the min (soonest) for all admins.
     * Returns 0 if not configured.
     */
    CARAPI_(Int64) GetPasswordExpirationLocked(
        /* [in] */ IComponentName* who,
        /* [in] */ Int32 userHandle);

    CARAPI_(void) LockNowUnchecked();

    CARAPI_(Boolean) IsExtStorageEncrypted();

    CARAPI_(void) WipeDeviceOrUserLocked(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userHandle);

    /**
     * Called any time the device password is updated.  Resets all password expiration clocks.
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
    CARAPI_(Int32) GetEncryptionStatus();

    /**
     * Hook to low-levels:  If needed, record the new admin setting for encryption.
     */
    CARAPI_(void) SetEncryptionRequested(
        /* [in] */ Boolean encrypt);

    CARAPI EnforceCrossUserPermission(
        /* [in] */ Int32 userHandle);

public:
    static const String ACTION_EXPIRED_PASSWORD_NOTIFICATION;

    /**
     * The system property used to share the state of the camera. The native camera service
     * is expected to read this property and act accordingly.
     */
    static const String SYSTEM_PROP_DISABLE_CAMERA;

    Object mLock;

private:
    static const String DEVICE_POLICIES_XML;

    static const String TAG;

    static const Int32 REQUEST_EXPIRE_PASSWORD;

    static const Int64 MS_PER_DAY;

    static const Int64 EXPIRATION_GRACE_PERIOD_MS;

    static const Boolean DBG;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IIPowerManager> mIPowerManager;
    AutoPtr<IIWindowManager> mIWindowManager;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IBroadcastReceiver> mReceiver;
    HashMap<Int32, AutoPtr<DevicePolicyData> > mUserData;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CDEVICEPOLICYMANAGERSERVICE_H__
