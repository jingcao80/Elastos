
#include "org/apache/http/conn/routing/CHttpRoute.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {

CHttpRoute::CHttpRoute()
    : mSecure(FALSE)
{}

CAR_INTERFACE_IMPL_3(CHttpRoute, Object, IHttpRoute, IRouteInfo, ICloneable)

CAR_OBJECT_IMPL(CHttpRoute)

ECode CHttpRoute::ToChain(
    /* [in] */ IHttpHost* proxy,
    /* [out] */ ArrayOf<IHttpHost*>** chain)
{
    VALIDATE_NOT_NULL(chain)
    if (proxy == NULL) {
        *chain = NULL;
        return NOERROR;
    }
    AutoPtr< ArrayOf<IHttpHost*> > proxies = ArrayOf<IHttpHost*>::Alloc(1);
    proxies->Set(0, proxy);
    *chain = proxies;
    REFCOUNT_ADD(*chain)
    return NOERROR;
}

ECode CHttpRoute::ToChain(
    /* [in] */ ArrayOf<IHttpHost*>* proxies,
    /* [out] */ ArrayOf<IHttpHost*>** chain)
{
    VALIDATE_NOT_NULL(chain)
    if ((proxies == NULL) || (proxies->GetLength() < 1)) {
        *chain = NULL;
        return NOERROR;
    }

    Int32 len = proxies->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        if ((*proxies)[i] == NULL) {
            Logger::E("CHttpRoute", "Proxy chain may not contain null elements.");
            *chain = NULL;
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // copy the proxy chain, the traditional way
    AutoPtr< ArrayOf<IHttpHost*> > result = ArrayOf<IHttpHost*>::Alloc(len);
    result->Copy(0, proxies, 0, len);
    // System.arraycopy(proxies, 0, result, 0, proxies.length);

    *chain = result;
    REFCOUNT_ADD(*chain)
    return NOERROR;
}

ECode CHttpRoute::GetTargetHost(
    /* [out] */ IHttpHost** host)
{
    VALIDATE_NOT_NULL(host)
    *host = mTargetHost;
    REFCOUNT_ADD(*host)
    return NOERROR;
}

ECode CHttpRoute::GetLocalAddress(
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
    *addr = mLocalAddress;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode CHttpRoute::GetHopCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = (mProxyChain == NULL) ? 1 : (mProxyChain->GetLength() + 1);
    return NOERROR;
}

ECode CHttpRoute::GetHopTarget(
    /* [in] */ Int32 hop,
    /* [out] */ IHttpHost** target)
{
    VALIDATE_NOT_NULL(target)
    *target = NULL;
    if (hop < 0) {
        Logger::E("CHttpRoute", "Hop index must not be negative: %d", hop);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 hopcount;
    GetHopCount(&hopcount);
    if (hop >= hopcount) {
        Logger::E("CHttpRoute", "Hop index : %d exceeds route length ", hop, hopcount);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (hop < hopcount-1) {
        *target = (*mProxyChain)[hop];
        REFCOUNT_ADD(*target)
    }
    else {
        *target = mTargetHost;
        REFCOUNT_ADD(*target)
    }

    return NOERROR;
}

ECode CHttpRoute::GetProxyHost(
    /* [out] */ IHttpHost** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = (mProxyChain == NULL) ? NULL : (*mProxyChain)[0];
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

ECode CHttpRoute::GetTunnelType(
    /* [out] */ TunnelType* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mTunnelled;
    return NOERROR;
}

ECode CHttpRoute::IsTunnelled(
    /* [out] */ Boolean* isTunnelled)
{
    VALIDATE_NOT_NULL(isTunnelled)
    *isTunnelled = (mTunnelled == TunnelType_TUNNELLED);
    return NOERROR;
}

ECode CHttpRoute::GetLayerType(
    /* [out] */ LayerType* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mLayered;
    return NOERROR;
}

ECode CHttpRoute::IsLayered(
    /* [out] */ Boolean* isLayered)
{
    VALIDATE_NOT_NULL(isLayered)
    *isLayered = (mLayered == LayerType_LAYERED);
    return NOERROR;
}

ECode CHttpRoute::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = mSecure;
    return NOERROR;
}

ECode CHttpRoute::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (other == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IHttpRoute::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<CHttpRoute> that = (CHttpRoute*)IHttpRoute::Probe(other);
    Boolean equal;
    IObject::Probe(mTargetHost)->Equals(that->mTargetHost, &equal);
    Boolean addrEquals;
    equal &= (mLocalAddress == that->mLocalAddress) ||
            ((mLocalAddress != NULL) &&
            (IObject::Probe(mLocalAddress)->Equals(that->mLocalAddress, &addrEquals), addrEquals));

    equal &= (mProxyChain == that->mProxyChain) ||
            ((mProxyChain != NULL) && (that->mProxyChain != NULL) &&
            (mProxyChain->GetLength() == that->mProxyChain->GetLength()));
    // comparison of actual proxies follows below
    equal &= (mSecure == that->mSecure) && (mTunnelled == that->mTunnelled) && (mLayered == that->mLayered);

    // chain length has been compared above, now check the proxies
    if (equal && (mProxyChain != NULL)) {
        for (Int32 i=0; equal && (i < mProxyChain->GetLength()); i++) {
            IObject::Probe((*mProxyChain)[i])->Equals((*that->mProxyChain)[i], &equal);
        }
    }

    *result = equal;
    return NOERROR;
}

ECode CHttpRoute::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hc;
    IObject::Probe(mTargetHost)->GetHashCode(&hc);

    if (mLocalAddress != NULL) {
        Int32 addrhc;
        IObject::Probe(mLocalAddress)->GetHashCode(&addrhc);
        hc ^= addrhc;
    }

    if (mProxyChain != NULL) {
        hc ^= mProxyChain->GetLength();
        for (Int32 i = 0; i < mProxyChain->GetLength(); ++i) {
            AutoPtr<IHttpHost> aProxyChain = (*mProxyChain)[i];
            Int32 proxyhc;
            IObject::Probe(aProxyChain)->GetHashCode(&proxyhc);
            hc ^= proxyhc;
        }
    }

    if (mSecure) hc ^= 0x11111111;

    // hc ^= this.tunnelled.hashCode();
    // hc ^= this.layered.hashCode();

    *hashCode = hc;
    return NOERROR;
}

ECode CHttpRoute::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count;
    GetHopCount(&count);
    StringBuilder cab(50 + count * 30);

    cab.Append("HttpRoute[");
    if (mLocalAddress != NULL) {
        cab.Append(mLocalAddress);
        cab.Append("->");
    }
    cab.AppendChar('{');
    if (mTunnelled == TunnelType_TUNNELLED) cab.AppendChar('t');
    if (mLayered == LayerType_LAYERED) cab.AppendChar('l');
    if (mSecure) cab.AppendChar('s');
    cab.Append("}->");
    if (mProxyChain != NULL) {
        for (Int32 i = 0; i < mProxyChain->GetLength(); ++i) {
            AutoPtr<IHttpHost> aProxyChain = (*mProxyChain)[i];
            cab.Append(aProxyChain);
            cab.Append("->");
        }
    }
    cab.Append(mTargetHost);
    cab.AppendChar(']');

    *result = cab.ToString();
    return NOERROR;
}

ECode CHttpRoute::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IHttpRoute> hr;
    FAIL_RETURN(CHttpRoute::New(mTargetHost, mLocalAddress, mProxyChain,
            mSecure, mTunnelled, mLayered, (IHttpRoute**)&hr))
    *result = hr->Probe(EIID_IInterface);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CHttpRoute::Init(
    /* [in] */ IInetAddress* local,
    /* [in] */ IHttpHost* target,
    /* [in] */ ArrayOf<IHttpHost*>* proxies,
    /* [in] */ Boolean secure,
    /* [in] */ TunnelType tunnelled,
    /* [in] */ LayerType layered)
{
    if (target == NULL) {
        Logger::E("CHttpRoute", "Target host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if ((tunnelled == TunnelType_TUNNELLED) && (proxies == NULL)) {
        Logger::E("CHttpRoute", "Proxy required if tunnelled.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // tunnelled is already checked above, that is in line with the default
    // if (tunnelled == null)
    //     tunnelled = TunnelType.PLAIN;
    // if (layered == null)
    //     layered = LayerType.PLAIN;

    mTargetHost   = target;
    mLocalAddress = local;
    mProxyChain   = proxies;
    mSecure       = secure;
    mTunnelled    = tunnelled;
    mLayered      = layered;
    return NOERROR;
}

ECode CHttpRoute::constructor(
    /* [in] */ IHttpHost* target,
    /* [in] */ IInetAddress* local,
    /* [in] */ ArrayOf<IHttpHost*>* proxies,
    /* [in] */ Boolean secure,
    /* [in] */ TunnelType tunnelled,
    /* [in] */ LayerType layered)
{
    AutoPtr< ArrayOf<IHttpHost*> > array;
    ToChain(proxies, (ArrayOf<IHttpHost*>**)&array);
    return Init(local, target, array, secure, tunnelled, layered);
}

ECode CHttpRoute::constructor(
    /* [in] */ IHttpHost* target,
    /* [in] */ IInetAddress* local,
    /* [in] */ IHttpHost* proxy,
    /* [in] */ Boolean secure,
    /* [in] */ TunnelType tunnelled,
    /* [in] */ LayerType layered)
{
    AutoPtr< ArrayOf<IHttpHost*> > array;
    ToChain(proxy, (ArrayOf<IHttpHost*>**)&array);
    return Init(local, target, array, secure, tunnelled, layered);
}

ECode CHttpRoute::constructor(
    /* [in] */ IHttpHost* target,
    /* [in] */ IInetAddress* local,
    /* [in] */ Boolean secure)
{
    return Init(local, target, NULL, secure, TunnelType_PLAIN, LayerType_PLAIN);
}

ECode CHttpRoute::constructor(
    /* [in] */ IHttpHost* target)
{
    return Init(NULL, target, NULL, FALSE, TunnelType_PLAIN, LayerType_PLAIN);
}

ECode CHttpRoute::constructor(
    /* [in] */ IHttpHost* target,
    /* [in] */ IInetAddress* local,
    /* [in] */ IHttpHost* proxy,
    /* [in] */ Boolean secure)
{
    AutoPtr< ArrayOf<IHttpHost*> > array;
    ToChain(proxy, (ArrayOf<IHttpHost*>**)&array);
    FAIL_RETURN(Init(local, target, array, secure,
            secure ? TunnelType_TUNNELLED : TunnelType_PLAIN,
            secure ? LayerType_LAYERED    : LayerType_PLAIN))
    if (proxy == NULL) {
        Logger::E("CHttpRoute", "Proxy host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Routing
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
