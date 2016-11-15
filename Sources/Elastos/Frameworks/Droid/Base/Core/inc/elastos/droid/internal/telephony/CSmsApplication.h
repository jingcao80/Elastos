
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSAPPLICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSAPPLICATION_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Internal_Telephony_CSmsApplication.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Content::PackageMonitor;

using Elastos::Core::Singleton;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsApplication)
    , public Singleton
    , public ISmsApplication
{
public:
    class SmsApplicationData
        : public Object
        , public ISmsApplicationData
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ const String& applicationName,
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        /**
         * Returns TRUE if this SmsApplicationData is Complete (all intents handled).
         * @return
         */
        IsComplete(
            /* [out] */ Boolean* result);

    public:
        /**
         * Name of this SMS app for display.
         */
        String mApplicationName;

        /**
         * Package name for this SMS app.
         */
        String mPackageName;

        /**
         * The class name of the SMS_DELIVER_ACTION receiver in this app.
         */
        String mSmsReceiverClass;

        /**
         * The class name of the WAP_PUSH_DELIVER_ACTION receiver in this app.
         */
        String mMmsReceiverClass;

        /**
         * The class name of the ACTION_RESPOND_VIA_MESSAGE intent in this app.
         */
        String mRespondViaMessageClass;

        /**
         * The class name of the ACTION_SENDTO intent in this app.
         */
        String mSendToClass;

        /**
         * The user-id for this application
         */
        Int32 mUid;
    };

private:
    /**
     * Tracks package changes and ensures that the default SMS app is always configured to be the
     * preferred activity for SENDTO sms/mms intents.
     */
    class SmsPackageMonitor
        : public PackageMonitor
    {
    public:
        SmsPackageMonitor(
            /* [in] */ IContext* context,
            /* [in] */ CSmsApplication* host);

        //@Override
        CARAPI OnPackageDisappeared(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 reason);

        //@Override
        CARAPI OnPackageAppeared(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 reason);

        //@Override
        CARAPI OnPackageModified(
            /* [in] */ const String& packageName);

    private:
        void OnPackageChanged(
            /* [in] */ const String& packageName);

    private:
        AutoPtr<IContext> mContext;
        CSmsApplication* mHost;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns the list of available SMS apps defined as apps that are registered for both the
     * SMS_RECEIVED_ACTION (SMS) and WAP_PUSH_RECEIVED_ACTION (MMS) broadcasts (and their broadcast
     * receivers are enabled)
     *
     * Requirements to be an SMS application:
     * Implement SMS_DELIVER_ACTION broadcast receiver.
     * Require BROADCAST_SMS permission.
     *
     * Implement WAP_PUSH_DELIVER_ACTION broadcast receiver.
     * Require BROADCAST_WAP_PUSH permission.
     *
     * Implement RESPOND_VIA_MESSAGE intent.
     * Support smsto Uri scheme.
     * Require SEND_RESPOND_VIA_MESSAGE permission.
     *
     * Implement ACTION_SENDTO intent.
     * Support smsto Uri scheme.
     */
    CARAPI GetApplicationCollection(
        /* [in] */ IContext* context,
        /* [out] */ ICollection** result);

    /**
     * Sets the specified package as the default SMS/MMS application. The caller of this method
     * needs to have permission to set AppOps and write to secure settings.
     */
    CARAPI SetDefaultApplication(
        /* [in] */ const String& packageName,
        /* [in] */ IContext* context);

    CARAPI InitSmsPackageMonitor(
        /* [in] */ IContext* context);

    /**
     * Returns SmsApplicationData for this package if this package is capable of being set as the
     * default SMS application.
     */
    CARAPI GetSmsApplicationData(
        /* [in] */ const String& packageName,
        /* [in] */ IContext* context,
        /* [out] */ ISmsApplicationData** result);

    /**
     * Gets the default SMS application
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to deliver SMS messages to
     */
    CARAPI GetDefaultSmsApplication(
        /* [in] */ IContext* context,
        /* [in] */ Boolean updateIfNeeded,
        /* [out] */ IComponentName** result);

    /**
     * Gets the default MMS application
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to deliver MMS messages to
     */
    CARAPI GetDefaultMmsApplication(
        /* [in] */ IContext* context,
        /* [in] */ Boolean updateIfNeeded,
        /* [out] */ IComponentName** result);

    /**
     * Gets the default Respond Via Message application
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to direct Respond Via Message intent to
     */
    CARAPI GetDefaultRespondViaMessageApplication(
        /* [in] */ IContext* context,
        /* [in] */ Boolean updateIfNeeded,
        /* [out] */ IComponentName** result);

    /**
     * Gets the default Send To (smsto) application.
     * <p>
     * Caller must pass in the correct user context if calling from a singleton service.
     * @param context context from the calling app
     * @param updateIfNeeded update the default app if there is no valid default app configured.
     * @return component name of the app and class to direct SEND_TO (smsto) intent to
     */
    CARAPI GetDefaultSendToApplication(
        /* [in] */ IContext* context,
        /* [in] */ Boolean updateIfNeeded,
        /* [out] */ IComponentName** result);

    /**
     * Returns whether need to write the SMS message to SMS database for this package.
     * <p>
     * Caller must pass in the correct user context if calling from a singleton service.
     */
    CARAPI ShouldWriteMessageForPackage(
        /* [in] */ const String& packageName,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

private:
    static Int32 GetIncomingUserId(
        /* [in] */ IContext* context);

    static AutoPtr<ICollection> GetApplicationCollectionInternal(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);
    /**
     * Checks to see if we have a valid installed SMS application for the specified package name
     * @return Data for the specified package name or null if there isn't one
     */
    static AutoPtr<SmsApplicationData> GetApplicationForPackage(
        /* [in] */ ICollection* applications,
        /* [in] */ const String& packageName);

    /**
     * Get the application we will use for delivering SMS/MMS messages.
     *
     * We return the preferred sms application with the following order of preference:
     * (1) User selected SMS App (if selected, and if still valid)
     * (2) Android Messaging (if installed)
     * (3) The currently configured highest priority broadcast receiver
     * (4) Null
     */
    static AutoPtr<SmsApplicationData> GetApplication(
        /* [in] */ IContext* context,
        /* [in] */ Boolean updateIfNeeded,
        /* [in] */ Int32 userId);

    static void SetDefaultApplicationInternal(
        /* [in] */ const String& packageName,
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    static void ConfigurePreferredActivity(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 userId);

    /**
     * Updates the ACTION_SENDTO activity to the specified component for the specified scheme.
     */
    static void ReplacePreferredActivity(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IComponentName* componentName,
        /* [in] */ Int32 userId,
        /* [in] */ const String& scheme);
public:
    static const String LOGTAG;

private:
    static const String PHONE_PACKAGE_NAME;
    static const String BLUETOOTH_PACKAGE_NAME;
    static const String MMS_SERVICE_PACKAGE_NAME;

    static const String SCHEME_SMS;
    static const String SCHEME_SMSTO;
    static const String SCHEME_MMS;
    static const String SCHEME_MMSTO;
    static const Boolean DEBUG_MULTIUSER;

    static AutoPtr<SmsPackageMonitor> sSmsPackageMonitor;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSAPPLICATION_H__
