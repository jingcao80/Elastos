
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/net/IpConfigStore.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/net/ReturnOutValue.h>

using Elastos::Droid::Net::CIpConfiguration;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::CNetworkUtils;
using Elastos::Droid::Net::CProxyInfo;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::CStaticIpConfiguration;
using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::IpConfigurationIpAssignment;
using Elastos::Droid::Net::IpConfigurationProxySettings;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;

using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBuffer;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Net::CProxy;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

//============================================================================
// IpConfigStore::InnerSub_Writer
//============================================================================
CAR_INTERFACE_IMPL(IpConfigStore::InnerSub_Writer, Object, IDelayedDiskWriteWriter);

IpConfigStore::InnerSub_Writer::InnerSub_Writer(
    /* [in] */ IpConfigStore* host,
    /* [in] */ ISparseArray* networks)
    : mHost(host)
    , mNetworks(networks)
{}

ECode IpConfigStore::InnerSub_Writer::OnWriteCalled(
    /* [in] */ IDataOutputStream* out)
{
    FAIL_RETURN(IDataOutput::Probe(out)->WriteInt32(IPCONFIG_FILE_VERSION))
    for (Int32 i = 0; i < Ptr(mNetworks)->Func(mNetworks->GetSize); i++) {
        Int32 key;
        AutoPtr<IInterface> value;
        mNetworks->KeyAt(i, &key);
        mNetworks->ValueAt(i, (IInterface**)&value);
        Boolean b;
        FAIL_RETURN(mHost->WriteConfig(out, key, IIpConfiguration::Probe(value), &b))
    }
    return NOERROR;
}
//============================================================================
// IpConfigStore
//============================================================================
const String IpConfigStore::TAG("IpConfigStore");
const Boolean IpConfigStore::DBG = FALSE;
const String IpConfigStore::ID_KEY("id");
const String IpConfigStore::IP_ASSIGNMENT_KEY("ipAssignment");
const String IpConfigStore::LINK_ADDRESS_KEY("linkAddress");
const String IpConfigStore::GATEWAY_KEY("gateway");
const String IpConfigStore::DNS_KEY("dns");
const String IpConfigStore::PROXY_SETTINGS_KEY("proxySettings");
const String IpConfigStore::PROXY_HOST_KEY("proxyHost");
const String IpConfigStore::PROXY_PORT_KEY("proxyPort");
const String IpConfigStore::PROXY_PAC_FILE("proxyPac");
const String IpConfigStore::EXCLUSION_LIST_KEY("exclusionList");
const String IpConfigStore::EOS("eos");
const Int32 IpConfigStore::IPCONFIG_FILE_VERSION = 2;

ECode IpConfigStore::constructor()
{
    mWriter = new DelayedDiskWrite();
    return NOERROR;
}

ECode IpConfigStore::WriteConfig(
    /* [in] */ IDataOutputStream* out,
    /* [in] */ Int32 configKey,
    /* [in] */ IIpConfiguration* config,
    /* [out] */ Boolean* result)
{
    Boolean written = FALSE;
    // try {
    ECode ec;
    switch (Ptr(config)->Func(config->GetIpAssignment)) {
        case Elastos::Droid::Net::STATIC_IpAssignment: {
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(IP_ASSIGNMENT_KEY), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(Ptr(config)->Func(config->GetIpAssignment))), label)
                AutoPtr<IStaticIpConfiguration> staticIpConfiguration;
                config->GetStaticIpConfiguration((IStaticIpConfiguration**)&staticIpConfiguration);
                if (staticIpConfiguration != NULL) {
                    if (Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetIpAddress) != NULL) {
                        AutoPtr<ILinkAddress> ipAddress;
                        staticIpConfiguration->GetIpAddress((ILinkAddress**)&ipAddress);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(LINK_ADDRESS_KEY), label)
                        String s = Ptr(ipAddress)->GetPtr(ipAddress->GetAddress)->Func(IInetAddress::GetHostAddress);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(s), label)
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(Ptr(ipAddress)->Func(ipAddress->GetPrefixLength)), label)
                    }
                    if (Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetGateway) != NULL) {
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(GATEWAY_KEY), label)
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(0), label)  // Default route.
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(1), label)  // Have a gateway.
                        String s;
                        Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetGateway)->GetHostAddress(&s);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(s), label)
                    }
                    FOR_EACH(iter, Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetDnsServers)) {
                        AutoPtr<IInetAddress> inetAddr = IInetAddress::Probe(Ptr(iter)->Func(iter->GetNext));
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(DNS_KEY), label)
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(Ptr(inetAddr)->Func(inetAddr->GetHostAddress)), label)
                    }
                }
                written = TRUE;
            }
            break;
        case Elastos::Droid::Net::DHCP_IpAssignment:
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(IP_ASSIGNMENT_KEY), label)
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(Ptr(config)->Func(config->GetIpAssignment))), label)
            written = TRUE;
            break;
        case Elastos::Droid::Net::UNASSIGNED_IpAssignment:
            /* Ignore */
            break;
        default:
            Loge("Ignore invalid ip assignment while writing");
            break;
    }
    switch (Ptr(config)->Func(config->GetProxySettings)) {
        case Elastos::Droid::Net::STATIC_ProxySettings: {
                AutoPtr<IProxyInfo> proxyProperties;
                config->GetHttpProxy((IProxyInfo**)&proxyProperties);
                String exclusionList;
                proxyProperties->GetExclusionListAsString(&exclusionList);
                IDataOutput::Probe(out)->WriteUTF(PROXY_SETTINGS_KEY);
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(Ptr(config)->Func(config->GetProxySettings))), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_HOST_KEY), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(Ptr(proxyProperties)->Func(proxyProperties->GetHost)), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_PORT_KEY), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(Ptr(proxyProperties)->Func(proxyProperties->GetPort)), label)
                if (exclusionList != NULL) {
                    FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(EXCLUSION_LIST_KEY), label)
                    FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(exclusionList), label)
                }
                written = TRUE;
            }
            break;
        case Elastos::Droid::Net::PAC_ProxySettings: {
                AutoPtr<IProxyInfo> proxyPacProperties;
                config->GetHttpProxy((IProxyInfo**)&proxyPacProperties);
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_SETTINGS_KEY), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(Ptr(config)->Func(config->GetProxySettings))), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_PAC_FILE), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(Object::ToString(Ptr(proxyPacProperties)->Func(proxyPacProperties->GetPacFileUrl))), label)
                written = TRUE;
            }
            break;
        case Elastos::Droid::Net::NONE_ProxySettings:
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_SETTINGS_KEY), label)
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(Ptr(config)->Func(config->GetProxySettings))), label)
            written = TRUE;
            break;
        case Elastos::Droid::Net::UNASSIGNED_ProxySettings:
            /* Ignore */
            break;
        default:
            Loge("Ignore invalid proxy settings while writing");
            break;
    }
    if (written) {
        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(ID_KEY), label)
        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(configKey), label)
    }
    // } catch (NullPointerException e) {
    label:
    if (FAILED(ec)) {
        if ((ECode)E_NULL_POINTER_EXCEPTION == ec)
            Loge(TAG, "Failure in writing %s%d", TO_CSTR(config), ec);
        else
            return ec;
    }
    // }
    IDataOutput::Probe(out)->WriteUTF(EOS);
    FUNC_RETURN(written)
}

ECode IpConfigStore::WriteIpAndProxyConfigurations(
    /* [in] */ const String& filePath,
    /* [in] */ ISparseArray* networks)
{
    return mWriter->Write(filePath, new InnerSub_Writer(this, networks));
}

ECode IpConfigStore::ReadIpAndProxyConfigurations(
    /* [in] */ const String& filePath,
    /* [out] */ ISparseArray** result)
{
    AutoPtr<ISparseArray> networks;
    CSparseArray::New((ISparseArray**)&networks);
    AutoPtr<IDataInputStream> in;
    ECode ec;
    AutoPtr<IFileInputStream> fileInputStream;
    AutoPtr<IBufferedInputStream> bufferedInputStream;
    FAIL_GOTO(ec = CFileInputStream::New(filePath, (IFileInputStream**)&fileInputStream), out_try)
    FAIL_GOTO(ec = CBufferedInputStream::New(IInputStream::Probe(fileInputStream), (IBufferedInputStream**)&bufferedInputStream), out_try)
    FAIL_GOTO(ec = CDataInputStream::New(IInputStream::Probe(bufferedInputStream), (IDataInputStream**)&in), out_try)
    Int32 version;
    FAIL_GOTO(ec = IDataInput::Probe(in)->ReadInt32(&version), out_try)
    if (version != 2 && version != 1) {
        Loge("Bad version on IP configuration file, ignore read");
        *result = NULL;
        if (in != NULL) {
            // try {
            ICloseable::Probe(in)->Close();
            // } catch (Exception e) {}
        }
        return NOERROR;
    }
    while (TRUE) {
        Int32 id = -1;
        // Default is DHCP with no proxy
        IpConfigurationIpAssignment ipAssignment = Elastos::Droid::Net::DHCP_IpAssignment;
        IpConfigurationProxySettings proxySettings = Elastos::Droid::Net::NONE_ProxySettings;
        AutoPtr<IStaticIpConfiguration> staticIpConfiguration;
        CStaticIpConfiguration::New((IStaticIpConfiguration**)&staticIpConfiguration);
        String proxyHost(NULL);
        String pacFileUrl(NULL);
        Int32 proxyPort = -1;
        String exclusionList(NULL);
        String key;
        do {
            FAIL_GOTO(ec = IDataInput::Probe(in)->ReadUTF(&key), out_try)
            ECode ec_inner;
            AutoPtr<INetworkUtils> helper;
            CNetworkUtils::AcquireSingleton((INetworkUtils**)&helper);
            if (key.Equals(ID_KEY)) {
                FAIL_GOTO(ec_inner = IDataInput::Probe(in)->ReadInt32(&id), inner_try)
            }
            else if (key.Equals(IP_ASSIGNMENT_KEY)) {
                ipAssignment = StringUtils::ParseInt32(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF));
            }
            else if (key.Equals(LINK_ADDRESS_KEY)) {
                AutoPtr<IInetAddress> inetAddress;
                FAIL_GOTO(ec_inner = helper->NumericToInetAddress(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF), (IInetAddress**)&inetAddress), inner_try)
                AutoPtr<ILinkAddress> linkAddr;
                CLinkAddress::New(inetAddress, Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadInt32), (ILinkAddress**)&linkAddr);
                if (IInet4Address::Probe(Ptr(linkAddr)->Func(linkAddr->GetAddress)) != NULL &&
                        Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetIpAddress) == NULL) {
                    staticIpConfiguration->SetIpAddress(linkAddr);
                }
                else {
                    Loge("Non-IPv4 or duplicate address: %s", TO_CSTR(linkAddr));
                }
            }
            else if (key.Equals(GATEWAY_KEY)) {
                AutoPtr<ILinkAddress> dest;
                AutoPtr<IInetAddress> gateway;
                if (version == 1) {
                    // only supported default gateways - leave the dest/prefix empty
                    FAIL_GOTO(ec_inner = helper->NumericToInetAddress(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF), (IInetAddress**)&gateway), inner_try)
                    if (Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetGateway) == NULL) {
                        staticIpConfiguration->SetGateway(gateway);
                    } else {
                        Loge("Duplicate gateway: %s", Ptr(gateway)->Func(gateway->GetHostAddress).string());
                    }
                } else {
                    if (Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadInt32) == 1) {
                        AutoPtr<IInetAddress> inetAddress;
                        FAIL_GOTO(ec_inner = helper->NumericToInetAddress(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF),
                            (IInetAddress**)&inetAddress), inner_try)
                        CLinkAddress::New(inetAddress, Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadInt32), (ILinkAddress**)&dest);
                    }
                    if (Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadInt32) == 1) {
                        FAIL_GOTO(ec_inner = helper->NumericToInetAddress(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF), (IInetAddress**)&gateway), inner_try)
                    }
                    AutoPtr<IRouteInfo> route;
                    CRouteInfo::New(dest, gateway, (IRouteInfo**)&route);
                    if (Ptr(route)->Func(route->IsIPv4Default) &&
                            Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetGateway) == NULL) {
                        staticIpConfiguration->SetGateway(gateway);
                    } else {
                        Loge("Non-IPv4 default or duplicate route: %s", TO_CSTR(route));
                    }
                }
            } else if (key.Equals(DNS_KEY)) {
                AutoPtr<IInetAddress> inetAddress;
                FAIL_GOTO(ec_inner = helper->NumericToInetAddress(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF), (IInetAddress**)&inetAddress), inner_try)
                Ptr(staticIpConfiguration)->Func(staticIpConfiguration->GetDnsServers)->Add(inetAddress);
            } else if (key.Equals(PROXY_SETTINGS_KEY)) {
                proxySettings = StringUtils::ParseInt32(Ptr(IDataInput::Probe(in))->Func(IDataInput::ReadUTF));
            } else if (key.Equals(PROXY_HOST_KEY)) {
                FAIL_GOTO(ec_inner = IDataInput::Probe(in)->ReadUTF(&proxyHost), inner_try)
            } else if (key.Equals(PROXY_PORT_KEY)) {
                FAIL_GOTO(ec_inner = IDataInput::Probe(in)->ReadInt32(&proxyPort), inner_try)
            } else if (key.Equals(PROXY_PAC_FILE)) {
                FAIL_GOTO(ec_inner = IDataInput::Probe(in)->ReadUTF(&pacFileUrl), inner_try)
            } else if (key.Equals(EXCLUSION_LIST_KEY)) {
                FAIL_GOTO(ec_inner = IDataInput::Probe(in)->ReadUTF(&exclusionList), inner_try)
            } else if (key.Equals(EOS)) {
                break;
            } else {
                Loge("Ignore unknown key %s while reading", key.string());
            }
            inner_try:
            if (FAILED(ec)) {
                if ((ECode)E_ILLEGAL_ARGUMENT_EXCEPTION == ec_inner)
                    Loge("Ignore invalid address while reading %d", ec_inner);
                else
                    goto out_try;
            }
        } while (TRUE);
        if (id != -1) {
            AutoPtr<IIpConfiguration> config;
            CIpConfiguration::New((IIpConfiguration**)&config);
            networks->Put(id, config);
            switch (ipAssignment) {
                case Elastos::Droid::Net::STATIC_IpAssignment:
                    config->SetStaticIpConfiguration(staticIpConfiguration);
                    config->SetIpAssignment(ipAssignment);
                    break;
                case Elastos::Droid::Net::DHCP_IpAssignment:
                    config->SetIpAssignment(ipAssignment);
                    break;
                case Elastos::Droid::Net::UNASSIGNED_IpAssignment:
                    Loge("BUG: Found UNASSIGNED IP on file, use DHCP");
                    config->SetIpAssignment(Elastos::Droid::Net::DHCP_IpAssignment);
                    break;
                default:
                    Loge("Ignore invalid ip assignment while reading.");
                    config->SetIpAssignment(Elastos::Droid::Net::UNASSIGNED_IpAssignment);
                    break;
            }
            switch (proxySettings) {
                case Elastos::Droid::Net::STATIC_ProxySettings: {
                        AutoPtr<IProxyInfo> proxyInfo;
                        CProxyInfo::New(proxyHost, proxyPort, exclusionList, (IProxyInfo**)&proxyInfo);
                        config->SetProxySettings(proxySettings);
                        config->SetHttpProxy(proxyInfo);
                    }
                    break;
                case Elastos::Droid::Net::PAC_ProxySettings: {
                        AutoPtr<IProxyInfo> proxyPacProperties;
                        CProxyInfo::New(pacFileUrl, (IProxyInfo**)&proxyPacProperties);
                        config->SetProxySettings(proxySettings);
                        config->SetHttpProxy(proxyPacProperties);
                    }
                    break;
                case Elastos::Droid::Net::NONE_ProxySettings:
                    config->SetProxySettings(proxySettings);
                    break;
                case Elastos::Droid::Net::UNASSIGNED_ProxySettings:
                    Loge("BUG: Found UNASSIGNED proxy on file, use NONE");
                    config->SetProxySettings(Elastos::Droid::Net::NONE_ProxySettings);
                    break;
                default:
                    Loge("Ignore invalid proxy settings while reading");
                    config->SetProxySettings(Elastos::Droid::Net::UNASSIGNED_ProxySettings);
                    break;
            }
        } else {
            if (DBG) Log("Missing id while parsing configuration");
        }
    }
out_try:
    if (FAILED(ec)) {
        if ((ECode)E_EOF_EXCEPTION == ec) {
        }
        else if ((ECode)E_IO_EXCEPTION == ec) {
            Loge("Error parsing configuration: %d", ec);
        }
    }
    // finally {
    if (in != NULL) {
        // try {
        ICloseable::Probe(in)->Close();
        // } catch (Exception e) {}
    }
    // }

    *result = networks;
    REFCOUNT_ADD(*result)
    return ec;
}

ECode IpConfigStore::Loge(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::E(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

ECode IpConfigStore::Log(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::D(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos
