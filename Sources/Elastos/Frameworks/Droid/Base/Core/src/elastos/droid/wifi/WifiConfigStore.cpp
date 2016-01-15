
#include "elastos/droid/wifi/WifiConfigStore.h"
#include "elastos/droid/net/NetworkUtils.h"
#ifdef DROID_CORE
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/CLinkProperties.h"
// TODO #include "elastos/droid/net/CProxyProperties.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/wifi/CWifiConfiguration.h"
#include "elastos/droid/wifi/CWifiSsid.h"
#include "elastos/droid/wifi/CWifiSsidHelper.h"
#include "elastos/droid/wifi/CWpsResult.h"
#include "elastos/droid/wifi/CWpsInfo.h"
#include "elastos/droid/wifi/CWifiConfigurationKeyMgmt.h"
#include "elastos/droid/wifi/CWifiConfigurationProtocol.h"
#include "elastos/droid/wifi/CWifiConfigurationAuthAlgorithm.h"
#include "elastos/droid/wifi/CWifiConfigurationPairwiseCipher.h"
#include "elastos/droid/wifi/CWifiConfigurationGroupCipher.h"
#include "elastos/droid/wifi/CWifiConfigurationHelper.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CUserHandleHelper.h"
// TODO #include "elastos/droid/utility/CParcelableObjectContainer.h"
#endif
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>


using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;

using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::Net::IInetAddress;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::CLinkProperties;
// TODO using Elastos::Droid::Net::CProxyProperties;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::TextUtils;
// TODO using Elastos::Droid::Utility::CParcelableObjectContainer;

#ifndef FAIL_GOTO
#define FAIL_GOTO(expr, label) \
    do { \
        ec = expr; \
        if (FAILED(ec)) goto label; \
    } while(0);
#endif

namespace Elastos {
namespace Droid {
namespace Wifi {

//==============================================================
// WifiConfigStore::DelayedDiskWrite
//==============================================================

ECode WifiConfigStore::DelayedDiskWrite::DiskWriteRunnable::Run()
{
    DelayedDiskWrite::OnWriteCalled(mNetworks);
    return NOERROR;
}

//==============================================================
// WifiConfigStore::DelayedDiskWrite
//==============================================================
AutoPtr<IHandlerThread> WifiConfigStore::DelayedDiskWrite::sDiskWriteHandlerThread;
AutoPtr<IHandler> WifiConfigStore::DelayedDiskWrite::sDiskWriteHandler;
/* Tracks multiple writes on the same thread */
Int32 WifiConfigStore::DelayedDiskWrite::sWriteSequence = 0;
String WifiConfigStore::DelayedDiskWrite::TAG("DelayedDiskWrite");
Mutex WifiConfigStore::DelayedDiskWrite::sLock;

void WifiConfigStore::DelayedDiskWrite::Write(
    /* [in] */ ArrayOf<IWifiConfiguration*>* networks)
{
    /* Do a delayed write to disk on a seperate handler thread */
    {
        AutoLock lock(sLock);
        if (++sWriteSequence == 1) {
            CHandlerThread::New(String("WifiConfigThread"), (IHandlerThread**)&sDiskWriteHandlerThread);
            sDiskWriteHandlerThread->Start();

            AutoPtr<ILooper> looper;
            sDiskWriteHandlerThread->GetLooper((ILooper**)&looper);
            CHandler::New(looper, (IHandler**)&sDiskWriteHandler);
        }
    }

    AutoPtr<IRunnable> runnable = new DiskWriteRunnable(networks);
    Boolean tmp;
    sDiskWriteHandler->Post(runnable, &tmp);
}

void WifiConfigStore::DelayedDiskWrite::OnWriteCalled(
    /* [in] */ ArrayOf<IWifiConfiguration*>* networks)
{
    AutoPtr<IDataOutputStream> _out;
    // try {
    AutoPtr<IFileOutputStream> fs;
    CFileOutputStream::New(mIpConfigFile, (IFileOutputStream**)&fs);

    AutoPtr<IBufferedOutputStream> bos;
    CBufferedOutputStream::New(fs, (IBufferedOutputStream**)&bos);
    CDataOutputStream::New(bos, (IDataOutputStream**)&_out);
    assert(_out != NULL);

    AutoPtr<IDataOutput> out = IDataOutput::Probe(_out);
    out->WriteInt32(IPCONFIG_FILE_VERSION);

    for (Int32 i = 0; i < networks->GetLength(); i++) {
        AutoPtr<IWifiConfiguration> config = (*networks)[i];
        Boolean writeToFile = FALSE;

        // try {
        AutoPtr<ILinkProperties> linkProperties;
        config->GetLinkProperties((ILinkProperties**)&linkProperties);
        IpAssignment ipAssignment;
        config->GetIpAssignment(&ipAssignment);
        switch (ipAssignment) {
            case IpAssignment_STATIC: {
                out->WriteUTF(IP_ASSIGNMENT_KEY);
                out->WriteInt32(ipAssignment);

                AutoPtr<IObjectContainer> linkAddresses;
                linkProperties->GetLinkAddresses((IObjectContainer**)&linkAddresses);
                if (linkAddresses != NULL) {
                    AutoPtr<IObjectEnumerator> ator;
                    linkAddresses->GetObjectEnumerator((IObjectEnumerator**)&ator);
                    Boolean hasNext = FALSE;
                    while (ator->MoveNext(&hasNext), hasNext) {
                        out->WriteUTF(LINK_ADDRESS_KEY);
                        AutoPtr<IInterface> obj;
                        ator->Current((IInterface**)&obj);
                        AutoPtr<ILinkAddress> linkAddr = ILinkAddress::Probe(obj);
                        AutoPtr<IInetAddress> addr;
                        linkAddr->GetAddress((IInetAddress**)&addr);
                        String hostAddress;
                        addr->GetHostAddress(&hostAddress);
                        out->WriteUTF(hostAddress);
                        Int32 len = 0;
                        linkAddr->GetNetworkPrefixLength(&len);
                        out->WriteInt32(len);
                    }
                }

                AutoPtr<IObjectContainer> routes;
                linkProperties->GetRoutes((IObjectContainer**)&routes);
                if (routes != NULL) {
                    AutoPtr<IObjectEnumerator> ator;
                    routes->GetObjectEnumerator((IObjectEnumerator**)&ator);
                    Boolean hasNext = FALSE;
                    while (ator->MoveNext(&hasNext), hasNext) {
                        out->WriteUTF(GATEWAY_KEY);
                        AutoPtr<IInterface> obj;
                        ator->Current((IInterface**)&obj);
                        AutoPtr<IRouteInfo> route = IRouteInfo::Probe(obj);
                        AutoPtr<ILinkAddress> dest;
                        route->GetDestination((ILinkAddress**)&dest);
                        if (dest != NULL) {
                            out->WriteInt32(1);

                            AutoPtr<IInetAddress> addr;
                            dest->GetAddress((IInetAddress**)&addr);
                            String hostAddress;
                            addr->GetHostAddress(&hostAddress);
                            out->WriteUTF(hostAddress);
                            Int32 len = 0;
                            dest->GetNetworkPrefixLength(&len);
                            out->WriteInt32(len);
                        } else {
                            out->WriteInt32(0);
                        }

                        AutoPtr<IInetAddress> addr;
                        route->GetGateway((IInetAddress**)&addr);
                        if (addr != NULL) {
                            out->WriteInt32(1);
                            String hostAddress;
                            addr->GetHostAddress(&hostAddress);
                            out->WriteUTF(hostAddress);
                        } else {
                            out->WriteInt32(0);
                        }
                    }
                }

                AutoPtr<IObjectContainer> dnses;
                linkProperties->GetRoutes((IObjectContainer**)&dnses);
                if (dnses != NULL) {
                    AutoPtr<IObjectEnumerator> ator;
                    dnses->GetObjectEnumerator((IObjectEnumerator**)&ator);
                    Boolean hasNext = FALSE;
                    while (ator->MoveNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> obj;
                        ator->Current((IInterface**)&obj);
                        AutoPtr<IInetAddress> inetAddr = IInetAddress::Probe(obj);
                        out->WriteUTF(DNS_KEY);
                        String hostAddress;
                        inetAddr->GetHostAddress(&hostAddress);
                        out->WriteUTF(hostAddress);
                    }
                }

                writeToFile = TRUE;
                break;
            }
            case IpAssignment_DHCP:
                out->WriteUTF(IP_ASSIGNMENT_KEY);
                out->WriteInt32(ipAssignment);
                writeToFile = TRUE;
                break;
            case IpAssignment_UNASSIGNED:
                /* Ignore */
                break;
            default:
                Logger::E(TAG, "Ignore invalid ip assignment while writing");
                break;
        }

        ProxySettings proxySettings;
        config->GetProxySettings(&proxySettings);
        switch (proxySettings) {
            case ProxySettings_STATIC: {
                AutoPtr<IProxyProperties> proxyProperties;
                linkProperties->GetHttpProxy((IProxyProperties**)&proxyProperties);
                String exclusionList;
                proxyProperties->GetExclusionList(&exclusionList);
                out->WriteUTF(PROXY_SETTINGS_KEY);
                out->WriteInt32(proxySettings);
                out->WriteUTF(PROXY_HOST_KEY);
                String host;
                proxyProperties->GetHost(&host);
                out->WriteUTF(host);
                out->WriteUTF(PROXY_PORT_KEY);
                Int32 port;
                proxyProperties->GetPort(&port);
                out->WriteInt32(port);
                out->WriteUTF(EXCLUSION_LIST_KEY);
                out->WriteUTF(exclusionList);
                writeToFile = TRUE;
                break;
            }
            case ProxySettings_NONE:
                out->WriteUTF(PROXY_SETTINGS_KEY);
                out->WriteInt32(proxySettings);
                writeToFile = TRUE;
                break;
            case ProxySettings_UNASSIGNED:
                /* Ignore */
                break;
            default:
                Logger::E(TAG, "Ignthisore invalid proxy settings while writing");
                break;
        }
        if (writeToFile) {
            out->WriteUTF(ID_KEY);
            out->WriteInt32(ConfigKey(config));
        }
        // } catch (NullPointerException e) {
        //     loge("Failure in writing " + config.linkProperties + e);
        // }
        out->WriteUTF(EOS);
    }

    // } catch (IOException e) {
    //     loge("Error writing data file");
    // } finally {
    if (_out != NULL) {
        // try {
        _out->Close();
        // } catch (Exception e) {}
    }

    //Quit if no more writes sent
    {
        AutoLock lock(sLock);
        if (--sWriteSequence == 0) {
            AutoPtr<ILooper> looper;
            sDiskWriteHandler->GetLooper((ILooper**)&looper);
            looper->Quit();
            sDiskWriteHandler = NULL;
            sDiskWriteHandlerThread = NULL;
        }
    }
}


//==============================================================
// WifiConfigStore
//==============================================================
String WifiConfigStore::TAG("WifiConfigStore");
const Boolean WifiConfigStore::DBG = FALSE;
String WifiConfigStore::mIpConfigFile = /*Environment::GetDataDirectory()*/String("/data/misc/wifi/ipconfig.txt");
const Int32 WifiConfigStore::IPCONFIG_FILE_VERSION = 2;

/* IP and proxy configuration keys */
String WifiConfigStore::ID_KEY("id");
String WifiConfigStore::IP_ASSIGNMENT_KEY("ipAssignment");
String WifiConfigStore::LINK_ADDRESS_KEY("linkAddress");
String WifiConfigStore::GATEWAY_KEY("gateway");
String WifiConfigStore::DNS_KEY("dns");
String WifiConfigStore::PROXY_SETTINGS_KEY("proxySettings");
String WifiConfigStore::PROXY_HOST_KEY("proxyHost");
String WifiConfigStore::PROXY_PORT_KEY("proxyPort");
String WifiConfigStore::EXCLUSION_LIST_KEY("exclusionList");
String WifiConfigStore::EOS("eos");

WifiConfigStore::WifiConfigStore(
    /* [in] */ IContext* c,
    /* [in] */ WifiNative* wn)
    : mContext(c)
    , mLastPriority(-1)
    , mWifiNative(wn)
{}

void WifiConfigStore::Initialize()
{
    // if (DBG) log("Loading config and enabling all networks");
    LoadConfiguredNetworks();
    EnableAllNetworks();
}

AutoPtr<IObjectContainer> WifiConfigStore::GetConfiguredNetworks()
{
    AutoPtr<IObjectContainer> networks;
    assert(0);
    // TODO
    // CParcelableObjectContainer::New((IObjectContainer**)&networks);
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator ator = mConfiguredNetworks.Begin();
    for (; ator != mConfiguredNetworks.End(); ++ator) {
        AutoPtr<IWifiConfiguration> newCig;
        CWifiConfiguration::New(ator->mSecond, (IWifiConfiguration**) &newCig);
        networks->Add(newCig);
    }

    return networks;
}

void WifiConfigStore::EnableAllNetworks()
{
    Boolean networkEnabledStateChanged = FALSE;
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it;
    for (it = mConfiguredNetworks.Begin(); it != mConfiguredNetworks.End(); ++it) {
        AutoPtr<IWifiConfiguration> config = it->mSecond;
        Int32 status;
        if(config != NULL && (config->GetStatus(&status), status == IWifiConfigurationStatus::DISABLED)) {
            Int32 networkId;
            config->GetNetworkId(&networkId);
            if(mWifiNative->EnableNetwork(networkId, FALSE)) {
                networkEnabledStateChanged = TRUE;
                config->SetStatus(IWifiConfigurationStatus::ENABLED);
            }
            else {
                Loge(String("Enable network failed on ") + StringUtils::Int32ToString(networkId));
            }
        }
    }

    if (networkEnabledStateChanged) {
        mWifiNative->SaveConfig();
        SendConfiguredNetworksChangedBroadcast();
    }
}

Boolean WifiConfigStore::SelectNetwork(
    /* [in] */ Int32 netId)
{
    if (netId == IWifiConfiguration::INVALID_NETWORK_ID) return FALSE;

    // Reset the priority of each network at start or if it goes too high.
    if (mLastPriority == -1 || mLastPriority > 1000000) {
        HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it;
        for (it = mConfiguredNetworks.Begin(); it != mConfiguredNetworks.End(); ++it) {
            AutoPtr<IWifiConfiguration> config = it->mSecond;
            Int32 networkId;
            config->GetNetworkId(&networkId);
            if (networkId != IWifiConfiguration::INVALID_NETWORK_ID) {
                config->SetPriority(0);
                AddOrUpdateNetworkNative(config);
            }
        }
        mLastPriority = 0;
    }

    // Set to the highest priority and save the configuration.
    AutoPtr<IWifiConfiguration> config;
    CWifiConfiguration::New((IWifiConfiguration**)&config);
    config->SetNetworkId(netId);
    config->SetPriority(++mLastPriority);

    AddOrUpdateNetworkNative(config);
    mWifiNative->SaveConfig();

    /* Enable the given network while disabling all other networks */
    EnableNetworkWithoutBroadcast(netId, TRUE);

   /* Avoid saving the config & sending a broadcast to prevent settings
    * from displaying a disabled list of networks */
    return TRUE;
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::SaveNetwork(
    /* [in] */ IWifiConfiguration* config)
{
    // A new network cannot have NULL SSID
    if (config == NULL) {
        return new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    Int32 networkId;
    config->GetNetworkId(&networkId);
    String ssid;
    config->GetSSID(&ssid);
    if (networkId == IWifiConfiguration::INVALID_NETWORK_ID && ssid.IsNull()) {
        return new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    Boolean newNetwork = (networkId== IWifiConfiguration::INVALID_NETWORK_ID);
    AutoPtr<NetworkUpdateResult> result = AddOrUpdateNetworkNative(config);
    Int32 netId = result->GetNetworkId();
    /* enable a new network */
    if (newNetwork && netId != IWifiConfiguration::INVALID_NETWORK_ID) {
        mWifiNative->EnableNetwork(netId, FALSE);
        mConfiguredNetworks[netId]->SetStatus(IWifiConfigurationStatus::ENABLED);
    }

    mWifiNative->SaveConfig();
    SendConfiguredNetworksChangedBroadcast(config, result->IsNewNetwork() ?
        IWifiManager::CHANGE_REASON_ADDED : IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    return result;
}

void WifiConfigStore::UpdateStatus(
    /* [in] */ Int32 netId,
    /* [in] */ NetworkInfoDetailedState state)
{
    if (netId != IWifiConfiguration::INVALID_NETWORK_ID) {
        HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
        if (it == mConfiguredNetworks.End()) return;
        AutoPtr<IWifiConfiguration> config = it->mSecond;
        switch (state) {
            case NetworkInfoDetailedState_CONNECTED:
                config->SetStatus(IWifiConfigurationStatus::CURRENT);
                break;
            case NetworkInfoDetailedState_DISCONNECTED: {
                //If network is already disabled, keep the status
                Int32 status;
                config->GetStatus(&status);
                if (status == IWifiConfigurationStatus::CURRENT) {
                    config->SetStatus(IWifiConfigurationStatus::ENABLED);
                }
                break;
            }
            default:
                //do nothing, retain the existing state
                break;
        }
    }
}

Boolean WifiConfigStore::ForgetNetwork(
    /* [in] */ Int32 netId)
{
    if (mWifiNative->RemoveNetwork(netId)) {
        mWifiNative->SaveConfig();
        AutoPtr<IWifiConfiguration> target;
        AutoPtr<IWifiConfiguration> config;
        HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
        if (it != mConfiguredNetworks.End()) {
            config = it->mSecond;
        }
        if (config != NULL) {
            target = it->mSecond;
            mConfiguredNetworks.Erase(it);
            mNetworkIds.Erase(ConfigKey(config));
        }
        if (target != NULL) {
            WriteIpAndProxyConfigurations();
            SendConfiguredNetworksChangedBroadcast(target, IWifiManager::CHANGE_REASON_REMOVED);
        }
        return TRUE;
    }
    else {
        Loge(String("Failed to remove network ") + StringUtils::Int32ToString(netId));
        return FALSE;
    }
}

Int32 WifiConfigStore::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config)
{
    AutoPtr<NetworkUpdateResult> result = AddOrUpdateNetworkNative(config);
    Int32 netId = result->GetNetworkId();
    if (netId != IWifiConfiguration::INVALID_NETWORK_ID) {
        AutoPtr<IWifiConfiguration> c;
        HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator ator = mConfiguredNetworks.Find(netId);
        if (ator != mConfiguredNetworks.End()) {
            c = ator->mSecond;
        }

        SendConfiguredNetworksChangedBroadcast(c,
                result->IsNewNetwork() ? IWifiManager::CHANGE_REASON_ADDED :
                    IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    }
    return netId;
}

Boolean WifiConfigStore::RemoveNetwork(
    /* [in] */ Int32 netId)
{
    Boolean ret = mWifiNative->RemoveNetwork(netId);
    AutoPtr<IWifiConfiguration> config;
    if (ret) {
        HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
        if (it != mConfiguredNetworks.End()) {
            config = it->mSecond;
        }
        if (config != NULL) {
            mConfiguredNetworks.Erase(it);
            mNetworkIds.Erase(ConfigKey(config));
        }
    }
    if (config != NULL) {
        SendConfiguredNetworksChangedBroadcast(config, IWifiManager::CHANGE_REASON_REMOVED);
    }
    return ret;
}

Boolean WifiConfigStore::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers)
{
    Boolean ret = EnableNetworkWithoutBroadcast(netId, disableOthers);
    if (disableOthers) {
        SendConfiguredNetworksChangedBroadcast();
    }
    else {
        AutoPtr<IWifiConfiguration> enabledNetwork;
        {
            AutoLock lock(mConfiguredNetworksLock);
            HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
            if (it != mConfiguredNetworks.End()) {
                enabledNetwork = it->mSecond;
            }
        }
        // check just in case the network was removed by someone else.
        if (enabledNetwork != NULL) {
            SendConfiguredNetworksChangedBroadcast(enabledNetwork,
                IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
        }
    }
    return ret;
}

Boolean WifiConfigStore::EnableNetworkWithoutBroadcast(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers)
{
    Boolean ret = mWifiNative->EnableNetwork(netId, disableOthers);

    AutoPtr<IWifiConfiguration> config;
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
    if (it != mConfiguredNetworks.End()) {
        config = it->mSecond;
    }
    if (config != NULL) config->SetStatus(IWifiConfigurationStatus::ENABLED);

    if (disableOthers) {
        MarkAllNetworksDisabledExcept(netId);
    }
    return ret;
}

Boolean WifiConfigStore::DisableNetwork(
    /* [in] */ Int32 netId)
{
    return DisableNetwork(netId, IWifiConfiguration::DISABLED_UNKNOWN_REASON);
}

Boolean WifiConfigStore::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Int32 reason)
{
    Boolean ret = mWifiNative->DisableNetwork(netId);
    AutoPtr<IWifiConfiguration> network;
    AutoPtr<IWifiConfiguration> config;
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
    if (it != mConfiguredNetworks.End()) {
        config = it->mSecond;
    }
    Int32 status;
    /* Only change the reason if the network was not previously disabled */
    if (config != NULL && (config->GetStatus(&status), status != IWifiConfigurationStatus::DISABLED)) {
        config->SetStatus(IWifiConfigurationStatus::DISABLED);
        config->SetDisableReason(reason);
        network = config;
    }
    if (network != NULL) {
        SendConfiguredNetworksChangedBroadcast(network,
                IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    }
    return ret;
}

Boolean WifiConfigStore::SaveConfig()
{
    return mWifiNative->SaveConfig();
}

AutoPtr<IWpsResult> WifiConfigStore::StartWpsWithPinFromAccessPoint(
    /* [in] */ IWpsInfo* config)
{
    AutoPtr<IWpsResult> result;
    CWpsResult::New((IWpsResult**)&result);
    String bssid, pin;
    config->GetBSSID(&bssid);
    config->GetPin(&pin);
    if (mWifiNative->StartWpsRegistrar(bssid, pin)) {
        /* WPS leaves all networks disabled */
        MarkAllNetworksDisabled();
        result->SetStatus(WpsResultStatus_SUCCESS);
    }
    else {
        Loge(String("Failed to start WPS pin method configuration"));
        result->SetStatus(WpsResultStatus_FAILURE);
    }
    return result;
}

AutoPtr<IWpsResult> WifiConfigStore::StartWpsWithPinFromDevice(
    /* [in] */ IWpsInfo* config)
{
    AutoPtr<IWpsResult> result;
    CWpsResult::New((IWpsResult**)&result);
    String bssid;
    config->GetBSSID(&bssid);
    String pin = mWifiNative->StartWpsPinDisplay(bssid);
    result->SetPin(pin);
    /* WPS leaves all networks disabled */
    if (!pin.IsNullOrEmpty()) {
        MarkAllNetworksDisabled();
        result->SetStatus(WpsResultStatus_SUCCESS);
    }
    else {
        Loge(String("Failed to start WPS pin method configuration"));
        result->SetStatus(WpsResultStatus_FAILURE);
    }
    return result;
}

AutoPtr<IWpsResult> WifiConfigStore::StartWpsPbc(
    /* [in] */ IWpsInfo* config)
{
    AutoPtr<IWpsResult> result;
    CWpsResult::New((IWpsResult**)&result);
    String bssid;
    config->GetBSSID(&bssid);
    if (mWifiNative->StartWpsPbc(bssid)) {
        /* WPS leaves all networks disabled */
        MarkAllNetworksDisabled();
        result->SetStatus(WpsResultStatus_SUCCESS);
    }
    else {
        Loge(String("Failed to start WPS push button configuration"));
        result->SetStatus(WpsResultStatus_FAILURE);
    }
    return result;
}

AutoPtr<ILinkProperties> WifiConfigStore::GetLinkProperties(
    /* [in] */ Int32 netId)
{
    AutoPtr<IWifiConfiguration> config;
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
    if (it != mConfiguredNetworks.End()) {
        config = it->mSecond;
    }
    if (config != NULL) {
        AutoPtr<ILinkProperties> lp, result;
        config->GetLinkProperties((ILinkProperties**)&lp);
        CLinkProperties::New(lp, (ILinkProperties**)&result);
        return result;
    }
    return NULL;
}

// AutoPtr<DhcpInfoInternal> WifiConfigStore::GetIpConfiguration(
//     /* [in] */ Int32 netId)
// {
//     AutoPtr<DhcpInfoInternal> dhcpInfoInternal = new DhcpInfoInternal();
//     AutoPtr<ILinkProperties> linkProperties = GetLinkProperties(netId);

//     if (linkProperties != NULL) {
//         AutoPtr<IObjectContainer> addresses;
//         linkProperties->GetLinkAddresses((IObjectContainer**)&addresses);
//         if (addresses != NULL) {
//             AutoPtr<IObjectEnumerator> enumerator;
//             addresses->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
//             Boolean hasNext = FALSE;
//             if (enumerator->MoveNext(&hasNext), hasNext) {
//                 AutoPtr<IInterface> obj;
//                 enumerator->Current((IInterface**)&obj);
//                 ILinkAddress* linkAddress = ILinkAddress::Probe(obj);
//                 assert(linkAddress != NULL);

//                 AutoPtr<IInetAddress> ia;
//                 linkAddress->GetAddress((IInetAddress**)&ia);
//                 ia->GetHostAddress(&dhcpInfoInternal->mIpAddress);

//                 AutoPtr<IObjectContainer> routes;
//                 linkProperties->GetRoutes((IObjectContainer**)&routes);
//                 AutoPtr<IObjectEnumerator> emu;
//                 routes->GetObjectEnumerator((IObjectEnumerator**)&emu);
//                 while (emu->MoveNext(&hasNext), hasNext) {
//                     AutoPtr<IInterface> obj;
//                     emu->Current((IInterface**)&obj);
//                     IRouteInfo* route = IRouteInfo::Probe(obj);
//                     dhcpInfoInternal->AddRoute(route);
//                 }

//                 linkAddress->GetNetworkPrefixLength(&dhcpInfoInternal->mPrefixLength);

//                 AutoPtr<IObjectContainer> dnses;
//                 linkProperties->GetDnses((IObjectContainer**)&dnses);
//                 AutoPtr<IObjectEnumerator> emuDns;
//                 dnses->GetObjectEnumerator((IObjectEnumerator**)&emuDns);
//                 if (emuDns->MoveNext(&hasNext), hasNext) {
//                     AutoPtr<IInterface> obj;
//                     emu->Current((IInterface**)&obj);
//                     IInetAddress* addr = IInetAddress::Probe(obj);
//                     addr->GetHostAddress(&dhcpInfoInternal->mDns1);
//                     if (emuDns->MoveNext(&hasNext), hasNext) {
//                         obj = NULL;
//                         emu->Current((IInterface**)&obj);
//                         addr = IInetAddress::Probe(obj);
//                         addr->GetHostAddress(&dhcpInfoInternal->mDns2);
//                     }
//                 }
//             }
//         }
//     }

//     return dhcpInfoInternal;
// }

// void WifiConfigStore::SetIpConfiguration(
//     /* [in] */ Int32 netId,
//     /* [in] */ DhcpInfoInternal* dhcpInfo)
// {
//     AutoPtr<ILinkProperties> linkProperties = dhcpInfo->MakeLinkProperties();

//     AutoPtr<IWifiConfiguration> config;
//     HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
//     if (it != mConfiguredNetworks.End()) {
//         config = it->mSecond;
//     }
//     if (config != NULL) {
//         AutoPtr<ILinkProperties> lp;
//         config->GetLinkProperties((ILinkProperties**)&lp);
//         // add old proxy details
//         if(lp != NULL) {
//             AutoPtr<IProxyProperties> proxy;
//             lp->GetHttpProxy((IProxyProperties**)&proxy);
//             linkProperties->SetHttpProxy(proxy);
//         }
//         config->SetLinkProperties(linkProperties);
//     }
// }

void WifiConfigStore::ClearIpConfiguration(
    /* [in] */ Int32 netId)
{
    AutoPtr<IWifiConfiguration> config;
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
    if (it != mConfiguredNetworks.End()) {
        config = it->mSecond;
    }
    AutoPtr<ILinkProperties> lp;
    if (config != NULL && (config->GetLinkProperties((ILinkProperties**)&lp), lp != NULL)) {
        // Clear everything except proxy
        AutoPtr<IProxyProperties> proxy;
        lp->GetHttpProxy((IProxyProperties**)&proxy);
        lp->Clear();
        lp->SetHttpProxy(proxy);
    }
}

AutoPtr<IProxyProperties> WifiConfigStore::GetProxyProperties(
    /* [in] */ Int32 netId)
{
    AutoPtr<ILinkProperties> linkProperties = GetLinkProperties(netId);
    if (linkProperties != NULL) {
        AutoPtr<IProxyProperties> proxy, result;
        linkProperties->GetHttpProxy((IProxyProperties**)&proxy);
        assert(0);
        // TODO
        // CProxyProperties::New(proxy, (IProxyProperties**)&result);
        // return result;
        return NULL;
    }
    return NULL;
}

Boolean WifiConfigStore::IsUsingStaticIp(
    /* [in] */ Int32 netId)
{
    AutoPtr<IWifiConfiguration> config;
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Find(netId);
    if (it != mConfiguredNetworks.End()) {
        config = it->mSecond;
    }
    IpAssignment ass;
    if (config != NULL && (config->GetIpAssignment(&ass), ass == IpAssignment_STATIC)) {
        return TRUE;
    }
    return FALSE;
}

void WifiConfigStore::SendConfiguredNetworksChangedBroadcast(
    /* [in] */ IWifiConfiguration* network,
    /* [in] */ Int32 reason)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::CONFIGURED_NETWORKS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutBooleanExtra(IWifiManager::EXTRA_MULTIPLE_NETWORKS_CHANGED, FALSE);
    intent->PutExtra(IWifiManager::EXTRA_WIFI_CONFIGURATION, IParcelable::Probe(network));
    intent->PutExtra(IWifiManager::EXTRA_CHANGE_REASON, reason);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    AutoPtr<IUserHandle> uh;
    helper->GetALL((IUserHandle**)&uh);
    mContext->SendBroadcastAsUser(intent, uh);
}

void WifiConfigStore::SendConfiguredNetworksChangedBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiManager::CONFIGURED_NETWORKS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutBooleanExtra(IWifiManager::EXTRA_MULTIPLE_NETWORKS_CHANGED, TRUE);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    AutoPtr<IUserHandle> uh;
    helper->GetALL((IUserHandle**)&uh);
    mContext->SendBroadcastAsUser(intent, uh);
}

void WifiConfigStore::LoadConfiguredNetworks()
{
    String listStr = mWifiNative->ListNetworks();
    mLastPriority = 0;

    mConfiguredNetworks.Clear();
    mNetworkIds.Clear();

    if (listStr.IsNull()) {
        return;
    }

    AutoPtr<ArrayOf<String> > lines;
    StringUtils::Split(listStr, "\n", (ArrayOf<String>**)&lines);

    if (lines != NULL) {
        Int32 networkId, status, priority;
        // Skip the first line, which is a header
        for (Int32 i = 1; i < lines->GetLength(); ++i) {
            AutoPtr<ArrayOf<String> > result;
            StringUtils::Split((*lines)[i], "\t", (ArrayOf<String>**)&result);
            assert(result != NULL);

            // network-id | ssid | bssid | flags
            if (FAILED(StringUtils::ParseInt32((*result)[0], 10, &networkId))) {
                continue;
            }

            AutoPtr<IWifiConfiguration> config;
            CWifiConfiguration::New((IWifiConfiguration**)&config);
            config->SetNetworkId(networkId);

            if (result->GetLength() > 3) {
                if ((*result)[3].IndexOf("[CURRENT]") != -1)
                    status = IWifiConfigurationStatus::CURRENT;
                else if ((*result)[3].IndexOf("[DISABLED]") != -1)
                    status = IWifiConfigurationStatus::DISABLED;
                else
                    status = IWifiConfigurationStatus::ENABLED;
            }
            else {
                status = IWifiConfigurationStatus::ENABLED;
            }

            config->SetStatus(status);
            ReadNetworkVariables(config);
            config->GetPriority(&priority);
            if (priority > mLastPriority) {
                mLastPriority = priority;
            }

            config->GetNetworkId(&networkId);
            mConfiguredNetworks[networkId] = config;
            mNetworkIds[ConfigKey(config)] = networkId;
        }
    }

    ReadIpAndProxyConfigurations();
    SendConfiguredNetworksChangedBroadcast();
}

void WifiConfigStore::MarkAllNetworksDisabledExcept(
    /* [in] */ Int32 netId)
{
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it = mConfiguredNetworks.Begin();

    for (it = mConfiguredNetworks.Begin(); it != mConfiguredNetworks.End(); ++it) {
        AutoPtr<IWifiConfiguration> config = it->mSecond;
        Int32 networkId;
        if(config != NULL && (config->GetNetworkId(&networkId), networkId != netId)) {
            Int32 status;
            config->GetStatus(&status);
            if (status != IWifiConfigurationStatus::DISABLED) {
                config->SetStatus(IWifiConfigurationStatus::DISABLED);
                config->SetDisableReason(IWifiConfiguration::DISABLED_UNKNOWN_REASON);
            }
        }
    }
}

void WifiConfigStore::MarkAllNetworksDisabled()
{
    MarkAllNetworksDisabledExcept(IWifiConfiguration::INVALID_NETWORK_ID);
}

void WifiConfigStore::WriteIpAndProxyConfigurations()
{
    /* Make a copy */
    AutoPtr< ArrayOf<IWifiConfiguration*> > networks = ArrayOf<IWifiConfiguration*>::Alloc(mConfiguredNetworks.GetSize());
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator it;
    Int32 index;
    for (it = mConfiguredNetworks.Begin(), index = 0; it != mConfiguredNetworks.End(); ++it, ++index) {
        AutoPtr<IWifiConfiguration> config = it->mSecond;
        AutoPtr<IWifiConfiguration> newConfig;
        CWifiConfiguration::New(config, (IWifiConfiguration**)&newConfig);
        networks->Set(index, newConfig);
    }

    DelayedDiskWrite::Write(networks);
}

void WifiConfigStore::ReadIpAndProxyConfigurations()
{
    AutoPtr<IDataInputStream> _in;
    AutoPtr<IFileInputStream> fs;
    AutoPtr<IBufferedInputStream> bis;
    AutoPtr<IDataInput> in;
    Int32 version = 0;
    ECode ec = NOERROR;
    FAIL_GOTO(CFileInputStream::New(mIpConfigFile, (IFileInputStream**)&fs), ParseError);
    FAIL_GOTO(CBufferedInputStream::New(fs, (IBufferedInputStream**)&bis), ParseError);
    FAIL_GOTO(CDataInputStream::New(bis, (IDataInputStream**)&_in), ParseError);

    in = IDataInput::Probe(_in);
    FAIL_GOTO(in->ReadInt32(&version), ParseError);
    if (version != 2 && version != 1) {
        Logger::E(TAG, "Bad version on IP configuration file, ignore read");
        if (_in != NULL) {
            _in->Close();
        }
        return;
    }

    while (TRUE) {
        Int32 id = -1;
        IpAssignment ipAssignment = IpAssignment_UNASSIGNED;
        ProxySettings proxySettings = ProxySettings_UNASSIGNED;
        AutoPtr<ILinkProperties> linkProperties;
        CLinkProperties::New((ILinkProperties**)&linkProperties);
        String proxyHost;
        Int32 proxyPort = -1;
        String exclusionList;
        String key;

        do {
            FAIL_GOTO(in->ReadUTF(&key), ParseError);
            if (key.Equals(ID_KEY)) {
                FAIL_GOTO(in->ReadInt32(&id), ParseError2);
            } else if (key.Equals(IP_ASSIGNMENT_KEY)) {
                FAIL_GOTO(in->ReadInt32(&ipAssignment), ParseError2);
            } else if (key.Equals(LINK_ADDRESS_KEY)) {
                AutoPtr<ILinkAddress> linkAddr;
                AutoPtr<IInetAddress> ia;
                String v;
                FAIL_GOTO(in->ReadUTF(&v), ParseError2);
                Int32 len = 0;
                FAIL_GOTO(in->ReadInt32(&len), ParseError2);
                NetworkUtils::NumericToInetAddress(v, (IInetAddress**)&ia);
                CLinkAddress::New(ia, len, (ILinkAddress**)&linkAddr);
                linkProperties->AddLinkAddress(linkAddr);
            } else if (key.Equals(GATEWAY_KEY)) {
                AutoPtr<ILinkAddress> dest;
                AutoPtr<IInetAddress> gateway;
                if (version == 1) {
                    // only supported default gateways - leave the dest/prefix empty
                    String v;
                    FAIL_GOTO(in->ReadUTF(&v), ParseError2);
                    NetworkUtils::NumericToInetAddress(v, (IInetAddress**)&gateway);
                } else {
                    Int32 v = 0;
                    FAIL_GOTO(in->ReadInt32(&v), ParseError2);
                    if (v == 1) {
                        String str;
                        FAIL_GOTO(in->ReadUTF(&str), ParseError2);
                        AutoPtr<IInetAddress> addr;
                        NetworkUtils::NumericToInetAddress(str, (IInetAddress**)&addr);
                        FAIL_GOTO(in->ReadInt32(&v), ParseError2);
                        CLinkAddress::New(addr, v, (ILinkAddress**)&dest);
                    }

                    FAIL_GOTO(in->ReadInt32(&v), ParseError2);
                    if (v == 1) {
                        String str;
                        FAIL_GOTO(in->ReadUTF(&str), ParseError2);
                        NetworkUtils::NumericToInetAddress(str, (IInetAddress**)&gateway);
                    }
                }
                AutoPtr<IRouteInfo> info;
                CRouteInfo::New(dest, gateway, (IRouteInfo**)&info);
                linkProperties->AddRoute(info);
            } else if (key.Equals(DNS_KEY)) {
                String v;
                FAIL_GOTO(in->ReadUTF(&v), ParseError2);
                AutoPtr<IInetAddress> dns;
                NetworkUtils::NumericToInetAddress(v, (IInetAddress**)&dns);
                linkProperties->AddDns(dns);
            } else if (key.Equals(PROXY_SETTINGS_KEY)) {
                FAIL_GOTO(in->ReadInt32(&proxySettings), ParseError2);
            } else if (key.Equals(PROXY_HOST_KEY)) {
                FAIL_GOTO(in->ReadUTF(&proxyHost), ParseError2);
            } else if (key.Equals(PROXY_PORT_KEY)) {
                FAIL_GOTO(in->ReadInt32(&proxyPort), ParseError2);
            } else if (key.Equals(EXCLUSION_LIST_KEY)) {
                FAIL_GOTO(in->ReadUTF(&exclusionList), ParseError2);
            } else if (key.Equals(EOS)) {
                break;
            } else {
                Logger::E(TAG, "Ignore unknown key %s while reading", key.string());
            }

        ParseError2:
            if (FAILED(ec)) {
                Logger::E(TAG, "Ignore invalid address while reading");
            }
        } while (TRUE);

        if (id != -1) {
            AutoPtr<IWifiConfiguration> config;
            HashMap<Int32, Int32>::Iterator netAtor = mNetworkIds.Find(id);
            if (netAtor != mNetworkIds.End()) {
                HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator ator =
                        mConfiguredNetworks.Find(netAtor->mSecond);
                if (ator != mConfiguredNetworks.End()) {
                    config = ator->mSecond;
                }
            }

            if (config == NULL) {
                Logger::E(TAG, "configuration found for missing network, ignored");
            } else {
                config->SetLinkProperties(linkProperties);
                switch (ipAssignment) {
                    case IpAssignment_STATIC:
                    case IpAssignment_DHCP:
                        config->SetIpAssignment(ipAssignment);
                        break;
                    case IpAssignment_UNASSIGNED:
                        //Ignore
                        break;
                    default:
                        Logger::E(TAG, "Ignore invalid ip assignment while reading");
                        break;
                }

                switch (proxySettings) {
                    case ProxySettings_STATIC: {
                        config->SetProxySettings(proxySettings);
                        AutoPtr<IProxyProperties> proxyProperties;
                        assert(0);
                        // TODO
                        // CProxyProperties::New(proxyHost, proxyPort, exclusionList,
                        //         (IProxyProperties**)&proxyProperties);

                        linkProperties->SetHttpProxy(proxyProperties);
                        break;
                    }
                    case ProxySettings_NONE:
                        config->SetProxySettings(proxySettings);
                        break;
                    case ProxySettings_UNASSIGNED:
                        //Ignore
                        break;
                    default:
                        Logger::E(TAG, "Ignore invalid proxy settings while reading");
                        break;
                }
            }
        } else {
            if (DBG) Logger::E(TAG, "Missing id while parsing configuration");
        }
    }

ParseError:
    if (FAILED(ec)) {
        Logger::E(TAG, "Error parsing configuration");
    }

    if (_in != NULL) {
        _in->Close();
    }
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::AddOrUpdateNetworkNative(
    /* [in] */ IWifiConfiguration* config)
{
    /*
     * If the supplied networkId is INVALID_NETWORK_ID, we create a new empty
     * network configuration. Otherwise, the networkId should
     * refer to an existing configuration.
     */
    Int32 netId;
    config->GetNetworkId(&netId);
    Boolean newNetwork = FALSE;
    // networkId of INVALID_NETWORK_ID means we want to create a new network
    if (netId == IWifiConfiguration::INVALID_NETWORK_ID) {
        HashMap<Int32, Int32>::Iterator it = mNetworkIds.Find(ConfigKey(config));
        if (it != mNetworkIds.End()) {
            netId = it->mSecond;
        }
        else {
            newNetwork = TRUE;
            netId = mWifiNative->AddNetwork();
            if (netId < 0) {
                Loge(String("Failed to add a network!"));
                return new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
            }
        }
    }

    Boolean updateFailed = TRUE;

    {
        String SSID;
        if ((config->GetSSID(&SSID), !SSID.IsNull()) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::ssidVarName,
                    SSID)) {
            Loge(String("failed to set SSID: ") + SSID);
            goto setVariables;
        }

        String BSSID;
        if ((config->GetBSSID(&BSSID), !BSSID.IsNull()) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::bssidVarName,
                    BSSID)) {
            Loge(String("failed to set BSSID: ") + BSSID);
            goto setVariables;
        }

        AutoPtr<IBitSet> allowedKeyManagement;
        config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
        AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
        AutoPtr< ArrayOf<String> > keyMgmtStrings;
        keyMgmt->GetStrings((ArrayOf<String>**)&keyMgmtStrings);
        String allowedKeyManagementString =
                MakeString(allowedKeyManagement, keyMgmtStrings);
        Int32 number;
        if ((allowedKeyManagement->Cardinality(&number), number != 0) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationKeyMgmt::varName,
                    allowedKeyManagementString)) {
            Loge(String("failed to set key_mgmt: ") +
                    allowedKeyManagementString);
            goto setVariables;
        }

        AutoPtr<IBitSet> allowedProtocols;
        config->GetAllowedProtocols((IBitSet**)&allowedProtocols);
        AutoPtr<IWifiConfigurationProtocol> protocol;
        CWifiConfigurationProtocol::AcquireSingleton((IWifiConfigurationProtocol**)&protocol);
        AutoPtr< ArrayOf<String> > protocolStrings;
        protocol->GetStrings((ArrayOf<String>**)&protocolStrings);
        String allowedProtocolsString =
                MakeString(allowedProtocols, protocolStrings);
        if ((allowedProtocols->Cardinality(&number), number != 0) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationProtocol::varName,
                    allowedProtocolsString)) {
            Loge(String("failed to set proto: ") +
                    allowedProtocolsString);
            goto setVariables;
        }

        AutoPtr<IBitSet> allowedAuthAlgorithms;
        config->GetAllowedAuthAlgorithms((IBitSet**)&allowedAuthAlgorithms);
        AutoPtr<IWifiConfigurationAuthAlgorithm> authAlgorithm;
        CWifiConfigurationAuthAlgorithm::AcquireSingleton((IWifiConfigurationAuthAlgorithm**)&authAlgorithm);
        AutoPtr< ArrayOf<String> > authAlgorithmStrings;
        authAlgorithm->GetStrings((ArrayOf<String>**)&authAlgorithmStrings);
        String allowedAuthAlgorithmsString =
                MakeString(allowedAuthAlgorithms, authAlgorithmStrings);
        if ((allowedAuthAlgorithms->Cardinality(&number), number != 0) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationAuthAlgorithm::varName,
                    allowedAuthAlgorithmsString)) {
            Loge(String("failed to set auth_alg: ") +
                    allowedAuthAlgorithmsString);
            goto setVariables;
        }

        AutoPtr<IBitSet> allowedPairwiseCiphers;
        config->GetAllowedPairwiseCiphers((IBitSet**)&allowedPairwiseCiphers);
        AutoPtr<IWifiConfigurationPairwiseCipher> pairwiseCipher;
        CWifiConfigurationPairwiseCipher::AcquireSingleton((IWifiConfigurationPairwiseCipher**)&pairwiseCipher);
        AutoPtr< ArrayOf<String> > pairwiseCipherStrings;
        pairwiseCipher->GetStrings((ArrayOf<String>**)&pairwiseCipherStrings);
        String allowedPairwiseCiphersString =
                MakeString(allowedPairwiseCiphers,
                pairwiseCipherStrings);
        if ((allowedPairwiseCiphers->Cardinality(&number), number != 0) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationPairwiseCipher::varName,
                    allowedPairwiseCiphersString)) {
            Loge(String("failed to set pairwise: ") +
                    allowedPairwiseCiphersString);
            goto setVariables;
        }

        AutoPtr<IBitSet> allowedGroupCiphers;
        config->GetAllowedGroupCiphers((IBitSet**)&allowedGroupCiphers);
        AutoPtr<IWifiConfigurationGroupCipher> groupCipher;
        CWifiConfigurationGroupCipher::AcquireSingleton((IWifiConfigurationGroupCipher**)&groupCipher);
        AutoPtr< ArrayOf<String> > groupCipherStrings;
        groupCipher->GetStrings((ArrayOf<String>**)&groupCipherStrings);
        String allowedGroupCiphersString =
            MakeString(allowedGroupCiphers, groupCipherStrings);
        if ((allowedGroupCiphers->Cardinality(&number), number != 0) &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfigurationGroupCipher::varName,
                    allowedGroupCiphersString)) {
            Loge(String("failed to set group: ") +
                    allowedGroupCiphersString);
            goto setVariables;
        }

        // Prevent client screw-up by passing in a WifiConfiguration we gave it
        // by preventing "*" as a key.
        String preSharedKey;
        config->GetPreSharedKey(&preSharedKey);
        if (!preSharedKey.IsNull() && !preSharedKey.Equals("*") &&
                !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::pskVarName,
                    preSharedKey)) {
            Loge(String("failed to set psk"));
            goto setVariables;
        }

        Boolean hasSetKey = FALSE;
        AutoPtr< ArrayOf<String> > wepKeys;
        config->GetWepKeys((ArrayOf<String>**)&wepKeys);
        if (wepKeys != NULL) {
            AutoPtr<IWifiConfigurationHelper> helper;
            CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&helper);
            for (Int32 i = 0; i < wepKeys->GetLength(); i++) {
                // Prevent client screw-up by passing in a WifiConfiguration we gave it
                // by preventing "*" as a key.
                if (!(*wepKeys)[i].IsNull() && !(*wepKeys)[i].Equals("*")) {
                    String wepKeyVarName;
                    helper->GetWepKeyVarName(i, &wepKeyVarName);
                    if (!mWifiNative->SetNetworkVariable(
                                netId,
                                wepKeyVarName,
                                (*wepKeys)[i])) {
                        // Loge("failed to set wep_key" + i + ": " + config.wepKeys[i]);
                        goto setVariables;
                    }
                    hasSetKey = TRUE;
                }
            }
        }

        if (hasSetKey) {
            Int32 wepTxKeyIndex;
            config->GetWepTxKeyIndex(&wepTxKeyIndex);
            if (!mWifiNative->SetNetworkVariable(
                        netId,
                        IWifiConfiguration::wepTxKeyIdxVarName,
                        StringUtils::Int32ToString(wepTxKeyIndex))) {
                Loge(String("failed to set wep_tx_keyidx: ") + StringUtils::Int32ToString(wepTxKeyIndex));
                goto setVariables;
            }
        }

        Int32 priority;
        config->GetPriority(&priority);
        if (!mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::priorityVarName,
                    StringUtils::Int32ToString(priority))) {
            // loge(config.SSID + ": failed to set priority: "
            //         +config.priority);
            goto setVariables;
        }

        Boolean hiddenSSID;
        config->GetHiddenSSID(&hiddenSSID);
        if (hiddenSSID && !mWifiNative->SetNetworkVariable(
                    netId,
                    IWifiConfiguration::hiddenSSIDVarName,
                    StringUtils::Int32ToString(hiddenSSID ? 1 : 0))) {
            // loge(config.SSID + ": failed to set hiddenSSID: "+
            //         config.hiddenSSID);
            goto setVariables;
        }

        AutoPtr< ArrayOf<IWifiConfigurationEnterpriseField*> > fields;
        config->GetEnterpriseFields((ArrayOf<IWifiConfigurationEnterpriseField*>**)&fields);
        AutoPtr<IWifiConfigurationEnterpriseField> engine, eap;
        config->GetEngine((IWifiConfigurationEnterpriseField**)&engine);
        config->GetEap((IWifiConfigurationEnterpriseField**)&eap);
        for (Int32 i = 0; i < fields->GetLength(); i++) {
            AutoPtr<IWifiConfigurationEnterpriseField> field = (*fields)[i];
            String varName, value;
            field->GetVarName(&varName);
            field->GetValue(&value);
            if (!value.IsNull()) {
                if (field == engine) {
                    /*
                     * If the field is declared as an integer, it must not
                     * be NULL
                     */
                    if (value.IsEmpty()) {
                        value = "0";
                    }
                }
                else if (field != eap) {
                    value = value.IsEmpty() ? "NULL" : ConvertToQuotedString(value);
                }
                if (!mWifiNative->SetNetworkVariable(
                            netId,
                            varName,
                            value)) {
                    // loge(config.SSID + ": failed to set " + varName +
                    //         ": " + value);
                    goto setVariables;
                }
            }
        }
        updateFailed = FALSE;
    }

setVariables:
    if (updateFailed) {
        if (newNetwork) {
            mWifiNative->RemoveNetwork(netId);
            Loge(String("Failed to set a network variable, removed network: ") + StringUtils::Int32ToString(netId));
        }
        return new NetworkUpdateResult(IWifiConfiguration::INVALID_NETWORK_ID);
    }

    /* An update of the network variables requires reading them
     * back from the supplicant to update mConfiguredNetworks.
     * This is because some of the variables (SSID, wep keys &
     * passphrases) reflect different values when read back than
     * when written. For example, wep key is stored as * irrespective
     * of the value sent to the supplicant
     */
    HashMap<Int32, AutoPtr<IWifiConfiguration> >::Iterator ator = mConfiguredNetworks.Find(netId);
    AutoPtr<IWifiConfiguration> currentConfig;
    if (ator == mConfiguredNetworks.End()) {
        CWifiConfiguration::New((IWifiConfiguration**)&currentConfig);
        currentConfig->SetNetworkId(netId);
    }
    else {
        currentConfig = ator->mSecond;
    }

    ReadNetworkVariables(currentConfig);

    mConfiguredNetworks[netId] = currentConfig;
    mNetworkIds[ConfigKey(currentConfig)] = netId;

    AutoPtr<NetworkUpdateResult> result = WriteIpAndProxyConfigurationsOnChange(currentConfig, config);
    result->SetIsNewNetwork(newNetwork);
    result->SetNetworkId(netId);
    return result;
}

AutoPtr<NetworkUpdateResult> WifiConfigStore::WriteIpAndProxyConfigurationsOnChange(
    /* [in] */ IWifiConfiguration* currentConfig,
    /* [in] */ IWifiConfiguration* newConfig)
{
    Boolean ipChanged = FALSE;
    Boolean proxyChanged = FALSE;

    AutoPtr<ILinkProperties> linkProperties;
    CLinkProperties::New((ILinkProperties**)&linkProperties);

    IpAssignment ipAssignment, curIpAssignment;
    newConfig->GetIpAssignment(&ipAssignment);
    currentConfig->GetIpAssignment(&curIpAssignment);
    switch (ipAssignment) {
        case IpAssignment_STATIC: {
            AutoPtr<ILinkProperties> currentLp;
            currentConfig->GetLinkProperties((ILinkProperties**)&currentLp);
            AutoPtr<IObjectContainer> currentLinkAddresses;
            currentLp->GetLinkAddresses((IObjectContainer**)&currentLinkAddresses);

            AutoPtr<ILinkProperties> newLp;
            newConfig->GetLinkProperties((ILinkProperties**)&newLp);
            AutoPtr<IObjectContainer> newLinkAddresses;
            newLp->GetLinkAddresses((IObjectContainer**)&newLinkAddresses);

            AutoPtr<IObjectContainer> currentDnses;
            currentLp->GetDnses((IObjectContainer**)&currentDnses);
            AutoPtr<IObjectContainer> newDnses;
            newLp->GetDnses((IObjectContainer**)&newDnses);

            AutoPtr<IObjectContainer> currentRoutes;
            currentLp->GetRoutes((IObjectContainer**)&currentRoutes);
            AutoPtr<IObjectContainer> newRoutes;
            newLp->GetRoutes((IObjectContainer**)&newRoutes);

            Int32 cs = 0, ns = 0;
            Boolean containsAll = FALSE;
            currentLinkAddresses->GetObjectCount(&cs);
            newLinkAddresses->GetObjectCount(&ns);
            currentLinkAddresses->ContainsAll(newLinkAddresses, &containsAll);
            Boolean linkAddressesDiffer = (cs != ns) || !containsAll;

            currentDnses->GetObjectCount(&cs);
            newDnses->GetObjectCount(&ns);
            currentDnses->ContainsAll(newDnses, &containsAll);
            Boolean dnsesDiffer = (cs != ns) || !containsAll;


            currentRoutes->GetObjectCount(&cs);
            newRoutes->GetObjectCount(&ns);
            currentRoutes->ContainsAll(newRoutes, &containsAll);
            Boolean routesDiffer = (cs != ns) || !containsAll;

            if ((curIpAssignment != ipAssignment) ||
                    linkAddressesDiffer ||
                    dnsesDiffer ||
                    routesDiffer) {
                ipChanged = TRUE;
            }
            break;
        }
        case IpAssignment_DHCP:
            if (curIpAssignment != ipAssignment) {
                ipChanged = TRUE;
            }
            break;
        case IpAssignment_UNASSIGNED:
            /* Ignore */
            break;
        default:
            Logger::E(TAG, "Ignore invalid ip assignment during write");
            break;
    }

    ProxySettings proxySettings;
    newConfig->GetProxySettings(&proxySettings);
    switch (proxySettings) {
        case ProxySettings_STATIC: {
            AutoPtr<ILinkProperties> newLp;
            newConfig->GetLinkProperties((ILinkProperties**)&newLp);
            AutoPtr<IProxyProperties> newHttpProxy;
            newLp->GetHttpProxy((IProxyProperties**)&newHttpProxy);

            AutoPtr<ILinkProperties> currentLp;
            currentConfig->GetLinkProperties((ILinkProperties**)&currentLp);
            AutoPtr<IProxyProperties> currentHttpProxy;
            currentLp->GetHttpProxy((IProxyProperties**)&currentHttpProxy);

            if (newHttpProxy != NULL) {
                newHttpProxy->Equals(currentHttpProxy, &proxyChanged);
                proxyChanged = !proxyChanged;
            } else {
                proxyChanged = (currentHttpProxy != NULL);
            }
            break;
        }
        case ProxySettings_NONE: {
            ProxySettings currentPs;
            currentConfig->GetProxySettings(&currentPs);
            if (currentPs != proxySettings) {
                proxyChanged = TRUE;
            }
            break;
        }
        case ProxySettings_UNASSIGNED:
            /* Ignore */
            break;
        default:
            Logger::E(TAG, "Ignore invalid proxy configuration during write");
            break;
    }

    if (!ipChanged) {
        AddIpSettingsFromConfig(linkProperties, currentConfig);
    } else {
        currentConfig->SetIpAssignment(ipAssignment);
        AddIpSettingsFromConfig(linkProperties, newConfig);
        String SSID;
        currentConfig->GetSSID(&SSID);
        String linkStr;
        linkProperties->ToString(&linkStr);
        Logger::E(TAG, "IP config changed SSID = %s linkProperties: %s", SSID.string(), linkStr.string());
    }

    if (!proxyChanged) {
        AutoPtr<ILinkProperties> currentLp;
        currentConfig->GetLinkProperties((ILinkProperties**)&currentLp);
        AutoPtr<IProxyProperties> currentHttpProxy;
        currentLp->GetHttpProxy((IProxyProperties**)&currentHttpProxy);
        linkProperties->SetHttpProxy(currentHttpProxy);
    } else {
        currentConfig->SetProxySettings(proxySettings);
        AutoPtr<ILinkProperties> newLp;
        newConfig->GetLinkProperties((ILinkProperties**)&newLp);
        AutoPtr<IProxyProperties> newHttpProxy;
        newLp->GetHttpProxy((IProxyProperties**)&newHttpProxy);
        linkProperties->SetHttpProxy(newHttpProxy);
        String SSID;
        currentConfig->GetSSID(&SSID);
        Logger::E(TAG ,"proxy changed SSID = %s", SSID.string());

        AutoPtr<IProxyProperties> tmp;
        linkProperties->GetHttpProxy((IProxyProperties**)&tmp);
        if (tmp != NULL) {
            String value;
            tmp->ToString(&value);
            Logger::E(TAG, " proxyProperties: %s", value.string());
        }
    }

    if (ipChanged || proxyChanged) {
        currentConfig->SetLinkProperties(linkProperties);
        WriteIpAndProxyConfigurations();
        SendConfiguredNetworksChangedBroadcast(currentConfig,
                IWifiManager::CHANGE_REASON_CONFIG_CHANGE);
    }

    return new NetworkUpdateResult(ipChanged, proxyChanged);
}

void WifiConfigStore::AddIpSettingsFromConfig(
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ IWifiConfiguration* config)
{
    assert(config != NULL && linkProperties != NULL);

    AutoPtr<ILinkProperties> lp;
    config->GetLinkProperties((ILinkProperties**)&lp);
    AutoPtr<IObjectContainer> linkAddresses;
    lp->GetLinkAddresses((IObjectContainer**)&linkAddresses);
    if (linkAddresses != NULL) {
        AutoPtr<IObjectEnumerator> ator;
        linkAddresses->GetObjectEnumerator((IObjectEnumerator**)&ator);
        Boolean hasNext = FALSE;
        while (ator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> linkAddr;
            ator->Current((IInterface**)&linkAddr);
            linkProperties->AddLinkAddress(ILinkAddress::Probe(linkAddr));
        }
    }

    AutoPtr<IObjectContainer> routes;
    lp->GetRoutes((IObjectContainer**)&routes);
    if (routes != NULL) {
        AutoPtr<IObjectEnumerator> ator;
        routes->GetObjectEnumerator((IObjectEnumerator**)&ator);
        Boolean hasNext = FALSE;
        while (ator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> route;
            ator->Current((IInterface**)&route);
            linkProperties->AddRoute(IRouteInfo::Probe(route));
        }
    }

    AutoPtr<IObjectContainer> dnses;
    lp->GetDnses((IObjectContainer**)&dnses);
    if (dnses != NULL) {
        AutoPtr<IObjectEnumerator> ator;
        dnses->GetObjectEnumerator((IObjectEnumerator**)&ator);
        Boolean hasNext = FALSE;
        while (ator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> dns;
            ator->Current((IInterface**)&dns);
            linkProperties->AddDns(IInetAddress::Probe(dns));
        }
    }
}

void WifiConfigStore::ReadNetworkVariables(
    /* [in] */ IWifiConfiguration* config)
{
    Int32 netId;
    config->GetNetworkId(&netId);
    if (netId < 0) {
        return;
    }

    /*
     * TODO: maybe should have a native method that takes an array of
     * variable names and returns an array of values. But we'd still
     * be doing a round trip to the supplicant daemon for each variable.
     */
    String value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::ssidVarName);
    if (!value.IsNullOrEmpty()) {
        if (value.GetChar(0) != '"') {
            StringBuilder sb("\"");
            AutoPtr<IWifiSsidHelper> helper;
            CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
            AutoPtr<IWifiSsid> ws;
            helper->CreateFromHex(value, (IWifiSsid**)&ws);
            String temp;
            ws->ToString(&temp);
            sb += temp;
            sb += "\"";
            config->SetSSID(sb.ToString());
            //TODO: convert a hex string that is not UTF-8 decodable to a P-formatted
            //supplicant string
        }
        else {
            config->SetSSID(value);
        }
    }
    else {
        config->SetSSID(String(NULL));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::bssidVarName);
    if (!value.IsNullOrEmpty()) {
        config->SetBSSID(value);
    }
    else {
        config->SetBSSID(String(NULL));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::priorityVarName);
    config->SetPriority(-1);
    if (!value.IsNullOrEmpty()) {
        //try {
        config->SetPriority(StringUtils::ParseInt32(value));
        //} catch (NumberFormatException ignore) {
        //}
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::hiddenSSIDVarName);
    config->SetHiddenSSID(FALSE);
    if (!value.IsNullOrEmpty()) {
        //try {
        config->SetHiddenSSID(StringUtils::ParseInt32(value) != 0);
        //} catch (NumberFormatException ignore) {
        //}
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::wepTxKeyIdxVarName);
    config->SetWepTxKeyIndex(-1);
    if (!value.IsNullOrEmpty()) {
        //try {
        config->SetWepTxKeyIndex(StringUtils::ParseInt32(value));
        //} catch (NumberFormatException ignore) {
        //}
    }

    AutoPtr<IWifiConfigurationHelper> helper;
    CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&helper);
    AutoPtr< ArrayOf<String> > wepKeys;
    config->GetWepKeys((ArrayOf<String>**)&wepKeys);
    for (Int32 i = 0; i < 4; i++) {
        String wepKeyVarName;
        helper->GetWepKeyVarName(i, &wepKeyVarName);
        value = mWifiNative->GetNetworkVariable(netId, wepKeyVarName);
        if (!value.IsNullOrEmpty()) {
            (*wepKeys)[i] = value;
        }
        else {
            (*wepKeys)[i] = NULL;
        }
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfiguration::pskVarName);
    if (!value.IsNullOrEmpty()) {
        config->SetPreSharedKey(value);
    }
    else {
        config->SetPreSharedKey(String(NULL));
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfigurationProtocol::varName);
    if (!value.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<String> > vals, protocolStrings;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        AutoPtr<IWifiConfigurationProtocol> protocol;
        CWifiConfigurationProtocol::AcquireSingleton((IWifiConfigurationProtocol**)&protocol);
        protocol->GetStrings((ArrayOf<String>**)&protocolStrings);
        for (Int32 i = 0; i < vals->GetLength(); ++i) {
            String val = (*vals)[i];
            Int32 index = LookupString(val, protocolStrings);
            if (0 <= index) {
                AutoPtr<IBitSet> allowedProtocols;
                config->GetAllowedProtocols((IBitSet**)&allowedProtocols);
                allowedProtocols->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfigurationKeyMgmt::varName);
    if (!value.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<String> > vals, keyMgmtStrings;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
        CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
        keyMgmt->GetStrings((ArrayOf<String>**)&keyMgmtStrings);
        for (Int32 i = 0; i < vals->GetLength(); ++i) {
            String val = (*vals)[i];
            Int32 index = LookupString(val, keyMgmtStrings);
            if (0 <= index) {
                AutoPtr<IBitSet> allowedKeyManagement;
                config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
                allowedKeyManagement->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfigurationAuthAlgorithm::varName);
    if (!value.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<String> > vals, authAlgorithmStrings;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        AutoPtr<IWifiConfigurationAuthAlgorithm> authAlgorithm;
        CWifiConfigurationAuthAlgorithm::AcquireSingleton((IWifiConfigurationAuthAlgorithm**)&authAlgorithm);
        authAlgorithm->GetStrings((ArrayOf<String>**)&authAlgorithmStrings);
        for (Int32 i = 0; i < vals->GetLength(); ++i) {
            String val = (*vals)[i];
            Int32 index = LookupString(val, authAlgorithmStrings);
            if (0 <= index) {
                AutoPtr<IBitSet> allowedAuthAlgorithms;
                config->GetAllowedAuthAlgorithms((IBitSet**)&allowedAuthAlgorithms);
                allowedAuthAlgorithms->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfigurationPairwiseCipher::varName);
    if (!value.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<String> > vals, pairwiseCipherStrings;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        AutoPtr<IWifiConfigurationPairwiseCipher> pairwiseCipher;
        CWifiConfigurationPairwiseCipher::AcquireSingleton((IWifiConfigurationPairwiseCipher**)&pairwiseCipher);
        pairwiseCipher->GetStrings((ArrayOf<String>**)&pairwiseCipherStrings);
        for (Int32 i = 0; i < vals->GetLength(); ++i) {
            String val = (*vals)[i];
            Int32 index = LookupString(val, pairwiseCipherStrings);
            if (0 <= index) {
                AutoPtr<IBitSet> allowedPairwiseCiphers;
                config->GetAllowedPairwiseCiphers((IBitSet**)&allowedPairwiseCiphers);
                allowedPairwiseCiphers->Set(index);
            }
        }
    }

    value = mWifiNative->GetNetworkVariable(netId, IWifiConfigurationGroupCipher::varName);
    if (!value.IsNullOrEmpty()) {
        AutoPtr< ArrayOf<String> > vals, groupCipherStrings;
        StringUtils::Split(value, String(" "), (ArrayOf<String>**)&vals);
        AutoPtr<IWifiConfigurationGroupCipher> groupCipher;
        CWifiConfigurationGroupCipher::AcquireSingleton((IWifiConfigurationGroupCipher**)&groupCipher);
        groupCipher->GetStrings((ArrayOf<String>**)&groupCipherStrings);
        for (Int32 i = 0; i < vals->GetLength(); ++i) {
            String val = (*vals)[i];
            Int32 index = LookupString(val, groupCipherStrings);
            if (0 <= index) {
                AutoPtr<IBitSet> allowedGroupCiphers;
                config->GetAllowedGroupCiphers((IBitSet**)&allowedGroupCiphers);
                allowedGroupCiphers->Set(index);
            }
        }
    }

    AutoPtr< ArrayOf<IWifiConfigurationEnterpriseField*> > fields;
    config->GetEnterpriseFields((ArrayOf<IWifiConfigurationEnterpriseField*>**)&fields);
    assert(fields != NULL);
    for (Int32 i = 0; i < fields->GetLength(); i++) {
        AutoPtr<IWifiConfigurationEnterpriseField> field = (*fields)[i];
        String varName;
        field->GetVarName(&varName);
        value = mWifiNative->GetNetworkVariable(netId, varName);
        if (!value.IsNullOrEmpty()) {
            AutoPtr<IWifiConfigurationEnterpriseField> eap, engine;
            config->GetEap((IWifiConfigurationEnterpriseField**)&eap);
            config->GetEngine((IWifiConfigurationEnterpriseField**)&engine);
            if (field != eap && field != engine) {
                value = RemoveDoubleQuotes(value);
            }
            field->SetValue(value);
        }
    }

    MigrateOldEapTlsIfNecessary(config, netId);
}

void WifiConfigStore::MigrateOldEapTlsIfNecessary(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ Int32 netId)
{
    String value = mWifiNative->GetNetworkVariable(netId,
            IWifiConfiguration::OLD_PRIVATE_KEY_NAME);
    /*
     * If the old configuration value is not present, then there is nothing
     * to do.
     */
    if (value.IsNullOrEmpty()) {
        return;
    }
    else {
        // Also ignore it if it's empty quotes.
        value = RemoveDoubleQuotes(value);
        if (value.IsNullOrEmpty()) {
            return;
        }
    }

    AutoPtr<IWifiConfigurationEnterpriseField> engine;
    config->GetEngine((IWifiConfigurationEnterpriseField**)&engine);
    engine->SetValue(IWifiConfiguration::ENGINE_ENABLE);
    AutoPtr<IWifiConfigurationEnterpriseField> engineId;
    config->GetEngineId((IWifiConfigurationEnterpriseField**)&engineId);
    engineId->SetValue(ConvertToQuotedString(IWifiConfiguration::KEYSTORE_ENGINE_ID));

    /*
     * The old key started with the keystore:// URI prefix, but we don't
     * need that anymore. Trim it off if it exists.
     */
    String keyName;
    if (value.StartWith(IWifiConfiguration::KEYSTORE_URI)) {
        keyName = value.Substring(IWifiConfiguration::KEYSTORE_URI.GetLength());
    } else {
        keyName = value;
    }
    AutoPtr<IWifiConfigurationEnterpriseField> keyId;
    config->GetKeyId((IWifiConfigurationEnterpriseField**)&keyId);
    keyId->SetValue(ConvertToQuotedString(keyName));

    // Now tell the wpa_supplicant the new configuration values.
    AutoPtr<ArrayOf<IWifiConfigurationEnterpriseField*> > needsUpdate = ArrayOf<IWifiConfigurationEnterpriseField*>::Alloc(3);
    needsUpdate->Set(0, engine);
    needsUpdate->Set(1, engineId);
    needsUpdate->Set(2, keyId);
    for (Int32 i = 0; i < needsUpdate->GetLength(); i++) {
        AutoPtr<IWifiConfigurationEnterpriseField> field = (*needsUpdate)[i];
        String varName, value;
        field->GetVarName(&varName);
        field->GetValue(&value);
        mWifiNative->SetNetworkVariable(netId, varName, value);
    }

    // Remove old private_key string so we don't run this again.
    mWifiNative->SetNetworkVariable(netId, IWifiConfiguration::OLD_PRIVATE_KEY_NAME,
            ConvertToQuotedString(String("")));

    SaveConfig();
}

String WifiConfigStore::RemoveDoubleQuotes(
    /* [in] */ const String& string)
{
    if (string.GetLength() <= 2) return String("");

    return string.Substring(1, string.GetLength() - 1);
}

String WifiConfigStore::ConvertToQuotedString(
    /* [in] */ const String& string)
{
    return String("\"") + string + String("\"");
}

String WifiConfigStore::MakeString(
    /* [in] */ IBitSet* set,
    /* [in] */ ArrayOf<String>* strings)
{
    StringBuffer buf;
    Int32 nextSetBit = -1;

    /* Make sure all set bits are in [0, strings.length) to avoid
     * going out of bounds on strings.  (Shouldn't happen, but...) */
    AutoPtr<IBitSet> newSet;
    set->Get(0, strings->GetLength(), (IBitSet**)&newSet);
    while (newSet->NextSetBit(nextSetBit + 1, &nextSetBit), nextSetBit != -1) {
        buf.AppendString((*strings)[nextSetBit].Replace('_', '-'));
        buf.AppendChar(' ');
    }

    // remove trailing space
    Int32 number;
    if (set->Cardinality(&number), number > 0) {
        String str;
        buf.Substring(0, buf.GetLength() - 1, &str);
        return str;
    }
    else {
        return buf.ToString();
    }

    return buf.ToString();
}

Int32 WifiConfigStore::LookupString(
    /* [in] */ const String& string,
    /* [in] */ ArrayOf<String>* strings)
{
    assert(strings != NULL);
    Int32 size = strings->GetLength();
    String tmp = string.Replace('-', '_');

    for (Int32 i = 0; i < size; i++) {
        if (tmp.Equals((*strings)[i])) {
            return i;
        }
    }

    // if we ever get here, we should probably add the
    // value to WifiConfiguration to reflect that it's
    // supported by the WPA supplicant
    Loge(String("Failed to look-up a string: ") + string);

    return -1;
}

Int32 WifiConfigStore::ConfigKey(
    /* [in] */ IWifiConfiguration* config)
{
    String key;

    String SSID;
    config->GetSSID(&SSID);
    AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
    CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);

    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    Boolean value;
    if (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &value), value) {
        String WPA_PSK;
        keyMgmt->ToString(IWifiConfigurationKeyMgmt::WPA_PSK, &WPA_PSK);
        key = SSID + WPA_PSK;
    }
    else if ((allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &value), value) ||
            (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &value), value)) {
        String WPA_EAP;
        keyMgmt->ToString(IWifiConfigurationKeyMgmt::WPA_EAP, &WPA_EAP);
        key = SSID + WPA_EAP;
    }
    else {
        AutoPtr< ArrayOf<String> > wepKeys;
        config->GetWepKeys((ArrayOf<String>**)&wepKeys);
        if (!(*wepKeys)[0].IsNull()) {
            key = SSID + String("WEP");
        }
        else {
            String NONE;
            keyMgmt->ToString(IWifiConfigurationKeyMgmt::NONE, &NONE);
            key = SSID + NONE;
        }
    }

    return key.GetHashCode();
}

String WifiConfigStore::Dump()
{
    // StringBuffer sb = new StringBuffer();
    // String LS = System.getProperty("line.separator");
    // sb.append("mLastPriority ").append(mLastPriority).append(LS);
    // sb.append("Configured networks ").append(LS);
    // for (WifiConfiguration conf : getConfiguredNetworks()) {
    //     sb.append(conf).append(LS);
    // }
    // return sb.toString();
    assert(0);
    return String(NULL);
}

String WifiConfigStore::GetConfigFile()
{
    return mIpConfigFile;
}

void WifiConfigStore::Loge(
    /* [in] */ const String& s)
{
   Logger::E(TAG, s.string());
}

void WifiConfigStore::Log(
    /* [in] */ const String& s)
{
   Logger::D(TAG, s.string());
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
