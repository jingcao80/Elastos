
#include "elastos/droid/server/ethernet/EthernetNetworkFactory.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/StringUtils.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::CDhcpResults;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IDhcpResults;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::IpConfigurationIpAssignment;
using Elastos::Droid::Net::STATIC_IpAssignment;
using Elastos::Droid::Net::IpConfigurationProxySettings;
using Elastos::Droid::Net::PAC_ProxySettings;
using Elastos::Droid::Net::STATIC_ProxySettings;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_OBTAINING_IPADDR;
using Elastos::Droid::Net::NetworkInfoState_DISCONNECTED;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::StringUtils;
using Elastos::IO::IPrintWriter;
using Elastos::Net::IInet4Address;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

ECode EthernetNetworkFactory::LocalNetworkFactory::constructor(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ ILooper* looper,
    /* [in] */ EthernetNetworkFactory* host)
{
    mHost = host;
    AutoPtr<INetworkCapabilities> nc;
    CNetworkCapabilities::New((INetworkCapabilities**)&nc);
    return NetworkFactory::constructor(looper, context, name, nc);
}

ECode EthernetNetworkFactory::LocalNetworkFactory::StartNetwork()
{
    return mHost->OnRequestNetwork();
}

ECode EthernetNetworkFactory::LocalNetworkFactory::StopNetwork()
{
    return NOERROR;
}

EthernetNetworkFactory::InterfaceObserver::InterfaceObserver(
    /* [in] */ EthernetNetworkFactory* host)
    : mHost(host)
{}

ECode EthernetNetworkFactory::InterfaceObserver::InterfaceLinkStateChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    mHost->UpdateInterfaceState(iface, up);
    return NOERROR;
}

ECode EthernetNetworkFactory::InterfaceObserver::InterfaceAdded(
    /* [in] */ const String& iface)
{
    mHost->MaybeTrackInterface(iface);
    return NOERROR;
}

ECode EthernetNetworkFactory::InterfaceObserver::InterfaceRemoved(
    /* [in] */ const String& iface)
{
    mHost->StopTrackingInterface(iface);
    return NOERROR;
}

EthernetNetworkFactory::DhcpRunnable::MyNetworkAgent::MyNetworkAgent(
    /* [in] */ EthernetNetworkFactory* host)
    : mHost(host)
{}

ECode EthernetNetworkFactory::DhcpRunnable::MyNetworkAgent::Unwanted()
{
    AutoLock lock(this);
    if (this == mHost->mNetworkAgent) {
        Boolean result;
        NetworkUtils::StopDhcp(mHost->mIface, &result);

        mHost->mLinkProperties->Clear();
        mHost->mNetworkInfo->SetDetailedState(
            NetworkInfoState_DISCONNECTED, String(NULL), mHost->mHwAddr);
        mHost->UpdateAgent();
        mHost->mNetworkAgent = NULL;
        if (FAILED(mHost->mNMService->ClearInterfaceAddresses(mIface))) {
            Logger::E(TAG, "Failed to clear addresses or disable ipv6");
        }
    }
    else {
        Logger::D(TAG, "Ignoring unwanted as we have a more modern instance");
    }

    return NOERROR;
}

EthernetNetworkFactory::DhcpRunnable::DhcpRunnable(
    /* [in] */ EthernetNetworkFactory* host)
    : mHost(host)
{}

ECode EthernetNetworkFactory::DhcpRunnable::Run()
{
    if (DBG)
        Logger::I(TAG, "dhcpThread(+%s): mNetworkInfo=%s", mHost->mIface.string(), TO_CSTR(mHost->mNetworkInfo));
    AutoPtr<ILinkProperties> linkProperties;

    AutoPtr<IIpConfiguration> config;
    mHost->mEthernetManager->GetConfiguration((IIpConfiguration**)&config);

    IpConfigurationIpAssignment ia;
    config->GetIpAssignment(&ia);
    if (ia == STATIC_IpAssignment) {
        AutoPtr<IStaticIpConfiguration> sic;
        config->GetStaticIpConfiguration((IStaticIpConfiguration**)&sic);
        if (!mHost->SetStaticIpAddress(sic)) {
            // We've already logged an error.
            return NOERROR;
        }
        sic->ToLinkProperties(mHost->mIface, (ILinkProperties**)&linkProperties);
    }
    else {
        mHost->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_OBTAINING_IPADDR,
            String(NULL), mHost->mHwAddr);

        AutoPtr<IDhcpResults> dhcpResults;
        CDhcpResults::New((IDhcpResults**)&dhcpResults);
        // TODO: Handle DHCP renewals better.
        // In general runDhcp handles DHCP renewals for us, because
        // the dhcp client stays running, but if the renewal fails,
        // we will lose our IP address and connectivity without
        // noticing.
        Boolean res = FALSE;
        NetworkUtils::RunDhcp(mHost->mIface, dhcpResults, &res);
        if (!res) {
            String str;
            NetworkUtils::GetDhcpError(&str);
            Logger::E(TAG, "DHCP request error:%s", str.string());
            // set our score lower than any network could go
            // so we get dropped.
            mHost->mFactory->SetScoreFilter(-1);
            return NOERROR;
        }
        IStaticIpConfiguration::Probe(dhcpResults)->ToLinkProperties(mHost->mIface, (ILinkProperties**)&linkProperties);
    }
    IpConfigurationProxySettings ps;
    config->GetProxySettings(&ps);
    if (ps == STATIC_ProxySettings || ps == PAC_ProxySettings) {
        AutoPtr<IProxyInfo> proxyInfo;
        config->GetHttpProxy((IProxyInfo**)&proxyInfo);
        linkProperties->SetHttpProxy(proxyInfo);
    }

    AutoPtr<IResources> res;
    mHost->mContext->GetResources((IResources**)&res);
    String tcpBufferSizes;
    res->GetString(R::string::config_ethernet_tcp_buffers, &tcpBufferSizes);
    if (TextUtils::IsEmpty(tcpBufferSizes) == FALSE) {
        linkProperties->SetTcpBufferSizes(tcpBufferSizes);
    }

    AutoLock lock(mHost);
    if (mHost->mNetworkAgent != NULL) {
        Logger::E(TAG, "Already have a NetworkAgent - aborting new request");
        return NOERROR;
    }
    mHost->mLinkProperties = linkProperties;
    mHost->mNetworkInfo->SetIsAvailable(TRUE);
    mHost->mNetworkInfo->SetDetailedState(
        NetworkInfoDetailedState_CONNECTED, String(NULL), mHost->mHwAddr);

    // Create our NetworkAgent.
    mHost->mNetworkAgent = new MyNetworkAgent(mHost);
    AutoPtr<ILooper> looper;
    mHost->mFactory->GetLooper((ILooper**)&looper);
    mHost->mNetworkAgent->constructor(looper, mHost->mContext, NETWORK_TYPE,
        mHost->mNetworkInfo, mHost->mNetworkCapabilities, mHost->mLinkProperties,
        NETWORK_SCORE);

    return NOERROR;
}

const String EthernetNetworkFactory::NETWORK_TYPE("Ethernet");
const String EthernetNetworkFactory::TAG("EthernetNetworkFactory");
const Int32 EthernetNetworkFactory::NETWORK_SCORE = 70;
const Boolean EthernetNetworkFactory::DBG = TRUE;

String EthernetNetworkFactory::mIfaceMatch("");
String EthernetNetworkFactory::mIface("");
Boolean EthernetNetworkFactory::mLinkUp = FALSE;

ECode EthernetNetworkFactory::constructor()
{
    CNetworkInfo::New(IConnectivityManager::TYPE_ETHERNET, 0,
        NETWORK_TYPE, String(""), (INetworkInfo**)&mNetworkInfo);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    InitNetworkCapabilities();

    return NOERROR;
}

void EthernetNetworkFactory::UpdateInterfaceState(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    if (!mIface.Equals(iface)) {
        return;
    }
    Logger::D(TAG, "updateInterface: %s link %s", iface.string(), (up ? "up" : "down"));

    AutoLock lock(this);
    mLinkUp = up;
    mNetworkInfo->SetIsAvailable(up);
    if (!up) {
        // Tell the agent we're disconnected. It will call disconnect().
        mNetworkInfo->SetDetailedState(NetworkInfoState_DISCONNECTED, String(NULL), mHwAddr);
    }
    UpdateAgent();
    // set our score lower than any network could go
    // so we get dropped.  TODO - just unregister the factory
    // when link goes down.
    mFactory->SetScoreFilter(up ? NETWORK_SCORE : -1);
}

void EthernetNetworkFactory::SetInterfaceUp(
    /* [in] */ const String& iface)
{
    // Bring up the interface so we get link status indications.
    // try {
        mNMService->SetInterfaceUp(iface);
        String hwAddr;
        AutoPtr<IInterfaceConfiguration> config;
        mNMService->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&config);

        if (config == NULL) {
            Logger::E(TAG, "NULL iterface config for %s. Bailing out.", iface.string());
            return;
        }

        AutoLock lock(this);
        if (mIface.IsEmpty()) {
            mIface = iface;
            config->GetHardwareAddress(&mHwAddr);
            mNetworkInfo->SetIsAvailable(TRUE);
            mNetworkInfo->SetExtraInfo(mHwAddr);
        }
        else {
            Logger::E(TAG, "Interface unexpectedly changed from %s to %s", iface.string(), mIface.string());
            mNMService->SetInterfaceDown(iface);
        }
    // } catch (RemoteException e) {
    //     Logger::E(TAG, "Error upping interface " + mIface + ": " + e);
    // }
}

Boolean EthernetNetworkFactory::MaybeTrackInterface(
    /* [in] */ const String& iface)
{
    // If we don't already have an interface, and if this interface matches
    // our regex, start tracking it.
    Boolean matches;
    StringUtils::Matches(iface, mIfaceMatch, &matches);
    if (!matches || !mIface.IsEmpty())
        return FALSE;

    Logger::D(TAG, "Started tracking interface %s", iface.string());
    SetInterfaceUp(iface);
    return TRUE;
}

void EthernetNetworkFactory::StopTrackingInterface(
    /* [in] */ const String& iface)
{
    if (!iface.Equals(mIface))
        return;

    Logger::D(TAG, "Stopped tracking interface %s", iface.string());
    // TODO: Unify this codepath with stop().
    AutoLock lock(this);
    Boolean result;
    NetworkUtils::StopDhcp(mIface, &result);
    mIface = "";
    mHwAddr = NULL;
    mNetworkInfo->SetExtraInfo(String(NULL));
    mLinkUp = FALSE;
    mNetworkInfo->SetDetailedState(NetworkInfoState_DISCONNECTED, String(NULL), mHwAddr);
    UpdateAgent();
    mNetworkAgent = NULL;
    mNetworkInfo = NULL;
    CNetworkInfo::New(IConnectivityManager::TYPE_ETHERNET, 0, NETWORK_TYPE, String(""), (INetworkInfo**)&mNetworkInfo);
    mLinkProperties = NULL;
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
}

Boolean EthernetNetworkFactory::SetStaticIpAddress(
    /* [in] */ IStaticIpConfiguration* staticConfig)
{
    AutoPtr<ILinkAddress> ipAddress;
    AutoPtr<IInetAddress> gateway;
    AutoPtr<IArrayList> dnsServers;
    staticConfig->GetIpAddress((ILinkAddress**)&ipAddress);
    staticConfig->GetGateway((IInetAddress**)&dnsServers);
    staticConfig->GetDnsServers((IArrayList**)&dnsServers);
    Int32 size;
    if (ipAddress != NULL && gateway != NULL &&
        (dnsServers->GetSize(&size), size > 0)) {
        // try {
            Logger::I(TAG, "Applying static IPv4 configuration to %s: %s", mIface.string(), TO_CSTR(staticConfig));
            AutoPtr<IInterfaceConfiguration> config;
            mNMService->GetInterfaceConfig(mIface, (IInterfaceConfiguration**)&config);
            config->SetLinkAddress(ipAddress);
            mNMService->SetInterfaceConfig(mIface, config);
            return TRUE;
        // } catch(RemoteException|IllegalStateException e) {
        //    Logger::E(TAG, "Setting static IP address failed: " + e.getMessage());
        // }
    } else {
        Logger::E(TAG, "Invalid static IP configuration.");
    }
    return FALSE;
}

ECode EthernetNetworkFactory::UpdateAgent()
{
    AutoLock lock(this);
    if (mNetworkAgent == NULL)
        return NOERROR;
    if (DBG) {
        Logger::I(TAG, "Updating mNetworkAgent with: %s, %s, %s",
            TO_CSTR(mNetworkCapabilities), TO_CSTR(mNetworkInfo), TO_CSTR(mLinkProperties));
    }
    mNetworkAgent->SendNetworkCapabilities(mNetworkCapabilities);
    mNetworkAgent->SendNetworkInfo(mNetworkInfo);
    mNetworkAgent->SendLinkProperties(mLinkProperties);
    // never set the network score below 0.
    mNetworkAgent->SendNetworkScore(mLinkUp? NETWORK_SCORE : 0);

    return NOERROR;
}

/* Called by the NetworkFactory on the handler thread. */
ECode EthernetNetworkFactory::OnRequestNetwork()
{
    // TODO: Handle DHCP renew.
    AutoPtr<DhcpRunnable> runnable = new DhcpRunnable(this);
    AutoPtr<IThread> dhcpThread;
    CThread::New(runnable, (IThread**)&dhcpThread);
    dhcpThread->Start();

    return NOERROR;
}

/**
 * Begin monitoring connectivity
 */
ECode EthernetNetworkFactory::Start(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    AutoLock lock(this);
    // The services we use.
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mNMService = IINetworkManagementService::Probe(service);
    service = NULL;
    context->GetSystemService(IContext::ETHERNET_SERVICE, (IInterface**)&service);
    mEthernetManager = IEthernetManager::Probe(service) ;

    // Interface match regex.
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetString(R::string::config_ethernet_iface_regex, &mIfaceMatch);

    // Create and register our NetworkFactory.
    AutoPtr<ILooper> looper;
    target->GetLooper((ILooper**)&looper);
    mFactory = new LocalNetworkFactory();
    mFactory->constructor(NETWORK_TYPE, context, looper, this);
    mFactory->SetCapabilityFilter(mNetworkCapabilities);
    mFactory->SetScoreFilter(-1); // this set high when we have an iface
    mFactory->Register();

    mContext = context;

    // Start tracking interface change events.
    mInterfaceObserver = new InterfaceObserver(this);
    // try {
        mNMService->RegisterObserver(mInterfaceObserver);
    // } catch (RemoteException e) {
    //     Logger::E(TAG, "Could not register InterfaceObserver " + e);
    // }

    // If an Ethernet interface is already connected, start tracking that.
    // Otherwise, the first Ethernet interface to appear will be tracked.
    // try {
        AutoPtr<ArrayOf<String> > ifaces;
        mNMService->ListInterfaces((ArrayOf<String>**)&ifaces);
        Int32 length = ifaces->GetLength();
        for (Int32 i = 0; i < length; i++) {
            String iface = (*ifaces)[i];
            // TODO: need synchronized here????
            // synchronized(this) {
                if (MaybeTrackInterface(iface)) {
                    // We have our interface. Track it.
                    // Note: if the interface already has link (e.g., if we
                    // crashed and got restarted while it was running),
                    // we need to fake a link up notification so we start
                    // configuring it. Since we're already holding the lock,
                    // any real link up/down notification will only arrive
                    // after we've done this.
                    AutoPtr<IInterfaceConfiguration> config;
                    mNMService->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&config);
                    Boolean hasFlag = FALSE;
                    config->HasFlag(String("running"), &hasFlag);
                    if (hasFlag) {
                        UpdateInterfaceState(iface, TRUE);
                    }
                    break;
                }
            // }
        }
    // } catch (RemoteException e) {
    //     Logger::E(TAG, "Could not get list of interfaces " + e);
    // }
    return NOERROR;
}

ECode EthernetNetworkFactory::Stop()
{
    AutoLock lock(this);
    Boolean result;
    NetworkUtils::StopDhcp(mIface, &result);
    // ConnectivityService will only forget our NetworkAgent if we send it a NetworkInfo object
    // with a state of DISCONNECTED or SUSPENDED. So we can't simply clear our NetworkInfo here:
    // that sets the state to IDLE, and ConnectivityService will still think we're connected.
    //
    // TODO: stop using explicit comparisons to DISCONNECTED / SUSPENDED in ConnectivityService,
    // and instead use isConnectedOrConnecting().
    mNetworkInfo->SetDetailedState(NetworkInfoState_DISCONNECTED, String(NULL), mHwAddr);
    mLinkUp = FALSE;
    UpdateAgent();
    mLinkProperties = NULL;
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    mNetworkAgent = NULL;
    mIface = "";
    mHwAddr = NULL;
    CNetworkInfo::New(IConnectivityManager::TYPE_ETHERNET, 0, NETWORK_TYPE, String(""), (INetworkInfo**)&mNetworkInfo);
    mFactory->Unregister();
    return NOERROR;
}

void EthernetNetworkFactory::InitNetworkCapabilities()
{
    mNetworkCapabilities = NULL;
    CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkCapabilities);
    mNetworkCapabilities->AddTransportType(INetworkCapabilities::TRANSPORT_ETHERNET);
    mNetworkCapabilities->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    mNetworkCapabilities->AddCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED);
    // We have no useful data on bandwidth. Say 100M up and 100M down. :-(
    mNetworkCapabilities->SetLinkUpstreamBandwidthKbps(100 * 1000);
    mNetworkCapabilities->SetLinkDownstreamBandwidthKbps(100 * 1000);
}

void EthernetNetworkFactory::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IIndentingPrintWriter* _pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoLock lock(this);
    IPrintWriter* pw = IPrintWriter::Probe(_pw);
    if (!TextUtils::IsEmpty(mIface)) {
        pw->Println(String("Tracking interface: ") + mIface);
        _pw->IncreaseIndent();
        pw->Println(String("MAC address: ") + mHwAddr);
        pw->Println(String("Link state: ") + (mLinkUp ? "up" : "down"));
        _pw->DecreaseIndent();
    }
    else {
        pw->Println(String("Not tracking any interface"));
    }

    pw->Println();
    pw->Println(String("NetworkInfo: ") + TO_STR(mNetworkInfo));
    pw->Println(String("LinkProperties: ") + TO_STR(mLinkProperties));
    pw->Println(String("NetworkAgent: ") + TO_STR(mNetworkAgent));
}

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos
