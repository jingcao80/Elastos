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

#ifndef __ORG_APACHE_HTTP_CONN_ROUTING_CROUTETRACKER_H_
#define __ORG_APACHE_HTTP_CONN_ROUTING_CROUTETRACKER_H_

#include "Elastos.CoreLibrary.Net.h"
#include "_Org_Apache_Http_Conn_Routing_CRouteTracker.h"
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
CarClass(CRouteTracker)
    , public Object
    , public IRouteTracker
    , public IRouteInfo
    , public ICloneable
{
public:
    CRouteTracker();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Tracks connecting to the target.
     *
     * @param secure    <code>true</code> if the route is secure,
     *                  <code>false</code> otherwise
     */
    CARAPI ConnectTarget(
        /* [in] */ Boolean secure);

    /**
     * Tracks connecting to the first proxy.
     *
     * @param proxy     the proxy connected to
     * @param secure    <code>true</code> if the route is secure,
     *                  <code>false</code> otherwise
     */
    CARAPI ConnectProxy(
        /* [in] */ IHttpHost* proxy,
        /* [in] */ Boolean secure);

    /**
     * Tracks tunnelling to the target.
     *
     * @param secure    <code>true</code> if the route is secure,
     *                  <code>false</code> otherwise
     */
    CARAPI TunnelTarget(
        /* [in] */ Boolean secure);

    /**
     * Tracks tunnelling to a proxy in a proxy chain.
     * This will extend the tracked proxy chain, but it does not mark
     * the route as tunnelled. Only end-to-end tunnels are considered there.
     *
     * @param proxy     the proxy tunnelled to
     * @param secure    <code>true</code> if the route is secure,
     *                  <code>false</code> otherwise
     */
    CARAPI TunnelProxy(
        /* [in] */ IHttpHost* proxy,
        /* [in] */ Boolean secure);

    /**
     * Tracks layering a protocol.
     *
     * @param secure    <code>true</code> if the route is secure,
     *                  <code>false</code> otherwise
     */
    CARAPI LayerProtocol(
        /* [in] */ Boolean secure);

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

    CARAPI IsConnected(
        /* [out] */ Boolean* isConnected);

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

    /**
     * Obtains the tracked route.
     * If a route has been tracked, it is {@link #isConnected connected}.
     * If not connected, nothing has been tracked so far.
     *
     * @return  the tracked route, or
     *          <code>null</code> if nothing has been tracked so far
     */
    CARAPI ToRoute(
        /* [out] */ IHttpRoute** route);

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
     * Creates a new route tracker.
     * The target and origin need to be specified at creation time.
     *
     * @param target    the host to which to route
     * @param local     the local address to route from, or
     *                  <code>null</code> for the default
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* target,
        /* [in] */ IInetAddress* local);

    /**
     * Creates a new tracker for the given route.
     * Only target and origin are taken from the route,
     * everything else remains to be tracked.
     *
     * @param route     the route to track
     */
    CARAPI constructor(
        /* [in] */ IHttpRoute* route);

private:
    /** The target host to connect to. */
    AutoPtr<IHttpHost> mTargetHost;

    /**
     * The local address to connect from.
     * <code>null</code> indicates that the default should be used.
     */
    AutoPtr<IInetAddress> mLocalAddress;

    // the attributes above are fixed at construction time
    // now follow attributes that indicate the established route

    /** Whether the first hop of the route is established. */
    Boolean mConnected;

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

#endif // __ORG_APACHE_HTTP_CONN_ROUTING_CROUTETRACKER_H_
