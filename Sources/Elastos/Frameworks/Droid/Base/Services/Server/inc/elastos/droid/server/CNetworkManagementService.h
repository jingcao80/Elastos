
#ifndef __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSERVICE_H__
#define __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_CNetworkManagementService.h"
#include "NativeDaemonConnector.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInterfaceAddress;
using Elastos::Net::INetworkInterface;
using Elastos::Net::INetworkInterfaceHelper;
using Elastos::Net::CNetworkInterfaceHelper;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Concurrent::CCountDownLatch;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::CInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::INetworkManagementEventObserver;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Net::CNetworkStatsEntry;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Internal::Net::INetworkStatsFactory;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CNetworkManagementService)
{
public:
    class NetdResponseCode
    {
    public:
        /* Keep in sync with system/netd/ResponseCode.h */
        static const Int32 InterfaceListResult       = 110;
        static const Int32 TetherInterfaceListResult = 111;
        static const Int32 TetherDnsFwdTgtListResult = 112;
        static const Int32 TtyListResult             = 113;

        static const Int32 TetherStatusResult        = 210;
        static const Int32 IpFwdStatusResult         = 211;
        static const Int32 InterfaceGetCfgResult     = 213;
        static const Int32 SoftapStatusResult        = 214;
        static const Int32 InterfaceRxCounterResult  = 216;
        static const Int32 InterfaceTxCounterResult  = 217;
        static const Int32 InterfaceRxThrottleResult = 218;
        static const Int32 InterfaceTxThrottleResult = 219;
        static const Int32 QuotaCounterResult        = 220;
        static const Int32 TetheringStatsResult      = 221;
        static const Int32 DnsProxyQueryResult       = 222;

        static const Int32 InterfaceChange           = 600;
        static const Int32 BandwidthControl          = 601;
        static const Int32 InterfaceClassActivity    = 613;
    };

private:
    /** Set of interfaces with active idle timers. */
    class IdleTimerParams : public ElRefBase
    {
    public:
        IdleTimerParams(
            /* [in] */ Int32 timeout,
            /* [in] */ const String& label)
            : mTimeout(timeout)
            , mLabel(label)
            , mNetworkCount(1)
        {}

    public:
        Int32 mTimeout;
        String mLabel;
        Int32 mNetworkCount;
    };

    class NetdCallbackReceiver
        : public ElRefBase
        , public INativeDaemonConnectorCallbacks
    {
    private:
        class ReceiverRunnable
            : public Runnable
        {
        public:
            ReceiverRunnable(
                /* [in] */ CNetworkManagementService* owner)
                : mOwner(owner)
            {}

            virtual CARAPI Run()
            {
                mOwner->PrepareNativeDaemon();
                return NOERROR;
            }

        private:
            CNetworkManagementService* mOwner;
        };

    public:
        NetdCallbackReceiver(
            /* [in] */ CNetworkManagementService* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnDaemonConnected();

        CARAPI OnEvent(
            /* [in] */ Int32 code,
            /* [in] */ const String& raw,
            /* [in] */ const ArrayOf<String>& cooked,
            /* [out] */ Boolean* result);

    private:
        CNetworkManagementService* mOwner;
    };

public:
    CNetworkManagementService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<CNetworkManagementService>) Create(
        /* [in] */ IContext* context);

    CARAPI_(void) SystemReady();

    CARAPI RegisterObserver(
        /* [in] */ INetworkManagementEventObserver* observer);

    CARAPI UnregisterObserver(
        /* [in] */ INetworkManagementEventObserver* observer);

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
        /* [in] */ const String& interfaceName,
        /* [in] */ IRouteInfo* route);

    CARAPI RemoveRoute(
        /* [in] */ const String& interfaceName,
        /* [in] */ IRouteInfo* route);

    CARAPI AddSecondaryRoute(
        /* [in] */ const String& interfaceName,
        /* [in] */ IRouteInfo* route);

    CARAPI RemoveSecondaryRoute(
        /* [in] */ const String& interfaceName,
        /* [in] */ IRouteInfo* route);

    CARAPI GetRoutes(
        /* [in] */ const String& interfaceName,
        /* [out, callee] */ ArrayOf<IRouteInfo*>** result);

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

    CARAPI StartReverseTethering(
        /* [in] */ const String& iface);

    CARAPI StopReverseTethering();

    CARAPI TetherInterface(
        /* [in] */ const String& iface);

    CARAPI UntetherInterface(
        /* [in] */ const String& iface);

    CARAPI ListTetheredInterfaces(
        /* [out] */ ArrayOf<String>** result);

    CARAPI SetDnsForwarders(
        /* [in] */ ArrayOf<String>* dns);

    CARAPI GetDnsForwarders(
        /* [out] */ ArrayOf<String>** result);

    CARAPI EnableNat(
        /* [in] */ const String& internalInterface,
        /* [in] */ const String& externalInterface);

    CARAPI DisableNat(
        /* [in] */ const String& internalInterface,
        /* [in] */ const String& externalInterface);

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
        /* [in] */ const String& label);

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
        /* [in] */ ArrayOf<String>* ifacePairs,
        /* [out] */ INetworkStats** stats);

    CARAPI SetInterfaceThrottle(
        /* [in] */ const String& iface,
        /* [in] */ Int32 rxKbps,
        /* [in] */ Int32 txKbps);

    CARAPI GetInterfaceRxThrottle(
        /* [in] */ const String& iface,
        /* [out] */ Int32* rxThrottle);

    CARAPI GetInterfaceTxThrottle(
        /* [in] */ const String& iface,
        /* [out] */ Int32* txThrottle);

    CARAPI SetDefaultInterfaceForDns(
        /* [in] */ const String& iface);

    CARAPI SetDnsServersForInterface(
        /* [in] */ const String& iface,
        /* [in] */ ArrayOf<String>* servers);

    CARAPI FlushDefaultDnsCache();

    CARAPI FlushInterfaceDnsCache(
        /* [in] */ const String& iface);

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

    CARAPI Monitor();

protected:
    void Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) NotifyInterfaceStatusChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI_(void) NotifyInterfaceLinkStateChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI_(void) NotifyInterfaceAdded(
        /* [in] */ const String& iface);

    CARAPI_(void) NotifyInterfaceRemoved(
        /* [in] */ const String& iface);

    CARAPI_(void) NotifyLimitReached(
        /* [in] */ const String& limitName,
        /* [in] */ const String& iface);

    CARAPI_(void) NotifyInterfaceClassActivity(
        /* [in] */ const String& label,
        /* [in] */ Boolean active);

    CARAPI_(void) PrepareNativeDaemon();

    CARAPI ModifyRoute(
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& action,
        /* [in] */ IRouteInfo* route,
        /* [in] */ const String& type);

    CARAPI ReadRouteList(
        /* [in] */ const String& filename,
        /* [out, callee] */ List<String>** result);

    CARAPI ModifyNat(
        /* [in] */ const String& action,
        /* [in] */ const String& internalInterface,
        /* [in] */ const String& externalInterface);

    static CARAPI_(String) GetSecurityType(
        /* [in] */ IWifiConfiguration* wifiConfig);

    CARAPI GetNetworkStatsTethering(
        /* [in] */ const String& ifaceIn,
        /* [in] */ const String& ifaceOut,
        /* [out] */ INetworkStatsEntry** result);

    CARAPI GetInterfaceThrottle(
        /* [in] */ const String& iface,
        /* [in] */ Boolean rx,
        /* [out] */ Int32* throttle);

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

    static const String ADD;
    static const String REMOVE;

    static const String ALLOW;
    static const String DENY;

    static const String DEFAULT;
    static const String SECONDARY;

    /**
     * Binder context for this service
     */
    AutoPtr<IContext> mContext;

    /**
     * connector object for communicating with netd
     */
    AutoPtr<NativeDaemonConnector> mConnector;

    AutoPtr<IHandler> mMainHandler;

    AutoPtr<IThread> mThread;
    AutoPtr<ICountDownLatch> mConnectedSignal;

    AutoPtr<IRemoteCallbackList> mObservers;

    AutoPtr<INetworkStatsFactory> mStatsFactory;

    Object mQuotaLock;
    /** Set of interfaces with active quotas. */
    HashMap<String, Int64> mActiveQuotas;
    /** Set of interfaces with active alerts. */
    HashMap<String, Int64> mActiveAlerts;
    /** Set of UIDs with active reject rules. */
    HashMap<Int32, Boolean> mUidRejectOnQuota;

    Object mIdleTimerLock;

    HashMap<String, AutoPtr<IdleTimerParams> > mActiveIdleTimers;

    volatile Boolean mBandwidthControlEnabled;
    volatile Boolean mFirewallEnabled;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CNETWORKMANAGEMENTSERVICE_H__
