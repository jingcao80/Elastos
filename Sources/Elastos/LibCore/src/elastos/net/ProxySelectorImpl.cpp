
#include "ProxySelectorImpl.h"
#include "CProxy.h"
#include "CInetSocketAddress.h"
#include "StringUtils.h"
#include "StringBuilder.h"
#include "Collections.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Collections;

namespace Elastos {
namespace Net {


ECode ProxySelectorImpl::ConnectFailed(
    /* [in] */ IURI* uri,
    /* [in] */ ISocketAddress* sa,
    /* [in] */ ECode ec)
{
    if (uri == NULL || sa == NULL || SUCCEEDED(ec)) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode ProxySelectorImpl::Select(
    /* [in] */ IURI* uri,
    /* [out] */ IList** container)
{
    VALIDATE_NOT_NULL(container);
    *container = NULL;

    AutoPtr<IProxy> proxy;
    FAIL_RETURN(SelectOneProxy(uri, (IProxy**)&proxy))
    return Collections::SingletonList(proxy, container);
}

ECode ProxySelectorImpl::SelectOneProxy(
    /* [in] */ IURI* uri,
    /* [out] */ IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = NULL;
    VALIDATE_NOT_NULL(uri)

    String scheme;
    uri->GetScheme(&scheme);
    if (scheme.IsNull()) {
        // throw new IllegalArgumentException("scheme == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 port = -1;
    String nonProxyHostsKey;
    Boolean httpProxyOkay = TRUE;
    if (scheme.EqualsIgnoreCase("http")) {
        port = 80;
        nonProxyHostsKey = String("http.nonProxyHosts");
        LookupProxy(String("http.proxyHost"), String("http.proxyPort"),
            ProxyType_HTTP, port, proxy);
    }
    else if (scheme.EqualsIgnoreCase("https")) {
        port = 443;
        nonProxyHostsKey = String("https.nonProxyHosts"); // RI doesn't support this
        LookupProxy(String("https.proxyHost"), String("https.proxyPort"),
            ProxyType_HTTP, port, proxy);
    }
    else if (scheme.EqualsIgnoreCase("ftp")) {
        port = 80; // not 21 as you might guess
        nonProxyHostsKey = String("ftp.nonProxyHosts");
        LookupProxy(String("ftp.proxyHost"), String("ftp.proxyPort"),
            ProxyType_HTTP, port, proxy);
    }
    else if (scheme.EqualsIgnoreCase("socket")) {
        httpProxyOkay = FALSE;
    }
    else {
        *proxy = CProxy::NO_PROXY;
        REFCOUNT_ADD(*proxy)
        return NOERROR;
    }

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String prop;
    system->GetProperty(nonProxyHostsKey, &prop);
    String host;
    uri->GetHost(&host);
    if (!nonProxyHostsKey.IsNull() && IsNonProxyHost(host, prop)) {
        *proxy = CProxy::NO_PROXY;
        REFCOUNT_ADD(*proxy)
        return NOERROR;
    }

    if (*proxy != NULL) {
        return NOERROR;
    }

    if (httpProxyOkay) {
        LookupProxy(String("proxyHost"), String("proxyPort"),
            ProxyType_HTTP, port, proxy);
        if (*proxy != NULL) {
            return NOERROR;
        }
    }

    LookupProxy(String("socksProxyHost"), String("socksProxyPort"),
        ProxyType_SOCKS, 1080, proxy);
    if (*proxy != NULL) {
        return NOERROR;
    }

    *proxy = CProxy::NO_PROXY;
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

ECode ProxySelectorImpl::LookupProxy(
    /* [in] */ const String& hostKey,
    /* [in] */ const String& portKey,
    /* [in] */ ProxyType type,
    /* [in] */Int32 defaultPort,
    /* [out] */ IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = NULL;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String host;
    system->GetProperty(hostKey, &host);
    if (host.IsNullOrEmpty()) {
        return NOERROR;
    }

    Int32 port;
    FAIL_RETURN(GetSystemPropertyInt(portKey, defaultPort, &port))
    AutoPtr<IInetSocketAddress> isa;
    CInetSocketAddress::CreateUnresolved(host, port, (IInetSocketAddress**)&isa);
    AutoPtr<IProxy> p;
    CProxy::New(type, ISocketAddress::Probe(isa), (IProxy**)&p);
    *proxy = p;
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

Boolean ProxySelectorImpl::IsNonProxyHost(
    /* [in] */ const String& host,
    /* [in] */ const String& nonProxyHosts)
{
    // nonProxyHosts is not set
    if (host.IsNull() || nonProxyHosts.IsNull()) {
        return FALSE;
    }
    // Construct regex expression of nonProxyHosts
    Char32 ch;
    StringBuilder buf;
    AutoPtr<ArrayOf<Char32> > char32Array = nonProxyHosts.GetChars();
    for (Int32 i = 0; i < char32Array->GetLength(); i++) {
        ch = (*char32Array)[i];
        switch (ch) {
            case '.':
                buf.Append("\\.");
                break;
            case '*':
                buf.Append(".*");
                break;
            default:
                buf.AppendChar(ch);
        }
    }
    String nonProxyHostsReg;
    buf.ToString(&nonProxyHostsReg);
    // check whether the host is the nonProxyHosts.
    Boolean isflag = FALSE;
    StringUtils::Matches(host, nonProxyHostsReg, &isflag);
    return isflag;
}

ECode ProxySelectorImpl::GetSystemPropertyInt(
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = defaultValue;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String prop;
    system->GetProperty(key, &prop);
    if (!prop.IsNull()) {
        StringUtils::Parse(prop, result);
    }

    return NOERROR;
}

} // namespace Net
} // namespace Elastos
