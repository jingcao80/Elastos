
#ifndef __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETNETWORKFACTORY_H__
#define __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETNETWORKFACTORY_H__

#include "elastos/droid/net/NetworkFactory.h"
#include "elastos/droid/net/NetworkAgent.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::NetworkAgent;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::NetworkFactory;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IEthernetManager;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Server::Net::BaseNetworkObserver;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {


/**
 * Manages connectivity for an Ethernet interface.
 *
 * Ethernet Interfaces may be present at boot time or appear after boot (e.g.,
 * for Ethernet adapters connected over USB). This class currently supports
 * only one interface. When an interface appears on the system (or is present
 * at boot time) this class will start tracking it and bring it up, and will
 * attempt to connect when requested. Any other interfaces that subsequently
 * appear will be ignored until the tracked interface disappears. Only
 * interfaces whose names match the <code>config_ethernet_iface_regex</code>
 * regular expression are tracked.
 *
 * This class reports a static network score of 70 when it is tracking an
 * interface and that interface's link is up, and a score of 0 otherwise.
 *
 * @hide
 */
class EthernetNetworkFactory : public Object
{
private:
    class LocalNetworkFactory : public NetworkFactory
    {
    public:
        CARAPI constructor(
            /* [in] */ const String& name,
            /* [in] */ IContext* context,
            /* [in] */ ILooper* looper,
            /* [in] */ EthernetNetworkFactory* host);

        TO_STRING_IMPL("EthernetNetworkFactory::LocalNetworkFactory")

    protected:
        CARAPI StartNetwork();

        CARAPI StopNetwork();

    private:
        EthernetNetworkFactory* mHost;
    };

    class InterfaceObserver : public BaseNetworkObserver
    {
    public:
        InterfaceObserver(
            /* [in] */ EthernetNetworkFactory* host);

        // @Override
        CARAPI InterfaceLinkStateChanged(
            /* [in] */ const String& iface,
            /* [in] */ Boolean up);

        // @Override
        CARAPI InterfaceAdded(
            /* [in] */ const String& iface);

        // @Override
        CARAPI InterfaceRemoved(
            /* [in] */ const String& iface);

    private:
        EthernetNetworkFactory* mHost;
    };

    class DhcpRunnable : public Runnable
    {
    private:
        class MyNetworkAgent : public NetworkAgent
        {
        public:
            MyNetworkAgent(
                /* [in] */ EthernetNetworkFactory* host);

            CARAPI Unwanted();

            TO_STRING_IMPL("EthernetNetworkFactory::DhcpRunnable::MyNetworkAgent")

        private:
            EthernetNetworkFactory* mHost;
        };

    public:
        DhcpRunnable(
            /* [in] */ EthernetNetworkFactory* host);

        CARAPI Run();

    private:
        EthernetNetworkFactory* mHost;
    };

public:
    CARAPI constructor();

    CARAPI UpdateAgent();

    /* Called by the NetworkFactory on the handler thread. */
    CARAPI OnRequestNetwork();

    /**
     * Begin monitoring connectivity
     */
    CARAPI Start(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    CARAPI Stop();

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IIndentingPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    /**
     * Updates interface state variables.
     * Called on link state changes or on startup.
     */
    CARAPI_(void) UpdateInterfaceState(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI_(void) SetInterfaceUp(
        /* [in] */ const String& iface);

    CARAPI_(Boolean) MaybeTrackInterface(
        /* [in] */ const String& iface);

    CARAPI_(void) StopTrackingInterface(
        /* [in] */ const String& iface);

    CARAPI_(Boolean) SetStaticIpAddress(
        /* [in] */ IStaticIpConfiguration* staticConfig);

    CARAPI_(void) InitNetworkCapabilities();

private:
    static const String NETWORK_TYPE;
    static const String TAG;
    static const Int32 NETWORK_SCORE;
    static const Boolean DBG;

    /** Tracks interface changes. Called from NetworkManagementService. */
    AutoPtr<InterfaceObserver> mInterfaceObserver;

    /** For static IP configuration */
    AutoPtr<IEthernetManager> mEthernetManager;

    /** To set link state and configure IP addresses. */
    AutoPtr<IINetworkManagementService> mNMService;

    /* To communicate with ConnectivityManager */
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;
    AutoPtr<NetworkAgent> mNetworkAgent;
    AutoPtr<LocalNetworkFactory> mFactory;
    AutoPtr<IContext> mContext;

    /** Product-dependent regular expression of interface names we track. */
    static String mIfaceMatch;

    /** Data members. All accesses to these must be synchronized(this). */
    static String mIface;
    String mHwAddr;
    static Boolean mLinkUp;
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<ILinkProperties> mLinkProperties;
};

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETNETWORKFACTORY_H__
