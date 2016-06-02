
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSAPPLICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSAPPLICATION_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using Elastos::Core::Object;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SmsApplication
    : public Singleton
    , public ISmsApplication
{
private:
    /**
     * Tracks package changes and ensures that the default SMS app is always configured to be the
     * preferred activity for SENDTO sms/mms intents.
     */
    class SmsPackageMonitor
        : public PackageMonitor
    {
    public:
        SmsPackageMonitor(IContext* context);

        //@Override
        CARAPI OnPackageDisappeared(String packageName, Int32 reason);

        //@Override
        CARAPI OnPackageAppeared(String packageName, Int32 reason);

        //@Override
        CARAPI OnPackageModified(String packageName);

    private:
        void OnPackageChanged(String packageName);

    private:
        AutoPtr<IContext> mContext;
    }

public:
    class SmsApplicationData
        : public Object
        , public ISmsApplicationData
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ String applicationName,
            /* [in] */ String packageName,
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

public:
    CAR_INTERFACE_DECL()

    SmsApplication();

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
        /* [in] */ String packageName,
        /* [in] */ IContext* context);

    CARAPI InitSmsPackageMonitor(
        /* [in] */ IContext* context);

    /**
     * Returns SmsApplicationData for this package if this package is capable of being set as the
     * default SMS application.
     */
    CARAPI GetSmsApplicationData(
        /* [in] */ String packageName,
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
        /* [in] */ String packageName,
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

private:
    static Int32 getIncomingUserId(IContext* context);
    static AutoPtr<ICollection> GetApplicationCollectionInternal(
        IContext* context,
        Int32 userId);
    /**
     * Checks to see if we have a valid installed SMS application for the specified package name
     * @return Data for the specified package name or null if there isn't one
     */
    static AutoPtr<ISmsApplicationData> GetApplicationForPackage(
            ICollection* applications,
            String packageName);

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
            IContext* context,
            Boolean updateIfNeeded,
            Int32 userId);

    static void SetDefaultApplicationInternal(String packageName, IContext* context,
            Int32 userId);

    static void ConfigurePreferredActivity(IPackageManager* packageManager,
            IComponentName* componentName, Int32 userId);

    /**
     * Updates the ACTION_SENDTO activity to the specified component for the specified scheme.
     */
    static void ReplacePreferredActivity(IPackageManager* packageManager,
            IComponentName* componentName, Int32 userId, String scheme);
public:
    static const String LOG_TAG;

private:
    static const String PHONE_PACKAGE_NAME;
    static const String BLUETOOTH_PACKAGE_NAME;
    static const String MMS_SERVICE_PACKAGE_NAME;

    static const String SCHEME_SMS;
    static const String SCHEME_SMSTO;
    static const String SCHEME_MMS;
    static const String SCHEME_MMSTO;
    static const Boolean DEBUG_MULTIUSER;

    static AutoPtr<ISmsPackageMonitor> sSmsPackageMonitor;

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SMSAPPLICATION_H__
