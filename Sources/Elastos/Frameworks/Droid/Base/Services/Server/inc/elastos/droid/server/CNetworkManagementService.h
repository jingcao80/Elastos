
#ifndef __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSERVICE_H__
#define __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSERVICE_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "_Elastos_Droid_Server_CNetworkManagementService.h"
#include "elastos/droid/server/NativeDaemonConnector.h"
#include "_Elastos.Droid.Server.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/net/NetworkUtils.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Runnable.h>
#include "elastos/droid/telephony/PhoneStateListener.h"
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Utility.h>

using Elastos::Core::CInteger16;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Net::INetworkStatsFactory;
using Elastos::Droid::Net::CInterfaceConfiguration;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::CNetworkStatsEntry;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::IUidRange;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::INetworkActivityListener;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::INativeDaemonConnectorCallbacks;
using Elastos::Droid::Server::IWatchdogMonitor;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IPrintWriter;
using Elastos::Net::CNetworkInterfaceHelper;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInterfaceAddress;
using Elastos::Net::INetworkInterface;
using Elastos::Net::INetworkInterfaceHelper;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CNetworkManagementService)
    , public Object
    , public IBinder
    , public IINetworkManagementService
    , public IWatchdogMonitor
{
public:
    class NetdResponseCode
    {
    public:
        /* Keep in sync with system/netd/server/ResponseCode.h */
        static const Int32 InterfaceListResult       = 110;
        static const Int32 TetherInterfaceListResult = 111;
        static const Int32 TetherDnsFwdTgtListResult = 112;
        static const Int32 TtyListResult             = 113;
        static const Int32 TetheringStatsListResult  = 114;

        static const Int32 TetherStatusResult        = 210;
        static const Int32 IpFwdStatusResult         = 211;
        static const Int32 InterfaceGetCfgResult     = 213;
        static const Int32 SoftapStatusResult        = 214;
        static const Int32 InterfaceRxCounterResult  = 216;
        static const Int32 InterfaceTxCounterResult  = 217;
        static const Int32 QuotaCounterResult        = 220;
        static const Int32 TetheringStatsResult      = 221;
        static const Int32 DnsProxyQueryResult       = 222;
        static const Int32 ClatdStatusResult         = 223;
        static const Int32 V6RtrAdvResult            = 227;

        static const Int32 InterfaceChange           = 600;
        static const Int32 BandwidthControl          = 601;
        static const Int32 InterfaceClassActivity    = 613;
        static const Int32 InterfaceAddressChange    = 614;
        static const Int32 InterfaceDnsServerInfo    = 615;
        static const Int32 RouteChange               = 616;
        static const Int32 InterfaceMessage          = 617;
    };

private:
    /** Set of interfaces with active idle timers. */
    class IdleTimerParams
        : public Object
    {
    public:
        IdleTimerParams(
            /* [in] */ Int32 timeout,
            /* [in] */ Int32 type);

    public:
        Int32 mTimeout;
        Int32 mType;
        Int32 mNetworkCount;
    };

    class NetdCallbackReceiver
        : public Object
        , public INativeDaemonConnectorCallbacks
    {
    private:
        class ReceiverRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            ReceiverRunnable(
                /* [in] */ CNetworkManagementService* owner);

            virtual CARAPI Run();

        private:
            CNetworkManagementService* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        NetdCallbackReceiver(
            /* [in] */ CNetworkManagementService* owner);

        CARAPI OnDaemonConnected();

        // @Override
        CARAPI OnCheckHoldWakeLock(
            /* [in] */ Int32 code,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnEvent(
            /* [in] */ Int32 code,
            /* [in] */ const String& raw,
            /* [in] */ ArrayOf<String>* cooked,
            /* [out] */ Boolean* result);

    private:
        CNetworkManagementService* mHost;
    };

    class InnerSub_PhoneStateListener
        : public PhoneStateListener
    {
    public:
        InnerSub_PhoneStateListener(
            /* [in] */ CNetworkManagementService* host);

        // @Override
        ECode OnDataConnectionRealTimeInfoChanged(
            /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo);

    private:
        CNetworkManagementService* mHost;
    };

    class TimerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        TimerRunnable(
            /* [in] */ CNetworkManagementService* host,
            /* [in] */ Int32 type);

        // @Override
        CARAPI Run();

    private:
        CNetworkManagementService* mHost;
        Int32 mType;
    };

    class RemoveRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        RemoveRunnable(
            /* [in] */ CNetworkManagementService* host,
            /* [in] */ IdleTimerParams* params);

        // @Override
        CARAPI Run();

    private:
        CNetworkManagementService* mHost;
        IdleTimerParams* mParams;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNetworkManagementService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& socket);

    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ const String& socket,
        /* [out] */ IINetworkManagementService** result);

    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [out] */ IINetworkManagementService** result);

    CARAPI_(void) SystemReady();

    CARAPI RegisterObserver(
        /* [in] */ IINetworkManagementEventObserver* observer);

    CARAPI UnregisterObserver(
        /* [in] */ IINetworkManagementEventObserver* observer);

    CARAPI ListInterfaces(
        /* [out] */ ArrayOf<String>** interfaces);

    CARAPI GetInterfaceConfig(
        /* [in] */ const String& iface,
        /* [out] */ IInterfaceConfiguration** config);

    CARAPI SetInterfaceConfig(
        /* [in] */ const String& iface,
        /* [in] */ IInterfaceConfiguration* cfg);

    CARAPI SetInterfaceDown(
        /* [in] */ const String& iface);

    CARAPI SetInterfaceUp(
        /* [in] */ const String& iface);

    CARAPI SetInterfaceIpv6PrivacyExtensions(
        /* [in] */ const String& iface,
        /* [in] */ Boolean enable);

    CARAPI ClearInterfaceAddresses(
        /* [in] */ const String& iface);

    CARAPI EnableIpv6(
        /* [in] */ const String& iface);

    CARAPI DisableIpv6(
        /* [in] */ const String& iface);

    CARAPI AddRoute(
        /* [in] */ Int32 netId,
        /* [in] */ IRouteInfo* route);

    CARAPI RemoveRoute(
        /* [in] */ Int32 netId,
        /* [in] */ IRouteInfo* route);

    CARAPI GetRoutes(
        /* [in] */ const String& interfaceName,
        /* [out, callee] */ ArrayOf<IRouteInfo*>** result);

    // @Override
    CARAPI SetMtu(
        /* [in] */ const String& iface,
        /* [in] */ Int32 mtu);

    CARAPI Shutdown();

    CARAPI GetIpForwardingEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetIpForwardingEnabled(
        /* [in] */ Boolean enabled);

    CARAPI StartTethering(
        /* [in] */ ArrayOf<String>* dhcpRange);

    CARAPI StopTethering();

    CARAPI IsTetheringStarted(
        /* [out] */ Boolean* result);

    CARAPI TetherInterface(
        /* [in] */ const String& iface);

    CARAPI UntetherInterface(
        /* [in] */ const String& iface);

    CARAPI ListTetheredInterfaces(
        /* [out] */ ArrayOf<String>** result);

    CARAPI SetDnsForwarders(
        /* [in] */ INetwork* network,
        /* [in] */ ArrayOf<String>* dns);

    CARAPI GetDnsForwarders(
        /* [out] */ ArrayOf<String>** result);

    CARAPI EnableNat(
        /* [in] */ const String& internalInterface,
        /* [in] */ const String& externalInterface);

    CARAPI DisableNat(
        /* [in] */ const String& internalInterface,
        /* [in] */ const String& externalInterface);

    /**
     * Add an upstream IPv6 interface
     */
    CARAPI AddUpstreamV6Interface(
        /* [in] */ const String& iface);

    /**
     * Remove an upstream IPv6 interface
     */
    CARAPI RemoveUpstreamV6Interface(
        /* [in] */ const String& iface);

    CARAPI ListTtys(
        /* [out] */ ArrayOf<String>** result);

    CARAPI AttachPppd(
        /* [in] */ const String& tty,
        /* [in] */ const String& localAddr,
        /* [in] */ const String& remoteAddr,
        /* [in] */ const String& dns1Addr,
        /* [in] */ const String& dns2Addr);

    CARAPI DetachPppd(
        /* [in] */ const String& tty);

    CARAPI StartAccessPoint(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ const String& wlanIface);

    CARAPI WifiFirmwareReload(
        /* [in] */ const String& wlanIface,
        /* [in] */ const String& mode);

    CARAPI StopAccessPoint(
        /* [in] */ const String& wlanIface);

    CARAPI SetAccessPoint(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ const String& wlanIface);

    CARAPI AddIdleTimer(
        /* [in] */ const String& iface,
        /* [in] */ Int32 timeout,
        /* [in] */ Int32 type);

    CARAPI RemoveIdleTimer(
        /* [in] */ const String& iface);

    CARAPI GetNetworkStatsSummaryDev(
        /* [out] */ INetworkStats** stats);

    CARAPI GetNetworkStatsSummaryXt(
        /* [out] */ INetworkStats** stats);

    CARAPI GetNetworkStatsDetail(
        /* [out] */ INetworkStats** stats);

    CARAPI SetInterfaceQuota(
        /* [in] */ const String& iface,
        /* [in] */ Int64 quotaBytes);

    CARAPI RemoveInterfaceQuota(
        /* [in] */ const String& iface);

    CARAPI SetInterfaceAlert(
        /* [in] */ const String& iface,
        /* [in] */ Int64 alertBytes);

    CARAPI RemoveInterfaceAlert(
        /* [in] */ const String& iface);

    CARAPI SetGlobalAlert(
        /* [in] */ Int64 alertBytes);

    CARAPI SetUidNetworkRules(
        /* [in] */ Int32 uid,
        /* [in] */ Boolean rejectOnQuotaInterfaces);

    CARAPI IsBandwidthControlEnabled(
        /* [out] */ Boolean* result);

    CARAPI GetNetworkStatsUidDetail(
        /* [in] */ Int32 uid,
        /* [out] */ INetworkStats** stats);

    CARAPI GetNetworkStatsTethering(
        /* [out] */ INetworkStats** stats);

    CARAPI SetDnsServersForNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ ArrayOf<String>* servers,
        /* [in] */ const String& iface);

    // @Override
    CARAPI AddVpnUidRanges(
        /* [in] */ Int32 netId,
        /* [in] */ ArrayOf<IUidRange*>* ranges);

    // @Override
    CARAPI RemoveVpnUidRanges(
        /* [in] */ Int32 netId,
        /* [in] */ ArrayOf<IUidRange*>* ranges);

    CARAPI FlushNetworkDnsCache(
        /* [in] */ Int32 netId);

    CARAPI SetFirewallEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsFirewallEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetFirewallInterfaceRule(
        /* [in] */ const String& iface,
        /* [in] */ Boolean allow);

    CARAPI SetFirewallEgressSourceRule(
        /* [in] */ const String& addr,
        /* [in] */ Boolean allow);

    CARAPI SetFirewallEgressDestRule(
        /* [in] */ const String& addr,
        /* [in] */ Int32 port,
        /* [in] */ Boolean allow);

    CARAPI SetFirewallUidRule(
        /* [in] */ Int32 uid,
        /* [in] */ Boolean allow);

    static CARAPI EnforceSystemUid();

    // @Override
    CARAPI StartClatd(
        /* [in] */ const String& interfaceName);

    // @Override
    CARAPI StopClatd();

    // @Override
    CARAPI IsClatdStarted(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI RegisterNetworkActivityListener(
        /* [in] */ INetworkActivityListener* listener);

    // @Override
    CARAPI UnregisterNetworkActivityListener(
        /* [in] */ INetworkActivityListener* listener);

    // @Override
    CARAPI IsNetworkActive(
        /* [out] */ Boolean* result);

    /** {@inheritDoc} */
    // @Override
    CARAPI Monitor();

    // @Override
    CARAPI CreatePhysicalNetwork(
        /* [in] */ Int32 netId);

    // @Override
    CARAPI CreateVirtualNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean hasDNS,
        /* [in] */ Boolean secure);

    // @Override
    CARAPI RemoveNetwork(
        /* [in] */ Int32 netId);

    // @Override
    CARAPI AddInterfaceToNetwork(
        /* [in] */ const String& iface,
        /* [in] */ Int32 netId);

    // @Override
    CARAPI RemoveInterfaceFromNetwork(
        /* [in] */ const String& iface,
        /* [in] */ Int32 netId);

    // @Override
    CARAPI AddLegacyRouteForNetId(
        /* [in] */ Int32 netId,
        /* [in] */ IRouteInfo* routeInfo,
        /* [in] */ Int32 uid);

    // @Override
    CARAPI SetDefaultNetId(
        /* [in] */ Int32 netId);

    // @Override
    CARAPI ClearDefaultNetId();

    // @Override
    CARAPI SetPermission(
        /* [in] */ const String& permission,
        /* [in] */ ArrayOf<Int32>* uids);

    // @Override
    CARAPI ClearPermission(
        /* [in] */ ArrayOf<Int32>* uids);

    // @Override
    CARAPI AllowProtect(
        /* [in] */ Int32 uid);

    // @Override
    CARAPI DenyProtect(
        /* [in] */ Int32 uid);

    // @Override
    CARAPI AddInterfaceToLocalNetwork(
        /* [in] */ const String& iface,
        /* [in] */ IList* routes);

    // @Override
    CARAPI RemoveInterfaceFromLocalNetwork(
        /* [in] */ const String& iface);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    void Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI NotifyInterfaceStatusChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI NotifyInterfaceLinkStateChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI NotifyInterfaceAdded(
        /* [in] */ const String& iface);

    CARAPI NotifyInterfaceRemoved(
        /* [in] */ const String& iface);

    CARAPI NotifyLimitReached(
        /* [in] */ const String& limitName,
        /* [in] */ const String& iface);

    /**
     * Notify our observers of a change in the data activity state of the interface
     */
    CARAPI NotifyInterfaceClassActivity(
        /* [in] */ Int32 type,
        /* [in] */ Int32 powerState,
        /* [in] */ Int64 tsNanos,
        /* [in] */ Boolean fromRadio);

    /**
     * Notify our observers of a change in the data activity state of the interface
     */
    CARAPI NotifyInterfaceMessage(
        /* [in] */ const String& message);

    CARAPI PrepareNativeDaemon();

    CARAPI ModifyRoute(
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& action,
        /* [in] */ IRouteInfo* route);

    CARAPI ReadRouteList(
        /* [in] */ const String& filename,
        /* [out, callee] */ List<String>** result);

    CARAPI ModifyNat(
        /* [in] */ const String& action,
        /* [in] */ const String& internalInterface,
        /* [in] */ const String& externalInterface);

    static CARAPI_(String) GetSecurityType(
        /* [in] */ IWifiConfiguration* wifiConfig);

    CARAPI GetBatteryStats(
        /* [out] */ IIBatteryStats** result);

    /**
     * Notify our observers of a new or updated interface address.
     */
    CARAPI NotifyAddressUpdated(
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    /**
     * Notify our observers of a deleted interface address.
     */
    CARAPI NotifyAddressRemoved(
        /* [in] */ const String& iface,
        /* [in] */ ILinkAddress* address);

    /**
     * Notify our observers of DNS server information received.
     */
    CARAPI NotifyInterfaceDnsServerInfo(
        /* [in] */ const String& iface,
        /* [in] */ Int64 lifetime,
        /* [in] */ ArrayOf<String>* addresses);

    /**
     * Notify our observers of a route change.
     */
    CARAPI NotifyRouteChange(
        /* [in] */ const String& action,
        /* [in] */ IRouteInfo* route);

    CARAPI ExcludeLinkLocal(
        /* [in] */ IList* addresses,
        /* [out] */ IList** result);

    CARAPI ReportNetworkActive();

    CARAPI ModifyInterfaceInNetwork(
        /* [in] */ const String& action,
        /* [in] */ const String& netId,
        /* [in] */ const String& iface);

public:
    /**
     * Name representing {@link #setGlobalAlert(long)} limit when delivered to
     * {@link INetworkManagementEventObserver#limitReached(String, String)}.
     */
    static const String LIMIT_GLOBAL_ALERT;

private:
    static const String TAG;
    static const Boolean DBG;
    static const String NETD_TAG;
    static const String NETD_SOCKET_NAME;
    static const Int32 MAX_UID_RANGES_PER_COMMAND;

    static const Int32 DAEMON_MSG_MOBILE_CONN_REAL_TIME_INFO;

    /**
     * Binder context for this service
     */
    AutoPtr<IContext> mContext;

    /**
     * connector object for communicating with netd
     */
    AutoPtr<NativeDaemonConnector> mConnector;

    AutoPtr<IHandler> mFgHandler;
    AutoPtr<IHandler> mDaemonHandler;
    AutoPtr<IPhoneStateListener> mPhoneStateListener;

    AutoPtr<IIBatteryStats> mBatteryStats;

    AutoPtr<IThread> mThread;
    AutoPtr<ICountDownLatch> mConnectedSignal;

    AutoPtr<IRemoteCallbackList> mObservers;

    AutoPtr<INetworkStatsFactory> mStatsFactory;

    Object mQuotaLock;
    /** Set of interfaces with active quotas. */
    AutoPtr<IHashMap> mActiveQuotas;
    /** Set of interfaces with active alerts. */
    AutoPtr<IHashMap> mActiveAlerts;
    /** Set of UIDs with active reject rules. */
    AutoPtr<ISparseBooleanArray> mUidRejectOnQuota;

    Object mIdleTimerLock;

    AutoPtr<IHashMap> mActiveIdleTimers;

    volatile Boolean mBandwidthControlEnabled;
    volatile Boolean mFirewallEnabled;

    Boolean mMobileActivityFromRadio;
    Int32 mLastPowerStateFromRadio;

    AutoPtr<IRemoteCallbackList> mNetworkActivityListeners;
    Boolean mNetworkActive;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSERVICE_H__
