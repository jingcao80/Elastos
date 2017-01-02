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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITOR_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/service/fingerprint/FingerprintManagerReceiver.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::Trust::ITrustListener;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState;
using Elastos::Droid::Service::Fingerprint::FingerprintManagerReceiver;
using Elastos::Droid::Service::Fingerprint::IFingerprintManagerReceiver;
using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Watches for updates that may be interesting to the keyguard, and provides
 * the up to date information as well as a registration for callbacks that care
 * to be updated.
 *
 * Note: under time crunch, this has been extended to include some stuff that
 * doesn't really belong here.  see {@link #handleBatteryUpdate} where it shutdowns
 * the device, and {@link #getFailedUnlockAttempts()}, {@link #reportFailedAttempt()}
 * and {@link #clearFailedUnlockAttempts()}.  Maybe we should rename this 'KeyguardContext'...
 */
class KeyguardUpdateMonitor
    : public Object
    , public IKeyguardUpdateMonitor
{
public:
    class TrustListener
        : public Object
        , public ITrustListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("KeyguardUpdateMonitor::TrustListener")

        TrustListener(
            /* [in] */ KeyguardUpdateMonitor* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnTrustChanged(
            /* [in] */ Boolean enabled,
            /* [in] */ Int32 userId,
            /* [in] */ Boolean initiatedByUser);

        //@Override
        CARAPI OnTrustManagedChanged(
            /* [in] */ Boolean managed,
            /* [in] */ Int32 userId);

    private:
        KeyguardUpdateMonitor* mHost;

    };

    class BatteryStatus
        : public Object
        , public IKeyguardUpdateMonitorBatteryStatus
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("KeyguardUpdateMonitor::BatteryStatus")

        BatteryStatus(
            /* [in] */ Int32 status,
            /* [in] */ Int32 level,
            /* [in] */ Int32 plugged,
            /* [in] */ Int32 health)
            : mStatus(status)
            , mLevel(level)
            , mPlugged(plugged)
            , mHealth(health)
        {}

        CARAPI SetStatus(
            /* [in] */ Int32 value);

        CARAPI GetStatus(
            /* [out] */ Int32* value);

        CARAPI SetLevel(
            /* [in] */ Int32 value);

        CARAPI GetLevel(
            /* [out] */ Int32* value);

        CARAPI SetPlugged(
            /* [in] */ Int32 value);

        CARAPI GetPlugged(
            /* [out] */ Int32* value);

        CARAPI SetHealth(
            /* [in] */ Int32 value);

        CARAPI GetHealth(
            /* [out] */ Int32* value);

        /**
         * Determine whether the device is plugged in (USB, power, or wireless).
         * @return true if the device is plugged in.
         */
        CARAPI IsPluggedIn(
            /* [out] */ Boolean* result);

        /**
         * Whether or not the device is charged. Note that some devices never return 100% for
         * battery level, so this allows either battery level or status to determine if the
         * battery is charged.
         * @return true if the device is charged
         */
        CARAPI IsCharged(
            /* [out] */ Boolean* result);
        /**
         * Whether battery is low and needs to be charged.
         * @return true if battery is low
         */
        CARAPI IsBatteryLow(
            /* [out] */ Boolean* result);

    public:
        Int32 mStatus;
        Int32 mLevel;
        Int32 mPlugged;
        Int32 mHealth;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("KeyguardUpdateMonitor::MyBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IKeyguardUpdateMonitor* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        KeyguardUpdateMonitor* mHost;
    };

    class MyBroadcastReceiver2
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("KeyguardUpdateMonitor::MyBroadcastReceiver2")

        CARAPI constructor(
            /* [in] */ IKeyguardUpdateMonitor* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        KeyguardUpdateMonitor* mHost;
    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("KeyguardUpdateMonitor::MyContentObserver")

        CARAPI constructor(
            /* [in] */ IKeyguardUpdateMonitor* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        KeyguardUpdateMonitor* mHost;
    };

    class UserSwitchObserver
        : public Object
        , public IIUserSwitchObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IKeyguardUpdateMonitor* monitor);

        CARAPI OnUserSwitching(
            /* [in] */ Int32 newUserId,
            /* [in] */ IIRemoteCallback* reply);

        CARAPI OnUserSwitchComplete(
            /* [in] */ Int32 newUserId);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        KeyguardUpdateMonitor* mHost;
    };

private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("KeyguardUpdateMonitor::MyHandler")

        MyHandler(
            /* [in] */ KeyguardUpdateMonitor* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        KeyguardUpdateMonitor* mHost;
    };

    friend class CKeyguardHostView;

    class DisplayClientState
        : public Object
        , public IKeyguardUpdateMonitorDisplayClientState
    {
    public:
        TO_STRING_IMPL("KeyguardUpdateMonitor::DisplayClientState")

        CAR_INTERFACE_DECL();

        DisplayClientState();

    public:
        Int32 mClientGeneration;
        Boolean mClearing;
        AutoPtr<IPendingIntent> mIntent;
        Int32 mPlaybackState;
        Int64 mPlaybackEventTime;
    };

    /**
     * When we receive a
     * {@link com.android.internal.telephony.TelephonyIntents#ACTION_SIM_STATE_CHANGED} broadcast,
     * and then pass a result via our handler to {@link KeyguardUpdateMonitor#handleSimStateChange},
     * we need a single object to pass to the handler.  This class helps decode
     * the intent and provide a {@link SimCard.State} result.
     */
    class SimArgs
        : public Object
    {
    public:
        SimArgs(
            /* [in] */ IccCardConstantsState state);

        static CARAPI FromIntent(
            /* [in] */ IIntent* intent,
            /* [out] */ SimArgs** args);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        IccCardConstantsState mSimState;
    };

    class MyFingerprintManagerReceiver
        : public FingerprintManagerReceiver
    {
    public:
        TO_STRING_IMPL("KeyguardUpdateMonitor::MyFingerprintManagerReceiver")

        MyFingerprintManagerReceiver(
            /* [in] */ KeyguardUpdateMonitor* host);

        //@Override
        CARAPI OnProcessed(
            /* [in] */ Int32 fingerprintId);

        //@Override
        CARAPI OnAcquired(
            /* [in] */ Int32 info);

        //@Override
        CARAPI OnError(
            /* [in] */ Int32 error);

    private:
        KeyguardUpdateMonitor* mHost;
    };

public:
    TO_STRING_IMPL("KeyguardUpdateMonitor")

    CAR_INTERFACE_DECL()

    KeyguardUpdateMonitor();

    CARAPI IsFaceUnlockRunning(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetUserHasTrust(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetUserTrustIsManaged(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IKeyguardUpdateMonitor>) GetInstance(
        /* [in] */ IContext* context);

    /**
     * IMPORTANT: Must be called from UI thread.
     */
    CARAPI DispatchSetBackground(
        /* [in] */ IBitmap* bmp);

    /**
     * This is exposed since {@link Intent#ACTION_BOOT_COMPLETED} is not sticky. If
     * keyguard crashes sometime after boot, then it will never receive this
     * broadcast and hence not handle the event. This method is ultimately called by
     * PhoneWindowManager in this case.
     */
    CARAPI DispatchBootCompleted();

    /**
     * We need to store this state in the KeyguardUpdateMonitor since this class will not be
     * destroyed.
     */
    CARAPI HasBootCompleted(
        /* [out] */ Boolean* result);

    CARAPI IsKeyguardVisible(
        /* [out] */ Boolean* result);

    /**
     * @return if the keyguard is currently in bouncer mode.
     */
    CARAPI IsKeyguardBouncer(
        /* [out] */ Boolean* result);

    CARAPI IsSwitchingUser(
        /* [out] */ Boolean* result);

    /**
     * Remove the given observer's callback.
     *
     * @param callback The callback to remove
     */
    CARAPI RemoveCallback(
        /* [in] */ IKeyguardUpdateMonitorCallback* callback);

    /**
     * Register to receive notifications about general keyguard information
     * (see {@link InfoCallback}.
     * @param callback The callback to register
     */
    CARAPI RegisterCallback(
        /* [in] */ IKeyguardUpdateMonitorCallback* callback);

    CARAPI SendKeyguardVisibilityChanged(
        /* [in] */ Boolean showing);

    /**
     * @see #handleKeyguardBouncerChanged(int)
     */
    CARAPI SendKeyguardBouncerChanged(
        /* [in] */ Boolean showingBouncer);

    CARAPI ReportClockVisible(
        /* [in] */ Boolean visible);

    CARAPI GetSimState(
        /* [out] */ IccCardConstantsState* state);

    /**
     * Report that the user successfully entered the SIM PIN or PUK/SIM PIN so we
     * have the information earlier than waiting for the intent
     * broadcast from the telephony code.
     *
     * NOTE: Because handleSimStateChange() invokes callbacks immediately without going
     * through mHandler, this *must* be called from the UI thread.
     */
    CARAPI ReportSimUnlocked();

    /**
     * Report that the emergency call button has been pressed and the emergency dialer is
     * about to be displayed.
     *
     * @param bypassHandler runs immediately.
     *
     * NOTE: Must be called from UI thread if bypassHandler == true.
     */
    CARAPI ReportEmergencyCallAction(
        /* [in] */ Boolean bypassHandler);

    CARAPI GetTelephonyPlmn(
        /* [out] */ ICharSequence** cchar);

    CARAPI GetTelephonySpn(
        /* [out] */ ICharSequence** cchar);

    /**
     * @return Whether the device is provisioned (whether they have gone through
     *   the setup wizard)
     */
    CARAPI IsDeviceProvisioned(
        /* [out] */ Boolean* result);

    CARAPI GetFailedUnlockAttempts(
        /* [out] */ Int32* attempts);

    CARAPI ClearFailedUnlockAttempts();

    CARAPI ClearFingerprintRecognized();

    CARAPI ReportFailedUnlockAttempt();

    CARAPI IsClockVisible(
        /* [out] */ Boolean* result);

    CARAPI GetPhoneState(
        /* [out] */ Int32* state);

    CARAPI ReportFailedBiometricUnlockAttempt();

    CARAPI GetMaxBiometricUnlockAttemptsReached(
        /* [out] */ Boolean* result);

    CARAPI IsAlternateUnlockEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetAlternateUnlockEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsSimLocked(
        /* [out] */ Boolean* result);

    static CARAPI IsSimLocked(
        /* [in] */ IccCardConstantsState state,
        /* [out] */ Boolean* result);

    CARAPI IsSimPinSecure(
        /* [out] */ Boolean* result);

    static CARAPI IsSimPinSecure(
        /* [in] */ IccCardConstantsState state,
        /* [out] */ Boolean* result);

    CARAPI GetCachedDisplayClientState(
        /* [out] */ IKeyguardUpdateMonitorDisplayClientState** state);

    // TODO: use these callbacks elsewhere in place of the existing notifyScreen*()
    // (KeyguardViewMediator, KeyguardHostView)
    CARAPI DispatchScreenTurnedOn();

    CARAPI DispatchScreenTurndOff(
        /* [in] */ Int32 why);

    CARAPI IsScreenOn(
        /* [out] */ Boolean* result);

protected:
    CARAPI HandleScreenTurnedOn();

    CARAPI HandleScreenTurnedOff(
        /* [in] */ Int32 arg1);

    /**
     * Handle {@link #MSG_DPM_STATE_CHANGED}
     */
    CARAPI HandleDevicePolicyManagerStateChanged();

    /**
     * Handle {@link #MSG_USER_SWITCHING}
     */
    CARAPI HandleUserSwitching(
        /* [in] */ Int32 userId,
        /* [in] */ IIRemoteCallback* reply);

    /**
     * Handle {@link #MSG_USER_SWITCH_COMPLETE}
     */
    CARAPI HandleUserSwitchComplete(
        /* [in] */ Int32 userId);

    /**
     * Handle {@link #MSG_BOOT_COMPLETED}
     */
    CARAPI HandleBootCompleted();

    /**
     * Handle {@link #MSG_USER_REMOVED}
     */
    CARAPI HandleUserRemoved(
        /* [in] */ Int32 userId);

    /**
     * Handle {@link #MSG_DEVICE_PROVISIONED}
     */
    CARAPI HandleDeviceProvisioned();

    /**
     * Handle {@link #MSG_PHONE_STATE_CHANGED}
     */
    CARAPI HandlePhoneStateChanged(
        /* [in] */ const String& newState);

    /**
     * Handle {@link #MSG_RINGER_MODE_CHANGED}
     */
    CARAPI HandleRingerModeChange(
        /* [in] */ Int32 mode);

private:
    CARAPI_(void) OnFingerprintRecognized(
        /* [in] */ Int32 userId);

    CARAPI_(void) HandleFingerprintProcessed(
        /* [in] */ Int32 fingerprintId);

    CARAPI_(void) HandleFingerprintAcquired(
        /* [in] */ Int32 info);

    CARAPI_(void) HandleFaceUnlockStateChanged(
        /* [in] */ Boolean running,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) IsTrustDisabled(
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) IsFingerprintDisabled(
        /* [in] */ Int32 userId);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsDeviceProvisionedInSettingsDb();

    CARAPI_(void) WatchForDeviceProvisioning();

    /**
     * Handle {@link #MSG_TIME_UPDATE}
     */
    CARAPI_(void) HandleTimeUpdate();

    /**
     * Handle {@link #MSG_BATTERY_UPDATE}
     */
    CARAPI_(void) HandleBatteryUpdate(
        /* [in] */ IKeyguardUpdateMonitorBatteryStatus* status);

    /**
     * Handle {@link #MSG_CARRIER_INFO_UPDATE}
     */
    CARAPI_(void) HandleCarrierInfoUpdate();

    /**
     * Handle {@link #MSG_SIM_STATE_CHANGE}
     */
    CARAPI_(void) HandleSimStateChange(
        /* [in] */ SimArgs* simArgs);

    /**
     * Handle {@link #MSG_CLOCK_VISIBILITY_CHANGED}
     */
    CARAPI_(void) HandleClockVisibilityChanged();

    /**
     * Handle {@link #MSG_KEYGUARD_VISIBILITY_CHANGED}
     */
    CARAPI_(void) HandleKeyguardVisibilityChanged(
        /* [in] */ Int32 showing);

    CARAPI_(void) HandleUserInfoChanged(
        /* [in] */ Int32 userId);

    /**
     * Handle {@link #MSG_KEYGUARD_BOUNCER_CHANGED}
     * @see #sendKeyguardBouncerChanged(boolean)
     */
    CARAPI_(void) HandleKeyguardBouncerChanged(
        /* [in] */ Int32 bouncer);

    /**
     * Handle {@link #MSG_REPORT_EMERGENCY_CALL_ACTION}
     */
    CARAPI_(void) HandleReportEmergencyCallAction();

    static CARAPI_(Boolean) IsBatteryUpdateInteresting(
        /* [in] */ IKeyguardUpdateMonitorBatteryStatus* old,
        /* [in] */ IKeyguardUpdateMonitorBatteryStatus* current);

    /**
     * @param intent The intent with action {@link TelephonyIntents#SPN_STRINGS_UPDATED_ACTION}
     * @return The string to use for the plmn, or null if it should not be shown.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetTelephonyPlmnFrom(
        /* [in] */ IIntent* intent);

    /**
     * @return The default plmn (no service)
     */
    CARAPI_(AutoPtr<ICharSequence>) GetDefaultPlmn();

    /**
     * @param intent The intent with action {@link Telephony.Intents#SPN_STRINGS_UPDATED_ACTION}
     * @return The string to use for the plmn, or null if it should not be shown.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetTelephonySpnFrom(
        /* [in] */ IIntent* intent);

    CARAPI SendUpdates(
        /* [in] */ IKeyguardUpdateMonitorCallback* callback);

private:
    friend class MyHandler;

    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean DEBUG_SIM_STATES;
    static const Int32 FAILED_BIOMETRIC_UNLOCK_ATTEMPTS_BEFORE_BACKUP;
    static const Int32 LOW_BATTERY_THRESHOLD;

    static const String ACTION_FACE_UNLOCK_STARTED;
    static const String ACTION_FACE_UNLOCK_STOPPED;

    // Callback messages
    static const Int32 MSG_TIME_UPDATE;
    static const Int32 MSG_BATTERY_UPDATE;
    static const Int32 MSG_CARRIER_INFO_UPDATE;
    static const Int32 MSG_SIM_STATE_CHANGE;
    static const Int32 MSG_RINGER_MODE_CHANGED;
    static const Int32 MSG_PHONE_STATE_CHANGED;
    static const Int32 MSG_CLOCK_VISIBILITY_CHANGED;
    static const Int32 MSG_DEVICE_PROVISIONED;
    static const Int32 MSG_DPM_STATE_CHANGED;
    static const Int32 MSG_USER_SWITCHING;
    static const Int32 MSG_USER_REMOVED;
    static const Int32 MSG_KEYGUARD_VISIBILITY_CHANGED;
    static const Int32 MSG_BOOT_COMPLETED;
    static const Int32 MSG_USER_SWITCH_COMPLETE;
    static const Int32 MSG_SET_CURRENT_CLIENT_ID;
    static const Int32 MSG_SET_PLAYBACK_STATE;
    static const Int32 MSG_USER_INFO_CHANGED;
    static const Int32 MSG_REPORT_EMERGENCY_CALL_ACTION;
    static const Int32 MSG_SCREEN_TURNED_ON;
    static const Int32 MSG_SCREEN_TURNED_OFF;
    static const Int32 MSG_KEYGUARD_BOUNCER_CHANGED;
    static const Int32 MSG_FINGERPRINT_PROCESSED;
    static const Int32 MSG_FINGERPRINT_ACQUIRED;
    static const Int32 MSG_FACE_UNLOCK_STATE_CHANGED;

    static const Int64 INVALID_SUBID;

    static AutoPtr<IKeyguardUpdateMonitor> sInstance;

    AutoPtr<IContext> mContext;

    // Telephony state
    IccCardConstantsState mSimState;
    AutoPtr<ICharSequence> mTelephonyPlmn;
    AutoPtr<ICharSequence> mTelephonySpn;

    Int32 mRingMode;
    Int32 mPhoneState;
    Boolean mKeyguardIsVisible;
    Boolean mBouncer;
    Boolean mBootCompleted;

    // Device provisioning state
    Boolean mDeviceProvisioned;

    // Battery status
    AutoPtr<IKeyguardUpdateMonitorBatteryStatus> mBatteryStatus;

    // Password attempts
    Int32 mFailedAttempts;
    Int32 mFailedBiometricUnlockAttempts;

    Boolean mAlternateUnlockEnabled;

    Boolean mClockVisible;

    AutoPtr<IList> mCallbacks;
    AutoPtr<IContentObserver> mDeviceProvisionedObserver;

    Boolean mSwitchingUser;

    Boolean mScreenOn;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ISparseBooleanArray> mUserHasTrust;
    AutoPtr<ISparseBooleanArray> mUserTrustIsManaged;
    AutoPtr<ISparseBooleanArray> mUserFingerprintRecognized;
    AutoPtr<ISparseBooleanArray> mUserFaceUnlockRunning;

    AutoPtr<DisplayClientState> mDisplayClientState;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    AutoPtr<IBroadcastReceiver> mBroadcastAllReceiver;

    AutoPtr<IFingerprintManagerReceiver> mFingerprintManagerReceiver;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDUPDATEMONITOR_H__