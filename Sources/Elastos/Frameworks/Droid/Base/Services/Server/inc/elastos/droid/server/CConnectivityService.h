
#ifndef __ELASTOS_DROID_SERVER_CCONNECTIVITYSERVICE_H__
#define __ELASTOS_DROID_SERVER_CCONNECTIVITYSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CConnectivityService.h"
#include "elastos/droid/server/connectivity/PermissionMonitor.h"
#include "elastos/droid/server/connectivity/Nat464Xlat.h"
#include "elastos/droid/server/connectivity/CTethering.h"
#include "elastos/droid/server/connectivity/Vpn.h"
#include "elastos/droid/server/connectivity/DataConnectionStats.h"
#include "elastos/droid/server/connectivity/PacManager.h"
#include "elastos/droid/server/net/LockdownVpnTracker.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Net::ILinkQualityInfo;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::IINetworkPolicyManager;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::INetworkConfig;
using Elastos::Droid::Net::INetworkMisc;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::IINetworkPolicyListener;
using Elastos::Droid::Net::INetworkQuotaInfo;
using Elastos::Droid::Net::INetworkState;
using Elastos::Droid::Net::INetworkStateTracker;
using Elastos::Droid::Net::IINetworkStatsService;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IProxyProperties;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Internal::Net::IVpnProfile;
using Elastos::Droid::Internal::Net::ILegacyVpnInfo;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Server::Connectivity::PermissionMonitor;
using Elastos::Droid::Server::Connectivity::Nat464Xlat;
using Elastos::Droid::Server::Connectivity::CTethering;
using Elastos::Droid::Server::Connectivity::Vpn;
using Elastos::Droid::Server::Connectivity::INetworkAgentInfo;
using Elastos::Droid::Server::Connectivity::DataConnectionStats;
using Elastos::Droid::Server::Connectivity::PacManager;
using Elastos::Droid::Server::Net::LockdownVpnTracker;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Server {

class CDataActivityObserver;

CarClass(CConnectivityService)
    , public Object
    , public IIConnectivityManager
    , public IBinder
{
public:

    friend class InternalBroadcastReceiver;
    friend class NetworkStateTrackerHandler;
    friend class SettingsObserver;
    friend class CNetworkPolicyListener;
    friend class CDataActivityObserver;

    /**
     * Implements support for the legacy "one network per network type" model.
     *
     * We used to have a static array of NetworkStateTrackers, one for each
     * network type, but that doesn't work any more now that we can have,
     * for example, more that one wifi network. This class stores all the
     * NetworkAgentInfo objects that support a given type, but the legacy
     * API will only see the first one.
     *
     * It serves two main purposes:
     *
     * 1. Provide information about "the network for a given type" (since this
     *    API only supports one).
     * 2. Send legacy connectivity change broadcasts. Broadcasts are sent if
     *    the first network for a given type changes, or if the default network
     *    changes.
     */
    class LegacyTypeTracker
        : public Object
    {
    public:
        LegacyTypeTracker(
            /* [in] */ CConnectivityService* host);

        CARAPI AddSupportedType(
            /* [in] */ Int32 type);

        Boolean IsTypeSupported(
            /* [in] */ Int32 type);

        AutoPtr<INetworkAgentInfo> GetNetworkForType(
            /* [in] */ Int32 type);

        /** Adds the given network to the specified legacy type list. */
        void Add(
            /* [in] */ Int32 type,
            /* [in] */ INetworkAgentInfo* nai);

        /** Removes the given network from the specified legacy type list. */
        void Remove(
            /* [in] */ Int32 type,
            /* [in] */ INetworkAgentInfo* nai);

        /** Removes the given network from all legacy type lists. */
        void Remove(
            /* [in] */ INetworkAgentInfo* nai);

        CARAPI Dump(
            /* [in] */ IIndentingPrintWriter* pw);

    private:
        // This class needs its own log method because it has a different TAG.
        void Log(
            /* [in] */ const String& s);

        String NaiToString(
            /* [in] */ INetworkAgentInfo* nai);

        void MaybeLogBroadcast(
            /* [in] */ INetworkAgentInfo* nai,
            /* [in] */ Boolean connected,
            /* [in] */ Int32 type);

    public:
        static const Boolean DBG;
        static const Boolean VDBG;
        static const String TAG;

    private:
        /**
         * Array of lists, one per legacy network type (e.g., TYPE_MOBILE_MMS).
         * Each list holds references to all NetworkAgentInfos that are used to
         * satisfy requests for that network type.
         *
         * This array is built out at startup such that an unsupported network
         * doesn't get an ArrayList instance, making this a tristate:
         * unsupported, supported but not active and active.
         *
         * The actual lists are populated when we scan the network types that
         * are supported on this device.
         */
         AutoPtr<ArrayOf<IArrayList*> > mTypeLists;// ArrayList<NetworkAgentInfo> []
          CConnectivityService* mHost;
    };

private:

    /**
     * Tracks info about the requester.
     * Also used to notice when the calling process dies so we can self-expire
     */
    class NetworkRequestInfo
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        NetworkRequestInfo(
            /* [in] */ IMessenger* m,
            /* [in] */ INetworkRequest* r,
            /* [in] */ IBinder* binder,
            /* [in] */ Boolean isRequest,
            /* [in] */ CConnectivityService* host);

        void UnlinkDeathRecipient();

        CARAPI ProxyDied();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        friend class CConnectivityService;

        static const Boolean REQUEST;
        static const Boolean LISTEN;

        AutoPtr<INetworkRequest> mRequest;
        AutoPtr<IBinder> mBinder;
        Int32 mPid;
        Int32 mUid;
        AutoPtr<IMessenger> mMessenger;
        Boolean mIsRequest;
         CConnectivityService* mHost;
    };

    class NetworkFactoryInfo
        : public Object
    {
    public:
        NetworkFactoryInfo(
            /* [in] */ const String& name,
            /* [in] */ IMessenger* messenger,
            /* [in] */ IAsyncChannel* asyncChannel)
            : mName(name)
            , mMessenger(messenger)
            , mAsyncChannel(asyncChannel)
        {
        }

    public:
        String mName;
        AutoPtr<IMessenger> mMessenger;
        AutoPtr<IAsyncChannel> mAsyncChannel;
    };

    class UserIntentReceiver
        : public BroadcastReceiver
    {
    public:
        UserIntentReceiver(
            /* [in] */ CConnectivityService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("::UserIntentReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
         CConnectivityService* mHost;
    };

    class InternalBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        InternalBroadcastReceiver(
            /* [in] */ CConnectivityService* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("::InternalBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
         CConnectivityService* mHost;
    };

    class PktCntSampleIntervalElapsedeceiver
        : public BroadcastReceiver
    {
    public:
        PktCntSampleIntervalElapsedeceiver(
            /* [in] */ CConnectivityService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("::PktCntSampleIntervalElapsedeceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
         CConnectivityService* mHost;
    };

    class InternalHandler
        : public Handler
    {
    public:
        InternalHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CConnectivityService* host)
            : Handler(looper)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
         CConnectivityService* mHost;
    };

    // must be stateless - things change under us.
    class NetworkStateTrackerHandler
        : public Handler
    {
    public:
        NetworkStateTrackerHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CConnectivityService* host)
            : Handler(looper)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
         CConnectivityService* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* observerHandler,
            /* [in] */ Int32 what,
            /* [in] */ CConnectivityService* host);

        CARAPI Observe(
            /* [in] */ IContext* context);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IHandler> mHandler;
        Int32 mWhat;
        CConnectivityService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CConnectivityService();

    ~CConnectivityService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* netd,
        /* [in] */ IINetworkStatsService* statsService,
        /* [in] */ IINetworkPolicyManager* policyManager);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* netManager,
        /* [in] */ IINetworkStatsService* statsService,
        /* [in] */ IINetworkPolicyManager* policyManager,
        /* [in] */ Handle32 netFac);


    /**
    * Return NetworkInfo for the active (i.e., connected) network interface.
    * It is assumed that at most one network is active at a time. If more
    * than one is active, it is indeterminate which will be returned.
    * @return the info for the active network, or {@code null} if none is
    * active
    */
    CARAPI GetActiveNetworkInfo(
        /* [out] */ INetworkInfo** info);

    /**
     * Find the first Provisioning network or the ActiveDefaultNetwork
     * if there is no Provisioning network
     *
     * @return NetworkInfo or null if none.
     */
    // @Override
    CARAPI GetProvisioningOrActiveNetworkInfo(
        /* [out] */ INetworkInfo** info);

    CARAPI GetActiveNetworkInfoUnfiltered(
        /* [out] */ INetworkInfo** info);

    CARAPI GetActiveNetworkInfoForUid(
        /* [in] */ Int32 uid,
        /* [out] */ INetworkInfo** info);

    CARAPI GetNetworkInfo(
        /* [in] */ Int32 networkType,
        /* [out] */ INetworkInfo** info);

    CARAPI GetNetworkInfoForNetwork(
        /* [in] */ INetwork* network,
        /* [out] */ INetworkInfo** info);

    CARAPI GetAllNetworkInfo(
        /* [out, callee] */ ArrayOf<INetworkInfo*>** allInfo);

    CARAPI GetNetworkForType(
        /* [in] */ Int32 networkType,
        /* [out] */ INetwork** network);

    CARAPI GetAllNetworks(
        /* [out, callee] */ ArrayOf<INetwork*>** networks);

    CARAPI IsNetworkSupported(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result);

    /**
    * Return LinkProperties for the active (i.e., connected) default
    * network interface.  It is assumed that at most one default network
    * is active at a time. If more than one is active, it is indeterminate
    * which will be returned.
    * @return the ip properties for the active network, or {@code null} if
    * none is active
    */
    CARAPI GetActiveLinkProperties(
        /* [out] */ ILinkProperties** properties);

    CARAPI GetLinkPropertiesForType(
        /* [in] */ Int32 networkType,
        /* [out] */ ILinkProperties** properties);

    // TODO - this should be ALL networks
    CARAPI GetLinkProperties(
        /* [in] */ INetwork* network,
        /* [out] */ ILinkProperties** properties);

    CARAPI GetNetworkCapabilities(
        /* [in] */ INetwork* network,
        /* [out] */ INetworkCapabilities** result);

    CARAPI GetAllNetworkState(
        /* [out] */ ArrayOf<INetworkState*>** allStates);

    CARAPI GetActiveNetworkQuotaInfo(
        /* [out] */ INetworkQuotaInfo** info);

    CARAPI IsActiveNetworkMetered(
        /* [out] */ Boolean* result);

    /**
    * Ensure that a network route exists to deliver traffic to the specified
    * host via the specified network interface.
    * @param networkType the type of the network over which traffic to the
    * specified host is to be routed
    * @param hostAddress the IP address of the host to which the route is
    * desired
    * @return {@code true} on success, {@code false} on failure
    */
    CARAPI RequestRouteToHostAddress(
        /* [in] */ Int32 networkType,
        /* [in] */ ArrayOf<Byte>* hostAddress,
        /* [out] */ Boolean* result);

    CARAPI SetDataDependency(
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean met);

    CARAPI SetPolicyDataEnable(
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean enabled);

    CARAPI SendConnectedBroadcast(
        /* [in] */ INetworkInfo* info);

    CARAPI_(void) SystemReady();

    /** @hide */
    CARAPI CaptivePortalCheckCompleted(
        /* [in] */ INetworkInfo* info,
        /* [in] */ Boolean isCaptivePortal);

    CARAPI Tether(
        /* [in] */ const String& iface,
        /* [out] */ Int32* status);

    CARAPI Untether(
        /* [in] */ const String& iface,
        /* [out] */ Int32* status);

    CARAPI GetLastTetherError(
        /* [in] */ const String& iface,
        /* [out] */ Int32* status);

    // TODO - proper iface API for selection by property, inspection, etc
    CARAPI GetTetherableUsbRegexs(
        /* [out, callee] */ ArrayOf<String>** regexs);

    CARAPI GetTetherableWifiRegexs(
        /* [out, callee] */ ArrayOf<String>** regexs);

    CARAPI GetTetherableBluetoothRegexs(
        /* [out, callee] */ ArrayOf<String>** regexs);

    CARAPI SetUsbTethering(
        /* [in] */ Boolean enable,
        /* [out] */ Int32* status);

    // TODO - move iface listing, queries, etc to new module
    // javadoc from interface
    CARAPI GetTetherableIfaces(
        /* [out, callee] */ ArrayOf<String>** ifaces);

    CARAPI GetTetheredIfaces(
        /* [out, callee] */ ArrayOf<String>** ifaces);

    CARAPI GetTetheredDhcpRanges(
        /* [out, callee] */ ArrayOf<String>** ifaces);

    CARAPI GetTetheringErroredIfaces(
        /* [out, callee] */ ArrayOf<String>** ifaces);

    // if ro.tether.denied = true we default to no tethering
    // gservices could set the secure setting to 1 though to enable it on a build where it
    // had previously been turned off.
    CARAPI IsTetheringSupported(
        /* [out] */ Boolean* status);

    // An API NetworkStateTrackers can call when they lose their network.
    // This will automatically be cleared after X seconds or a network becomes CONNECTED,
    // whichever happens first.  The timer is started by the first caller and not
    // restarted by subsequent callers.
    CARAPI RequestNetworkTransitionWakelock(
        /* [in] */ const String& forWhom);

    // 100 percent is full good, 0 is full bad.
    CARAPI ReportInetCondition(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 percentage);

    CARAPI ReportBadNetwork(
        /* [in] */ INetwork* network);

    CARAPI GetProxy(
        /* [out] */ IProxyInfo** proxyInfo);

    CARAPI SetGlobalProxy(
        /* [in] */ IProxyInfo* proxyInfo);

    CARAPI GetGlobalProxy(
        /* [out] */ IProxyInfo** proxyInfo);

    CARAPI_(Int32) ConvertFeatureToNetworkType(
        /* [in] */ Int32 networkType,
        /* [in] */ const String& feature);

    /**
     * Set whether the current VPN package has the ability to launch VPNs without
     * user intervention. This method is used by system UIs and not available
     * in ConnectivityManager. Permissions are checked in Vpn class.
     * @hide
     */
    // @Override
    CARAPI PrepareVpn(
        /* [in] */ const String& oldPackage,
        /* [in] */ const String& newPackage,
        /* [out] */ Boolean* status);

    /**
     * Set whether the current VPN package has the ability to launch VPNs without
     * user intervention. This method is used by system UIs and not available
     * in ConnectivityManager. Permissions are checked in Vpn class.
     * @hide
     */
    //@Override
    CARAPI SetVpnPackageAuthorization(
        /* [in] */ Boolean authorized);

    /**
     * Configure a TUN interface and return its file descriptor. Parameters
     * are encoded and opaque to this class. This method is used by VpnBuilder
     * and not available in ConnectivityManager. Permissions are checked in
     * Vpn class.
     * @hide
     */
    CARAPI EstablishVpn(
        /* [in] */ IVpnConfig* config,
        /* [out] */ IParcelFileDescriptor** fd);

    /**
     * Start legacy VPN, controlling native daemons as needed. Creates a
     * secondary thread to perform connection work, returning quickly.
     */
    CARAPI StartLegacyVpn(
        /* [in] */ IVpnProfile* profile);

    /**
     * Return the information of the ongoing legacy VPN. This method is used
     * by VpnSettings and not available in ConnectivityManager. Permissions
     * are checked in Vpn class.
     * @hide
     */
    CARAPI GetLegacyVpnInfo(
        /* [out] */ ILegacyVpnInfo** info);

    /**
     * Returns the information of the ongoing VPN. This method is used by VpnDialogs and
     * not available in ConnectivityManager.
     * Permissions are checked in Vpn class.
     * @hide
     */
    // @Override
    CARAPI GetVpnConfig(
        /* [out] */ IVpnConfig** config);

    CARAPI UpdateLockdownVpn(
        /* [out] */ Boolean* status);

    CARAPI GetRestoreDefaultNetworkDelay(
        /* [in] */ Int32 networkType,
        /* [out] */ Int32* result);

    CARAPI SupplyMessenger(
        /* [in] */ Int32 networkType,
        /* [in] */ IMessenger* messenger);

    CARAPI FindConnectionTypeForIface(
        /* [in] */ const String& iface,
        /* [out] */ Int32* result);

    CARAPI CheckMobileProvisioning(
        /* [in] */ Int32 suggestedTimeOutMs,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetMobileRedirectedProvisioningUrl(
        /* [out] */ String* result);

    //@Override
    CARAPI GetMobileProvisioningUrl(
        /* [out] */ String* result);

    //@Override
    CARAPI SetProvisioningNotificationVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 networkType,
        /* [in] */ const String& action);

    //@Override
    CARAPI SetAirplaneMode(
        /* [in] */ Boolean enable);

    //@Override
    CARAPI GetLinkQualityInfo(
        /* [in] */ Int32 networkType,
        /* [out] */ ILinkQualityInfo** info);

    //@Override
    CARAPI GetActiveLinkQualityInfo(
        /* [out] */ ILinkQualityInfo** info);

    //@Override
    CARAPI GetAllLinkQualityInfo(
        /* [out, callee] */ ArrayOf<ILinkQualityInfo*>** infos);

    //@Override
    CARAPI RequestNetwork(
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ IMessenger* messenger,
        /* [in] */ Int32 timeoutMs,
        /* [in] */ IBinder* binder,
        /* [in] */ Int32 legacyType,
        /* [out] */ INetworkRequest** request);

    //@Override
    CARAPI PendingRequestForNetwork(
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ IPendingIntent* operation,
        /* [out] */ INetworkRequest** request);

    //@Override
    CARAPI ListenForNetwork(
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ IMessenger* messenger,
        /* [in] */ IBinder* binder,
        /* [out] */ INetworkRequest** request);

    //@Override
    CARAPI PendingListenForNetwork(
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ IPendingIntent* operation);

    //@Override
    CARAPI ReleaseNetworkRequest(
        /* [in] */ INetworkRequest* networkRequest);

    //@Override
    CARAPI RegisterNetworkFactory(
        /* [in] */ IMessenger* messenger,
        /* [in] */ const String& name);

    //@Override
    CARAPI UnregisterNetworkFactory(
        /* [in] */ IMessenger* messenger);

    CARAPI RegisterNetworkAgent(
        /* [in] */ IMessenger* messenger,
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ Int32 currentScore,
        /* [in] */ INetworkMisc* networkMisc);

    //@Override
    CARAPI AddVpnAddress(
        /* [in] */ const String& address,
        /* [in] */ Int32 prefixLength,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RemoveVpnAddress(
        /* [in] */ const String& address,
        /* [in] */ Int32 prefixLength,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);
protected:
    //@Override
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ const ArrayOf<String>& args);

private:
    CARAPI_(Int32) NextNetworkRequestId();

    /**
     * Find the first Provisioning network.
     *
     * @return NetworkInfo or null if none.
     */
    AutoPtr<INetworkInfo> GetProvisioningNetworkInfo();

    CARAPI AssignNextNetId(
        /* [in] */ INetworkAgentInfo* nai);

    CARAPI_(Int32) GetConnectivityChangeDelay();

    CARAPI_(Boolean) Teardown(
        /* [in] */ INetworkStateTracker* netTracker);

    /**
     * Check if UID should be blocked from using the network represented by the given networkType.
     * @deprecated Uses mLegacyTypeTracker; cannot deal with multiple Networks of the same type.
     */
    CARAPI_(Boolean) IsNetworkBlocked(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 uid);

    /**
     * Check if UID should be blocked from using the network represented by the given
     * NetworkAgentInfo.
     */
    Boolean IsNetworkBlocked(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ Int32 uid);

    /**
     * Check if UID should be blocked from using the network with the given LinkProperties.
     */
    Boolean IsNetworkWithLinkPropertiesBlocked(
        /* [in] */ ILinkProperties* lp,
        /* [in] */ Int32 uid);

    /**
     * Return a filtered {@link NetworkInfo}, potentially marked
     * {@link DetailedState#BLOCKED} based on
     * {@link #isNetworkBlocked}.
     * @deprecated Uses mLegacyTypeTracker; cannot deal with multiple Networks of the same type.
     */
    CARAPI_(AutoPtr<INetworkInfo>) GetFilteredNetworkInfo(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 uid);

    /*
     * @deprecated Uses mLegacyTypeTracker; cannot deal with multiple Networks of the same type.
     */
    AutoPtr<INetworkInfo> GetFilteredNetworkInfo(
        /* [in] */ INetworkInfo* info,
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 uid);

    AutoPtr<INetworkInfo> GetFilteredNetworkInfo(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ Int32 uid);

    CARAPI GetNetworkInfo(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 uid,
        /* [out] */ INetworkInfo** info);

    CARAPI_(AutoPtr<INetworkState>) GetNetworkStateUnchecked(
        /* [in] */ Int32 networkType);

    CARAPI_(Boolean) IsNetworkMeteredUnchecked(
        /* [in] */ Int32 networkType);

    CARAPI_(Boolean) IsLiveNetworkAgent(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ const String& msg);

    CARAPI_(Boolean) IsRequest(
        /* [in] */ INetworkRequest* request);

    CARAPI_(void) HandleAsyncChannelHalfConnect(
        /* [in] */ IMessage* msg);

    CARAPI_(void) HandleAsyncChannelDisconnected(
        /* [in] */ IMessage* msg);

    CARAPI_(void) HandleRegisterNetworkRequest(
        /* [in] */ IMessage* msg);

    CARAPI_(void) HandleReleaseNetworkRequest(
        /* [in] */ INetworkRequest* request,
        /* [in] */ Int32 callingUid);

    CARAPI_(Boolean) AddLegacyRouteToHost(
        /* [in] */ ILinkProperties* lp,
        /* [in] */ IInetAddress* addr,
        /* [in] */ Int32 netId,
        /* [in] */ Int32 uid);

    CARAPI_(void) HandleSetDependencyMet(
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean met);

    CARAPI_(void) HandleSetPolicyDataEnable(
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean enabled);

    CARAPI EnforceInternetPermission();

    CARAPI EnforceAccessPermission();

    // TODO Make this a special check when it goes public
    CARAPI EnforceChangePermission();

    CARAPI EnforceTetherAccessPermission();

    CARAPI EnforceConnectivityInternalPermission();

    CARAPI_(void) SendConnectedBroadcastDelayed(
        /* [in] */ INetworkInfo* info,
        /* [in] */ Int32 delayMs);

    CARAPI_(void) SendInetConditionBroadcast(
        /* [in] */ INetworkInfo* info);

    CARAPI_(AutoPtr<IIntent>) MakeGeneralIntent(
        /* [in] */ INetworkInfo* info,
        /* [in] */ const String& bcastType);

    CARAPI_(void) SendGeneralBroadcast(
        /* [in] */ INetworkInfo* info,
        /* [in] */ const String& bcastType);

    CARAPI_(void) SendGeneralBroadcastDelayed(
        /* [in] */ INetworkInfo* info,
        /* [in] */ const String& bcastType,
        /* [in] */ Int32 delayMs);

    CARAPI_(void) SendDataActivityBroadcast(
        /* [in] */ Int32 deviceType,
        /* [in] */ Boolean active,
        /* [in] */ Int64 tsNanos);

    CARAPI_(void) SendStickyBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI_(void) SendStickyBroadcastDelayed(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 delayMs);

    /**
     * Setup data activity tracking for the given network.
     *
     * Every {@code setupDataActivityTracking} should be paired with a
     * {@link #removeDataActivityTracking} for cleanup.
     */
    CARAPI_(void) SetupDataActivityTracking(
        /* [in] */ INetworkAgentInfo* networkAgent);

    /**
     * Remove data activity tracking when network disconnects.
     */
    CARAPI_(void) RemoveDataActivityTracking(
        /* [in] */ INetworkAgentInfo* networkAgent);

    /**
     * Reads the network specific MTU size from reources.
     * and set it on it's iface.
     */
    CARAPI_(void) UpdateMtu(
        /* [in] */ ILinkProperties* newLp,
        /* [in] */ ILinkProperties* oldLp);

    CARAPI_(void) UpdateTcpBufferSizes(
        /* [in] */ INetworkAgentInfo* nai);

    CARAPI_(void) FlushVmDnsCache();

    CARAPI_(void) LoadGlobalProxy();

    CARAPI_(void) HandleApplyDefaultProxy(
        /* [in] */ IProxyInfo* proxy);

    CARAPI_(void) HandleDeprecatedGlobalHttpProxy();

    CARAPI_(void) SendProxyBroadcast(
        /* [in] */ IProxyInfo* proxy);

    /**
     * Internally set new {@link LockdownVpnTracker}, shutting down any existing
     * {@link LockdownVpnTracker}. Can be {@code null} to disable lockdown.
     */
    CARAPI_(void) SetLockdownTracker(
        /* [in] */ LockdownVpnTracker* tracker);

    CARAPI ThrowIfLockdownEnabled();

    /**
     * Have mobile data fail fast if enabled.
     *
     * @param enabled DctConstants.ENABLED/DISABLED
     */
    void SetEnableFailFastMobileData(
        /* [in] */ Int32 enabled);

    void SetProvNotificationVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 networkType,
        /* [in] */ const String& action);

    /**
     * Show or hide network provisioning notificaitons.
     *
     * @param id an identifier that uniquely identifies this notification.  This must match
     *         between show and hide calls.  We use the NetID value but for legacy callers
     *         we concatenate the range of types with the range of NetIDs.
     */
    void SetProvNotificationVisibleIntent(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 id,
        /* [in] */ Int32 networkType,
        /* [in] */ const String& extraInfo,
        /* [in] */ IPendingIntent* intent);

    CARAPI GetProvisioningUrlBaseFromFile(
        /* [in] */ Int32 type,
        /* [out] */ String* result);

    void OnUserStart(
        /* [in] */ Int32 userId);

    void OnUserStop(
        /* [in] */ Int32 userId);

    /* Infrastructure for network sampling */

    void HandleNetworkSamplingTimeout();

    /**
     * Sets a network sampling alarm.
     */
    void SetAlarm(
        /* [in] */ Int32 timeoutInMilliseconds,
        /* [in] */ IPendingIntent* intent);

    void HandleRegisterNetworkFactory(
        /* [in] */ NetworkFactoryInfo* nfi);

    void HandleUnregisterNetworkFactory(
        /* [in] */ IMessenger* messenger);

    Boolean IsDefaultNetwork(
        /* [in] */ INetworkAgentInfo* nai);

    void HandleRegisterNetworkAgent(
        /* [in] */ INetworkAgentInfo* na);

    void UpdateLinkProperties(
        /* [in] */ INetworkAgentInfo* networkAgent,
        /* [in] */ ILinkProperties* oldLp);


    void UpdateClat(
        /* [in] */ ILinkProperties* newLp,
        /* [in] */ ILinkProperties* oldLp,
        /* [in] */ INetworkAgentInfo* na);

    void UpdateInterfaces(
        /* [in] */ ILinkProperties* newLp,
        /* [in] */ ILinkProperties* oldLp,
        /* [in] */ Int32 netId);

    /**
     * Have netd update routes from oldLp to newLp.
     * @return true if routes changed between oldLp and newLp
     */
    Boolean UpdateRoutes(
        /* [in] */ ILinkProperties* newLp,
        /* [in] */ ILinkProperties* oldLp,
        /* [in] */ Int32 netId);

    void UpdateDnses(
        /* [in] */ ILinkProperties* newLp,
        /* [in] */ ILinkProperties* oldLp,
        /* [in] */ Int32 netId,
        /* [in] */ Boolean flush);

    void SetDefaultDnsSystemProperties(
        /* [in] */ ICollection* dnses);

    void UpdateCapabilities(
        /* [in] */ INetworkAgentInfo* networkAgent,
        /* [in] */ INetworkCapabilities* networkCapabilities);

    void SendUpdatedScoreToFactories(
        /* [in] */ INetworkAgentInfo* nai);

    void SendUpdatedScoreToFactories(
        /* [in] */ INetworkRequest* networkRequest,
        /* [in] */ Int32 score);

    void CallCallbackForRequest(
        /* [in] */ NetworkRequestInfo* nri,
        /* [in] */ INetworkAgentInfo* networkAgent,
        /* [in] */ Int32 notificationType);

    void TeardownUnneededNetwork(
        /* [in] */ INetworkAgentInfo* nai);

    void HandleLingerComplete(
        /* [in] */ INetworkAgentInfo* oldNetwork);

    void MakeDefault(
        /* [in] */ INetworkAgentInfo* newNetwork);

    // Handles a network appearing or improving its score.
    //
    // - Evaluates all current NetworkRequests that can be
    //   satisfied by newNetwork, and reassigns to newNetwork
    //   any such requests for which newNetwork is the best.
    //
    // - Lingers any Networks that as a result are no longer
    //   needed. A network is needed if it is the best network for
    //   one or more NetworkRequests, or if it is a VPN.
    //
    // - Tears down newNetwork if it just became validated
    //   (i.e. nascent==true) but turns out to be unneeded.
    //   Does not tear down newNetwork if it is unvalidated,
    //   because future validation may improve newNetwork's
    //   score enough that it is needed.
    //
    // NOTE: This function only adds NetworkRequests that "newNetwork" could satisfy,
    // it does not remove NetworkRequests that other Networks could better satisfy.
    // If you need to handle decreases in score, use {@link rematchAllNetworksAndRequests}.
    // This function should be used when possible instead of {@code rematchAllNetworksAndRequests}
    // as it performs better by a factor of the number of Networks.
    //
    // @param nascent indicates if newNetwork just became validated, in which case it should be
    //               torn down if unneeded.  If nascent is false, no action is taken if newNetwork
    //               is found to be unneeded by this call.  Presumably, in this case, either:
    //               - newNetwork is unvalidated (and left alive), or
    //               - the NetworkRequests keeping newNetwork alive have been transitioned to
    //                 another higher scoring network by another call to rematchNetworkAndRequests()
    //                 and this other call also lingered newNetwork.
    void RematchNetworkAndRequests(
        /* [in] */ INetworkAgentInfo* newNetwork,
        /* [in] */ Boolean nascent);

    // Attempt to rematch all Networks with NetworkRequests.  This may result in Networks
    // being disconnected.
    // If only one Network's score or capabilities have been modified since the last time
    // this function was called, pass this Network in via the "changed" arugment, otherwise
    // pass null.
    // If only one Network has been changed but its NetworkCapabilities have not changed,
    // pass in the Network's score (from getCurrentScore()) prior to the change via
    // "oldScore", otherwise pass changed.getCurrentScore() or 0 if "changed" is null.
    void RematchAllNetworksAndRequests(
        /* [in] */ INetworkAgentInfo* changed,
        /* [in] */ Int32 oldScore);

    void UpdateInetCondition(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ Boolean valid);

    void NotifyLockdownVpn(
        /* [in] */ INetworkAgentInfo* nai);

    void UpdateNetworkInfo(
        /* [in] */ INetworkAgentInfo* networkAgent,
        /* [in] */ INetworkInfo* newInfo);

    void UpdateNetworkScore(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ Int32 score);

    // notify only this one new request of the current state
    void NotifyNetworkCallback(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ NetworkRequestInfo* nri);

    void SendLegacyNetworkBroadcast(
        /* [in] */ INetworkAgentInfo* nai,
        /* [in] */ Boolean connected,
        /* [in] */ Int32 type);

    void NotifyNetworkCallbacks(
        /* [in] */ INetworkAgentInfo* networkAgent,
        /* [in] */ Int32 notifyType);

    String NotifyTypeToName(
        /* [in] */ Int32 notifyType);

    AutoPtr<ILinkProperties> GetLinkPropertiesForTypeInternal(
        /* [in] */ Int32 networkType);

    AutoPtr<INetworkInfo> GetNetworkInfoForType(
        /* [in] */ Int32 networkType);

    AutoPtr<INetworkCapabilities> GetNetworkCapabilitiesForType(
        /* [in] */ Int32 networkType);

public:
    friend class InternalHandler;


    /**
     * used internally to change our mobile data enabled flag
     */
    static const Int32 EVENT_CHANGE_MOBILE_DATA_ENABLED; // = 2;

    /**
     * used internally to clear a wakelock when transitioning
     * from one net to another.  Clear happens when we get a new
     * network - EVENT_EXPIRE_NET_TRANSITION_WAKELOCK happens
     * after a timeout if no network is found (typically 1 min).
     */
    static const Int32 EVENT_CLEAR_NET_TRANSITION_WAKELOCK; // = 8;

    /**
     * used internally to reload global proxy settings
     */
    static const Int32 EVENT_APPLY_GLOBAL_HTTP_PROXY; // = 9;

    /**
     * used internally to set external dependency met/unmet
     * arg1 = ENABLED (met) or DISABLED (unmet)
     * arg2 = NetworkType
     */
    static const Int32 EVENT_SET_DEPENDENCY_MET; // = 10;

    /**
     * used internally to send a sticky broadcast delayed.
     */
    static const Int32 EVENT_SEND_STICKY_BROADCAST_INTENT; // = 11;

    /**
     * Used internally to
     * {@link NetworkStateTracker#setPolicyDataEnable(Boolean)}.
     */
    static const Int32 EVENT_SET_POLICY_DATA_ENABLE; // = 12;

    /**
     * Used internally to disable fail fast of mobile data
     */
    static const Int32 EVENT_ENABLE_FAIL_FAST_MOBILE_DATA; // = 14;

    /**
     * used internally to indicate that data sampling interval is up
     */
    static const Int32 EVENT_SAMPLE_INTERVAL_ELAPSED; // = 15;

    /**
     * PAC manager has received new port.
     */
    static const Int32 EVENT_PROXY_HAS_CHANGED; // = 16;

    /**
     * used internally when registering NetworkFactories
     * obj = NetworkFactoryInfo
     */
    static const Int32 EVENT_REGISTER_NETWORK_FACTORY; // = 17;

    /**
     * used internally when registering NetworkAgents
     * obj = Messenger
     */
    static const Int32 EVENT_REGISTER_NETWORK_AGENT; // = 18;

    /**
     * used to add a network request
     * includes a NetworkRequestInfo
     */
    static const Int32 EVENT_REGISTER_NETWORK_REQUEST; // = 19;

    /**
     * indicates a timeout period is over - check if we had a network yet or not
     * and if not, call the timeout calback (but leave the request live until they
     * cancel it.
     * includes a NetworkRequestInfo
     */
    static const Int32 EVENT_TIMEOUT_NETWORK_REQUEST; // = 20;

    /**
     * used to add a network listener - no request
     * includes a NetworkRequestInfo
     */
    static const Int32 EVENT_REGISTER_NETWORK_LISTENER; // = 21;

    /**
     * used to remove a network request, either a listener or a real request
     * arg1 = UID of caller
     * obj = NetworkRequest
     */
    static const Int32 EVENT_RELEASE_NETWORK_REQUEST; // = 22;

    /**
     * used internally when registering NetworkFactories
     * obj = Messenger
     */
    static const Int32 EVENT_UNREGISTER_NETWORK_FACTORY; // = 23;

    /**
     * used internally to expire a wakelock when transitioning
     * from one net to another.  Expire happens when we fail to find
     * a new network (typically after 1 minute) -
     * EVENT_CLEAR_NET_TRANSITION_WAKELOCK happens if we had found
     * a replacement network.
     */
    static const Int32 EVENT_EXPIRE_NET_TRANSITION_WAKELOCK; // = 24;

    /**
     * Used internally to indicate the system is ready.
     */
    static const Int32 EVENT_SYSTEM_READY; // = 25;

private:
    static const String TAG;

    static const Boolean DBG;
    static const Boolean VDBG;

    // network sampling debugging
    static const Boolean SAMPLE_DBG;

    static const Boolean LOGD_RULES;

    // TODO: create better separation between radio types and network types

    // how long to wait before switching back to a radio's default network
    static const Int32 RESTORE_DEFAULT_NETWORK_DELAY;
    // system property that can override the above value
    static const String NETWORK_RESTORE_DELAY_PROP_NAME;

    // Default value if FAIL_FAST_TIME_MS is not set
    static const Int32 DEFAULT_FAIL_FAST_TIME_MS;
    // system property that can override DEFAULT_FAIL_FAST_TIME_MS
    static const String FAIL_FAST_TIME_MS;

    static const String ACTION_PKT_CNT_SAMPLE_INTERVAL_ELAPSED;

    static const int SAMPLE_INTERVAL_ELAPSED_REQUEST_CODE;

    AutoPtr<IPendingIntent> mSampleIntervalElapsedIntent;

    // Set network sampling interval at 12 minutes, this way, even if the timers get
    // aggregated, it will fire at around 15 minutes, which should allow us to
    // aggregate this timer with other timers (specially the socket keep alive timers)
    static const int DEFAULT_SAMPLING_INTERVAL_IN_SECONDS;

    // start network sampling a minute after booting ...
    static const int DEFAULT_START_SAMPLING_INTERVAL_IN_SECONDS;

    AutoPtr<IAlarmManager> mAlarmManager;

    AutoPtr<CTethering> mTethering;

    AutoPtr<PermissionMonitor> mPermissionMonitor;

    AutoPtr<IKeyStore> mKeyStore;

    HashMap<Int32, AutoPtr<Vpn> > mVpns;
    Object mVpnsLock;

    Boolean mLockdownEnabled;
    AutoPtr<LockdownVpnTracker> mLockdownTracker;

    AutoPtr<Nat464Xlat> mClat;

    /** Lock around {@link #mUidRules} and {@link #mMeteredIfaces}. */
    Object mRulesLock;
    /** Currently active network rules by UID. */
    HashMap<Int32, Int32> mUidRules;
    /** Set of ifaces that are costly. */
    HashSet<String> mMeteredIfaces;

    /**
     * Sometimes we want to refer to the individual network state
     * trackers separately, and sometimes we just want to treat them
     * abstractly.
     */
    AutoPtr< ArrayOf<INetworkStateTracker*> > mNetTrackers;

    AutoPtr<IContext> mContext;
    Int32 mNetworkPreference;
    Int32 mActiveDefaultNetwork;
    // 0 is full bad, 100 is full good
    Int32 mDefaultInetConditionPublished;

    Object mDnsLock;
    Int32 mNumDnsEntries;

    Boolean mTestMode;
    static AutoPtr<CConnectivityService> sServiceInstance;

    AutoPtr<IINetworkManagementService> mNetd;
    AutoPtr<IINetworkPolicyManager> mPolicyManager;

    String mCurrentTcpBufferSizes;

    static const Int32 ENABLED;
    static const Int32 DISABLED;

    static const Boolean ADD;
    static const Boolean REMOVE;

    static const Boolean TO_DEFAULT_TABLE;
    static const Boolean TO_SECONDARY_TABLE;

    /** Handler used for internal events. */
    AutoPtr<InternalHandler> mHandler;
    /** Handler used for incoming {@link NetworkStateTracker} events. */
    AutoPtr<NetworkStateTrackerHandler> mTrackerHandler;

    Boolean mSystemReady;
    AutoPtr<IIntent> mInitialBroadcast;

    AutoPtr<IPowerManagerWakeLock> mNetTransitionWakeLock;
    String mNetTransitionWakeLockCausedBy;
    Int32 mNetTransitionWakeLockSerialNumber;
    Int32 mNetTransitionWakeLockTimeout;

    AutoPtr<IInetAddress> mDefaultDns;

    // used in DBG mode to track inet condition reports
    static const Int32 INET_CONDITION_LOG_MAX_SIZE;
    AutoPtr< List<String> > mInetLog;

    // track the current default http proxy - tell the world if we get a new one (real change)
    AutoPtr<IProxyInfo> mDefaultProxy;
    Object mProxyLock;
    Boolean mDefaultProxyDisabled;

    // track the global proxy.
    AutoPtr<IProxyInfo> mGlobalProxy;

    AutoPtr<PacManager> mPacManager;

    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<IUserManager> mUserManager;

    AutoPtr< ArrayOf<INetworkConfig*> > mNetConfigs;
    Int32 mNetworksDefined;


    // the set of network types that can only be enabled by system/sig apps
    List<Int32> mProtectedNetworks;

    AutoPtr<DataConnectionStats> mDataConnectionStats;

    AutoPtr<IAtomicInteger32> mEnableFailFastMobileDataTag;

    AutoPtr<ITelephonyManager> mTelephonyManager;
    // sequence number for Networks; keep in sync with system/netd/NetworkController.cpp
    static const Int32 MIN_NET_ID; // some reserved marks
    static const Int32 MAX_NET_ID;
    Int32 mNextNetId;

    // sequence number of NetworkRequests
    Int32 mNextNetworkRequestId;

    AutoPtr<LegacyTypeTracker> mLegacyTypeTracker;

    static const String DEFAULT_TCP_BUFFER_SIZES;


    /** Location to an updatable file listing carrier provisioning urls.
     *  An example:
     *
     * <?xml version="1.0" encoding="utf-8"?>
     *  <provisioningUrls>
     *   <provisioningUrl mcc="310" mnc="4">http://myserver.com/foo?mdn=%3$s&amp;iccid=%1$s&amp;imei=%2$s</provisioningUrl>
     *   <redirectedUrl mcc="310" mnc="4">http://www.google.com</redirectedUrl>
     *  </provisioningUrls>
     */
    static const String PROVISIONING_URL_PATH;

    AutoPtr<IFile> mProvisioningUrlFile;

    /** XML tag for root element. */
    static const String TAG_PROVISIONING_URLS;
    /** XML tag for individual url */
    static const String TAG_PROVISIONING_URL;
    /** XML tag for redirected url */
    static const String TAG_REDIRECTED_URL;
    /** XML attribute for mcc */
    static const String ATTR_MCC;
    /** XML attribute for mnc */
    static const String ATTR_MNC;

    static const Int32 REDIRECTED_PROVISIONING;
    static const Int32 PROVISIONING;

    static const String NOTIFICATION_ID;
    volatile Boolean mIsNotificationVisible;

    AutoPtr<IINetworkManagementEventObserver> mDataActivityObserver;

    AutoPtr<IINetworkPolicyListener> mPolicyListener;

    AutoPtr<IBroadcastReceiver> mUserPresentReceiver;

    AutoPtr<IBroadcastReceiver> mUserIntentReceiver;

    AutoPtr<IHashMap> mNetworkFactoryInfos; //= new HashMap<Messenger, NetworkFactoryInfo>();
    AutoPtr<IHashMap> mNetworkRequests; //= new HashMap<NetworkRequest, NetworkRequestInfo>();

    /**
     * NetworkAgentInfo supporting a request by requestId.
     * These have already been vetted (their Capabilities satisfy the request)
     * and the are the highest scored network available.
     * the are keyed off the Requests requestId.
     */
    AutoPtr<ISparseArray> mNetworkForRequestId; //=new SparseArray<NetworkAgentInfo>();

    AutoPtr<ISparseArray> mNetworkForNetId; //= new SparseArray<NetworkAgentInfo>();

    // NetworkAgentInfo keyed off its connecting messenger
    // TODO - eval if we can reduce the number of lists/hashmaps/sparsearrays
    AutoPtr<IHashMap> mNetworkAgentInfos; //= new HashMap<Messenger, NetworkAgentInfo>();

    AutoPtr<INetworkRequest> mDefaultRequest;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CCONNECTIVITYSERVICE_H__
