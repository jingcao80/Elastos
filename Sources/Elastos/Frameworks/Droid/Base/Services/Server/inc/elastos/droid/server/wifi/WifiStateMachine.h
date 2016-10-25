#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISTATEMACHINE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISTATEMACHINE_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.CoreLibrary.Utility.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/net/NetworkAgent.h"
#include "elastos/droid/net/NetworkFactory.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/server/wifi/p2p/WifiP2pServiceImpl.h"
#include "elastos/droid/server/wifi/SupplicantStateTracker.h"
#include "elastos/droid/server/wifi/WifiTrafficPoller.h"
#include "elastos/droid/server/wifi/WifiAutoJoinController.h"
#include "elastos/droid/server/wifi/WifiConfigStore.h"
#include "elastos/droid/server/wifi/WifiMonitor.h"
#include "elastos/droid/server/wifi/WifiNative.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IDhcpResults;
using Elastos::Droid::Net::IDhcpStateMachine;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkAgent;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkFactory;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Server::Net::INetlinkTracker;
using Elastos::Droid::Server::Net::INetlinkTrackerCallback;
using Elastos::Droid::Server::Wifi::P2p::WifiP2pServiceImpl;
using Elastos::Droid::Server::Wifi::WifiTrafficPoller;
using Elastos::Droid::Wifi::IBatchedScanSettings;
using Elastos::Droid::Wifi::IRssiPacketCountInfo;
using Elastos::Droid::Wifi::IScanSettings;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiConnectionStatistics;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiLinkLayerStats;
using Elastos::Droid::Utility::ILruCache;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IQueue;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
 * Track the state of Wifi connectivity. All event handling is done here,
 * and all changes in connectivity state are initiated here.
 *
 * Wi-Fi now supports three modes of operation: Client, SoftAp and p2p
 * In the current implementation, we support concurrent wifi p2p and wifi operation.
 * The WifiStateMachine handles SoftAp and Client operations while WifiP2pService
 * handles p2p operation.
 *
 * @hide
 */
class WifiStateMachine
    : public StateMachine
{
public:
    class SimAuthRequestData
        : public Object
    {
    public:
        Int32 networkId;
        Int32 protocol;
        String ssid;
        AutoPtr<ArrayOf<String> > challenges;
    };

    class SimAuthResponseData
        : public Object
    {
    public:
        Int32 id;
        String Kc1;
        String SRES1;
        String Kc2;
        String SRES2;
        String Kc3;
        String SRES3;
    };

    class DefaultState
        : public State
    {
    public:
        DefaultState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class InitialState
        : public State
    {
    public:
        InitialState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class SupplicantStartingState
        : public State
    {
    public:
        SupplicantStartingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        CARAPI_(void) InitializeWpsDetails();

    private:
        WifiStateMachine* mHost;
    };

    class SupplicantStartedState
        : public State
    {
    public:
        SupplicantStartedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class SupplicantStoppingState
        : public State
    {
    public:
        SupplicantStoppingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class DriverStartingState
        : public State
    {
    public:
        DriverStartingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
        Int32 mTries;
    };

    class DriverStartedState
        : public State
    {
    public:
        DriverStartedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class WaitForP2pDisableState
        : public State
    {
    public:
        WaitForP2pDisableState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
        AutoPtr<IState> mTransitionToState;
    };

    class DriverStoppingState
        : public State
    {
    public:
        DriverStoppingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class DriverStoppedState
        : public State
    {
    public:
        DriverStoppedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class ScanModeState
        : public State
    {
    public:
        ScanModeState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
        Int32 mLastOperationMode;
    };

    class ConnectModeState
        : public State
    {
    public:
        ConnectModeState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class L2ConnectedState
        : public State
    {
    public:
        L2ConnectedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class ObtainingIpState
        : public State
    {
    public:
        ObtainingIpState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class VerifyingLinkState
        : public State
    {
    public:
        VerifyingLinkState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class RoamingState
        : public State
    {
    public:
        RoamingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
        Boolean mAssociated;
    };

    class ConnectedState
        : public State
    {
    public:
        ConnectedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class DisconnectingState
        : public State
    {
    public:
        DisconnectingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class DisconnectedState
        : public State
    {
    public:
        DisconnectedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class WpsRunningState
        : public State
    {
    public:
        WpsRunningState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
        // Tracks the source to provide a reply
        AutoPtr<IMessage> mSourceMessage;
    };

    class SoftApStartingState
        : public State
    {
    public:
        SoftApStartingState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class SoftApStartedState
        : public State
    {
    public:
        SoftApStartedState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class TetheringState
        : public State
    {
    public:
        TetheringState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class TetheredState
        : public State
    {
    public:
        TetheredState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

    class UntetheringState
        : public State
    {
    public:
        UntetheringState(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Enter();

        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WifiStateMachine* mHost;
    };

private:
    class TetherStateChange
        : public Object
    {
    public:
        TetherStateChange(
            /* [in] */ IArrayList* av,
            /* [in] */ IArrayList* ac);

    public:
        AutoPtr<IArrayList> available;
        AutoPtr<IArrayList> active;
    };

    class WifiNetworkFactory
        : public NetworkFactory
    {
    public:
        TO_STRING_IMPL("WifistateMachine::WifiNetworkFactory")

        WifiNetworkFactory(
            /* [in] */ ILooper* l,
            /* [in] */ IContext* c,
            /* [in] */ const String& TAG,
            /* [in] */ INetworkCapabilities* f);

    protected:
        CARAPI StartNetwork();

        CARAPI StopNetwork();
    };

    class WifiNetworkAgent
        : public NetworkAgent
    {
    public:
        TO_STRING_IMPL("WiFiStateMachine::WifiNetworkAgent")

        WifiNetworkAgent(
            /* [in] */ ILooper* l,
            /* [in] */ IContext* c,
            /* [in] */ const String& TAG,
            /* [in] */ INetworkInfo* ni,
            /* [in] */ INetworkCapabilities* nc,
            /* [in] */ ILinkProperties* lp,
            /* [in] */ Int32 score,
            /* [in] */ WifiStateMachine* host);

    protected:
        CARAPI Unwanted();

        CARAPI_(void) NetworkStatus(
            /* [in] */ Int32 status);

    private:
        WifiStateMachine* mHost;
    };

    class StartSoftApWithConfigRunnable
        : public Runnable
    {
    public:
        StartSoftApWithConfigRunnable(
            /* [in] */ WifiStateMachine* host,
            /* [in] */ IWifiConfiguration* config)
            : mHost(host)
            , mConfig(config)
        {}

        CARAPI Run();

    private:
        WifiStateMachine* mHost;
        AutoPtr<IWifiConfiguration> mConfig;
    };

    class NetlinkTrackerCallback
        : public Object
        , public INetlinkTrackerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        NetlinkTrackerCallback(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI Update(
            /* [in] */ ILinkProperties* lp);

    private:
        WifiStateMachine* mHost;
    };

    class TetherStateChangedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WiFiStateMachine::TetherStateChangedBroadcastReceiver")

        TetherStateChangedBroadcastReceiver(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiStateMachine* mHost;
    };

    class StartScanBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WiFiStateMachine::StartScanBroadcastReceiver")

        StartScanBroadcastReceiver(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiStateMachine* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WiFiStateMachine::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiStateMachine* mHost;
    };

    class DelayedDriverStopBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WiFiStateMachine::DelayedDriverStopBroadcastReceiver")

        DelayedDriverStopBroadcastReceiver(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiStateMachine* mHost;
    };

    class BootCompletedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        BootCompletedBroadcastReceiver(
            /* [in] */ WifiStateMachine* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiStateMachine* mHost;
    };

    class RegisterContentObserver
        : public ContentObserver
    {
    public:
        RegisterContentObserver()
        {
        }

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ WifiStateMachine* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiStateMachine* mHost;
    };

public:
    friend class WifiNetworkAgent;
    friend class StartSoftApWithConfigRunnable;

    WifiStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ const String& wlanInterface,
        /* [in] */ WifiTrafficPoller* trafficPoller);

    CARAPI IsRoaming(
        /* [out] */ Boolean* result);

    CARAPI AutoRoamSetBSSID(
        /* [in] */ Int32 netId,
        /* [in] */ const String& bssid);

    CARAPI AutoRoamSetBSSID(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ const String& bssid,
        /* [out] */ Boolean* result);

    CARAPI GetVerboseLoggingLevel(
        /* [out] */ Int32* result);

    CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    CARAPI GetAggressiveHandover(
        /* [out] */ Int32* result);

    CARAPI EnableAggressiveHandover(
        /* [in] */ Int32 enabled);

    CARAPI SetAllowScansWithTraffic(
        /* [in] */ Int32 enabled);

    CARAPI GetAllowScansWithTraffic(
        /* [out] */ Int32* result);

    CARAPI GetMessenger(
        /* [out] */ IMessenger** result);

    CARAPI GetWifiMonitor(
        /* [out] */ WifiMonitor** result);

    CARAPI SyncPingSupplicant(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Boolean* result);

    CARAPI SyncGetChannelList(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IList** result);

    /**
     * Initiate a wifi scan. If workSource is not null, blame is given to it, otherwise blame is
     * given to callingUid.
     *
     * @param callingUid The uid initiating the wifi scan. Blame will be given here unless
     *                   workSource is specified.
     * @param workSource If not null, blame is given to workSource.
     * @param settings Scan settings, see {@link ScanSettings}.
     */
    CARAPI StartScan(
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 scanCounter,
        /* [in] */ IScanSettings* settings,
        /* [in] */ IWorkSource* workSource);

    /**
     * start or stop batched scanning using the given settings
     */
    CARAPI SetBatchedScanSettings(
        /* [in] */ IBatchedScanSettings* settings,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 csph,
        /* [in] */ IWorkSource* workSource);

    CARAPI SyncGetBatchedScanResultsList(
        /* [out] */ IList** result);

    CARAPI RequestBatchedScanPoll();

    CARAPI GetDisconnectedTimeMilli(
        /* [out] */ Int64* result);

    CARAPI ReportOnTime(
        /* [out] */ String* result);

    CARAPI GetWifiLinkLayerStats(
        /* [in] */ Boolean dbg,
        /* [out] */ IWifiLinkLayerStats** result);

    CARAPI StartRadioScanStats();

    CARAPI CloseRadioScanStats();

    CARAPI SetSupplicantRunning(
        /* [in] */ Boolean enable);

    CARAPI SetHostApRunning(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ Boolean enable);

    CARAPI SetWifiApConfiguration(
        /* [in] */ IWifiConfiguration* config);

    CARAPI SyncGetWifiApConfiguration(
        /* [out] */ IWifiConfiguration** result);

    CARAPI SyncGetWifiState(
        /* [out] */ Int32* result);

    CARAPI SyncGetWifiStateByName(
        /* [out] */ String* result);

    CARAPI SyncGetWifiApState(
        /* [out] */ Int32* result);

    CARAPI SyncGetWifiApStateByName(
        /* [out] */ String* result);

    /**
     * Get status information for the current connection, if any.
     * @return a {@link WifiInfo} object containing information about the current connection
     *
     */
    CARAPI SyncRequestConnectionInfo(
        /* [out] */ IWifiInfo** result);

    CARAPI SyncGetDhcpResults(
        /* [out] */ IDhcpResults** result);

    CARAPI SetDriverStart(
        /* [in] */ Boolean enable);

    CARAPI SetOperationalMode(
        /* [in] */ Int32 mode);

    CARAPI SyncGetScanResultsList(
        /* [out] */ IList** result);

    /**
     * Get unsynchronized pointer to scan result list
     * Can be called only from AutoJoinController which runs in the WifiStateMachine context
     */
    CARAPI GetScanResultsListNoCopyUnsync(
        /* [out] */ IList** result);

    /**
     * Disconnect from Access Point
     */
    CARAPI DisconnectCommand();

    /**
     * Initiate a reconnection to AP
     */
    CARAPI ReconnectCommand();

    /**
     * Initiate a re-association to AP
     */
    CARAPI ReassociateCommand();

    /**
     * Reload networks and then reconnect; helps load correct data for TLS networks
     */
    CARAPI ReloadTlsNetworksAndReconnect();

    /**
     * Add a network synchronously
     *
     * @return network id of the new network
     */
    CARAPI SyncAddOrUpdateNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Int32* result);

    /**
     * Get configured networks synchronously
     * @param channel
     * @return
     */
    CARAPI SyncGetConfiguredNetworks(
        /* [in] */ Int32 uuid,
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IList** result);

    CARAPI SyncGetPrivilegedConfiguredNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IList** result);

    /**
     * Get connection statistics synchronously
     * @param channel
     * @return
     */
    CARAPI SyncGetConnectionStatistics(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IWifiConnectionStatistics** result);

    /**
     * Get adaptors synchronously
     */
    CARAPI SyncGetSupportedFeatures(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Int32* result);

    /**
     * Get link layers stats for adapter synchronously
     */
    CARAPI SyncGetLinkLayerStats(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ IWifiLinkLayerStats** result);

    /**
     * Delete a network
     *
     * @param networkId id of the network to be removed
     */
    CARAPI SyncRemoveNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ Int32 networkId,
        /* [out] */ Boolean* result);

    /**
     * Enable a network
     *
     * @param netId network id of the network
     * @param disableOthers true, if all other networks have to be disabled
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     */
    CARAPI SyncEnableNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* result);

    /**
     * Disable a network
     *
     * @param netId network id of the network
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     */
    CARAPI SyncDisableNetwork(
        /* [in] */ IAsyncChannel* channel,
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
     * Retrieves a WPS-NFC configuration token for the specified network
     * @return a hex string representation of the WPS-NFC configuration token
     */
    CARAPI SyncGetWpsNfcConfigurationToken(
        /* [in] */ Int32 netId,
        /* [out] */ String* result);

    /**
     * Blacklist a BSSID. This will avoid the AP if there are
     * alternate APs to connect
     *
     * @param bssid BSSID of the network
     */
    CARAPI AddToBlacklist(
        /* [in] */ const String& bssid);

    /**
     * Clear the blacklist list
     *
     */
    CARAPI ClearBlacklist();

    CARAPI EnableRssiPolling(
        /* [in] */ Boolean enabled);

    CARAPI EnableAllNetworks();

    /**
     * Start filtering Multicast v4 packets
     */
    CARAPI StartFilteringMulticastV4Packets();

    /**
     * Stop filtering Multicast v4 packets
     */
    CARAPI StopFilteringMulticastV4Packets();

    /**
     * Start filtering Multicast v4 packets
     */
    CARAPI StartFilteringMulticastV6Packets();

    /**
     * Stop filtering Multicast v4 packets
     */
    CARAPI StopFilteringMulticastV6Packets();

    /**
     * Set high performance mode of operation.
     * Enabling would set active power mode and disable suspend optimizations;
     * disabling would set auto power mode and enable suspend optimizations
     * @param enable true if enable, FALSE otherwise
     */
    CARAPI SetHighPerfModeEnabled(
        /* [in] */ Boolean enable);

    /**
     * Set the country code
     * @param countryCode following ISO 3166 format
     * @param persist {@code true} if the setting should be remembered.
     */
    CARAPI SetCountryCode(
        /* [in] */ const String& countryCode,
        /* [in] */ Boolean persist);

    CARAPI SyncIsIbssSupported(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Int32* result);

    /**
     * Set the operational frequency band
     * @param band
     * @param persist {@code true} if the setting should be remembered.
     */
    CARAPI SetFrequencyBand(
        /* [in] */ Int32 band,
        /* [in] */ Boolean persist);

    /**
     * Enable TDLS for a specific MAC address
     */
    CARAPI EnableTdls(
        /* [in] */ const String& remoteMacAddress,
        /* [in] */ Boolean enable);

    /**
     * Returns the operational frequency band
     */
    CARAPI GetFrequencyBand(
        /* [out] */ Int32* result);

    /**
     * Returns the wifi configuration file
     */
    CARAPI GetConfigFile(
        /* [out] */ String* result);

    /**
     * Send a message indicating bluetooth adapter connection state changed
     */
    CARAPI SendBluetoothAdapterStateChange(
        /* [in] */ Int32 state);

    /**
     * Save configuration on supplicant
     *
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     *
     * TODO: deprecate this
     */
    CARAPI SyncSaveConfig(
        /* [in] */ IAsyncChannel* channel,
        /* [out] */ Boolean* result);

    CARAPI UpdateBatteryWorkSource(
        /* [in] */ IWorkSource* newSource);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    /**
     *  Determine if we need to switch network:
     * - the delta determine the urgency to switch and/or or the expected evilness of the disruption
     * - match the uregncy of the switch versus the packet usage at the interface
     */
    CARAPI ShouldSwitchNetwork(
        /* [in] */ Int32 networkDelta,
        /* [out] */ Boolean* result);

    CARAPI GetTxPacketRate(
        /* [out] */ Double* result);

    CARAPI GetRxPacketRate(
        /* [out] */ Double* result);

    CARAPI HandlePreDhcpSetup();

    CARAPI StartDhcp();

    CARAPI RenewDhcp();

    CARAPI StopDhcp();

    CARAPI HandlePostDhcpSetup();

    CARAPI SmToString(
        /* [in] */ IMessage* message,
        /* [out] */ String* result);

    CARAPI SmToString(
        /* [in] */ Int32 what,
        /* [out] */ String* result);

    CARAPI RegisterConnected();

    CARAPI RegisterDisconnected();

    CARAPI NoteWifiDisabledWhileAssociated();

    CARAPI SetInternetAccessState(
        /* [in] */ Boolean enabled);

    CARAPI GetCurrentWifiConfiguration(
        /* [out] */ IWifiConfiguration** result);

    CARAPI GetCurrentScanResult(
        /* [out] */ IScanResult** result);

    CARAPI GetCurrentBSSID(
        /* [out] */ String* result);

    CARAPI UnwantedNetwork(
        /* [in] */ Int32 reason);

    CARAPI StartScanForConfiguration(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ Boolean restrictChannelList,
        /* [out] */ Boolean* result);

    CARAPI ClearCurrentConfigBSSID(
        /* [in] */ const String& dbg);

    CARAPI ClearConfigBSSID(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ const String& dbg);

    CARAPI HandleGsmAuthRequest(
        /* [in] */ SimAuthRequestData* requestData);

    CARAPI Handle3GAuthRequest(
        /* [in] */ SimAuthRequestData* requestData);

protected:
    /**
     * Log with error attribute
     *
     * @param s is string log
     */
    CARAPI_(void) Loge(
        /* [in] */ const String& s);

    CARAPI_(void) Logd(
        /* [in] */ const String& s);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

    /**
     * Return the additional string to be logged by LogRec, default
     *
     * @param msg that was processed
     * @return information to be logged as a String
     */
    CARAPI_(String) GetLogRecString(
        /* [in] */ IMessage* msg);

private:
    CARAPI_(void) SetScanAlarm(
        /* [in] */ Boolean enabled,
        /* [in] */ Int32 delayMilli);

    CARAPI_(Boolean) SetRandomMacOui();

    CARAPI_(void) StartBatchedScan();

    // called from BroadcastListener
    CARAPI_(void) StartNextBatchedScanAsync();

    CARAPI_(void) StartNextBatchedScan();

    CARAPI_(void) HandleBatchedScanPollRequest();

    // return true if new/different
    CARAPI_(Boolean) RecordBatchedScanSettings(
        /* [in] */ Int32 responsibleUid,
        /* [in] */ Int32 csph,
        /* [in] */ IBundle* bundle);

    CARAPI_(void) StopBatchedScan();

    CARAPI_(void) SetNextBatchedAlarm(
        /* [in] */ Int32 scansExpected);

    CARAPI_(void) RetrieveBatchedScanData();

    //TODO: this is used only to track connection attempts, however the link state and packet per
    //TODO: second logic should be folded into that
    CARAPI_(Boolean) IsScanAllowed(
        /* [in] */ Int32 scanSource);

    // If workSource is not null, blame is given to it, otherwise blame is given to callingUid.
    CARAPI_(void) NoteScanStart(
        /* [in] */ Int32 callingUid,
        /* [in] */ IWorkSource* workSource);

    CARAPI_(void) NoteScanEnd();

    CARAPI_(void) NoteBatchedScanStart();

    CARAPI_(void) NoteBatchedScanStop();

    CARAPI_(void) HandleScanRequest(
        /* [in] */ Int32 type,
        /* [in] */ IMessage* message);

    /** return true iff scan request is accepted */
    CARAPI_(Boolean) StartScanNative(
        /* [in] */ Int32 type,
        /* [in] */ const String& freqs);

    CARAPI_(void) LogStateAndMessage(
        /* [in] */ IMessage* message,
        /* [in] */ const String& state);

    CARAPI_(void) HandleBSSIDBlacklist(
        /* [in] */ Boolean enable,
        /* [in] */ const String& bssid,
        /* [in] */ Int32 reason);

    CARAPI_(void) HandleStateChange(
        /* [in] */ Int32 state);

    CARAPI_(void) HandlePrefChange(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 featureParam,
        /* [in] */ Int32 value);

    CARAPI_(void) HandleScreenStateChanged(
        /* [in] */ Boolean screenOn,
        /* [in] */ Boolean startBackgroundScanIfNeeded);

    CARAPI_(void) CheckAndSetConnectivityInstance();

    CARAPI_(Boolean) StartTethering(
        /* [in] */ IArrayList* available);

    CARAPI_(void) StopTethering();

    CARAPI_(Boolean) IsWifiTethered(
        /* [in] */ IArrayList* active);

    /**
     * Set the country code from the system setting value, if any.
     */
    CARAPI_(void) SetCountryCode();

    /**
     * Set the frequency band from the system setting value, if any.
     */
    CARAPI_(void) SetFrequencyBand();

    CARAPI_(void) SetSuspendOptimizationsNative(
        /* [in] */ Int32 reason,
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetSuspendOptimizations(
        /* [in] */ Int32 reason,
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetWifiState(
        /* [in] */ Int32 wifiState);

    CARAPI_(void) SetWifiApState(
        /* [in] */ Int32 wifiApState);

    /**
     * Format:
     *
     * id=1
     * bssid=68:7f:76:d7:1a:6e
     * freq=2412
     * level=-44
     * tsf=1344626243700342
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=zfdy
     * ====
     * id=2
     * bssid=68:5f:74:d7:1a:6f
     * freq=5180
     * level=-73
     * tsf=1344626243700373
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=zuby
     * ====
     */
    CARAPI_(void) SetScanResults();

    /*
     * Fetch RSSI, linkspeed, and frequency on current connection
     */
    CARAPI_(void) FetchRssiLinkSpeedAndFrequencyNative();

    // Polling has completed, hence we wont have a score anymore
    CARAPI_(void) CleanWifiScore();

    CARAPI_(void) CalculateWifiScore(
        /* [in] */ IWifiLinkLayerStats* stats);

    /**
     * Fetch TX packet counters on current connection
     */
    CARAPI_(void) FetchPktcntNative(
        /* [in] */ IRssiPacketCountInfo* info);

    CARAPI_(Boolean) ClearIPv4Address(
        /* [in] */ const String& iface);

    CARAPI_(Boolean) IsProvisioned(
        /* [in] */ ILinkProperties* lp);

    /**
     * Updates mLinkProperties by merging information from various sources.
     *
     * This is needed because the information in mLinkProperties comes from multiple sources (DHCP,
     * netlink, static configuration, ...). When one of these sources of information has updated
     * link properties, we can't just assign them to mLinkProperties or we'd lose track of the
     * information that came from other sources. Instead, when one of those sources has new
     * information, we update the object that tracks the information from that source and then
     * call this method to apply the change to mLinkProperties.
     *
     * The information in mLinkProperties is currently obtained as follows:
     * - Interface name: set in the constructor.
     * - IPv4 and IPv6 addresses: netlink, passed in by mNetlinkTracker.
     * - IPv4 routes, DNS servers, and domains: DHCP.
     * - IPv6 routes and DNS servers: netlink, passed in by mNetlinkTracker.
     * - HTTP proxy: the wifi config store.
     */
    CARAPI_(void) UpdateLinkProperties(
        /* [in] */ Int32 reason,
        /* [in] */ ILinkProperties* lp);

    /**
     * Clears all our link properties.
     */
    CARAPI_(void) ClearLinkProperties();

    /**
     * try to update default route MAC address.
     */
    CARAPI_(String) UpdateDefaultRouteMacAddress(
        /* [in] */ Int32 timeout);

    CARAPI_(void) SendScanResultsAvailableBroadcast();

    CARAPI_(void) SendRssiChangeBroadcast(
        /* [in] */ Int32 newRssi);

    CARAPI_(void) SendNetworkStateChangeBroadcast(
        /* [in] */ const String& bssid);

    CARAPI_(void) SendLinkConfigurationChangedBroadcast();

    CARAPI_(void) SendSupplicantConnectionChangedBroadcast(
        /* [in] */ Boolean connected);

    /**
     * Record the detailed state of a network.
     * @param state the new {@code DetailedState}
     */
    CARAPI_(Boolean) SetNetworkDetailedState(
        /* [in] */ NetworkInfoDetailedState state);

    CARAPI_(NetworkInfoDetailedState) GetNetworkDetailedState();

    CARAPI_(SupplicantState) HandleSupplicantStateChange(
        /* [in] */ IMessage* message);

    /**
     * Resets the Wi-Fi Connections by clearing any state, resetting any sockets
     * using the interface, stopping DHCP & disabling interface
     */
    CARAPI_(void) HandleNetworkDisconnect();

    CARAPI_(void) HandleSupplicantConnectionLoss(
        /* [in] */ Boolean killSupplicant);

    CARAPI_(void) HandleIPv4Success(
        /* [in] */ IDhcpResults* dhcpResults,
        /* [in] */ Int32 reason);

    CARAPI_(void) HandleSuccessfulIpConfiguration();

    CARAPI_(void) HandleIPv4Failure(
        /* [in] */ Int32 reason);

    CARAPI_(void) HandleIpConfigurationLost();

    /* Current design is to not set the config on a running hostapd but instead
     * stop and start tethering when user changes config on a running access point
     *
     * TODO: Add control channel setup through hostapd that allows changing config
     * on a running daemon
     */
    CARAPI_(void) StartSoftApWithConfig(
        /* [in] */ IWifiConfiguration* config);

    /*
     * Read a MAC address in /proc/arp/table, used by WifistateMachine
     * so as to record MAC address of default gateway.
     **/
    CARAPI_(String) MacAddressFromRoute(
        /* [in] */ const String& ipAddress);

    CARAPI_(void) SendConnectedState();

    //State machine initiated requests can have replyTo set to null indicating
    //there are no recepients, we ignore those reply actions
    CARAPI_(void) ReplyToMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 what);

    CARAPI_(void) ReplyToMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    CARAPI_(void) ReplyToMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * arg2 on the source message has a unique id that needs to be retained in replies
     * to match the request

     * see WifiManager for details
     */
    CARAPI_(AutoPtr<IMessage>) ObtainMessageWithArg2(
        /* [in] */ IMessage* srcMsg);

    static CARAPI ParseHex(
        /* [in] */ Byte ch,
        /* [out] */ Int32* result);

    CARAPI ParseHex(
        /* [in] */ const String& hex,
        /* [out] */ ArrayOf<Byte>** result);

    static CARAPI_(String) MakeHex(
        /* [in] */ ArrayOf<Byte>* bytes);

    static CARAPI_(String) MakeHex(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 from,
        /* [in] */ Int32 len);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) Concat(
        /* [in] */ ArrayOf<Byte>* array1,
        /* [in] */ ArrayOf<Byte>* array2,
        /* [in] */ ArrayOf<Byte>* array3);

    CARAPI_(void) SetScanIntevelOnMiracastModeChange(
        /* [in] */ Int32 mode);

    CARAPI_(void) HandlePnoFailError();

public:
    /* The base for wifi message types */
    static const Int32 BASE = IProtocol::BASE_WIFI;
    /* Start the supplicant */
    static const Int32 CMD_START_SUPPLICANT                = BASE + 11;
    /* Stop the supplicant */
    static const Int32 CMD_STOP_SUPPLICANT                 = BASE + 12;
    /* Start the driver */
    static const Int32 CMD_START_DRIVER                    = BASE + 13;
    /* Stop the driver */
    static const Int32 CMD_STOP_DRIVER                     = BASE + 14;
    /* Indicates Static IP succeeded */
    static const Int32 CMD_STATIC_IP_SUCCESS               = BASE + 15;
    /* Indicates Static IP failed */
    static const Int32 CMD_STATIC_IP_FAILURE               = BASE + 16;
    /* Indicates supplicant stop failed */
    static const Int32 CMD_STOP_SUPPLICANT_FAILED          = BASE + 17;
    /* Delayed stop to avoid shutting down driver too quick*/
    static const Int32 CMD_DELAYED_STOP_DRIVER             = BASE + 18;
    /* A delayed message sent to start driver when it fail to come up */
    static const Int32 CMD_DRIVER_START_TIMED_OUT          = BASE + 19;

    /* Start the soft access point */
    static const Int32 CMD_START_AP                        = BASE + 21;
    /* Indicates soft ap start succeeded */
    static const Int32 CMD_START_AP_SUCCESS                = BASE + 22;
    /* Indicates soft ap start failed */
    static const Int32 CMD_START_AP_FAILURE                = BASE + 23;
    /* Stop the soft access point */
    static const Int32 CMD_STOP_AP                         = BASE + 24;
    /* Set the soft access point configuration */
    static const Int32 CMD_SET_AP_CONFIG                   = BASE + 25;
    /* Soft access point configuration set completed */
    static const Int32 CMD_SET_AP_CONFIG_COMPLETED         = BASE + 26;
    /* Request the soft access point configuration */
    static const Int32 CMD_REQUEST_AP_CONFIG               = BASE + 27;
    /* Response to access point configuration request */
    static const Int32 CMD_RESPONSE_AP_CONFIG              = BASE + 28;
    /* Invoked when getting a tether state change notification */
    static const Int32 CMD_TETHER_STATE_CHANGE             = BASE + 29;
    /* A delayed message sent to indicate tether state change failed to arrive */
    static const Int32 CMD_TETHER_NOTIFICATION_TIMED_OUT   = BASE + 30;

    static const Int32 CMD_BLUETOOTH_ADAPTER_STATE_CHANGE  = BASE + 31;

    /* Supplicant commands */
    /* Is supplicant alive ? */
    static const Int32 CMD_PING_SUPPLICANT                 = BASE + 51;
    /* Add/update a network configuration */
    static const Int32 CMD_ADD_OR_UPDATE_NETWORK           = BASE + 52;
    /* Delete a network */
    static const Int32 CMD_REMOVE_NETWORK                  = BASE + 53;
    /* Enable a network. The device will attempt a connection to the given network. */
    static const Int32 CMD_ENABLE_NETWORK                  = BASE + 54;
    /* Enable all networks */
    static const Int32 CMD_ENABLE_ALL_NETWORKS             = BASE + 55;
    /* Blacklist network. De-prioritizes the given BSSID for connection. */
    static const Int32 CMD_BLACKLIST_NETWORK               = BASE + 56;
    /* Clear the blacklist network list */
    static const Int32 CMD_CLEAR_BLACKLIST                 = BASE + 57;
    /* Save configuration */
    static const Int32 CMD_SAVE_CONFIG                     = BASE + 58;
    /* Get configured networks */
    static const Int32 CMD_GET_CONFIGURED_NETWORKS         = BASE + 59;
    /* Get available frequencies */
    static const Int32 CMD_GET_CAPABILITY_FREQ             = BASE + 60;
    /* Get adaptors */
    static const Int32 CMD_GET_SUPPORTED_FEATURES          = BASE + 61;
    /* Get configured networks with real preSharedKey */
    static const Int32 CMD_GET_PRIVILEGED_CONFIGURED_NETWORKS = BASE + 62;
    /* Get Link Layer Stats thru HAL */
    static const Int32 CMD_GET_LINK_LAYER_STATS            = BASE + 63;
    /* Supplicant commands after driver start*/
    /* Initiate a scan */
    static const Int32 CMD_START_SCAN                      = BASE + 71;
    /* Set operational mode. CONNECT, SCAN ONLY, SCAN_ONLY with Wi-Fi off mode */
    static const Int32 CMD_SET_OPERATIONAL_MODE            = BASE + 72;
    /* Disconnect from a network */
    static const Int32 CMD_DISCONNECT                      = BASE + 73;
    /* Reconnect to a network */
    static const Int32 CMD_RECONNECT                       = BASE + 74;
    /* Reassociate to a network */
    static const Int32 CMD_REASSOCIATE                     = BASE + 75;
    /* Get Connection Statistis */
    static const Int32 CMD_GET_CONNECTION_STATISTICS       = BASE + 76;

    /* Controls suspend mode optimizations
     *
     * When high perf mode is enabled, suspend mode optimizations are disabled
     *
     * When high perf mode is disabled, suspend mode optimizations are enabled
     *
     * Suspend mode optimizations include:
     * - packet filtering
     * - turn off roaming
     * - DTIM wake up settings
     */
    static const Int32 CMD_SET_HIGH_PERF_MODE              = BASE + 77;
    /* Set the country code */
    static const Int32 CMD_SET_COUNTRY_CODE                = BASE + 80;
    /* Enables RSSI poll */
    static const Int32 CMD_ENABLE_RSSI_POLL                = BASE + 82;
    /* RSSI poll */
    static const Int32 CMD_RSSI_POLL                       = BASE + 83;
    /* Set up packet filtering */
    static const Int32 CMD_START_PACKET_FILTERING          = BASE + 84;
    /* Clear packet filter */
    static const Int32 CMD_STOP_PACKET_FILTERING           = BASE + 85;
    /* Enable suspend mode optimizations in the driver */
    static const Int32 CMD_SET_SUSPEND_OPT_ENABLED         = BASE + 86;
    /* Delayed NETWORK_DISCONNECT */
    static const Int32 CMD_DELAYED_NETWORK_DISCONNECT      = BASE + 87;
    /* When there are no saved networks, we do a periodic scan to notify user of
     * an open network */
    static const Int32 CMD_NO_NETWORKS_PERIODIC_SCAN       = BASE + 88;
    /* Test network Disconnection NETWORK_DISCONNECT */
    static const Int32 CMD_TEST_NETWORK_DISCONNECT         = BASE + 89;

    /* arg1 values to CMD_STOP_PACKET_FILTERING and CMD_START_PACKET_FILTERING */
    static const Int32 MULTICAST_V6 = 1;
    static const Int32 MULTICAST_V4 = 0;

   /* Set the frequency band */
    static const Int32 CMD_SET_FREQUENCY_BAND              = BASE + 90;
    /* When there are saved networks and PNO fails, we do a periodic scan to notify
       a saved/open network in suspend mode */
    static const Int32 CMD_PNO_PERIODIC_SCAN               = BASE + 91;
    /* Enable TDLS on a specific MAC address */
    static const Int32 CMD_ENABLE_TDLS                     = BASE + 92;
    /* DHCP/IP configuration watchdog */
    static const Int32 CMD_OBTAINING_IP_ADDRESS_WATCHDOG_TIMER   = BASE + 93;

    /**
     * Make this timer 40 seconds, which is about the normal DHCP timeout.
     * In no valid case, the WiFiStateMachine should remain stuck in ObtainingIpAddress
     * for more than 30 seconds.
     */
    static const Int32 OBTAINING_IP_ADDRESS_GUARD_TIMER_MSEC = 40000;

    Int32 obtainingIpWatchdogCount; //= 0;

    /* Commands from/to the SupplicantStateTracker */
    /* Reset the supplicant state tracker */
    static const Int32 CMD_RESET_SUPPLICANT_STATE          = BASE + 111;

    /**
     * Watchdog for protecting against b/16823537
     * Leave time for 4-ways handshake to succeed
     */
    static const Int32 ROAM_GUARD_TIMER_MSEC = 15000;

    Int32 roamWatchdogCount; //= 0;
    /* Roam state watchdog */
    static const Int32 CMD_ROAM_WATCHDOG_TIMER   = BASE + 94;
    /* Screen change intent handling */
    static const Int32 CMD_SCREEN_STATE_CHANGED             = BASE + 95;

    Int32 disconnectingWatchdogCount; //= 0;
    static const Int32 DISCONNECTING_GUARD_TIMER_MSEC = 5000;

    /* Disconnecting state watchdog */
    static const Int32 CMD_DISCONNECTING_WATCHDOG_TIMER    = BASE + 96;

    /* P2p commands */
    /* We are ok with no response here since we wont do much with it anyway */
    static const Int32 CMD_ENABLE_P2P               = BASE + 131;
    /* In order to shut down supplicant cleanly, we wait till p2p has
     * been disabled */
    static const Int32 CMD_DISABLE_P2P_REQ          = BASE + 132;
    static const Int32 CMD_DISABLE_P2P_RSP          = BASE + 133;

    static const Int32 CMD_BOOT_COMPLETED           = BASE + 134;

    /* change the batch scan settings.
     * arg1 = responsible UID
     * arg2 = csph (channel scans per hour)
     * obj = bundle with the new settings and the optional worksource
     */
    static const Int32 CMD_SET_BATCHED_SCAN         = BASE + 135;
    static const Int32 CMD_START_NEXT_BATCHED_SCAN  = BASE + 136;
    static const Int32 CMD_POLL_BATCHED_SCAN        = BASE + 137;

    /* We now have a valid IP configuration. */
    static const Int32 CMD_IP_CONFIGURATION_SUCCESSFUL     = BASE + 138;
    /* We no longer have a valid IP configuration. */
    static const Int32 CMD_IP_CONFIGURATION_LOST           = BASE + 139;
    /* Link configuration (IP address, DNS, ...) changes notified via netlink */
    static const Int32 CMD_UPDATE_LINKPROPERTIES           = BASE + 140;

    /* Supplicant is trying to associate to a given BSSID */
    static const Int32 CMD_TARGET_BSSID                    = BASE + 141;

    /* Reload all networks and reconnect */
    static const Int32 CMD_RELOAD_TLS_AND_RECONNECT        = BASE + 142;

    static const Int32 CMD_AUTO_CONNECT                    = BASE + 143;

    static const Int32 network_status_unwanted_disconnect = 0;
    static const Int32 network_status_unwanted_disable_autojoin = 1;

    static const Int32 CMD_UNWANTED_NETWORK                = BASE + 144;

    static const Int32 CMD_AUTO_ROAM                       = BASE + 145;

    static const Int32 CMD_AUTO_SAVE_NETWORK               = BASE + 146;

    static const Int32 CMD_ASSOCIATED_BSSID               = BASE + 147;

    /* Supplicant is trying to associate to a given SSID */
    static const Int32 CMD_TARGET_SSID                    = BASE + 148;

    /* Is IBSS mode supported by the driver? */
    static const Int32 CMD_GET_IBSS_SUPPORTED       = BASE + 149;

    /* Wifi state machine modes of operation */
    /* CONNECT_MODE - connect to any 'known' AP when it becomes available */
    static const Int32 CONNECT_MODE                  = 1;
    /* SCAN_ONLY_MODE - don't connect to any APs; scan, but only while apps hold lock */
    static const Int32 SCAN_ONLY_MODE                = 2;
    /* SCAN_ONLY_WITH_WIFI_OFF - scan, but don't connect to any APs */
    static const Int32 SCAN_ONLY_WITH_WIFI_OFF_MODE  = 3;

    Int32 mRunningBeaconCount; //= 0;

    Int32 emptyScanResultCount; //= 0;

    Int32 mBadLinkspeedcount; //= 0;

    // For debug, provide information about the last scoring operation
    String wifiScoringReport;

private:
    static const String NETWORKTYPE; //= "WIFI";
    static Boolean DBG; //= FALSE;
    static Boolean VDBG; //= FALSE;
    static Boolean VVDBG; //= FALSE;
    static Boolean mLogMessages; //= FALSE;

    static const Int32 ONE_HOUR_MILLI; //= 1000 * 60 * 60;

    static const String GOOGLE_OUI; //= "DA-A1-19";

    static const String EXTRA_FEATURE_ID; //= "cneFeatureId";

    static const String EXTRA_FEATURE_PARAMETER; //= "cneFeatureParameter";

    static const String EXTRA_PARAMETER_VALUE; //= "cneParameterValue";

    static const Int32 FEATURE_ID; //= 1;

    static const Int32 FEATURE_PARAM; //= 1;

    static const Int32 FEATURE_OFF; //= 1;

    static const Int32 FEATURE_ON; //= FEATURE_OFF + 1;

    /* temporary debug flag - best network selection development */
    static Boolean PDBG; //= FALSE;

    /* debug flag, indicating if handling of ASSOCIATION_REJECT ended up blacklisting
     * the corresponding BSSID.
     */
    Boolean didBlackListBSSID; //= FALSE;

    AutoPtr<WifiMonitor> mWifiMonitor;
    AutoPtr<WifiNative> mWifiNative;
    AutoPtr<WifiConfigStore> mWifiConfigStore;
    AutoPtr<WifiAutoJoinController> mWifiAutoJoinController;
    AutoPtr<IINetworkManagementService> mNwService;
    AutoPtr<IConnectivityManager> mCm;

    Boolean mP2pSupported;
    Boolean mIbssSupported;
    AutoPtr<IAtomicBoolean> mP2pConnected; //= new AtomicBoolean(FALSE);
    Boolean mTemporarilyDisconnectWifi; //= FALSE;
    String mPrimaryDeviceType;

    /* Scan results handling */
    AutoPtr<IArrayList> mScanResults; //= new ArrayList<ScanResult>();
    static AutoPtr<IPattern> scanResultPattern; //= Pattern.compile("\t+");
    static const Int32 SCAN_RESULT_CACHE_SIZE; //= 160;
    AutoPtr<ILruCache> mScanResultCache; //LruCache<String, AutoPtr<IScanResult> >
    // For debug, number of known scan results that were found as part of last scan result event,
    // as well the number of scans results returned by the supplicant with that message
    Int32 mNumScanResultsKnown;
    Int32 mNumScanResultsReturned;
    Int32 mWpsNetworkId; //= -1;
    Boolean mWpsSuccess; //= FALSE;
    const Int32 reason3; //= 3;

    /* Batch scan results */
    AutoPtr<IList> mBatchedScanResults; //= new ArrayList<BatchedScanResult>();
    Int32 mBatchedScanOwnerUid; //= UNKNOWN_SCAN_SOURCE;
    Int32 mExpectedBatchedScans; //= 0;
    Int64 mBatchedScanMinPollTime; //= 0;

    Boolean mScreenOn; //= FALSE;

    /* Chipset supports background scan */
    Boolean mBackgroundScanSupported;

    /* Flag to verify backgroundScan is configured successfully */
    Boolean mBackgroundScanConfigured; //= FALSE;

    String mInterfaceName;
    /* Tethering interface could be separate from wlan interface */
    String mTetherInterfaceName;

    Int32 mLastSignalLevel; //= -1;
    String mLastBssid;
    Int32 mLastNetworkId; // The network Id we successfully joined
    Boolean linkDebouncing; //= FALSE;

    // Testing various network disconnect cases by sending lots of spurious
    // disconnect to supplicant
    Boolean testNetworkDisconnect; //= FALSE;

    Boolean mEnableRssiPolling; //= FALSE;
    Boolean mEnableBackgroundScan; //= FALSE;
    Boolean mDisabled5GhzFrequencies; //= FALSE;
    Int32 mRssiPollToken; //= 0;
    /* 3 operational states for STA operation: CONNECT_MODE, SCAN_ONLY_MODE, SCAN_ONLY_WIFI_OFF_MODE
    * In CONNECT_MODE, the STA can scan and connect to an access point
    * In SCAN_ONLY_MODE, the STA can only scan for access points
    * In SCAN_ONLY_WIFI_OFF_MODE, the STA can only scan for access points with wifi toggle being off
    */
    Int32 mOperationalMode; //= CONNECT_MODE;
    Boolean mIsScanOngoing; //= FALSE;
    Boolean mIsFullScanOngoing; //= FALSE;
    AutoPtr<IQueue> mBufferedScanMsg; //= new LinkedList<Message>();
    AutoPtr<IWorkSource> mScanWorkSource;
    static const Int32 UNKNOWN_SCAN_SOURCE; //= -1;
    static const Int32 SCAN_ALARM_SOURCE; //= -2;
    static const Int32 ADD_OR_UPDATE_SOURCE; //= -3;

    static const Int32 SCAN_REQUEST_BUFFER_MAX_SIZE; //= 10;
    static const String CUSTOMIZED_SCAN_SETTING; //= "customized_scan_settings";
    static const String CUSTOMIZED_SCAN_WORKSOURCE; //= "customized_scan_worksource";
    static const String SCAN_REQUEST_TIME; //= "scan_request_time";

    static const String BATCHED_SETTING; //= "batched_settings";
    static const String BATCHED_WORKSOURCE; //= "batched_worksource";

    /* Tracks if state machine has received any screen state change broadcast yet.
     * We can miss one of these at boot.
     */
    AutoPtr<IAtomicBoolean> mScreenBroadcastReceived; //= new AtomicBoolean(FALSE);

    Boolean mBluetoothConnectionActive; //= FALSE;

    AutoPtr<IPowerManagerWakeLock> mSuspendWakeLock;

    /**
     * Interval in milliseconds between polling for RSSI
     * and linkspeed information
     */
    static const Int32 POLL_RSSI_INTERVAL_MSECS; //= 3000;

    /**
     * Interval in milliseconds between receiving a disconnect event
     * while connected to a good AP, and handling the disconnect proper
     */
    static const Int32 LINK_FLAPPING_DEBOUNCE_MSEC; //= 7000;

    /**
     * Delay between supplicant restarts upon failure to establish connection
     */
    static const Int32 SUPPLICANT_RESTART_INTERVAL_MSECS; //= 5000;

    /**
     * Number of times we attempt to restart supplicant
     */
    static const Int32 SUPPLICANT_RESTART_TRIES; //= 5;

    Int32 mSupplicantRestartCount; //= 0;
    /* Tracks sequence number on stop failure message */
    Int32 mSupplicantStopFailureToken; //= 0;

    /**
     * Tether state change notification time out
     */
    static const Int32 TETHER_NOTIFICATION_TIME_OUT_MSECS; //= 5000;

    /* Tracks sequence number on a tether notification time out */
    Int32 mTetherToken; //= 0;

    /**
     * Driver start time out.
     */
    static const Int32 DRIVER_START_TIME_OUT_MSECS; //= 10000;

    /* Tracks sequence number on a driver time out */
    Int32 mDriverStartToken; //= 0;

    /**
     * The link properties of the wifi interface.
     * Do not modify this directly; use updateLinkProperties instead.
     */
    AutoPtr<ILinkProperties> mLinkProperties;

    /* Tracks sequence number on a periodic scan message */
    Int32 mPeriodicScanToken; //= 0;

    /* Tracks sequence number on a periodic scan message in case of PNO failure */
    Int32 mPnoPeriodicScanToken; //= 0;

    // Wakelock held during wifi start/stop and driver load/unload
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<IContext> mContext;

    Object mDhcpResultsLock;
    AutoPtr<IDhcpResults> mDhcpResults;
    AutoPtr<IWifiInfo> mWifiInfo;
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;
    AutoPtr<SupplicantStateTracker> mSupplicantStateTracker;
    AutoPtr<IDhcpStateMachine> mDhcpStateMachine;
    Boolean mDhcpActive; //= FALSE;

    Int32 mWifiLinkLayerStatsSupported; //= 4; // Temporary disable

    AutoPtr<IAtomicInteger32> mCountryCodeSequence; //= new AtomicInteger();

    // Whether the state machine goes thru the Disconnecting->Disconnected->ObtainingIpAddress
    Int32 mAutoRoaming; //= IWifiAutoJoinController::AUTO_JOIN_IDLE;

    // Roaming failure count
    Int32 mRoamFailCount; //= 0;

    // This is the BSSID we are trying to associate to, it can be set to "any"
    // if we havent selected a BSSID for joining.
    // The BSSID we are associated to is found in mWifiInfo
    String mTargetRoamBSSID; //= "any";

    Int64 mLastDriverRoamAttempt; //= 0;

    AutoPtr<IWifiConfiguration> targetWificonfiguration;

    // Used as debug to indicate which configuration last was saved
    AutoPtr<IWifiConfiguration> lastSavedConfigurationAttempt;

    // Used as debug to indicate which configuration last was removed
    AutoPtr<IWifiConfiguration> lastForgetConfigurationAttempt;

    /**
     * Subset of link properties coming from netlink.
     * Currently includes IPv4 and IPv6 addresses. In the future will also include IPv6 DNS servers
     * and domains obtained from router advertisements (RFC 6106).
     */
    AutoPtr<INetlinkTracker> mNetlinkTracker;

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mScanIntent;
    AutoPtr<IPendingIntent> mDriverStopIntent;
    AutoPtr<IPendingIntent> mBatchedScanIntervalIntent;

    /* Tracks current frequency mode */
    AutoPtr<IAtomicInteger32> mFrequencyBand; //= new AtomicInteger(WifiManager.WIFI_FREQUENCY_BAND_AUTO);

    /* Tracks if we are filtering Multicast v4 packets. Default is to filter. */
    AutoPtr<IAtomicBoolean> mFilteringMulticastV4Packets; //= new AtomicBoolean(true);

    // Channel for sending replies.
    AutoPtr<IAsyncChannel> mReplyChannel; //= new AsyncChannel();

    AutoPtr<WifiP2pServiceImpl> mWifiP2pServiceImpl;

    // Used to initiate a connection with WifiP2pService
    AutoPtr<IAsyncChannel> mWifiP2pChannel;
    AutoPtr<IAsyncChannel> mWifiApConfigChannel;

    AutoPtr<WifiNetworkFactory> mNetworkFactory;
    AutoPtr<WifiNetworkAgent> mNetworkAgent;

    // Keep track of various statistics, for retrieval by System Apps, i.e. under @SystemApi
    // We should really persist that into the networkHistory.txt file, and read it back when
    // WifiStateMachine starts up
    AutoPtr<IWifiConnectionStatistics> mWifiConnectionStatistics; //= new WifiConnectionStatistics();

    // Used to filter out requests we couldn't possibly satisfy.
    AutoPtr<INetworkCapabilities> mNetworkCapabilitiesFilter; //= new NetworkCapabilities();

    Int32 testNetworkDisconnectCounter; //= 0;

    static const Int32 SUCCESS; //= 1;
    static const Int32 FAILURE; //= -1;

    /* Tracks if suspend optimizations need to be disabled by DHCP,
     * screen or due to high perf mode.
     * When any of them needs to disable it, we keep the suspend optimizations
     * disabled
     */
    Int32 mSuspendOptNeedsDisabled; //= 0;

    static const Int32 SUSPEND_DUE_TO_DHCP      ; //= 1;
    static const Int32 SUSPEND_DUE_TO_HIGH_PERF ; //= 1<<1;
    static const Int32 SUSPEND_DUE_TO_SCREEN    ; //= 1<<2;

    /* Tracks if user has enabled suspend optimizations through settings */
    AutoPtr<IAtomicBoolean> mUserWantsSuspendOpt; //= new AtomicBoolean(true);

    /**
     * Default framework scan interval in milliseconds. This is used in the scenario in which
     * wifi chipset does not support background scanning to set up a
     * periodic wake up scan so that the device can connect to a new access
     * point on the move. {@link Settings.Global#WIFI_FRAMEWORK_SCAN_INTERVAL_MS} can
     * override this.
     */
    Int32 mDefaultFrameworkScanIntervalMs;

    Int32 mDisconnectedScanPeriodMs; //= 10000;

    /**
     * Supplicant scan interval in milliseconds.
     * Comes from {@link Settings.Global#WIFI_SUPPLICANT_SCAN_INTERVAL_MS} or
     * from the default config if the setting is not set
     */
    Int64 mSupplicantScanIntervalMs;

    /**
     * timeStamp of last full band scan we perfoemed for autojoin while connected with screen lit
     */
    Int64 lastFullBandConnectedTimeMilli;

    /**
     * time interval to the next full band scan we will perform for
     * autojoin while connected with screen lit
     */
    Int64 fullBandConnectedTimeIntervalMilli;

    /**
     * max time interval to the next full band scan we will perform for
     * autojoin while connected with screen lit
     * Max time is 5 minutes
     */
    static const Int64 maxFullBandConnectedTimeIntervalMilli; //= 1000 * 60 * 5;

    /**
     * Minimum time interval between enabling all networks.
     * A device can end up repeatedly connecting to a bad network on screen on/off toggle
     * due to enabling every time. We add a threshold to avoid this.
     */
    static const Int32 MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS; //= 10 * 60 * 1000; /* 10 minutes */
    Int64 mLastEnableAllNetworksTime;

    /**
     * Starting and shutting down driver too quick causes problems leading to driver
     * being in a bad state. Delay driver stop.
     */
    Int32 mDriverStopDelayMs;
    Int32 mDelayedStopCounter;
    Boolean mInDelayedStop; //= FALSE;

    // sometimes telephony gives us this data before boot is complete and we can't store it
    // until after, so the write is deferred
    /*volatile*/ String mPersistedCountryCode;

    // Supplicant doesn't like setting the same country code multiple times (it may drop
    // currently connected network), so we save the country code here to avoid redundency
    String mLastSetCountryCode;

    /* Default parent state */
    AutoPtr<State> mDefaultState; //= new DefaultState();
    /* Temporary initial state */
    AutoPtr<State> mInitialState; //= new InitialState();
    /* Driver loaded, waiting for supplicant to start */
    AutoPtr<State> mSupplicantStartingState; //= new SupplicantStartingState();
    /* Driver loaded and supplicant ready */
    AutoPtr<State> mSupplicantStartedState; //= new SupplicantStartedState();
    /* Waiting for supplicant to stop and monitor to exit */
    AutoPtr<State> mSupplicantStoppingState; //= new SupplicantStoppingState();
    /* Driver start issued, waiting for completed event */
    AutoPtr<State> mDriverStartingState; //= new DriverStartingState();
    /* Driver started */
    AutoPtr<State> mDriverStartedState; //= new DriverStartedState();
    /* Wait until p2p is disabled
     * This is a special state which is entered right after we exit out of DriverStartedState
     * before transitioning to another state.
     */
    AutoPtr<State> mWaitForP2pDisableState; //= new WaitForP2pDisableState();
    /* Driver stopping */
    AutoPtr<State> mDriverStoppingState; //= new DriverStoppingState();
    /* Driver stopped */
    AutoPtr<State> mDriverStoppedState; //= new DriverStoppedState();
    /* Scan for networks, no connection will be established */
    AutoPtr<State> mScanModeState; //= new ScanModeState();
    /* Connecting to an access point */
    AutoPtr<State> mConnectModeState; //= new ConnectModeState();
    /* Connected at 802.11 (L2) level */
    AutoPtr<State> mL2ConnectedState; //= new L2ConnectedState();
    /* fetching IP after connection to access point (assoc+auth complete) */
    AutoPtr<State> mObtainingIpState; //= new ObtainingIpState();
    /* Waiting for link quality verification to be complete */
    AutoPtr<State> mVerifyingLinkState; //= new VerifyingLinkState();
    /* Connected with IP addr */
    AutoPtr<State> mConnectedState; //= new ConnectedState();
    /* Roaming */
    AutoPtr<State> mRoamingState; //= new RoamingState();
    /* disconnect issued, waiting for network disconnect confirmation */
    AutoPtr<State> mDisconnectingState; //= new DisconnectingState();
    /* Network is not connected, supplicant assoc+auth is not complete */
    AutoPtr<State> mDisconnectedState; //= new DisconnectedState();
    /* Waiting for WPS to be completed*/
    AutoPtr<State> mWpsRunningState; //= new WpsRunningState();

    /* Soft ap is starting up */
    AutoPtr<State> mSoftApStartingState; //= new SoftApStartingState();
    /* Soft ap is running */
    AutoPtr<State> mSoftApStartedState; //= new SoftApStartedState();
    /* Soft ap is running and we are waiting for tether notification */
    AutoPtr<State> mTetheringState; //= new TetheringState();
    /* Soft ap is running and we are tethered through connectivity service */
    AutoPtr<State> mTetheredState; //= new TetheredState();
    /* Waiting for untether confirmation before stopping soft Ap */
    AutoPtr<State> mUntetheringState; //= new UntetheringState();

    /**
     * One of  {@link WifiManager#WIFI_STATE_DISABLED},
     *         {@link WifiManager#WIFI_STATE_DISABLING},
     *         {@link WifiManager#WIFI_STATE_ENABLED},
     *         {@link WifiManager#WIFI_STATE_ENABLING},
     *         {@link WifiManager#WIFI_STATE_UNKNOWN}
     *
     */
    AutoPtr<IAtomicInteger32> mWifiState; //= new AtomicInteger(WIFI_STATE_DISABLED);

    /**
     * One of  {@link WifiManager#WIFI_AP_STATE_DISABLED},
     *         {@link WifiManager#WIFI_AP_STATE_DISABLING},
     *         {@link WifiManager#WIFI_AP_STATE_ENABLED},
     *         {@link WifiManager#WIFI_AP_STATE_ENABLING},
     *         {@link WifiManager#WIFI_AP_STATE_FAILED}
     *
     */
    AutoPtr<IAtomicInteger32> mWifiApState; //= new AtomicInteger(WIFI_AP_STATE_DISABLED);

    static const Int32 SCAN_REQUEST; //= 0;
    static const String ACTION_START_SCAN; //= "com.android.server.WifiManager.action.START_SCAN";

    static const String DELAYED_STOP_COUNTER; //= "DelayedStopCounter";
    static const Int32 DRIVER_STOP_REQUEST; //= 0;
    static const String ACTION_DELAYED_DRIVER_STOP; //= "com.android.server.WifiManager.action.DELAYED_DRIVER_STOP";

    static const String ACTION_REFRESH_BATCHED_SCAN; //= "com.android.server.WifiManager.action.REFRESH_BATCHED_SCAN";
    /**
     * Keep track of whether WIFI is running.
     */
    Boolean mIsRunning; //= FALSE;

    /**
     * Keep track of whether we last told the battery stats we had started.
     */
    Boolean mReportedRunning; //= FALSE;

    /**
     * Most recently set source of starting WIFI.
     */
    AutoPtr<IWorkSource> mRunningWifiUids; //= new WorkSource();

    /**
     * The last reported UIDs that were responsible for starting WIFI.
     */
    AutoPtr<IWorkSource> mLastRunningWifiUids; //= new WorkSource();

    AutoPtr<IIBatteryStats> mBatteryStats;

    AutoPtr<IBatchedScanSettings> mBatchedScanSettings;

    /**
     * Track the worksource/cost of the current settings and track what's been noted
     * to the battery stats, so we can mark the end of the previous when changing.
     */
    AutoPtr<IWorkSource> mBatchedScanWorkSource;
    Int32 mBatchedScanCsph; //= 0;
    AutoPtr<IWorkSource> mNotedBatchedScanWorkSource;
    Int32 mNotedBatchedScanCsph; //= 0;

    String mTcpBufferSizes;

    // Used for debug and stats gathering
    static Int32 sScanAlarmIntentCount; //= 0;
    Boolean isPropFeatureEnabled; //= FALSE;

    static Int32 DEFAULT_SCORE; //= INetworkAgent::WIFI_BASE_SCORE;

    Int32 mVerboseLoggingLevel; //= 0;

    Int32 mAggressiveHandover; //= 0;

    Boolean mAlarmEnabled; //= FALSE;
    /* This is set from the overlay config file or from a secure setting.
     * A value of 0 disables scanning in the framework.
     */
    Int64 mFrameworkScanIntervalMs; //= 10000;

    Int64 mCurrentScanAlarmMs; //= 10000;

    static const Boolean DEBUG_PARSE; //= FALSE;

    Int64 mDisconnectedTimeStamp; //= 0;

    // Keeping track of scan requests
    Int64 lastStartScanTimeStamp; //= 0;
    Int64 lastScanDuration; //= 0;
    // Last connect attempt is used to prevent scan requests:
    //  - for a period of 10 seconds after attempting to connect
    Int64 lastConnectAttempt; //= 0;
    String lastScanFreqs;

    // For debugging, keep track of last message status handling
    // TODO, find an equivalent mechanism as part of parent class
    static Int32 MESSAGE_HANDLING_STATUS_PROCESSED; //= 2;
    static Int32 MESSAGE_HANDLING_STATUS_OK; //= 1;
    static Int32 MESSAGE_HANDLING_STATUS_UNKNOWN; //= 0;
    static Int32 MESSAGE_HANDLING_STATUS_REFUSED; //= -1;
    static Int32 MESSAGE_HANDLING_STATUS_FAIL; //= -2;
    static Int32 MESSAGE_HANDLING_STATUS_BUFFERED; //= -3;
    static Int32 MESSAGE_HANDLING_STATUS_DEFERRED; //= -4;
    static Int32 MESSAGE_HANDLING_STATUS_DISCARD; //= -5;
    static Int32 MESSAGE_HANDLING_STATUS_LOOPED; //= -6;
    static Int32 MESSAGE_HANDLING_STATUS_HANDLING_ERROR; //= -7;

    Int32 messageHandlingStatus; //= 0;
    static Int64 lastScanDuringP2p; //= 0;

    Int32 mOnTime; //= 0;
    Int32 mTxTime; //= 0;
    Int32 mRxTime; //= 0;
    Int32 mOnTimeStartScan; //= 0;
    Int32 mTxTimeStartScan; //= 0;
    Int32 mRxTimeStartScan; //= 0;
    Int32 mOnTimeScan; //= 0;
    Int32 mTxTimeScan; //= 0;
    Int32 mRxTimeScan; //= 0;
    Int32 mOnTimeThisScan; //= 0;
    Int32 mTxTimeThisScan; //= 0;
    Int32 mRxTimeThisScan; //= 0;

    Int32 mOnTimeScreenStateChange; //= 0;
    Int32 mOnTimeAtLastReport; //= 0;
    Int64 lastOntimeReportTimeStamp; //= 0;
    Int64 lastScreenStateChangeTimeStamp; //= 0;
    Int32 mOnTimeLastReport; //= 0;
    Int32 mTxTimeLastReport; //= 0;
    Int32 mRxTimeLastReport; //= 0;

    Int64 lastLinkLayerStatsUpdate; //= 0;

    static const String ID_STR; //= "id=";
    static const String BSSID_STR; //= "bssid=";
    static const String FREQ_STR; //= "freq=";
    static const String LEVEL_STR; //= "level=";
    static const String TSF_STR; //= "tsf=";
    static const String FLAGS_STR; //= "flags=";
    static const String SSID_STR; //= "ssid=";
    static const String DELIMITER_STR; //= "====";
    static const String END_STR; //= "####";
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_SERVER_WIFI_WIFISTATEMACHINE_H__
