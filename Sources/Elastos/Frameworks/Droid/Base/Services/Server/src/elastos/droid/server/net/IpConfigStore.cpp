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
using Elastos::Droid::Net::IUri;
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
using Elastos::Utility::IArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::IIterator;
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
    Int32 size;
    mNetworks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
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
    ECode ec = NOERROR;
    IpConfigurationIpAssignment ipAssignment;
    config->GetIpAssignment(&ipAssignment);
    switch (ipAssignment) {
        case Elastos::Droid::Net::STATIC_IpAssignment: {
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(IP_ASSIGNMENT_KEY), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(ipAssignment)), label)
                AutoPtr<IStaticIpConfiguration> staticIpConfiguration;
                config->GetStaticIpConfiguration((IStaticIpConfiguration**)&staticIpConfiguration);
                if (staticIpConfiguration != NULL) {
                    AutoPtr<ILinkAddress> ipAddress;
                    staticIpConfiguration->GetIpAddress((ILinkAddress**)&ipAddress);
                    if (ipAddress != NULL) {
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(LINK_ADDRESS_KEY), label)
                        AutoPtr<IInetAddress> address;
                        ipAddress->GetAddress((IInetAddress**)&address);
                        String host;
                        address->GetHostAddress(&host);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(host), label)
                        Int32 length;
                        ipAddress->GetPrefixLength(&length);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(length), label)
                    }
                    AutoPtr<IInetAddress> gateway;
                    staticIpConfiguration->GetGateway((IInetAddress**)&gateway);
                    if (gateway != NULL) {
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(GATEWAY_KEY), label)
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(0), label)  // Default route.
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(1), label)  // Have a gateway.
                        String s;
                        gateway->GetHostAddress(&s);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(s), label)
                    }
                    AutoPtr<IArrayList> dnsServers;
                    staticIpConfiguration->GetDnsServers((IArrayList**)&dnsServers);
                    AutoPtr<IIterator> it;
                    dnsServers->GetIterator((IIterator**)&it);
                    Boolean hasNext;
                    while (it->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> obj;
                        it->GetNext((IInterface**)&obj);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(DNS_KEY), label)
                        String host;
                        IInetAddress::Probe(obj)->GetHostAddress(&host);
                        FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(host), label)
                    }
                }
                written = TRUE;
            }
            break;
        case Elastos::Droid::Net::DHCP_IpAssignment:
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(IP_ASSIGNMENT_KEY), label)
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(ipAssignment)), label)
            written = TRUE;
            break;
        case Elastos::Droid::Net::UNASSIGNED_IpAssignment:
            /* Ignore */
            break;
        default:
            Loge("Ignore invalid ip assignment while writing");
            break;
    }
    IpConfigurationProxySettings proxySettings;
    config->GetProxySettings(&proxySettings);
    switch (proxySettings) {
        case Elastos::Droid::Net::STATIC_ProxySettings: {
                AutoPtr<IProxyInfo> proxyProperties;
                config->GetHttpProxy((IProxyInfo**)&proxyProperties);
                String exclusionList;
                proxyProperties->GetExclusionListAsString(&exclusionList);
                IDataOutput::Probe(out)->WriteUTF(PROXY_SETTINGS_KEY);
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(proxySettings)), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_HOST_KEY), label)
                String host;
                proxyProperties->GetHost(&host);
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(host), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_PORT_KEY), label)
                Int32 port;
                proxyProperties->GetPort(&port);
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteInt32(port), label)
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
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(proxySettings)), label)
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_PAC_FILE), label)
                AutoPtr<IUri> uri;
                proxyPacProperties->GetPacFileUrl((IUri**)&uri);
                FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(Object::ToString(uri)), label)
                written = TRUE;
            }
            break;
        case Elastos::Droid::Net::NONE_ProxySettings:
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(PROXY_SETTINGS_KEY), label)
            FAIL_GOTO(ec = IDataOutput::Probe(out)->WriteUTF(StringUtils::ToString(proxySettings)), label)
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
    *result = written;
    return NOERROR;
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
                String str;
                IDataInput::Probe(in)->ReadUTF(&str);
                ipAssignment = StringUtils::ParseInt32(str);
            }
            else if (key.Equals(LINK_ADDRESS_KEY)) {
                String str;
                IDataInput::Probe(in)->ReadUTF(&str);
                AutoPtr<IInetAddress> inetAddress;
                FAIL_GOTO(ec_inner = helper->NumericToInetAddress(str, (IInetAddress**)&inetAddress), inner_try)
                Int32 iv;
                IDataInput::Probe(in)->ReadInt32(&iv);
                AutoPtr<ILinkAddress> linkAddr;
                CLinkAddress::New(inetAddress, iv, (ILinkAddress**)&linkAddr);
                inetAddress = NULL;
                linkAddr->GetAddress((IInetAddress**)&inetAddress);
                AutoPtr<ILinkAddress> configLinkAddr;
                staticIpConfiguration->GetIpAddress((ILinkAddress**)&configLinkAddr);
                if (IInet4Address::Probe(inetAddress) != NULL && configLinkAddr == NULL) {
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
                    String str;
                    IDataInput::Probe(in)->ReadUTF(&str);
                    FAIL_GOTO(ec_inner = helper->NumericToInetAddress(str, (IInetAddress**)&gateway), inner_try)
                    AutoPtr<IInetAddress> gateway;
                    staticIpConfiguration->GetGateway((IInetAddress**)&gateway);
                    if (gateway == NULL) {
                        staticIpConfiguration->SetGateway(gateway);
                    }
                    else {
                        String host;
                        gateway->GetHostAddress(&host);
                        Loge("Duplicate gateway: %s", host.string());
                    }
                }
                else {
                    Int32 iv;
                    IDataInput::Probe(in)->ReadInt32(&iv);
                    if (iv == 1) {
                        String str;
                        IDataInput::Probe(in)->ReadUTF(&str);
                        AutoPtr<IInetAddress> inetAddress;
                        FAIL_GOTO(ec_inner = helper->NumericToInetAddress(str,
                            (IInetAddress**)&inetAddress), inner_try)
                        IDataInput::Probe(in)->ReadInt32(&iv);
                        CLinkAddress::New(inetAddress, iv, (ILinkAddress**)&dest);
                    }
                    IDataInput::Probe(in)->ReadInt32(&iv);
                    if (iv == 1) {
                        String str;
                        IDataInput::Probe(in)->ReadUTF(&str);
                        FAIL_GOTO(ec_inner = helper->NumericToInetAddress(str, (IInetAddress**)&gateway), inner_try)
                    }
                    AutoPtr<IRouteInfo> route;
                    CRouteInfo::New(dest, gateway, (IRouteInfo**)&route);
                    Boolean ipv4;
                    route->IsIPv4Default(&ipv4);
                    AutoPtr<IInetAddress> gateway;
                    staticIpConfiguration->GetGateway((IInetAddress**)&gateway);
                    if (ipv4 && gateway == NULL) {
                        staticIpConfiguration->SetGateway(gateway);
                    }
                    else {
                        Loge("Non-IPv4 default or duplicate route: %s", TO_CSTR(route));
                    }
                }
            } else if (key.Equals(DNS_KEY)) {
                String str;
                IDataInput::Probe(in)->ReadUTF(&str);
                AutoPtr<IInetAddress> inetAddress;
                FAIL_GOTO(ec_inner = helper->NumericToInetAddress(str, (IInetAddress**)&inetAddress), inner_try)
                AutoPtr<IArrayList> dnsServers;
                staticIpConfiguration->GetDnsServers((IArrayList**)&dnsServers);
                dnsServers->Add(inetAddress);
            } else if (key.Equals(PROXY_SETTINGS_KEY)) {
                String str;
                IDataInput::Probe(in)->ReadUTF(&str);
                proxySettings = StringUtils::ParseInt32(str);
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
