#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/server/wifi/WifiController.h"
#include "elastos/droid/server/wifi/WifiServiceImpl.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                     WifiController::DefaultState
//=====================================================================
ECode WifiController::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::DefaultState", "what: %d", what);
    switch (what) {
        case CMD_SCREEN_ON:
            mOwner->mAlarmManager->Cancel(mOwner->mIdleIntent);
            mOwner->mScreenOff = FALSE;
            mOwner->mDeviceIdle = false;
            mOwner->UpdateBatteryWorkSource();
            break;
        case CMD_SCREEN_OFF:
            mOwner->mScreenOff = TRUE;
            /*
            * Set a timer to put Wi-Fi to sleep, but only if the screen is off
            * AND the "stay on while plugged in" setting doesn't match the
            * current power conditions (i.e, not plugged in, plugged in to USB,
            * or plugged in to AC).
            */
            if (!mOwner->ShouldWifiStayAwake(mOwner->mPluggedType)) {
                //Delayed shutdown if wifi is connected
                NetworkInfoDetailedState nidState;
                mOwner->mNetworkInfo->GetDetailedState(&nidState);
                if (nidState == Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED) {
                    if (DBG) Logger::D(TAG, "set idle timer: %d ms", mOwner->mIdleMillis);
                    AutoPtr<ISystem> system;
                    CSystem::AcquireSingleton((ISystem**)&system);
                    Int64 currentTimeMillis;
                    system->GetCurrentTimeMillis(&currentTimeMillis);
                    mOwner->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP,
                            currentTimeMillis + mOwner->mIdleMillis, mOwner->mIdleIntent);
                } else {
                    mOwner->SendMessage(CMD_DEVICE_IDLE);
                }
            }
            break;
        case CMD_DEVICE_IDLE:
            mOwner->mDeviceIdle = TRUE;
            mOwner->UpdateBatteryWorkSource();
            break;
        case CMD_BATTERY_CHANGED: {
            /*
            * Set a timer to put Wi-Fi to sleep, but only if the screen is off
            * AND we are transitioning from a state in which the device was supposed
            * to stay awake to a state in which it is not supposed to stay awake.
            * If "stay awake" state is not changing, we do nothing, to avoid resetting
            * the already-set timer.
            */
            Int32 pluggedType;
            msg->GetArg1(&pluggedType);
            if (DBG) Logger::D(TAG, "battery changed pluggedType: %d", pluggedType);
            if (mOwner->mScreenOff && mOwner->ShouldWifiStayAwake(mOwner->mPluggedType) &&
                    !mOwner->ShouldWifiStayAwake(pluggedType)) {
                AutoPtr<ISystem> system;
                CSystem::AcquireSingleton((ISystem**)&system);
                Int64 currentTimeMillis;
                system->GetCurrentTimeMillis(&currentTimeMillis);
                Int64 triggerTime = currentTimeMillis + mOwner->mIdleMillis;
                if (DBG) Logger::D(TAG, "set idle timer for %d ms", mOwner->mIdleMillis);
                mOwner->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, triggerTime, mOwner->mIdleIntent);
            }

            mOwner->mPluggedType = pluggedType;
            break;
        }
        case CMD_SET_AP:
        case CMD_SCAN_ALWAYS_MODE_CHANGED:
        case CMD_LOCKS_CHANGED:
        case CMD_WIFI_TOGGLED:
        case CMD_AIRPLANE_TOGGLED:
        case CMD_EMERGENCY_MODE_CHANGED:
            break;
        case CMD_USER_PRESENT:
            mOwner->mFirstUserSignOnSeen = TRUE;
            break;
        case CMD_DEFERRED_TOGGLE:
            Logger::D("WifiController::DefaultState", "DEFERRED_TOGGLE ignored due to state change");
            break;
        default:
            //throw new RuntimeException("WifiController.handleMessage " + msg.what);
            Logger::E("WifiController::DefaultState", "ProcessMessage: %d", what);
    }
    *result = TRUE;
    return NOERROR;
}

//=====================================================================
//                  WifiController::ApStaDisabledState
//=====================================================================
ECode WifiController::ApStaDisabledState::Enter()
{
    if (DBG) Logger::D("WifiController", "ApStaDisabledState::Enter\n");
    mOwner->mWifiStateMachine->SetSupplicantRunning(FALSE);
    // Supplicant can't restart right away, so not the time we switched off
    mDisabledTimestamp = SystemClock::GetElapsedRealtime();
    mDeferredEnableSerialNumber++;
    mHaveDeferredEnable = FALSE;
    return NOERROR;
}

ECode WifiController::ApStaDisabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController", "ApStaDisabledState what: %d", what);
    switch (what) {
        case CMD_WIFI_TOGGLED:
        case CMD_AIRPLANE_TOGGLED: {
            Boolean isWifiToggleEnabled;
            Boolean isScanAlwaysAvailable;
            if (mOwner->mSettingsStore->IsWifiToggleEnabled(&isWifiToggleEnabled), isWifiToggleEnabled) {
                if (DoDeferEnable(msg)) {
                    if (mHaveDeferredEnable) {
                        //  have 2 toggles now, inc serial number an ignore both
                        mDeferredEnableSerialNumber++;
                    }
                    mHaveDeferredEnable = !mHaveDeferredEnable;
                    break;
                }
                if (mOwner->mDeviceIdle == FALSE) {
                    mOwner->CheckLocksAndTransitionWhenDeviceActive();
                } else {
                    mOwner->CheckLocksAndTransitionWhenDeviceIdle();
                }
            } else if (mOwner->mSettingsStore->IsScanAlwaysAvailable(&isScanAlwaysAvailable), isScanAlwaysAvailable) {
                mOwner->TransitionTo(mOwner->mStaDisabledWithScanState);
            }
            break;
        }
        case CMD_SCAN_ALWAYS_MODE_CHANGED: {
            Boolean isScanAlwaysAvailable;
            if (mOwner->mSettingsStore->IsScanAlwaysAvailable(&isScanAlwaysAvailable), isScanAlwaysAvailable) {
                mOwner->TransitionTo(mOwner->mStaDisabledWithScanState);
            }
            break;
        }
        case CMD_SET_AP: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                IWifiConfiguration* wfConfig = IWifiConfiguration::Probe(obj);
                mOwner->mWifiStateMachine->SetHostApRunning(wfConfig, TRUE);
                mOwner->TransitionTo(mOwner->mApEnabledState);
            }
            break;
        }
        case CMD_DEFERRED_TOGGLE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 != mDeferredEnableSerialNumber) {
                Logger::D("WifiController", "ApStaDisabledState CMD_DEFERRED_TOGGLE ignored due to serial mismatch");
                break;
            }
            Logger::D("WifiController", "ApStaDisabledState CMD_DEFERRED_TOGGLE handled");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMessage* message = IMessage::Probe(obj);
            mOwner->SendMessage(message);
            break;
        }
        default: {
            *result = FALSE;//NOT_HANDLED;
            return NOERROR;
        }
    }
    *result = TRUE;//HANDLED;
    return NOERROR;
}

Boolean WifiController::ApStaDisabledState::DoDeferEnable(
    /* [in] */ IMessage* msg)
{
    Int64 delaySoFar = SystemClock::GetElapsedRealtime() - mDisabledTimestamp;
    if (delaySoFar >= mOwner->mReEnableDelayMillis) {
        return FALSE;
    }

    Logger::D("WifiController::ApStaDisabledState", "WifiController msg xx deferred for %lld ms",
            (mOwner->mReEnableDelayMillis - delaySoFar));

    // need to defer this action.
    AutoPtr<IMessage> deferredMsg;
    mOwner->ObtainMessage(CMD_DEFERRED_TOGGLE, (IMessage**)&deferredMsg);
    AutoPtr<IMessage> message;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain(msg, (IMessage**)&message);

    deferredMsg->SetObj(message);
    deferredMsg->SetArg1(++mDeferredEnableSerialNumber);
    mOwner->SendMessageDelayed(deferredMsg, mOwner->mReEnableDelayMillis - delaySoFar + DEFER_MARGIN_MS);
    return TRUE;
}

//=====================================================================
//                   WifiController::StaEnabledState
//=====================================================================
ECode WifiController::StaEnabledState::Enter()
{
    mOwner->mWifiStateMachine->SetSupplicantRunning(TRUE);
    return NOERROR;
}

ECode WifiController::StaEnabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::StaEnabledState", "what: %d", what);
    switch (what) {
        case CMD_WIFI_TOGGLED: {
            Int32 mWifiState;
            mOwner->mWifiStateMachine->SyncGetWifiState(&mWifiState);
            Boolean isWifiToggleEnabled;
            if (!(mOwner->mSettingsStore->IsWifiToggleEnabled(&isWifiToggleEnabled), isWifiToggleEnabled)) {
                Boolean isScanAlwaysAvailable;
                if (mOwner->mSettingsStore->IsScanAlwaysAvailable(&isScanAlwaysAvailable), isScanAlwaysAvailable) {
                    mOwner->TransitionTo(mOwner->mStaDisabledWithScanState);
                } else {
                    mOwner->TransitionTo(mOwner->mApStaDisabledState);
                }
            }
            if ((mWifiState != IWifiManager::WIFI_STATE_ENABLING) &&
                (mWifiState != IWifiManager::WIFI_STATE_ENABLED)) {
                if (DBG) {
                    Logger::D(TAG, "Mismatch in the state %d", mWifiState);
                }
                mOwner->mWifiStateMachine->SetSupplicantRunning(TRUE);
            }
            break;
        }
        case CMD_AIRPLANE_TOGGLED: {
            /* When wi-fi is turned off due to airplane,
            * disable entirely (including scan)
            */
            Boolean isWifiToggleEnabled;
            if (!(mOwner->mSettingsStore->IsWifiToggleEnabled(&isWifiToggleEnabled), isWifiToggleEnabled)) {
                mOwner->TransitionTo(mOwner->mApStaDisabledState);
            }
            break;
        }
        case CMD_EMERGENCY_MODE_CHANGED: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                mOwner->TransitionTo(mOwner->mEcmState);
                break;
            }
        }
        default:
            *result = FALSE;//NOT_HANDLED;
            return NOERROR;

    }
    *result = TRUE;//HANDLED;
    return NOERROR;
}

//=====================================================================
//               WifiController::StaDisabledWithScanState
//=====================================================================
ECode WifiController::StaDisabledWithScanState::Enter()
{
    mOwner->mWifiStateMachine->SetSupplicantRunning(TRUE);
    mOwner->mWifiStateMachine->SetOperationalMode(WifiStateMachine::SCAN_ONLY_WITH_WIFI_OFF_MODE);
    mOwner->mWifiStateMachine->SetDriverStart(TRUE);
    // Supplicant can't restart right away, so not the time we switched off
    mDisabledTimestamp = SystemClock::GetElapsedRealtime();
    mDeferredEnableSerialNumber++;
    mHaveDeferredEnable = FALSE;
    return NOERROR;
}

ECode WifiController::StaDisabledWithScanState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::StaDisabledWithScanState", "what: %d", what);
    switch (what) {
        case CMD_WIFI_TOGGLED: {
            Boolean isWifiToggleEnabled;
            if (mOwner->mSettingsStore->IsWifiToggleEnabled(&isWifiToggleEnabled), isWifiToggleEnabled) {
                if (DoDeferEnable(msg)) {
                    if (mHaveDeferredEnable) {
                        // have 2 toggles now, inc serial number and ignore both
                        mDeferredEnableSerialNumber++;
                    }
                    mHaveDeferredEnable = !mHaveDeferredEnable;
                    break;
                }
                if (mOwner->mDeviceIdle == FALSE) {
                    mOwner->CheckLocksAndTransitionWhenDeviceActive();
                } else {
                    mOwner->CheckLocksAndTransitionWhenDeviceIdle();
                }
            }
            break;
        }
        case CMD_AIRPLANE_TOGGLED: {
            Boolean isAirplaneModeOn;
            Boolean isWifiToggleEnabled;
            if ((mOwner->mSettingsStore->IsAirplaneModeOn(&isAirplaneModeOn), isAirplaneModeOn) &&
                    ! (mOwner->mSettingsStore->IsWifiToggleEnabled(&isWifiToggleEnabled), isWifiToggleEnabled)) {
                mOwner->TransitionTo(mOwner->mApStaDisabledState);
            }
        }
        case CMD_SCAN_ALWAYS_MODE_CHANGED: {
            Boolean isScanAlwaysAvailable;
            if (! (mOwner->mSettingsStore->IsScanAlwaysAvailable(&isScanAlwaysAvailable), isScanAlwaysAvailable)) {
                mOwner->TransitionTo(mOwner->mApStaDisabledState);
            }
            break;
        }
        case CMD_SET_AP: {
            // Before starting tethering, turn off supplicant for scan mode
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                mOwner->DeferMessage(msg);
                mOwner->TransitionTo(mOwner->mApStaDisabledState);
            }
            break;
        }
        case CMD_DEFERRED_TOGGLE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 != mDeferredEnableSerialNumber) {
                Logger::D("WifiController::StaDisabledWithScanState", "DEFERRED_TOGGLE ignored due to serial mismatch");
                break;
            }
            Logger::D("WifiController::StaDisabledWithScanState", "DEFERRED_TOGGLE handled");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMessage* message = IMessage::Probe(obj);
            mOwner->SendMessage(message);
            break;
        }
        default:
            *result = FALSE;//NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;//HANDLED;
    return NOERROR;
}

Boolean WifiController::StaDisabledWithScanState::DoDeferEnable(
    /* [in] */ IMessage* msg)
{
    Int64 delaySoFar = SystemClock::GetElapsedRealtime() - mDisabledTimestamp;
    if (delaySoFar >= mOwner->mReEnableDelayMillis) {
        return FALSE;
    }

    Logger::D("WifiController::StaDisabledWithScanState", "WifiController msg xx deferred for %lld ms",
            (mOwner->mReEnableDelayMillis - delaySoFar));

    // need to defer this action.
    AutoPtr<IMessage> deferredMsg;
    mOwner->ObtainMessage(CMD_DEFERRED_TOGGLE, (IMessage**)&deferredMsg);

    AutoPtr<IMessage> message;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain(msg, (IMessage**)&message);

    deferredMsg->SetObj(message);
    deferredMsg->SetArg1(++mDeferredEnableSerialNumber);
    mOwner->SendMessageDelayed(deferredMsg, mOwner->mReEnableDelayMillis - delaySoFar + DEFER_MARGIN_MS);
    return TRUE;
}

//=====================================================================
//                    WifiController::ApEnabledState
//=====================================================================
ECode WifiController::ApEnabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::ApEnabledState", "what: %d", what);
    switch (what) {
        case CMD_AIRPLANE_TOGGLED: {
            Boolean isAirplaneModeOn;
            if (mOwner->mSettingsStore->IsAirplaneModeOn(&isAirplaneModeOn), isAirplaneModeOn) {
                mOwner->mWifiStateMachine->SetHostApRunning(NULL, FALSE);
                mOwner->TransitionTo(mOwner->mApStaDisabledState);
            }
            break;
        }
        case CMD_SET_AP: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 0) {
                mOwner->mWifiStateMachine->SetHostApRunning(NULL, FALSE);
                mOwner->TransitionTo(mOwner->mApStaDisabledState);
            }
            break;
        }
        default:
            *result = FALSE;//NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;//HANDLED;
    return NOERROR;
}

//=====================================================================
//                       WifiController::EcmState
//=====================================================================
ECode WifiController::EcmState::Enter()
{
    mOwner->mWifiStateMachine->SetSupplicantRunning(FALSE);
    return NOERROR;
}

ECode WifiController::EcmState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::EcmState", "what: %d", what);
    Int32 arg1;
    msg->GetArg1(&arg1);
    if (what == CMD_EMERGENCY_MODE_CHANGED && arg1 == 0) {
        Boolean isWifiToggleEnabled;
        Boolean isScanAlwaysAvailable;
        if (mOwner->mSettingsStore->IsWifiToggleEnabled(&isWifiToggleEnabled), isWifiToggleEnabled) {
            if (mOwner->mDeviceIdle == FALSE) {
                mOwner->CheckLocksAndTransitionWhenDeviceActive();
            } else {
                mOwner->CheckLocksAndTransitionWhenDeviceIdle();
            }
        } else if (mOwner->mSettingsStore->IsScanAlwaysAvailable(&isScanAlwaysAvailable), isScanAlwaysAvailable) {
            mOwner->TransitionTo(mOwner->mStaDisabledWithScanState);
        } else {
            mOwner->TransitionTo(mOwner->mApStaDisabledState);
        }
        *result = TRUE;//HANDLED;
    } else {
        *result = FALSE;//NOT_HANDLED;
    }
    return NOERROR;
}

//=====================================================================
//                  WifiController::DeviceActiveState
//=====================================================================
ECode WifiController::DeviceActiveState::Enter()
{
    mOwner->mWifiStateMachine->SetOperationalMode(WifiStateMachine::CONNECT_MODE);
    mOwner->mWifiStateMachine->SetDriverStart(TRUE);
    mOwner->mWifiStateMachine->SetHighPerfModeEnabled(FALSE);
    return NOERROR;
}

ECode WifiController::DeviceActiveState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::DeviceActiveState", "what: %d", what);
    if (what == CMD_DEVICE_IDLE) {
        mOwner->CheckLocksAndTransitionWhenDeviceIdle();
        // We let default state handle the rest of work
    } else if (what == CMD_LOCKS_CHANGED) {
        mOwner->CheckLocksAndTransitionWhenDeviceActive();
        *result = TRUE;//HANDLED;
        return NOERROR;
    } else if (what == CMD_USER_PRESENT) {
        // TLS networks can't connect until user unlocks keystore. KeyStore
        // unlocks when the user punches PIN after the reboot. So use this
        // trigger to get those networks connected.
        if (mOwner->mFirstUserSignOnSeen == FALSE) {
            mOwner->mWifiStateMachine->ReloadTlsNetworksAndReconnect();
        }
        mOwner->mFirstUserSignOnSeen = TRUE;
        *result = TRUE;//HANDLED;
        return NOERROR;
    }
    *result = FALSE;//NOT_HANDLED;
    return NOERROR;
}

//=====================================================================
//              WifiController::DeviceActiveHighPerfState
//=====================================================================
ECode WifiController::DeviceActiveHighPerfState::Enter()
{
    mOwner->mWifiStateMachine->SetOperationalMode(WifiStateMachine::CONNECT_MODE);
    mOwner->mWifiStateMachine->SetDriverStart(TRUE);
    mOwner->mWifiStateMachine->SetHighPerfModeEnabled(TRUE);
    return NOERROR;
}

//=====================================================================
//                 WifiController::DeviceInactiveState
//=====================================================================
ECode WifiController::DeviceInactiveState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    Logger::D("WifiController::DeviceInactiveState", "what: %d", what);
    switch (what) {
        case CMD_LOCKS_CHANGED:
            mOwner->CheckLocksAndTransitionWhenDeviceIdle();
            mOwner->UpdateBatteryWorkSource();
            *result = TRUE;//HANDLED;
        case CMD_SCREEN_ON:
            mOwner->CheckLocksAndTransitionWhenDeviceActive();
            // More work in default state
            *result = FALSE;//NOT_HANDLED;
        default:
            *result = FALSE;//NOT_HANDLED;
    }
    return NOERROR;
}

//=====================================================================
//                WifiController::ScanOnlyLockHeldState
//=====================================================================
ECode WifiController::ScanOnlyLockHeldState::Enter()
{
    mOwner->mWifiStateMachine->SetOperationalMode(WifiStateMachine::SCAN_ONLY_MODE);
    mOwner->mWifiStateMachine->SetDriverStart(TRUE);
    return NOERROR;
}

//=====================================================================
//                  WifiController::FullLockHeldState
//=====================================================================
ECode WifiController::FullLockHeldState::Enter()
{
    mOwner->mWifiStateMachine->SetOperationalMode(WifiStateMachine::CONNECT_MODE);
    mOwner->mWifiStateMachine->SetDriverStart(TRUE);
    mOwner->mWifiStateMachine->SetHighPerfModeEnabled(FALSE);
    return NOERROR;
}

//=====================================================================
//              WifiController::FullHighPerfLockHeldState
//=====================================================================
ECode WifiController::FullHighPerfLockHeldState::Enter()
{
    mOwner->mWifiStateMachine->SetOperationalMode(WifiStateMachine::CONNECT_MODE);
    mOwner->mWifiStateMachine->SetDriverStart(TRUE);
    mOwner->mWifiStateMachine->SetHighPerfModeEnabled(TRUE);
    return NOERROR;
}

//=====================================================================
//                   WifiController::NoLockHeldState
//=====================================================================
ECode WifiController::NoLockHeldState::Enter()
{
    mOwner->mWifiStateMachine->SetDriverStart(FALSE);
    return NOERROR;
}

//=====================================================================
//               WifiController::InnerBroadcastReceiver1
//=====================================================================
WifiController::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiController* owner)
    : mOwner(owner)
{
}

ECode WifiController::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(ACTION_DEVICE_IDLE)) {
        mOwner->SendMessage(CMD_DEVICE_IDLE);
    } else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> obj;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&obj);
        mOwner->mNetworkInfo = INetworkInfo::Probe(obj);
    }
    return NOERROR;
}

//=====================================================================
//                WifiController::InnerContentObserver1
//=====================================================================
WifiController::InnerContentObserver1::InnerContentObserver1(
    /* [in] */ WifiController* owner,
    /* [in] */ IHandler* handler)
    : mOwner(owner)
{
    ContentObserver::constructor(handler);
}

ECode WifiController::InnerContentObserver1::OnChange(
    /* [in] */ Boolean selfChange)
{
    mOwner->ReadStayAwakeConditions();
    return NOERROR;
}

//=====================================================================
//                WifiController::InnerContentObserver3
//=====================================================================
WifiController::InnerContentObserver3::InnerContentObserver3(
    /* [in] */ WifiController* owner,
    /* [in] */ IHandler* handler)
    : mOwner(owner)
{
    ContentObserver::constructor(handler);
}

ECode WifiController::InnerContentObserver3::OnChange(
    /* [in] */ Boolean selfChange)
{
    mOwner->ReadWifiIdleTime();
    return NOERROR;
}

//=====================================================================
//                WifiController::InnerContentObserver5
//=====================================================================
WifiController::InnerContentObserver5::InnerContentObserver5(
    /* [in] */ WifiController* owner,
    /* [in] */ IHandler* handler)
    : mOwner(owner)
{
    ContentObserver::constructor(handler);
}

ECode WifiController::InnerContentObserver5::OnChange(
    /* [in] */ Boolean selfChange)
{
    mOwner->ReadWifiSleepPolicy();
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
    : mScreenOff(FALSE)
    , mDeviceIdle(FALSE)
    , mPluggedType(0)
    , mStayAwakeConditions(0)
    , mIdleMillis(0)
    , mSleepPolicy(0)
    , mFirstUserSignOnSeen(FALSE)
    , mReEnableDelayMillis(0)
{
    StateMachine::constructor(TAG, looper);
    CNetworkInfo::New(IConnectivityManager::TYPE_WIFI, 0, String("WIFI"), String(""), (INetworkInfo**)&mNetworkInfo);
    CWorkSource::New((IWorkSource**)&mTmpWorkSource);
    mDefaultState = new DefaultState(this);
    mStaEnabledState = new StaEnabledState(this);
    mApStaDisabledState = new ApStaDisabledState(this);
    mStaDisabledWithScanState = new StaDisabledWithScanState(this);
    mApEnabledState = new ApEnabledState(this);
    mDeviceActiveState = new DeviceActiveState(this);
    mDeviceActiveHighPerfState = new DeviceActiveHighPerfState(this);
    mDeviceInactiveState = new DeviceInactiveState(this);
    mScanOnlyLockHeldState = new ScanOnlyLockHeldState(this);
    mFullLockHeldState = new FullLockHeldState(this);
    mFullHighPerfLockHeldState = new FullHighPerfLockHeldState(this);
    mNoLockHeldState = new NoLockHeldState(this);
    mEcmState = new EcmState(this);

    mContext = context;
    mWifiStateMachine = service->mWifiStateMachine;
    mSettingsStore = service->mSettingsStore;
    mLocks = service->mLocks;

    AutoPtr<IInterface> alarmObj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&alarmObj);
    mAlarmManager = IAlarmManager::Probe(alarmObj);

    AutoPtr<IIntent> idleIntent;
    CIntent::New(ACTION_DEVICE_IDLE, NULL, (IIntent**)&idleIntent);
    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    pendingIntentHelper->GetBroadcast(mContext, IDLE_REQUEST, idleIntent, 0, (IPendingIntent**)&mIdleIntent);

    AddState(mDefaultState);
        AddState(mApStaDisabledState, mDefaultState);
        AddState(mStaEnabledState, mDefaultState);
            AddState(mDeviceActiveState, mStaEnabledState);
                AddState(mDeviceActiveHighPerfState, mDeviceActiveState);
            AddState(mDeviceInactiveState, mStaEnabledState);
                AddState(mScanOnlyLockHeldState, mDeviceInactiveState);
                AddState(mFullLockHeldState, mDeviceInactiveState);
                AddState(mFullHighPerfLockHeldState, mDeviceInactiveState);
                AddState(mNoLockHeldState, mDeviceInactiveState);
        AddState(mStaDisabledWithScanState, mDefaultState);
        AddState(mApEnabledState, mDefaultState);
        AddState(mEcmState, mDefaultState);

    Boolean isAirplaneModeOn;
    mSettingsStore->IsAirplaneModeOn(&isAirplaneModeOn);
    Boolean isWifiEnabled;
    mSettingsStore->IsWifiToggleEnabled(&isWifiEnabled);
    Boolean isScanningAlwaysAvailable;
    mSettingsStore->IsScanAlwaysAvailable(&isScanningAlwaysAvailable);

    Logger::D(TAG, "isAirplaneModeOn = %d, isWifiEnabled = %d, isScanningAvailable = %d",
            isAirplaneModeOn, isWifiEnabled, isScanningAlwaysAvailable);

    if (isScanningAlwaysAvailable) {
        SetInitialState(mStaDisabledWithScanState);
    } else {
        SetInitialState(mApStaDisabledState);
    }

    SetLogRecSize(100);
    SetLogOnlyTransitions(false);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ACTION_DEVICE_IDLE);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    AutoPtr<IBroadcastReceiver> br= new InnerBroadcastReceiver1(this);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(filter, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intentTemp;
    mContext->RegisterReceiver(br, intentFilter, (IIntent**)&intentTemp);

    InitializeAndRegisterForSettingsChange(looper);
}

ECode WifiController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StateMachine::Dump(fd, pw, args);

    pw->Print(String("mScreenOff "));
    pw->Println(mScreenOff);
    pw->Print(String("mDeviceIdle "));
    pw->Println(mDeviceIdle);
    pw->Print(String("mPluggedType "));
    pw->Println(mPluggedType);
    pw->Print(String("mIdleMillis "));
    pw->Println(mIdleMillis);
    pw->Print(String("mSleepPolicy "));
    pw->Println(mSleepPolicy);
    return NOERROR;
}

void WifiController::InitializeAndRegisterForSettingsChange(
    /* [in] */ ILooper* looper)
{
    AutoPtr<IHandler> handler;
    CHandler::New(looper, (IHandler**)&handler);
    ReadStayAwakeConditions();
    RegisterForStayAwakeModeChange(handler);
    ReadWifiIdleTime();
    RegisterForWifiIdleTimeChange(handler);
    ReadWifiSleepPolicy();
    RegisterForWifiSleepPolicyChange(handler);
    ReadWifiReEnableDelay();
}

void WifiController::ReadStayAwakeConditions()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Settings::Global::GetInt32(cr, ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, 0, &mStayAwakeConditions);
}

void WifiController::ReadWifiIdleTime()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Settings::Global::GetInt64(cr, ISettingsGlobal::WIFI_IDLE_MS, DEFAULT_IDLE_MS, &mIdleMillis);
}

void WifiController::ReadWifiSleepPolicy()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_SLEEP_POLICY,
            ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER, &mSleepPolicy);
}

void WifiController::ReadWifiReEnableDelay()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Settings::Global::GetInt64(cr, ISettingsGlobal::WIFI_REENABLE_DELAY_MS, DEFAULT_REENABLE_DELAY_MS, &mReEnableDelayMillis);
}

void WifiController::RegisterForStayAwakeModeChange(
    /* [in] */ IHandler* handler)
{
    AutoPtr<IContentObserver> contentObserver = new InnerContentObserver1(this, handler);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, (IUri**)&uri);

    cr->RegisterContentObserver(uri, FALSE, contentObserver);
}

void WifiController::RegisterForWifiIdleTimeChange(
    /* [in] */ IHandler* handler)
{
    AutoPtr<IContentObserver> contentObserver = new InnerContentObserver3(this, handler);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::WIFI_IDLE_MS, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, contentObserver);
}

void WifiController::RegisterForWifiSleepPolicyChange(
    /* [in] */ IHandler* handler)
{
    AutoPtr<IContentObserver> contentObserver = new InnerContentObserver5(this, handler);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::WIFI_SLEEP_POLICY, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, contentObserver);
}

Boolean WifiController::ShouldWifiStayAwake(
    /* [in] */ Int32 pluggedType)
{
    if (mSleepPolicy == ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER) {
        // Never sleep
        return TRUE;
    } else if ((mSleepPolicy == ISettingsGlobal::WIFI_SLEEP_POLICY_NEVER_WHILE_PLUGGED) &&
            (pluggedType != 0)) {
        // Never sleep while plugged, and we're plugged
        return TRUE;
    } else {
        // Default
        return ShouldDeviceStayAwake(pluggedType);
    }
    return FALSE;
}

Boolean WifiController::ShouldDeviceStayAwake(
    /* [in] */ Int32 pluggedType)
{
    return (mStayAwakeConditions & pluggedType) != 0;
}

void WifiController::UpdateBatteryWorkSource()
{
    mTmpWorkSource->Clear();
    if (mDeviceIdle) {
        ((WifiServiceImpl::LockList*)mLocks.Get())->UpdateWorkSource(mTmpWorkSource);
    }
    mWifiStateMachine->UpdateBatteryWorkSource(mTmpWorkSource);
}

void WifiController::CheckLocksAndTransitionWhenDeviceActive()
{
    WifiServiceImpl::LockList* lockList = (WifiServiceImpl::LockList*)mLocks.Get();
    Boolean hasLocks;
    Int32 strongestLockMode;
    if ((lockList->HasLocks(&hasLocks), hasLocks) &&
            ((lockList->GetStrongestLockMode(&strongestLockMode), strongestLockMode) == IWifiManager::WIFI_MODE_FULL_HIGH_PERF)) {
        // It is possible for the screen to be off while the device is
        // is active (mIdleMillis), so we need the high-perf mode
        // otherwise powersaving mode will be turned on.
        TransitionTo(mDeviceActiveHighPerfState);
    } else {
        TransitionTo(mDeviceActiveState);
    }
}

void WifiController::CheckLocksAndTransitionWhenDeviceIdle()
{
    WifiServiceImpl::LockList* lockList = (WifiServiceImpl::LockList*)mLocks.Get();
    Boolean hasLocks;
    Int32 strongestLockMode;
    if (lockList->HasLocks(&hasLocks), hasLocks) {
        switch (lockList->GetStrongestLockMode(&strongestLockMode), strongestLockMode) {
            case IWifiManager::WIFI_MODE_FULL:
                TransitionTo(mFullLockHeldState);
                break;
            case IWifiManager::WIFI_MODE_FULL_HIGH_PERF:
                TransitionTo(mFullHighPerfLockHeldState);
                break;
            case IWifiManager::WIFI_MODE_SCAN_ONLY:
                TransitionTo(mScanOnlyLockHeldState);
                break;
            default:
                Logger::E(TAG, "Illegal lock %d", strongestLockMode);
        }
    } else {
        Boolean isScanningAlwaysAvailable;
        if (mSettingsStore->IsScanAlwaysAvailable(&isScanningAlwaysAvailable), isScanningAlwaysAvailable) {
            TransitionTo(mScanOnlyLockHeldState);
        } else {
            TransitionTo(mNoLockHeldState);
        }
    }
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
