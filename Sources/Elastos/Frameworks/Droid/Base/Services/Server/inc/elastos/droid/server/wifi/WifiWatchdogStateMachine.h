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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFIWATCHDOGSTATEMACHINE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFIWATCHDOGSTATEMACHINE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include <elastos/utility/etl/HashMap.h>
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Wifi::ISupplicantState;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiWatchdogStateMachine : public StateMachine
{
public:
    class NetworkBroadcastReceiver : public BroadcastReceiver
    {
    public:
        NetworkBroadcastReceiver(
            /* [in] */ WifiWatchdogStateMachine* host);

        //Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("WifiWatchdogStateMachine::NetworkBroadcastReceiver: ")
    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * Default state, guard for unhandled messages.
     */
    class DefaultState : public State
    {
    public:
        DefaultState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("DefaultState");
        }

    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * WiFi watchdog is disabled by the setting.
     */
    class WatchdogDisabledState : public State
    {
    public:
        WatchdogDisabledState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("WatchdogDisabledState");
        }

    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * WiFi watchdog is enabled by the setting.
     */
    class WatchdogEnabledState : public State
    {
    public:
        WatchdogEnabledState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("WatchdogEnabledState");
        }

    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * WiFi is disconnected.
     */
    class NotConnectedState : public State
    {
    public:
        NotConnectedState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        CARAPI_(String) GetName()
        {
            return String("NotConnectedState");
        }

    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * WiFi is connected, but waiting for good link detection message.
     */
    class VerifyingLinkState : public State
    {
    public:
        VerifyingLinkState(
            /* [in] */ WifiWatchdogStateMachine* owner)
            : mSampleCount(0)
            , mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("VerifyingLinkState");
        }

    private:
        Int32 mSampleCount;
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * WiFi is connected and link is verified.
     */
    class ConnectedState : public State
    {
    public:
        ConnectedState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("ConnectedState");
        }

    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * RSSI is high enough and don't need link monitoring.
     */
    class OnlineWatchState : public State
    {
    public:
        OnlineWatchState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("OnlineWatchState");
        }

    private:
        CARAPI_(void) HandleRssiChange();

    private:
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * Keep sampling the link and monitor any poor link situation.
     */
    class LinkMonitoringState : public State
    {
    public:
        LinkMonitoringState(
            /* [in] */ WifiWatchdogStateMachine* owner)
            : mSampleCount(0)
            , mLastRssi(0)
            , mLastTxGood(0)
            , mLastTxBad(0)
            , mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("LinkMonitoringState");
        }

    private:
        Int32 mSampleCount;

        Int32 mLastRssi;
        Int32 mLastTxGood;
        Int32 mLastTxBad;
        WifiWatchdogStateMachine* mOwner;
    };

    /**
     * Child state of ConnectedState indicating that we are online and there is nothing to do.
     */
    class OnlineState : public State
    {
    public:
        OnlineState(
            /* [in] */ WifiWatchdogStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("OnlineState");
        }

    private:
        WifiWatchdogStateMachine* mOwner;
    };

private:
    /**
     * Bundle of good link count parameters
     */
    class GoodLinkTarget: public Object
    {
    public:
        GoodLinkTarget(
            /* [in] */ Int32 adj,
            /* [in] */ Int32 count,
            /* [in] */ Int32 time)
            : RSSI_ADJ_DBM(adj)
            , SAMPLE_COUNT(count)
            , REDUCE_TIME_MS(time)
        {}

    public:
        const Int32 RSSI_ADJ_DBM;
        const Int32 SAMPLE_COUNT;
        const Int32 REDUCE_TIME_MS;
    };

    /**
     * Bundle of max avoidance time parameters
     */
    class MaxAvoidTime: public Object
    {
    public:
        MaxAvoidTime(
            /* [in] */ Int32 time,
            /* [in] */ Int32 rssi)
            : TIME_MS(time)
            , MIN_RSSI_DBM(rssi)
        {}

    public:
        const Int32 TIME_MS;
        const Int32 MIN_RSSI_DBM;
    };

    /**
     * Volume-weighted Exponential Moving Average (V-EMA)
     *    - volume-weighted:  each update has its own weight (number of packets)
     *    - exponential:      O(1) time and O(1) space for both update and query
     *    - moving average:   reflect most recent results and expire old ones
     */
    class VolumeWeightedEMA: public Object
    {
    public:
        VolumeWeightedEMA(
            /* [in] */ Double coefficient)
            : mValue(0)
            , mVolume(0)
            , mProduct(0)
            , mAlpha(coefficient)
        {}

        CARAPI_(void) Update(
            /* [in] */ Double newValue,
            /* [in] */ Int32 newVolume);

    private:
        friend class WifiWatchdogStateMachine;

        Double mValue;
        Double mVolume;
        Double mProduct;
        const Double mAlpha;
    };

    /**
     * Record (RSSI -> pakce loss %) mappings of one BSSID
     */
    class BssidStatistics: public Object
    {
    public:
        /**
         * Constructor
         *
         * @param bssid is the address of this BSSID
         */
        BssidStatistics(
            /* [in] */ const String& bssid,
            /* [in] */ WifiWatchdogStateMachine* owner);

        /**
         * Update this BSSID cache
         *
         * @param rssi is the RSSI
         * @param value is the new instant loss value at this RSSI
         * @param volume is the volume for this single update
         */
        CARAPI_(void) UpdateLoss(
            /* [in] */ Int32 rssi,
            /* [in] */ Double value,
            /* [in] */ Int32 volume);

        /**
         * Get preset loss if the cache has insufficient data, observed from experiments.
         *
         * @param rssi is the input RSSI
         * @return preset loss of the given RSSI
         */
        CARAPI_(Double) PresetLoss(
            /* [in] */ Int32 rssi);

        /**
         * A poor link is detected, calculate a target RSSI to bring WiFi back.
         *
         * @param rssi is the current RSSI
         * @return TRUE iff the current BSSID should be avoided
         */
        CARAPI_(Boolean) PoorLinkDetected(
            /* [in] */ Int32 rssi);

        /**
         * A new BSSID is connected, recalculate target RSSI threshold
         */
        CARAPI_(void) NewLinkDetected();

        /**
         * Return the first RSSI within the range where loss[rssi] < threshold
         *
         * @param from start scanning from this RSSI
         * @param to stop scanning at this RSSI
         * @param threshold target threshold for scanning
         * @return target RSSI
         */
        CARAPI_(Int32) FindRssiTarget(
            /* [in] */ Int32 from,
            /* [in] */ Int32 to,
            /* [in] */ Double threshold);

    private:
        friend class WifiWatchdogStateMachine;

        /* MAC address of this BSSID */
        String mBssid;

        /* RSSI -> packet loss % mappings */
        AutoPtr< ArrayOf<VolumeWeightedEMA*> > mEntries;
        Int32 mRssiBase;
        Int32 mEntriesSize;

        /* Target to send good link notification, set when poor link is detected */
        Int32 mGoodLinkTargetRssi;
        Int32 mGoodLinkTargetCount;

        /* Index of GOOD_LINK_TARGET array */
        Int32 mGoodLinkTargetIndex;

        /* Timestamps of some last events */
        Int64 mLastTimeSample;
        Int64 mLastTimeGood;
        Int64 mLastTimePoor;

        /* Max time to avoid this BSSID */
        Int64 mBssidAvoidTimeMax;
        WifiWatchdogStateMachine* mOwner;
    };

    class LocalContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("WifiWatchdogStateMachine::LocalContentObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ Int32 msg,
            /* [in] */ WifiWatchdogStateMachine* owner);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiWatchdogStateMachine* mOwner;
        Int32 mMsg;
    };

public:
    /**
     * STATE MAP
     *          Default
     *         /       \
     * Disabled      Enabled
     *             /     \     \
     * NotConnected  Verifying  Connected
     *                         /---------\
     *                       (all other states)
     */
    WifiWatchdogStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ IMessenger* dstMessenger);

    static CARAPI_(AutoPtr<WifiWatchdogStateMachine>) MakeWifiWatchdogStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ IMessenger* dstMessenger);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) SetupNetworkReceiver();

    /**
     * Observes the watchdog on/off setting, and takes action when changed.
     */
    CARAPI_(void) RegisterForWatchdogToggle();

    /**
     * Observes watchdogs secure setting changes.
     */
    CARAPI_(void) RegisterForSettingsChanges();

    CARAPI_(Boolean) IsWatchdogEnabled();

    CARAPI_(void) UpdateSettings();

    CARAPI_(void) UpdateCurrentBssid(
        /* [in] */ const String& bssid);

    CARAPI_(Int32) CalculateSignalLevel(
        /* [in] */ Int32 rssi);

    CARAPI_(void) SendLinkStatusNotification(
        /* [in] */ Boolean isGood);

    /**
     * Convenience function for retrieving a single secure settings value as a
     * Boolean. Note that internally setting values are always stored as
     * strings; this function converts the string to a Boolean for you. The
     * default value will be returned if the setting is not defined or not a
     * valid Boolean.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     * @param def Value to return if the setting is not defined.
     * @return The setting's current value, or 'def' if it is not defined or not
     *         a valid Boolean.
     */
    static CARAPI_(Boolean) GetSettingsGlobalBoolean(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Boolean def);

    /**
     * Convenience function for updating a single settings value as an integer.
     * This will either create a new entry in the table if the given name does
     * not exist, or modify the value of the existing row with that name. Note
     * that internally setting values are always stored as strings, so this
     * function converts the given value to a string before storing it.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to modify.
     * @param value The new value for the setting.
     * @return true if the value was set, false on database errors
     */
    static CARAPI_(Boolean) PutSettingsGlobalBoolean(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    static CARAPI_(void) Logd(
        /* [in] */ const String& s);

    static CARAPI_(void) Loge(
        /* [in] */ const String& s);

private:
    static const Int32 BASE = IProtocol::BASE_WIFI_WATCHDOG;

public:
    /* Notifications from/to WifiStateMachine */
    static const Int32 POOR_LINK_DETECTED = BASE + 21;
    static const Int32 GOOD_LINK_DETECTED = BASE + 22;

    /* Internal events */
    static const Int32 EVENT_WATCHDOG_TOGGLED = BASE + 1;
    static const Int32 EVENT_NETWORK_STATE_CHANGE = BASE + 2;
    static const Int32 EVENT_RSSI_CHANGE = BASE + 3;
    static const Int32 EVENT_SUPPLICANT_STATE_CHANGE = BASE + 4;
    static const Int32 EVENT_WIFI_RADIO_STATE_CHANGE = BASE + 5;
    static const Int32 EVENT_WATCHDOG_SETTINGS_CHANGE = BASE + 6;
    static const Int32 EVENT_BSSID_CHANGE = BASE + 7;
    static const Int32 EVENT_SCREEN_ON = BASE + 8;
    static const Int32 EVENT_SCREEN_OFF = BASE + 9;

    /* Internal messages */
    static const Int32 CMD_RSSI_FETCH = BASE + 11;

    //static const Boolean DEFAULT_POOR_NETWORK_AVOIDANCE_ENABLED = FALSE;

private:
    /* STOPSHIP: Keep this configurable for debugging until ship */
    static const Boolean DBG;
    //static const String TAG;

    /*
     * RSSI levels as used by notification icon
     * Level 4  -55 <= RSSI
     * Level 3  -66 <= RSSI < -55
     * Level 2  -77 <= RSSI < -67
     * Level 1  -88 <= RSSI < -78
     * Level 0         RSSI < -88
     */

    /**
     * WiFi link statistics is monitored and recorded actively below this threshold.
     * <p>
     * Larger threshold is more adaptive but increases sampling cost.
     */
    static const Int32 LINK_MONITOR_LEVEL_THRESHOLD = IWifiManager::RSSI_LEVELS - 1;

    /**
     * Remember packet loss statistics of how many BSSIDs.
     * <p>
     * Larger size is usually better but requires more space.
     */
    static const Int32 BSSID_STAT_CACHE_SIZE = 20;

    /**
     * RSSI range of a BSSID statistics.
     * Within the range, (RSSI -> packet loss %) mappings are stored.
     * <p>
     * Larger range is usually better but requires more space.
     */
    static const Int32 BSSID_STAT_RANGE_LOW_DBM = -105;

    /**
     * See {@link #BSSID_STAT_RANGE_LOW_DBM}.
     */
    static const Int32 BSSID_STAT_RANGE_HIGH_DBM = -45;

    /**
     * How many consecutive empty data point to trigger a empty-cache detection.
     * In this case, a preset/default loss value (function on RSSI) is used.
     * <p>
     * In normal uses, some RSSI values may never be seen due to channel randomness.
     * However, the size of such empty RSSI chunk in normal use is generally 1~2.
     */
    static const Int32 BSSID_STAT_EMPTY_COUNT = 3;

    /**
     * Sample interval for packet loss statistics, in msec.
     * <p>
     * Smaller interval is more accurate but increases sampling cost (battery consumption).
     */
    static const Int64 LINK_SAMPLING_INTERVAL_MS = 1 * 1000;

    /**
     * Coefficients (alpha) for moving average for packet loss tracking.
     * Must be within (0.0, 1.0).
     * <p>
     * Equivalent number of samples: N = 2 / alpha - 1 .
     * We want the historic loss to base on more data points to be statistically reliable.
     * We want the current instant loss to base on less data points to be responsive.
     */
    static const Double EXP_COEFFICIENT_RECORD = 0.1;

    /**
     * See {@link #EXP_COEFFICIENT_RECORD}.
     */
    static const Double EXP_COEFFICIENT_MONITOR = 0.5;

    /**
     * Thresholds for sending good/poor link notifications, in packet loss %.
     * Good threshold must be smaller than poor threshold.
     * Use smaller poor threshold to avoid WiFi more aggressively.
     * Use smaller good threshold to bring back WiFi more conservatively.
     * <p>
     * When approaching the boundary, loss ratio jumps significantly within a few dBs.
     * 50% loss threshold is a good balance between accuracy and reponsiveness.
     * <=10% good threshold is a safe value to avoid jumping back to WiFi too easily.
     */
    static const Double POOR_LINK_LOSS_THRESHOLD = 0.5;

    /**
     * See {@link #POOR_LINK_LOSS_THRESHOLD}.
     */
    static const Double GOOD_LINK_LOSS_THRESHOLD = 0.1;

    /**
     * Number of samples to confirm before sending a poor link notification.
     * Response time = confirm_count * sample_interval .
     * <p>
     * A smaller threshold improves response speed but may suffer from randomness.
     * According to experiments, 3~5 are good values to achieve a balance.
     * These parameters should be tuned along with {@link #LINK_SAMPLING_INTERVAL_MS}.
     */
    static const Int32 POOR_LINK_SAMPLE_COUNT = 3;

    /**
     * Minimum volume (converted from pkt/sec) to detect a poor link, to avoid randomness.
     * <p>
     * According to experiments, 1pkt/sec is too sensitive but 3pkt/sec is slightly unresponsive.
     */
    static const Double POOR_LINK_MIN_VOLUME = 2.0 * LINK_SAMPLING_INTERVAL_MS / 1000.0;

    /**
     * When a poor link is detected, we scan over this range (based on current
     * poor link RSSI) for a target RSSI that satisfies a target packet loss.
     * Refer to {@link #GOOD_LINK_TARGET}.
     * <p>
     * We want range_min not too small to avoid jumping back to WiFi too easily.
     */
    static const Int32 GOOD_LINK_RSSI_RANGE_MIN = 3;

    /**
     * See {@link #GOOD_LINK_RSSI_RANGE_MIN}.
     */
    static const Int32 GOOD_LINK_RSSI_RANGE_MAX = 20;

    /**
     * Adaptive good link target to avoid flapping.
     * When a poor link is detected, a good link target is calculated as follows:
     * <p>
     *      targetRSSI = min { rssi | loss(rssi) < GOOD_LINK_LOSS_THRESHOLD } + rssi_adj[i],
     *                   where rssi is within the above GOOD_LINK_RSSI_RANGE.
     *      targetCount = sample_count[i] .
     * <p>
     * While WiFi is being avoided, we keep monitoring its signal strength.
     * Good link notification is sent when we see current RSSI >= targetRSSI
     * for targetCount consecutive times.
     * <p>
     * Index i is incremented each time after a poor link detection.
     * Index i is decreased to at most k if the last poor link was at lease reduce_time[k] ago.
     * <p>
     * Intuitively, larger index i makes it more difficult to get back to WiFi, avoiding flapping.
     * In experiments, (+9 dB / 30 counts) makes it quite difficult to achieve.
     * Avoid using it unless flapping is really bad (say, last poor link is < 1 min ago).
     */
    static AutoPtr<GoodLinkTarget> GOOD_LINK_TARGET[4];

    /**
     * The max time to avoid a BSSID, to prevent avoiding forever.
     * If current RSSI is at least min_rssi[i], the max avoidance time is at most max_time[i]
     * <p>
     * It is unusual to experience high packet loss at high RSSI. Something unusual must be
     * happening (e.g. strong interference). For higher signal strengths, we set the avoidance
     * time to be low to allow for quick turn around from temporary interference.
     * <p>
     * See {@link BssidStatistics#poorLinkDetected}.
     */
    static AutoPtr<MaxAvoidTime> MAX_AVOID_TIME[3];

    /* Framework related */
    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<IIntentFilter> mIntentFilter;
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<AsyncChannel> mWsmChannel;
    AutoPtr<IWifiInfo> mWifiInfo;
    AutoPtr<ILinkProperties> mLinkProperties;

    /* System settingss related */
    static Boolean sWifiOnly;
    Boolean mPoorNetworkDetectionEnabled;

    /* Poor link detection related */
    HashMap<String, AutoPtr<BssidStatistics> > mBssidCache;// =
            // new LruCache<String, BssidStatistics>(BSSID_STAT_CACHE_SIZE);
    Int32 mRssiFetchToken;
    Int32 mCurrentSignalLevel;
    AutoPtr<BssidStatistics> mCurrentBssid;
    AutoPtr<VolumeWeightedEMA> mCurrentLoss;
    Boolean mIsScreenOn;
    static AutoPtr< ArrayOf<Double> > sPresetLoss;

    /* WiFi watchdog state machine related */
    AutoPtr<DefaultState> mDefaultState;
    AutoPtr<WatchdogDisabledState> mWatchdogDisabledState;
    AutoPtr<WatchdogEnabledState> mWatchdogEnabledState;
    AutoPtr<NotConnectedState> mNotConnectedState;
    AutoPtr<VerifyingLinkState> mVerifyingLinkState;
    AutoPtr<ConnectedState> mConnectedState;
    AutoPtr<OnlineWatchState> mOnlineWatchState;
    AutoPtr<LinkMonitoringState> mLinkMonitoringState;
    AutoPtr<OnlineState> mOnlineState;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Wifi::WifiWatchdogStateMachine::VolumeWeightedEMA, IInterface);
#endif // __ELASTOS_DROID_SERVER_WIFI_WIFIWATCHDOGSTATEMACHINE_H__
