
#include "org/apache/http/conn/routing/CRouteTracker.h"
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

CRouteTracker::CRouteTracker()
    : mConnected(FALSE)
    , mSecure(FALSE)
{}

CAR_INTERFACE_IMPL_3(CRouteTracker, Object, IRouteTracker, IRouteInfo, ICloneable)

CAR_OBJECT_IMPL(CRouteTracker)

ECode CRouteTracker::ConnectTarget(
    /* [in] */ Boolean secure)
{
    if (mConnected) {
        Logger::E("CRouteTracker", "Already connected.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mConnected = TRUE;
    mSecure = secure;
    return NOERROR;
}

ECode CRouteTracker::ConnectProxy(
    /* [in] */ IHttpHost* proxy,
    /* [in] */ Boolean secure)
{
    if (proxy == NULL) {
        Logger::E("CRouteTracker", "Proxy host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mConnected) {
        Logger::E("CRouteTracker", "Already connected.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mConnected = TRUE;
    mProxyChain = ArrayOf<IHttpHost*>::Alloc(1);
    (*mProxyChain)[0] = proxy;
    mSecure = secure;
    return NOERROR;
}

ECode CRouteTracker::TunnelTarget(
    /* [in] */ Boolean secure)
{
    if (!mConnected) {
        Logger::E("CRouteTracker", "No tunnel unless connected.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mProxyChain == NULL) {
        Logger::E("CRouteTracker", "No tunnel without proxy.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTunnelled = TunnelType_TUNNELLED;
    mSecure = secure;
    return NOERROR;
}

ECode CRouteTracker::TunnelProxy(
    /* [in] */ IHttpHost* proxy,
    /* [in] */ Boolean secure)
{
    if (proxy == NULL) {
        Logger::E("CRouteTracker", "Proxy host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!mConnected) {
        Logger::E("CRouteTracker", "No tunnel unless connected.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mProxyChain == NULL) {
        Logger::E("CRouteTracker", "No tunnel without proxy.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // prepare an extended proxy chain
    AutoPtr< ArrayOf<IHttpHost*> > proxies = ArrayOf<IHttpHost*>::Alloc(mProxyChain->GetLength() + 1);
    proxies->Copy(0, mProxyChain, 0, mProxyChain->GetLength());
    // System.arraycopy(this.proxyChain, 0,
    //                  proxies, 0, this.proxyChain.length);
    proxies->Set(proxies->GetLength() - 1, proxy);
    mProxyChain = proxies;
    mSecure = secure;
    return NOERROR;
}

ECode CRouteTracker::LayerProtocol(
    /* [in] */ Boolean secure)
{
    // it is possible to layer a protocol over a direct connection,
    // although this case is probably not considered elsewhere
    if (!mConnected) {
        Logger::E("CRouteTracker", "No tunnel unless connected.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLayered = LayerType_LAYERED;
    mSecure = secure;
    return NOERROR;
}

ECode CRouteTracker::GetTargetHost(
    /* [out] */ IHttpHost** host)
{
    VALIDATE_NOT_NULL(host)
    *host = mTargetHost;
    REFCOUNT_ADD(*host)
    return NOERROR;
}

ECode CRouteTracker::GetLocalAddress(
    /* [out] */ IInetAddress** addr)
{
    VALIDATE_NOT_NULL(addr)
    *addr = mLocalAddress;
    REFCOUNT_ADD(*addr)
    return NOERROR;
}

ECode CRouteTracker::GetHopCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 hops = 0;
    if (mConnected) {
        if (mProxyChain == NULL)
            hops = 1;
        else
            hops = mProxyChain->GetLength() + 1;
    }
    *count = hops;
    return NOERROR;
}

ECode CRouteTracker::GetHopTarget(
    /* [in] */ Int32 hop,
    /* [out] */ IHttpHost** target)
{
    VALIDATE_NOT_NULL(target)
    *target = NULL;
    if (hop < 0) {
        Logger::E("CRouteTracker", "Hop index must not be negative: %d", hop);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 hopcount;
    GetHopCount(&hopcount);
    if (hop >= hopcount) {
        Logger::E("CRouteTracker", "Hop index : %d exceeds route length ", hop, hopcount);
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

ECode CRouteTracker::GetProxyHost(
    /* [out] */ IHttpHost** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = (mProxyChain == NULL) ? NULL : (*mProxyChain)[0];
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

ECode CRouteTracker::IsConnected(
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected)
    *isConnected = mConnected;
    return NOERROR;
}

ECode CRouteTracker::GetTunnelType(
    /* [out] */ TunnelType* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mTunnelled;
    return NOERROR;
}

ECode CRouteTracker::IsTunnelled(
    /* [out] */ Boolean* isTunnelled)
{
    VALIDATE_NOT_NULL(isTunnelled)
    *isTunnelled = (mTunnelled == TunnelType_TUNNELLED);
    return NOERROR;
}

ECode CRouteTracker::GetLayerType(
    /* [out] */ LayerType* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mLayered;
    return NOERROR;
}

ECode CRouteTracker::IsLayered(
    /* [out] */ Boolean* isLayered)
{
    VALIDATE_NOT_NULL(isLayered)
    *isLayered = (mLayered == LayerType_LAYERED);
    return NOERROR;
}

ECode CRouteTracker::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = mSecure;
    return NOERROR;
}

ECode CRouteTracker::ToRoute(
    /* [out] */ IHttpRoute** route)
{
    VALIDATE_NOT_NULL(route)
    if (!mConnected) {
        *route = NULL;
        return NOERROR;
    }
    else {
        return CHttpRoute::New(mTargetHost, mLocalAddress, mProxyChain,
                mSecure, mTunnelled, mLayered, route);
    }
}

ECode CRouteTracker::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (other == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IRouteTracker::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<CRouteTracker> that = (CRouteTracker*)IRouteTracker::Probe(other);
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
    equal &= (mConnected == that->mConnected) && (mSecure == that->mSecure)
            && (mTunnelled == that->mTunnelled) && (mLayered == that->mLayered);

    // chain length has been compared above, now check the proxies
    if (equal && (mProxyChain != NULL)) {
        for (Int32 i=0; equal && (i < mProxyChain->GetLength()); i++) {
            IObject::Probe((*mProxyChain)[i])->Equals((*that->mProxyChain)[i], &equal);
        }
    }

    *result = equal;
    return NOERROR;
}

ECode CRouteTracker::GetHashCode(
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

    if (mConnected) hc ^= 0x11111111;
    if (mSecure) hc ^= 0x22222222;

    // hc ^= this.tunnelled.hashCode();
    // hc ^= this.layered.hashCode();

    *hashCode = hc;
    return NOERROR;
}

ECode CRouteTracker::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count;
    GetHopCount(&count);
    StringBuilder cab(50 + count * 30);

    cab.Append("RouteTracker[");
    if (mLocalAddress != NULL) {
        cab.Append(mLocalAddress);
        cab.Append("->");
    }
    cab.AppendChar('{');
    if (mConnected) cab.AppendChar('c');
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

ECode CRouteTracker::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<IRouteTracker> rt;
    FAIL_RETURN(CRouteTracker::New(mTargetHost, mLocalAddress, (IRouteTracker**)&rt))
    *result = rt->Probe(EIID_IInterface);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CRouteTracker::constructor(
    /* [in] */ IHttpHost* target,
    /* [in] */ IInetAddress* local)
{
    if (target == NULL) {
        Logger::E("CRouteTracker", "Target host may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTargetHost = target;
    mLocalAddress = local;
    mTunnelled = TunnelType_PLAIN;
    mLayered = LayerType_PLAIN;
    return NOERROR;
}

ECode CRouteTracker::constructor(
    /* [in] */ IHttpRoute* route)
{
    AutoPtr<IHttpHost> host;
    IRouteInfo::Probe(route)->GetTargetHost((IHttpHost**)&host);
    AutoPtr<IInetAddress> addr;
    IRouteInfo::Probe(route)->GetLocalAddress((IInetAddress**)&addr);
    return constructor(host, addr);
}

} // namespace Routing
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
