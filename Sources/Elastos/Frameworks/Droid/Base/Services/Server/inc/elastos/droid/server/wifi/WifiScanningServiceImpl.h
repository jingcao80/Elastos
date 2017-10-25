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

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICEIMPL_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/server/wifi/WifiNative.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiScannerScanSettings;
using Elastos::Droid::Wifi::IWifiScannerHotlistSettings;
using Elastos::Droid::Wifi::IIWifiScanner;
using Elastos::Droid::Wifi::IWifiScannerChannelSpec;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class WifiScanningServiceImpl
    : public Object
    , public IIWifiScanner
    , public IBinder
{
public:
    class WifiScanningStateMachine
        : public StateMachine
        , public IWifiNativeScanEventHandler
        , public IWifiNativeHotlistEventHandler
        , public IWifiNativeSignificantWifiChangeEventHandler
    {
    public:
        class DefaultState
            : public State
        {
        public:
            DefaultState(
                /* [in] */ WifiScanningStateMachine* owner)
                : mOwner(owner)
            {
            }

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
            WifiScanningStateMachine* mOwner;
        };

        class StartedState
            : public State
        {
        public:
            StartedState(
                /* [in] */ WifiScanningStateMachine* owner)
                : mOwner(owner)
            {
            }

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("StartedState");
            }

        private:
            WifiScanningStateMachine* mOwner;
        };

        class PausedState
            : public State
        {
        public:
            PausedState(
                /* [in] */ WifiScanningStateMachine* owner)
                : mOwner(owner)
            {
            }

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("PausedState");
            }

        private:
            WifiScanningStateMachine* mOwner;
        };

    public:
        CAR_INTERFACE_DECL()

        WifiScanningStateMachine(
            /* [in] */ WifiScanningServiceImpl* owner,
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
            /* [in] */ ArrayOf<IScanResult*>* results);

        // @Override
        CARAPI OnChangesFound(
            /* [in] */ ArrayOf<IScanResult*>* results);

        // @Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

    private:
        AutoPtr<DefaultState> mDefaultState;
        AutoPtr<StartedState> mStartedState;
        AutoPtr<PausedState> mPausedState;
        WifiScanningServiceImpl* mOwner;
    };

    class ClientInfo
        : public Object
    {
    public:
        ClientInfo(
            /* [in] */ WifiScanningServiceImpl* owner,
            /* [in] */ IAsyncChannel* c,
            /* [in] */ IMessenger* m);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        virtual CARAPI AddScanRequest(
            /* [in] */ IWifiScannerScanSettings* settings,
            /* [in] */ Int32 id);

        virtual CARAPI RemoveScanRequest(
            /* [in] */ Int32 id);

        virtual CARAPI GetScans(
            /* [out] */ IIterator** result);//Map.Entry<Integer, WifiScanner.ScanSettings

        virtual CARAPI GetScanSettings(
            /* [out] */ ICollection** result);//IWifiScannerScanSettings

        virtual CARAPI ReportScanResults(
            /* [in] */ ArrayOf<IScanResult*>* results);

        virtual CARAPI ReportScanResults(
            /* [in] */ ArrayOf<IScanResult*>* results,
            /* [in] */ Int32 handler);

        virtual CARAPI DeliverScanResults(
            /* [in] */ Int32 handler,
            /* [in] */ ArrayOf<IScanResult*>* results);

        virtual CARAPI ReportFullScanResult(
            /* [in] */ IScanResult* result);

        virtual CARAPI ReportPeriodChanged(
            /* [in] */ Int32 handler,
            /* [in] */ IWifiScannerScanSettings* settings,
            /* [in] */ Int32 newPeriodInMs);

        virtual CARAPI AddHostlistSettings(
            /* [in] */ IWifiScannerHotlistSettings* settings,
            /* [in] */ Int32 handler);

        virtual CARAPI RemoveHostlistSettings(
            /* [in] */ Int32 handler);

        virtual CARAPI GetHotlistSettings(
            /* [out] */ ICollection** result);//IWifiScannerHotlistSettings

        virtual CARAPI ReportHotlistResults(
            /* [in] */ ArrayOf<IScanResult*>* results);

        virtual CARAPI AddSignificantWifiChange(
            /* [in] */ Int32 handler);

        virtual CARAPI RemoveSignificantWifiChange(
            /* [in] */ Int32 handler);

        virtual CARAPI GetWifiChangeHandlers(
            /* [out] */ ICollection** result);//Integer

        virtual CARAPI ReportWifiChanged(
            /* [in] */ ArrayOf<IScanResult*>* results);

        virtual CARAPI ReportWifiStabilized(
            /* [in] */ ArrayOf<IScanResult*>* results);

        virtual CARAPI Cleanup();

    public:
        AutoPtr<IHashMap> mScanSettings;//Integer, IWifiScannerScanSettings
        AutoPtr<IHashMap> mScanPeriods;//Integer, Integer
        AutoPtr<IHashMap> mHotlistSettings;//Integer, WifiScanner::HotlistSettings
        AutoPtr<IHashSet> mSignificantWifiHandlers;//Integer

    private:
        static const Int32 MAX_LIMIT = 16;
        AutoPtr<IAsyncChannel> mChannel;
        AutoPtr<IMessenger> mMessenger;
        WifiScanningServiceImpl* mOwner;
    };

    class WifiChangeStateMachine
        : public StateMachine
        , public IWifiNativeSignificantWifiChangeEventHandler
    {
    public:
        class DefaultState : public State
        {
        public:
            DefaultState(
                /* [in] */ WifiChangeStateMachine* owner)
                : mOwner(owner)
            {
            }

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
            WifiChangeStateMachine* mOwner;
        };

        class StationaryState : public State
        {
        public:
            StationaryState(
                /* [in] */ WifiChangeStateMachine* owner)
                : mOwner(owner)
            {
            }

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("StationaryState");
            }

        private:
            WifiChangeStateMachine* mOwner;
        };

        class MovingState : public State
        {
        public:
            MovingState(
                /* [in] */ WifiChangeStateMachine* owner)
                : mOwner(owner)
            {
            }

            // @Override
            CARAPI Enter();

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            // @Override
            CARAPI Exit();

            virtual CARAPI IssueFullScan();

            CARAPI_(String) GetName()
            {
                return String("MovingState");
            }

        public:
            Boolean mWifiChangeDetected;
            Boolean mScanResultsPending;
        private:
            WifiChangeStateMachine* mOwner;
        };

        class ClientInfoLocal
            : public ClientInfo
        {
        public:
            ClientInfoLocal(
                /* [in] */ WifiChangeStateMachine* owner);

            // @Override
            CARAPI DeliverScanResults(
                /* [in] */ Int32 handler,
                /* [in] */ ArrayOf<IScanResult*>* results);

            // @Override
            CARAPI ReportPeriodChanged(
                /* [in] */ Int32 handler,
                /* [in] */ IWifiScannerScanSettings* settings,
                /* [in] */ Int32 newPeriodInMs);
        private:
            WifiChangeStateMachine* mOwner;
        };

    private:
        class InnerBroadcastReceiver3
            : public BroadcastReceiver
        {
        public:
            TO_STRING_IMPL("WifiScanningServiceImpl::InnerBroadcastReceiver3")

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
        CAR_INTERFACE_DECL()

        WifiChangeStateMachine(
            /* [in] */ WifiScanningServiceImpl* owner,
            /* [in] */ ILooper* looper);

        virtual CARAPI Enable();

        virtual CARAPI Disable();

        virtual CARAPI Configure(
            /* [in] */ IWifiScannerWifiChangeSettings* settings);

        virtual CARAPI ReconfigureScan(
            /* [in] */ ArrayOf<IScanResult*>* results,
            /* [in] */ Int32 period);

        virtual CARAPI ReconfigureScan(
            /* [in] */ IWifiScannerWifiChangeSettings* settings);

        // @Override
        CARAPI OnChangesFound(
            /* [in] */ ArrayOf<IScanResult*>* results);

        virtual CARAPI AddScanRequest(
            /* [in] */ IWifiScannerScanSettings* settings);

        virtual CARAPI RemoveScanRequest();

        virtual CARAPI TrackSignificantWifiChange(
            /* [in] */ IWifiScannerWifiChangeSettings* settings);

        virtual CARAPI UntrackSignificantWifiChange();

    public:
        AutoPtr<State> mDefaultState;
        AutoPtr<State> mStationaryState;
        AutoPtr<State> mMovingState;
        AutoPtr<IAlarmManager> mAlarmManager;
        AutoPtr<IPendingIntent> mTimeoutIntent;
        AutoPtr<ArrayOf<IScanResult*> > mCurrentBssids;
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
        WifiScanningServiceImpl* mOwner;
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

        SettingsComputer();

        virtual CARAPI PrepChannelMap(
            /* [in] */  IWifiScannerScanSettings* settings);

        virtual CARAPI AddScanRequestToBucket(
            /* [in] */ IWifiScannerScanSettings* settings,
            /* [out] */ Int32* result);

        virtual CARAPI GetComputedSettings(
            /* [out] */ WifiNative::ScanSettings** result);

        virtual CARAPI CompressBuckets();

    private:
        static CARAPI_(AutoPtr<ArrayOf<TimeBucket*> >) MiddleInitMtimebuckets();

        CARAPI_(Int32) GetBestBucket(
            /* [in] */  IWifiScannerScanSettings* settings);

    public:
        AutoPtr<IHashMap> mChannelToBucketMap;//Integer, Integer

    private:
        static AutoPtr<ArrayOf<TimeBucket*> > mTimeBuckets;
        static const Int32 MAX_BUCKETS = 8;
        static const Int32 MAX_CHANNELS = 8;
        static const Int32 DEFAULT_MAX_AP_PER_SCAN = 10;
        static const Int32 DEFAULT_REPORT_THRESHOLD = 10;
        static const Int32 DEFAULT_BASE_PERIOD_MS = 5000;
        AutoPtr<WifiNative::ScanSettings> mSettings;
    };

private:
    class ClientHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("WifiScanningServiceImpl::ClientHandler")

        ClientHandler(
            /* [in] */ WifiScanningServiceImpl* owner,
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        WifiScanningServiceImpl* mOwner;
    };

    class InnerBroadcastReceiver1
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WifiScanningServiceImpl::InnerBroadcastReceiver1")

        InnerBroadcastReceiver1(
            /* [in] */ WifiScanningServiceImpl* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiScanningServiceImpl* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    WifiScanningServiceImpl();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // DFS needs 120 ms
    // @Override
    CARAPI GetMessenger(
        /* [out] */ IMessenger** messenger);

    // @Override
    CARAPI GetAvailableChannels(
        /* [in] */ Int32 band,
        /* [out] */ IBundle** bundle);

    virtual CARAPI StartService(
        /* [in] */ IContext* context);

    virtual CARAPI ReplySucceeded(
        /* [in] */ IMessage* msg,
        /* [in] */ IInterface* obj);

    virtual CARAPI ReplyFailed(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 reason,
        /* [in] */ const String& description);

    virtual CARAPI ResetBuckets(
        /* [out] */ Boolean* result);

    virtual CARAPI AddScanRequest(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler,
        /* [in] */ IWifiScannerScanSettings* settings,
        /* [out] */ Boolean* result);

    virtual CARAPI RemoveScanRequest(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler);

    virtual CARAPI GetScanResults(
        /* [in] */ ClientInfo* ci,
        /* [out] */ ArrayOf<IScanResult*>** result);

    virtual CARAPI ResetHotlist();

    virtual CARAPI SetHotlist(
        /* [in] */ ClientInfo* ci,
        /* [in] */ Int32 handler,
        /* [in] */ IWifiScannerHotlistSettings* settings);

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
        /* [in] */ IWifiScannerWifiChangeSettings* settings);

    virtual CARAPI ReportWifiChanged(
        /* [in] */ ArrayOf<IScanResult*>* results);

    virtual CARAPI ReportWifiStabilized(
        /* [in] */ ArrayOf<IScanResult*>* results);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }

private:
    CARAPI_(void) EnforceConnectivityInternalPermission();

    static CARAPI_(AutoPtr<ArrayOf<IWifiScannerChannelSpec*> >) GetChannelsForBand(
        /* [in] */ Int32 band);

    static CARAPI_(Int32) GetBandFromChannels(
        /* [in] */ ArrayOf<IWifiScannerChannelSpec*>* channels);

    static CARAPI_(Int32) GetBandFromChannels(
        /* [in] */ ArrayOf<WifiNative::ChannelSettings*>* channels);

public:
    /* client management */
    AutoPtr<IHashMap> mClients;// IMessenger, ClientInfo
    AutoPtr<WifiChangeStateMachine> mWifiChangeStateMachine;

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 INVALID_KEY = 0;
    // same as WifiScanner
    static const Int32 MIN_PERIOD_PER_CHANNEL_MS = 200;
    static const Int32 BASE = IProtocol::BASE_WIFI_SCANNER_SERVICE;
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

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Wifi::WifiScanningServiceImpl::SettingsComputer::TimeBucket, IInterface);
#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISCANNINGSERVICEIMPL_H__
