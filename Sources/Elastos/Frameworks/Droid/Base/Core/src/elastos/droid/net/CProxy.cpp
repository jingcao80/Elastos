
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CProxy.h"
#include "elastos/droid/net/Proxy.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CProxy, Singleton, IProxy)

CAR_SINGLETON_IMPL(CProxy)

ECode CProxy::GetProxy(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& url,
        /* [out] */ Elastos::Net::IProxy** result)
{
    return CProxy::GetProxy(ctx, url, result);
}

ECode CProxy::GetHost(
        /* [in] */ IContext* ctx,
        /* [out] */ String* result)
{
    return CProxy::GetHost(ctx, result);
}

ECode CProxy::GetPort(
        /* [in] */ IContext* ctx,
        /* [out] */ Int32* result)
{
    return CProxy::GetPort(ctx, result);
}

ECode CProxy::GetDefaultHost(
        /* [out] */ String* result)
{
    return CProxy::GetDefaultHost(result);
}

ECode CProxy::GetDefaultPort(
        /* [out] */ Int32* result)
{
    return CProxy::GetDefaultPort(result);
}

ECode CProxy::GetPreferredHttpHost(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [out] */ IHttpHost** result)
{
    return CProxy::GetPreferredHttpHost(context, url, result);
}

ECode CProxy::Validate(
        /* [in] */ const String& hostname,
        /* [in] */ const String& port,
        /* [in] */ const String& exclList,
        /* [out] */ Int32* result)
{
    return CProxy::Validate(hostname, port, exclList, result);
}

ECode CProxy::SetHttpProxySystemProperty(
        /* [in] */ IProxyInfo* p)
{
    return CProxy::SetHttpProxySystemProperty(p);
}

ECode CProxy::SetHttpProxySystemProperty(
        /* [in] */ const String& host,
        /* [in] */ const String& port,
        /* [in] */ const String& exclList,
        /* [in] */ IUri* pacFileUrl)
{
    return CProxy::SetHttpProxySystemProperty(host, port, exclList, pacFileUrl);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

