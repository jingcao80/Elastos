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

#ifndef __ORG_APACHE_HTTP_CONN_ROUTING_CHttpRoute_H_
#define __ORG_APACHE_HTTP_CONN_ROUTING_CHttpRoute_H_

#include "Elastos.CoreLibrary.Net.h"
#include "_Org_Apache_Http_Conn_Routing_CHttpRoute.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Elastos::Net::IInetAddress;
using Org::Apache::Http::IHttpHost;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {

/**
 * The route for a request.
 * Instances of this class are unmodifiable and therefore suitable
 * for use as lookup keys.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 653041 $
 *
 * @since 4.0
 */
CarClass(CHttpRoute)
    , public Object
    , public IHttpRoute
    , public IRouteInfo
    , public ICloneable
{
public:
    CHttpRoute();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Obtains the target host.
     *
     * @return the target host
     */
    CARAPI GetTargetHost(
        /* [out] */ IHttpHost** host);

    /**
     * Obtains the local address to connect from.
     *
     * @return  the local address,
     *          or <code>null</code>
     */
    CARAPI GetLocalAddress(
        /* [out] */ IInetAddress** addr);

    /**
     * Obtains the number of hops in this route.
     * A direct route has one hop. A route through a proxy has two hops.
     * A route through a chain of <i>n</i> proxies has <i>n+1</i> hops.
     *
     * @return  the number of hops in this route
     */
    CARAPI GetHopCount(
        /* [out] */ Int32* count);

    /**
     * Obtains the target of a hop in this route.
     * The target of the last hop is the {@link #getTargetHost target host},
     * the target of previous hops is the respective proxy in the chain.
     * For a route through exactly one proxy, target of hop 0 is the proxy
     * and target of hop 1 is the target host.
     *
     * @param hop       index of the hop for which to get the target,
     *                  0 for first
     *
     * @return  the target of the given hop
     *
     * @throws IllegalArgumentException
     *  if the argument is negative or not less than
     *  {@link #getHopCount getHopCount()}
     */
    CARAPI GetHopTarget(
        /* [in] */ Int32 hop,
        /* [out] */ IHttpHost** target);

    /**
     * Obtains the first proxy host.
     *
     * @return the first proxy in the proxy chain, or
     *         <code>null</code> if this route is direct
     */
    CARAPI GetProxyHost(
        /* [out] */ IHttpHost** proxy);

    /**
     * Obtains the tunnel type of this route.
     * If there is a proxy chain, only end-to-end tunnels are considered.
     *
     * @return  the tunnelling type
     */
    CARAPI GetTunnelType(
        /* [out] */ TunnelType* type);

    /**
     * Checks whether this route is tunnelled through a proxy.
     * If there is a proxy chain, only end-to-end tunnels are considered.
     *
     * @return  <code>true</code> if tunnelled end-to-end through at least
     *          one proxy,
     *          <code>false</code> otherwise
     */
    CARAPI IsTunnelled(
        /* [out] */ Boolean* isTunnelled);

    /**
     * Obtains the layering type of this route.
     * In the presence of proxies, only layering over an end-to-end tunnel
     * is considered.
     *
     * @return  the layering type
     */
    CARAPI GetLayerType(
        /* [out] */ LayerType* type);

    /**
     * Checks whether this route includes a layered protocol.
     * In the presence of proxies, only layering over an end-to-end tunnel
     * is considered.
     *
     * @return  <code>true</code> if layered,
     *          <code>false</code> otherwise
     */
    CARAPI IsLayered(
        /* [out] */ Boolean* isLayered);

    /**
     * Checks whether this route is secure.
     *
     * @return  <code>true</code> if secure,
     *          <code>false</code> otherwise
     */
    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Clone(
        /* [out] */ IInterface** result);

    /**
     * Creates a new route with all attributes specified explicitly.
     *
     * @param target    the host to which to route
     * @param local     the local address to route from, or
     *                  <code>null</code> for the default
     * @param proxies   the proxy chain to use, or
     *                  <code>null</code> for a direct route
     * @param secure    <code>true</code> if the route is (to be) secure,
     *                  <code>false</code> otherwise
     * @param tunnelled the tunnel type of this route
     * @param layered   the layering type of this route
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* target,
        /* [in] */ IInetAddress* local,
        /* [in] */ ArrayOf<IHttpHost*>* proxies,
        /* [in] */ Boolean secure,
        /* [in] */ TunnelType tunnelled,
        /* [in] */ LayerType layered);

    /**
     * Creates a new route with at most one proxy.
     *
     * @param target    the host to which to route
     * @param local     the local address to route from, or
     *                  <code>null</code> for the default
     * @param proxy     the proxy to use, or
     *                  <code>null</code> for a direct route
     * @param secure    <code>true</code> if the route is (to be) secure,
     *                  <code>false</code> otherwise
     * @param tunnelled <code>true</code> if the route is (to be) tunnelled
     *                  via the proxy,
     *                  <code>false</code> otherwise
     * @param layered   <code>true</code> if the route includes a
     *                  layered protocol,
     *                  <code>false</code> otherwise
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* target,
        /* [in] */ IInetAddress* local,
        /* [in] */ IHttpHost* proxy,
        /* [in] */ Boolean secure,
        /* [in] */ TunnelType tunnelled,
        /* [in] */ LayerType layered);

    /**
     * Creates a new direct route.
     * That is a route without a proxy.
     *
     * @param target    the host to which to route
     * @param local     the local address to route from, or
     *                  <code>null</code> for the default
     * @param secure    <code>true</code> if the route is (to be) secure,
     *                  <code>false</code> otherwise
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* target,
        /* [in] */ IInetAddress* local,
        /* [in] */ Boolean secure);

    /**
     * Creates a new direct insecure route.
     *
     * @param target    the host to which to route
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* target);

    /**
     * Creates a new route through a proxy.
     * When using this constructor, the <code>proxy</code> MUST be given.
     * For convenience, it is assumed that a secure connection will be
     * layered over a tunnel through the proxy.
     *
     * @param target    the host to which to route
     * @param local     the local address to route from, or
     *                  <code>null</code> for the default
     * @param proxy     the proxy to use
     * @param secure    <code>true</code> if the route is (to be) secure,
     *                  <code>false</code> otherwise
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* target,
        /* [in] */ IInetAddress* local,
        /* [in] */ IHttpHost* proxy,
        /* [in] */ Boolean secure);

private:
    /**
     * Internal, fully-specified constructor.
     * This constructor does <i>not</i> clone the proxy chain array,
     * nor test it for <code>null</code> elements. This conversion and
     * check is the responsibility of the public constructors.
     * The order of arguments here is different from the similar public
     * constructor, as required by Java.
     *
     * @param local     the local address to route from, or
     *                  <code>null</code> for the default
     * @param target    the host to which to route
     * @param proxies   the proxy chain to use, or
     *                  <code>null</code> for a direct route
     * @param secure    <code>true</code> if the route is (to be) secure,
     *                  <code>false</code> otherwise
     * @param tunnelled the tunnel type of this route, or
     *                  <code>null</code> for PLAIN
     * @param layered   the layering type of this route, or
     *                  <code>null</code> for PLAIN
     */
    CARAPI Init(
        /* [in] */ IInetAddress* local,
        /* [in] */ IHttpHost* target,
        /* [in] */ ArrayOf<IHttpHost*>* proxies,
        /* [in] */ Boolean secure,
        /* [in] */ TunnelType tunnelled,
        /* [in] */ LayerType layered);

    /**
     * Helper to convert a proxy to a proxy chain.
     *
     * @param proxy     the only proxy in the chain, or <code>null</code>
     *
     * @return  a proxy chain array, or <code>null</code>
     */
    static CARAPI ToChain(
        /* [in] */ IHttpHost* proxy,
        /* [out] */ ArrayOf<IHttpHost*>** chain);


    /**
     * Helper to duplicate and check a proxy chain.
     * An empty proxy chain is converted to <code>null</code>.
     *
     * @param proxies   the proxy chain to duplicate, or <code>null</code>
     *
     * @return  a new proxy chain array, or <code>null</code>
     */
    static CARAPI ToChain(
        /* [in] */ ArrayOf<IHttpHost*>* proxies,
        /* [out] */ ArrayOf<IHttpHost*>** chain);

private:
    /** The target host to connect to. */
    AutoPtr<IHttpHost> mTargetHost;

    /**
     * The local address to connect from.
     * <code>null</code> indicates that the default should be used.
     */
    AutoPtr<IInetAddress> mLocalAddress;

    /** The proxy servers, if any. */
    AutoPtr< ArrayOf<IHttpHost*> > mProxyChain;

    /** Whether the the route is tunnelled through the proxy. */
    TunnelType mTunnelled;

    /** Whether the route is layered. */
    LayerType mLayered;

    /** Whether the route is (supposed to be) secure. */
    Boolean mSecure;

};

} // namespace Routing
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_ROUTING_CHttpRoute_H_
