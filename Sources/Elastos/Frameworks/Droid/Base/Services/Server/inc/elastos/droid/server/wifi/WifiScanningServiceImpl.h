/*
  * Copyright (C) 2008 The Android Open Source Project
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICEIMPL_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.app.AlarmManager;
// import android.app.PendingIntent;
// import android.content.BroadcastReceiver;
// import android.content.Context;
// import android.content.Intent;
// import android.content.IntentFilter;
// import android.net.wifi.IWifiScanner;
// import android.net.wifi.ScanResult;
// import android.net.wifi.WifiManager;
// import android.net.wifi.WifiScanner;
// import android.net.wifi.WifiScanner.ScanSettings;
// import android.net.wifi.WifiSsid;
// import android.os.Bundle;
// import android.os.Handler;
// import android.os.HandlerThread;
// import android.os.Looper;
// import android.os.Message;
// import android.os.Messenger;
// import android.os.RemoteException;
// import android.os.SystemClock;
// import android.util.Log;
// import android.util.Slog;
// import com.android.internal.util.AsyncChannel;
// import com.android.internal.util.Protocol;
// import com.android.internal.util.StateMachine;
// import com.android.internal.util.State;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import java.util.ArrayList;
// import java.util.Collection;
// import java.util.HashMap;
// import java.util.HashSet;
// import java.util.Iterator;
// import java.util.List;
// import java.util.Map;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Net::Wifi::IScanResult;
using Elastos::Io::IFileDescriptor;
using Elastos::Io::IPrintWriter;
using Elastos::Droid::Net::Wifi::WifiScanner::IScanSettings;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Com.android.internal.Utility.AsyncChannel;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Net::Wifi::IWifiScanner;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiScanningServiceImpl
    : public Object
    , public IWifiScanner::Stub
{
public:
    class WifiScanningStateMachine
        : public Object
        , public IStateMachine
        , public WifiNative::ScanEventHandler
        , public WifiNative::HotlistEventHandler
        , public WifiNative::SignificantWifiChangeEventHandler
    {
    public:
        class DefaultState
            : public Object
            , public IState
        {
        public:
            // @Override
            CARAPI Enter();

            // @Override
            CARAPI_(Boolean) ProcessMessage(
                /* [in] */ IMessage* msg);
        };

        class StartedState
            : public Object
            , public IState
        {
        public:
            // @Override
            CARAPI Enter();

            // @Override
            CARAPI_(Boolean) ProcessMessage(
                /* [in] */ IMessage* msg);
        };

        class PausedState
            : public Object
            , public IState
        {
        public:
            // @Override
            CARAPI Enter();

            // @Override
            CARAPI_(Boolean) ProcessMessage(
                /* [in] */ IMessage* msg);
        };

    public:
        WifiScanningStateMachine(
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI OnScanResultsAvailable();

        // @Override
        CARAPI OnSingleScanComplete();

        // @Override
        CARAPI OnFullScanResult(
            /* [in] */ IScanResult* fullScanResult);

        // @Override
        CARAPI OnScanPaused();

        // @Override
        CARAPI OnScanRestarted();

        // @Override
        CARAPI OnHotlistApFound(
            /* [in] */ ArrayOf<IScanResult>* results);

        // @Override
        CARAPI OnChangesFound(
            /* [in] */ ArrayOf<IScanResult>* results);

        // @Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

    private:
        /*const*/ AutoPtr<DefaultState> mDefaultState;
        /*const*/ AutoPtr<StartedState> mStartedState;
        /*const*/ AutoPtr<PausedState> mPausedState;
    };

    class WifiChangeStateMachine
        : public Object
        , public IStateMachine
        , public WifiNative::SignificantWifiChangeEventHandler
    {
    public:
        class DefaultState
            : public Object
            , public IState
        {
        public:
            // @Override
            CARAPI Enter();

            // @Override
            CARAPI_(Boolean) ProcessMessage(
                /* [in] */ IMessage* msg);
        };

        class StationaryState
            : public Object
            , public IState
        {
        public:
            // @Override
            CARAPI Enter();

            // @Override
            CARAPI_(Boolean) ProcessMessage(
                /* [in] */ IMessage* msg);
        };

        class MovingState
            : public Object
            , public IState
        {
        public:
            // @Override
            CARAPI Enter();

            // @Override
            CARAPI_(Boolean) ProcessMessage(
                /* [in] */ IMessage* msg);

            // @Override
            CARAPI Exit();

            virtual CARAPI IssueFullScan();

        public:
            Boolean mWifiChangeDetected;
            Boolean mScanResultsPending;
        };

        class ClientInfoLocal
            : public Object
            , public ClientInfo
        {
        public:
            ClientInfoLocal();

            // @Override
            CARAPI DeliverScanResults(
                /* [in] */ Int32 handler,
                /* [in] */ IScanResult* results[]);

            // @Override
            CARAPI ReportPeriodChanged(
                /* [in] */ Int32 handler,
                /* [in] */ IScanSettings* settings,
                /* [in] */ Int32 newPeriodInMs);
        };

    private:
        class InnerBroadcastReceiver3
            : public Object
            , public IBroadcastReceiver
        {
        public:
            InnerBroadcastReceiver3(
                /* [in] */ WifiChangeStateMachine* owner);

            // @Override
            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

        private:
            WifiChangeStateMachine* mOwner;
        };

    public:
        WifiChangeStateMachine(
            /* [in] */ ILooper* looper);

        virtual CARAPI Enable();

        virtual CARAPI Disable();

        virtual CARAPI Configure(
            /* [in] */  WifiScanner);

        virtual CARAPI ReconfigureScan(
            /* [in] */ ArrayOf<IScanResult>* results,
            /* [in] */ Int32 period);

        virtual CARAPI ReconfigureScan(
            /* [in] */  WifiScanner);

        // @Override
        CARAPI OnChangesFound(
            /* [in] */ IScanResult* results[]);

        virtual CARAPI AddScanRequest(
            /* [in] */  WifiScanner);

        virtual CARAPI RemoveScanRequest();

        virtual CARAPI TrackSignificantWifiChange(
            /* [in] */  WifiScanner);

        virtual CARAPI UntrackSignificantWifiChange();

    public:
        AutoPtr<IState> mDefaultState;
        AutoPtr<IState> mStationaryState;
        AutoPtr<IState> mMovingState;
        AutoPtr<IAlarmManager> mAlarmManager;
        AutoPtr<IPendingIntent> mTimeoutIntent;
        AutoPtr<IScanResult> mCurrentBssids[];
        AutoPtr<ClientInfo> mClientInfo;

    private:
        static const String TAG;
        static const Int32 WIFI_CHANGE_CMD_NEW_SCAN_RESULTS = 0;
        static const Int32 WIFI_CHANGE_CMD_CHANGE_DETECTED = 1;
        static const Int32 WIFI_CHANGE_CMD_CHANGE_TIMEOUT = 2;
        static const Int32 WIFI_CHANGE_CMD_ENABLE = 3;
        static const Int32 WIFI_CHANGE_CMD_DISABLE = 4;
        static const Int32 WIFI_CHANGE_CMD_CONFIGURE = 5;
        static const Int32 MAX_APS_TO_TRACK = 3;
        static const Int32 MOVING_SCAN_PERIOD_MS = 10000;
        static const Int32 STATIONARY_SCAN_PERIOD_MS = 5000;
        static const Int32 MOVING_STATE_TIMEOUT_MS = 30000;
        static const String ACTION_TIMEOUT;
        static const Int32 SCAN_COMMAND_ID = 1;
    };

private:
    class ClientHandler
        : public Object
        , public IHandler
    {
    public:
        ClientHandler(
            /* [in] */  android);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

    class InnerBroadcastReceiver1
        : public Object
        , public IBroadcastReceiver
    {
    public:
        InnerBroadcastReceiver1(
            /* [in] */ WifiScanningServiceImpl* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiScanningServiceImpl* mOwner;
    };

    class ClientInfo
        : public Object
    {
    public:
        ClientInfo(
            /* [in] */ IAsyncChannel* c,
            /* [in] */ IMessenger* m);

        // @Override
        CARAPI_(String) ToString();

        virtual CARAPI AddScanRequest(
            /* [in] */ IScanSettings* settings,
            /* [in] */ Int32 id);

        virtual CARAPI RemoveScanRequest(
            /* [in] */ Int32 id);

        virtual CARAPI GetScans(
            /* [out] */ Iterator<Map.Entry<Integer, WifiScanner.ScanSettings>>** result);

        virtual CARAPI GetScanSettings(
            /* [out] */ Collection<ScanSettings>** result);

        virtual CARAPI ReportScanResults(
            /* [in] */ ArrayOf<IScanResult>* results);

        virtual CARAPI ReportScanResults(
            /* [in] */ ArrayOf<IScanResult>* results,
            /* [in] */ Int32 handler);

        virtual CARAPI DeliverScanResults(
            /* [in] */ Int32 handler,
            /* [in] */ IScanResult* results[]);

        virtual CARAPI ReportFullScanResult(
            /* [in] */ IScanResult* result);

        virtual CARAPI ReportPeriodChanged(
            /* [in] */ Int32 handler,
            /* [in] */ IScanSettings* settings,
            /* [in] */ Int32 newPeriodInMs);

        virtual CARAPI AddHostlistSettings(
            /* [in] */ IWifiScanner* ::HotlistSettings* settings,
            /* [in] */ Int32 handler);

        virtual CARAPI RemoveHostlistSettings(
            /* [in] */ Int32 handler);

        virtual CARAPI GetHotlistSettings(
            /* [out] */ Collection<WifiScanner.HotlistSettings>** result);

        virtual CARAPI ReportHotlistResults(
            /* [in] */ ArrayOf<IScanResult>* results);

        virtual CARAPI AddSignificantWifiChange(
            /* [in] */ Int32 handler);

        virtual CARAPI RemoveSignificantWifiChange(
            /* [in] */ Int32 handler);

        virtual CARAPI GetWifiChangeHandlers(
            /* [out] */ Collection<Integer>** result);

        virtual CARAPI ReportWifiChanged(
            /* [in] */ ArrayOf<IScanResult>* results);

        virtual CARAPI ReportWifiStabilized(
            /* [in] */ ArrayOf<IScanResult>* results);

        virtual CARAPI Cleanup();

    public:
        AutoPtr< IHashMap<Integer, IScanSettings> > mScanSettings;
        AutoPtr< IHashMap<Integer, Integer> > mScanPeriods;
        AutoPtr< IHashMap<Integer, WifiScanner::HotlistSettings> > mHotlistSettings;
        AutoPtr< IHashSet<Integer> > mSignificantWifiHandlers;

    private:
        static const Int32 MAX_LIMIT = 16;
        /*const*/ AutoPtr<IAsyncChannel> mChannel;
        /*const*/ AutoPtr<IMessenger> mMessenger;
    };

    class SettingsComputer
        : public Object
    {
    private:
        class TimeBucket
            : public Object
        {
        public:
            TimeBucket(
                /* [in] */ Int32 p,
                /* [in] */ Int32 min,
                /* [in] */ Int32 max);

        public:
            Int32 periodInSecond;
            Int32 periodMinInSecond;
            Int32 periodMaxInSecond;
        };

    public:
        virtual CARAPI PrepChannelMap(
            /* [in] */  WifiScanner);

        virtual CARAPI AddScanRequestToBucket(
            /* [in] */  WifiScanner,
            /* [out] */ Int32* result);

        virtual CARAPI .ScanSettings getComputedSettings(
            /* [out] */ WifiNative** result);

        virtual CARAPI CompressBuckets();

    private:
        static CARAPI_(AutoPtr< ArrayOf< AutoPtr<TimeBucket> > >) MiddleInitMtimebuckets();

        CARAPI_(Int32) GetBestBucket(
            /* [in] */  WifiScanner);

    public:
        AutoPtr< IHashMap<Integer, Integer> > mChannelToBucketMap;

    private:
        static AutoPtr< ArrayOf<TimeBucket> > mTimeBuckets;
        static const Int32 MAX_BUCKETS = 8;
        static const Int32 MAX_CHANNELS = 8;
        static const Int32 DEFAULT_MAX_AP_PER_SCAN = 10;
        static const Int32 DEFAULT_REPORT_THRESHOLD = 10;
        static const Int32 DEFAULT_BASE_PERIOD_MS = 5000;
        AutoPtr<WifiNative::ScanSettings> mSettings;
    };

public:
    WifiScanningServiceImpl();

    WifiScanningServiceImpl(
        /* [in] */ IContext* context);

    // DFS needs 120 ms
    // @Override
    CARAPI_(AutoPtr<IMessenger>) GetMessenger();

    // @Override
    CARAPI_(AutoPtr<IBundle>) GetAvailableChannels(
        /* [in] */ Int32 band);

    virtual CARAPI StartService(
        /* [in] */ IContext* context);

    virtual CARAPI ReplySucceeded(
        /* [in] */ IMessage* msg,
        /* [in] */ Object* obj);

    virtual CARAPI ReplyFailed(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 reason,
        /* [in] */ String description);

    virtual CARAPI ResetBuckets(
        /* [out] */ Boolean* result);

    virtual CARAPI AddScanRequest(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler,
        /* [in] */ IScanSettings* settings,
        /* [out] */ Boolean* result);

    virtual CARAPI RemoveScanRequest(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler);

    virtual CARAPI GetScanResults(
        /* [in] */ ClientInfo* ci,
        /* [out] */ ScanResult[]** result);

    virtual CARAPI ResetHotlist();

    virtual CARAPI SetHotlist(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler,
        /* [in] */  WifiScanner);

    virtual CARAPI ResetHotlist(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler);

    virtual CARAPI TrackWifiChanges(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler);

    virtual CARAPI UntrackWifiChanges(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler);

    virtual CARAPI ConfigureWifiChange(
        /* [in] */  WifiScanner);

    virtual CARAPI ReportWifiChanged(
        /* [in] */ IScanResult* results[]);

    virtual CARAPI ReportWifiStabilized(
        /* [in] */ IScanResult* results[]);

private:
    CARAPI_(void) EnforceConnectivityInternalPermission();

    static CARAPI_(AutoPtr<IWifiScanner>) .ChannelSpec[] getChannelsForBand(
        /* [in] */ Int32 band);

    static CARAPI_(Int32) GetBandFromChannels(
        /* [in] */  WifiScanner);

    static CARAPI_(Int32) GetBandFromChannels(
        /* [in] */  WifiNative);

public:
    /* client management */
    AutoPtr< IHashMap<IMessenger, ClientInfo> > mClients;
    AutoPtr<WifiChangeStateMachine> mWifiChangeStateMachine;

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 INVALID_KEY = 0;
    // same as WifiScanner
    static const Int32 MIN_PERIOD_PER_CHANNEL_MS = 200;
    static const Int32 BASE = Protocol.BASE_WIFI_SCANNER_SERVICE;
    static const Int32 CMD_SCAN_RESULTS_AVAILABLE = BASE + 0;
    static const Int32 CMD_FULL_SCAN_RESULTS = BASE + 1;
    static const Int32 CMD_HOTLIST_AP_FOUND = BASE + 2;
    static const Int32 CMD_HOTLIST_AP_LOST = BASE + 3;
    static const Int32 CMD_WIFI_CHANGE_DETECTED = BASE + 4;
    static const Int32 CMD_WIFI_CHANGES_STABILIZED = BASE + 5;
    static const Int32 CMD_DRIVER_LOADED = BASE + 6;
    static const Int32 CMD_DRIVER_UNLOADED = BASE + 7;
    static const Int32 CMD_SCAN_PAUSED = BASE + 8;
    static const Int32 CMD_SCAN_RESTARTED = BASE + 9;
    AutoPtr<IContext> mContext;
    AutoPtr<WifiScanningStateMachine> mStateMachine;
    AutoPtr<ClientHandler> mClientHandler;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICEIMPL_H__

