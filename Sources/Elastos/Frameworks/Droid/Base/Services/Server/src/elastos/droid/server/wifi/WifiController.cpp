
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/server/wifi/WifiController.h"

using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Net::CNetworkInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                     WifiController::DefaultState
//=====================================================================
Boolean WifiController::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case CMD_SCREEN_ON:
    //         mAlarmManager.cancel(mIdleIntent);
    //         mScreenOff = false;
    //         mDeviceIdle = false;
    //         updateBatteryWorkSource();
    //         break;
    //     case CMD_SCREEN_OFF:
    //         mScreenOff = true;
    //         /*
    //         * Set a timer to put Wi-Fi to sleep, but only if the screen is off
    //         * AND the "stay on while plugged in" setting doesn't match the
    //         * current power conditions (i.e, not plugged in, plugged in to USB,
    //         * or plugged in to AC).
    //         */
    //         if (!shouldWifiStayAwake(mPluggedType)) {
    //             //Delayed shutdown if wifi is connected
    //             if (mNetworkInfo.getDetailedState() ==
    //                     NetworkInfo.DetailedState.CONNECTED) {
    //                 if (DBG) Slog.d(TAG, "set idle timer: " + mIdleMillis + " ms");
    //                 mAlarmManager.set(AlarmManager.RTC_WAKEUP,
    //                         System.currentTimeMillis() + mIdleMillis, mIdleIntent);
    //             } else {
    //                 sendMessage(CMD_DEVICE_IDLE);
    //             }
    //         }
    //         break;
    //     case CMD_DEVICE_IDLE:
    //         mDeviceIdle = true;
    //         updateBatteryWorkSource();
    //         break;
    //     case CMD_BATTERY_CHANGED:
    //         /*
    //         * Set a timer to put Wi-Fi to sleep, but only if the screen is off
    //         * AND we are transitioning from a state in which the device was supposed
    //         * to stay awake to a state in which it is not supposed to stay awake.
    //         * If "stay awake" state is not changing, we do nothing, to avoid resetting
    //         * the already-set timer.
    //         */
    //         int pluggedType = msg.arg1;
    //         if (DBG) Slog.d(TAG, "battery changed pluggedType: " + pluggedType);
    //         if (mScreenOff && shouldWifiStayAwake(mPluggedType) &&
    //                 !shouldWifiStayAwake(pluggedType)) {
    //             long triggerTime = System.currentTimeMillis() + mIdleMillis;
    //             if (DBG) Slog.d(TAG, "set idle timer for " + mIdleMillis + "ms");
    //             mAlarmManager.set(AlarmManager.RTC_WAKEUP, triggerTime, mIdleIntent);
    //         }
    //
    //         mPluggedType = pluggedType;
    //         break;
    //     case CMD_SET_AP:
    //     case CMD_SCAN_ALWAYS_MODE_CHANGED:
    //     case CMD_LOCKS_CHANGED:
    //     case CMD_WIFI_TOGGLED:
    //     case CMD_AIRPLANE_TOGGLED:
    //     case CMD_EMERGENCY_MODE_CHANGED:
    //         break;
    //     case CMD_USER_PRESENT:
    //         mFirstUserSignOnSeen = true;
    //         break;
    //     case CMD_DEFERRED_TOGGLE:
    //         log("DEFERRED_TOGGLE ignored due to state change");
    //         break;
    //     default:
    //         throw new RuntimeException("WifiController.handleMessage " + msg.what);
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//                  WifiController::ApStaDisabledState
//=====================================================================
ECode WifiController::ApStaDisabledState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setSupplicantRunning(false);
    // // Supplicant can't restart right away, so not the time we switched off
    // mDisabledTimestamp = SystemClock.elapsedRealtime();
    // mDeferredEnableSerialNumber++;
    // mHaveDeferredEnable = false;
    assert(0);
    return NOERROR;
}

Boolean WifiController::ApStaDisabledState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case CMD_WIFI_TOGGLED:
    //     case CMD_AIRPLANE_TOGGLED:
    //         if (mSettingsStore.isWifiToggleEnabled()) {
    //             if (doDeferEnable(msg)) {
    //                 if (mHaveDeferredEnable) {
    //                     //  have 2 toggles now, inc serial number an ignore both
    //                     mDeferredEnableSerialNumber++;
    //                 }
    //                 mHaveDeferredEnable = !mHaveDeferredEnable;
    //                 break;
    //             }
    //             if (mDeviceIdle == false) {
    //                 checkLocksAndTransitionWhenDeviceActive();
    //             } else {
    //                 checkLocksAndTransitionWhenDeviceIdle();
    //             }
    //         } else if (mSettingsStore.isScanAlwaysAvailable()) {
    //             transitionTo(mStaDisabledWithScanState);
    //         }
    //         break;
    //     case CMD_SCAN_ALWAYS_MODE_CHANGED:
    //         if (mSettingsStore.isScanAlwaysAvailable()) {
    //             transitionTo(mStaDisabledWithScanState);
    //         }
    //         break;
    //     case CMD_SET_AP:
    //         if (msg.arg1 == 1) {
    //             mWifiStateMachine.setHostApRunning((WifiConfiguration) msg.obj,
    //                     true);
    //             transitionTo(mApEnabledState);
    //         }
    //         break;
    //     case CMD_DEFERRED_TOGGLE:
    //         if (msg.arg1 != mDeferredEnableSerialNumber) {
    //             log("DEFERRED_TOGGLE ignored due to serial mismatch");
    //             break;
    //         }
    //         log("DEFERRED_TOGGLE handled");
    //         sendMessage((Message)(msg.obj));
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

Boolean WifiController::ApStaDisabledState::DoDeferEnable(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // long delaySoFar = SystemClock.elapsedRealtime() - mDisabledTimestamp;
    // if (delaySoFar >= mReEnableDelayMillis) {
    //     return false;
    // }
    //
    // log("WifiController msg " + msg + " deferred for " +
    //         (mReEnableDelayMillis - delaySoFar) + "ms");
    //
    // // need to defer this action.
    // Message deferredMsg = obtainMessage(CMD_DEFERRED_TOGGLE);
    // deferredMsg.obj = Message.obtain(msg);
    // deferredMsg.arg1 = ++mDeferredEnableSerialNumber;
    // sendMessageDelayed(deferredMsg, mReEnableDelayMillis - delaySoFar + DEFER_MARGIN_MS);
    // return true;
    assert(0);
    return FALSE;
}

//=====================================================================
//                   WifiController::StaEnabledState
//=====================================================================
ECode WifiController::StaEnabledState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setSupplicantRunning(true);
    assert(0);
    return NOERROR;
}

Boolean WifiController::StaEnabledState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case CMD_WIFI_TOGGLED:
    //         int mWifiState = mWifiStateMachine.syncGetWifiState();
    //         if (! mSettingsStore.isWifiToggleEnabled()) {
    //             if (mSettingsStore.isScanAlwaysAvailable()) {
    //                 transitionTo(mStaDisabledWithScanState);
    //             } else {
    //                 transitionTo(mApStaDisabledState);
    //             }
    //         }
    //         if ((mWifiState != WIFI_STATE_ENABLING) &&
    //             (mWifiState != WIFI_STATE_ENABLED)) {
    //             if (DBG) {
    //                 Slog.d(TAG, "Mismatch in the state " + mWifiState);
    //             }
    //             mWifiStateMachine.setSupplicantRunning(true);
    //         }
    //         break;
    //     case CMD_AIRPLANE_TOGGLED:
    //         /* When wi-fi is turned off due to airplane,
    //         * disable entirely (including scan)
    //         */
    //         if (! mSettingsStore.isWifiToggleEnabled()) {
    //             transitionTo(mApStaDisabledState);
    //         }
    //         break;
    //     case CMD_EMERGENCY_MODE_CHANGED:
    //         if (msg.arg1 == 1) {
    //             transitionTo(mEcmState);
    //             break;
    //         }
    //     default:
    //         return NOT_HANDLED;
    //
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//               WifiController::StaDisabledWithScanState
//=====================================================================
ECode WifiController::StaDisabledWithScanState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setSupplicantRunning(true);
    // mWifiStateMachine.setOperationalMode(WifiStateMachine.SCAN_ONLY_WITH_WIFI_OFF_MODE);
    // mWifiStateMachine.setDriverStart(true);
    // // Supplicant can't restart right away, so not the time we switched off
    // mDisabledTimestamp = SystemClock.elapsedRealtime();
    // mDeferredEnableSerialNumber++;
    // mHaveDeferredEnable = false;
    assert(0);
    return NOERROR;
}

Boolean WifiController::StaDisabledWithScanState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case CMD_WIFI_TOGGLED:
    //         if (mSettingsStore.isWifiToggleEnabled()) {
    //             if (doDeferEnable(msg)) {
    //                 if (mHaveDeferredEnable) {
    //                     // have 2 toggles now, inc serial number and ignore both
    //                     mDeferredEnableSerialNumber++;
    //                 }
    //                 mHaveDeferredEnable = !mHaveDeferredEnable;
    //                 break;
    //             }
    //             if (mDeviceIdle == false) {
    //                 checkLocksAndTransitionWhenDeviceActive();
    //             } else {
    //                 checkLocksAndTransitionWhenDeviceIdle();
    //             }
    //         }
    //         break;
    //     case CMD_AIRPLANE_TOGGLED:
    //         if (mSettingsStore.isAirplaneModeOn() &&
    //                 ! mSettingsStore.isWifiToggleEnabled()) {
    //             transitionTo(mApStaDisabledState);
    //         }
    //     case CMD_SCAN_ALWAYS_MODE_CHANGED:
    //         if (! mSettingsStore.isScanAlwaysAvailable()) {
    //             transitionTo(mApStaDisabledState);
    //         }
    //         break;
    //     case CMD_SET_AP:
    //         // Before starting tethering, turn off supplicant for scan mode
    //         if (msg.arg1 == 1) {
    //             deferMessage(msg);
    //             transitionTo(mApStaDisabledState);
    //         }
    //         break;
    //     case CMD_DEFERRED_TOGGLE:
    //         if (msg.arg1 != mDeferredEnableSerialNumber) {
    //             log("DEFERRED_TOGGLE ignored due to serial mismatch");
    //             break;
    //         }
    //         logd("DEFERRED_TOGGLE handled");
    //         sendMessage((Message)(msg.obj));
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

Boolean WifiController::StaDisabledWithScanState::DoDeferEnable(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // long delaySoFar = SystemClock.elapsedRealtime() - mDisabledTimestamp;
    // if (delaySoFar >= mReEnableDelayMillis) {
    //     return false;
    // }
    //
    // log("WifiController msg " + msg + " deferred for " +
    //         (mReEnableDelayMillis - delaySoFar) + "ms");
    //
    // // need to defer this action.
    // Message deferredMsg = obtainMessage(CMD_DEFERRED_TOGGLE);
    // deferredMsg.obj = Message.obtain(msg);
    // deferredMsg.arg1 = ++mDeferredEnableSerialNumber;
    // sendMessageDelayed(deferredMsg, mReEnableDelayMillis - delaySoFar + DEFER_MARGIN_MS);
    // return true;
    assert(0);
    return FALSE;
}

//=====================================================================
//                    WifiController::ApEnabledState
//=====================================================================
Boolean WifiController::ApEnabledState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case CMD_AIRPLANE_TOGGLED:
    //         if (mSettingsStore.isAirplaneModeOn()) {
    //             mWifiStateMachine.setHostApRunning(null, false);
    //             transitionTo(mApStaDisabledState);
    //         }
    //         break;
    //     case CMD_SET_AP:
    //         if (msg.arg1 == 0) {
    //             mWifiStateMachine.setHostApRunning(null, false);
    //             transitionTo(mApStaDisabledState);
    //         }
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//                       WifiController::EcmState
//=====================================================================
ECode WifiController::EcmState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setSupplicantRunning(false);
    assert(0);
    return NOERROR;
}

Boolean WifiController::EcmState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (msg.what == CMD_EMERGENCY_MODE_CHANGED && msg.arg1 == 0) {
    //     if (mSettingsStore.isWifiToggleEnabled()) {
    //         if (mDeviceIdle == false) {
    //             checkLocksAndTransitionWhenDeviceActive();
    //         } else {
    //             checkLocksAndTransitionWhenDeviceIdle();
    //         }
    //     } else if (mSettingsStore.isScanAlwaysAvailable()) {
    //         transitionTo(mStaDisabledWithScanState);
    //     } else {
    //         transitionTo(mApStaDisabledState);
    //     }
    //     return HANDLED;
    // } else {
    //     return NOT_HANDLED;
    // }
    assert(0);
    return FALSE;
}

//=====================================================================
//                  WifiController::DeviceActiveState
//=====================================================================
ECode WifiController::DeviceActiveState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setOperationalMode(WifiStateMachine.CONNECT_MODE);
    // mWifiStateMachine.setDriverStart(true);
    // mWifiStateMachine.setHighPerfModeEnabled(false);
    assert(0);
    return NOERROR;
}

Boolean WifiController::DeviceActiveState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (msg.what == CMD_DEVICE_IDLE) {
    //     checkLocksAndTransitionWhenDeviceIdle();
    //     // We let default state handle the rest of work
    // } else if (msg.what == CMD_LOCKS_CHANGED) {
    //     checkLocksAndTransitionWhenDeviceActive();
    //     return HANDLED;
    // } else if (msg.what == CMD_USER_PRESENT) {
    //     // TLS networks can't connect until user unlocks keystore. KeyStore
    //     // unlocks when the user punches PIN after the reboot. So use this
    //     // trigger to get those networks connected.
    //     if (mFirstUserSignOnSeen == false) {
    //         mWifiStateMachine.reloadTlsNetworksAndReconnect();
    //     }
    //     mFirstUserSignOnSeen = true;
    //     return HANDLED;
    // }
    // return NOT_HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//              WifiController::DeviceActiveHighPerfState
//=====================================================================
ECode WifiController::DeviceActiveHighPerfState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setOperationalMode(WifiStateMachine.CONNECT_MODE);
    // mWifiStateMachine.setDriverStart(true);
    // mWifiStateMachine.setHighPerfModeEnabled(true);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                 WifiController::DeviceInactiveState
//=====================================================================
Boolean WifiController::DeviceInactiveState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case CMD_LOCKS_CHANGED:
    //         checkLocksAndTransitionWhenDeviceIdle();
    //         updateBatteryWorkSource();
    //         return HANDLED;
    //     case CMD_SCREEN_ON:
    //         checkLocksAndTransitionWhenDeviceActive();
    //         // More work in default state
    //         return NOT_HANDLED;
    //     default:
    //         return NOT_HANDLED;
    // }
    assert(0);
    return FALSE;
}

//=====================================================================
//                WifiController::ScanOnlyLockHeldState
//=====================================================================
ECode WifiController::ScanOnlyLockHeldState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setOperationalMode(WifiStateMachine.SCAN_ONLY_MODE);
    // mWifiStateMachine.setDriverStart(true);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                  WifiController::FullLockHeldState
//=====================================================================
ECode WifiController::FullLockHeldState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setOperationalMode(WifiStateMachine.CONNECT_MODE);
    // mWifiStateMachine.setDriverStart(true);
    // mWifiStateMachine.setHighPerfModeEnabled(false);
    assert(0);
    return NOERROR;
}

//=====================================================================
//              WifiController::FullHighPerfLockHeldState
//=====================================================================
ECode WifiController::FullHighPerfLockHeldState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setOperationalMode(WifiStateMachine.CONNECT_MODE);
    // mWifiStateMachine.setDriverStart(true);
    // mWifiStateMachine.setHighPerfModeEnabled(true);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                   WifiController::NoLockHeldState
//=====================================================================
ECode WifiController::NoLockHeldState::Enter()
{
    // ==================before translated======================
    // mWifiStateMachine.setDriverStart(false);
    assert(0);
    return NOERROR;
}

//=====================================================================
//               WifiController::InnerBroadcastReceiver1
//=====================================================================
WifiController::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiController* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiController::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // String action = intent.getAction();
    // if (action.equals(ACTION_DEVICE_IDLE)) {
    //     sendMessage(CMD_DEVICE_IDLE);
    // } else if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
    //     mNetworkInfo = (NetworkInfo) intent.getParcelableExtra(
    //             WifiManager.EXTRA_NETWORK_INFO);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                WifiController::InnerContentObserver1
//=====================================================================
WifiController::InnerContentObserver1::InnerContentObserver1(
    /* [in] */ WifiController* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiController::InnerContentObserver1::OnChange(
    /* [in] */ Boolean selfChange)
{
    // ==================before translated======================
    // readStayAwakeConditions();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                WifiController::InnerContentObserver3
//=====================================================================
WifiController::InnerContentObserver3::InnerContentObserver3(
    /* [in] */ WifiController* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiController::InnerContentObserver3::OnChange(
    /* [in] */ Boolean selfChange)
{
    // ==================before translated======================
    // readWifiIdleTime();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                WifiController::InnerContentObserver5
//=====================================================================
WifiController::InnerContentObserver5::InnerContentObserver5(
    /* [in] */ WifiController* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiController::InnerContentObserver5::OnChange(
    /* [in] */ Boolean selfChange)
{
    // ==================before translated======================
    // readWifiSleepPolicy();
    assert(0);
    return NOERROR;
}

//=====================================================================
//                            WifiController
//=====================================================================
const Int32 WifiController::CMD_EMERGENCY_MODE_CHANGED;
const Int32 WifiController::CMD_SCREEN_ON;
const Int32 WifiController::CMD_SCREEN_OFF;
const Int32 WifiController::CMD_BATTERY_CHANGED;
const Int32 WifiController::CMD_DEVICE_IDLE;
const Int32 WifiController::CMD_LOCKS_CHANGED;
const Int32 WifiController::CMD_SCAN_ALWAYS_MODE_CHANGED;
const Int32 WifiController::CMD_WIFI_TOGGLED;
const Int32 WifiController::CMD_AIRPLANE_TOGGLED;
const Int32 WifiController::CMD_SET_AP;
const Int32 WifiController::CMD_DEFERRED_TOGGLE;
const Int32 WifiController::CMD_USER_PRESENT;
const String WifiController::TAG("WifiController");
const Boolean WifiController::DBG = false;
const Int32 WifiController::IDLE_REQUEST;
const Int64 WifiController::DEFAULT_IDLE_MS;
const Int64 WifiController::DEFAULT_REENABLE_DELAY_MS;
const Int64 WifiController::DEFER_MARGIN_MS;
const String WifiController::ACTION_DEVICE_IDLE("com.android.server.WifiManager.action.DEVICE_IDLE");
const Int32 WifiController::BASE;

WifiController::WifiController(
    /* [in] */ IContext* context,
    /* [in] */ WifiServiceImpl* service,
    /* [in] */ ILooper* looper)
{
    CNetworkInfo::New(IConnectivityManager::TYPE_WIFI, 0, String("WIFI"), String(""), (INetworkInfo**)&mNetworkInfo);
    CWorkSource::New((IWorkSource**)&mTmpWorkSource);
    mDefaultState = new DefaultState();
    mStaEnabledState = new StaEnabledState();
    mApStaDisabledState = new ApStaDisabledState();
    mStaDisabledWithScanState = new StaDisabledWithScanState();
    mApEnabledState = new ApEnabledState();
    mDeviceActiveState = new DeviceActiveState();
    mDeviceActiveHighPerfState = new DeviceActiveHighPerfState();
    mDeviceInactiveState = new DeviceInactiveState();
    mScanOnlyLockHeldState = new ScanOnlyLockHeldState();
    mFullLockHeldState = new FullLockHeldState();
    mFullHighPerfLockHeldState = new FullHighPerfLockHeldState();
    mNoLockHeldState = new NoLockHeldState();
    mEcmState = new EcmState();
    // ==================before translated======================
    // super(TAG, looper);
    // mContext = context;
    // mWifiStateMachine = service.mWifiStateMachine;
    // mSettingsStore = service.mSettingsStore;
    // mLocks = service.mLocks;
    //
    // mAlarmManager = (AlarmManager)mContext.getSystemService(Context.ALARM_SERVICE);
    // Intent idleIntent = new Intent(ACTION_DEVICE_IDLE, null);
    // mIdleIntent = PendingIntent.getBroadcast(mContext, IDLE_REQUEST, idleIntent, 0);
    //
    // addState(mDefaultState);
    //     addState(mApStaDisabledState, mDefaultState);
    //     addState(mStaEnabledState, mDefaultState);
    //         addState(mDeviceActiveState, mStaEnabledState);
    //             addState(mDeviceActiveHighPerfState, mDeviceActiveState);
    //         addState(mDeviceInactiveState, mStaEnabledState);
    //             addState(mScanOnlyLockHeldState, mDeviceInactiveState);
    //             addState(mFullLockHeldState, mDeviceInactiveState);
    //             addState(mFullHighPerfLockHeldState, mDeviceInactiveState);
    //             addState(mNoLockHeldState, mDeviceInactiveState);
    //     addState(mStaDisabledWithScanState, mDefaultState);
    //     addState(mApEnabledState, mDefaultState);
    //     addState(mEcmState, mDefaultState);
    //
    // boolean isAirplaneModeOn = mSettingsStore.isAirplaneModeOn();
    // boolean isWifiEnabled = mSettingsStore.isWifiToggleEnabled();
    // boolean isScanningAlwaysAvailable = mSettingsStore.isScanAlwaysAvailable();
    //
    // log("isAirplaneModeOn = " + isAirplaneModeOn +
    //         ", isWifiEnabled = " + isWifiEnabled +
    //         ", isScanningAvailable = " + isScanningAlwaysAvailable);
    //
    // if (isScanningAlwaysAvailable) {
    //     setInitialState(mStaDisabledWithScanState);
    // } else {
    //     setInitialState(mApStaDisabledState);
    // }
    //
    // setLogRecSize(100);
    // setLogOnlyTransitions(false);
    //
    // IntentFilter filter = new IntentFilter();
    // filter.addAction(ACTION_DEVICE_IDLE);
    // filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
    // mContext.registerReceiver(
    //         new BroadcastReceiver() {
    //             @Override
    //             public void onReceive(Context context, Intent intent) {
    //                 String action = intent.getAction();
    //                 if (action.equals(ACTION_DEVICE_IDLE)) {
    //                     sendMessage(CMD_DEVICE_IDLE);
    //                 } else if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
    //                     mNetworkInfo = (NetworkInfo) intent.getParcelableExtra(
    //                             WifiManager.EXTRA_NETWORK_INFO);
    //                 }
    //             }
    //         },
    //         new IntentFilter(filter));
    //
    // initializeAndRegisterForSettingsChange(looper);
}

ECode WifiController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    // ==================before translated======================
    // super.dump(fd, pw, args);
    //
    // pw.println("mScreenOff " + mScreenOff);
    // pw.println("mDeviceIdle " + mDeviceIdle);
    // pw.println("mPluggedType " + mPluggedType);
    // pw.println("mIdleMillis " + mIdleMillis);
    // pw.println("mSleepPolicy " + mSleepPolicy);
    assert(0);
    return NOERROR;
}

void WifiController::InitializeAndRegisterForSettingsChange(
    /* [in] */ ILooper* looper)
{
    // ==================before translated======================
    // Handler handler = new Handler(looper);
    // readStayAwakeConditions();
    // registerForStayAwakeModeChange(handler);
    // readWifiIdleTime();
    // registerForWifiIdleTimeChange(handler);
    // readWifiSleepPolicy();
    // registerForWifiSleepPolicyChange(handler);
    // readWifiReEnableDelay();
    assert(0);
}

void WifiController::ReadStayAwakeConditions()
{
    // ==================before translated======================
    // mStayAwakeConditions = Settings.Global.getInt(mContext.getContentResolver(),
    //         Settings.Global.STAY_ON_WHILE_PLUGGED_IN, 0);
    assert(0);
}

void WifiController::ReadWifiIdleTime()
{
    // ==================before translated======================
    // mIdleMillis = Settings.Global.getLong(mContext.getContentResolver(),
    //         Settings.Global.WIFI_IDLE_MS, DEFAULT_IDLE_MS);
    assert(0);
}

void WifiController::ReadWifiSleepPolicy()
{
    // ==================before translated======================
    // mSleepPolicy = Settings.Global.getInt(mContext.getContentResolver(),
    //         Settings.Global.WIFI_SLEEP_POLICY,
    //         Settings.Global.WIFI_SLEEP_POLICY_NEVER);
    assert(0);
}

void WifiController::ReadWifiReEnableDelay()
{
    // ==================before translated======================
    // mReEnableDelayMillis = Settings.Global.getLong(mContext.getContentResolver(),
    //         Settings.Global.WIFI_REENABLE_DELAY_MS, DEFAULT_REENABLE_DELAY_MS);
    assert(0);
}

void WifiController::RegisterForStayAwakeModeChange(
    /* [in] */ IHandler* handler)
{
    // ==================before translated======================
    // ContentObserver contentObserver = new ContentObserver(handler) {
    //     @Override
    //     public void onChange(boolean selfChange) {
    //         readStayAwakeConditions();
    //     }
    // };
    //
    // mContext.getContentResolver().registerContentObserver(
    //         Settings.Global.getUriFor(Settings.Global.STAY_ON_WHILE_PLUGGED_IN),
    //         false, contentObserver);
    assert(0);
}

void WifiController::RegisterForWifiIdleTimeChange(
    /* [in] */ IHandler* handler)
{
    // ==================before translated======================
    // ContentObserver contentObserver = new ContentObserver(handler) {
    //     @Override
    //     public void onChange(boolean selfChange) {
    //         readWifiIdleTime();
    //     }
    // };
    //
    // mContext.getContentResolver().registerContentObserver(
    //         Settings.Global.getUriFor(Settings.Global.WIFI_IDLE_MS),
    //         false, contentObserver);
    assert(0);
}

void WifiController::RegisterForWifiSleepPolicyChange(
    /* [in] */ IHandler* handler)
{
    // ==================before translated======================
    // ContentObserver contentObserver = new ContentObserver(handler) {
    //     @Override
    //     public void onChange(boolean selfChange) {
    //         readWifiSleepPolicy();
    //     }
    // };
    // mContext.getContentResolver().registerContentObserver(
    //         Settings.Global.getUriFor(Settings.Global.WIFI_SLEEP_POLICY),
    //         false, contentObserver);
    assert(0);
}

Boolean WifiController::ShouldWifiStayAwake(
    /* [in] */ Int32 pluggedType)
{
    // ==================before translated======================
    // if (mSleepPolicy == Settings.Global.WIFI_SLEEP_POLICY_NEVER) {
    //     // Never sleep
    //     return true;
    // } else if ((mSleepPolicy == Settings.Global.WIFI_SLEEP_POLICY_NEVER_WHILE_PLUGGED) &&
    //         (pluggedType != 0)) {
    //     // Never sleep while plugged, and we're plugged
    //     return true;
    // } else {
    //     // Default
    //     return shouldDeviceStayAwake(pluggedType);
    // }
    assert(0);
    return FALSE;
}

Boolean WifiController::ShouldDeviceStayAwake(
    /* [in] */ Int32 pluggedType)
{
    // ==================before translated======================
    // return (mStayAwakeConditions & pluggedType) != 0;
    assert(0);
    return FALSE;
}

void WifiController::UpdateBatteryWorkSource()
{
    // ==================before translated======================
    // mTmpWorkSource.clear();
    // if (mDeviceIdle) {
    //     mLocks.updateWorkSource(mTmpWorkSource);
    // }
    // mWifiStateMachine.updateBatteryWorkSource(mTmpWorkSource);
    assert(0);
}

void WifiController::CheckLocksAndTransitionWhenDeviceActive()
{
    // ==================before translated======================
    // if (mLocks.hasLocks() && mLocks.getStrongestLockMode() == WIFI_MODE_FULL_HIGH_PERF) {
    //     // It is possible for the screen to be off while the device is
    //     // is active (mIdleMillis), so we need the high-perf mode
    //     // otherwise powersaving mode will be turned on.
    //     transitionTo(mDeviceActiveHighPerfState);
    // } else {
    //     transitionTo(mDeviceActiveState);
    // }
    assert(0);
}

void WifiController::CheckLocksAndTransitionWhenDeviceIdle()
{
    // ==================before translated======================
    // if (mLocks.hasLocks()) {
    //     switch (mLocks.getStrongestLockMode()) {
    //         case WIFI_MODE_FULL:
    //             transitionTo(mFullLockHeldState);
    //             break;
    //         case WIFI_MODE_FULL_HIGH_PERF:
    //             transitionTo(mFullHighPerfLockHeldState);
    //             break;
    //         case WIFI_MODE_SCAN_ONLY:
    //             transitionTo(mScanOnlyLockHeldState);
    //             break;
    //         default:
    //             loge("Illegal lock " + mLocks.getStrongestLockMode());
    //     }
    // } else {
    //     if (mSettingsStore.isScanAlwaysAvailable()) {
    //         transitionTo(mScanOnlyLockHeldState);
    //     } else {
    //         transitionTo(mNoLockHeldState);
    //     }
    // }
    assert(0);
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


