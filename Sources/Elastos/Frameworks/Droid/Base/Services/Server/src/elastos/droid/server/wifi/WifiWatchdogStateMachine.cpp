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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Text.h"
#include "elastos/droid/server/wifi/WifiWatchdogStateMachine.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Text::IDecimalFormat;
using Elastos::Text::INumberFormat;
using Elastos::Text::CDecimalFormat;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::SupplicantState;
using Elastos::Droid::Wifi::IRssiPacketCountInfo;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

const Boolean WifiWatchdogStateMachine::DBG = FALSE;
const Int32 WifiWatchdogStateMachine::BASE;
const Int32 WifiWatchdogStateMachine::EVENT_WATCHDOG_TOGGLED;
const Int32 WifiWatchdogStateMachine::EVENT_NETWORK_STATE_CHANGE;
const Int32 WifiWatchdogStateMachine::EVENT_RSSI_CHANGE;
const Int32 WifiWatchdogStateMachine::EVENT_SUPPLICANT_STATE_CHANGE;
const Int32 WifiWatchdogStateMachine::EVENT_WIFI_RADIO_STATE_CHANGE;
const Int32 WifiWatchdogStateMachine::EVENT_WATCHDOG_SETTINGS_CHANGE;
const Int32 WifiWatchdogStateMachine::EVENT_BSSID_CHANGE;
const Int32 WifiWatchdogStateMachine::EVENT_SCREEN_ON;
const Int32 WifiWatchdogStateMachine::EVENT_SCREEN_OFF;
const Int32 WifiWatchdogStateMachine::CMD_RSSI_FETCH;
const Int32 WifiWatchdogStateMachine::POOR_LINK_DETECTED;
const Int32 WifiWatchdogStateMachine::GOOD_LINK_DETECTED;
const Int32 WifiWatchdogStateMachine::LINK_MONITOR_LEVEL_THRESHOLD;
const Int32 WifiWatchdogStateMachine::BSSID_STAT_CACHE_SIZE;
const Int32 WifiWatchdogStateMachine::BSSID_STAT_RANGE_LOW_DBM;
const Int32 WifiWatchdogStateMachine::BSSID_STAT_RANGE_HIGH_DBM;
const Int32 WifiWatchdogStateMachine::BSSID_STAT_EMPTY_COUNT;
const Int64 WifiWatchdogStateMachine::LINK_SAMPLING_INTERVAL_MS;
const Double WifiWatchdogStateMachine::EXP_COEFFICIENT_RECORD;
const Double WifiWatchdogStateMachine::EXP_COEFFICIENT_MONITOR;
const Double WifiWatchdogStateMachine::POOR_LINK_LOSS_THRESHOLD;
const Double WifiWatchdogStateMachine::GOOD_LINK_LOSS_THRESHOLD;
const Int32 WifiWatchdogStateMachine::POOR_LINK_SAMPLE_COUNT;
const Double WifiWatchdogStateMachine::POOR_LINK_MIN_VOLUME;
const Int32 WifiWatchdogStateMachine::GOOD_LINK_RSSI_RANGE_MIN;
const Int32 WifiWatchdogStateMachine::GOOD_LINK_RSSI_RANGE_MAX;
AutoPtr<WifiWatchdogStateMachine::GoodLinkTarget> WifiWatchdogStateMachine::GOOD_LINK_TARGET[4] = {
    /*                 rssi_adj,       sample_count,   reduce_time */
    new GoodLinkTarget(0,              3,              30 * 60000),
    new GoodLinkTarget(3,              5,              5  * 60000),
    new GoodLinkTarget(6,              10,             1  * 60000),
    new GoodLinkTarget(9,              30,             0  * 60000),
};
AutoPtr<WifiWatchdogStateMachine::MaxAvoidTime> WifiWatchdogStateMachine::MAX_AVOID_TIME[3] = {
    /*               max_time,           min_rssi */
    new MaxAvoidTime(30 * 60000,         -200),
    new MaxAvoidTime(5  * 60000,         -70 ),
    new MaxAvoidTime(0  * 60000,         -55 ),
};
Boolean WifiWatchdogStateMachine::sWifiOnly;
AutoPtr< ArrayOf<Double> > WifiWatchdogStateMachine::sPresetLoss;


//==============================================================
// WifiWatchdogStateMachine::DefaultState
//==============================================================
ECode WifiWatchdogStateMachine::DefaultState::Enter()
{
    if (DBG) Logger::D("WifiWatchdogStateMachine::DefaultState", "Enter()");
    return NOERROR;
}

ECode WifiWatchdogStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_WATCHDOG_SETTINGS_CHANGE:
            mOwner->UpdateSettings();
            if (DBG) Logger::D("WifiWatchdogStateMachine::DefaultState",
                "ProcessMessage EVENT_WATCHDOG_SETTINGS_CHANGE");
            break;
        case EVENT_RSSI_CHANGE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mCurrentSignalLevel = mOwner->CalculateSignalLevel(arg1);
            if (DBG) Logger::D("WifiWatchdogStateMachine::DefaultState",
                "ProcessMessage EVENT_RSSI_CHANGE %d, level %d", arg1, mOwner->mCurrentSignalLevel);
            break;
        }
        case EVENT_WIFI_RADIO_STATE_CHANGE:
        case EVENT_NETWORK_STATE_CHANGE:
        case EVENT_SUPPLICANT_STATE_CHANGE:
        case EVENT_BSSID_CHANGE:
        case CMD_RSSI_FETCH:
        case IWifiManager::RSSI_PKTCNT_FETCH_SUCCEEDED:
        case IWifiManager::RSSI_PKTCNT_FETCH_FAILED:
            // ignore
            break;
        case EVENT_SCREEN_ON:
            mOwner->mIsScreenOn = TRUE;
            break;
        case EVENT_SCREEN_OFF:
            mOwner->mIsScreenOn = FALSE;
            break;
        default:
            // loge("Unhandled message " + msg + " in state " + getCurrentState().getName());
            mOwner->Loge(String("Unhandled message"));
            break;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::WatchdogDisabledState
//==============================================================
ECode WifiWatchdogStateMachine::WatchdogDisabledState::Enter()
{
    if (DBG) mOwner->Logd(GetName());
    return NOERROR;
}

ECode WifiWatchdogStateMachine::WatchdogDisabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_WATCHDOG_TOGGLED: {
            if (mOwner->IsWatchdogEnabled()) {
                mOwner->TransitionTo(mOwner->mNotConnectedState);
            }
            *result = HANDLED;
            return NOERROR;
        }
        case EVENT_NETWORK_STATE_CHANGE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IIntent> intent = IIntent::Probe(obj);

            AutoPtr<IParcelable> parcelable;
            intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
            AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(parcelable);

            if (networkInfo != NULL) {
                NetworkInfoDetailedState state;
                networkInfo->GetDetailedState(&state);
                switch (state) {
                    case Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK:
                        if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogDisabledState",
                            "Watchdog disabled, verify link");
                        mOwner->SendLinkStatusNotification(TRUE);
                        break;
                    default:
                        break;
                }
            }

            break;
        }
    }
    *result = NOT_HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::WatchdogEnabledState
//==============================================================
ECode WifiWatchdogStateMachine::WatchdogEnabledState::Enter()
{
    // if (DBG) logd(getName());
    if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState", "Enter");
    return NOERROR;
}

ECode WifiWatchdogStateMachine::WatchdogEnabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    AutoPtr<IIntent> intent;
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case EVENT_WATCHDOG_TOGGLED: {
            if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState",
                "ProcessMessage EVENT_WATCHDOG_TOGGLED");
            if (!mOwner->IsWatchdogEnabled()) {
                mOwner->TransitionTo(mOwner->mWatchdogDisabledState);
            }
            break;
        }
        case EVENT_NETWORK_STATE_CHANGE: {
            if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState",
                "ProcessMessage EVENT_NETWORK_STATE_CHANGE");

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            intent = IIntent::Probe(obj);

            AutoPtr<IParcelable> parcelable;
            intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
            AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(parcelable);

            assert(networkInfo != NULL);
            NetworkInfoDetailedState state;
            networkInfo->GetDetailedState(&state);
            if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState",
                "Network state change %d", state);

            parcelable = NULL;
            intent->GetParcelableExtra(IWifiManager::EXTRA_WIFI_INFO, (IParcelable**)&parcelable);
            mOwner->mWifiInfo = IWifiInfo::Probe(parcelable);
            String bssID;
            if (mOwner->mWifiInfo != NULL) {
                mOwner->mWifiInfo->GetBSSID(&bssID);
            }
            mOwner->UpdateCurrentBssid(bssID);

            switch (state) {
                case Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK:
                    parcelable = NULL;
                    intent->GetParcelableExtra(IWifiManager::EXTRA_LINK_PROPERTIES,
                        (IParcelable**)&parcelable);
                    mOwner->mLinkProperties = ILinkProperties::Probe(parcelable);

                    if (mOwner->mPoorNetworkDetectionEnabled) {
                        if (mOwner->mWifiInfo == NULL || mOwner->mCurrentBssid == NULL) {
                            Logger::E("WifiWatchdogStateMachine::WatchdogEnabledState",
                                "Ignore, wifiinfo bssid %p", mOwner->mCurrentBssid.Get());
                            mOwner->SendLinkStatusNotification(TRUE);
                        }
                        else {
                            mOwner->TransitionTo(mOwner->mVerifyingLinkState);
                        }
                    }
                    else {
                        mOwner->SendLinkStatusNotification(TRUE);
                    }
                    break;
                case Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED:
                    if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState::ProcessMessage",
                        "===========CONNECTED===========");
                    mOwner->TransitionTo(mOwner->mOnlineWatchState);
                    break;
                default:
                    mOwner->TransitionTo(mOwner->mNotConnectedState);
                    break;
            }
            break;
        }
        case EVENT_SUPPLICANT_STATE_CHANGE: {
            if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState",
                "ProcessMessage EVENT_SUPPLICANT_STATE_CHANGE");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            intent = IIntent::Probe(obj);
            AutoPtr<IParcelable> parcelable;
            intent->GetParcelableExtra(IWifiManager::EXTRA_NEW_STATE, (IParcelable**)&parcelable);
            AutoPtr<ISupplicantState> iss = ISupplicantState::Probe(parcelable.Get());
            SupplicantState supplicantState;
            iss->Get(&supplicantState);
            if (supplicantState == Elastos::Droid::Wifi::SupplicantState_COMPLETED) {
                mOwner->mWifiInfo = NULL;
                mOwner->mWifiManager->GetConnectionInfo((IWifiInfo**)&mOwner->mWifiInfo);
                String bssid;
                mOwner->mWifiInfo->GetBSSID(&bssid);
                mOwner->UpdateCurrentBssid(bssid);
            }
            break;
        }
        case EVENT_WIFI_RADIO_STATE_CHANGE: {
            if (DBG) Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState",
                "ProcessMessage EVENT_WIFI_RADIO_STATE_CHANGE");
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == IWifiManager::WIFI_STATE_DISABLING) {
                mOwner->TransitionTo(mOwner->mNotConnectedState);
            }
            break;
        }
        default:
            if (DBG) {
                if (what == EVENT_RSSI_CHANGE)
                    Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState", "ProcessMessage EVENT_RSSI_CHANGE");
                else if (what == EVENT_BSSID_CHANGE)
                    Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState", "ProcessMessage EVENT_BSSID_CHANGE");
                else
                    Logger::D("WifiWatchdogStateMachine::WatchdogEnabledState", "ProcessMessage other %08x", what);
            }
            *result = NOT_HANDLED;
            return NOERROR;
    }

    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::NotConnectedState
//==============================================================
ECode WifiWatchdogStateMachine::NotConnectedState::Enter()
{
    if (DBG) mOwner->Logd(GetName());
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::VerifyingLinkState
//==============================================================
ECode WifiWatchdogStateMachine::VerifyingLinkState::Enter()
{
    if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState", "Enter");
    mSampleCount = 0;
    if (mOwner->mCurrentBssid != NULL)
        mOwner->mCurrentBssid->NewLinkDetected();
    AutoPtr<IMessage> m;
    mOwner->ObtainMessage(CMD_RSSI_FETCH, ++mOwner->mRssiFetchToken, 0, (IMessage**)&m);
    mOwner->SendMessage(m);
    return NOERROR;
}

ECode WifiWatchdogStateMachine::VerifyingLinkState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_WATCHDOG_SETTINGS_CHANGE:
            if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                "ProcessMessage EVENT_WATCHDOG_SETTINGS_CHANGE");
            mOwner->UpdateSettings();
            if (!mOwner->mPoorNetworkDetectionEnabled) {
                mOwner->SendLinkStatusNotification(TRUE);
            }
            break;

        case EVENT_BSSID_CHANGE:
            if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                "ProcessMessage EVENT_BSSID_CHANGE");
            mOwner->TransitionTo(mOwner->mVerifyingLinkState);
            break;

        case CMD_RSSI_FETCH: {
            if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                "ProcessMessage CMD_RSSI_FETCH");
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == mOwner->mRssiFetchToken) {
                mOwner->mWsmChannel->SendMessage(IWifiManager::RSSI_PKTCNT_FETCH);
                AutoPtr<IMessage> m;
                mOwner->ObtainMessage(CMD_RSSI_FETCH, ++mOwner->mRssiFetchToken, 0, (IMessage**)&m);
                mOwner->SendMessageDelayed(m, LINK_SAMPLING_INTERVAL_MS);
            }
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH_SUCCEEDED: {
            if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                "ProcessMessage RSSI_PKTCNT_FETCH_SUCCEEDED");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            if (mOwner->mCurrentBssid == NULL || obj == NULL) {
                break;
            }
            IRssiPacketCountInfo* info = IRssiPacketCountInfo::Probe(obj);
            Int32 rssi;
            info->GetRssi(&rssi);
            if (DBG) Logger::E("WifiWatchdogStateMachine", "Fetch RSSI succeed, rssi=%d", rssi);

            Int64 time = mOwner->mCurrentBssid->mBssidAvoidTimeMax - SystemClock::GetElapsedRealtime();
            if (time <= 0) {
                // max avoidance time is met
                if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                    "Max avoid time elapsed");
                mOwner->SendLinkStatusNotification(TRUE);
            }
            else {
                if (rssi >= mOwner->mCurrentBssid->mGoodLinkTargetRssi) {
                    if (++mSampleCount >= mOwner->mCurrentBssid->mGoodLinkTargetCount) {
                        // link is good again
                        if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                            "Good link detected, rssi=%d", rssi);
                        mOwner->mCurrentBssid->mBssidAvoidTimeMax = 0;
                        mOwner->SendLinkStatusNotification(TRUE);
                    }
                }
                else {
                    mSampleCount = 0;
                    if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                        "Link is still poor, time left=%d", time);
                }
            }
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH_FAILED:
            if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                "ProcessMessage RSSI_PKTCNT_FETCH_FAILED");
            break;

        default:
            if (DBG) Logger::D("WifiWatchdogStateMachine::VerifyingLinkState",
                "ProcessMessage other %08x", what);
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::ConnectedState
//==============================================================
ECode WifiWatchdogStateMachine::ConnectedState::Enter()
{
    if (DBG) Logger::D("WifiWatchdogStateMachine::ConnectedState", "Enter()");
    return NOERROR;
}

ECode WifiWatchdogStateMachine::ConnectedState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_WATCHDOG_SETTINGS_CHANGE:
            mOwner->UpdateSettings();

            if (mOwner->mPoorNetworkDetectionEnabled) {
                mOwner->TransitionTo(mOwner->mOnlineWatchState);
            }
            else {
                mOwner->TransitionTo(mOwner->mOnlineState);
            }
            *result = HANDLED;
            return NOERROR;
    }
    *result = NOT_HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::OnlineWatchState
//==============================================================
ECode WifiWatchdogStateMachine::OnlineWatchState::Enter()
{
    if (DBG) Logger::D("WifiWatchdogStateMachine::OnlineWatchState", "Enter()");

    if (mOwner->mPoorNetworkDetectionEnabled) {
        // treat entry as an rssi change
        HandleRssiChange();
    }
    else {
        mOwner->TransitionTo(mOwner->mOnlineState);
    }
    return NOERROR;
}

void WifiWatchdogStateMachine::OnlineWatchState::HandleRssiChange()
{
    if (mOwner->mCurrentSignalLevel <= LINK_MONITOR_LEVEL_THRESHOLD && mOwner->mCurrentBssid != NULL) {
        mOwner->TransitionTo(mOwner->mLinkMonitoringState);
    }
    else {
        // stay here
    }
}

ECode WifiWatchdogStateMachine::OnlineWatchState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_RSSI_CHANGE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mCurrentSignalLevel = mOwner->CalculateSignalLevel(arg1);
            HandleRssiChange();
            break;
        }
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::LinkMonitoringState
//==============================================================
ECode WifiWatchdogStateMachine::LinkMonitoringState::Enter()
{
    if (DBG) Logger::D("WifiWatchdogStateMachine::LinkMonitoringState", "Enter()");

    mSampleCount = 0;
    mOwner->mCurrentLoss = new VolumeWeightedEMA(EXP_COEFFICIENT_MONITOR);
    AutoPtr<IMessage> m;
    mOwner->ObtainMessage(CMD_RSSI_FETCH, ++mOwner->mRssiFetchToken, 0, (IMessage**)&m);
    mOwner->SendMessage(m);
    return NOERROR;
}

ECode WifiWatchdogStateMachine::LinkMonitoringState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    if (DBG) Logger::D("WifiWatchdogStateMachine::LinkMonitoringState", "ProcessMessage %08x", what);

    switch (what) {
        case EVENT_RSSI_CHANGE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mOwner->mCurrentSignalLevel = mOwner->CalculateSignalLevel(arg1);
            if (mOwner->mCurrentSignalLevel <= LINK_MONITOR_LEVEL_THRESHOLD) {
                // stay here;
            }
            else {
                // we don't need frequent RSSI monitoring any more
                mOwner->TransitionTo(mOwner->mOnlineWatchState);
            }
            break;
        }
        case EVENT_BSSID_CHANGE:
            mOwner->TransitionTo(mOwner->mLinkMonitoringState);
            break;

        case CMD_RSSI_FETCH: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (!mOwner->mIsScreenOn) {
                mOwner->TransitionTo(mOwner->mOnlineState);
            }
            else if (arg1 == mOwner->mRssiFetchToken) {
                mOwner->mWsmChannel->SendMessage(IWifiManager::RSSI_PKTCNT_FETCH);
                AutoPtr<IMessage> m;
                mOwner->ObtainMessage(CMD_RSSI_FETCH, ++mOwner->mRssiFetchToken, 0, (IMessage**)&m);
                mOwner->SendMessageDelayed(m, LINK_SAMPLING_INTERVAL_MS);
            }
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH_SUCCEEDED: {
            if (mOwner->mCurrentBssid == NULL) {
                break;
            }
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IRssiPacketCountInfo* info = IRssiPacketCountInfo::Probe(obj);
            Int32 rssi;
            info->GetRssi(&rssi);
            Int32 mrssi = (mLastRssi + rssi) / 2;
            Int32 txbad, txgood;
            info->GetTxbad(&txbad);
            info->GetTxgood(&txgood);
            // if (DBG) logd("Fetch RSSI succeed, rssi=" + rssi + " mrssi=" + mrssi + " txbad="
            //         + txbad + " txgood=" + txgood);

            // skip the first data point as we want incremental values
            Int64 now = SystemClock::GetElapsedRealtime();
            if (now - mOwner->mCurrentBssid->mLastTimeSample < LINK_SAMPLING_INTERVAL_MS * 2) {

                // update packet loss statistics
                Int32 dbad = txbad - mLastTxBad;
                Int32 dgood = txgood - mLastTxGood;
                Int32 dtotal = dbad + dgood;

                if (dtotal > 0) {
                    // calculate packet loss in the last sampling interval
                    Double loss = ((Double) dbad) / ((Double) dtotal);

                    mOwner->mCurrentLoss->Update(loss, dtotal);

                    if (DBG) {
                        AutoPtr<IDecimalFormat> df;
                        CDecimalFormat::New(String("#.##"), (IDecimalFormat**)&df);
                        String values, volumes;
                        INumberFormat::Probe(df)->Format(mOwner->mCurrentLoss->mValue * 100, &values);
                        INumberFormat::Probe(df)->Format(mOwner->mCurrentLoss->mVolume, &volumes);
                        Logd(String("Incremental loss=") + StringUtils::ToString(dbad) + "/" + StringUtils::ToString(dtotal) +
                                " Current loss=" + values + "% volume=" + volumes);
                    }

                    mOwner->mCurrentBssid->UpdateLoss(mrssi, loss, dtotal);

                    // check for high packet loss and send poor link notification
                    if (mOwner->mCurrentLoss->mValue > POOR_LINK_LOSS_THRESHOLD
                            && mOwner->mCurrentLoss->mVolume > POOR_LINK_MIN_VOLUME) {
                        if (++mSampleCount >= POOR_LINK_SAMPLE_COUNT)
                            if (mOwner->mCurrentBssid->PoorLinkDetected(rssi)) {
                                mOwner->SendLinkStatusNotification(FALSE);
                                ++mOwner->mRssiFetchToken;
                            }
                    }
                    else {
                        mSampleCount = 0;
                    }
                }
            }

            mOwner->mCurrentBssid->mLastTimeSample = now;
            mLastTxBad = txbad;
            mLastTxGood = txgood;
            mLastRssi = rssi;
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH_FAILED:
            // can happen if we are waiting to get a disconnect notification
            if (DBG) mOwner->Logd(String("RSSI_FETCH_FAILED"));
            break;

        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::OnlineState
//==============================================================
ECode WifiWatchdogStateMachine::OnlineState::Enter()
{
    if (DBG) Logger::D("WifiWatchdogStateMachine::OnlineState", "Enter()");
    return NOERROR;
}

ECode WifiWatchdogStateMachine::OnlineState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_SCREEN_ON:
            mOwner->mIsScreenOn = TRUE;
            if (mOwner->mPoorNetworkDetectionEnabled) {
                mOwner->TransitionTo(mOwner->mOnlineWatchState);
            }
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}


//==============================================================
// WifiWatchdogStateMachine::VolumeWeightedEMA
//==============================================================
void WifiWatchdogStateMachine::VolumeWeightedEMA::Update(
    /* [in] */ Double newValue,
    /* [in] */ Int32 newVolume)
{
    if (newVolume <= 0) return;
    // core update formulas
    Double newProduct = newValue * newVolume;
    mProduct = mAlpha * newProduct + (1 - mAlpha) * mProduct;
    mVolume = mAlpha * newVolume  + (1 - mAlpha) * mVolume;
    mValue = mProduct / mVolume;
}


//==============================================================
// WifiWatchdogStateMachine::BssidStatistics
//==============================================================
WifiWatchdogStateMachine::BssidStatistics::BssidStatistics(
    /* [in] */ const String& bssid,
    /* [in] */ WifiWatchdogStateMachine* owner)
    : mBssid(bssid)
    , mRssiBase(BSSID_STAT_RANGE_LOW_DBM)
    , mEntriesSize(BSSID_STAT_RANGE_HIGH_DBM - BSSID_STAT_RANGE_LOW_DBM + 1)
    , mGoodLinkTargetRssi(0)
    , mGoodLinkTargetCount(0)
    , mGoodLinkTargetIndex(0)
    , mLastTimeSample(0)
    , mLastTimeGood(0)
    , mLastTimePoor(0)
    , mBssidAvoidTimeMax(0)
    , mOwner(owner)
{
    mEntries = ArrayOf<VolumeWeightedEMA*>::Alloc(mEntriesSize);
    for (Int32 i = 0; i < mEntriesSize; i++) {
        mEntries->Set(i, new VolumeWeightedEMA(EXP_COEFFICIENT_RECORD));
    }
}

void WifiWatchdogStateMachine::BssidStatistics::UpdateLoss(
    /* [in] */ Int32 rssi,
    /* [in] */ Double value,
    /* [in] */ Int32 volume)
{
    if (volume <= 0) return;
    Int32 index = rssi - mRssiBase;
    if (index < 0 || index >= mEntriesSize) return;
    (*mEntries)[index]->Update(value, volume);
    if (DBG) {
       AutoPtr<IDecimalFormat> df;
       CDecimalFormat::New(String("#.##"), (IDecimalFormat**)&df);
       String values, volumes;
       INumberFormat::Probe(df)->Format((*mEntries)[index]->mValue * 100, &values);
       INumberFormat::Probe(df)->Format((*mEntries)[index]->mVolume, &volumes);
       mOwner->Logd(String("Cache updated: loss[")+ StringUtils::ToString(rssi) +"]=" + values + "% volume=" + volumes);
    }
}

Double WifiWatchdogStateMachine::BssidStatistics::PresetLoss(
    /* [in] */ Int32 rssi)
{
    if (rssi <= -90) return 1.0;
    if (rssi > 0) return 0.0;

    if (sPresetLoss == NULL) {
        // pre-calculate all preset losses only once, then reuse them
        Int32 size = 90;
        sPresetLoss = ArrayOf<Double>::Alloc(size);
        for (Int32 i = 0; i < size; i++) (*sPresetLoss)[i] = 1.0 / Elastos::Core::Math::Pow(90 - i, 1.5);
    }
    return (*sPresetLoss)[-rssi];
}

Boolean WifiWatchdogStateMachine::BssidStatistics::PoorLinkDetected(
    /* [in] */ Int32 rssi)
{
    if (DBG) mOwner->Logd(String("Poor link detected, rssi=") + StringUtils::ToString(rssi));

    Int64 now = SystemClock::GetElapsedRealtime();
    //Int64 lastGood = now - mLastTimeGood;
    Int64 lastPoor = now - mLastTimePoor;

    // reduce the difficulty of good link target if last avoidance was long time ago
    while (mGoodLinkTargetIndex > 0
            && lastPoor >= GOOD_LINK_TARGET[mGoodLinkTargetIndex - 1]->REDUCE_TIME_MS)
        mGoodLinkTargetIndex--;
    mGoodLinkTargetCount = GOOD_LINK_TARGET[mGoodLinkTargetIndex]->SAMPLE_COUNT;

    // scan for a target RSSI at which the link is good
    Int32 from = rssi + GOOD_LINK_RSSI_RANGE_MIN;
    Int32 to = rssi + GOOD_LINK_RSSI_RANGE_MAX;
    mGoodLinkTargetRssi = FindRssiTarget(from, to, GOOD_LINK_LOSS_THRESHOLD);
    mGoodLinkTargetRssi += GOOD_LINK_TARGET[mGoodLinkTargetIndex]->RSSI_ADJ_DBM;
    if (mGoodLinkTargetIndex < 4/*GOOD_LINK_TARGET.length*/ - 1) mGoodLinkTargetIndex++;

    // calculate max avoidance time to prevent avoiding forever
    Int32 p = 0, pmax = 3/*MAX_AVOID_TIME.length*/ - 1;
    while (p < pmax && rssi >= MAX_AVOID_TIME[p + 1]->MIN_RSSI_DBM) p++;
    Int64 avoidMax = MAX_AVOID_TIME[p]->TIME_MS;

    // don't avoid if max avoidance time is 0 (RSSI is super high)
    if (avoidMax <= 0) return FALSE;

    // set max avoidance time, send poor link notification
    mBssidAvoidTimeMax = now + avoidMax;

    // if (DBG) logd("goodRssi=" + mGoodLinkTargetRssi + " goodCount=" + mGoodLinkTargetCount
    //         + " lastGood=" + lastGood + " lastPoor=" + lastPoor + " avoidMax=" + avoidMax);

    return TRUE;
}

void WifiWatchdogStateMachine::BssidStatistics::NewLinkDetected()
{
    // if this BSSID is currently being avoided, the reuse those values
    if (mBssidAvoidTimeMax > 0) {
        if (DBG) Logd(String("Previous avoidance still in effect, rssi=") + StringUtils::ToString(mGoodLinkTargetRssi) + " count=" +
                StringUtils::ToString(mGoodLinkTargetCount));
        return;
    }

    // calculate a new RSSI threshold for new link verifying
    Int32 from = BSSID_STAT_RANGE_LOW_DBM;
    Int32 to = BSSID_STAT_RANGE_HIGH_DBM;
    mGoodLinkTargetRssi = FindRssiTarget(from, to, GOOD_LINK_LOSS_THRESHOLD);
    mGoodLinkTargetCount = 1;
    mBssidAvoidTimeMax = SystemClock::GetElapsedRealtime() + MAX_AVOID_TIME[0]->TIME_MS;
    // if (DBG) logd("New link verifying target set, rssi=" + mGoodLinkTargetRssi + " count="
    //         + mGoodLinkTargetCount);
}

Int32 WifiWatchdogStateMachine::BssidStatistics::FindRssiTarget(
    /* [in] */ Int32 from,
    /* [in] */ Int32 to,
    /* [in] */ Double threshold)
{
    from -= mRssiBase;
    to -= mRssiBase;
    Int32 emptyCount = 0;
    Int32 d = from < to ? 1 : -1;
    for (Int32 i = from; i != to; i += d)
        // don't use a data point if it volume is too small (statistically unreliable)
        if (i >= 0 && i < mEntriesSize && (*mEntries)[i]->mVolume > 1.0) {
            emptyCount = 0;
            if ((*mEntries)[i]->mValue < threshold) {
                // scan target found
                Int32 rssi = mRssiBase + i;
                if (DBG) {
                    AutoPtr<IDecimalFormat> df;
                    CDecimalFormat::New(String("#.##"), (IDecimalFormat**)&df);
                    String ths, values, volumes;
                    INumberFormat* nf = INumberFormat::Probe(df);
                    nf->Format(threshold * 100, &ths);
                    nf->Format((*mEntries)[i]->mValue * 100, &values);
                    nf->Format((*mEntries)[i]->mVolume, &volumes);
                    mOwner->Logd(String("Scan target found: rssi=") + StringUtils::ToString(rssi) + " threshold=" + ths + "% value=" +
                        values + "% volume=" + volumes);
                }
                return rssi;
            }
        }
        else if (++emptyCount >= BSSID_STAT_EMPTY_COUNT) {
            // cache has insufficient data around this RSSI, use preset loss instead
            Int32 rssi = mRssiBase + i;
            Double lossPreset = PresetLoss(rssi);
            if (lossPreset < threshold) {
                if (DBG) {
                    AutoPtr<IDecimalFormat> df;
                    CDecimalFormat::New(String("#.##"), (IDecimalFormat**)&df);
                    String ths, ls;
                    INumberFormat::Probe(df)->Format(threshold * 100, &ths);
                    INumberFormat::Probe(df)->Format(lossPreset * 100, &ls);
                    Logd(String("Scan target found: rssi=") + StringUtils::ToString(rssi) + "threshold=" + ths + "% value=" +
                           ls + "% volume=preset");
                }
                return rssi;
            }
        }

    return mRssiBase + to;
}

//==============================================================
// WifiWatchdogStateMachine::LocalContentObserver
//==============================================================

ECode WifiWatchdogStateMachine::LocalContentObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg,
    /* [in] */ WifiWatchdogStateMachine* owner)
{
    mOwner = owner;
    mMsg = msg;
    return ContentObserver::constructor(handler);
}

ECode WifiWatchdogStateMachine::LocalContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mOwner->SendMessage(mMsg);
    return NOERROR;
}

//==============================================================
// WifiWatchdogStateMachine::NetworkBroadcastReceiver
//==============================================================
WifiWatchdogStateMachine::NetworkBroadcastReceiver::NetworkBroadcastReceiver(
    /* [in] */ WifiWatchdogStateMachine* host)
    : mOwner(host)
{}

ECode WifiWatchdogStateMachine::NetworkBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    String action;
    intent->GetAction(&action);
    if (DBG) Logger::D("WifiWatchdogStateMachine::NetworkBroadcastReceiver", "\n ### OnReceive: %s\n", action.string());

    if (action.Equals(IWifiManager::RSSI_CHANGED_ACTION)) {
        Int32 extra;
        intent->GetInt32Extra(IWifiManager::EXTRA_NEW_RSSI, -200, &extra);
        AutoPtr<IMessage> m;
        mOwner->ObtainMessage(EVENT_RSSI_CHANGE, extra, 0, (IMessage**)&m);
        mOwner->SendMessage(m);
    }
    else if (action.Equals(IWifiManager::SUPPLICANT_STATE_CHANGED_ACTION)) {
        mOwner->SendMessage(EVENT_SUPPLICANT_STATE_CHANGE, intent->Probe(EIID_IInterface));
    }
    else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        mOwner->SendMessage(EVENT_NETWORK_STATE_CHANGE, intent->Probe(EIID_IInterface));
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        mOwner->SendMessage(EVENT_SCREEN_ON);
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        mOwner->SendMessage(EVENT_SCREEN_OFF);
    }
    else if (action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)) {
        Int32 extra;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE, IWifiManager::WIFI_STATE_UNKNOWN, &extra);
        AutoPtr<IInteger32> obj;
        CInteger32::New(extra, (IInteger32**)&obj);
        mOwner->SendMessage(EVENT_WIFI_RADIO_STATE_CHANGE, obj);
    }
    return NOERROR;
}

//==============================================================
// WifiWatchdogStateMachine
//==============================================================
WifiWatchdogStateMachine::WifiWatchdogStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ IMessenger* dstMessenger)
    : mContext(context)
    , mPoorNetworkDetectionEnabled(FALSE)
    , mRssiFetchToken(0)
    , mCurrentSignalLevel(0)
    , mIsScreenOn(TRUE)
{
    StateMachine::constructor(String("WifiWatchdogStateMachine"));
    mDefaultState = new DefaultState(this);
    mWatchdogDisabledState = new WatchdogDisabledState(this);
    mWatchdogEnabledState = new WatchdogEnabledState(this);
    mNotConnectedState = new NotConnectedState(this);
    mVerifyingLinkState = new VerifyingLinkState(this);
    mConnectedState = new ConnectedState(this);
    mOnlineWatchState = new OnlineWatchState(this);
    mLinkMonitoringState = new LinkMonitoringState(this);
    mOnlineState = new OnlineState(this);

    context->GetContentResolver((IContentResolver**)&mContentResolver);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);
    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);
    mWsmChannel = new AsyncChannel();
    Int32 status;
    mWsmChannel->ConnectSync(mContext, h, dstMessenger, &status);

    SetupNetworkReceiver();

    // the content observer to listen needs a handler
    RegisterForSettingsChanges();
    RegisterForWatchdogToggle();
    AddState(mDefaultState);
    AddState(mWatchdogDisabledState, mDefaultState);
    AddState(mWatchdogEnabledState, mDefaultState);
    AddState(mNotConnectedState, mWatchdogEnabledState);
    AddState(mVerifyingLinkState, mWatchdogEnabledState);
    AddState(mConnectedState, mWatchdogEnabledState);
    AddState(mOnlineWatchState, mConnectedState);
    AddState(mLinkMonitoringState, mConnectedState);
    AddState(mOnlineState, mConnectedState);

    if (IsWatchdogEnabled()) {
        SetInitialState(mNotConnectedState);
    }
    else {
        SetInitialState(mWatchdogDisabledState);
    }
    SetLogRecSize(25);
    SetLogOnlyTransitions(TRUE);
    UpdateSettings();
}

AutoPtr<WifiWatchdogStateMachine> WifiWatchdogStateMachine::MakeWifiWatchdogStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ IMessenger* dstMessenger)
{
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    Boolean supported;
    cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &supported);
    sWifiOnly = (supported == FALSE);

    // Watchdog is always enabled. Poor network detection can be seperately turned on/off
    // TODO: Remove this setting & clean up state machine since we always have
    // watchdog in an enabled state
    PutSettingsGlobalBoolean(contentResolver, ISettingsGlobal::WIFI_WATCHDOG_ON, TRUE);

    AutoPtr<WifiWatchdogStateMachine> wwsm = new WifiWatchdogStateMachine(context, dstMessenger);
    wwsm->Start();
    return wwsm;
}

void WifiWatchdogStateMachine::SetupNetworkReceiver()
{
    mBroadcastReceiver = new NetworkBroadcastReceiver(this);

    assert(mIntentFilter == NULL);
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);

    mIntentFilter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiManager::RSSI_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiManager::SUPPLICANT_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    mIntentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);

    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReceiver, mIntentFilter, (IIntent**)&intent);
}

void WifiWatchdogStateMachine::RegisterForWatchdogToggle()
{
    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);
    AutoPtr<LocalContentObserver> contentObserver = new LocalContentObserver();
    contentObserver->constructor(h, EVENT_WATCHDOG_TOGGLED, this);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IUri> uri;
    global->GetUriFor(ISettingsGlobal::WIFI_WATCHDOG_ON, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, contentObserver);
}

void WifiWatchdogStateMachine::RegisterForSettingsChanges()
{
    AutoPtr<IHandler> h;
    GetHandler((IHandler**)&h);
    AutoPtr<LocalContentObserver> contentObserver = new LocalContentObserver();
    contentObserver->constructor(h, EVENT_WATCHDOG_SETTINGS_CHANGE, this);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IUri> uri;
    global->GetUriFor(ISettingsGlobal::WIFI_WATCHDOG_POOR_NETWORK_TEST_ENABLED, (IUri**)&uri);
    resolver->RegisterContentObserver(uri,
            FALSE, contentObserver);
}

ECode WifiWatchdogStateMachine::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)

{
    Logger::D("WifiWatchdogStateMachine::Dump", "TODO");
    // super.dump(fd, pw, args);
    // pw.print("WatchdogStatus: ");
    // pw.print("State: " + getCurrentState());
    // pw.println("mWifiInfo: [" + mWifiInfo + "]");
    // pw.println("mLinkProperties: [" + mLinkProperties + "]");
    // pw.println("mCurrentSignalLevel: [" + mCurrentSignalLevel + "]");
    // pw.println("mPoorNetworkDetectionEnabled: [" + mPoorNetworkDetectionEnabled + "]");
    return NOERROR;
}

Boolean WifiWatchdogStateMachine::IsWatchdogEnabled()
{
    Boolean ret = GetSettingsGlobalBoolean(
        mContentResolver, ISettingsGlobal::WIFI_WATCHDOG_ON, TRUE);
    if (DBG) Logger::D("WifiWatchdogStateMachine", "Watchdog enabled %d", ret);
    return ret;
}

void WifiWatchdogStateMachine::UpdateSettings()
{
    if (DBG) Logd(String("Updating secure settings"));

    // disable poor network avoidance
    if (sWifiOnly) {
        Logd(String("Disabling poor network avoidance for wi-fi only device"));
        mPoorNetworkDetectionEnabled = FALSE;
    }
    else {
        mPoorNetworkDetectionEnabled = GetSettingsGlobalBoolean(mContentResolver,
                ISettingsGlobal::WIFI_WATCHDOG_POOR_NETWORK_TEST_ENABLED,
                IWifiManager::DEFAULT_POOR_NETWORK_AVOIDANCE_ENABLED);
    }
}

void WifiWatchdogStateMachine::UpdateCurrentBssid(
    /* [in] */ const String& bssid)
{
    // if (DBG) logd("Update current BSSID to " + (bssid != NULL ? bssid : "NULL"));

    // if currently not connected, then set current BSSID to NULL
    if (bssid.IsNull()) {
        if (mCurrentBssid == NULL) return;
        mCurrentBssid = NULL;
        if (DBG) Logd(String("BSSID changed"));
        SendMessage(EVENT_BSSID_CHANGE);
        return;
    }

    // if it is already the current BSSID, then done
    if (mCurrentBssid != NULL && bssid.Equals(mCurrentBssid->mBssid)) return;

    // search for the new BSSID in the cache, add to cache if not found
    HashMap<String, AutoPtr<BssidStatistics> >::Iterator it = mBssidCache.Find(bssid);
    if (it == mBssidCache.End()) {
        mCurrentBssid = new BssidStatistics(bssid, this);
        mBssidCache[bssid] = mCurrentBssid;
    }
    else {
        mCurrentBssid = it->mSecond;
    }

    // send BSSID change notification
    if (DBG) Logd(String("BSSID changed"));
    SendMessage(EVENT_BSSID_CHANGE);
}

Int32 WifiWatchdogStateMachine::CalculateSignalLevel(
    /* [in] */ Int32 rssi)
{
    AutoPtr<IWifiManagerHelper> helper;
    CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
    Int32 signalLevel = 0;
    helper->CalculateSignalLevel(rssi, IWifiManager::RSSI_LEVELS, &signalLevel);
    if (DBG) Logger::D("WifiWatchdogStateMachine", "RSSI current: %d new: %d, %d", mCurrentSignalLevel, rssi, signalLevel);
    return signalLevel;
}

void WifiWatchdogStateMachine::SendLinkStatusNotification(
    /* [in] */ Boolean isGood)
{
    if (DBG) Logd(String("########################################"));
    if (isGood) {
        mWsmChannel->SendMessage(GOOD_LINK_DETECTED);
        if (mCurrentBssid != NULL) {
            mCurrentBssid->mLastTimeGood = SystemClock::GetElapsedRealtime();
        }
        if (DBG) Logger::D("WifiWatchdogStateMachine", "GOOD_LINK_DETECTED: Good link notification is sent");
    }
    else {
        mWsmChannel->SendMessage(POOR_LINK_DETECTED);
        if (mCurrentBssid != NULL) {
            mCurrentBssid->mLastTimePoor = SystemClock::GetElapsedRealtime();
        }
        if (DBG) Logger::D("WifiWatchdogStateMachine", "POOR_LINK_DETECTED: Poor link notification is sent");
    }
}

Boolean WifiWatchdogStateMachine::GetSettingsGlobalBoolean(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Boolean def)
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 value = 0;
    global->GetInt32(cr, name, def ? 1 : 0, &value);
    return value == 1;
}

Boolean WifiWatchdogStateMachine::PutSettingsGlobalBoolean(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean result;
    global->PutInt32(cr, name, value ? 1 : 0, &result);
    return result;
}

void WifiWatchdogStateMachine::Logd(
    /* [in] */ const String& s)
{
    Logger::D("WifiWatchdogStateMachine", s);
}

void WifiWatchdogStateMachine::Loge(
    /* [in] */ const String& s)
{
    Logger::E("WifiWatchdogStateMachine", s);
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
