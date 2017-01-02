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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITORCALLBACK_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITORCALLBACK_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Callback for general information relevant to lock screen.
 */
class KeyguardUpdateMonitorCallback
    : public Object
    , public IKeyguardUpdateMonitorCallback
{
public:
    CAR_INTERFACE_DECL()

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
     * @param subId The subscription id which PLMN or SPN changed.
     */
    CARAPI OnRefreshCarrierInfo(
        /* [in] */ ICharSequence* plmn,
        /* [in] */ ICharSequence* spn);

    /**
     * Called when the airplane mode changes.
     *
     * @param on Indicates if the airplane mode is now enable.
     */
    CARAPI OnAirplaneModeChanged(
        /* [in] */ Boolean on);

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
     * Called when the SIM state of a subscription changes.
     * @param simState
     * @param subId The subscription id which SIM state changed.
     */
    CARAPI OnSimStateChanged(
        /* [in] */ IccCardConstantsState simState);

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
        /* [in] */ Int32 userid);

    /**
     * Called when a subId for the slot is changed.
     * @param oldSubId.
     * @param newSubId.
     */
    CARAPI OnSubIdUpdated(
        /* [in] */ Int64 oldSubId,
        /* [in] */ Int64 newSubId);

    /**
     * Called when the SubInfo content changed
     * @param subId The subscription id which subscription info record is updated
     * @param column The column name which is updated
     * @param sValue The new string if the colum value is string
     * @param iValue The new integer value if the colum value is integer
     */
    CARAPI OnSubInfoContentChanged(
        /* [in] */ Int64 subId,
        /* [in] */ const String& column,
        /* [in] */ const String& sValue,
        /* [in] */ Int32 iValue);

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