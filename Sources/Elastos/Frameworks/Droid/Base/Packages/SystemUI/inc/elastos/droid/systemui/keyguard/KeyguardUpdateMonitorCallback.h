
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITORCALLBACK_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITORCALLBACK_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Callback for general information relevant to lock screen.
 */
class KeyguardUpdateMonitorCallback
    : public Object
{
public:
    TO_STRING_IMPL("KeyguardUpdateMonitorCallback")

    KeyguardUpdateMonitorCallback();

    /**
     * Called when the battery status changes, e.g. when plugged in or unplugged, charge
     * level, etc. changes.
     *
     * @param status current battery status
     */
    CARAPI OnRefreshBatteryInfo(
        /* [in] */ IKeyguardUpdateMonitorBatteryStatus* status);

    /**
     * Called once per minute or when the time changes.
     */
    CARAPI OnTimeChanged();

    /**
     * Called when the carrier PLMN or SPN changes.
     *
     * @param plmn The operator name of the registered network.  May be null if it shouldn't
     *   be displayed.
     * @param spn The service provider name.  May be null if it shouldn't be displayed.
     */
    CARAPI OnRefreshCarrierInfo(
        /* [in] */ ICharSequence* plmn,
        /* [in] */ ICharSequence* spn);

    /**
     * Called when the ringer mode changes.
     * @param state the current ringer state, as defined in
     * {@link AudioManager#RINGER_MODE_CHANGED_ACTION}
     */
    CARAPI OnRingerModeChanged(
        /* [in] */ Int32 state);

    /**
     * Called when the phone state changes. String will be one of:
     * {@link TelephonyManager#EXTRA_STATE_IDLE}
     * {@link TelephonyManager@EXTRA_STATE_RINGING}
     * {@link TelephonyManager#EXTRA_STATE_OFFHOOK
     */
    CARAPI OnPhoneStateChanged(
        /* [in] */ Int32 phoneState);

    /**
     * Called when the visibility of the keyguard changes.
     * @param showing Indicates if the keyguard is now visible.
     */
    CARAPI OnKeyguardVisibilityChanged(
        /* [in] */ Boolean showing);

    CARAPI OnKeyguardVisibilityChangedRaw(
        /* [in] */ Boolean showing);

    /**
     * Called when the keyguard enters or leaves bouncer mode.
     * @param bouncer if true, keyguard is now in bouncer mode.
     */
    CARAPI OnKeyguardBouncerChanged(
        /* [in] */ Boolean bouncer);

    /**
     * Called when visibility of lockscreen clock changes, such as when
     * obscured by a widget.
     */
    CARAPI OnClockVisibilityChanged();

    /**
     * Called when the device becomes provisioned
     */
    CARAPI OnDeviceProvisioned();

    /**
     * Called when the device policy changes.
     * See {@link DevicePolicyManager#ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED}
     */
    CARAPI OnDevicePolicyManagerStateChanged();

    /**
     * Called when the user change begins.
     */
    CARAPI OnUserSwitching(
        /* [in] */ Int32 userId);

    /**
     * Called when the user change is complete.
     */
    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 userId);

    /**
     * Called when the SIM state changes.
     * @param simState
     */
    CARAPI OnSimStateChanged(
        /* [in] */ IIccCardConstantsState simState);

    /**
     * Called when a user is removed.
     */
    CARAPI OnUserRemoved(
        /* [in] */ Int32 userId);

    /**
     * Called when the user's info changed.
     */
    CARAPI OnUserInfoChanged(
        /* [in] */ Int32 userId);

    /**
     * Called when boot completed.
     *
     * Note, this callback will only be received if boot complete occurs after registering with
     * KeyguardUpdateMonitor.
     */
    CARAPI OnBootCompleted();

    /**
     * Called when the emergency call button is pressed.
     */
    CARAPI OnEmergencyCallAction();

    /**
     * Called when the transport background changes.
     * @param bitmap
     */
    CARAPI OnSetBackground(
        /* [in] */ IBitmap* bitmap);

    /**
     * Called when the screen turns on
     */
    CARAPI OnScreenTurnedOn();

    /**
     * Called when the screen turns off
     * @param why either {@link WindowManagerPolicy#OFF_BECAUSE_OF_ADMIN},
     * {@link WindowManagerPolicy#OFF_BECAUSE_OF_USER}, or
     * {@link WindowManagerPolicy#OFF_BECAUSE_OF_TIMEOUT}.
     */
    CARAPI OnScreenTurnedOff(
        /* [in] */ Int32 why);

    /**
     * Called when trust changes for a user.
     */
    CARAPI OnTrustChanged(
        /* [in] */ Int32 userId);

    /**
     * Called when trust being managed changes for a user.
     */
    CARAPI OnTrustManagedChanged(
        /* [in] */ Int32 userId);

    /**
     * Called when the user has proved to a trust agent that they want to use the device.
     */
    CARAPI OnTrustInitiatedByUser(
        /* [in] */ Int32 userId);

    /**
     * Called when a fingerprint is recognized.
     * @param userId
     */
    CARAPI OnFingerprintRecognized(
        /* [in] */ Int32 userId);

    /**
     * Called when fingerprint is acquired but not yet recognized
     */
    CARAPI OnFingerprintAcquired(
        /* [in] */ Int32 info);

    /**
     * Called when the state of face unlock changed.
     */
    CARAPI OnFaceUnlockStateChanged(
        /* [in] */ Boolean running,
        /* [in] */ Int32 userId);

private:
    static const Int64 VISIBILITY_CHANGED_COLLAPSE_MS;
    Int64 mVisibilityChangedCalled;
    Boolean mShowing;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITORCALLBACK_H__