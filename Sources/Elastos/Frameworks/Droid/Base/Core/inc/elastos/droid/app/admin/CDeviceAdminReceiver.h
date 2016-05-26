
#ifndef __ELASTOS_DROID_APP_ADMIN_CDEVICEADMINRECEIVER_H__
#define __ELASTOS_DROID_APP_ADMIN_CDEVICEADMINRECEIVER_H__

#include "_Elastos_Droid_App_Admin_CDeviceAdminReceiver.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;

CarClass(CDeviceAdminReceiver)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()


    /**
     * Retrieve the DevicePolicyManager interface for this administrator to work
     * with the system.
     */
    CARAPI GetManager(
        /* [in] */ IContext* context,
        /* [out] */ IDevicePolicyManager** manager);

    /**
     * Retrieve the ComponentName describing who this device administrator is, for
     * use in {@link DevicePolicyManager} APIs that require the administrator to
     * identify itself.
     */
    CARAPI GetWho(
        /* [in] */ IContext* context,
        /* [out] */ IComponentName** component);

    /**
     * Called after the administrator is first enabled, as a result of
     * receiving {@link #ACTION_DEVICE_ADMIN_ENABLED}.  At this point you
     * can use {@link DevicePolicyManager} to set your desired policies.
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnEnabled(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called when the user has asked to disable the administrator, as a result of
     * receiving {@link #ACTION_DEVICE_ADMIN_DISABLE_REQUESTED}, giving you
     * a chance to present a warning message to them.  The message is returned
     * as the result; if null is returned (the default implementation), no
     * message will be displayed.
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     * @return Return the warning message to display to the user before
     * being disabled; if null is returned, no message is displayed.
     */
    CARAPI OnDisableRequested(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [out] */ ICharSequence** message);

    /**
     * Called prior to the administrator being disabled, as a result of
     * receiving {@link #ACTION_DEVICE_ADMIN_DISABLED}.  Upon return, you
     * can no longer use the protected parts of the {@link DevicePolicyManager}
     * API.
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnDisabled(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called after the user has changed their password, as a result of
     * receiving {@link #ACTION_PASSWORD_CHANGED}.  At this point you
     * can use {@link DevicePolicyManager#getCurrentFailedPasswordAttempts()
     * DevicePolicyManager.getCurrentFailedPasswordAttempts()}
     * to retrieve the active password characteristics.
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnPasswordChanged(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called after the user has failed at entering their current password, as a result of
     * receiving {@link #ACTION_PASSWORD_FAILED}.  At this point you
     * can use {@link DevicePolicyManager} to retrieve the number of failed
     * password attempts.
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnPasswordFailed(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called after the user has succeeded at entering their current password,
     * as a result of receiving {@link #ACTION_PASSWORD_SUCCEEDED}.  This will
     * only be received the first time they succeed after having previously
     * failed.
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnPasswordSucceeded(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called periodically when the password is about to expire or has expired.  It will typically
     * be called at these times: on device boot, once per day before the password expires,
     * and at the time when the password expires.
     *
     * <p>If the password is not updated by the user, this method will continue to be called
     * once per day until the password is changed or the device admin disables password expiration.
     *
     * <p>The admin will typically post a notification requesting the user to change their password
     * in response to this call. The actual password expiration time can be obtained by calling
     * {@link DevicePolicyManager#getPasswordExpiration(ComponentName) }
     *
     * <p>The admin should be sure to take down any notifications it posted in response to this call
     * when it receives {@link DeviceAdminReceiver#onPasswordChanged(Context, Intent) }.
     *
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnPasswordExpiring(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called when provisioning of a managed profile or managed device has completed successfully.
     *
     * <p> As a prerequisit for the execution of this callback the (@link DeviceAdminReceiver} has
     * to declare an intent filter for {@link #ACTION_PROFILE_PROVISIONING_COMPLETE}.
     * Its component must also be specified in the {@link DevicePolicyManager#EXTRA_DEVICE_ADMIN}
     * of the {@link DevicePolicyManager#ACTION_PROVISION_MANAGED_PROFILE} intent that started the
     * managed provisioning.
     *
     * <p>When provisioning is complete, the managed profile is hidden until the profile owner
     * calls {DevicePolicyManager#setProfileEnabled(ComponentName admin)}. Typically a profile
     * owner will enable the profile when it has finished any additional setup such as adding an
     * account by using the {@link AccountManager} and calling apis to bring the profile into the
     * desired state.
     *
     * <p> Note that provisioning completes without waiting for any server interactions, so the
     * profile owner needs to wait for data to be available if required (e.g android device ids or
     * other data that is set as a result of server interactions).
     *
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnProfileProvisioningComplete(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Called when a device is entering lock task mode by a package authorized
     * by {@link DevicePolicyManager#isLockTaskPermitted(String)}
     *
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     * @param pkg If entering, the authorized package using lock task mode, otherwise null.
     */
    CARAPI OnLockTaskModeEntering(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& pkg);

    /**
     * Called when a device is exiting lock task mode by a package authorized
     * by {@link DevicePolicyManager#isLockTaskPermitted(String)}
     *
     * @param context The running context as per {@link #onReceive}.
     * @param intent The received intent as per {@link #onReceive}.
     */
    CARAPI OnLockTaskModeExiting(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Intercept standard device administrator broadcasts.  Implementations
     * should not override this method; it is better to implement the
     * convenience callbacks for each action.
     */
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static Boolean sLocalLOGV;

    AutoPtr<CDevicePolicyManager> mManager;
    AutoPtr<IComponentName> mWho;
};

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ADMIN_CDEVICEADMINRECEIVER_H__
