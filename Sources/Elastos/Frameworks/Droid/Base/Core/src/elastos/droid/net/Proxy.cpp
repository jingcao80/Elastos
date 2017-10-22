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

#include <Elastos.CoreLibrary.Apache.h>
#include "elastos/droid/net/Proxy.h"
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/net/CPacProxySelector.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/PacProxySelector.h"
#include "elastos/droid/net/Uri.h"
#include <elastos/core/StringUtils.h>
#include <elastos/net/ProxySelector.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::NetworkUtils;

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Net::CProxy;
using Elastos::Net::CURIHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IProxySelectorHelper;
using Elastos::Net::IURIHelper;
using Elastos::Net::ProxySelector;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;
using Org::Apache::Http::CHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {

const String Proxy::NAME_IP_REGEX("[a-zA-Z0-9]+(\\-[a-zA-Z0-9]+)*(\\.[a-zA-Z0-9]+(\\-[a-zA-Z0-9]+)*)*");
const String Proxy::EXCL_REGEX("[a-zA-Z0-9*]+(\\-[a-zA-Z0-9*]+)*(\\.[a-zA-Z0-9*]+(\\-[a-zA-Z0-9*]+)*)*");
const String Proxy::EXCLLIST_REGEXP = String("^$|^") + EXCL_REGEX + "(," + EXCL_REGEX + ")*$";
const Boolean Proxy::DEBUG = FALSE;
const String Proxy::TAG("Proxy");
AutoPtr<IConnectivityManager> Proxy::sConnectivityManager = NULL;
const String Proxy::HOSTNAME_REGEXP = String("^$|^") + NAME_IP_REGEX + "$";

AutoPtr<IPattern> Proxy::HOSTNAME_PATTERN = getHOSTNAME_PATTERN();
AutoPtr<IPattern> Proxy::EXCLLIST_PATTERN = getEXCLLIST_PATTERN();
AutoPtr<IProxySelector> Proxy::sDefaultProxySelector = getDEFAULT_PROXY_SELECTOR();

AutoPtr<IPattern> Proxy::getHOSTNAME_PATTERN()
{
    AutoPtr<IPattern> rev;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(HOSTNAME_REGEXP, (IPattern**)&rev);
    return rev;
}

AutoPtr<IPattern> Proxy::getEXCLLIST_PATTERN()
{
    AutoPtr<IPattern> rev;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(EXCLLIST_REGEXP, (IPattern**)&rev);
    return rev;
}

AutoPtr<IProxySelector> Proxy::getDEFAULT_PROXY_SELECTOR()
{
    AutoPtr<IProxySelector> rev;
    ProxySelector::GetDefault((IProxySelector**)&rev);
    return rev;
}

ECode Proxy::GetProxy(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& url,
    /* [out] */ Elastos::Net::IProxy** result)
{
    VALIDATE_NOT_NULL(result)

    String host("");
    if ((url != String(NULL)) && !IsLocalHost(host)) {
        AutoPtr<IURIHelper> helper;
        CURIHelper::AcquireSingleton((IURIHelper**)&helper);
        AutoPtr<IURI> uri;
        helper->Create(url, (IURI**)&uri);
        AutoPtr<IProxySelector> proxySelector;
        ProxySelector::GetDefault((IProxySelector**)&proxySelector);
        AutoPtr<IList> proxyList;
        proxySelector->Select(uri, (IList**)&proxyList);
        Int32 size;
        proxyList->GetSize(&size);
        if (size > 0) {
            AutoPtr<IInterface> obj;
            proxyList->Get(0, (IInterface**)&obj);
            *result = Elastos::Net::IProxy::Probe(obj);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    AutoPtr<Elastos::Net::IProxyHelper> helper;
    Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noPorxy;
    helper->GetNO_PROXY(result);
    return NOERROR;
}

ECode Proxy::GetHost(
    /* [in] */ IContext* ctx,
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);

    AutoPtr<Elastos::Net::IProxyHelper> helper;
    Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> proxy;
    AutoPtr<Elastos::Net::IProxy> noproxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noproxy);
    String str;
    GetProxy(ctx, str, (Elastos::Net::IProxy**)&proxy);
    if (proxy == noproxy) {
        *host = NULL;
        return NOERROR;
    }
    AutoPtr<ISocketAddress> socketAddress;
    proxy->GetAddress((ISocketAddress**)&socketAddress);
    AutoPtr<IInetSocketAddress> address = IInetSocketAddress::Probe(socketAddress);
    return address->GetHostName(host);
}

ECode Proxy::GetPort(
    /* [in] */ IContext* ctx,
    /* [out] */ Int32* portNum)
{
    VALIDATE_NOT_NULL(portNum);

    AutoPtr<Elastos::Net::IProxyHelper> helper;
    Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noproxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noproxy);
    String str;
    AutoPtr<Elastos::Net::IProxy> proxy;
    GetProxy(ctx, str, (Elastos::Net::IProxy**)&proxy);
    if (proxy == noproxy) {
         *portNum = -1;
         return NOERROR;
    }
    AutoPtr<ISocketAddress> socketAddress;
    proxy->GetAddress((ISocketAddress**)&socketAddress);
    AutoPtr<IInetSocketAddress> inetaddress;
    AutoPtr<IInetSocketAddress> address = IInetSocketAddress::Probe(socketAddress);
    return address->GetPort(portNum);
}

ECode Proxy::GetDefaultHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetProperty(String("http.proxyHost"), host);
    return NOERROR;
}

ECode Proxy::GetDefaultPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    String host;
    GetDefaultHost(&host);
    if (host.IsNull()) {
        *port = -1;
        return NOERROR;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    String portS;
    system->GetProperty(String("http.proxyPort"), &portS);
    *port = StringUtils::ParseInt32(portS);
    return NOERROR;
}

ECode Proxy::GetPreferredHttpHost(
    /* [in] */ IContext* context,
    /* [in] */ const String& url,
    /* [out] */ IHttpHost** httpHost)
{
    VALIDATE_NOT_NULL(httpHost);

    AutoPtr<Elastos::Net::IProxyHelper> helper;
    Elastos::Net::CProxyHelper::AcquireSingleton((Elastos::Net::IProxyHelper**)&helper);
    AutoPtr<Elastos::Net::IProxy> noproxy;
    helper->GetNO_PROXY((Elastos::Net::IProxy**)&noproxy);
    AutoPtr<Elastos::Net::IProxy> prefProxy;
    GetProxy(context, url, (Elastos::Net::IProxy**)&prefProxy);
    if (prefProxy == noproxy) {
        *httpHost = NULL;
        return NOERROR;
    } else {
        AutoPtr<ISocketAddress> socketaddress;
        prefProxy->GetAddress((ISocketAddress**)&socketaddress);
        AutoPtr<IInetSocketAddress> sa = IInetSocketAddress::Probe(socketaddress);
        String hostName;
        sa->GetHostName(&hostName);
        Int32 portNum;
        sa->GetPort(&portNum);
        return CHttpHost::New(hostName, portNum, String("http"), httpHost);
    }
    return NOERROR;
}

Boolean Proxy::IsLocalHost(
    /* [in] */ const String& host)
{
    if (host.IsNull()) {
        return FALSE;
    }

    if (!(host.IsNull())) {
        if (host.EqualsIgnoreCase("localhost")) {
            return TRUE;
        }
        AutoPtr<IInetAddress> address;
        NetworkUtils::NumericToInetAddress(host, (IInetAddress**)&address);
        Boolean isLoopback;
        address->IsLoopbackAddress(&isLoopback);
        if (isLoopback == TRUE) {
            return TRUE;
        }
    }

    return FALSE;
}

ECode Proxy::Validate(
    /* [in] */ const String& hostname,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMatcher> match;
    HOSTNAME_PATTERN->Matcher(hostname, (IMatcher**)&match);
    AutoPtr<IMatcher> listMatch;
    EXCLLIST_PATTERN->Matcher(exclList, (IMatcher**)&listMatch);
    Boolean isMatch;
    if (match->Matches(&isMatch), !isMatch) {
        *result = IProxy::PROXY_HOSTNAME_INVALID;
        return NOERROR;
    }
    if (listMatch->Matches(&isMatch), !isMatch) {
        *result = IProxy::PROXY_EXCLLIST_INVALID;
        return NOERROR;
    }
    if (hostname.GetLength() > 0 && port.GetLength() == 0) {
        *result = IProxy::PROXY_PORT_EMPTY;
        return NOERROR;
    }
    if (port.GetLength() > 0) {
        if (hostname.GetLength() == 0) {
            *result = IProxy::PROXY_HOSTNAME_EMPTY;
            return NOERROR;
        }
        Int32 portVal = -1;
        // try {
        ECode ec = StringUtils::Parse(port, &portVal);
        // } catch (NumberFormatException ex) {
        if (FAILED(ec)) {
            if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                *result = IProxy::PROXY_PORT_INVALID;
                return NOERROR;
            }
            else return ec;
        }
        // }
        if (portVal <= 0 || portVal > 0xFFFF) {
            *result = IProxy::PROXY_PORT_INVALID;
            return NOERROR;
        }
    }
    *result = IProxy::PROXY_VALID;
    return NOERROR;
}

ECode Proxy::SetHttpProxySystemProperty(
    /* [in] */ IProxyInfo* proxyp)
{
    String host;
    String port;
    String exclList;
    AutoPtr<IUri> pacFileUrl;
    Uri::GetEMPTY((IUri**)&pacFileUrl);
    if (proxyp != NULL) {
        proxyp->GetHost(&host);
        Int32 portNum;
        proxyp->GetPort(&portNum);
        port = StringUtils::ToString(portNum);
        proxyp->GetExclusionListAsString(&exclList);
        proxyp->GetPacFileUrl((IUri**)&pacFileUrl);
    }
    SetHttpProxySystemProperty(host, port, exclList, pacFileUrl);
    return NOERROR;
}

ECode Proxy::SetHttpProxySystemProperty(
    /* [in] */ const String& host,
    /* [in] */ const String& port,
    /* [in] */ const String& exclList,
    /* [in] */ IUri* pacFileUrl)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String tmpExclList(exclList);
    if (!(tmpExclList.IsNull())) {
        tmpExclList = tmpExclList.Replace(',', '|');
    }
    String s;
    if (FALSE)
        Logger::D(TAG, "setHttpProxySystemProperty :%s:%s - %s", host.string(), port.string(), tmpExclList.string());
    if (host != String(NULL)) {
        system->SetProperty(String("http.proxyHost"), host, &s);
        system->SetProperty(String("https.proxyHost"), host, &s);
    }
    else {
        system->ClearProperty(String("http.proxyHost"), &s);
        system->ClearProperty(String("https.proxyHost"), &s);
    }
    if (port != String(NULL)) {
        system->SetProperty(String("http.proxyPort"), port, &s);
        system->SetProperty(String("https.proxyPort"), port, &s);
    }
    else {
        system->ClearProperty(String("http.proxyPort"), &s);
        system->ClearProperty(String("https.proxyPort"), &s);
    }
    if (tmpExclList != String(NULL)) {
        system->SetProperty(String("http.nonProxyHosts"), tmpExclList, &s);
        system->SetProperty(String("https.nonProxyHosts"), tmpExclList, &s);
    } else {
        system->ClearProperty(String("http.nonProxyHosts"), &s);
        system->ClearProperty(String("https.nonProxyHosts"), &s);
    }
    AutoPtr<IUri> empty;
    Uri::GetEMPTY((IUri**)&empty);
    AutoPtr<IProxySelectorHelper> helper;
    Boolean isEquals;
    IObject::Probe(empty)->Equals(pacFileUrl, &isEquals);
    if (!isEquals) {
        AutoPtr<IPacProxySelector> newProxySelector;
        CPacProxySelector::New((IPacProxySelector**)&newProxySelector);
        ProxySelector::SetDefault(IProxySelector::Probe(newProxySelector));
    } else {
        ProxySelector::SetDefault(sDefaultProxySelector);
    }
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
